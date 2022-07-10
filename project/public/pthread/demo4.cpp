// 本程序演示线程参数的传递（用强制转换的方法传变量的值）。
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
int ii=0;
void *thmain1(void *arg);    // 线程1的主函数。
int main(int argc,char *argv[])
{
  pthread_t thid1=0;
  // 创建线程。
  if (pthread_create(&thid1,NULL,thmain1,NULL)!=0) { printf("pthread_create failed.\n"); exit(-1); }
  usleep(100);pthread_cancel(thid1);
  int ret=0;
  printf("join...\n");
  int res=pthread_join(thid1,(void**)&ret);  
  printf("join ok.res=%d\tret=%d\n",res,ret);
  printf("ii =%d\n",ii);
}

void *thmain1(void *arg)    // 线程主函数。
{
  
  printf("线程1开始运行。\n");
  pthread_setcanceltype(PTHREAD_CANCEL_DEFERRED,NULL);
  for(ii=0;ii<100000000;ii++)
  {
   ;
   pthread_testcancel(); 
  }
  printf("ii =%d",ii);
  pthread_exit((void*)1);
}
