/*
 * 程序名：tcpputfiles.cpp，tcp文件上传。
 * 作者：jens
*/
#include "_public.h"

//程序运行的参数
struct st_arg{
  int clienttype;// 客户端类型，1-上传文件，2-下载文件。
  char ip[31];// 服务端的ip。
  int port;// 服务端的端口。
  int  ptype;              // 文件上传后文件的处理方式：1-删除；2-移动到备份目录。
  char clientpath[301];// 本地存放文件的根目录。
  char clientpathbak[301];//本地文件存放的备份目录。
  bool andchild;        //是否上传clientpath的子目录。
  char matchname[101];//上传文件匹配的规则。
  char srvpath[301];//服务器文件存放的根目录。
  int timetvl;  //扫描本地文件的时间间隔。
  int timeout;//进程心跳的超时时间
  char pname[51];//进程名，建议用"tcpputfiles_后缀"
} starg;

CTcpClient TcpClient;

CLogFile logfile;

void _help();
void EXIT(int sig);
bool Login(const char *argv);    // 登录业务。
bool ActiveTest();    // 心跳。
bool _tcpputfiles();//上传文件主函数
bool bcontinue=true; //如果调用_tcpputfiles发送了文件，bcontinue=true,初始化true
bool SendFile(const int connfd,const char *filename,const size_t filesize);//发送文件函数
bool AckMessage(const char *buffer);//删除或转存文件
char strrecvbuffer[1024],strsendbuffer[1024];


 
//解析参数函数
bool _xmltoarg(char *strxmlbuffer);

CPActive PActive;//进程心跳

int main(int argc,char *argv[])
{
  //help
  if(argc!=3){ _help(); return -1; }
  //把服务器上的某目录下的文件下载到本地目录
  
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
  
  while(true) 
  {
    //文件上传的主函数
    if(_tcpputfiles()==false){ logfile.Write("_tcpputfiles() failed.\n"); EXIT(-1); }
    if(bcontinue==false)
    {
      sleep(starg.timetvl);
      if (ActiveTest()==false) break;
    }
    PActive.UptATime();
  } 

  EXIT(0);
}

// 心跳。 
bool ActiveTest()    
{
  memset(strrecvbuffer,0,sizeof(strrecvbuffer));
  memset(strsendbuffer,0,sizeof(strsendbuffer)); 
  
  SPRINTF(strsendbuffer,sizeof(strsendbuffer),"<activetest>ok</activetest>");
  //logfile.Write("发送：%s\n",strsendbuffer);
  if (TcpClient.Write(strsendbuffer)==false) return false; // 向服务端发送请求报文。

  if (TcpClient.Read(strrecvbuffer,20)==false) return false; // 接收服务端的回应报文。
  //logfile.Write("接收：%s\n",strrecvbuffer);

  return true;
}

