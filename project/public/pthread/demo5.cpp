// 本程序演示线程参数的传递（用强制转换的方法传变量的值）。
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
int aa=0;

void *thmain1(void *arg);    // 线程1的主函数。

pthread_mutex_t mutex=PTHREAD_MUTEX_INITIALIZER;//initializer

int main(int argc,char *argv[])
{
  pthread_t thid1=0,thid2=0;
  // 创建线程。
  if (pthread_create(&thid1,NULL,thmain1,NULL)!=0) { printf("pthread_create failed.\n"); exit(-1); }
  if (pthread_create(&thid2,NULL,thmain1,NULL)!=0) { printf("pthread_create failed.\n"); exit(-1); }
  //pthread_mutex_init(&mutex,NULL);
  printf("join...\n");
  pthread_join(thid1,NULL);  
  pthread_join(thid2,NULL);  
  printf("join ok.\n");
  printf("aa =%d\n",aa);
  pthread_mutex_destroy(&mutex);
}

void *thmain1(void *arg)    // 线程主函数。
{
  
  printf("线程1开始运行。\n");
  for(int ii=0;ii<100000000;ii++)
  {
    pthread_mutex_lock(&mutex);
    aa++; 
    pthread_mutex_unlock(&mutex);

  }
}
