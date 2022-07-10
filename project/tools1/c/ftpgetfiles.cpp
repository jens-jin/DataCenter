//#include "_public.h"
#include "_ftp.h"

struct st_arg{
  char host[31];// 远程服务器的IP和端口
  int mode;// 传输模式，1-被动模式，2-主动模式，缺省采用被动模式。
  char username[31];// 远程服务器ftp的用户名。
  char password[31];// 远程服务器ftp的密码
  char remotepath[301];// 远程服务器存放文件的目录。
  char localpath[301];//本地文件存放的目录。
  char matchname[101];//下载文件匹配的规则。
  char listfilename[301];//下载前保存从服务器返回的目录和文件名列表
  int  ptype;              // 下载后服务器文件的处理方式：1-什么也不做；2-删除；3-备份。
  char remotepathbak[301]; // 下载后服务器文件的备份目录。
  char okfilename[301];
  bool checkmtime;//是否检查时间
  int timeout;//进程心跳的超时时间
  char pname[51];//进程名，建议用"ftpgetfiles_后缀"
} starg;

struct st_fileinfo{
  char filename[301];
  char mtime[21];
};

vector<struct st_fileinfo> vlistfile1;//已下载
vector<struct st_fileinfo> vlistfile2;//下载前
vector<struct st_fileinfo> vlistfile3;//不下载
vector<struct st_fileinfo> vlistfile4;//待下载
bool LoadOKFile(); //okfile->1
bool ComVector();//比较1 2->3 4
bool WriteToOKFile();//3->okfile
bool AppendToOKFile(struct st_fileinfo *strfilename);
//加载到容器
bool LoadListFile();

CLogFile logfile;

Cftp ftp;

void EXIT(int sig);

void _help();

//解析参数函数
bool _xmltoarg(char *strxmlbuffer);

//下载文件的主函数
bool _ftpgetfiles();

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

  //登陆ftp 进入目录
  if( ftp.login(starg.host,starg.username,starg.password,starg.mode)==false )
  {
    logfile.Write("ftp.login(%s,%s,%s) failed.\n",starg.host,starg.username,starg.password); return -1;
  }  
  
 // logfile.Write("ftp.login ok.\n ");
  //下载文件
  _ftpgetfiles();
  

  //退出
  ftp.logout();

  return 0; 
}


void EXIT(int sig)
{
  printf("程序退出。收到sig=%d",sig);
  exit(0);
}

