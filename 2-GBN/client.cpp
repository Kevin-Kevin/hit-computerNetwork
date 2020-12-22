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

const int lengthOfBuffer = 1100;
const int seqSize = 20;
int totalRecvData = 0;
char receiveData[1024 * 1024*128];
int getAck = -1;
int sendAck = -1;
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

    int waitTime = 0;
    int recvSize = 0;
    int runningTime = 0;
    sendto(socketClient, "-testGBN", strlen("-testGBN") + 1, 0, (sockaddr *)&socketServerAddr, sizeof(sockaddr));
    printf("gbn test begin\n");
    while(testRunning){

      printf("test running ...%d\n", runningTime);
      switch (stage)
      {       
      case 0:
        // 进入握手阶段
        recvfrom(socketClient, buffer, lengthOfBuffer, 0, (sockaddr *)&socketServerAddr, &sizeOfServerAddr);
        if(strcmp(buffer, "come on")==0){
          printf("--->>> server reply : come on\n");
          sendto(socketClient, "let's go", strlen("let's go") + 1, 0, (sockaddr *)&socketServerAddr, sizeof(sockaddr));
          printf("let's go\n");
          stage = 1;
        }
        break;
      // 文件传输环节
      case 1:
        printf("stage = 1\n");
        recvSize = recvfrom(socketClient, buffer, lengthOfBuffer, 0, (sockaddr *)&socketServerAddr, &sizeOfServerAddr);
        // 服务器超时未传输, 退出测试
        if(recvSize == -1){

            printf("wait server time out \n");
            testRunning = false;
            stage = 0;
        }
        else {
          if(strcmp(buffer, "-quit") == 0){
            stage = 3;
            break;
          } else {
            // 获取 ack ;
            memcpy(&getAck, (int *)(buffer + 1025), sizeof(int));
            
            printf("get ack from server ... %d\n", getAck );
            if(getAck == (sendAck + 1)){
              printf("get ack = %d, send ack = %d\n", getAck, sendAck);
              // 获取 数据帧
              char ackStr[10];
              memcpy(ackStr, &sendAck, sizeof(int));
              ackStr[5] = '\0';
              memcpy(receiveData+(1024 * totalRecvData), buffer + 1, 1024);
              totalRecvData++;
              // 发送 ack
              sendAck++;
              sendto(socketClient, ackStr, sizeof(int)+1, 0, (sockaddr *)&socketServerAddr, sizeof(sockaddr));
              printf("send %d ack \n", sendAck);
            }
          }
        }
      
        break;
      case 3:
        printf("file = \n%s\n", receiveData);
        int recvFileID = open("./recvFile.txt", O_RDWR | O_CREAT | O_TRUNC, S_IRWXO);
        write(recvFileID, receiveData, strlen(receiveData));
        stage = 0;
        testRunning = false;
        break;
      }
      runningTime++;
if(runningTime>10){
  stage = 3;
  // break;
}
    }

    system("pause");
    return 0;
}
