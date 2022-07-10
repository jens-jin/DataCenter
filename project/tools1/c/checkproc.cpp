#include "_public.h"

CLogFile logfile;


int main(int argc,char * argv[])
{
  //程序的帮助
  if(argc!=2)
  {
    printf("\n");
    printf("Using:./checkproc logfilename\n");

    printf("Example:/project/tools1/bin/procctl 10 /project/tools1/bin/checkproc /tmp/log/checkproc.log\n\n");

    printf("本程序用于检查后台服务程序是否超时，如果已超时，就终止它。\n");
    printf("注意：\n");
    printf("  1）本程序由procctl启动，运行周期建议为10秒。\n");
    printf("  2）为了避免被普通用户误杀，本程序应该用root用户启动。\n");
    printf("  3）如果要停止本程序，只能用killall -9 终止。\n\n\n");
    return -1;   
  }

    // 忽略全部的信号和IO，不希望程序被干扰。
    CloseIOAndSignal(true);
    
  //打开日志文件
  if( logfile.Open(argv[1],"a+") == false ){ printf("logfile.Open(%s) failed.\n",argv[1]); return -1; }  

  int shmid=0;
  //获取共享内存
  if( (shmid=shmget((key_t)SHMKEYP, MAXNUMP*sizeof(struct st_procinfo), 0666|IPC_CREAT) )==-1 )
  {
   logfile.Write("创建/获取共享内存(%x)失败。\n",SHMKEYP); return -1; 
  } 
  

  //连接共享内存到当前地址空间
  struct st_procinfo *shm = (struct st_procinfo *)shmat(shmid,0,0);

  /*
  struct st_procinfo
  {
    int    pid;         // 进程id。
    char   pname[51];   // 进程名称，可以为空。
    int    timeout;     // 超时时间，单位：秒。
    time_t atime;       // 最后一次心跳的时间，用整数表示。
  };
  */
  //遍历共享内存全部记录
  
  for(int ii=0;ii<MAXNUMP;ii++)
  {
    //pid=0,表示空记录
    if( shm[ii].pid==0 ) continue;
    
    //pid!=0,表示是服务程序的心跳记录

    // logfile.Write("pid=%d,pname=%s,timeout=%d,atime=%d\n",shm[ii].pid,shm[ii].pname,shm[ii].timeout,shm[ii].atime);
     // logfile.Write("ii=%d,pid=%d\n",ii,shm[ii].pid); 
  
    //发送信号0 判断是否存在 
    int iret=kill(shm[ii].pid,0);
    if(iret==-1)
    {
      logfile.Write("进程pid=%d(%s)不存在. \n",shm[ii].pid,shm[ii].pname);
      memset(shm+ii,0,sizeof(struct st_procinfo));//从共享内存中删除
      continue;
    }

    //判断超时
    time_t now=time(0);
    //未超时
    if( (now-shm[ii].atime)<shm[ii].timeout  ) continue;
    
    //超时 终止 15 9
    logfile.Write("进程pid=%d(%s)已经超时。\n",(shm+ii)->pid,(shm+ii)->pname);
    
    kill(shm[ii].pid,15);
    
    //服务程序退出需要时间。5s检测
    for(int jj=0;jj<5;jj++)
    {
      sleep(1);
      iret=kill(shm[ii].pid,0);
      if( iret==-1 ) break;//进程已退出
    }
      
    //仍存在 kill 9
    if (iret==-1)
    logfile.Write("进程pid=%d(%s)已经正常终止。\n",(shm+ii)->pid,(shm+ii)->pname);
    else
    {
      kill(shm[ii].pid,9);  // 如果进程仍存在，就发送信号9，强制终止它。
      logfile.Write("进程pid=%d(%s)已经强制终止。\n",(shm+ii)->pid,(shm+ii)->pname);
    }
    //从共享内存中删掉心跳记录
    memset(shm+ii,0,sizeof(struct st_procinfo));
  } 
 
  //分离m 与当前进程
  shmdt(shm);
  return 0;
}
