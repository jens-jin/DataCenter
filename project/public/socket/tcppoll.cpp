/*
 * 程序名：tcppoll.cpp，此程序演示采用开发框架的CTcpServer类实现socket通讯多线程的服务端。
*/
#include <stdio.h>
 #include <arpa/inet.h>
#include <unistd.h>
#include <sys/socket.h>
 #include <stdlib.h>
#include <sys/select.h>
#include <string.h>
#include <sys/types.h>
#include <poll.h>
//ulimit -a  打开文件数 sockfd 为文件

#define MAXNFDS 1024

int initserver(int port);

int main(int argc,char *argv[])
{
  int aa=1,bb=1;
  if((aa&bb)==0){printf("POLL=");return -1;}
  printf("POLL=%d\n",POLLIN);
  if (argc!=2)
  {
    printf("Using:./tcppoll port \nExample:./tcppoll 5005 \n\n"); return -1;
  }

  // 关闭全部的信号和输入输出。
  // 设置信号,在shell状态下可用 "kill + 线程号" 正常终止些线程
  // 但请不要用 "kill -9 +线程号" 强行终止
  //CloseIOAndSignal(); signal(SIGINT,EXIT); signal(SIGTERM,EXIT);


  // 服务端初始化。
  int listensock=initserver(atoi(argv[1]));
  if (listensock<0) { printf("initserver failed\n"); return -1; }
 
  struct pollfd fds[MAXNFDS]; 
  for(int ii=0;ii<MAXNFDS;ii++) fds[ii].fd=-1;

  fds[listensock].fd=listensock;
  fds[listensock].events=POLLIN;
 
  int maxfd=listensock; 
  struct timeval timeout; 

  while (true)
  {
    int infds=poll(fds,maxfd+1,5000);//time=-1 无限 0 立即
    if (infds<0) 
    { perror("poll failed\n"); break; }

    if (infds==0) 
    { printf("timeout \n"); continue; }

    for(int eventfd=0;eventfd<=maxfd;eventfd++)
    {
      if (fds[eventfd].fd<0) continue;
      if ((fds[eventfd].revents&POLLIN)==0)continue;//返回事件revents是否与events相同
      fds[eventfd].revents=0;

    
      //客户端=listensock;
      if (eventfd==listensock)
      {
        struct sockaddr_in client;
        socklen_t len =sizeof(client);
        int clientsock = accept(listensock,(struct sockaddr *)&client,&len);
	if (clientsock<0) { perror("accept() failed\n"); continue;  }

	printf("accept client(sock=%d) ok\n",clientsock);

        fds[clientsock].fd=clientsock;
        fds[clientsock].events=POLLIN;
        if (maxfd<clientsock) maxfd=clientsock;
      }
      //客户端事件
      else
      {
        char buffer[1024]; memset(buffer,0,sizeof(buffer));
        //disconnect
        if (recv(eventfd,buffer,sizeof(buffer),0)<=0)
        {
          printf("client (sock=%d) disconnect\n",eventfd);
          close(eventfd);
          fds[eventfd].fd=-1;
          fds[eventfd].events=0;
          //重新计算maxfd
          if(maxfd==eventfd)
          {
            for(int ii=maxfd;ii>0;ii--)
            {
              if (fds[ii].fd!=-1) { maxfd=ii; break; }
            }
          }
        }
        else
        {
          printf("recv client(sock=%d) =%s=\n",eventfd,buffer);
	  send(eventfd,buffer,strlen(buffer),0);//只需这一行
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
