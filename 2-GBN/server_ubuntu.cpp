/* 
 * 基本思路
 * - 建立服务器套接字
 * - 
 */

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


#define SERVER_PORT 10240
#define SERVER_IP "0.0.0.0"

const int sendWindowSize = 10;
const int seqSize = 20;
bool ack[seqSize];
int curSeq;
int curAck;
int totalSendSeq; // 已发送的数据
int totalPacket;  // 需要发送的包总数
const int lengthOfBuffer = 1026;

int socketServer;
sockaddr_in socketServerAddr;

void clientGetTime();
void clientQuit();
void clientTestBegin();
bool InitSocket();

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
  {
    printf("socket init failed !!! \n");
  }
  else
  {
    printf("socket init successed !!! \n");
  }

  // 读取文件到内存
  char toBeSendedData[1024 * 113];

  int fd = open("./test.txt", O_RDONLY);
  int fileSize = lseek(fd, 0, SEEK_END);
  lseek(fd, 0, SEEK_SET);
  printf("file size = %d\n", fileSize);
  read(fd, toBeSendedData, fileSize);
  printf("--->file = \n%s\n--->file end\n", toBeSendedData);

  // wait for new information from client

  char buffer[lengthOfBuffer]; // 接收缓冲区
  int recvSize = 0;            // 接收缓冲区大小
  sockaddr_in addrClient;      // 客户端地址
  socklen_t sizeOfClientAddr = sizeof(addrClient);
  int ServerWaitTime = 0;
  while (1)
  { 
    // 接收消息
    ServerWaitTime++;
    printf("wait for client ...%d\n",ServerWaitTime);
    recvSize = recvfrom(socketServer, buffer, lengthOfBuffer, 0, (sockaddr *)&addrClient, &sizeOfClientAddr);
    if (recvSize < 0)
    {
      sleep(1);
      continue;
    }

    char clientIp[28];
    inet_ntoa(addrClient.sin_addr);
    printf("recv from client %s : %s\n",clientIp , buffer);
    // 回应 “-time”
    if (strcmp(buffer, "-time") == 0)
    {
      time_t t;
      t = time(NULL);
      printf("time = %s\n", ctime(&t));
      strcpy(buffer, ctime(&t));
      sendto(socketServer, buffer, strlen(buffer) + 1, 0, (sockaddr *)&addrClient, sizeof(sockaddr));
    }
    // 回应 “-quit”
    else if (strcmp(buffer, "-quit") == 0)
    {
      strcpy(buffer, "Good bye!");
      sendto(socketServer, buffer, strlen(buffer) + 1, 0, (sockaddr *)&addrClient, sizeof(sockaddr));
    }
    // 回应 "-testGBN"
    else if (strcmp(buffer, "-testGBN") == 0)
    {
      bool testRunning = true;
      int stage = 0;
      while (testRunning)
      {
        int delayCount = 0;
        switch (stage)
        {
          case 0 :
            // 告诉 client 我准备好了
            memset(buffer, 0, sizeof(buffer));
            strcpy(buffer, "come on");
            sendto(socketServer, buffer, strlen(buffer) + 1, 0, (sockaddr *)&addrClient, sizeof(sockaddr));
            stage = 1;
            break;
          case 1 :
            // 等待 client 回答
            int recvSize = -1;
            recvSize = recvfrom(socketServer, buffer, strlen(buffer) + 1, 0, (sockaddr *)&addrClient, &sizeOfClientAddr);
            // 如果 client 正确回应
            if (strcmp(buffer, "let's go") == 0)
            {
              printf("万事俱备,开车\n");
              printf("file size is %d B\n each paket is 1 KB \n packet total num is %d \n", fileSize, totalPacket);
              stage = 2;
            } else {
              delayCount++;
              if(delayCount > 2){
                printf("error : time out , client 放你鸽子了");
                testRunning = false;
                stage = 0;
              }
              sleep(1);
            }
            break;
          // case 2:
          //   break;
          
        }
      }
    }
    
    sleep(1);
  }

  system("pause");
  return 0;
}

