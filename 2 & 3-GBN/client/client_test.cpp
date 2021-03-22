/* 
 * 基本思路
 * - 建立服务器套接字
 * - 
 */

#include<stdlib.h>
#include<stdio.h>
#include<iostream>
#include <time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <fstream>
#include <netinet/udp.h>
#include <netinet/in.h>
#include <fcntl.h>
#include <unistd.h>
#include <arpa/inet.h>

#define SERVER_PORT 12340
#define SERVER_IP "127.0.0.1"


int socketClient;
sockaddr_in socketServerAddr;
socklen_t sizeOfServerAddr = sizeof(socketServerAddr);

bool InitSocket();

bool InitSocket()
{
   
    socketClient = socket(AF_INET, SOCK_DGRAM, 0);
    if (-1 == socketClient)
    {
        printf("创建套接字失败，错误代码为：\n");
        return false;
    }
/*     //设置套接字为非阻塞模式
    int flags = fcntl(socketClient, F_GETFL, 0);
    fcntl(socketClient, F_SETFL, flags | O_NONBLOCK); //1：非阻塞，0：阻塞 */
    socketServerAddr.sin_family = AF_INET;
    socketServerAddr.sin_port = htons(SERVER_PORT); // 本地字节顺序转换至玩过字节数据(16bit)
    socketServerAddr.sin_addr.s_addr = inet_addr(SERVER_IP);

    return true;
}

int main()
{
    
    // server socket init
    if (!InitSocket())
    {
        printf("client socket init failed !!! \n");
    }
    else
    {
        printf("client socket init successed !!! \n");
    }

  char buffer[10];
  int ack = 0;
  int charAck = 0;
  buffer[0] = 'a';
  for (int i = 0; i < 10; i++){

    memcpy(buffer+1, &ack, 4);
    buffer[5] = 'a';
    buffer[6] = '\0';
    memcpy(&charAck, buffer+1, 4);
    
    printf("char ack = %s\n", buffer);
    sendto(socketClient, buffer, sizeof(buffer), 0, (sockaddr *)&socketServerAddr, sizeof(sockaddr));
    printf("send ack = %d\n", ack);
    ack++;
    usleep(1000);
  }

  // char wait;
  // std::cin >> wait;

  return 0;

}