/* 
 * 基本思路
 * - web server
 * - listen port 80
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
#include <thread>

#define SERVER_PORT 80
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

  int acceptSocket;
  sockaddr clientSocket;
  socklen_t clientSocketLength;

  char toBeSendedData[1024 * 1024];
  char buffer[lengthOfBuffer]; // 接收缓冲区
bool InitSocket();
int sendHtmlFile();

int main()
{
  // server socket init
  if (!InitSocket())
    printf("socket init failed !!! \n");
  else
    printf("socket init successed !!! \n");

  // 读取文件到内存

  int fd = open("/root/webServer/homePage.html", O_RDONLY);
  int fileSize = lseek(fd, 0, SEEK_END);
  lseek(fd, 0, SEEK_SET);
  read(fd, toBeSendedData, fileSize);
  totalPacket = (fileSize / 1024) + 1;
  printf("file size = %d B, total packet = %d KB\n", fileSize, totalPacket);
  //printf("--->file = \n%s\n--->file end\n", toBeSendedData);

  // wait for new information from client

 
  int waitTime = 0;
  int threadCount = -1;
  
  //代理服务器不断监听
  while (true)
  {
    printf("wait client ...%d\n", waitTime);
    acceptSocket = accept(socketServer, &clientSocket, &clientSocketLength);
    if(acceptSocket == -1){
      printf(" accept error \n");
      
    } else{

      int recvSize = recv(acceptSocket, buffer, sizeof(buffer), 0);
      if(recvSize<0){

      } else {
        threadCount++;
        memset(buffer, 0, sizeof(buffer));
        memcpy(buffer, toBeSendedData, strlen(toBeSendedData));
        printf("get a client connect\n");
        char header[] = {" HTTP/1.1 200 OK\nConnection: close\nContent-Type: text/html\nContent-Length: %d\n\n"};
        send(acceptSocket, header, strlen(header), 0);
        send(acceptSocket, buffer, sizeof(buffer), 0);
        sleep(1);
        close(acceptSocket);
        sleep(1);
        acceptSocket = -1;
      }
    }
    waitTime++;
    sleep(1);
  }

  close(socketServer);

  return 0;
}

// 初始化 socket
bool InitSocket()
{

  socketServer = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
  if (-1 == socketServer)
  {
    printf("创建套接字失败，错误代码为：\n");
    return false;
  }
  // //设置套接字为非阻塞模式
  // int flags = fcntl(socketServer, F_GETFL, 0);
  // fcntl(socketServer, F_SETFL, flags | O_NONBLOCK); //1：非阻塞，0：阻塞

  socketServerAddr.sin_family = AF_INET;
  socketServerAddr.sin_port = htons(SERVER_PORT); // 本地字节顺序转换至玩过字节数据(16bit)
  socketServerAddr.sin_addr.s_addr = htons(INADDR_ANY);
  if (bind(socketServer, (sockaddr *)&socketServerAddr, sizeof(sockaddr)) == -1)
  {
    printf("绑定套接字失败\n");
    return false;
  }
  if (listen(socketServer, SOMAXCONN) == -1)
  {
    printf("监听端口%d 失败", SERVER_PORT);
    return false;
  }

  return true;
}
// 发送 html 文件
int  sendHtmlFile(){
        
        return 0;
}