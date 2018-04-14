.386 ;386指令集
.model flat,stdcall ;内存模型平坦，windowsAPI调用约定
option casemap:none ;区分大小写

include msvcrt.inc	;包含c运行时库文件
includelib msvcrt.lib	;包含c运行时链接库
include user32.inc
includelib user32.lib



.data		;数据段

;声明结构体
student struct
	id dword ?
	sz byte 20 dup(?)
student ends


;定义结构体变量
mystudent student <?>


;定义数据格式
;定义字符串   变量名，类型，初始值
		szStr byte "hello world",0ah,0dh,0
		szTitle byte "15pb",0
;普通变量
;		变量名，类型，初始值
		ID  dword 99
		
;定义数组	变量名 类型 数组长度 初始值
		Array dword 10 dup(2)
		

.code		;伪指令，代码段开始


;定义函数  函数名 proc
fun proc 
	
	mov eax,99	;返回值eax
	ret

fun endp

start:			
	push offset szStr	;传递参数，offset伪指令，变量的地址
	call crt_printf		;调用printf函数
	add esp,04h;		;平衡堆栈
	
	mov esi,offset Array	;使用数组
	mov ecx, 5;
	
Loop1:	;定义标号
	call fun    		;调用自定义函数
	

	dec ecx;
	cmp ecx,0;
	jz Loopend;		；跳转结束标号
	jmp Loop1		;跳转得到指定处
	
Loopend:

	push 0		;参数4
	push offset szTitle
	push offset szStr
	push 0
	call MessageBox
	
	
	
	ret	
end start	;程序入口点
end		;文件结束



