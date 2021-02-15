#include <stdio.h>
#include <Windows.h>
#include <process.h>
#include <string.h>
/* 
 * 监听端口
 * 有 http 请求就记录后转发
 * 
 */
SOCKET ProxyServer;
sockaddr_in ProxyServerAddr;
const int ProxyPort = 10240;
// 初始化 socket
BOOL InitSocket(){


    WORD wVersionRequested;
    WSADATA wsaData;
    // 版本 2.2
    wVersionRequested = MAKEWORD(2, 2);
    int err;

    // 加载 socket 库
    err = WSAStartup(wVersionRequested, &wsaData);
    if(err != 0){
        printf("加载 winsock 失败，错误代码为: %d\n", WSAGetLastError());
        return FALSE;
    }
    if (LOBYTE(wsaData.wVersion) != 2 || HIBYTE(wsaData.wVersion) != 2)
    {
        printf("不能找到正确的 winsock 版本\n");
        WSACleanup();
        return FALSE;
    }
    //创建 socket
    ProxyServer = socket(AF_INET, SOCK_STREAM, 0);
    if (INVALID_SOCKET == ProxyServer)
    {
        printf("创建套接字失败，错误代码为：%d\n", WSAGetLastError());
        return FALSE;
    }

    // socket 服务器端绑定端口
    ProxyServerAddr.sin_family = AF_INET;
    ProxyServerAddr.sin_port = htons(ProxyPort);
    ProxyServerAddr.sin_addr.S_un.S_addr = INADDR_ANY;

    err = bind(ProxyServer, (SOCKADDR *)&ProxyServerAddr, sizeof(SOCKADDR));
    if (err == SOCKET_ERROR){
        printf("绑定套接字失败\n");
        return FALSE;
    }

    // 监听端口
    err = listen(ProxyServer, SOMAXCONN);
    if (err == SOCKET_ERROR){
        printf("监听端口%d 失败\n", ProxyPort);
        return FALSE;
    }
    return TRUE;

}

 

int main(){
    printf("代理服务器正在启动\n");
    printf("初始化...\n");
    if (!InitSocket())
    {
        printf("socket 初始化失败\n");
        system("pause");
        return -1;
    }
    printf("代理服务器正在运行，监听端口 %d\n", ProxyPort);

    SOCKET acceptSocket = INVALID_SOCKET;
    while(true)
    {
        acceptSocket = accept(ProxyServer, NULL, NULL);
        Sleep(200);
    
    }


    closesocket(ProxyServer);
    WSACleanup();
    system("pause");
    return 0;
}
