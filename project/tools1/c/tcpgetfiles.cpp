/*
 * 程序名：tcpgetfiles.cpp，此程序为tcp文件传输系统的客户端下载模块
 * 
*/
#include "_public.h"

//程序运行的参数
struct st_arg{
  int clienttype;// 客户端类型，1-下载文件，2-下载文件。
  char ip[31];// 服务端的ip。
  int port;// 服务端的端口。
  int  ptype;              // 文件下载后文件的处理方式：1-删除；2-移动到备份目录。
  char srvpath[301];// 服务端存放文件的根目录。
  char srvpathbak[301];//服务端文件存放的备份目录。
  bool andchild;        //是否下载srvpath的子目录。
  char matchname[101];//下载文件匹配的规则。
  char clientpath[301];//客户端文件存放的根目录。
  int timetvl;  //扫描服务端文件的时间间隔。
  int timeout;//进程心跳的超时时间
  char pname[51];//进程名，建议用"tcpgetfiles_后缀"
} starg;

CTcpClient TcpClient;

CLogFile logfile;

bool Login(const char *argv);    // 登录业务。
void _tcpgetfiles();//下载文件主函数
//接收上传文件内容的主函数
bool RecvFile(const int connfd,const char *filename,const char *mtime,size_t filesize);
char strrecvbuffer[1024],strsendbuffer[1024];

void EXIT(int sig);

void _help();
 
//解析参数函数
bool _xmltoarg(char *strxmlbuffer);

CPActive PActive;//进程心跳

int main(int argc,char *argv[])
{
  //help
  if(argc!=3){ _help(); return -1; }
  //把本地上的某目录下的文件下载到本地目录
  
  //关闭信号和IO 启用2 15
  CloseIOAndSignal(); signal(SIGINT,EXIT);signal(SIGTERM,EXIT);

  if( logfile.Open(argv[1],"a+")==false )
  {
    printf("logfile.Open(%s) failed.\n",argv[1]); return -1;

  }
  
  //解析参数
  if( _xmltoarg(argv[2])==false )return -1 ;  
 
  PActive.AddPInfo(starg.timeout,starg.pname);//进程心跳写入共享内存  

  // 向服务端发起连接请求。
  if (TcpClient.ConnectToServer(starg.ip,starg.port)==false)
  {
    logfile.Write("TcpClient.ConnectToServer(%s,%d) failed.\n",starg.ip,starg.port); EXIT(-1);
  }

  // 登录业务。
  if (Login(argv[2])==false) { logfile.Write("Login() failed.\n"); EXIT(-1); }
  
  //文件下载的主函数
  _tcpgetfiles(); 

  EXIT(0);
}



// 登录业务。 
bool Login(const char *argv)    
{
  memset(strrecvbuffer,0,sizeof(strrecvbuffer));
  memset(strsendbuffer,0,sizeof(strsendbuffer)); 
  
  SPRINTF(strsendbuffer,sizeof(strsendbuffer),"%s<clienttype>2</clienttype>",argv);
  logfile.Write("发送：%s\n",strsendbuffer);
  if (TcpClient.Write(strsendbuffer)==false) return false; // 向服务端发送请求报文。

  if (TcpClient.Read(strrecvbuffer,20)==false) return false; // 接收服务端的回应报文。
  logfile.Write("接收：%s\n",strrecvbuffer);

  logfile.Write("登录(%s:%d)成功。\n",starg.ip,starg.port); 

  return true;
}

