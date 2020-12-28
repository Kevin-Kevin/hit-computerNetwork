/* 
 * 使用线程
 * 首先创建一个主线程
 * 在固定端口监听客户端请求
 * 当从客户端收到TCP连接请求时，它将通过另一个端口建立TCP连接
 * 并在另外的单独线程中为客户端请求提供服务
 * 这样在每个请求/响应对的独立线程中将有一个独立的TCP连接
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
#include <pthread.h>

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


sockaddr clientSocket;
socklen_t clientSocketLength;

char toBeSendedData[1024 * 1024];
char buffer[lengthOfBuffer]; // 接收缓冲区

bool InitSocket();
int sendHtmlFile();
void* mainThread(void* arg);


int main()
{

  int NUM_THREADS = 100;
  // 定义线程的 id 变量，多个变量使用数组
  pthread_t tids[NUM_THREADS];
  // 创建主线程
  pthread_create(&tids[0], NULL, &mainThread, NULL);
  pthread_join(tids[0], NULL);
  
  return 0;
}

// 初始化 socket
bool InitSocket()
{

  socketServer = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
  if (-1 == socketServer)
  {
    printf("创建 套接字失败\n");
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
    printf("绑定 套接字失败\n");
    return false;
  }
  if (listen(socketServer, SOMAXCONN) == -1)
  {
    printf("监听 端口%d 失败", SERVER_PORT);
    return false;
  }

  return true;
}
// 主线程监听
void* mainThread(void* arg){
  // server socket init
  if (!InitSocket())
    printf("socket init failed !!! \n");
  else
    printf("socket init successed !!! \n");



  int waitTime = 0;
  int threadCount = -1;
  char header[] = {" HTTP/1.1 200 OK\nConnection: close\nContent-Type: text/html\nContent-Length: %d\n\n"};
  char notFound[] = {"HTTP/1.1 404 Not Found\n\n"};
  //代理服务器监听
  while (true)
  {
    printf("wait client ...%d\n", waitTime);
    int acceptSocket = accept(socketServer, &clientSocket, &clientSocketLength);
    if(acceptSocket == -1){
      printf(" accept error \n");
    } else{

      int recvSize = recv(acceptSocket, buffer, sizeof(buffer), 0);
      if(recvSize<0){

      } else {
        printf("get a client connect\n");
        // 不是 get 方法, 发送 404
        if(buffer[0] != 'G'){
          printf("client not get \n");
          send(acceptSocket, notFound, strlen(notFound), 0);
          close(acceptSocket);
          continue;
        }

        // 获取浏览器要 get 的文件
        char *flieName;
        strtok(buffer, "/");
        flieName = strtok(NULL, " ");
        // 要获取的不是 homePage.html ,发送 404
        if (strcmp(flieName, "homePage.html") != 0)
        {
          printf("do not have this file\n");
          send(acceptSocket, notFound, strlen(notFound), 0);
          close(acceptSocket);
          continue;
        }
        char readFileName[100] = {"/root/webServer/"};
        strcat(readFileName, flieName);
        printf("read file name = %s\n", readFileName);
        // 读取文件到内存
        int fd = open("/root/webServer/homePage.html", O_RDONLY);
        int fileSize = lseek(fd, 0, SEEK_END);
        lseek(fd, 0, SEEK_SET);
        read(fd, toBeSendedData, fileSize);
        totalPacket = (fileSize / 1024) + 1;
        printf("file size = %d B, total packet = %d KB\n", fileSize, totalPacket);
        // data 放入 buffer
        memset(buffer, 0, sizeof(buffer));
        memcpy(buffer, toBeSendedData, strlen(toBeSendedData));

        int res = 0;
        // 发送 header
        res = send(acceptSocket, header, strlen(header), 0);
        if(res == -1){
          printf("send header error\n");
          continue;
        }
        printf("send header \n");
        // 发送 homePage.html
        res = send(acceptSocket, buffer, sizeof(buffer), 0);
        if(res == -1){
          printf("send html error\n");
          continue;
        }
        printf("send html \n");
        close(acceptSocket);
        printf("close accept socket \n");
      }
    }
    waitTime++;
    sleep(1);
  }
  close(socketServer);
  return 0;
}

// 发送 html 文件
int  sendHtmlFile(){
        return 0;
}