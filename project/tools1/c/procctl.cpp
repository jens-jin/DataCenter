#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <string.h>

int main(int argc,char *argv[])
{
  if(argc<3)
  {
    printf("Using:./procctl timetvl program argv ...\n");
    printf("Example:/project/tools1/bin/procctl 5 /usr/bin/ls -lt ...\n");
    printf("本程序用于调用其他程序，周期 .. 秒\n");
    return -1;
  } 
  
  for(int ii=0;ii<64;ii++)
  {
    signal(ii,SIG_IGN);close(ii);//关闭IO
  }  

  if(fork()!=0) exit(0);//父进程退出,1号进程接管，程序运行在后台。

  signal(SIGCHLD,SIG_DFL);//父进程等待子进程退出,因为上面忽略了,重新启用子进程退出信号

  //execv 参数数组
  char *pargv[argc];
  for(int ii=2;ii<argc;ii++)
  {
    pargv[ii-2]=argv[ii];
  }
   pargv[argc-2]=NULL;


  while(true)
  {
    if(fork()==0)
    {
      // execl(argv[2],argv[2],argv[3],(char*)0);
      execv(argv[2],pargv);
      exit(0);//execv 调用失败时执行。
    }
    else
    {
      int status;
      wait(&status);
      sleep(atoi(argv[1]));
    }
    
  }  
  return 0;
}
