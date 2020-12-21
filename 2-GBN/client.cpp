/* 
 * 基本思路
 * - 建立服务器套接字
 * - 
 */

#include<stdlib.h>
#include<stdio.h>
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
#define SERVER_IP "59.110.61.243"

const int lengthOfBuffer = 1026;
int nihao;
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

    // sent new information to server
    char buffer[lengthOfBuffer]; // 接收缓冲区
    
    // 获取时间
    printf("test : get time form server\n");
    sendto(socketClient, "-time", strlen("-time") + 1, 0, (sockaddr *)&socketServerAddr, sizeof(sockaddr));
    if( -1 == recvfrom(socketClient, buffer, lengthOfBuffer, 0, (sockaddr *)&socketServerAddr, &sizeOfServerAddr) ){
      printf("get time error\n");
    } else {
      printf("--->>> server reply : %s\n", buffer);
    }

    // test gbn protocol
    bool testRunning = true;
    int stage = 0;
    sendto(socketClient, "-testGBN", strlen("-testGBN") + 1, 0, (sockaddr *)&socketServerAddr, sizeof(sockaddr));
    printf("gbn test begin\n");
    while(testRunning){
      switch (stage)
      {       
      case 0:
        recvfrom(socketClient, buffer, lengthOfBuffer, 0, (sockaddr *)&socketServerAddr, &sizeOfServerAddr);
        if(strcmp(buffer, "come on")==0){
          printf("--->>> server reply : come on\n");
          sendto(socketClient, "let's go", strlen("let's go") + 1, 0, (sockaddr *)&socketServerAddr, sizeof(sockaddr));
          stage = 1;
        }
        break;
      case 1:


        break;
      default:
        break;
      }
    }

    system("pause");
    return 0;
}
