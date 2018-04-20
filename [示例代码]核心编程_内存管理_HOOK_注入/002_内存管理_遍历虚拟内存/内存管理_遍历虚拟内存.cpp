// 内存管理_遍历虚拟内存.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include <Windows.h>
#include <list>
using std::list;

enum MEMORYSTATE {
	e_stat_free = MEM_FREE ,
	e_stat_reserve = MEM_RESERVE ,
	e_stat_commit = MEM_COMMIT
};

enum MEMORYTYPE {
	e_type_image = MEM_IMAGE ,
	e_type_mapped = MEM_MAPPED ,
	e_type_private = MEM_PRIVATE ,
};

typedef struct VMINFO {
	DWORD		address;
	DWORD		size;
	MEMORYSTATE state;
}VMINFO;




void queryVirtualMemoryStatue( HANDLE hProcess , list<VMINFO>* memoryStatue ) {

	MEMORY_BASIC_INFORMATION	mbi			= { 0 };
	VMINFO						statue		= { 0 };
	DWORD						dwAddress	= 0;
	DWORD						dwSize		= 0;
	BOOL						bRet		= FALSE;
	while( 1 ) {

		bRet = VirtualQueryEx( hProcess , 
							   (LPCVOID)dwAddress , 
							   &mbi , 
							   sizeof( MEMORY_BASIC_INFORMATION ) );
		if( bRet == FALSE )
			break;


		statue.address	= dwAddress;
		statue.state	= (MEMORYSTATE)mbi.State;
		dwSize			= mbi.RegionSize;
		
		// 输出内存状态, 
		// 内存状态用于描述虚拟内存有没有和物理存储器进行关联.
		// 或是否被预定.
		// free   : 闲置,没有预定,没有和物理存储器关联
		// reserve: 保留,被预定,没有和物理存储器关联
		// commit : 提交,已经和物理存储器关联
		switch( statue.state ) {
			case e_stat_free:
				printf( "0x%08X : Free\n" , statue.address );
				break;
			case e_stat_reserve:
				printf( "0x%08X : reserve\n" , statue.address  );
				break;
			case e_stat_commit:
				printf( "0x%08X : commit\n" , statue.address );
				break;
		}

		// 如果内存地址已经提交到物理内存,则遍历提交到的每一个内存块.
		if( statue.state == e_stat_commit ) {

			dwSize						= 0;
			LPVOID	dwAllocationBase	= mbi.AllocationBase;
			DWORD	dwBlockAddress		= (DWORD)dwAddress;
			while( 1 ) {

				bRet = VirtualQueryEx( hProcess ,
									   (LPCVOID)dwBlockAddress ,
									   &mbi ,
									   sizeof( MEMORY_BASIC_INFORMATION ) );
				if( bRet == FALSE ) {
					break;
				}


				// 判断遍历出来的内存块是否是同一块.(看它们的分配的首地址是否相等.)
				// 如果不是,则跳出循环.
				if( mbi.AllocationBase != dwAllocationBase )
					break;

				printf( "\t0x%08X " , dwBlockAddress );

				// 输出内存类型
				// 内存类型表示虚拟内存是以何种方式和物理存储器进行关联
				// image  : 是从影像文件中映射而来
				// mapped : 内存映射
				// private: 私有内存,其它进程无法访问.
				switch( mbi.Type ) {
					case e_type_image:
						printf( " 类型: image   " );
						break;
					case e_type_mapped:
						printf( " 类型: mapped  " );
						break;
					case e_type_private:
						printf( " 类型: private " );
						break;
					default:
						break;
				}

				// 输出内存分页属性
				// 内存分页属性用于表示内存分页能够进行何种访问,如读,写,执行,写时拷贝.
				if (mbi.Protect == 0)
					printf("---");
				else if (mbi.Protect & PAGE_EXECUTE)
					printf("E--");
				else if (mbi.Protect & PAGE_EXECUTE_READ)
					printf("ER-");
				else if (mbi.Protect & PAGE_EXECUTE_READWRITE)
					printf("ERW");
				else if (mbi.Protect & PAGE_READONLY)
					printf("-R-");
				else if (mbi.Protect & PAGE_READWRITE)
					printf("-RW");
				else if (mbi.Protect & PAGE_WRITECOPY)
					printf("WCOPY");
				else if (mbi.Protect & PAGE_EXECUTE_WRITECOPY)
					printf("EWCOPY");

				// 输出内存块的大小.
				printf( " 大小:0x%X\n", mbi.RegionSize );

				// 索引到下一个内存块
				dwBlockAddress +=  mbi.RegionSize;

				// 累加内存块的大小
				dwSize += mbi.RegionSize;
			}
		}

		statue.size = dwSize;
		memoryStatue->push_back( statue );

		// 遍历下一块虚拟内存.
		dwAddress += dwSize;
	}
}

int _tmain(int argc, _TCHAR* argv[])
{
	list<VMINFO> vmList;

	queryVirtualMemoryStatue( GetCurrentProcess( ) , &vmList );

	return 0;
}

