// 03_veh.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include <windows.h>


LONG WINAPI veh(EXCEPTION_POINTERS* pExce)
{
	printf("veh\n");
	// 继续执行, 说明异常已被处理,产生异常的指令将会
	// 被继续执行
	EXCEPTION_CONTINUE_EXECUTION;
	// 让下一个veh节点处理异常.
	return EXCEPTION_CONTINUE_SEARCH;
}

LONG WINAPI seh(EXCEPTION_POINTERS* pExce){
	printf("seh\n");
	// 让下一个veh节点处理异常.
	return EXCEPTION_CONTINUE_SEARCH;
}

int _tmain(int argc, _TCHAR* argv[])
{
	//1. 将异常处理函数注册到系统
	AddVectoredExceptionHandler(TRUE, veh);
	__try{

		*(int*)0 = 0;
	}
	__except (seh(GetExceptionInformation())){

	}

	return 0;
}