void _help()
{
  printf("\n");
  printf("USing:/project/tools1/bin/ftpgetfiles logfilename xmlbuffer\n ");
  printf("Example:/project/tools1/bin/ftpgetfiles /log/idc/ftpgetfiles_surfdata.log"\
         " \"<host>127.0.0.1:21</host><mode>1</mode><username>jens</username>"\
         "<password>123456</password><localpath>/idcdata/surfdata</localpath>"\
         "<remotepath>/tmp/idc/surfdata</remotepath><matchname>SURF_ZH*.XML,SURF_ZH*.CSV</matchname>"\
         "<listfilename>/idcdata/ftplist/ftpgetfiles_surfdata.list</listfilename>"\
         "<ptype>3</ptype><remotepathbak>/tmp/idc/surfdatabak</remotepathbak>"\
         "<okfilename>/idcdata/ftplist/ftpgetfiles_surfdata.xml</okfilename><checkmtime>true</checkmtime><timeout>80</timeout><pname>ftpgetfiles_surfdata</pname>\"\n\n\n");
  printf("Sample:/project/tools1/bin/procctl 30 /project/tools1/bin/ftpgetfiles /log/idc/ftpgetfiles_surfdata.log"\
         " \"<host>127.0.0.1:21</host><mode>1</mode><username>jens</username>"\
         "<password>123456</password><localpath>/idcdata/surfdata</localpath>"\
         "<remotepath>/tmp/idc/surfdata</remotepath><matchname>SURF_ZH*.XML,SURF_ZH*.CSV</matchname>"\
         "<listfilename>/idcdata/ftplist/ftpgetfiles_surfdata.list</listfilename>"\
         "<ptype>3</ptype><remotepathbak>/tmp/idc/surfdatabak</remotepathbak>"\
         "<okfilename>/idcdata/ftplist/ftpgetfiles_surfdata.xml</okfilename><checkmtime>true</checkmtime><timeout>80</timeout><pname>ftpgetfiles_surfdata</pname>\"\n\n\n");

  printf("本程序是通用的功能模块，用于把远程ftp服务器的文件下载到本地目录。\n");
  printf("logfilename是本程序运行的日志文件。\n");
  printf("xmlbuffer为文件下载的参数，如下：\n");
  printf("<host>127.0.0.1:21</host> 远程服务器的IP和端口。\n");
  printf("<mode>1</mode> 传输模式，1-被动模式，2-主动模式，缺省采用被动模式。\n");
  printf("<username>jens</username> 远程服务器ftp的用户名。\n");
  printf("<password>123456</password> 远程服务器ftp的密码。\n");
  printf("<remotepath>/tmp/idc/surfdata</remotepath> 远程服务器存放文件的目录。\n");
  printf("<localpath>/idcdata/surfdata</localpath> 本地文件存放的目录。\n");
  printf("<matchname>SURF_ZH*.XML,SURF_ZH*.CSV</matchname> 待下载文件匹配的规则。"\
         "不匹配的文件不会被下载，本字段尽可能设置精确，不建议用*匹配全部的文件。\n");
  printf("<listfilename>/idcdata/ftplist/ftpgetfiles_surfdata.list</listfilename>保存从服务器返回的目录和文件名列表\n");
   printf("<ptype>1</ptype> 文件下载成功后，远程服务器文件的处理方式：1-什么也不做；2-删除；3-备份，如果为3，还要指定备份的目录。\n");
  printf("<remotepathbak>/tmp/idc/surfdatabak</remotepathbak> 文件下载成功后，服务器文件的备份目录，此参数只有当ptype=3时才有效。\n");
  printf("<okfilename>/idcdata/ftplist/ftpgetfiles_surfdata.xml</okfilename> 已下载成功文件名清单，此参数只有当ptype=1时才有效。\n");
  printf("<checkmtime>true</checkmtime> 是否检查时间\n");
  printf("<timeout>80</timeout> 下载文件超时时间\n");
  printf("<pname>ftpgetfiles_surfdata</pname> 进程名\n\n\n");

}