void _help()
{
  printf("\n");
  printf("USing:/project/tools1/bin/tcpgetfiles logfilename xmlbuffer\n ");
  printf("Sample:/project/tools1/bin/procctl 30 /project/tools1/bin/tcpgetfiles /log/idc/tcpgetfiles.log"\
         " \"<ip>127.0.0.1</ip><port>5005</port><ptype>1</ptype>"\
         "<srvpath>/tmp/tcp/surfdata2</srvpath><srvpathbak>/tmp/tcp/surfdata2bak</srvpathbak>"\
         "<andchild>true</andchild><matchname>*.XML,*.CSV</matchname>"\
         "<clientpath>/tmp/tcp/surfdata1</clientpath>"\
         "<timetvl>10</timetvl><timeout>50</timeout>"\
         "<pname>tcpgetfiles_surfdata</pname>\"\n\n\n");
  printf("Sample:/project/tools1/bin/procctl 30 /project/tools1/bin/tcpgetfiles /log/idc/tcpgetfiles.log"\
         " \"<ip>127.0.0.1</ip><port>5005</port><ptype>2</ptype>"\
         "<srvpath>/tmp/tcp/surfdata2</srvpath><srvpathbak>/tmp/tcp/surfdata2bak</srvpathbak>"\
         "<andchild>true</andchild><matchname>*.XML,*.CSV</matchname>"\
         "<clientpath>/tmp/tcp/surfdata1</clientpath>"\
         "<timetvl>10</timetvl><timeout>50</timeout>"\
         "<pname>tcpgetfiles_surfdata</pname>\"\n\n\n");
  printf("本程序是数据中心的公共功能模块，采用tcp下载文件。\n");
  printf("logfilename是本程序运行的日志文件。\n");
  printf("xmlbuffer为文件下载的参数，如下：\n");
  printf(" clienttype  客户端类型，1-下载文件，2-下载文件\n");
  printf("ip  服务端的ip\n");
  printf("port 服务端的端口\n");
  printf("ptype 文件下载后文件的处理方式：1-删除；2-移动到备份目录。\n");
  printf("srvpath 本地存放文件的根目录。\n");
  printf("srvpathbak 本地文件存放的备份目录。\n");
  printf("andchild 是否下载clientpath的子目录\n");
  printf("matchname 下载文件匹配的规则。\n");
   printf("clientpath 本地文件存放的根目录。\n");
  printf("timetvl 扫描本地文件的时间间隔。\n");
  printf("timeout 下载文件超时时间\n");
  printf("pname 进程名\n\n\n");
}


void EXIT(int sig)
{
  logfile.Write("程序退出。收到sig=%d",sig);
  exit(0);
}

bool _xmltoarg(char *strxmlbuffer)
{
  memset(&starg,0,sizeof(struct st_arg));
  GetXMLBuffer(strxmlbuffer,"ip",starg.ip,30);
  if ( strlen(starg.ip)==0 ) { logfile.Write("ip is null .\n"); return false; }

  GetXMLBuffer(strxmlbuffer,"port",&starg.port);
  if ( starg.port==0 ){ logfile.Write("port is null .\n"); return false; } 

  GetXMLBuffer(strxmlbuffer,"ptype",&starg.ptype);
  if ( starg.ptype!=1 && starg.ptype!=2) {logfile.Write("ptype is not 1/2 .\n"); return false;}

  GetXMLBuffer(strxmlbuffer,"srvpath",starg.srvpath,300);
  if ( strlen(starg.srvpath)==0 ) {logfile.Write("srvpath is null .\n"); return false; }

  GetXMLBuffer(strxmlbuffer,"srvpathbak",starg.srvpathbak,300);
  if ( strlen(starg.srvpathbak)==0 ) { logfile.Write("srvpathbak is null .\n"); return false;}

  GetXMLBuffer(strxmlbuffer,"andchild",&starg.andchild);

  GetXMLBuffer(strxmlbuffer,"matchname",starg.matchname,100);
  if ( strlen(starg.matchname)==0 ) { logfile.Write("matchname is null .\n"); return false; }
  
  GetXMLBuffer(strxmlbuffer,"clientpath",starg.clientpath,300);
  if ( strlen(starg.clientpath)==0 ) {  logfile.Write("clientpath is null .\n"); return false; }
 
  GetXMLBuffer(strxmlbuffer,"timetvl",&starg.timetvl);
  if ( starg.timetvl==0 ) { logfile.Write("timetvl is null.\n"); return false; }
 // 扫描本地目录文件的时间间隔，单位：秒。
 // starg.timetvl没有必要超过30秒。
  if (starg.timetvl>30) starg.timetvl=30;

 // 进程心跳的超时时间，一定要大于starg.timetvl，没有必要小于50秒。
  GetXMLBuffer(strxmlbuffer,"timeout",&starg.timeout);
  if ( starg.timeout==0 ) { logfile.Write("timeout is null.\n");  return false; }
  if (starg.timeout<50) starg.timeout=50;

  GetXMLBuffer(strxmlbuffer,"pname",starg.pname,50); 
  if ( strlen(starg.pname)==0 ) { logfile.Write("pname is null.\n");  return false; }
  
  return true;
}

