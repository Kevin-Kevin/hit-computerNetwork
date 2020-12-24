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

#define SERVER_PORT 12340
#define SERVER_IP "0.0.0.0"

const int sendWindowSize = 10;
const int seqSize = 20;
bool ack[seqSize];
int nextSeq = 0;
int curAck = 0;
int totalSendSeq = 0; // 已发送的数据
int totalPacket;      // 需要发送的包总数
const int lengthOfBuffer = 1100;

int socketServer;
sockaddr_in socketServerAddr;

void clientGetTime();
void clientQuit();
void clientTestBegin();
bool InitSocket();
bool isSequenceAvailable(int nextSeq);

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

// 查看 curseq 是否处于窗口内
bool isSequenceAvailable(int nextSeq)
{
  if ((nextSeq - curAck) > sendWindowSize)
  {
    return false;
  }
  else
  {
    return true;
  }
}
int main()
{
  // server socket init
  if (!InitSocket())
    printf("socket init failed !!! \n");
  else
    printf("socket init successed !!! \n");

  // 读取文件到内存
  char toBeSendedData[1024 * 1024];

  int fd = open("./test.txt", O_RDONLY);
  int fileSize = lseek(fd, 0, SEEK_END);
  lseek(fd, 0, SEEK_SET);
  read(fd, toBeSendedData, fileSize);
  totalPacket = (fileSize / 1024) + 1;
  printf("file size = %d B, total packet = %d KB\n", fileSize, totalPacket);
  //printf("--->file = \n%s\n--->file end\n", toBeSendedData);

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
    printf("wait for client ...%d\n", ServerWaitTime);
    recvSize = recvfrom(socketServer, buffer, lengthOfBuffer, 0, (sockaddr *)&addrClient, &sizeOfClientAddr);
    if (recvSize < 0)
    {
      sleep(1);
      continue;
    }

    char clientIp[28];
    inet_ntoa(addrClient.sin_addr);
    printf("recv from client %s : %s\n", clientIp, buffer);
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
      int delayCount = 0;
      int waitTime = 0;
      int retransimtCount = 0;
      int recvSize = -1;
      while (testRunning)
      {
        switch (stage)
        {
        case 0:
          // 告诉 client 我准备好了

          sendto(socketServer, "come on", strlen("come on") + 1, 0, (sockaddr *)&addrClient, sizeof(sockaddr));
          printf("come on\n");
          stage = 1;
          break;
        case 1:
          // 等待 client 回答
          recvSize = recvfrom(socketServer, buffer, strlen(buffer) + 1, 0, (sockaddr *)&addrClient, &sizeOfClientAddr);
          // 如果 client 正确回应
          if (strcmp(buffer, "let's go") == 0)
          {
            printf("万事俱备,开车\n");
            printf("file size is %d B\n each packet is 1 KB \n packet total num is %d \n", fileSize, totalPacket);
            stage = 2;
            printf("stage = 2\n");
            break;
          }
          else
          {
            delayCount++;
            printf("delay = %d, buffer = %s\n", delayCount, buffer);
            if (delayCount > 2)
            {
              delayCount = 0;
              printf("error : time out , client 放你鸽子了\n");
              testRunning = false;
              stage = 0;
            }
            sleep(1);
          }
          break;

        // 进入发送数据帧环节
        case 2:
          
          // 只要 curseq 还在窗口内,就一直发送数据
          if ((nextSeq - totalSendSeq) <= sendWindowSize && nextSeq <= totalPacket)
          {
            printf("nextseq = %d\n", nextSeq);
            memset(buffer, 0, sizeof(buffer));
            // buffer 第一位放 1 ,因为放 0 UDP协议不传输
            buffer[0] = 1;
            // buffer 1-1025 位放数据
            memcpy(buffer + 1, toBeSendedData + (nextSeq * 1024), 1024);
            // buffer 后四位放 next sequence
            memcpy(buffer + 1025, (void *)&nextSeq, sizeof(int));
            // 发送给 client
            sendto(socketServer, buffer, sizeof(buffer), 0, (sockaddr *)&addrClient, sizeof(sockaddr));
            printf("server send %d seq \n", nextSeq);
           // printf("send frame = %s\n",buffer);
            nextSeq++;
          }
          // 从 client 接收 ack
          char ackStr[10];

          recvSize = recvfrom(socketServer, ackStr, sizeof(ackStr), 0, (sockaddr *)&addrClient, &sizeOfClientAddr);
          // 如果未收到 client 的 ack
          if (recvSize < 0)
          {
            waitTime++;
            // 如果超时未收到ack,重传
            if (waitTime > 1000)
            {
              printf("time out , retransmit\n");
              nextSeq = totalSendSeq + 1;
              waitTime = 0;
              retransimtCount++;
              printf("retransmit is %d\n",retransimtCount);
                // if(retransimtCount>300){
                //   printf("retransmit is over 300, go to stage 3\n");
                //   stage = 3;
                //   break;
                // }
              
            }
          }
          // 如果收到 ack
          else
          {
            memcpy(&curAck, ackStr+1, 4);
            printf("--->>>client send %d ack\n",curAck);
            totalSendSeq = curAck;
          }
          // 如果 ack 等于总的文件帧,进入结束阶段
          if (curAck == totalPacket)
          {
            stage = 3;
            printf("transmision is going to end...\n");
          }
          usleep(1000);
          break;

        case 3:

          sendto(socketServer, "-quit", strlen("-quit") + 1, 0, (sockaddr *)&addrClient, sizeof(sockaddr));
          
          stage = 0;
          testRunning = false;
          printf("retransmit = %d\n", retransimtCount);
          printf("---------------transmit end--------------\n");
          break;
        }
      }
    }
    nextSeq = 0;
    curAck = 0;
    totalSendSeq = 0; // 已发送的数据
    usleep(1000);

  }

  system("pause");
  return 0;
}
