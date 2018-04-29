# encoding:utf-8

# 文件操作的流程和c语言类似
# 1. 打开文件 , open函数
# 2. 读写文件 , read/write
# 2.1 seek
# 3. 关闭文件

# open函数的文件的打开方式有: r,w,a,r+,w+,a+,b
# 字符串加u表示这个字符串的编码是Unicode编码
f = open(u'文件名' , 'w')
# 写入文件
f.write('大家好,我是渣渣辉')
f.close()

# 通过with as 打开文件,无须手动关闭文件, 离开作用域
# 后,文件会被自动关闭
with open(u'文件名', 'r') as f:
    print(f.readline())

# 读写二进制文件时, 一定要在文件打开模式中加'b'
