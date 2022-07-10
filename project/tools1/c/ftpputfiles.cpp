//#include "_public.h"
#include "_ftp.h"

struct st_arg{
  char host[31];// 远程服务器的IP和端口
  int mode;// 传输模式，1-被动模式，2-主动模式，缺省采用被动模式。
  char username[31];// 远程服务器ftp的用户名。
  char password[31];// 远程服务器ftp的密码
  char remotepath[301];// 远程服务器存放文件的目录。
  char localpath[301];//本地文件存放的目录。
  char matchname[101];//上传文件匹配的规则。
  int  ptype;              // 上传后客服端文件的处理方式：1-什么也不做；2-删除；3-备份。
  char localpathbak[301]; // 上传后客户端文件的备份目录。
  char okfilename[301];
  int timeout;//进程心跳的超时时间
  char pname[51];//进程名，建议用"ftpputfiles_后缀"
} starg;

struct st_fileinfo{
  char filename[301];
  char mtime[21];
};

vector<struct st_fileinfo> vlistfile1;//已上传
vector<struct st_fileinfo> vlistfile2;//上传前
vector<struct st_fileinfo> vlistfile3;//不上传
vector<struct st_fileinfo> vlistfile4;//待上传
bool LoadOKFile(); //okfile->1
bool ComVector();//比较1 2->3 4
bool WriteToOKFile();//3->okfile
bool AppendToOKFile(struct st_fileinfo *strfilename);
//加载到容器
bool LoadLocalFile();

CLogFile logfile;

Cftp ftp;

void EXIT(int sig);

void _help();

//解析参数函数
bool _xmltoarg(char *strxmlbuffer);

//上传文件的主函数
bool _ftpputfiles();

CPActive PActive;//进程心跳

int main(int argc,char *argv[])
{
  //help
  if(argc!=3){ _help(); return -1; }
  //把服务器上的某目录下的文件上传到本地目录
  
  //关闭信号和IO 启用2 15
  CloseIOAndSignal(); signal(SIGINT,EXIT);signal(SIGTERM,EXIT);

  if( logfile.Open(argv[1],"a+")==false )
  {
    printf("logfile.Open(%s) failed.\n",argv[1]); return -1;

  }
  
  //解析参数
  if( _xmltoarg(argv[2])==false )return -1 ;  
 
  PActive.AddPInfo(starg.timeout,starg.pname);//进程心跳写入共享内存  

  //登陆ftp 进入目录
  if( ftp.login(starg.host,starg.username,starg.password,starg.mode)==false )
  {
    logfile.Write("ftp.login(%s,%s,%s) failed.\n",starg.host,starg.username,starg.password); return -1;
  }  
  
 // logfile.Write("ftp.login ok.\n ");
  //上传文件
  _ftpputfiles();
  

  //退出
  ftp.logout();

  return 0; 
}


void EXIT(int sig)
{
  printf("程序退出。收到sig=%d",sig);
  exit(0);
}

void _help()
{
  printf("\n");
  printf("USing:/project/tools1/bin/ftpputfiles logfilename xmlbuffer\n ");
  printf("Example:/project/tools1/bin/ftpputfiles /log/idc/ftpputfiles_surfdata.log"\
         " \"<host>127.0.0.1:21</host><mode>1</mode><username>jens</username>"\
         "<password>123456</password><localpath>/tmp/idc/surfdata</localpath>"\
         "<remotepath>/tmp/ftpputtest</remotepath><matchname>SURF_ZH*.JSON</matchname>"\
         "<ptype>1</ptype><localpathbak>/tmp/idc/surfdatabak</localpathbak>"\
         "<okfilename>/idcdata/ftplist/ftpputfiles_surfdata.xml</okfilename><timeout>80</timeout><pname>ftpputfiles_surfdata</pname>\"\n\n\n");
  printf("Sample:/project/tools1/bin/procctl 30 /project/tools1/bin/ftpputfiles /log/idc/ftpputfiles_surfdata.log"\
         " \"<host>127.0.0.1:21</host><mode>1</mode><username>jens</username>"\
         "<password>123456</password><localpath>/tmp/idc/surfdata</localpath>"\
         "<remotepath>/tmp/ftpputtest</remotepath><matchname>SURF_ZH*.JSON</matchname>"\
         "<ptype>1</ptype><localpathbak>/tmp/idc/surfdatabak</localpathbak>"\
         "<okfilename>/idcdata/ftplist/ftpputfiles_surfdata.xml</okfilename><timeout>80</timeout><pname>ftpputfiles_surfdata</pname>\"\n\n\n");

  printf("本程序是通用的功能模块，用于把远程ftp服务器的文件上传到本地目录。\n");
  printf("logfilename是本程序运行的日志文件。\n");
  printf("xmlbuffer为文件上传的参数，如下：\n");
  printf("<host>127.0.0.1:21</host> 远程服务器的IP和端口。\n");
  printf("<mode>1</mode> 传输模式，1-被动模式，2-主动模式，缺省采用被动模式。\n");
  printf("<username>jens</username> 远程服务器ftp的用户名。\n");
  printf("<password>123456</password> 远程服务器ftp的密码。\n");
  printf("<remotepath>/tmp/ftpputtest</remotepath> 远程服务器存放文件的目录。\n");
  printf("<localpath>/tmp/idc/surfdata</localpath> 本地文件存放的目录。\n");
  printf("<matchname>SURF_ZH*.JSON</matchname> 待上传文件匹配的规则。"\
         "不匹配的文件不会被上传，本字段尽可能设置精确，不建议用*匹配全部的文件。\n");
   printf("<ptype>1</ptype> 文件上传成功后，远程服务器文件的处理方式：1-什么也不做；2-删除；3-备份，如果为3，还要指定备份的目录。\n");
  printf("<localpathbak>/tmp/idc/surfdatabak</localpathbak> 文件上传成功后，客户端文件的备份目录，此参数只有当ptype=3时才有效。\n");
  printf("<okfilename>/idcdata/ftplist/ftpputfiles_surfdata.xml</okfilename> 已上传成功文件名清单，此参数只有当ptype=1时才有效。\n");
  printf("<timeout>80</timeout> 上传文件超时时间\n");
  printf("<pname>ftpputfiles_surfdata</pname> 进程名\n\n\n");

}

