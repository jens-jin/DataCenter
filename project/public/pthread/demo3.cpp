// 本程序演示线程参数的传递（用强制转换的方法传变量的值）。
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>

void *thmain1(void *arg);    // 线程1的主函数。
int var;
struct st_args
{
  int no;
  char name[10];
};
int main(int argc,char *argv[])
{
  pthread_t thid1=0,thid2=0;   // 线程id typedef unsigned long pthread_t

  // 创建线程。
  struct st_args *stargs=new struct st_args;
  stargs->no=14;strcpy(stargs->name,"cheshi");
  if (pthread_create(&thid1,NULL,thmain1,stargs)!=0) { printf("pthread_create failed.\n"); exit(-1); }

  // 等待子线程退出。
  printf("join...\n");
  pthread_join(thid1,NULL);  
  printf("join ok.\n");
}

void *thmain1(void *arg)    // 线程主函数。
{
  struct st_args *ptr=(struct st_args*)arg;
  printf("no=%d\n,name=%s\n",ptr->no,ptr->name);
  printf("线程1开始运行。\n");
  delete (struct st_args *)arg;
}
