/*
 *s.cpp 服务端
 *
 */
#include "../_public.h"

class CTcpServer1
{
public:
  
  int listenfd;
  int clientfd;
 CTcpServer1()
  {
    listenfd=0;
    clientfd=0;
  }

  ~CTcpServer1()
  {
     close(listenfd);
     close(clientfd);   
  }
  bool initserver(int port)
  {
    listenfd=socket(AF_INET,SOCK_STREAM,0);
    if(listenfd==-1){printf("socket\n");return false;} 

    struct sockaddr_in servaddr;
    memset(&servaddr,0,sizeof(servaddr));
    servaddr.sin_family=AF_INET;
    servaddr.sin_addr.s_addr=htonl(INADDR_ANY);
   // servaddr.sin_addr.s_addr=inet_addr("127.0.0.1");
    servaddr.sin_port=htons(port); 
    if(bind(listenfd,(struct sockaddr *)&servaddr,sizeof(servaddr))!=0){perror("bind\n");return false;}

    if(listen(listenfd,5)!=0) {printf("listen\n");return false;}
    
    return true;
  }

  bool Accept()
  {
    struct sockaddr_in clientaddr;
    int len=sizeof( struct sockaddr_in);
    if((clientfd=accept(listenfd,(struct sockaddr *)&clientaddr,(socklen_t *)&len))!=0)return false;

    printf("客户端（%s）已连接。\n",inet_ntoa(clientaddr.sin_addr));
    return true;
  }


};

int main()
{
  //create struct  sockaddr ->...
  //bind=socket
  //accept
  //send/recv
  //
  //创建socket=socket();，绑定ip,port=bind();
  //listen());

  CTcpServer1 TcpServer;
  TcpServer.initserver(5005);
  TcpServer.Accept();
  int iret=0;
  char buffer[102400];
  while(1)
  {
    memset(buffer,0,sizeof(buffer));
    if ( (iret=recv(TcpServer.clientfd,buffer,sizeof(buffer),0))<=0) // 接收客户端的请求报文。
    {
       printf("iret=%d\n",iret); break;
    }
    printf("接收：%s\n\n",buffer);
    //strcpy(buffer,"ok");
    //send(TcpServer.clientfd,buffer,strlen(buffer),0);
    //printf("发送：%s\n",buffer);
  } 
   // socket(); connect() recv/send


}
