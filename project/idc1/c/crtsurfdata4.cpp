/*
 * 程序名：srtsurfdata4.cpp 生成观测的分钟数据
 * 作者:jens
 */
 
#include "_public.h"

//站点参数结构体
struct st_stcode{
  char provname[31];//省
  char obtid[11];//站号
  char obtname[31];//站名
  double lat;//经度
  double lon;//纬度
  double height;//海拔高度
};

//定义存放站点参数的容器
vector<struct st_stcode> vstcode;

//把站点参数文件加载到容器里
bool LoadSTCode(const char *inifile);

//站点分钟观测数据结构
struct st_surfdata
{
  char obtid[11];      // 站点代码。
  char ddatetime[21];  // 数据时间：格式yyyymmddhh24miss
  int  t;              // 气温：单位，0.1摄氏度。
  int  p;              // 气压：0.1百帕。
  int  u;              // 相对湿度，0-100之间的值。
  int  wd;             // 风向，0-360之间的值。
  int  wf;             // 风速：单位0.1m/s
  int  r;              // 降雨量：0.1mm。
  int  vis;            // 能见度：0.1米。
};

vector<struct st_surfdata> vsurfdata; // 存放全国气象站点分钟观测数据的容器

// 模拟生成全国气象站点分钟观测数据，存放在vsurfdata容器中。
void CrtSurfData();

//把观测数据写入csv文件(生成并写入)
bool CrtSurfFile(const char *outpath,const char *datafmt);


char strddatetime[21]; //观测时间

CLogFile logfile;//日志类

int main(int argc,char *argv[]){
  //inifile outpath logfile
  if(argc!=5)
  {
     printf("Using:./crtsurfdata4 inifile outpath logfile datafmt\n");
     printf("Example:/project/idc1/bin/crtsurfdata4 /project/idc1/ini/stcode.ini /tmp/surfdata /log/idc/crtsurfdata4.log xml,json,csv\n");
     printf("inifile 气象站点参数文件名。\n");
     printf("outpath 气象站点数据文件存放目录。\n");
     printf("logfile 程序的日志文件。\n");
     printf("datafmt 生成文件的格式(xml、json、csv)。\n\n");
   
  
    return -1;
  }

  if(logfile.Open(argv[3],"a+",false)==false)
  {
    printf("logfile.Open(%s) failed.\n",argv[3]);return -1;
  }
  
  logfile.Write("srtsurfdata4 start running.\n");
  
  //插入业务代码
  /*
    for (int ii=0;ii<10000000;ii++)
    {
    logfile.Write("这是第%d条记录。\n",ii);
    }
  */
 
  //把站点参数文件加载到容器里
  if(LoadSTCode(argv[1])==false)
  {
    return -1; 
  }
 
  // 模拟生成全国气象站点分钟观测数据，存放在vsurfdata容器中
  CrtSurfData();

  //把观测数据写入csv文件(生成并写入)
  if((strstr(argv[4],"xml")!=0)) CrtSurfFile(argv[2],"xml");
  if((strstr(argv[4],"json")!=0)) CrtSurfFile(argv[2],"json");
  if((strstr(argv[4],"csv")!=0)) CrtSurfFile(argv[2],"csv");
    


  logfile.WriteEx("srtsurfdata4 end running.\n");

  return 0;
}


