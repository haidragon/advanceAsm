# encoding:utf-8
import socket

s = socket.socket(socket.AF_INET,socket.SOCK_DGRAM,socket.IPPROTO_UDP)

# 绑定udp服务端
s.bind( ('127.0.0.1', 3456)  )

data = s.recvfrom(100 )
s.sendto('数据', ('127.0.0.1' , 6789))


