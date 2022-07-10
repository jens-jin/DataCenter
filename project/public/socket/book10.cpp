/*
 * 程序名：book10.cpp，此程序演示采用开发框架的CTcpServer类实现socket通讯多线程的服务端。
*/
#include "../_public.h"
#include <pthread.h>
CLogFile logfile;      // 服务程序的运行日志。
CTcpServer TcpServer;  // 创建服务端对象。

void EXIT(int sig);  // 父线程退出函数。
void *thmain(void *arg);
void thcleanup(void *arg);
vector <pthread_t> vthid; 
pthread_spinlock_t vthidlock;
int main(int argc,char *argv[])
{
  if (argc!=3)
  {
    printf("Using:./book10 port logfile\nExample:./book10 5005 /tmp/book10.log\n\n"); return -1;
  }

  // 关闭全部的信号和输入输出。
  // 设置信号,在shell状态下可用 "kill + 线程号" 正常终止些线程
  // 但请不要用 "kill -9 +线程号" 强行终止
  CloseIOAndSignal(); signal(SIGINT,EXIT); signal(SIGTERM,EXIT);

  if (logfile.Open(argv[2],"a+")==false) { printf("logfile.Open(%s) failed.\n",argv[2]); return -1; }

  // 服务端初始化。
  if (TcpServer.InitServer(atoi(argv[1]))==false)
  {
    logfile.Write("TcpServer.InitServer(%s) failed.\n",argv[1]); return -1;
  }
  
  pthread_spin_init(&vthidlock,0);
  
  while (true)
  {
    // 等待客户端的连接请求。
    if (TcpServer.Accept()==false)
    {
      logfile.Write("TcpServer.Accept() failed.\n"); EXIT(-1);
    }

    logfile.Write("客户端（%s）已连接。\n",TcpServer.GetIP());

    pthread_t thid;
    if (pthread_create(&thid,NULL,thmain,(void*)(long)TcpServer.m_connfd)!=0)
    { logfile.Write("pthread_create() failed\n"); TcpServer.CloseListen(); continue; } 
    
    pthread_spin_lock(&vthidlock);       
    vthid.push_back(thid);
    pthread_spin_unlock(&vthidlock);       

  }
}

// 线程退出函数。
void EXIT(int sig)  
{
  // 以下代码是为了防止信号处理函数在执行的过程中被信号中断。
  signal(SIGINT,SIG_IGN); signal(SIGTERM,SIG_IGN);

  logfile.Write("父线程退出，sig=%d。\n",sig);

  TcpServer.CloseListen();    // 关闭监听的socket。

  //取消全部的进程，创建时记下thid
  for(int ii=0;ii<vthid.size();ii++)
  {
    pthread_cancel(vthid[ii]);
  }
  sleep(1);
  pthread_spin_destroy(&vthidlock);
  exit(0);
}


void *thmain(void *arg)
{
  pthread_cleanup_push(thcleanup,arg);
  int connfd=(int)(long)arg;
  // 子线程与客户端进行通讯，处理业务。
  pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS,NULL);
  pthread_detach(pthread_self());
  char buffer[102400];
  int buflen=0;

  // 与客户端通讯，接收客户端发过来的报文后，回复ok。
  while (1)
  {
    memset(buffer,0,sizeof(buffer));
    if (TcpRead(connfd,buffer,&buflen,20)==false) break; // 接收客户端的请求报文。
    logfile.Write("接收：%s\n",buffer);

    strcpy(buffer,"ok");
    if (TcpWrite(connfd,buffer)==false) break; // 向客户端发送响应结果。
    logfile.Write("发送：%s\n",buffer);
  }
  close(connfd);
  pthread_spin_lock(&vthidlock);       
  for(int ii=0;ii<vthid.size();ii++)
  {
    if (pthread_equal(pthread_self(),vthid[ii])) { vthid.erase(vthid.begin()+ii); break; }
  }
  pthread_spin_unlock(&vthidlock);       

  pthread_cleanup_pop(1);
}

void thcleanup(void *arg)
{
 close((int)(long)arg);
 logfile.Write("线程%lu退出\n",pthread_self()); 
}