bool _xmltoarg(char *strxmlbuffer)
{
  memset(&starg,0,sizeof(struct st_arg));
  GetXMLBuffer(strxmlbuffer,"host",starg.host,30);
  if ( strlen(starg.host)==0 )
  {
    logfile.Write("host is null .\n"); return false;
  }

  GetXMLBuffer(strxmlbuffer,"mode",&starg.mode);//传输模式 1-被动 2-主动 缺省被动。
  if ( starg.mode!=2 ) starg.mode=1;

  GetXMLBuffer(strxmlbuffer,"username",starg.username,30);
  if ( strlen(starg.username)==0 )
  {
    logfile.Write("username is null .\n"); return false;
  }

  GetXMLBuffer(strxmlbuffer,"password",starg.password,30);
  if ( strlen(starg.password)==0 )
  {
    logfile.Write("password is null .\n"); return false;
  }

  GetXMLBuffer(strxmlbuffer,"remotepath",starg.remotepath,300);
  if ( strlen(starg.remotepath)==0 )
  {
    logfile.Write("remotepath is null .\n"); return false;
  }

  GetXMLBuffer(strxmlbuffer,"localpath",starg.localpath,300);
  if ( strlen(starg.localpath)==0 )
  {
    logfile.Write("localpath is null .\n"); return false;
  }

  GetXMLBuffer(strxmlbuffer,"matchname",starg.matchname,100);
  if ( strlen(starg.matchname)==0 )
  {
    logfile.Write("matchname is null .\n"); return false;
  }
  
  GetXMLBuffer(strxmlbuffer,"listfilename",starg.listfilename,300);
  if ( strlen(starg.listfilename)==0 )
  {
    logfile.Write("listfilename is null .\n"); return false;
  }
 
  GetXMLBuffer(strxmlbuffer,"ptype",&starg.ptype);
  if ( (starg.ptype!=1) && (starg.ptype!=2) && (starg.ptype!=3) )
  { logfile.Write("ptype is error.\n"); return false; }

  GetXMLBuffer(strxmlbuffer,"remotepathbak",starg.remotepathbak,300); // 下载后服务器文件的备份目录。
  if ( (starg.ptype==3) && (strlen(starg.remotepathbak)==0) )
  { logfile.Write("remotepathbak is null.\n");  return false; }
 
  GetXMLBuffer(strxmlbuffer,"okfilename",starg.okfilename,300); // 已下载后服务器文件的清单。
  if ( (starg.ptype==1) && (strlen(starg.okfilename)==0) )
  { logfile.Write("okfilename is null.\n");  return false; }
 
  GetXMLBuffer(strxmlbuffer,"checkmtime",&starg.checkmtime);
  GetXMLBuffer(strxmlbuffer,"timeout",&starg.timeout);
  if ( starg.timeout==0 )
  { logfile.Write("timeout is null.\n");  return false; }
  GetXMLBuffer(strxmlbuffer,"pname",starg.pname,50); 
  if ( strlen(starg.pname)==0 )
  { logfile.Write("pname is null.\n");  return false; }
  return true;
}


//下载文件的主函数
bool _ftpgetfiles()
{
  // chdir nlist vector for(..)  get ok 采用两个步骤 不要目录名
  if( ftp.chdir(starg.remotepath)==false )
  {
    logfile.Write(" ftp.chdir(%s) failed.\n",starg.remotepath); return false;
  } 

  if( ftp.nlist(".",starg.listfilename)==false )//"."当前目录 
  {
    logfile.Write("ftp.nlist(%s) failed.\n",starg.remotepath);return false;
  }

  PActive.UptATime();

  //加载到容器
  if ( LoadListFile()==false  )
  {
    logfile.Write("LoadListFile() failed.\n");return false;
  } 
 
  PActive.UptATime();
  if(starg.ptype==1)
  {
    LoadOKFile(); //okfile->1
    ComVector();//比较1 2->3 4
    WriteToOKFile();//3->okfile
    vlistfile2.clear();vlistfile2.swap(vlistfile4);//4->2
  }

  PActive.UptATime();
 
  char strremotefilename[301],strlocalfilename[301];
  //下载
  for(int ii=0;ii<vlistfile2.size();ii++)
  { 
    SNPRINTF(strremotefilename,sizeof(strremotefilename),300,"%s/%s",starg.remotepath,vlistfile2[ii].filename);
    SNPRINTF(strlocalfilename,sizeof(strlocalfilename),300,"%s/%s",starg.localpath,vlistfile2[ii].filename);
    logfile.Write("get %s ...",strremotefilename);

    if ( ftp.get(strremotefilename,strlocalfilename)==false ){logfile.WriteEx("get file failed\n"); return false;}

    logfile.Write(" ok\n",strremotefilename);

  PActive.UptATime();
    //下载新增文件追加到okfilename
    if(starg.ptype==1) AppendToOKFile(&vlistfile2[ii]);

    //删除文件
    if (starg.ptype==2)
    {
      if (ftp.ftpdelete(strremotefilename)==false)
      {
        logfile.Write("ftp.ftpdelete(%s) failed.\n",strremotefilename); return false;
      }
    }

    //备份文件
    if(starg.ptype==3)
    {
      char strremotefilenamebak[301];
      SNPRINTF(strremotefilenamebak,sizeof(strremotefilenamebak),300,"%s/%s",starg.remotepathbak,vlistfile2[ii].filename);
      if (ftp.ftprename(strremotefilename,strremotefilenamebak)==false)
      {
        logfile.Write("ftp.ftprename(%s,%s) failed.\n",strremotefilename,strremotefilenamebak); return false;
      }    

    }
  }
  return true;
}

