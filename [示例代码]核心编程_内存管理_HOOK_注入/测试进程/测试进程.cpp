// 测试进程.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include <windows.h>

void fun( ) {

}
int _tmain(int argc, _TCHAR* argv[])
{
	DWORD dwFunctionAddress = (DWORD)&fun;

	system( "pause" );
	MessageBox( 0, 
				L"---------" , 
				L"****" , 0 );
	system( "pause" );
	return 0;
}

