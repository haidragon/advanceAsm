// 02x64汇编.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"

//x64函数调用
long long fun(long long a, long long b, long long c, long long d, long long x)
{
	return a + b + c + d + x;

}

int main()
{
	//64位调用
	fun(0x1, 0x2, 0x3, 0x4, 0x5);

    return 0;
}

