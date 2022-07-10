#include "_public.h"
#include "_mysql.h"

struct st_zhobtmind{
  char obtid[11];
  char ddatetime[21];
  char t[11];
  char p[11];
  char u[11];
  char wd[11];
  char wf[11];
  char r[11];
  char vis[11];
};

class CZHOBTMIND
{
public:
  connection *m_conn;
  CLogFile *m_logfile;

  sqlstatement m_stmt;

  char m_buffer[1024];
  struct st_zhobtmind m_stzhobtmind;

  CZHOBTMIND();
  CZHOBTMIND(connection *conn,CLogFile *logfile);

  ~CZHOBTMIND();

  void BindConnLog(connection *conn,CLogFile *logfile);
  bool SplitBuffer(char *buffer);
  bool InsertTable();
};

CZHOBTMIND::CZHOBTMIND()
{
  m_conn=0;m_logfile=0;
}

CZHOBTMIND::CZHOBTMIND(connection *conn,CLogFile *logfile)
{
  m_conn=conn;
  m_logfile=logfile;
}

CZHOBTMIND::~CZHOBTMIND()
{
}


void CZHOBTMIND::BindConnLog(connection *conn,CLogFile *logfile)
{
  m_conn=conn;
  m_logfile=logfile;
}

bool CZHOBTMIND::SplitBuffer(char *buffer)
{
  memset(&m_stzhobtmind,0,sizeof(struct st_zhobtmind));
  GetXMLBuffer(buffer,"obtid",m_stzhobtmind.obtid,10);
  GetXMLBuffer(buffer,"ddatetime",m_stzhobtmind.ddatetime,14);
  char tmp[11];
  GetXMLBuffer(buffer,"t",tmp,10); if(strlen(tmp)>0) snprintf(m_stzhobtmind.t,10,"%d",(int)(atof(tmp)*10));
  GetXMLBuffer(buffer,"p",tmp,10); if(strlen(tmp)>0) snprintf(m_stzhobtmind.p,10,"%d",(int)(atof(tmp)*10));
  GetXMLBuffer(buffer,"u",m_stzhobtmind.u,10);
  GetXMLBuffer(buffer,"wd",m_stzhobtmind.wd,10);
  GetXMLBuffer(buffer,"wf",tmp,10); if(strlen(tmp)>0) snprintf(m_stzhobtmind.wf,10,"%d",(int)(atof(tmp)*10));
  GetXMLBuffer(buffer,"r",tmp,10); if(strlen(tmp)>0) snprintf(m_stzhobtmind.r,10,"%d",(int)(atof(tmp)*10));
  GetXMLBuffer(buffer,"vis",tmp,10); if(strlen(tmp)>0) snprintf(m_stzhobtmind.vis,10,"%d",(int)(atof(tmp)*10));
  
  STRCPY(m_buffer,sizeof(m_buffer),buffer);
  return true;
}
bool CZHOBTMIND::InsertTable()
{
    if(m_stmt.m_state==0)
    {
      m_stmt.connect(m_conn);
      m_stmt.prepare("insert into T_ZHOBTMIND(obtid,ddatetime,t,p,u,wd,wf,r,vis) values(:1,str_to_date(:2,'%%Y%%m%%d%%H%%i%%s'),:3,:4,:5,:6,:7,:8,:9)");
      m_stmt.bindin(1,m_stzhobtmind.obtid,10);
      m_stmt.bindin(2,m_stzhobtmind.ddatetime,14);
      m_stmt.bindin(3,m_stzhobtmind.t,10);
      m_stmt.bindin(4,m_stzhobtmind.p,10);
      m_stmt.bindin(5,m_stzhobtmind.u,10);
      m_stmt.bindin(6,m_stzhobtmind.wd,10);
      m_stmt.bindin(7,m_stzhobtmind.wf,10);
      m_stmt.bindin(8,m_stzhobtmind.r,10);
      m_stmt.bindin(9,m_stzhobtmind.vis,10);
    }
  
    if(m_stmt.execute()!=0)
    {
      //1062 记录重复
      if(m_stmt.m_cda.rc!=1062)
        {
          m_logfile->Write("buffer=%s\n",m_buffer);
          m_logfile->Write("m_stmt.execute() failed\n%s\n%s\n",m_stmt.m_sql,m_stmt.m_cda.message);
        }
      return false;
    } 
  return true;
}





