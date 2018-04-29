# encoding:utf-8

# encode 字符串对象拥有该函数 , 返回一个字节流
# 1. 默认将Unicode编码的字符串编码成指定格式的字符串
#   u'如花'.encode('gb2312') 将字符串编码成gb2312格式的字符串
# 2. 如果要编码的字符串不是Unicode格式,就会报异常
# decode 字节流对象拥有该函数 , 返回一个字符串
# 1. 将指定编码格式的字符串解码还原回Unicode编码的字符串
# 2. 如果要解码的字符串的编码格式和形参中说明的编码格式不一致,
#    会报异常

# 下面是一个字符串
print '如花'
# 下面是一个字节流
# 运行结束后,str保存的是gb2312的编码
str = u'如花'.encode('gb2312')

# 运行结束后,str2保存的是Unicode
str2 =  str.decode('gb2312')




# gb2312(ascii编码的一种变种)
# utf-8
# utf-16
# utf-32
# unicode