bool LoadOKFile() //okfile->1
{
  vlistfile1.clear();
  CFile File;

  if( File.Open(starg.okfilename,"r")==false) return true;//第一次下载 okfilename不存在
  
  char strbuffer[501];
  struct st_fileinfo stfileinfo;//放入容器先放结构体

  while(true)
  {
    memset(&stfileinfo,0,sizeof( struct st_fileinfo));
    if( File.Fgets(strbuffer,300,true)==false) break;
   // {logfile.Write("File.Fgets(%s) failed \n",stfileinfo.filename); break;}//最后一行一定会faile
    //如果不匹配 不放入容器a
    GetXMLBuffer(strbuffer,"filename",stfileinfo.filename);
    GetXMLBuffer(strbuffer,"mtime",stfileinfo.mtime);
    vlistfile1.push_back(stfileinfo);
  }     
  return true;
}

bool ComVector()//比较1 2->3 4
{
  vlistfile3.clear();  vlistfile4.clear();
  int ii,jj;
  for(ii=0;ii<vlistfile2.size();ii++)
  { 
    for(jj=0;jj<vlistfile1.size();jj++)
    {
      //比较文件名+时间
      if( strcmp(vlistfile2[ii].filename,vlistfile1[jj].filename)==0 &&
          strcmp(vlistfile2[ii].mtime,vlistfile1[jj].mtime)==0 )
      {
        vlistfile3.push_back(vlistfile2[ii]);break;
      }
    }
    if(jj==vlistfile1.size()) vlistfile4.push_back(vlistfile2[ii]); 
  }
  return true;
}

bool WriteToOKFile()//3->okfile
{
  CFile File;
  if( File.Open(starg.okfilename,"w")==false)
  {
    logfile.Write("File.Open(%s) failed.\n",starg.okfilename);return false; 
  }
  for(int ii=0;ii<vlistfile3.size();ii++)
  {
    File.Fprintf("<filename>%s</filename><mtime>%s</mtime>\n",vlistfile3[ii].filename,vlistfile3[ii].mtime);
  }
  return true;
}


bool AppendToOKFile(struct st_fileinfo *strfilename)
{
  CFile File;
  if( File.Open(starg.okfilename,"a")==false)
  { 
    logfile.Write("File.Open(%s) failed.\n",starg.okfilename);return false;
  }
  File.Fprintf("<filename>%s</filename><mtime>%s</mtime>\n",strfilename->filename,strfilename->mtime);

  return true;
}


//加载到容器
bool LoadListFile()
{
  vlistfile2.clear();//容器清空
  
  CFile File;
  if (File.Open(starg.listfilename,"r") ==false)
  {
    logfile.Write("File.Open(%s) failed.\n",starg.listfilename);return false;
  } 
 
  struct st_fileinfo stfileinfo;//放入容器先放结构体
  
  while(true)
  {  
    memset(&stfileinfo,0,sizeof( struct st_fileinfo));
    if( File.Fgets(stfileinfo.filename,300,true)==false) break;
   // {logfile.Write("File.Fgets(%s) failed \n",stfileinfo.filename); break;}
    //如果不匹配 不放入容器
    if( MatchStr(stfileinfo.filename,starg.matchname)==false )continue;
    if(starg.ptype==1&&starg.checkmtime==true)
      if (ftp.mtime(stfileinfo.filename)==false )
         {
           logfile.Write("ftp.mtim(%s) failed.\n",stfileinfo.filename);return false;
         } 
      strcpy(stfileinfo.mtime,ftp.m_mtime);
    vlistfile2.push_back(stfileinfo);
  }
  
  return true;
}