//下载文件主函数
void _tcpgetfiles()
{
  PActive.AddPInfo(starg.timeout,starg.pname);//进程心跳写入共享内存 
  while(true)
  {
     memset(strrecvbuffer,0,sizeof(strrecvbuffer));
     memset(strsendbuffer,0,sizeof(strsendbuffer));
   
     PActive.UptATime();
 
     //接收服务端的报文
     if(TcpClient.Read(strrecvbuffer,starg.timetvl+10)==false)
     { logfile.Write("TcpClient.Read() failed\n");return; }
     //logfile.Write("strrecvbuffer=%s\n",strrecvbuffer);
    
     //处理心跳报文
     if(strcmp(strrecvbuffer,"<activetest>ok</activetest>")==0)
     {
       strcpy(strsendbuffer,"ok");
       //logfile.Write("strrecvbuffer=%s\n",strrecvbuffer);
       if(TcpClient.Write(strsendbuffer)==false)
       { logfile.Write("TcpClient.Write() failed\n");return; }
     }

     //处理下载文件的报文
     if(strncmp(strrecvbuffer,"<filename>",10)==0)
     {
       //解析xml
        char serverfilename[301]; memset(serverfilename,0,sizeof(serverfilename));
        char mtime[21];           memset(mtime,0,sizeof(mtime));
        size_t  filesize=0;
        GetXMLBuffer(strrecvbuffer,"filename", serverfilename,300); 
        GetXMLBuffer(strrecvbuffer,"mtime", mtime,19); 
        GetXMLBuffer(strrecvbuffer,"size", &filesize); 
        
       //利用UpdateStr函数替换
       char clientfilename[301]; memset(clientfilename,0,sizeof(clientfilename));
       strcpy(clientfilename,serverfilename);
       UpdateStr(clientfilename,starg.srvpath,starg.clientpath,false);

       //接收内容,先生成文件名
       logfile.Write("recv %s(%d) ...",clientfilename,filesize);
       if( RecvFile(TcpClient.m_connfd,clientfilename,mtime,filesize)==true )
       {
         logfile.WriteEx("ok\n");
         SNPRINTF(strsendbuffer,sizeof(strsendbuffer),1000,"<filename>%s</filename><result>ok</result>",serverfilename);
       }
       else 
       {
         logfile.WriteEx("failed\n");
         SNPRINTF(strsendbuffer,sizeof(strsendbuffer),1000,"<filename>%s</filename><result>failed</result>",clientfilename);
       }

       //logfile.Write("strsendbuffer=%s\n",strsendbuffer);       
       if(TcpClient.Write(strsendbuffer)==false)
       { logfile.Write("TcpClient.Write() failed\n");return; }
       //返回结果给对端    


     }
   }
}


//接收下载文件内容的主函数
bool RecvFile(const int connfd,const char *filename,const char *mtime,size_t filesize)
{
  //生成临时文件名
  char strfilenametmp[301];
  SNPRINTF(strfilenametmp,sizeof(strfilenametmp),300,"%s.tmp",filename);
  
  size_t totalbytes=0;
  size_t onread=0;
  char buffer[1000];
  FILE *fp=NULL;

  //创建临时文件
  if( (fp=FOPEN(strfilenametmp,"wb"))==NULL ) return false;

  //写入文件while
  while(true)
  {
    memset(buffer,0,sizeof(buffer));
    //计算本次应该接收的字节数
    if(filesize-totalbytes>1000) onread=1000;
    else onread=filesize-totalbytes;


    //接收文件内容
    if( Readn(connfd,buffer,onread)==false)
    {fclose(fp);return false; }
    
    //写入文件
    fwrite(buffer,1,onread,fp);

    //计算接收总字节数
    totalbytes+=onread;
    if(filesize==totalbytes) break;
  }
  //关闭临时文件
  fclose(fp);
 
  //重置文件时间
  UTime(strfilenametmp,mtime);

  //重命名文件
  if( RENAME(strfilenametmp,filename)==false ) return false;

  return true;
}



