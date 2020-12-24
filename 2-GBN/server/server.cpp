/* 
 * 基本思路
 * - 建立服务器套接字
 * - 
 */


#include<stdlib.h>
#include<stdio.h>
#include <time.h>
#include <WinSock2.h>
#include <fstream>

#define SERVER_PORT 12340
#define SERVER_IP "0.0.0.0"

const int lengthOfBuffer = 1026;

SOCKET socketClient;
SOCKADDR_IN socketServerAddr;


void clientGetTime();
void clientQuit();
void clientTestBegin();
BOOL InitSocket();

BOOL InitSocket()
{
    //加载套接字库（必须）
    WORD wVersionRequested;
    WSADATA wsaData;
    //套接字加载时错误提示
    int err;
    //版本 2.2
    wVersionRequested = MAKEWORD(2, 2);
    //加载 dll 文件 Scoket 库
    err = WSAStartup(wVersionRequested, &wsaData);
    if (err != 0)
    {
        //找不到 winsock.dll
        printf("加载 winsock 失败，错误代码为: %d\n", WSAGetLastError());
        return FALSE;
    }
    if (LOBYTE(wsaData.wVersion) != 2 || HIBYTE(wsaData.wVersion) != 2)
    {
        printf("不能找到正确的 winsock 版本\n");
        WSACleanup();
        return FALSE;
    }
    socketClient = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    //设置套接字为非阻塞模式
    int iMode = 1;                                          //1：非阻塞，0：阻塞
    ioctlsocket(socketClient, FIONBIO, (u_long FAR *)&iMode); //非阻塞设置

    if (INVALID_SOCKET == socketClient)
    {
        printf("创建套接字失败，错误代码为：%d\n", WSAGetLastError());
        return FALSE;
    }
    socketServerAddr.sin_family = AF_INET;
    socketServerAddr.sin_port = htons(SERVER_PORT); // 本地字节顺序转换至玩过字节数据(16bit)
    socketServerAddr.sin_addr.S_un.S_addr = htons(INADDR_ANY);
    if (bind(socketClient, (SOCKADDR *)&socketServerAddr, sizeof(SOCKADDR)) == SOCKET_ERROR)
    {
        printf("绑定套接字失败\n");
        return FALSE;
    }

    return TRUE;
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
    SOCKADDR_IN addrClient;         // 客户端地址
    int sizeOfClientAddr = sizeof(addrClient);
    while(1){
        recvSize = recvfrom(socketClient, buffer, lengthOfBuffer, 0, (SOCKADDR *)&addrClient, &sizeOfClientAddr);
        if(recvSize<0){
            Sleep(200);
            continue;
        }
        printf("recv from client: %s\n", buffer);
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
        sendto(socketClient, buffer, strlen(buffer) + 1, 0, (SOCKADDR *)&addrClient,
               sizeof(SOCKADDR));
        Sleep(500);
    }

    system("pause");
    return 0;
}

