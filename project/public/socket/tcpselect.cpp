/*
 * 程序名：tcpselect.cpp，此程序演示采用开发框架的CTcpServer类实现socket通讯多线程的服务端。
*/
#include <stdio.h>
 #include <arpa/inet.h>
#include <unistd.h>
#include <sys/socket.h>
 #include <stdlib.h>
#include <sys/select.h>
#include <string.h>
       #include <sys/types.h>

int initserver(int port);

int main(int argc,char *argv[])
{
  if (argc!=2)
  {
    printf("Using:./tcpselect port \nExample:./tcpselect 5005 \n\n"); return -1;
  }

  // 关闭全部的信号和输入输出。
  // 设置信号,在shell状态下可用 "kill + 线程号" 正常终止些线程
  // 但请不要用 "kill -9 +线程号" 强行终止
  //CloseIOAndSignal(); signal(SIGINT,EXIT); signal(SIGTERM,EXIT);


  // 服务端初始化。
  int listensock=initserver(atoi(argv[1]));
  if (listensock<0) { printf("initserver failed\n"); return -1; }
  
  fd_set readfds;
  FD_ZERO(&readfds);  
  FD_SET(listensock,&readfds); 
  
 
  int maxfd=listensock; 
  struct timeval timeout; 

  while (true)
  {
    fd_set tmpfds=readfds;//传入的 &reads 应该是原始数据的拷贝
    timeout.tv_sec=10;timeout.tv_usec=0;
    int infds=select(maxfd+1,&tmpfds,NULL,NULL,&timeout);//<0 fail =0 timeout,>0 count
    if (infds<0) 
    { perror("select failed\n"); break; }

    if (infds==0) 
    { printf("timeout \n"); continue; }

    for(int eventfd=0;eventfd<=maxfd;eventfd++)
    {
      if (FD_ISSET(eventfd,&tmpfds)<=0) continue;

      //客户端=listensock;
      if (eventfd==listensock)
      {
        struct sockaddr_in client;
        socklen_t len =sizeof(client);
        int clientsock = accept(listensock,(struct sockaddr *)&client,&len);
	if (clientsock<0) { perror("accept() failed\n"); continue;  }

	printf("accept client(sock=%d) ok\n",clientsock);

        FD_SET(clientsock,&readfds);
        if (maxfd<clientsock) maxfd=clientsock;
      }
      else
      {
        char buffer[1024]; memset(buffer,0,sizeof(buffer));
        //disconnect
        if (recv(eventfd,buffer,sizeof(buffer),0)<=0)
        {
          printf("client (sock=%d) disconnect\n",eventfd);
          close(eventfd);
          FD_CLR(eventfd,&readfds);
          //重新计算maxfd
          if(maxfd==eventfd)
          {
            for(int ii=maxfd;ii>0;ii--)
            {
              if (FD_ISSET(ii,&readfds)) { maxfd=ii; break; }
            }
          }
        }
        else
        {
          printf("recv clent(sock=%d) =%s=\n",eventfd,buffer);
          fd_set tmpfds1;
          FD_ZERO(&tmpfds1);
	  FD_SET(eventfd,&tmpfds1);
          if (select(eventfd+1,NULL,&tmpfds1,NULL,NULL)<=0)
	  {
	    perror("select failed\n");
	  }
          else 
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
