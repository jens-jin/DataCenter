#include <stdio.h>
 #include <arpa/inet.h>
#include <unistd.h>
#include <sys/socket.h>
 #include <stdlib.h>
#include <sys/select.h>
#include <string.h>
#include <fcntl.h>
#include <poll.h>
#include <sys/types.h>

int initserver(int port);

int main(int argc,char *argv[])
{
  if (argc!=3)
  {
    printf("Using:./client ip port \nExample:./tcpselect 127.0.0.1 5006 \n\n"); return -1;
  }

  // 关闭全部的信号和输入输出。
  //   // 设置信号,在shell状态下可用 "kill + 线程号" 正常终止些线程
  int sockfd;
  struct sockaddr_in servaddr;
  char buffer[1024];
  
  if ( (sockfd=socket(AF_INET,SOCK_STREAM,0))<0 ){close(sockfd);return -1;}
  memset(&servaddr,0,sizeof(servaddr));
  servaddr.sin_family=AF_INET;
  servaddr.sin_addr.s_addr=inet_addr(argv[1]);
  servaddr.sin_port=htons(atoi(argv[2]));
  //fcntl(sockfd,F_SETFL,fcntl(sockfd,F_GETFD,0)|O_NONBLOCK);
  if( connect(sockfd,(struct sockaddr *)&servaddr,sizeof(servaddr))!=0){close(sockfd);return -1;}
  
  printf("ok\n");
/* 
  int bufsize=0;  socklen_t optlen=sizeof(bufsize);
  getsockopt(sockfd, SOL_SOCKET, SO_RCVBUF,&bufsize, &optlen);
  printf("len=%d\n",bufsize);
  getsockopt(sockfd, SOL_SOCKET, SO_SNDBUF,&bufsize, &optlen);
  printf("len=%d\n",bufsize);
  return 0;
 */
  for(int ii=0;ii<2000000;ii++)
  {
    memset(buffer,0,sizeof(buffer));
    printf("please input:");scanf("%s",buffer);
    //strcpy(buffer,"ffffffffffff");
   if (send(sockfd,buffer,strlen(buffer),0)<=0){close(sockfd);return -1;}
  /* 
   struct pollfd fds;
   fds.fd=sockfd;
   fds.events=POLLIN;
   poll(&fds,1,-1);
*/
    memset(buffer,0,sizeof(buffer));
   if (recv(sockfd,buffer,sizeof(buffer),0)<=0){close(sockfd);return -1;}
   //printf("=%s=\n",buffer);
  }
}
