.386
.model flat,stdcall
option casemap:none


include msvcrt.inc
includelib msvcrt.lib

.data
	sz byte "hello world",0ah,0dh,0


.code

;函数
fun proc
	
	mov eax,5
	ret
	;ret 08h    stdcall调用方式
fun endp

;add加法函数
fun_add proc
	
	mov eax,[esp+04h];第一个参数
	add eax,[esp+08h];第二个参数
	ret 

fun_add endp

;local_fun 局部变量 未开辟栈帧
fun_local proc
	
	xor eax,eax
	mov eax,099h;
	
	
	mov dword ptr[esp-04h],055h	;局部变量
	mov dword ptr[esp-08h],077h	;局部变量
	
	sub esp,010h			;抬高栈帧
	
	push offset sz 		
	call crt_printf		;printf(sz);
	add esp,04h		;平衡堆栈
	ret

fun_local endp



;local2_fun 局部变量 开辟栈帧
fun_local2 proc 
	push ebp	;开辟栈帧
	mov ebp,esp
	
	xor eax,eax
	mov eax,055h
	
	mov esp,ebp
	pop ebp
	ret

fun_local endp


;程序入口点
start:
	;push 99h	;参数2
	;push offset sz	;参数1
	;call fun	;调用自己函数 fun(sz,0x99);
	
	;push 50h	;参数2
	;push 100h	;参数1
	;call fun_add	;调用加法函数 fun_add(100,50)
	;add esp,08h	;cdecl调用方式	
	
	;call fun_local
	call fun_local2
	
	ret
end start
end