bool _xmltoarg(char *strxmlbuffer)
{
  memset(&starg,0,sizeof(struct st_arg));
  GetXMLBuffer(strxmlbuffer,"host",starg.host,30);
  if ( strlen(starg.host)==0 )
  {
    logfile.Write("host is null .\n"); return false;
  }

  GetXMLBuffer(strxmlbuffer,"mode",&starg.mode);//传输模式 1-被动 2-主动 缺省被动。
  if ( starg.mode!=2 ) starg.mode=1;

  GetXMLBuffer(strxmlbuffer,"username",starg.username,30);
  if ( strlen(starg.username)==0 )
  {
    logfile.Write("username is null .\n"); return false;
  }

  GetXMLBuffer(strxmlbuffer,"password",starg.password,30);
  if ( strlen(starg.password)==0 )
  {
    logfile.Write("password is null .\n"); return false;
  }

  GetXMLBuffer(strxmlbuffer,"remotepath",starg.remotepath,300);
  if ( strlen(starg.remotepath)==0 )
  {
    logfile.Write("remotepath is null .\n"); return false;
  }

  GetXMLBuffer(strxmlbuffer,"localpath",starg.localpath,300);
  if ( strlen(starg.localpath)==0 )
  {
    logfile.Write("localpath is null .\n"); return false;
  }

  GetXMLBuffer(strxmlbuffer,"matchname",starg.matchname,100);
  if ( strlen(starg.matchname)==0 )
  {
    logfile.Write("matchname is null .\n"); return false;
  }
  
 
  GetXMLBuffer(strxmlbuffer,"ptype",&starg.ptype);
  if ( (starg.ptype!=1) && (starg.ptype!=2) && (starg.ptype!=3) )
  { logfile.Write("ptype is error.\n"); return false; }

  GetXMLBuffer(strxmlbuffer,"localpathbak",starg.localpathbak,300); // 上传后客户端文件的备份目录。
  if ( (starg.ptype==3) && (strlen(starg.localpathbak)==0) )
  { logfile.Write("localpathbak is null.\n");  return false; }
 
  GetXMLBuffer(strxmlbuffer,"okfilename",starg.okfilename,300); // 已上传后客户端文件的清单。
  if ( (starg.ptype==1) && (strlen(starg.okfilename)==0) )
  { logfile.Write("okfilename is null.\n");  return false; }
 
  GetXMLBuffer(strxmlbuffer,"timeout",&starg.timeout);
  if ( starg.timeout==0 )
  { logfile.Write("timeout is null.\n");  return false; }
  GetXMLBuffer(strxmlbuffer,"pname",starg.pname,50); 
  if ( strlen(starg.pname)==0 )
  { logfile.Write("pname is null.\n");  return false; }
  return true;
}