CLogFile logfile;

void help();

void EXIT(int sig);

bool _obtmindtodb(char *pathname,char *connstr,char *charset);

connection conn;

CPActive PActive;

int main(int argc,char *argv[])
{
  //帮助文档 help files
  if(argc!=5){help();return -1;}

  //处理退出信号 Handler exit signal
  //CloseIOAndSignal();signal(SIGINT,EXIT);signal(SIGTERM,EXIT);
  
  //打开日志文件 open logfile
  if(logfile.Open(argv[4],"a+")==false){printf("logfile.Open(%s) failed\n",argv[4]);return -1;}

  //PActive.AddPInfo(30,"obtmindtodb");
  PActive.AddPInfo(5000,"obtmindtodb");
  _obtmindtodb(argv[1],argv[2],argv[3]);
  
 
  return 0;
}


void help()
{
  printf("\nUsing:./obtmindtodb pathname connstr charset logfile\n");
  printf("Example:/project/tools1/bin/procctl 120 /project/idc1/bin/obtmindtodb /idcdata/surfdata"\
         " \"127.0.0.1,root,*#u1604#*,mysql,3306\" utf8 /log/idc/obtmindtodb.log\n\n");
  printf("pathname : 全国站点分钟观测数据文件\n");
  printf("connstr : 数据库连接参数:ip,username,password,daname,port)\n");
  printf("charset : 数据库字符集\n");
  printf("logfile : 日志文件\n");
  printf("程序每10秒运行一次，由procctl程序调度\n\n\n"); 
}


void EXIT(int sig)
{
  logfile.Write("程序退出，收到信号sig=%d\n\n",sig);
  conn.disconnect();
  exit(0);
}

bool _obtmindtodb(char *pathname,char *connstr,char *charset)
{
  //打开目录 
  CDir Dir;
  if(Dir.OpenDir(pathname,"*.xml")==false) { logfile.Write("(Dir.OpenDir(%s) failed\n",pathname);return false;}
  
  CFile File;
  CZHOBTMIND ZHOBTMIND(&conn,&logfile); 
  int tc=0,ic=0;
  CTimer Timer;
  while(true)
  {
    //读取目录，获得一个数据文件名
    if( Dir.ReadDir()==false) break;

    //连接数据库connect to db
    if(conn.m_state==0)
    {
      if(conn.connecttodb(connstr,charset)!=0)
      { logfile.Write("conn.connecttodb(%s,%s)\n%s\n",connstr,charset,conn.m_cda.message);return false; } 
      logfile.Write("connect db ok\n");
    }
    
    //打开文件
    if(File.Open(Dir.m_FullFileName,"r")==false) 
    { logfile.Write("(File.Open(%s) failed\n",Dir.m_FullFileName);return false;}

    tc=0,ic=0;
    char buffer[1001];
    while(true)
    {
      //读取每一行
      memset(buffer,0,sizeof(buffer));
      if(File.FFGETS(buffer,1000,"</vis>")==false) break;
      //logfile.Write("buffer=%s=\n",buffer);
      tc++;
      ZHOBTMIND.SplitBuffer(buffer);
      if( ZHOBTMIND.InsertTable()==true) ic++;

    }
    //删除文件，提交事务
   // File.CloseAndRemove();
    conn.commit();
    logfile.Write("已处理文件%s(totalcount=%d,insertcount=%d) 耗时=%.2f秒。\n",Dir.m_FullFileName,tc,ic,Timer.Elapsed());
  }

  return true;
}



