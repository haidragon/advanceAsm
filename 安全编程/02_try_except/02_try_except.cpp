// 02_try_except.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include <windows.h>

int* g_pNum = NULL;


void fun()
{
	__try{
		*(int*)0 = 0;
	}
	__except (EXCEPTION_CONTINUE_SEARCH){

	}
}

// 异常过滤函数, 用于处理程序中出现的异常.
int seh(EXCEPTION_POINTERS* pExce)
{
	
	printf("在%08X处产生了%08X异常\n",
		pExce->ExceptionRecord->ExceptionAddress,
		pExce->ExceptionRecord->ExceptionCode);
	printf("EAX:%08X ECX:%08X\n",
		pExce->ContextRecord->Eax,
		pExce->ContextRecord->Ecx);

	pExce->ContextRecord->Eax = (DWORD)new int;
	return EXCEPTION_CONTINUE_EXECUTION;
}


int _tmain(int argc, _TCHAR* argv[])
{
	//执行处理程序(except块)
	EXCEPTION_EXECUTE_HANDLER;

	//继续搜索
	// 将异常传递到上一层的try和except,将异常交给它执行
	EXCEPTION_CONTINUE_SEARCH;
	
	//继续执行
	// 继续执行产生异常的那条指令
	EXCEPTION_CONTINUE_EXECUTION;

	__try{
		fun();
		//*(int*)0 = 0;
		*g_pNum = 10;
		printf("try块\n");
	}
	__except ( seh(GetExceptionInformation())){
		printf("finally块\n");
	}


	printf("main()\n");
	__try{
		*(int*)0 = 0;
		printf("try块\n");
	}
	__except (EXCEPTION_CONTINUE_EXECUTION){
		printf("finally块\n");
	}

	return 0;
}

