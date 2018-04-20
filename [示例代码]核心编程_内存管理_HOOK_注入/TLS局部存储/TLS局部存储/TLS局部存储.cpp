// TLS局部存储.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include <windows.h>

// 普通的全局变量
int g_nNum2 = 0xFFFFFFFF;

// TLS变量
__declspec ( thread ) int  g_nTlsNum = 0x11111111;
__declspec ( thread ) char g_szTlsStr[ ] = "TLS g_nNum = 0x%p ...\r\n";

// TLS回调函数A
void NTAPI t_TlsCallBack_A( PVOID DllHandle , DWORD Reason , PVOID Red )
{
	g_nTlsNum = 0;
    if( DLL_THREAD_DETACH == Reason ) // 如果线程退出则打印信息
        printf( "t_TlsCallBack_A -> ThreadDetach!\r\n" );

	if(DLL_THREAD_ATTACH == Reason) {
		HWND hDbg = FindWindow(L"Qt5QWindowIcon", L"x32dbg");
		if(hDbg != NULL) {
			ExitProcess(0);
		}
	}
    return;
}

// TLS回调函数B
void NTAPI t_TlsCallBack_B( PVOID DllHandle , DWORD Reason , PVOID Red )
{
    if( DLL_THREAD_DETACH == Reason ) // 如果线程退出则打印信息
        printf( "t_TlsCallBack_B -> ThreadDetach!\r\n" );

    //ExitProcess( 0 );
    return;
}

/*
* 注册TLS回调函数，".CRT$XLB"的含义是：
* CRT表明使用C RunTime机制
* X表示标识名随机
* L表示TLS callback section
* B其实也可以为B-Y的任意一个字母
* 有一种说法是：编译器会按照这个字母的
* 顺序来将我们添加的段保存到pe文件中。
*/
#pragma data_seg(".CRT$XLB")
PIMAGE_TLS_CALLBACK p_thread_callback[ ] = 
{
	t_TlsCallBack_A ,  // TLS线程回调函数
	t_TlsCallBack_B ,
    NULL 
};
#pragma data_seg()

DWORD WINAPI t_ThreadFun( PVOID pParam )
{
    printf( "t_Thread ->  first printf:" );
    printf( g_szTlsStr , g_nTlsNum );
    printf( "[1]普通的全局变量:%08X\n" , g_nNum2 );
    
    g_nNum2 = 0;
    //注意这里
    // 将会有两个线程修改此处
    g_nTlsNum = 0x22222222; 


    printf( "t_Thread -> second printf:" );
    printf( g_szTlsStr , g_nTlsNum );
    printf( "[2]普通的全局变量:%08X\n" , g_nNum2 );
    return 0;
}
/*
    当一个线程被创建的时候
    TLS线程回调函数和普通线程回调函数的执行顺序:
    1. 先将TLS线程回调函数都调用完
        1.1 TLS线程回调函数注册多少个,就调用多少个,而且按照定义的顺序调用
    2. 再调用线程回调函数

    当一个线程退出的时候
    1. 将TLS线程回调函数都调用完
        1.1 TLS线程回调函数注册多少个,就调用多少个,而且按照定义的顺序调用
*/

int _tmain( int argc , _TCHAR* argv[ ] )
{
    printf( "_tmain -> TlsDemo.exe is runing...\r\n\r\n" );

	WaitForSingleObject(CreateThread(NULL, 0, t_ThreadFun, NULL, 0, 0),
						-1);

	CreateThread(NULL, 0, t_ThreadFun, NULL, 0, 0);


    //Sleep( 500 );  // 睡眠100毫秒用于确保第一个线程执行完毕
    //printf( "\r\n" );
    //CreateThread( NULL , 0 , t_ThreadFun , NULL , 0 , 0 );
	//IMAGE_TLS_DIRECTORY

    system( "pause" );
    return 0;
}
