## IP 分组的收发

### 要实现功能

1. 在 stud_ip_recv(), 实现分组的接收, 然后使用 ip_SedntoUp() ,将信息提交给上层协议, 或者使用 ip_DiscardPkt() 丢弃有错误的分组并报告

2. 在 stud_ip_Upsend() 中实现IPv4分组封装发送的功能。根据所传参数完成IPv4分组的封装，之后调用接口函数ip_SendtoLower()把分组交给下层完成发送

### 观摩了一下其他人的代码

哈工大一个个都是人才

强制转换和位运算用的一个比一个溜

结构体的内存分配用的也是牛逼

### end

2021年03月13日20:02:02

搞完了

