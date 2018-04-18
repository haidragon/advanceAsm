// TLS.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include <windows.h>

#include <delayimp.h>
#pragma comment(lib, "Delayimp.lib")

//自己的dll
#include "DllTest.h"
#pragma comment(lib,"TestDll.lib")

#pragma comment(linker, "/INCLUDE:__tls_used")

// TLS变量
__declspec (thread) int  g_nNum = 0x11111111;
__declspec (thread) char g_szStr[] = "TLS g_nNum = 0x%p ...\r\n";
// TLS回调函数A
void NTAPI t_TlsCallBack_A(PVOID DllHandle, DWORD Reason, PVOID Red) {
	
	if (DLL_THREAD_ATTACH == Reason) // 如果线程退出则打印信息
		printf("t_TlsCallBack_A -> ThreadDetach!\r\n");
	return;
}
// TLS回调函数B
void NTAPI t_TlsCallBack_B(PVOID DllHandle, DWORD Reason, PVOID Red) {
	printf("t_TlsCallBack_B -> ThreadDetach!\r\n");
// 	if (DLL_THREAD_DETACH == Reason) // 如果线程退出则打印信息
// 		printf("t_TlsCallBack_B -> ThreadDetach!\r\n");
	return;
}
#pragma data_seg(".CRT$XLB")
PIMAGE_TLS_CALLBACK p_thread_callback[] = {
	t_TlsCallBack_A,
	t_TlsCallBack_B,
	NULL };
#pragma data_seg()

DWORD WINAPI fun(LPVOID)
{
	printf("-----");
	return 0;
}
int _tmain(int argc, _TCHAR* argv[])
{
	//CreateThread(0, 0, fun, 0, 0, 0);
	
	//Sleep(100);
	funA();
	//system("pause");
	return 0;
}

