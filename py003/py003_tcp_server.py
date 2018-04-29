# encoding:utf-8
import socket

# 1. 创建一个服务端套接字
server = socket.socket(socket.AF_INET , # 协议cu
                       socket.SOCK_STREAM,# 套接字的类型
                       socket.IPPROTO_TCP)#协议类型
# 2. 绑定套接字
server.bind( ('127.0.0.1' , 1234) )

# 3. 监听
server.listen(socket.SOMAXCONN)

print('正在等待客户端连接')
# 4. 等待客户端连接
#    函数会返回一个元组(客户端套接字对象, 地址)
client , clientAddr = server.accept()


# 5. 和客户端进行数据的收发
while True:
    data = client.recv(100) # 接收数据
    print('客户端说: '+ data)
    data = raw_input('服务端> ')
    client.send(data)  # 发送数据

# 6. 关闭套接字

