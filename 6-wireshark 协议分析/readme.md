## wireshark 协议分析

> 协议分析, 直接写 md 文件就好了, 不用写 c 了

### wireshark 的使用

这个就看看实验指导书就好

### http 分析

这是捕获的截图

![image-20210316152643782](https://gitee.com/kevinzhang1999/my-picture/raw/master/uPic/image-20210316152643782-1615879603918.png)

这是电脑发送的 http 请求帧的内容

> vÅ0ØaÞÉÅE.@@À¨iÛÙâýPóÖ¤_~R%
> 3Í"ÿåÀGET /news HTTP/1.1
> Host: hitgs.hit.edu.cn
> Connection: keep-alive
> Upgrade-Insecure-Requests: 1
> User-Agent: Mozilla/5.0 (Macintosh; Intel Mac OS X 10_15_4) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/89.0.4389.82 Safari/537.36
> Accept: text/html,application/xhtml+xml,application/xml;q=0.9,image/avif,image/webp,image/apng,*/*;q=0.8,application/signed-exchange;v=b3;q=0.9
> Accept-Encoding: gzip, deflate
> Accept-Language: en,zh-CN;q=0.9,zh;q=0.8
> Cookie: JSESSIONID=29E01B685053E52CFB059060E05938A2



思考题

- 浏览器运行 HTTP1.1, 服务器运行 HTTP1.1

- 浏览器指出它能接收 

  > Accept: text/html,application/xhtml+xml,application/xml;q=0.9,image/avif,image/webp,image/apng,*/*;q=0.8,application/signed-exchange;v=b3;q=0.9

- 我的计算机 ip 地址是 192.... , 典型的内网地址 ; 服务器 ip 地址 219.2 ... ,公网 ip
- 服务器返回 302 状态代码



----------------------------------------------------------

> 时间 : 2021年03月16日15:35:49
>
> 等我把前面的协议复习了再来继续做更有体会

