#include <stdio.h>
#include <stdlib.h>
#include <string.h>
void ip_DiscardPkt(char* pBuffer, int type);


/*
参数：
    pBuffer：指向接收缓冲区的指针，指向IPv4分组头部
    length：IPv4分组长度
返回值：
    0：成功接收IP分组并交给上层处理
    1：IP分组接收失败
*/
int stud_ip_recv(char* pBuffer, unsigned short length) {
    // 检查接收到的IPv4分组头部的字段
    // 包括版本号（Version）、头部长度（IP  Head  length）、生存时间（Time  to  live）以及头校验和（Header checksum）字段
    // 对于出错的分组调用ip_DiscardPkt()丢弃，并说明错误类型。
    int version = pBuffer[0] >> 4;        // 取第一个字节的前 4 位
    int headLength = pBuffer[0] & 0xf;  // 取第一个字节后 4 位
    int timeToLive = pBuffer[4];        // 取第五个字节所有的八位
    unsigned short headChecksum = *(unsigned short*)(pBuffer + 10);   // 取第 11 个字节的开始位置后面 16 位作为 checksum
    unsigned int destinationIP = *(unsigned int*)(pBuffer + 15);    // 取第 16 个字节的开始位置后面 32 位作为 目的 ip 地址

 // --------------------------------- 分割线 ---------------------------------
    
    // 这里有一个问题, 既先校验哪一个首部字段
    // 按我的理解应该是先校验 checksum, 如果首部出错, 一切免谈



    // 首部校验和
    // 这里的代码参考了 rfc 1071 中检验 checksum 的示例代码
    // 反码算术计算和
    unsigned long sum = 0;
    // sum 取反后的数, 只去后 16 位, 是 short 不是 long
    unsigned short checksum = 0;
    // 首部字节长度, 需要 headLength * 4
    int count = headLength*4;
    // IP 数据报的起始地址, 方便取出一个 16 位的数
    char * addr = pBuffer;
    
    // 以首部中每 16 位作为一个数, 将其相加 
    while (count > 1) {
        sum += *(unsigned short*)addr++;
        count -= 2;
    }
    // 这里有点问题
    // 首部长度是 4 的倍数个字节, 所以肯定也是 2 的倍数, 不存在说 count 每次减 2 ,然后剩下一个字节
    // 不过这是 rfc 1071 的示例代码, 还是加上去
    if (count > 0)
        sum += *(unsigned char*)addr;
    // 然后把 sum 的前 16 位加到后十六位上
    while (sum >> 16)
        sum = (sum & 0xffff) + (sum >> 16);
    // sum 取反后取后十六位
    checksum = ~sum;
    // 结果不为 0, 则代表
    if (checksum != 0) {
        ip_DiscardPkt(pBuffer, stud_ip_test_checksum_error);
        return 1;
    }
    
    // 版本出错, 不是 ipv4
    // 指导书没说具体怎么校验, 在 github 找到的 netriver 实验环境也登录不了(毕竟不是哈工大的人啊)
    // 这里就以 ipv4 为例好了
    if (version!= 4) {
        ip_DiscardPkt(pBuffer, stud_ip_test_version_error);
        return 1;
    }

    // 首部长度出错, 不是 20 个字节或者不是 32 位的倍数
    
    if (headLength*4 >= 20 && headLength*4 % 32 == 0) {
    }
    else {
        ip_DiscardPkt(pBuffer, stud_ip_test_headLength_error);
    }
    // ttl 出错, ttl 值等于 0
    // 这里有个问题, 书上说每经过一个路由器 ttl-1, 那如果经过最后一个路由器时刚好减一到了 0, 那还会不会传过来呢, 路由器会不会直接丢弃呢 
    if (timeToLive == 0) {
        ip_DiscardPkt(pBuffer, stud_ip_test_TTL_error);
    }

    // ②检查IPv4分组是否应该由本机接收
    // 如果分组的目的地址是本机地址或广播地址，则说明此分组是发送给本机的；
    // 否则调用ip_DiscardPkt()丢弃，并说明错误类型。
    
    // 目的地址出错, 不是当前地址也不是广播地址
    if (ntohl(destinationIP) != getIpv4Address() && ntohl(destinationIP) != 0xffffffff) {
        ip_DiscardPkt(pBuffer, stud_ip_destination__error);
        return 1;
    }
    // ③如果IPV4分组应该由本机接收，则提取得到上层协议类型
    // 调用ip_SendtoUp()接口函数，交给系统进行后续接收处理。
    ip_SendtoUp(pBuffer+headLength*4, length-headLength*4);
}


/* 
参数:
    pBuffer:指向发送缓冲区的指针，指向 IPv4 上层协议数据头部
    len : IPv4 上层协议数据长度
    srcAddr : 源 IPv4 地址
    dstAddr : 目的 IPv4 地址
    protocol : IPv4 上层协议号
    ttl : 生存时间(Time To Live)
返回值 :
    0 : 成功发送 IP 分组 1 : 发送 IP 分组失败

*/
int stud_ip_Upsend(char* pBuffer, unsigned short len, unsigned int srcAddr, unsigned int dstAddr, byte protocol, byte ttl){
    // 没有可变部分, length = len +20
   
    char* buffer    = (char*)malloc((len + 20) * sizeof(char));
    for(int i =0;i<len+20;i++){
        buffer[i] = '0';
    }
    // ipv4 , 首部长度为 20/4
    buffer[0] = 0x45;
    
    // 区分服务跳过
    
    // 总长度 len+20
    unsigned short length = len + 20;
    length = htons(length);
    memcpy(buffer + 2, &length, 2);
    
    
    // 标识
    // 剩下的就不写了
    // checksum 再算一遍就好了
    // 目的地址源地址放入的时候记得转换成网络字节序
    

    

    
}

