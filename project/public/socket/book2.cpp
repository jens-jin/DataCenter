/*
 * 程序名：book2.cpp，作业。
*/
#include "../_public.h"
#include <pthread.h>
void EXIT(int sig);  // 父线程退出函数。
void *thmain(void *arg);
void thcleanup(void *arg);
int aa=0;
FILE *fp=0;  
pthread_spinlock_t spinlock;
int main(int argc,char *argv[])
{
  if (argc!=1)
  {
    printf("Using:./book2 txtfile\nExample:./book2 /project/public/socket/1.txt\n\n"); return -1;
  }

  // 关闭全部的信号和输入输出。
  // 设置信号,在shell状态下可用 "kill + 线程号" 正常终止些线程
  // 但请不要用 "kill -9 +线程号" 强行终止
  //CloseIOAndSignal(); signal(SIGINT,EXIT); signal(SIGTERM,EXIT);
   if ( (fp=fopen("/project/public/socket/1.txt","r"))==0 )
   {printf("fopen() failed\n");return -1;}
   //create n fp
   //
    pthread_spin_init(&spinlock,0);
  
    for(int ii=0;ii<33;ii++)
    {
      pthread_t thid;
      if (pthread_create(&thid,NULL,thmain,(void*)(long)fp)!=0)break;
    }
    usleep(3);
    printf("aa=%d\n",aa);
}

// 线程退出函数。
void EXIT(int sig)  
{
  // 以下代码是为了防止信号处理函数在执行的过程中被信号中断。
  signal(SIGINT,SIG_IGN); signal(SIGTERM,SIG_IGN);

  pthread_spin_destroy(&spinlock);
  fclose(fp);
  exit(0);
}


void *thmain(void *arg)
{
  FILE *fp1=(FILE *)(long)arg;
  char buffer[10];
  pthread_cleanup_push(thcleanup,(void*)(long)fp1);
  pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS,NULL);
  pthread_detach(pthread_self());
  //printf("")
  while(1)
  { 
    memset(buffer,0,sizeof(buffer));
    pthread_spin_lock(&spinlock);
    if(fgets(buffer,10,fp1)==0)break;
    pthread_spin_unlock(&spinlock);
  
    DeleteRChar(buffer,'\n');
  
    pthread_spin_lock(&spinlock);
    aa+=atoi(buffer);
    pthread_spin_unlock(&spinlock);
    printf("线程%lu...\n",pthread_self());
    usleep(1);
  }
  pthread_cleanup_pop(1);
}

void thcleanup(void *arg)
{
}
