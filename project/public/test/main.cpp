//#include "include/Pipeline.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <signal.h>
#include <stdio.h>

#include <fcntl.h>
#include <time.h>
#include <iostream>

using namespace std;

int fifoIn;
int fifoOut;

int flag = 0; //入库
void func1(int sig)
{	
		if(flag == 0)
		{
			flag = 1; //切换到出库模式
			cout<<"切换到出库模式"<<endl;
		}
		else
		{
			flag = 0;//切换到入库模式
			cout<<"切换到入库模式"<<endl;
		}
		kill(getppid(), SIGUSR1);
	
}

void func(int sig)
{
	cout << "正在进行车牌识别" << endl;
	
	
	//发送识别完成信号
	cout << "车牌识别完成" << endl;
	kill(getppid(), SIGUSR2);
	
	
}

int main(int argc,char **argv)
{
	
	printf("车牌识别启动\n");
	if(flag=0) printf("现在是入库模式\n");
	 else printf("现在是出库模式\n");
	//设置收到SIGUSR1信号之后去调用函数func，该函数实现车牌识别
	signal(SIGUSR1, func);
	signal(SIGUSR2, func1);
	
	


	while(1);

	return 0;
}
