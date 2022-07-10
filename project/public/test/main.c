#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>
#include <signal.h> 
typedef int uint8_t;
int SCREEN_W, SCREEN_H;
int CAMERA_W, CAMERA_H;
int redoffset;
int greenoffset;
int blueoffset;
//struct fb_var_screeninfo lcdinfo;
uint8_t *fb;
uint8_t *gYUV;
//extern sqlite3 *db;
char aa = 0;

//实时监控显示
void display(uint8_t *yuv)
{
	printf("开始播放视频....\n");
	sleep(100);
}

int x, y;	  //触摸屏坐标
pid_t pid;	  //车牌识别进程的进程号
int flag = 0; //能否进行拍照识别的标志位    0是可以拍照并识别    1是不能
int flag1 = 0;
void *task(void *arg)
{
	while (1)
	{
		if ((aa = getchar()) == '1')
		{ //拍照按键按下

			if (flag == 0)
			{
				printf("正在拍照\n");
				//拍照
				printf("拍照完成\n");

				//给车牌识别的进程发送一个信号，让他进行识别
				printf("启动识别\n");
				kill(pid, SIGUSR1);
				flag = 1;
			}
			else
				printf("正在进行车牌识别，请稍后。。。\n");
		}
		else if (aa == '2')
		{
			if (flag1 == 0)
			{
				// flag1 = 1;
				printf("切换模式\n");
				kill(pid, SIGUSR2);
			}
			else
			{
				printf("切换模式\n");
				kill(pid, SIGUSR2);
			}
		}
	}
}

void fun(int sig)
{
	printf("车牌识别完成！\n");
	flag = 0;
}
void fun1(int sig)
{
	printf("模式切换完成！\n");
	if (flag1 == 0)
		{flag1 = 1; printf("出库模式\n");}
	else{flag1 = 0; printf("入库模式\n");}
}

int main(int argc, char **argv)
{
	// 0，确保合法参数
	if (argc != 2)
	{
		printf("参数错误，请指定摄像头设备文件名\n");
		exit(0);
	}

	//接受车牌识别完成信号
	signal(SIGUSR2, fun);
	signal(SIGUSR1, fun1);

	printf("打开摄像头 ok\n");

	printf("摄像头参数 ok\n");




	printf("获取lcd信息 ok\n");

	printf("启动摄像头 ok\n");

	//创建一个新的线程，不断的获取触摸屏坐标
	pthread_t tid;
	pthread_create(&tid, NULL, task, NULL);


	//创建一个进程，让该进程去执行车牌识别的程序
	pid = fork();
	if (pid == 0)
	{ //新创建的进程需要执行的

		printf("正在启动车牌识别！\n");
		execl("./alpr", "alpr", NULL);
		printf("启动车牌失败失败！\n");
		exit(0);
	}

	//  开始抓取摄像头数据并在屏幕播放视频
	printf("开始抓取摄像头数据并在屏幕播放视频。。。\n");
	int i = 0;
	while (1)
	{

		//获取摄像头的yuv数据，转换后显示到开发板屏幕
		display(gYUV);

		// i++;
	}

}
