#include "_public.h"

void EXIT(int sig);

int main(int argc,char *argv[])
{
  if(argc!=4)
  {
    //help close sem&IO get timeout opendir fordir compare  
    printf("Using:/project/tools1/bin/deletefiles pathname match timeout\n\n ");  
    printf("Example:/project/tools1/bin/deletefiles /log/idc  \"*.log.20*\" 0.02\n ");
    printf("这是一个工具程序，用于删除历史日志文件或数据文件\n");  
    return -1;

  }
  
  //关闭信号
  CloseIOAndSignal(true); signal(SIGINT,EXIT); signal(SIGTERM,EXIT);

  //获取文件超时时间点
  char strTimeOut[21];
  LocalTime(strTimeOut,"yyyy-mm-dd hh24:mi:ss",0-(int)(atof(argv[3])*24*60*60));
  
  CDir Dir;
  //打开目录
  if( Dir.OpenDir(argv[1],argv[2],10000,true) ==false)
  {
    printf("Dir.OpenDir(%s) failed.\n",argv[1]);
    return -1;
  }
  
  while(true)
  {
    //找到一个文件信息
    if( Dir.ReadDir()==false ) break;
 
    
    //比较时间
    if( strcmp(Dir.m_ModifyTime,strTimeOut)<0 )
    {
      //压缩 调用 REMOVE 
      if( REMOVE(Dir.m_FullFileName)==true )
	printf("REMOVE %s ok.\n",Dir.m_FullFileName);
      else
	printf("REMOVE %s failed.\n",Dir.m_FullFileName);
	
    }  
  }
  return 0;
}


void EXIT(int sig)
{
  printf("程序退出,sig=%d\n",sig);exit(0);
}
