#include <stdio.h>
#include <string.h>   
#include <string>       // string需要的头文件
#include <vector>       // vector需要的头文件
 
int main ()
{
  char strtmp[50];                // 存放姓名的临时变量
//  std::string name;               // 存放从键盘输入的超女姓名
  std::vector<std::string> vname; // 存放超女姓名的容器
 
  while (true)
  {
    printf("请输入超女姓名（0-结束输入）：");
    scanf("%s",strtmp);               // 接受键盘输入的数据
    if (strcmp(strtmp,"0")==0) break; // 0-结束输入
    vname.push_back(strtmp);          // 把数据追加入容器
  }
 
  for (int ii=0;ii<vname.size();ii++) // 显示容器中的记录
  {
    printf("vname[%d]=%s\n",ii,vname[ii].c_str());
  }
  
}
