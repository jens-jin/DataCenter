/*
 * 程序名：inetd.cpp，正向代理
*/
#include "_public.h" 
CLogFile logfile;
struct st_route
{
  int listenport;
  char dip[31];
  int dport;
  int listensock;
}stroute;

vector<struct st_route> vroute;

bool Loadroute(const char *filename);

int initserver(int port);
int conntodst(const char *ip,const int port);
#define MAXSOCK 1024
int clientsocks[MAXSOCK];
int clientatime[MAXSOCK];//每个socket最后一次收发报文的时间，判断是否长时间连接
//l[10]=15,l[15]=10

void EXIT(int sig);
int main(int argc,char *argv[])
{
  if (argc!=3)
  {
    printf("Using:./inetd logfile inifile \nExample:./inetd /tmp/log/inetd.log /etc/inetd.conf  \n"); 
    printf("Example:/project/tools1/bin/inetd /tmp/log/inetd.log /etc/inetd.conf  \n\n"); return -1;
  }

  // 关闭全部的信号和输入输出。
  // 设置信号,在shell状态下可用 "kill + 线程号" 正常终止些线程
  // 但请不要用 "kill -9 +线程号" 强行终止
  //CloseIOAndSignal(); signal(SIGINT,EXIT); signal(SIGTERM,EXIT);
  // 服务端初始化。
  if (logfile.Open(argv[1],"a+")==false) 
  {
    logfile.Write("logfile.Open() failed\n");return -1;
  }
  if (Loadroute(argv[2])==false)  return -1;
  logfile.Write("Loadroute(%ld) ok.\n",vroute.size()); 

  //初始化监听socket
  for(int ii=0;ii<vroute.size();ii++)
  {  
    if ((vroute[ii].listensock=initserver(vroute[ii].listenport))<0 )
    { logfile.Write("initserver (%d) failed.\n",vroute[ii].listenport); return -1; }

    //socket->非阻塞
    fcntl(vroute[ii].listensock,F_SETFL,fcntl(vroute[ii].listensock,F_GETFD,0)|O_NONBLOCK);
  }
 
  int epollfd=epoll_create(1);
  struct epoll_event ev;

  //prepare read events
  for(int ii=0;ii<vroute.size();ii++)
  {
    ev.events=EPOLLIN;
    ev.data.fd=vroute[ii].listensock;
  
    //socket->ev
    epoll_ctl(epollfd,EPOLL_CTL_ADD,vroute[ii].listensock,&ev);   
  }
  struct epoll_event evs[10];
  while (true)
  {
    int infds=epoll_wait(epollfd,evs,10,5000);
    if (infds<0) 
    { perror("epoll failed\n"); break; }


    for(int ii=0;ii<infds;ii++)
    {
   
      logfile.Write("infds=%d\n",infds);
      /////////////////////////////////////////////////////////
      //客户端=listensock;
      int jj=0;
      for(jj=0;jj<vroute.size();jj++)
      {
        if(evs[ii].data.fd==vroute[jj].listensock)
        {
          struct sockaddr_in client;
          socklen_t len =sizeof(client);
          int srcsock = accept(vroute[jj].listensock,(struct sockaddr *)&client,&len);

    	  if (srcsock<0)  break;  
          if (srcsock>=MAXSOCK)
          { logfile.Write("连接数已达最大值\n"); close(srcsock); break; }
 	  logfile.Write("time-%ld accept client(%d) ok\n",time(0),srcsock);
    
          //发起连接
          logfile.Write("ip=%s p=%d\n",vroute[jj].dip,vroute[jj].dport);
          int dsock=conntodst(vroute[jj].dip,vroute[jj].dport);
          if (dsock<0)  break;  
          if (dsock>=MAXSOCK)
          { logfile.Write("连接数已达最大值\n"); close(dsock); break; }

          logfile.Write("accept on port %d client(%d,%d)\n",vroute[jj].listensock,srcsock,dsock);

          //准备可读事件，添加到epoll
          ev.data.fd=srcsock; ev.events=EPOLLIN;
          epoll_ctl(epollfd,EPOLL_CTL_ADD,srcsock,&ev);
          ev.data.fd=dsock; ev.events=EPOLLIN;
          epoll_ctl(epollfd,EPOLL_CTL_ADD,dsock,&ev);

          //两个socket的对应关系用数组存放clientsocks value+time
          clientsocks[srcsock]=dsock; clientsocks[dsock]=srcsock;
          clientatime[srcsock]=time(0); clientatime[dsock]=time(0);         

          break;
        }
      }
      //表示事件在上面的for循环中已被处理。
      if(jj<vroute.size()) continue; 
      /////////////////////////////////////////////////////////////////
       // 如果是客户端连接的socke有事件，表示有报文发过来或者连接已断开。
      char buffer[1024];
      int buflen=0;  memset(buffer,0,sizeof(buffer));
      //disconnect
      if ((buflen=recv(evs[ii].data.fd,buffer,sizeof(buffer),0))<=0)
      {
        logfile.Write("time=%ld client (%d,%d) disconnect\n",time(0),evs[ii].data.fd,clientsocks[evs[ii].data.fd]);
        close(evs[ii].data.fd);
        close(clientsocks[evs[ii].data.fd]);
        clientsocks[clientsocks[evs[ii].data.fd]]=0;
        clientsocks[evs[ii].data.fd]=0;
        continue;
      }
      logfile.Write("from %d to %d  len=%d=\n",evs[ii].data.fd,clientsocks[evs[ii].data.fd],buflen);
      logfile.Write("recv(buffer=%s)\n",buffer);
       sleep(2);
      if(send(clientsocks[evs[ii].data.fd],buffer,buflen,0)==-1) logfile.Write("aaa\n");//只需这一行
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


bool Loadroute(const char *filename)
{
  CFile File;
  //打开文件
  File.Open(filename,"r");
  //读取一行(while(ture))
  char strbuffer[301];
  CCmdStr CmdStr;
  while(true)
  {
    memset(strbuffer,0,sizeof(strbuffer));
    if(File.Fgets(strbuffer,300,true)==false) break;
    //分割字符串
    char *pos=strstr(strbuffer , "#");
    //删除说明文字。
    if (pos!=0)pos[0]=0;
    //删除右边的空格。
    DeleteRChar(strbuffer,' ');
    //把两个空格替换成一个空格，注意第三个参数。  一直替换 直到一个空格
    UpdateStr(strbuffer , "  "," ",true);
    CmdStr.SplitToCmd(strbuffer ," ");
    if(CmdStr.CmdCount()!=3) continue;

    memset(&stroute,0,sizeof(struct st_route));
    CmdStr.GetValue(0,&stroute.listenport);
    CmdStr.GetValue(1,stroute.dip);
    CmdStr.GetValue(2 ,&stroute.dport);
    logfile.Write("port=%d,ip=%s,p=%d\n",stroute.listenport,stroute.dip,stroute.dport);
    //放入容器
    vroute.push_back(stroute);

  }
  return true;
}

int conntodst(const char *ip,const int port)
{
   // 第1步：创建客户端的socket。
  int sockfd;
  if ( (sockfd = socket(AF_INET,SOCK_STREAM,0))==-1) return -1;
  
  // 第2步：向服务器发起连接请求。
  struct hostent* h;
  if ( (h = gethostbyname(ip)) == 0 ) { close(sockfd); return -1; }
  
  struct sockaddr_in servaddr;
  memset(&servaddr,0,sizeof(servaddr));
  servaddr.sin_family = AF_INET;
  servaddr.sin_port = htons(port); // 指定服务端的通讯端口。
  memcpy(&servaddr.sin_addr,h->h_addr,h->h_length);
  
  // 把socket设置为非阻塞。
  fcntl(sockfd,F_SETFL,fcntl(sockfd,F_GETFD,0)|O_NONBLOCK);
  
  if(connect(sockfd, (struct sockaddr *)&servaddr,sizeof(servaddr))==-1) logfile.Write("connect failed\n");
  return sockfd;
}

void EXIT(int sig)
{
  logfile.Write("程序退出，收到信号=%d\n",sig);
  for(int ii=0;ii<vroute.size();ii++) close(vroute[ii].listensock);
  for(int ii=0;ii<MAXSOCK;ii++) 
     if(clientsocks[ii]>0) close(clientsocks[ii]);
  
}
