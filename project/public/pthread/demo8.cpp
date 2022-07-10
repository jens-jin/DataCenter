// 本程序演示线程参数的传递（用强制转换的方法传变量的值）。
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>
#include <signal.h>
int aa=0;
void func(int sig);
void *thmain1(void *arg);    // 线程1的主函数。
void *thmain2(void *arg);    // 线程1的主函数。
pthread_mutex_t mutex= PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond=PTHREAD_COND_INITIALIZER;
int main(int argc,char *argv[])
{
  signal(15,func);
  pthread_t thid1=0,thid2=0;
  // 创建线程。
  if (pthread_create(&thid1,NULL,thmain1,NULL)!=0) { printf("pthread_create failed.\n"); exit(-1); }
  if (pthread_create(&thid2,NULL,thmain2,NULL)!=0) { printf("pthread_create failed.\n"); exit(-1); }
 // pthread_rwlock_init(&mutex,NULL);
  printf("join...\n");
  pthread_join(thid1,NULL);  
  pthread_join(thid2,NULL);  
  printf("join ok.\n");
  printf("aa =%d\n",aa);
  pthread_mutex_destroy(&mutex);
  pthread_cond_destroy(&cond);
}

void *thmain1(void *arg)    // 线程主函数。
{
  printf("线程1开始运行。\n");
  printf("线程1开始申请锁。\n");
  pthread_mutex_lock(&mutex);
  printf("线程1申请锁成功。\n");
  sleep(3);
  printf("线程1开始等待条件信号\n");
  pthread_cond_wait(&cond,&mutex);
  printf("线程1等待条件信号成功\n");
}

void *thmain2(void *arg)    // 线程主函数。
{
  printf("线程2开始申请锁。\n");
  pthread_mutex_lock(&mutex);
  printf("线程2申请锁成功。\n");
  pthread_cond_signal(&cond);sleep(10);
  printf("2解锁\n");
  pthread_mutex_unlock(&mutex);
  return 0;
  printf("线程2开始运行。\n");
  printf("线程2开始申请锁。\n");
  pthread_mutex_lock(&mutex);
  printf("线程2申请锁成功。\n");
  printf("线程2开始等待条件信号\n");
  pthread_cond_wait(&cond,&mutex);
  printf("线程2等待条件信号成功\n");
}

void func(int sig)
{
  printf("申请锁..\n");
  pthread_mutex_lock(&mutex);
  printf("申请锁ok..\n");
}
