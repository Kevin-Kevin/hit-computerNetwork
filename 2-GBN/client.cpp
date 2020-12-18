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

#define SERVER_PORT 10240
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
    printf("test : get time form server\n");
 
           sendto(socketClient, "-time", strlen("-time") + 1, 0,
                  (sockaddr *)&socketServerAddr, sizeof(sockaddr));
    while(1){
        recvfrom(socketClient, buffer, lengthOfBuffer, 0, (sockaddr *)&socketServerAddr, &sizeOfServerAddr);
        printf("--->>> server reply : %s\n", buffer);
        sleep(500);
    }
    system("pause");
    return 0;
}
 static 