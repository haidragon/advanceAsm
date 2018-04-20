// 006-02-内存管理_打开映射对象.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include <windows.h>

int main()
{
	// 打开文件映射对象
	HANDLE hMap = 
		OpenFileMapping(GENERIC_READ | GENERIC_WRITE, 
						FALSE,
						L"Global\\15pb_wenjianyingshe");

	// 将文件映射对象映射到进程的虚拟地址空间中
	LPBYTE p    = (LPBYTE)MapViewOfFile(hMap,
									 FILE_MAP_READ | FILE_MAP_WRITE,
									 0,
									 0,
									 4096);

	*(DWORD*)p  = 0xFFFFFFFF;
	
	UnmapViewOfFile(p);
	CloseHandle(hMap);
    return 0;
}

