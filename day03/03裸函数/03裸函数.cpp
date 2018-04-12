// 03裸函数.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"

//裸函数 编译器不做任何优化
void _declspec(naked) fun1()
{
	_asm mov eax,99
	_asm ret
	
}

//普通函数
void fun2()
{
	//会添加开辟栈帧
}


int main()
{
	//1.裸函数函数调用
	fun1();
	//2.普通属性
	fun2();
    return 0;
}

