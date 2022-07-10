#include "_public.h"

void EXIT(int sig);

int main(int argc,char *argv[])
{
  if(argc!=4)
  {
    //help close sem&IO get timeout opendir fordir compare  gzip
    printf("Using:/project/tools1/bin/gzipfiles pathname match timeout\n\n ");  
    printf("Example:/project/tools1/bin/gzipfiles /log/idc  \"*.log.20*\" 0.02\n ");
    printf("这是一个工具程序，用于压缩历史日志文件或数据文件\n");  
    return -1;

  }
  
  //关闭信号
 // CloseIOAndSignal(true);
  signal(SIGINT,EXIT); signal(SIGTERM,EXIT);

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
  
  char strCmd[1024];
  while(true)
  {
    //找到一个文件信息
    if( Dir.ReadDir()==false ) break;
 
    
    //比较时间
    if( strcmp(Dir.m_ModifyTime,strTimeOut)<0&&MatchStr(Dir.m_FullFileName,"*.gz")==false )
    {
      //压缩 调用gzip 1>/dev/null 2>/dev/null 把标准输出和标准错误定位到空=不输出 
      SNPRINTF(strCmd,sizeof(strCmd),1000,"/user/bin/gzip -f %s 1>/dev/null 2>/dev/null",Dir.m_FullFileName);
      if( system(strCmd)==0 )
	printf("gzip %s ok.\n",Dir.m_FullFileName);
      else
	printf("gzip %s failed.\n",Dir.m_FullFileName);
	
    }  
  }
  return 0;
}


void EXIT(int sig)
{
  printf("程序退出,sig=%d\n",sig);exit(0);
}
