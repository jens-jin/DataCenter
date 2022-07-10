#include "_public.h"
#include "_mysql.h"

CLogFile logfile;
//站点参数结构体
struct st_stcode{
  char proname[31];//省
  char obtid[11];//站号
  char cityname[31];//站名
  char lat[11];//经度
  char lon[11];//纬度
  char height[11];//海拔高度 空值字段用字符串表示
};

//定义存放站点参数的容器
vector<struct st_stcode> vstcode;

void help();

void EXIT(int sig);

//把站点参数文件加载到容器里
bool LoadSTCode(const char *inifile);

connection conn;

CPActive PActive;

int main(int argc,char *argv[])
{
  //帮助文档 help files
  if(argc!=5){help();return -1;}

  //处理退出信号 Handler exit signal
  //CloseIOAndSignal();signal(SIGINT,EXIT);signal(SIGTERM,EXIT);
  
  //打开日志文件 open logfile
  if(logfile.Open(argv[4],"a++")==false){printf("logfile.Open(%s) failed\n",argv[4]);return -1;}

  PActive.AddPInfo(10,"obtcodetodb");
  //把文件加载到容器中 file to vstcode  
  if(LoadSTCode(argv[1])==false) return -1;
  
  logfile.Write("加载参数文件(%s) 成功，站点数(%d)\n",argv[1],vstcode.size());

  //连接数据库connect to db
  if(conn.connecttodb(argv[2],argv[3])!=0)
  { logfile.Write("conn.connecttodb(%s,%s)\n%s\n",argv[2],argv[3],conn.m_cda.message);return -1; } 
  logfile.Write("connect db ok\n");
  //准备insert和update语句 prepare insert update setence
  struct st_stcode stcode;
  sqlstatement stmtins(&conn);
  stmtins.prepare("\
        insert into T_ZHOBTCODE(obtid,cityname,proname,lat,lon,height,uptime) values(:1,:2,:3,:4*100,:5*100,:6*10,now())");
  stmtins.bindin(1,stcode.obtid,10);
  stmtins.bindin(2,stcode.cityname,30);
  stmtins.bindin(3,stcode.proname,30);
  stmtins.bindin(4,stcode.lat,10);
  stmtins.bindin(5,stcode.lon,10);
  stmtins.bindin(6,stcode.height,10);

  sqlstatement stmtupt(&conn);
  stmtupt.prepare("\
        update T_ZHOBTCODE set cityname=:1,proname=:2,lat=:3*100,lon=:4*100,height=:5*10,uptime=now() where obtid=:6");
  stmtupt.bindin(1,stcode.cityname,30);
  stmtupt.bindin(2,stcode.proname,30);
  stmtupt.bindin(3,stcode.lat,10);
  stmtupt.bindin(4,stcode.lon,10);
  stmtupt.bindin(5,stcode.height,10);
  stmtupt.bindin(6,stcode.obtid,10);

  int inscount=0,uptcount=0;
  CTimer Timer;
  //遍历容器loop vstcode 
  for(int ii=0;ii<vstcode.size();ii++)
  {
    //取出一条记录加载到结构体
    memcpy(&stcode,&vstcode[ii],sizeof(struct st_stcode));
    //执行插入语句
    if(stmtins.execute()!=0)
    {
      //如果记录已存在(error_code=1062)，执行更新语句
      if(stmtins.m_cda.rc==1062)
      {
        if(stmtupt.execute()!=0)
        { logfile.Write("update table failed\n%s\n%s\n",stmtupt.m_sql,stmtupt.m_cda.message);return -1; }
        else
	  uptcount++;
      }
      else
        { logfile.Write("insert table failed\n%s\n%s\n",stmtins.m_sql,stmtins.m_cda.message);return -1; }
    }
    else
      inscount++;
  }

  //把记录总数，插入记录数，更新记录数，消耗时长记录日志
  logfile.Write("总数=%d，插入=%d，更新=%d，耗时=%.2f秒",vstcode.size(),inscount,uptcount,Timer.Elapsed());
  //提交事务
  conn.commit();
 
  return 0;
}


void help()
{
  printf("\nUsing:./obtcodetodb inifile connstr charset logfile\n");
  printf("Example:/project/tools1/bin/procctl 120 /project/idc1/bin/obtcodetodb /project/idc1/ini/stcode.ini\
          \"127.0.0.1,root,*#u1604#*,mysql,3306\" utf8 /log/idc/obtcodetodb.log\n\n");
  printf("inifile : 全国站点参数文件\n");
  printf("connstr : 数据库连接参数:ip,username,password,daname,port)\n");
  printf("charset : 数据库字符集\n");
  printf("logfile : 日志文件\n");
  printf("程序每120秒运行一次，由procctl程序调度\n\n\n"); 
}

//把站点参数文件加载到容器里
bool LoadSTCode(const char *inifile){

  CFile File;
  //打开站点参数文件
  if(File.Open(inifile,"r")==false)
  {
  logfile.Write("File.Open(%s) failed.\n",inifile);return false;
  }

  char strBuffer[301];

  CCmdStr CmdStr;

  struct st_stcode stcode; //定义结构体放入容器 

  while(true)
  {
    //读一行，完跳出循环
    // memset(strBuffer,0,sizeof(strBuffer));调用Fget会初始化
    if(File.Fgets(strBuffer,300,true)==false) break;
    /*{
      printf("=%s=File.Fgets(..) failed,会有这个提示是因为到了最后1行(size=300)\n",strBuffer); break;
    }*/
    
    //行拆分
    CmdStr.SplitToCmd(strBuffer,",",true);
    if(CmdStr.CmdCount()!=6) continue;//扔掉无效的行 
    //数据项存到结构体
    CmdStr.GetValue(0, stcode.proname,30);
    CmdStr.GetValue(1, stcode.obtid,10);
    CmdStr.GetValue(2, stcode.cityname,30);
    CmdStr.GetValue(3, stcode.lat,10);
    CmdStr.GetValue(4, stcode.lon,10);
    CmdStr.GetValue(5, stcode.height,10);
   
    //结构体放入容器
    vstcode.push_back(stcode);
    
    //关闭文件 析构函数自动关闭
  }
  return true;
}

void EXIT(int sig)
{
  logfile.Write("程序退出，收到信号sig=%d\n\n",sig);
  conn.disconnect();
  exit(0);
}
