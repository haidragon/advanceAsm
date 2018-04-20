// 注入_远程线程注入.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include <Windows.h>

bool injectDll( DWORD dwPid , const char* pszDllPath );

int _tmain(int argc, _TCHAR* argv[])
{
	DWORD	dwPid;
	char	szDllPath[ MAX_PATH ] = { "E:\\code\\课上代码\\核心编程_内存管理\\Debug\\testDll2.dll"};


	printf( "输入要注入到的进程PID:" );
	scanf_s( "%d[*]" , &dwPid );

	printf( "输入要注入到的DLL:" );
	//scanf_s( "%s" , szDllPath , MAX_PATH );

	injectDll( dwPid , szDllPath );

	return 0;
}


// 
bool injectDll( DWORD dwPid , const char* pszDllPath ) {

	/**
	* 远程注入的目标: 将一个DLL注入到其它进程的地址空间中.
	* 注入方法:
	* 背景知识:
	* 1. windwos中有一个创建远程线程的API. 这个API能够在
	*    目标进程中创建一个线程. 在创建线程时,能够由自己指
	*	 定线程的回调函数, 但这个函数的地址必须在目标进程
	*    的地址空间中. 线程被创建起来之后, 这个函数就会被
	*	 执行.
	* 2. 当一个DLL被进程加载后, 操作系统会在物理内存中分配
	*    一块空间来保存它, 当这个DLL再次被其它进程加载时,
	*	 系统不会再次分配物理内存来保存这个DLL,而是将这个DLL
	*	 所在的物理内存映射到新进程的虚拟地址空间中.
	*	 因此,系统DLL在每次开机之后,它们的加载地址都是不变的,
	*	 所以,所有进程的系统DLL的加载基址都是相同的,因为它们
	*	 的加载基址相同,故每一个API的地址都在任何进程中也都是
	*    相同的.
	*    使用CreateRemoteThread函数,在目标进程中创建线程.
	*	 CreateRemoteThread需要指定线程回调函数,这个回调
	*    函数只有一个参数, 而LoadLibrary这个系统API刚好也
	*    只有一个参数.而LoadLibrary是一个系统DLL中的函数,
	*	 它在所有进程中的地址都是同一个, 正好为我们所用.
	*	 这样一来, 当我们创建远程线程,LoadLibrary就会被调
	*	 用,现在我们只需要给LoadLibrary函数传一个DLL路径
	*    就成功了. 但这个字符串必须保存在目标进程的地址空间中.
	*	 因为,远程线程的回调函数LoadLibrary虽然在任何进程中
	*    的地址都是同一个, 但是执行它的是其它进程,因此,它的
	*    参数中用到的地址也必须是它所在进程的地址.
	* 注入过程:
	* 1. 使用VirtualAllocEx在目标进程中开辟内存空间.
	* 2. 使用WriteProcessMemory将DLL路径写入到目标进程新
	*	 开的内存空间中.
	* 3. 创建远程线程, 使用LoadLibrary函数作为线程的回调函数,
	*    使用VirtualAllocEx开辟出的内存空间首地址作为回调函数的参数
	* 4. 等待线程退出.
	* 5. 销毁VirtualAllocEx开辟出来的内存空间
	*/

	bool	bRet			= false;
	HANDLE	hProcess		= 0;
	HANDLE	hRemoteThread	= 0;
	LPVOID	pRemoteBuff		= NULL;
	SIZE_T 	dwWrite			= 0 ;
	DWORD	dwSize			= 0;



	// 打开进程
	hProcess = OpenProcess(
		PROCESS_CREATE_THREAD | PROCESS_VM_OPERATION |PROCESS_VM_WRITE ,/*创建线程和写内存权限*/
		FALSE ,
		dwPid /*进程ID*/
		);

	if( hProcess == NULL ) {
		printf( "打开进程失败,可能由于本程序的权限太低,请以管理员身份运行再尝试\n" );
		goto _EXIT;
	}


	// 1. 在远程进程上开辟内存空间
	pRemoteBuff = VirtualAllocEx( hProcess ,
								  NULL ,
								  64 * 1024 , /*大小:64Kb*/
								  MEM_COMMIT ,/*预定并提交*/
								  PAGE_EXECUTE_READWRITE/*可读可写可执行的属性*/
								  );
	if( pRemoteBuff == NULL ) {
		printf( "在远程进程上开辟空降失败\n" );
		goto _EXIT;
	}

	// 2. 将DLL路径写入到新开的内存空间中
	dwSize = strlen( pszDllPath ) + 1;
	WriteProcessMemory( hProcess ,
					   pRemoteBuff,	/* 要写入的地址 */
					   pszDllPath,	/* 要写入的内容的地址 */
					   dwSize,		/* 写入的字节数 */
					   &dwWrite		/* 输出:函数实际写入的字节数 */
						);
	if( dwWrite != dwSize ) {
		printf( "写入DLL路径失败\n" );
		goto _EXIT;
	}

	//3. 创建远程线程,
	//   远程线程创建成功后,DLL就会被加载,DLL被加载后DllMain函数
	//	 就会被执行,如果想要执行什么代码,就在DllMain中调用即可.
	hRemoteThread = CreateRemoteThread(
		hProcess ,
		0 , 0 ,
		(LPTHREAD_START_ROUTINE)LoadLibraryA ,  /* 线程回调函数 */
		pRemoteBuff ,							/* 回调函数参数 */
		0 , 0 );

	// 等待远程线程退出.
	// 退出了才释放远程进程的内存空间.
	WaitForSingleObject( hRemoteThread , -1 );


	bRet = true;


_EXIT:
	// 释放远程进程的内存
	VirtualFreeEx( hProcess , pRemoteBuff , 0 , MEM_RELEASE );
	// 关闭进程句柄
	CloseHandle( hProcess );

	return bRet;
}

