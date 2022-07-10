#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>

int main(int argc,char *argv[])
{
  int a=2;
  char *c=(char*)&a;
  printf("*c=%d\n",*c);
  *c=1;
  printf("a=%d\n",a);
  printf("c=%d\n",c);
  printf("&a=%d\n",&a);
  a=0;
  c+=3;
  *c=1;
//00000001 00000000:x
  printf("a=%d\n",a);
  printf("c=%d\n",c);
  printf("&a=%d\n",&a);

  return 0;

  char text[100];
  strcpy(text,"abcd");

  int ilen= strlen(text);
  char buffer[10];
  memset(buffer,0,sizeof(buffer));
  memcpy(buffer,&ilen,4);
  memcpy(buffer+4,text,ilen);
  printf("%s\n",buffer+4);  
  printf("ilen=%s\n",&ilen);
  return 0;
}
