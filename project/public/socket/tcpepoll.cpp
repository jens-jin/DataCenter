/*
 * 程序名：tcpepoll.cpp，此程序演示采用开发框架的CTcpServer类实现socket通讯多线程的服务端。
*/
#include <stdio.h>
 #include <arpa/inet.h>
#include <unistd.h>
#include <sys/socket.h>
 #include <stdlib.h>
#include <sys/select.h>
#include <string.h>
#include <time.h>
#include <sys/types.h>
#include <sys/epoll.h>
#include <errno.h>
#include <fcntl.h>

int initserver(int port);

int main(int argc,char *argv[])
{
  if (argc!=2)
  {
    printf("Using:./tcpepoll port \nExample:./tcpepoll 5005 \n\n"); return -1;
  }

  // 关闭全部的信号和输入输出。
  // 设置信号,在shell状态下可用 "kill + 线程号" 正常终止些线程
  // 但请不要用 "kill -9 +线程号" 强行终止
  //CloseIOAndSignal(); signal(SIGINT,EXIT); signal(SIGTERM,EXIT);
  // 服务端初始化。
  int listensock=initserver(atoi(argv[1]));
  if (listensock<0) { printf("initserver failed\n"); return -1; }
  
  fcntl(listensock,F_SETFL,fcntl(listensock,F_GETFD,0)|O_NONBLOCK);//非阻塞模式
 
  int epollfd=epoll_create(1);

  struct epoll_event ev;
  ev.data.fd=listensock;
  ev.events=EPOLLIN|EPOLLET;//边缘触发
  epoll_ctl(epollfd,EPOLL_CTL_ADD,listensock,&ev);   
 
  struct epoll_event evs[10];//返回句柄

  while (true)
  {
    int infds=epoll_wait(epollfd,evs,10,5000);
    if (infds<0) 
    { perror("epoll failed\n"); break; }

    if (infds==0) 
    { printf("timeout \n"); continue; }

    for(int ii=0;ii<infds;ii++)
    {
   
      printf("infds=%d\n",infds);
      //客户端=listensock;
      if(evs[ii].data.fd==listensock)
      {
        while(true)
        {
          struct sockaddr_in client;
          socklen_t len =sizeof(client);
          int clientsock = accept(listensock,(struct sockaddr *)&client,&len);
          if (errno==EAGAIN) break;
    	 // if (clientsock<0) { perror("accept() failed\n"); continue;  }

 	  printf("time-%ld accept client(sock=%d) ok\n",time(0),clientsock);
          fcntl(clientsock,F_SETFL,fcntl(clientsock,F_GETFD,0)|O_NONBLOCK);
          ev.data.fd=clientsock;
          ev.events=EPOLLIN|EPOLLET;
          epoll_ctl(epollfd,EPOLL_CTL_ADD,clientsock,&ev);
          break;
        }
      }
      else
      {
        char buffer[1024]; memset(buffer,0,sizeof(buffer));
        //disconnect
        int iret=0;
        char *ptr=buffer;
        while(true)
        {
          if ((iret=recv(evs[ii].data.fd,ptr,10,0))<=0)
          {
            if(errno==EAGAIN) break; 
            printf("time=%ld client (sock=%d) disconnect\n",time(0),evs[ii].data.fd);
            close(evs[ii].data.fd);
            break;
          }
          ptr+=iret;
        }
        if(strlen(buffer)>0)
        {
          printf("recv cilent(sock=%d) =%s=\n",evs[ii].data.fd,buffer);
	   while(true)
           {
              send(evs[ii].data.fd,buffer,strlen(buffer),0);//只需这一行
              if(errno==EAGAIN) break;
           }
        }
      }
    }
  }

  return 0;
}

int initserver(int port)
{
  int sock=socket(AF_INET,SOCK_STREAM,0);
  if(sock<0)
  { perror("socket failed\n"); return -1; }
  
  int opt=1; unsigned int len= sizeof(opt);
  setsockopt(sock,SOL_SOCKET,SO_REUSEADDR,&opt,len);

  struct sockaddr_in servaddr;
  servaddr.sin_family=AF_INET;
  servaddr.sin_addr.s_addr=htonl(INADDR_ANY);
  servaddr.sin_port=htons(port);
  
  if (bind(sock,(struct sockaddr *)&servaddr,sizeof(servaddr)) <0 )
  { perror("bind failed\n"); close(sock); return -1; }

  if(listen(sock,5)!=0)
  { perror("listen failed\n"); close(sock); return -1; }

  return sock; 
}
