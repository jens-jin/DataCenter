/*
 *execsql.cpp  执行sql脚本文件
 */

#include "_public.h"
#include "_mysql.h"

CLogFile logfile;

void help();

void EXIT(int sig);

connection conn;
CPActive PActive;

int main(int argc,char *argv[])
{
  //帮助文档 help files
  if(argc!=5){help();return -1;}

  //处理退出信号 Handler exit signal
  CloseIOAndSignal();signal(SIGINT,EXIT);signal(SIGTERM,EXIT);
  
  //打开日志文件 open logfile
  if(logfile.Open(argv[4],"a+")==false){printf("logfile.Open(%s) failed\n",argv[4]);return -1;}

  PActive.AddPInfo(500,"execsql");
  //把文件加载到容器中 file to vstcode  
  //连接数据库connect to db connecttodb (a,b,1)  1 自动提交
  if(conn.connecttodb(argv[2],argv[3],1)!=0)
  { logfile.Write("conn.connecttodb(%s,%s)\n%s\n",argv[2],argv[3],conn.m_cda.message);return -1; } 
  logfile.Write("connect db ok\n");
  
  CFile File;
  File.Open(argv[1],"r");
  char buffer[1001];


  while(true)
  {
    memset(buffer,0,sizeof(buffer));
    if(File.FFGETS(buffer,1000,";")==false) break;
    char *pp =strstr(buffer,";");
    if(pp==0) continue;
    pp[0]=0;
    logfile.Write("sql=%s\n",buffer);
    if(conn.execute(buffer)==0)
      logfile.Write("exec ok(rpc=%ld).\n",conn.m_cda.rpc);
    else 
      logfile.Write("exec failed(%s)\n",conn.m_cda.message);
    PActive.UptATime();
  }
  logfile.WriteEx("\n");
  return 0;
}


void help()
{
  printf("\nUsing:./execsql sqlfile connstr charset logfile\n");
  printf("Example:/project/tools1/bin/procctl 120 /project/tools1/bin/execsql /project/idc1/sql/cleardata.sql"\
          " \"127.0.0.1,root,*#u1604#*,mysql,3306\" utf8 /log/idc/execsql.log\n\n");
  printf("sqlfile : sql脚本文件\n");
  printf("connstr : 数据库连接参数:ip,username,password,daname,port)\n");
  printf("charset : 数据库字符集\n");
  printf("logfile : 日志文件\n");
  printf("程序每120秒运行一次，由procctl程序调度\n\n\n"); 
}


void EXIT(int sig)
{
  logfile.Write("程序退出，收到信号sig=%d\n\n",sig);
  conn.disconnect();
  exit(0);
}
