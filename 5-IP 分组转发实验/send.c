
/*
  指导书说了那么多, 就是要实现 3 个函数和路由表
    一个是路由表初始化
    一个是写给系统, 用于向路由表添加信息
    一个是收到 ip 分组时的处理函数
    路由表就用数组实现, 到时候可以去查
*/

#include<stdio.h>
#include<stdlib.h>
#include<math.h>

// 这个是一个路由信息结构体
// 使用路由表添加函数时, 参数就是这个
typedef struct stud_route_msg {
  unsigned int dest;
  unsigned int masklen;
  unsigned int nexthop;

}stud_route_msg, * pointerRouteMsg;

// 路由表中项的结构体
// 只存了网络块 ip 和 下一跳地址
typedef struct routeItem {
  unsigned int hostNetworkBlockIP;
  unsigned int nexthop;
  unsigned int mask;
}routeItem, * pointerRouteItem;

// 路由表的结构体, 本来想用哈弗曼树表示, 但是直接用结构体数组了,到时候比较网络号就好
typedef struct routeTable {
  pointerRouteItem* items;
  int itemNum;
  int size;
}*pointerRouteTable;

pointerRouteTable route_table;

void stud_Route_Init();
void stud_route_add(stud_route_msg* proute);
int stud_fwd_deal(char* pBuffer, int length);
void fwd_LocalRcv(char* pBuffer, int length);
void fwd_SendtoLower(char* pBuffer, int length, unsigned int nexthop);
void stud_Route_Init() {
  route_table = (pointerRouteTable)malloc(sizeof(struct routeTable));
  route_table->size = 1000;
  route_table->items = (pointerRouteItem*)malloc(1000 * sizeof(pointerRouteItem));
  route_table->itemNum = 0;
}
void stud_route_add(stud_route_msg* proute) {
  // 这是我自己想的根据 masklen 求子关掩码, 下面那个是用有符号数右移高位会补符号位的原理 
  // int last = 32 - proute->masklen;
  // unsigned int mask = 0;
  // for(int i =0;i<last;i++){
  //   mask = mask | (0x1 << i);
  // }
  // mask = ~mask;

  // 网络字节序转换为本地字节序
  unsigned int hostMaskLen = ntohl(proute->masklen);
  unsigned int hostDestination = ntohl(proute->dest);
  unsigned int hostNexthop = ntohl(proute->nexthop);
  // 和子关掩码相与计算出网络块 ip
  unsigned int mask = ((int)(1 << 31)) >> (hostMaskLen - 1);
  unsigned int host_ip = hostDestination & mask;

  pointerRouteItem item = (pointerRouteItem)malloc(sizeof(routeItem));

  item->hostNetworkBlockIP = host_ip;
  item->nexthop = hostNexthop;
  item->mask = mask;
  // 路由表添加 item
  route_table->items[++route_table->itemNum] = item;


}

// 收到 ip 分组, 进行处理
int stud_fwd_deal(char* pBuffer, int length) {


  // 1. 先把数据读进来
  // -------------------------------------------------------------------

  int version = pBuffer[0] >> 4;        // 取第一个字节的前 4 位  
  int headLength = pBuffer[0] & 0xf;  // 取第一个字节后 4 位
  int timeToLive = pBuffer[4];        // 取第五个字节所有的八位
  unsigned short headChecksum = *(unsigned short*)(pBuffer + 10);   // 取第 11 个字节的开始位置后面 16 位作为 checksum
  unsigned int destinationIP = *(unsigned int*)(pBuffer + 15);    // 取第 16 个字节的开始位置后面 32 位作为 目的 ip 地址

  // 2. 判断首部是否正确
  // ------------------------------------------------------------------

  // 检查接收到的IPv4分组头部的字段
  // 这里只检验 ttl, 因为给出的 ip_DiscardPkt() 没有其他错误参数

  if (timeToLive == 0) {
    ip_DiscardPkt(pBuffer, stud_forward_test_TTLERROR);
    return 1;
  }


  // 3. 首部校验完毕, 判断是 转发 或 接收 或 丢弃
  // ------------------------------------------------------------------

  // 3.1 目的地址为当前地址或广播地址,发给上层接口
  if (ntohl(destinationIP) == getIpv4Address() || ntohl(destinationIP) == 0xffffffff) {
    fwd_LocalRcv(pBuffer + headLength * 4, length - headLength * 4);
    return 0;
  }
  // 3.2 ip 分组不是给本机的, 查找路由表,看是转发还是丢弃
  
  // 最长前缀匹配的 mask 数值
  unsigned int longestMatch = -1;
  // 找的的最长匹配的 item 在路由表的索引
  int IndexOfRouteTable = -1;
  // 本地字节序目的 ip 地址
  unsigned host_dstIP = ntohl(destinationIP);

  // 查找路由表获取下一跳地址
  for (int i = 0;i < route_table->itemNum;i++) {
    if ((host_dstIP & route_table->items[i]->mask) == route_table->items[i]->hostNetworkBlockIP) {
      if (route_table->items[i]->mask > longestMatch) {
        longestMatch = route_table->items[i]->mask;
        IndexOfRouteTable = i;
      }
    }
  }
  // 3.2.1 目标地址不在路由表中, 丢弃
  if (IndexOfRouteTable == -1) {
    fwd_DiscardPkt(pBuffer, stud_forward_test_noroute);
    return 1;
  }

  // 3.2.2 目标地址在路由表中, 修改 ttl 和校验和后 转发

  unsigned int nextPosition = htonl(route_table->items[IndexOfRouteTable]->nexthop);
  pBuffer[4] = (char)(timeToLive+1);
  
  // 首部校验和 10,11
  unsigned int sum = 0;
  // 首部划分为 16 位的序列,依次相加, 直接跳过校验和
  for (int i = 0;i < length;i = i + 2) {
    if (i == 10) {
      continue;
    }
    sum += (((unsigned int)pBuffer[i]) << 8) + (unsigned int)pBuffer[i + 1];
  }
  // 将溢出的到前 16 位的加到后 16 位
  unsigned short short_sum = (sum >> 16) + (sum & 0xffff);
  // 取反
  short_sum = ~short_sum;
  // 写入
  memcpy(pBuffer + 10, &short_sum, 2);
  
  // 发送给下层协议
  fwd_SendtoLower(pBuffer, length, nextPosition);
  printf(pBuffer, length);
  return 0;

}










