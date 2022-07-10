#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>
void fun1();
void *thmain1(void *arg);
void *thmain2(void *arg);
int a=0;
int main(int argc,char *argv[])
{
  pthread_t thid1=0;
  pthread_t thid2=0;
 int *a=new int;*a=1;
  if (pthread_create(&thid1,NULL,thmain1,a)!=0) { printf("error\n"); exit(-1); } 
 int *b=new int;*b=1;
  if (pthread_create(&thid2,NULL,thmain2,b)!=0) { printf("error\n"); exit(-1); } 

  printf("...i1\n");
  pthread_join(thid1, NULL);   
  pthread_join(thid2, NULL);   
  printf("...i2\n");
}

void *thmain1(void *arg)
{
    printf("1...%d\n",*(int *)arg);
 a=100; 
    delete (int*)arg;
}
void *thmain2(void *arg)
{
    printf("2...%d\n",*(int*)arg);
    sleep(2);
    fun1();
    printf("fun1...\n");
    delete (int*)arg;
}

void fun1()
{
  printf("fun1\n");
  pthread_exit(0) ;
}
