// 01调用约定.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"

//1.c调用方式 :从右到左入栈,函数外部平衡堆栈
int _cdecl fun_cdecl(int a,int b)
{
	return a + b;
}

//2.stdcall windowsAPI调用约定 :从右到左入栈,函数内部平衡堆栈 : ret 8
int _stdcall fun_stdcall(int a, int b)
{
	return a + b;
}


//3.fastcall快速调用约定 :从右到左入栈,函数内部平衡堆栈 
int _fastcall fun_fastcall(int a, int b,int c,int d )
{
	return a + b+c+d;
}

//4.this c++调用约定    从右到左入栈,函数内部平衡堆栈
class OBJ {
public:
	int  fun_thiscall(int a, int b, int c, int d)
	{
		return a + b + c + d;
	}
	int m_number;
};


int main()
{
	//1.c调用方式
	//fun_cdecl(1, 2);

	//2.stdcall windowsAPI调用约定
	//fun_stdcall(1, 2);

	//3.fastcall快速调用约定
	//fun_fastcall(1, 2,3,4);

	//4.this c++调用约定
	OBJ obj;
	obj.fun_thiscall(1, 2, 3, 4);
    return 0;
}

