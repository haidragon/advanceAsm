// 05_手工安装SEH节点.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include <windows.h>

EXCEPTION_DISPOSITION NTAPI seh(struct _EXCEPTION_RECORD *ExceptionRecord,PVOID EstablisherFrame,struct _CONTEXT *ContextRecord,PVOID DispatcherContext)
{
	printf("seh\n");
	// 继续执行
	return ExceptionContinueExecution;
}

int _tmain(int argc, _TCHAR* argv[])
{
//	EXCEPTION_REGISTRATION_RECORD node;
	/*
	  * 产生异常后 , 操作系统使用fs段寄存器找到TEB, 
	  * 通过TEB.ExceptionList 找到SEH链表的头节点, 
	  * 通过节点中记录的异常处理函数的地址调用该函数.
	*/
// 	node.Handler = seh;
// 	node.Next = NULL;

	_asm
	{
		push seh; // 将SEH异常处理函数的地址入栈
		push fs:[0];//将SEH头节点的地址入栈
		;// esp + 0 -- > [fs:0]; node.Next;
		;// esp + 4 -- > [seh]; node.handler;
		mov fs:[0], esp;// fs:[0] = &node;
	}


	*(int*)0 = 0;


	// 平衡栈空间
	// 还原FS:[0]原始的头节点
	_asm{
		pop fs : [0]; // 将栈顶的数据(原异常头节点的地址)恢复到FS:[0],然后再平衡4个字节的栈
		add esp, 4; // 平衡剩下的4字节的栈.
	}
	return 0;
}

