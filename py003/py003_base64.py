# encoding:utf-8

import base64
import hashlib

data = base64.b64encode('hello world')
print data
print base64.b64decode(data)

md5Obj =  hashlib.md5()
md5Obj.update('qq123456')
print md5Obj.hexdigest()
