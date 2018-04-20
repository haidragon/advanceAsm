// 注入_windwos消息HOOK注入.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include "../messageHookDll/messageHookDll.h"
#include <Windows.h>

int _tmain(int argc, _TCHAR* argv[])

{
	cb_installHook	pfnInstallHook = NULL;

	HMODULE hHookDll = LoadLibrary( L"messageHook.dll" );
	if( hHookDll == NULL ) {
		printf( "dll加载失败\n" );
		return 0;
	}

	pfnInstallHook = GetProcAddress( hHookDll , FUNCTION_INSTALLHOOK );
	if( pfnInstallHook == NULL ) {
		printf( "获取dll导出函数失败\n" );
		return 0;
	}

	pfnInstallHook( );

	system( "pause" );
	return 0;
}

