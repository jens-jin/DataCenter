#include "../_public.h"

int main(int argc,char *argv[])
{
  int sockfd=socket(AF_INET,SOCK_STREAM,0);
  
  //struct hostent* h;
  //h=gethostbyname("127.0.0.1");
  struct sockaddr_in servaddr;
  memset(&servaddr,0,sizeof(servaddr));
  servaddr.sin_family = AF_INET;
  servaddr.sin_port = htons(atoi("5005")); // 指定服务端的通讯端口。
  servaddr.sin_addr.s_addr = inet_addr("127.0.0.1"); // 指定服务端的ip。
  //struct hostent* h;
  //h=gethostbyname(argv[1]);
  //memcpy(&servaddr.sin_addr,h->h_addr,h->h_length);// 指定服务端的ip。
  if (connect(sockfd, (struct sockaddr *)&servaddr,sizeof(servaddr)) != 0)  // 向服务端发起连接清求。
  { perror("connect"); close(sockfd); return -1; }
  
    int iret;
  char buffer[102400];

  // 第3步：与服务端通讯，发送一个报文后等待回复，然后再发下一个报文。
  for (int ii=0;ii<1000;ii++)
  {
    memset(buffer,0,sizeof(buffer));
    sprintf(buffer,"这是第%d个超级女生，编号%03d。",ii+1,ii+1);
    if ( (iret=send(sockfd,buffer,strlen(buffer),0))<=0) // 向服务端发送请求报文。
    { perror("send"); break; }
    //printf("发送：%s\n",buffer);

   /* memset(buffer,0,sizeof(buffer));
    if ( (iret=recv(sockfd,buffer,sizeof(buffer),0))<=0) // 接收服务端的回应报文。
    {
      printf("iret=%d\n",iret); break;
    }
    printf("接收：%s\n",buffer);
*/
    //sleep(1);  // 每隔一秒后再次发送报文。
  }
}
