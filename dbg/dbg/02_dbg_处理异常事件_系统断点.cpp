// dbg.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include <windows.h>

#define DBGPRINT(error)  \
		printf("文件：%s中函数：%s 第%d行，错误：%s\n",\
			__FILE__,\
			__FUNCTION__,\
			__LINE__,\
			error);

DWORD OnException(DEBUG_EVENT& dbgEvent);

int main()
{
	TCHAR path[MAX_PATH] = { L"../debug/helloworld.exe" };;

	// 1. 创建调试会话
	STARTUPINFO si = { sizeof(STARTUPINFO) };
	PROCESS_INFORMATION pi = { 0 };
	BOOL bRet = 0;
	bRet = CreateProcess(path,
		NULL,
		NULL,
		NULL,
		FALSE,
		DEBUG_ONLY_THIS_PROCESS | CREATE_NEW_CONSOLE,
		NULL,
		NULL,
		&si,
		&pi);
	if (bRet == FALSE) {
		DBGPRINT("无法创建进程");
	}

	// 2. 处理调试事件
	DEBUG_EVENT dbgEvent;
	DWORD       code = 0;
	while (true)
	{
		// 如果被调试进程产生了调试事件， 函数就会
		// 将对应的信息输出到结构体变量中，并从
		// 函数中返回。如果被调试进程没有调试事件，
		// 函数会处于阻塞状态。
		WaitForDebugEvent(&dbgEvent, -1);
		code = DBG_CONTINUE;
		switch (dbgEvent.dwDebugEventCode)
		{
			case EXCEPTION_DEBUG_EVENT:
			printf("异常事件\n");
			code = OnException(dbgEvent);
			break;


			case CREATE_PROCESS_DEBUG_EVENT: 
			printf("进程创建事件\n");
			printf("\n加载基址：%08X,OEP:%08X\n",
				dbgEvent.u.CreateProcessInfo.lpBaseOfImage,
				dbgEvent.u.CreateProcessInfo.lpStartAddress);
			break; 
			case CREATE_THREAD_DEBUG_EVENT :
			printf("线程创建事件\n");
			break;
			case EXIT_PROCESS_DEBUG_EVENT  : 
			printf("进程退出事件\n");
			goto _EXIT;

			case EXIT_THREAD_DEBUG_EVENT   :
			printf("线程退出事件\n"); 
			break;
			case LOAD_DLL_DEBUG_EVENT 	   : 
			printf("DLL加载事件\n");
			printf("\t加载基址：%08X\n", 
				dbgEvent.u.LoadDll.lpBaseOfDll);
			break;
			case UNLOAD_DLL_DEBUG_EVENT    :
			printf("DLL卸载事件\n");
			break;
			case OUTPUT_DEBUG_STRING_EVENT : 
			printf("调试字符串输出事件\n");
			break;
			case RIP_EVENT 				   :
			printf("RIP事件，已经不使用了\n"); 
			break;
		}
		// 2.1 输出调试信息
		// 2.2 接受用户控制

		// 3. 回复调试子系统
		// 被调试进程产生调试事件之后，会被系统挂起
		// 在调试器回复调试子系统之后，被调试进程才
		// 会运行（回复DBG_CONTINUE才能运行），如果
		// 回复了DBG_CONTINUE，那么被调试的进程的异常
		// 处理机制将无法处理异常。
		// 如果回复了DBG_EXCEPTION_HANDLED： 在异常
		// 分发中，如果是第一次异常处理，异常就被转发到
		// 用户的异常处理机制去处理。如果是第二次，程序
		// 就被结束掉。
		// 一般情况下，处理异常事件之外，都回复DBG_CONTINUE
		// 在异常事件下，根据需求进行不同的回复，原则是：
		// 1. 如果异常是被调试进程自身产生的，那么调试器必须
		//    回复DBG_EXCEPTION_HANDLED，这样做是为了让
		//    被调试进程的异常处理机制处理掉异常。
		// 2. 如果异常是调试器主动制造的(下断点)，那么调试器
		//    需要在去掉异常之后回复DBG_CONTINUE。
		ContinueDebugEvent(dbgEvent.dwProcessId,
			dbgEvent.dwThreadId,
			code);
	}


_EXIT:
	system("pause");
    return 0;
}

// 处理异常事件的函数
DWORD OnException(DEBUG_EVENT& dbgEvent)
{
	// 
	//typedef struct _EXCEPTION_DEBUG_INFO {
	//	// 异常记录
	//	EXCEPTION_RECORD ExceptionRecord;
	//	// 是否是第一次异常分发
	//	DWORD dwFirstChance;
	//} EXCEPTION_DEBUG_INFO, *LPEXCEPTION_DEBUG_INFO;
	// 被调试进程产生的第一个异常事件，这个
	// 异常事件就是系统断点
	EXCEPTION_RECORD& er = dbgEvent.u.Exception.ExceptionRecord;
	printf("\t异常代码：%08X\n", er.ExceptionCode);
	printf("\t异常地址：%08X\n", er.ExceptionAddress);
	static BOOL isSystemBreakpoint = TRUE;
	if (isSystemBreakpoint) {
		printf("\t到达系统断点\n");
		isSystemBreakpoint = FALSE;
	}


	return DBG_CONTINUE;
}

