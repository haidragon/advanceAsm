// 04x64混合编程.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"

//声明函数
extern "C" long add_fun(int a, int c);


int main()
{
	//调用汇编函数
	int a =add_fun(5, 6);
    return 0;
}

