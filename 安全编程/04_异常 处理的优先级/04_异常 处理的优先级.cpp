// 04_异常 处理的优先级.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include <windows.h>

LONG WINAPI vch(EXCEPTION_POINTERS* pExcept){
	printf("vch\n");
	return EXCEPTION_CONTINUE_SEARCH;
}

LONG WINAPI veh(EXCEPTION_POINTERS* pExcept){
	printf("veh\n");
	return EXCEPTION_CONTINUE_SEARCH;
}

LONG WINAPI seh(EXCEPTION_POINTERS* pExcept){
	printf("seh\n");
	return EXCEPTION_CONTINUE_SEARCH;
}


LONG WINAPI ueh(EXCEPTION_POINTERS* pExcept){
	printf("ueh\n");
	return EXCEPTION_CONTINUE_SEARCH;
}

int _tmain(int argc, _TCHAR* argv[])
{
	AddVectoredContinueHandler(TRUE, vch);
	AddVectoredExceptionHandler(TRUE, veh);
	// 在64位系统下, 当程序被调试时,UEH不会被调用
	// 不被调试才会被调用.
	// 在32位系统下,被调试时也会被调用.
	SetUnhandledExceptionFilter(ueh);
	__try{
		*(int*)0 = 0;
	}
	__except (seh(GetExceptionInformation())){

	}
	return 0;
}

