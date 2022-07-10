// 本程序演示线程参数的传递（用强制转换的方法传变量的值）。
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>
int aa=0;

void *thmain1(void *arg);    // 线程1的主函数。

sem_t sem;
int main(int argc,char *argv[])
{
  sem_init(&sem,0,1);
  pthread_t thid1=0,thid2=0;
  // 创建线程。
  if (pthread_create(&thid1,NULL,thmain1,NULL)!=0) { printf("pthread_create failed.\n"); exit(-1); }
  if (pthread_create(&thid2,NULL,thmain1,NULL)!=0) { printf("pthread_create failed.\n"); exit(-1); }
 // pthread_rwlock_init(&mutex,NULL);
  printf("join...\n");
  pthread_join(thid1,NULL);  
  pthread_join(thid2,NULL);  
  printf("join ok.\n");
  printf("aa =%d\n",aa);
  sem_destroy(&sem);
}

void *thmain1(void *arg)    // 线程主函数。
{
  
    int mm=0;
  printf("线程1开始运行。\n");
  for(int ii=0;ii<100000000;ii++)
  {
    sem_getvalue(&sem,&mm);
    printf("%d",mm);
    sem_wait(&sem);
    sem_getvalue(&sem,&mm);
    printf("%d",mm);
    aa++; 
    sem_post(&sem);
    sem_getvalue(&sem,&mm);
    printf("%d",mm);
  }
    sem_getvalue(&sem,&mm);
    printf("%d\n",mm);
}
