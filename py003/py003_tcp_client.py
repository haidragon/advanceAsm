# encoding:utf-8
import socket
# 1. 创建一个套接字
client = socket.socket(  )

# 2. 连接到服务端
client.connect( ('127.0.0.1' , 1234 ) )

# 3. 收发数据
while True:
    data = raw_input('客户端> ')
    client.send(data)
    data = client.recv(100)
    print '服务端说:' , data

# 4. 关闭套接字