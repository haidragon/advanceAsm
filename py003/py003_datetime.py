# encoding:utf-8
import time

# 获取当地前时间, 并并转换成格式化字符串
# 1. 获取当前的时间戳
curTime =  time.time()
print(curTime)

# 2. 将时间戳转换成可读的格式
timeTuple =  time.localtime(curTime)
print(timeTuple)

# 3. 转换成格式化字符串
strTime = time.strftime('%m/%d/%Y %H:%M:%S',timeTuple)
print(strTime)

# 参数的单位是秒
time.sleep(0.5)
