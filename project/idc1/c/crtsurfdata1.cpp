/*
 * 程序名：srtsurfdata1.cpp 生成观测的分钟数据
 * 作者:jens
 */
 
#include "_public.h"

CLogFile logfile(10);

int main(int argc,char *argv[]){
  //inifile outpath logfile
  if(argc!=4)
  {
     printf("Using:./crtsurfdata1 inifile outpath logfile\n");
     printf("Example:/project/idc1/bin/crtsurfdata1 /project/idc1/ini/stcode.ini /tmp/surfdata /log/idc/crtsurfdata1.log\n");
     printf("inifile 气象站点参数文件名。\n");
     printf("outpath 气象站点数据文件存放目录。\n");
     printf("logfile 程序的日志文件。\n\n");
   
  
    return -1;
  }

  if(logfile.Open(argv[3],"a+",false)==false)
  {
    printf("logfile.Open(%s) failed.\n",argv[3]);return -1;
  }
  
  logfile.Write("srtsurfdata1 start running.\n");
  
  //插入业务代码
  for (int ii=0;ii<10000000;ii++){
    logfile.Write("这是第%d条记录。\n",ii);
  }

  logfile.WriteEx("srtsurfdata1 end running.\n");
  return 0;
}
