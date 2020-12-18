/* 
 * 基本思路
 * - 建立服务器套接字
 * - 
 */


#include<stdlib.h>
#include<stdio.h>
#include <time.h>
#include <sys/types.h>
#include<sys/socket.h>
#include <fstream>
#include <netinet/udp.h>
#include <netinet/in.h>
#include <fcntl.h>
#include <unistd.h>
#define SERVER_PORT 12340
#define SERVER_IP "0.0.0.0"

const int lengthOfBuffer = 1026;

int socketClient;
sockaddr_in socketServerAddr;


void clientGetTime();
void clientQuit();
void clientTestBegin();
bool InitSocket();

bool InitSocket()
{

    socketClient = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if ( -1 == socketClient)
    {
        printf("创建套接字失败，错误代码为：\n") ;
        return false;
    }
    //设置套接字为非阻塞模式
    int flags = fcntl(socketClient, F_GETFL, 0);
    fcntl(socketClient, F_SETFL, flags | O_NONBLOCK);         //1：非阻塞，0：阻塞
 
    socketServerAddr.sin_family = AF_INET;
    socketServerAddr.sin_port = htons(SERVER_PORT); // 本地字节顺序转换至玩过字节数据(16bit)
    socketServerAddr.sin_addr.s_addr = htons(INADDR_ANY);
    if (bind(socketClient, (sockaddr *)&socketServerAddr, sizeof(sockaddr)) == -1)
    {
        printf("绑定套接字失败\n");
        return false;
    }

    return true;
}

int main(){
    // server socket init
    if( !InitSocket() ){
        printf("socket init failed !!! \n");
    } else {
        printf("socket init successed !!! \n");
    }
    // wait for new information from client
    
    char buffer[lengthOfBuffer];    // 接收缓冲区
    int recvSize = 0;               // 接收缓冲区大小
    sockaddr_in addrClient;         // 客户端地址
    socklen_t sizeOfClientAddr = sizeof(addrClient);
    while(1){
        recvSize = recvfrom(socketClient, buffer, lengthOfBuffer, 0, (sockaddr *)&addrClient, &sizeOfClientAddr);
        if(recvSize<0){
            (200);
            continue;
        }
        printf("recv from client %s : %s\n", addrClient.sin_addr.s_addr, buffer);
        if (strcmp(buffer, "-time") == 0)
        {
            time_t t;
            t = time(NULL);
            printf("time = %s\n", ctime(&t));
            strcpy(buffer, ctime(&t));
        }
        else if (strcmp(buffer, "-quit") == 0)
        {
            strcpy(buffer, "Good bye!");
        }
        sendto(socketClient, buffer, strlen(buffer) + 1, 0, (sockaddr *)&addrClient,
               sizeof(sockaddr));
        sleep(500);
    }

    system("pause");
    return 0;
}

