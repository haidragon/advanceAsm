// 内存管理_VirtualAlloc.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include <windows.h>

int _tmain(int argc, _TCHAR* argv[])
{
	// 内存管理函数
	//VirtualAlloc( );
	//HeapCreate(	 );
	//CreateFileMapping( );
	//MapViewOfFile( );
	//
	//malloc( );
	//new int;






	LPVOID pBuff = NULL;


	// 预定内存
	// 预定之后, 内存状态就变成预定状态(reserve), 但还没有和物理内存进行关联
	// 因此不能往这种状态的内存中进行读写操作.
	// 预定之后, 需要进行提交才能对内存进行读写.
	// 要释放预定的内存, 需要使用VirtualFree,并使用MEM_RELEASE标志
	// 如果分配的大小不够一个内存分配粒度(64Kb),则函数会向上取整到64Kb
	pBuff = VirtualAlloc( NULL ,		  /*申请在指定地址上预定内存.*/
						  1024*64*3  ,    /*预定3个分配粒度单位的内存*/
						  MEM_RESERVE ,   /*预定内存的标志,保留*/
						  PAGE_NOACCESS /*内存分页属性,在预定时无法设置内存分页属性*/
						  );

	if( pBuff == NULL ) {
		printf( "预定内存失败\n" );
	}

	// 尝试写入刚刚预定的内存,
	__try {
		*(DWORD*)pBuff = 0;
	}
	__except( EXCEPTION_EXECUTE_HANDLER ) {
		printf( "预定的内存必须经过提交才能使用\n" );
	}

	// 将预定的内存提交到物理内存.
	// 提交之后, 内存状态就从预定状态(reserve)变成提交状态(commit)
	// 提交之后, 预定的内存就能够和关联物理内存, 就能够对这种状态内存
	// 进行读写操作(如果这块内存支持读/写权限的话).
	// 如果想要取消提交, 可以使用VirtualFree函数并传入MEM_DECOMMIT
	// 来取消提交.
	// 应当注意的是, 提交之时, 提交的首地址必须是以内存分页粒度进行对齐
	// 的, 内存的大小理应也是内存分页粒度的倍数. 如果首地址不是内存分页
	// 粒度的倍数, 则函数内部会向下取整到内存分页粒度的倍数, 
	// 如果内存地址不足一个内存分页, 函数内部会自动向上取整到内存分页粒度
	// 的倍数.
	// 内存分页粒度一般是4096Kb
	pBuff = VirtualAlloc( pBuff , /* 要和物理内存进行关联的虚拟地址. */
						  4096 ,  /* 需要关联的字节数 */
						  MEM_COMMIT , /* 提交的到物理内存的标志,设置为提交 */
						  PAGE_READWRITE  /*虚拟内存分页属性:可读可写*/
						  );

	// 尝试写入刚刚申请出来的内存
	*(DWORD*)pBuff  = 0;

	pBuff = VirtualAlloc((LPVOID)((DWORD)pBuff+4096), /* 要和物理内存进行关联的虚拟地址. */
						 4096,  /* 需要关联的字节数 */
						 MEM_COMMIT, /* 提交的到物理内存的标志,设置为提交 */
						 PAGE_READWRITE  /*虚拟内存分页属性:可读可写*/
						);

	*(DWORD*)pBuff = 0;

	// 将保留和提交同时进程.
	LPVOID pBuff2;
	pBuff2 = VirtualAlloc(NULL,
						  64 * 1024,
						  MEM_RESERVE | MEM_COMMIT,
						  PAGE_READWRITE
						  );


	// 取消提交到物理内存的虚拟内存.
	// 取消提交后, 可以再次使用VirtualAlloc再次提交
	VirtualFree( pBuff ,  /*开始地址*/
				 4096 ,   /*取消提交的字节数*/
				 MEM_DECOMMIT  /*取消提交的标志*/
				 );


	// 释放预定的内存
	// 释放时, 释放的地址必须是调用VirtualAlloc预定内存所返回的内存首地址
	// 大小必须是0
	// 释放之后, 内存状态就被设置为闲置状态(free)
	VirtualFree( pBuff , 
				 0 , 
				 MEM_RELEASE );

	// 查询一个地址的内存状态
	MEMORY_BASIC_INFORMATION mbi = { 0 };
	/*
	typedef struct _MEMORY_BASIC_INFORMATION {
		PVOID BaseAddress;		// 要查询的地址
		PVOID AllocationBase;  //地址的所在内存块的首地址
		DWORD AllocationProtect;// 分配的内存分页属性
		SIZE_T RegionSize;     // 所在区域的大小
		DWORD State;		  // 内存的状体(闲置,保留,提交)
		DWORD Protect;		  // 内存分配的内存分页属性
		DWORD Type;			 // 映射方式(private,image,mapped)
	} MEMORY_BASIC_INFORMATION
	*/
	HMODULE hMod = GetModuleHandle(NULL);
	VirtualQuery((LPVOID)((DWORD)hMod + 500) ,/*需要查询的地址*/
				 &mbi, /*接收查询结果的结构体*/
				 sizeof(mbi)/*结构体的字节数*/
				 );

	VirtualQuery((LPVOID)0x401000,
				 &mbi, /*接收查询结果的结构体*/
				 sizeof(mbi)/*结构体的字节数*/);

	if (mbi.State == MEM_FREE)// MEM_FREE,MEM_COMMIT,MEM_RESERVE
	{
		VirtualAlloc((LPVOID)0x401000, 64 * 1024, MEM_RESERVE | MEM_COMMIT, 0);
	}
	

	return 0;
}