//把站点参数文件加载到容器里
bool LoadSTCode(const char *inifile){
  CFile File;

  //打开站点参数文件
  if(File.Open(inifile,"r")==false)
  {
  logfile.Write("File.Open(%s) failed.\n",inifile);return false;
  }

  char strBuffer[301];

  CCmdStr CmdStr;

  struct st_stcode stcode; //定义结构体放入容器 

  while(true)
  {
    //读一行，完跳出循环
    // memset(strBuffer,0,sizeof(strBuffer));调用Fget会初始化
    if(File.Fgets(strBuffer,300,true)==false) break;
    /*{
      printf("=%s=File.Fgets(..) failed,会有这个提示是因为到了最后1行(size=300)\n",strBuffer); break;
    }*/
    logfile.Write("=%s=\n",strBuffer);
    
    //行拆分
    CmdStr.SplitToCmd(strBuffer,",",true);
    if(CmdStr.CmdCount()!=6) continue;//扔掉无效的行 
    //数据项存到结构体
    CmdStr.GetValue(0, stcode.provname,30);
    CmdStr.GetValue(1, stcode.obtid,10);
    CmdStr.GetValue(2, stcode.obtname,30);
    CmdStr.GetValue(3,&stcode.lat);
    CmdStr.GetValue(4,&stcode.lon);
    CmdStr.GetValue(5,&stcode.height);
   
    //结构体放入容器
    vstcode.push_back(stcode);
    
    
    
    //关闭文件 析构函数自动关闭
   
  }
  //输出到log
  for(int ii=0;ii<vstcode.size();ii++)
  {
    logfile.Write("provname=%s,obtid=%s,obtname=%s,lat=%.2f,lon=%.2f,height=%.2f\n",\
                   vstcode[ii].provname,vstcode[ii].obtid,vstcode[ii].obtname,vstcode[ii].lat,\
                   vstcode[ii].lon,vstcode[ii].height);
  
  }
  return true;
}

// 模拟生成全国气象站点分钟观测数据，存放在vsurfdata容器中。
void CrtSurfData()
{
  //播下随机种子
  srand(time(0));//当前时间

  //获取当前时间作为观测时间
  memset(strddatetime,0,sizeof(strddatetime));
  LocalTime(strddatetime,"yyyymmddhh24miss");//获取操作系统时间，格式为yyyymmddhh24miss。年月日时分秒。
 
  struct st_surfdata stsurfdata;
  
  //遍历站点容器
  for(int ii=0;ii<vstcode.size();ii++)
  {
    memset(&stsurfdata,0,sizeof(struct st_surfdata));
    strncpy(stsurfdata.obtid,vstcode[ii].obtid,10);      // 站点代码。
    strncpy(stsurfdata.ddatetime,strddatetime,20);  // 数据时间：格式yyyymmddhh24miss
    stsurfdata.t=rand()%351;       // 气温：单位，0.1摄氏度
    stsurfdata.p=rand()%265+10000; // 气压：0.1百帕
    stsurfdata.u=rand()%100+1;     // 相对湿度，0-100之间的值。
    stsurfdata.wd=rand()%360;      // 风向，0-360之间的值。
    stsurfdata.wf=rand()%150;      // 风速：单位0.1m/s
    stsurfdata.r=rand()%16;        // 降雨量：0.1mm
    stsurfdata.vis=rand()%5001+100000;  // 能见度：0.1米}

   //放入容器
   vsurfdata.push_back(stsurfdata);
  }
}

//把观测数据写入csv文件(生成并写入)
bool CrtSurfFile(const char *outpath,const char *datafmt)
{
  CFile File;

  //打开文件 写入 关闭 
  //生成文件名 例如：/tmp/idc/surfdata/SURF_ZH_20210629092200_2254.csv
  char strFileName[301];
  sprintf(strFileName,"%s/SURF_ZH_%s_%d.%s",outpath,strddatetime,getpid(),datafmt);

  //打开文件 用临时文件
  if(File.OpenForRename(strFileName,"w")==false)
  {
    logfile.Write("File.OpenForRename(%s) failed.\n",strFileName);return false;
  }

  //写入标题  if  csv
  if(strcmp(datafmt,"csv")==0) File.Fprintf("站点代码,数据时间,气温,气压,相对湿度,风向,风速,降雨量,能见度\n");

  //容器写入文件
  for(int ii=0;ii<vsurfdata.size();ii++)
  {
    File.Fprintf("%s,%s,%.1f,%.1f,%d,%d,%.1f,%.1f,%.1f\n",\
        vsurfdata[ii].obtid,vsurfdata[ii].ddatetime,vsurfdata[ii].t/10.0,vsurfdata[ii].p/10.0,\
        vsurfdata[ii].u,vsurfdata[ii].wd,vsurfdata[ii].wf/10.0,vsurfdata[ii].r/10.0,vsurfdata[ii].vis/10.0);   
  } 
  //关闭文件
  File.CloseAndRename();
  
  logfile.Write("生成数据文件%s成功,数据时间%s,记录数%d\n",strFileName,strddatetime,vsurfdata.size());
  
  return true;
}

