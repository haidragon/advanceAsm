// HOOK_IAT.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include <windows.h>

bool	hookIat( const char* pszDllName , const char* pszFunction , LPVOID pNewFunction );


// 盗版的MessageBox
DWORD WINAPI MyMessageBox( HWND hWnd , TCHAR* pText , TCHAR* pTitle , DWORD type ) {

	// 还原IAT
	hookIat( "User32.dll" , 
			 "MessageBoxW" , 
			 GetProcAddress(GetModuleHandleA("User32.dll"),"MessageBoxW" )
			 );

	// 调用原版函数
	MessageBox( 0 , L"在盗版的MessageBox中弹出此框" , L"提示" , 0 );


	// HOOK IAT
	hookIat( "User32.dll" , "MessageBoxW" , &MyMessageBox );

	return 0;
}



int _tmain(int argc, _TCHAR* argv[])
{
	MessageBox( 0 , L"正版API" , L"提示" , 0 );
	// HOOK IAT
	hookIat( "User32.dll" , "MessageBoxW" , &MyMessageBox );

	MessageBox( 0 , L"正版API" , L"提示" , 0 );
	MessageBox( 0 , L"正版API" , L"提示" , 0 );
	return 0;
}


bool hookIat( const char* pszDllName ,
			  const char* pszFunction ,
			  LPVOID pNewFunction ) {

	// PE文件中,所有的API的地址都保存到了导入表中.

	// 程序调用一个API时, 先会从导入表中得到API
	// 的地址, 再调用这个地址.
	// 如果将导入表中的API地址替换掉, 那么调用
	// API时, 就会调用被替换的地址.

	// HOOK IAT的步骤:
	// 1. 解析PE文件,找到导入表
	// 2. 找到导入表中对应的模块
	// 3. 找到对应模块的对应函数.
	// 4. 修改函数地址.

	// 导入表中有两张表, 一张是导入名称表, 一张是导入
	// 地址表, 这两张表的元素一一对应的.
	// 导入名称表中存放的是函数名
	// 导入地址表中存放的是函数地址.

	HANDLE hProc = GetCurrentProcess( );
	
	PIMAGE_DOS_HEADER			pDosHeader; // Dos头
	PIMAGE_NT_HEADERS			pNtHeader;	// Nt头
	PIMAGE_IMPORT_DESCRIPTOR	pImpTable;	// 导入表
	PIMAGE_THUNK_DATA			pInt;		// 导入表中的导入名称表
	PIMAGE_THUNK_DATA			pIat;		// 导入表中的导入地址表
	DWORD						dwSize;
	DWORD						hModule;
	char*						pFunctionName;
	DWORD						dwOldProtect;

	hModule = ( DWORD)GetModuleHandle( NULL );

	// 读取dos头
	pDosHeader = (PIMAGE_DOS_HEADER)hModule;

	// 读取Nt头
	pNtHeader = (PIMAGE_NT_HEADERS)( hModule + pDosHeader->e_lfanew );


	// 获取导入表
	pImpTable = ( PIMAGE_IMPORT_DESCRIPTOR )
		(hModule + pNtHeader->OptionalHeader.DataDirectory[1].VirtualAddress);

	// 遍历导入表
	while( pImpTable->FirstThunk != 0 && pImpTable->OriginalFirstThunk != 0 ) {


		// 判断是否找到了对应的模块名
		if( _stricmp( (char*)(pImpTable->Name+hModule) , pszDllName ) != 0 ) {
			++pImpTable;
			continue;
		}
		
		// 遍历名称表,找到函数名
		pInt = (PIMAGE_THUNK_DATA)( pImpTable->OriginalFirstThunk + hModule );
		pIat = (PIMAGE_THUNK_DATA)( pImpTable->FirstThunk + hModule );

		while( pInt->u1.AddressOfData != 0 ) {

			// 判断是以名称导入还是以需要导入
			if( pInt->u1.Ordinal & 0x80000000 == 1 ) {
				// 以序号导入

				// 判断是否找到了对应的函数序号
				if( pInt->u1.Ordinal == ( (DWORD)pszFunction ) & 0xFFFF ) {
					// 找到之后,将钩子函数的地址写入到iat
					VirtualProtect( &pIat->u1.Function ,
									4 ,
									PAGE_READWRITE ,
									&dwOldProtect
									);

					pIat->u1.Function = (DWORD)pNewFunction;

					VirtualProtect( &pIat->u1.Function ,
									4 ,
									dwOldProtect ,
									&dwOldProtect
									);
					return true;
				}
			}
			else {
				// 以名称导入
				pFunctionName = (char*)( pInt->u1.Function + hModule + 2);

				// 判断是否找到了对应的函数名
				if( strcmp( pszFunction , pFunctionName ) == 0 ) {

					VirtualProtect( &pIat->u1.Function ,
									4 ,
									PAGE_READWRITE ,
									&dwOldProtect
									);

					// 找到之后,将钩子函数的地址写入到iat
					pIat->u1.Function = (DWORD)pNewFunction;

					VirtualProtect( &pIat->u1.Function ,
									4 ,
									dwOldProtect ,
									&dwOldProtect
									);

					return true;
				}
			}
			
			++pIat;
			++pInt;
		}


		++pImpTable;
	}

	return false;
	
}