// 登录业务。 
bool Login(const char *argv)    
{
  memset(strrecvbuffer,0,sizeof(strrecvbuffer));
  memset(strsendbuffer,0,sizeof(strsendbuffer)); 
  
  SPRINTF(strsendbuffer,sizeof(strsendbuffer),"%s<clienttype>1</clienttype>",argv);
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
  printf("USing:/project/tools1/bin/tcpputfiles logfilename xmlbuffer\n ");
  printf("Sample:/project/tools1/bin/procctl 30 /project/tools1/bin/tcpputfiles /log/idc/tcpputfiles.log"\
         " \"<ip>127.0.0.1</ip><port>5005</port><ptype>1</ptype>"\
         "<clientpath>/tmp/tcp/surfdata1</clientpath><clientpathbak>/tmp/tcp/surfdata1bak</clientpathbak>"\
         "<andchild>true</andchild><matchname>*.XML,*.CSV</matchname>"\
         "<srvpath>/tmp/tcp/surfdata2</srvpath>"\
         "<timetvl>10</timetvl><timeout>50</timeout>"\
         "<pname>tcpputfiles_surfdata</pname>\"\n\n\n");
  printf("Sample:/project/tools1/bin/procctl 30 /project/tools1/bin/tcpputfiles /log/idc/tcpputfiles.log"\
         " \"<ip>127.0.0.1</ip><port>5005</port><ptype>2</ptype>"\
         "<clientpath>/tmp/tcp/surfdata1</clientpath><clientpathbak>/tmp/tcp/surfdata1bak</clientpathbak>"\
         "<andchild>true</andchild><matchname>*.XML,*.CSV</matchname>"\
         "<srvpath>/tmp/tcp/surfdata2</srvpath>"\
         "<timetvl>10</timetvl><timeout>50</timeout>"\
         "<pname>tcpputfiles_surfdata</pname>\"\n\n\n");
  printf("本程序是数据中心的公共功能模块，采用tcp上传文件。\n");
  printf("logfilename是本程序运行的日志文件。\n");
  printf("xmlbuffer为文件下载的参数，如下：\n");
  printf("clienttype  客户端类型，1-上传文件，2-下载文件\n");
  printf("ip  服务端的ip\n");
  printf("port 服务端的端口\n");
  printf("ptype 文件上传后文件的处理方式：1-删除；2-移动到备份目录。\n");
  printf("clientpath 本地存放文件的根目录。\n");
  printf("clientpathbak 本地文件存放的备份目录。\n");
  printf("andchild 是否上传clientpath的子目录\n");
  printf("matchname 上传文件匹配的规则。\n");
   printf("srvpath 服务器文件存放的根目录。\n");
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

  GetXMLBuffer(strxmlbuffer,"clientpath",starg.clientpath,300);
  if ( strlen(starg.clientpath)==0 ) {logfile.Write("clientpath is null .\n"); return false; }

  GetXMLBuffer(strxmlbuffer,"clientpathbak",starg.clientpathbak,300);
  if ( strlen(starg.clientpathbak)==0 ) { logfile.Write("clientpathbak is null .\n"); return false;}

  GetXMLBuffer(strxmlbuffer,"andchild",&starg.andchild);

  GetXMLBuffer(strxmlbuffer,"matchname",starg.matchname,100);
  if ( strlen(starg.matchname)==0 ) { logfile.Write("matchname is null .\n"); return false; }
  
  GetXMLBuffer(strxmlbuffer,"srvpath",starg.srvpath,300);
  if ( strlen(starg.srvpath)==0 ) {  logfile.Write("srvpath is null .\n"); return false; }
 
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

//上传文件主函数
bool _tcpputfiles()
{
  CDir Dir;
  if(Dir.OpenDir(starg.clientpath,starg.matchname,10000,starg.andchild)==false){ logfile.Write("Dir.OpenDir(%s) failed.\n",starg.clientpath);  return false; } 
 
  int delayed=0; //未收到对端的确认报文的数量，发送+1，收到-1
  int buflen=0;

  bcontinue=false; 
 
  while(true)
  {
  
    memset(strsendbuffer,0,sizeof(strsendbuffer));
    //读取目录每一文件
    if(Dir.ReadDir()==false)  break; //用break;
    bcontinue=true; //有文件就设为true; 
   
    SNPRINTF(strsendbuffer,sizeof(strsendbuffer),1000,"<filename>%s</filename><mtime>%s</mtime><size>%ld</size>",Dir.m_FullFileName,Dir.m_ModifyTime,Dir.m_FileSize);

    //logfile.Write("strsendbuffer=%s\n",strsendbuffer);

    //发送文件名、文件时间、文件大小给对端
    if( TcpClient.Write(strsendbuffer)==false ){ logfile.Write(" TcpClient.Write() failed\n");return false; } 
     
 

    //发送内容给对端
    logfile.Write("send %s(%d) ... ",Dir.m_FullFileName,Dir.m_FileSize);
    if(SendFile(TcpClient.m_connfd,Dir.m_FullFileName,Dir.m_FileSize)==true) {logfile.WriteEx("ok\n");delayed++;}
    else {logfile.WriteEx("failed.\n");TcpClient.Close();return false;} 
     
    PActive.UptATime();

    //接收对方的确认报文,用TcpRead
    while(delayed>0)
    {
      memset(strrecvbuffer,0,sizeof(strrecvbuffer));
      if(TcpRead(TcpClient.m_connfd,strrecvbuffer,&buflen,-1)==false) break; 
      delayed--;
    
      //logfile.Write("strrecvbuffer=%s\n",strrecvbuffer);  
  
      //删除或转存本地文件
      AckMessage(strrecvbuffer); 
    }
  }
    //发送完后,继续接收对方的确认报文,用TcpRead,报文延时10s足够
    while(delayed>0)
    {
      memset(strrecvbuffer,0,sizeof(strrecvbuffer));
      if(TcpRead(TcpClient.m_connfd,strrecvbuffer,&buflen,10)==false) break; 
      delayed--;
    
      //logfile.Write("strrecvbuffer=%s\n",strrecvbuffer);  
      AckMessage(strrecvbuffer); 
  
    } 
  
    return true;
}

bool SendFile(const int connfd,const char *filename,const size_t filesize)
{
  size_t onread=0;//每次调用fread打算读取的字节数
  int bytes=0;//每次读取的字节数
  char buffer[1000];//用于存放读取到的数据
  size_t totalbytes=0;//已读取的总字节数
  FILE *fp=NULL;

  //use mode "rb" open file 文本+二进制
  if( (fp=fopen(filename,"rb"))==NULL ) return false;
  //use while get bytes(1000)
  //send to server
  while(true)
  {
    memset(buffer,0,sizeof(buffer));
    //计算本次应该读取的字节数，大于1000，取1000
    if(filesize-totalbytes) onread=1000;
    else onread=filesize-totalbytes;

    //use fread get data
    bytes=fread(buffer,1,onread,fp);
   
    //发送给对端
    if(bytes>0)
    {
      if (Writen(connfd,buffer,bytes)==false) {fclose(fp);return false;}
    }
   
    //计算已读取的字节总数
    totalbytes+=bytes;
    if(totalbytes==filesize) break;

  }
  fclose(fp);

  return true;
}

//删除或转存文件
bool AckMessage(const char *buffer)
{
  char filename[301]; memset(filename,0,sizeof(filename));
  char result[11];  memset(result,0,sizeof(result));
  GetXMLBuffer(buffer,"filename",filename,300);  
  GetXMLBuffer(buffer,"result",result,10);  
  
  //不是ok 不用做
  if(strcmp(result,"ok")!=0) return true;
  
  //ptype==1 delete
  if(starg.ptype==1)
  {
    if( REMOVE(filename)==false ) {logfile.Write("REMOVE(%s) failed\n",filename);return false;}
  }

  //ptype==2 backup
  if(starg.ptype==2)
  {
    //生成备份目录文件名
    char backupfilename[301];
    STRCPY(backupfilename,sizeof(backupfilename),filename);
    UpdateStr(backupfilename,starg.clientpath,starg.clientpathbak,false);
    if( RENAME(filename,backupfilename)==false )
   {logfile.Write("RENAME(%s,%s) failed\n",filename,backupfilename);return false;}
    

  }


  
  return true;
}





