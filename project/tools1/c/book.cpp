#include "_public.h"

void EXIT(int sig)
{
  printf("sig=%d\n",sig);
  if(sig==2)exit(0);//exit函数不会调用局部对象的析构函数。
}

CPActive Active;
int main(int argc,char *argv[])
{
  
  if(argc!=3){printf("Using:./book provname timeout \n");return -1;}
  signal(2,EXIT); signal(15,EXIT);

  Active.AddPInfo(atoi(argv[2]),argv[1]);
  
  while(true)
  {
    // Active.UptATime();

    sleep(10);
  }  


  return 0;
}
