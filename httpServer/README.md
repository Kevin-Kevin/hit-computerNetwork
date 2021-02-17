# http server
## Background
准备写一个http 服务器
- 浏览器发送 get 请求
- 服务器接受后返回请求的资源文件
## Detail
- 服务器主程序一直监听 80 端口
- 接收到请求后新建线程
- 线程使用 acceptedSocket 与客户端通信
    - 线程判断客户端请求是否为get
    - 是为 get , 则发送资源文件
    - 不为 get , 发送 404