//上传文件的主函数
bool _ftpputfiles()
{

  //加载到容器
  if ( LoadLocalFile()==false  )
  {
    logfile.Write("LoadLocalFile() failed.\n");return false;
  } 
 
  PActive.UptATime();
  if(starg.ptype==1)
  {
    LoadOKFile(); //okfile->1
    ComVector();//比较1 2->3 4
    WriteToOKFile();//3->okfile
    vlistfile2.clear();vlistfile2.swap(vlistfile4);//4->2
  }

  PActive.UptATime();
 
  char strremotefilename[301],strlocalfilename[301];
  //上传
  for(int ii=0;ii<vlistfile2.size();ii++)
  { 
    SNPRINTF(strremotefilename,sizeof(strremotefilename),300,"%s/%s",starg.remotepath,vlistfile2[ii].filename);
    SNPRINTF(strlocalfilename,sizeof(strlocalfilename),300,"%s/%s",starg.localpath,vlistfile2[ii].filename);
    logfile.Write("put %s ...",strlocalfilename);

    if ( ftp.put(strlocalfilename,strremotefilename,true)==false ){logfile.WriteEx("get file failed\n"); return false;}

    logfile.Write(" ok\n",strremotefilename);

  PActive.UptATime();
    //上传新增文件追加到okfilename
    if(starg.ptype==1) AppendToOKFile(&vlistfile2[ii]);

    //删除文件
    if (starg.ptype==2)
    {
      if (REMOVE(strlocalfilename)==false)
      {
        logfile.Write("REMOVE(%s) failed.\n",strlocalfilename); return false;
      }
    }

    //备份文件
    if(starg.ptype==3)
    {
      char strlocalfilenamebak[301];
      SNPRINTF(strlocalfilenamebak,sizeof(strlocalfilenamebak),300,"%s/%s",starg.localpathbak,vlistfile2[ii].filename);
      if (RENAME(strlocalfilename,strlocalfilenamebak)==false)
      {
        logfile.Write("RENAME(%s,%s) failed.\n",strlocalfilename,strlocalfilenamebak); return false;
      }    

    }
  }
  return true;
}

bool LoadOKFile() //okfile->1
{
  vlistfile1.clear();
  CFile File;

  if( File.Open(starg.okfilename,"r")==false) return true;//第一次上传 okfilename不存在
  
  char strbuffer[501];
  struct st_fileinfo stfileinfo;//放入容器先放结构体

  while(true)
  {
    memset(&stfileinfo,0,sizeof( struct st_fileinfo));
    if( File.Fgets(strbuffer,300,true)==false) break;
   // {logfile.Write("File.Fgets(%s) failed \n",stfileinfo.filename); break;}//最后一行一定会faile
    //如果不匹配 不放入容器a
    GetXMLBuffer(strbuffer,"filename",stfileinfo.filename);
    GetXMLBuffer(strbuffer,"mtime",stfileinfo.mtime);
    vlistfile1.push_back(stfileinfo);
  }     
  return true;
}

bool ComVector()//比较1 2->3 4
{
  vlistfile3.clear();  vlistfile4.clear();
  int ii,jj;
  for(ii=0;ii<vlistfile2.size();ii++)
  { 
    for(jj=0;jj<vlistfile1.size();jj++)
    {
      //比较文件名+时间
      if( strcmp(vlistfile2[ii].filename,vlistfile1[jj].filename)==0 &&
          strcmp(vlistfile2[ii].mtime,vlistfile1[jj].mtime)==0 )
      {
        vlistfile3.push_back(vlistfile2[ii]);break;
      }
    }
    if(jj==vlistfile1.size()) vlistfile4.push_back(vlistfile2[ii]); 
  }
  return true;
}

bool WriteToOKFile()//3->okfile
{
  CFile File;
  if( File.Open(starg.okfilename,"w")==false)
  {
    logfile.Write("File.Open(%s) failed.\n",starg.okfilename);return false; 
  }
  for(int ii=0;ii<vlistfile3.size();ii++)
  {
    File.Fprintf("<filename>%s</filename><mtime>%s</mtime>\n",vlistfile3[ii].filename,vlistfile3[ii].mtime);
  }
  return true;
}


bool AppendToOKFile(struct st_fileinfo *strfilename)
{
  CFile File;
  if( File.Open(starg.okfilename,"a")==false)
  { 
    logfile.Write("File.Open(%s) failed.\n",starg.okfilename);return false;
  }
  File.Fprintf("<filename>%s</filename><mtime>%s</mtime>\n",strfilename->filename,strfilename->mtime);

  return true;
}


//加载到容器
bool LoadLocalFile()
{
  vlistfile2.clear();//容器清空
  
  CDir Dir;
  // 设置文件时间的格式，支持"yyyy-mm-dd hh24:mi:ss"和"yyyymmddhh24miss"两种，缺省是前者。
  Dir.SetDateFMT("yyyymmddhh24miss");
  if ( Dir.OpenDir(starg.localpath,starg.matchname)==false)
  {
    logfile.Write("Dir.OpenDir(%s) failed.\n",starg.localpath);return false;
  }  
 
  struct st_fileinfo stfileinfo;//放入容器先放结构体
  
  while(true)
  {  
    memset(&stfileinfo,0,sizeof( struct st_fileinfo));

    if(Dir.ReadDir()==false) break;

    strcpy(stfileinfo.filename,Dir.m_FileName);
    strcpy(stfileinfo.mtime,Dir.m_ModifyTime);
   // {logfile.Write("File.Fgets(%s) failed \n",stfileinfo.filename); break;}
    //如果不匹配 不放入容器
    vlistfile2.push_back(stfileinfo);
  }
  
  return true;
}
