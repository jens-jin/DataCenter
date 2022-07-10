/*
 * 程序名：srtsurfdata2.cpp 生成观测的分钟数据
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

CLogFile logfile;//日志类

int main(int argc,char *argv[]){
  //inifile outpath logfile
  if(argc!=4)
  {
     printf("Using:./crtsurfdata2 inifile outpath logfile\n");
     printf("Example:/project/idc1/bin/crtsurfdata2 /project/idc1/ini/stcode.ini /tmp/surfdata /log/idc/crtsurfdata2.log\n");
     printf("inifile 气象站点参数文件名。\n");
     printf("outpath 气象站点数据文件存放目录。\n");
     printf("logfile 程序的日志文件。\n\n");
   
  
    return -1;
  }

  if(logfile.Open(argv[3],"a+",false)==false)
  {
    printf("logfile.Open(%s) failed.\n",argv[3]);return -1;
  }
  
  logfile.Write("srtsurfdata2 start running.\n");
  
  //插入业务代码
  /*for (int ii=0;ii<10000000;ii++)
    {
    logfile.Write("这是第%d条记录。\n",ii);
    }*/
 
  //把站点参数文件加载到容器里
  if(LoadSTCode(argv[1])==false)
  {
    return -1; 
  } 

  logfile.WriteEx("srtsurfdata2 end running.\n");
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

  struct st_stcode stcode;  

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
   
    //放入容器
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
