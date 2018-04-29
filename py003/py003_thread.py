# encoding:utf-8
import threading
import time

# 创建一个全局锁对象
g_lock = threading.Lock()
g_num = 0

def threadProc(*args , **kwargs):
    print args ,'  ', kwargs
    while True:
        global g_num

        # python中的线程互斥
        g_lock.acquire() # 加锁
        g_num +=1        # 需要互斥的代码
        g_lock.release() # 解锁

        print 'threadProc'
        time.sleep(1)

# 1. 创建一个线程对象, 并给线程对象传入一个回调函数
t1 = threading.Thread(target=threadProc ,
                      args=(1,2,3,4,5),
                      kwargs={'name':'如花','age':18})

# 2. 调用线程对象的start来启动线程
t1.start()
while True:
    print '主模块函数'
    time.sleep(0.5)

# 3. 可以使用join函数来等待线程退出
