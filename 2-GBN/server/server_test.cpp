
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <fstream>

#include <netinet/udp.h>
#include <netinet/in.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <arpa/inet.h>

#define SERVER_PORT 12340
#define SERVER_IP "0.0.0.0"

int socketServer;
sockaddr_in socketServerAddr;
sockaddr_in clientAddr;
socklen_t lenOfClientAddr;
socklen_t lenOfServerAddr;
// 初始化 socket
bool InitSocket()
{

  socketServer = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
  if (-1 == socketServer)
  {
    printf("创建套接字失败，错误代码为：\n");
    return false;
  }
  //设置套接字为非阻塞模式
  int flags = fcntl(socketServer, F_GETFL, 0);
  fcntl(socketServer, F_SETFL, flags | O_NONBLOCK); //1：非阻塞，0：阻塞

  socketServerAddr.sin_family = AF_INET;
  socketServerAddr.sin_port = htons(SERVER_PORT); // 本地字节顺序转换至玩过字节数据(16bit)
  socketServerAddr.sin_addr.s_addr = htons(INADDR_ANY);
  if (bind(socketServer, (sockaddr *)&socketServerAddr, sizeof(sockaddr)) == -1)
  {
    printf("绑定套接字失败\n");
    return false;
  }

  return true;
}

int main()
{
  // server socket init
  if (!InitSocket())
    printf("socket init failed !!! \n");
  else
    printf("socket init successed !!! \n");

  int waitTime = 0;
  char buffer[10];
  int ack = 0;
  int recvSize = -1;
  while (1)
  {
    //printf("wait client ...%d\n", waitTime);
    recvSize = recvfrom(socketServer, buffer, sizeof(buffer), 0, (sockaddr *)&clientAddr, &lenOfClientAddr);
    if (recvSize < 0)
    {

    }
    else
    {
      memcpy(&ack,(buffer+1), 4);
      printf("client ack = %d\n", ack);
      printf("client buffer = %s\n", buffer);
    }
    waitTime++;
    usleep(1000);
  }
  return 0;
}