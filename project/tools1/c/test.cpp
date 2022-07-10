//#include "_public.h"
#include "_ftp.h"

Cftp ftp;

int main(int argc,char *argv[])
{
  if(argc!=4) {printf("./test 127.0.0.1:21 jens 123456 1\n");return -1;}
  //登陆ftp 进入目录
  if( ftp.login("127.0.0.1:21","jens","123456")==false )
  {
    printf("ftp.login failed.\n%s",ftp.m_loginfailed); return -1;
  }  
  
  printf("ftp.login ok.\n ");
  //下载文件
  if( ftp.size("/home/jens/crt.txt")==false )
  {
    printf("ftp.size() failed.\n"); 
    ftp.logout();return -1;
  }  
  printf("ftp.size=%d ok.\n ",ftp.m_size);
   

  //退出
  ftp.logout();

  return 0; 
}
