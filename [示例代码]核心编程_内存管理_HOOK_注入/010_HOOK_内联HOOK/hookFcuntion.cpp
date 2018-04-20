#include "stdafx.h"
#include "hookFunction.h"
#include <windows.h>
#include <map>
using std::map;

#pragma pack(1) // 设置结构体对齐粒度为1字节对齐
struct JMPCODE{
	BYTE	call;		// call $0
	DWORD	callOffset; // call 的偏移
};

#pragma pack(1)
struct FUNCTION 
{
	LPVOID		desFucntion; // 目标函数的地址(HOOK到的函数)
	LPVOID		srcFcuntion; // 原函数地址
	JMPCODE		backupData;	 // 备份出来的函数数据
};

struct STACK {
	DWORD		entryAddress;
	DWORD		dwOldProtect ;
	DWORD		dwWrite ;
	FUNCTION*	pObj;
	void( _cdecl *fn )( );
	JMPCODE		jmpCode;
};


map<LPVOID , FUNCTION> g_hookMap;
typedef map<LPVOID , FUNCTION>::iterator MapItr;




FUNCTION* getFunctionInfo( LPVOID address ) {

	static FUNCTION obj;
	auto itr = g_hookMap.find( address );
	if( itr == g_hookMap.end( ) )
		return NULL;
	obj = itr->second;
	return &obj;
}





STACK g_Stack;

// 中间函数
void __declspec(naked) bridge_32( ) 
{
	_asm
	{
		mov eax , [ esp ];		// 得到被钩住的函数的地址
		add esp , 4;			// 平衡堆栈

		pushad;					// 保存寄存器
		lea esi , g_Stack;		
		sub  eax , 5;			// call的返回地址-5 => 被钩住函数的首地址
		mov [esi], eax;			// 保存首地址
	}

	// 根据函数首地址找到函数信息.
	g_Stack.pObj = getFunctionInfo( (LPVOID)g_Stack.entryAddress );
	if( g_Stack.pObj == NULL )
		goto _EXIT;
	
	// 修改内存分页属性
	VirtualProtectEx( GetCurrentProcess( ) ,
					  (LPVOID)g_Stack.entryAddress ,
					  sizeof(JMPCODE) ,
					  PAGE_EXECUTE_READWRITE ,
					  &g_Stack.dwOldProtect
					  );

	// 保存当前的shellcode
	ReadProcessMemory( GetCurrentProcess( ) ,
					   (LPVOID)g_Stack.entryAddress ,
					   &g_Stack.jmpCode ,
					   sizeof( JMPCODE ) ,
					   &g_Stack.dwWrite
					   );



	// 恢复函数原来的内容
	WriteProcessMemory( GetCurrentProcess( ) ,
						(LPVOID)g_Stack.entryAddress ,
						&g_Stack.pObj->backupData ,
						sizeof( JMPCODE ) ,
						&g_Stack.dwWrite
						);

	// 调用原版函数
	g_Stack.fn = ( void( _cdecl * )( ) )g_Stack.pObj->desFucntion;
	
	_asm
	{
		;// 调用前, 先将堆栈恢复,否则钩子函数无法访问到原函数的参数.
		popad;					// 恢复堆栈
		mov eax , g_Stack.fn;	// 获取函数地址
		pop g_Stack.fn;			// 弹出返回地址
		call eax;				// 调用函数

		;// 
		push g_Stack.fn;		// 把返回地址放回去
		pushad					// 保存寄存器
	}
	


	// 调用完原版函数之后,再次HOOK这个函数
	WriteProcessMemory( GetCurrentProcess( ) ,
						(LPVOID)g_Stack.entryAddress ,
						&g_Stack.jmpCode ,
						sizeof( JMPCODE ) ,
						&g_Stack.dwWrite
						);

	//	6. 恢复内存分页的属性
	VirtualProtectEx( GetCurrentProcess( ) ,
					  (LPVOID)g_Stack.entryAddress ,
					  sizeof( JMPCODE ) ,
					  g_Stack.dwOldProtect ,
					  &g_Stack.dwOldProtect
					  );


_EXIT:
	_asm
	{
		popad;  //恢复寄存器
		ret;
	}
}


bool	hookFunction( void* targetFunction , void* pNewFcuntionAddress ) 
{

	/**
	* HOOK流程
	* 无论要钩住住哪个函数, 这里的shellcode都只会call到bridge_32函数.
	* 在bridge_32函数中,利用特殊手段来找到被钩住的是哪个函数, 然后调用
	* 与之对应的钩子函数.
	* 
	* 1. 配置shellcode
	*	 call bridge_32
	* 2. 将被钩住的函数,和钩子函数, 以及被钩住函数的原始代码字节保存
	* 3. 修改被钩住函数的内存分页属性
	* 4. 保存被钩住函数的代码字节
	* 5. 将shellcode写入到被钩住的函数中.
	* 6. 恢复分页属性
	* 7. 将被钩住的函数和钩子函数的信息保存起来.
	*/

	// shellcode
	JMPCODE		shellcode = { 0xE8 , 0 } ;
	// 计算call的偏移
	shellcode.callOffset= (DWORD)&bridge_32 - (DWORD)targetFunction - 5 ;

	// 函数信息结构体
	FUNCTION	functionInfo = { 0 };
	functionInfo.desFucntion = pNewFcuntionAddress; // 钩子函数
	functionInfo.srcFcuntion = targetFunction;		// 被钩住的函数
	

	DWORD dwWrite = 0;
	DWORD dwOldProtect = 0;

	// 修改内存分页属性
	VirtualProtectEx( GetCurrentProcess( ) ,
					  (LPVOID)targetFunction ,
					  sizeof( JMPCODE ) ,
					  PAGE_EXECUTE_READWRITE ,
					  &dwOldProtect
					  );

	// 保存被钩住的函数的原始代码字节
	ReadProcessMemory( GetCurrentProcess( ) ,
					   (LPVOID)targetFunction ,
					   &functionInfo.backupData ,
					   sizeof( JMPCODE ) ,
					   &dwWrite
					   );


	// 将shellcode写入到被钩住的函数中
	WriteProcessMemory( GetCurrentProcess( ) ,
						(LPVOID)targetFunction ,
						&shellcode ,
						sizeof( JMPCODE ) ,
						&dwWrite
						);

	//	6. 恢复内存分页的属性
	VirtualProtectEx( GetCurrentProcess( ) ,
					  (LPVOID)targetFunction,
					  sizeof( JMPCODE ) ,
					  dwOldProtect ,
					  &dwOldProtect
					  );

	// 将信息保存.
	return g_hookMap.insert( 
		std::pair<LPVOID , FUNCTION>( targetFunction , functionInfo ) ).second;
}



// 卸载钩子
bool	unhookFunction( void* targetFunction )
{
	// 根据被钩住的函数的地址找到信息
	MapItr itr = g_hookMap.find( targetFunction );

	// 如果没有找到, 说明没有HOOK
	if( itr == g_hookMap.end( ) )
		return false;

	
	DWORD dwOldProtect = 0;
	DWORD dwWrite = 0;
	// 修改内存分页属性
	VirtualProtectEx( GetCurrentProcess( ) ,
					  targetFunction ,
					  sizeof( JMPCODE ) ,
					  PAGE_EXECUTE_READWRITE ,
					  &dwOldProtect
					  );

	// 还原被钩函数的原始代码字节
	WriteProcessMemory( GetCurrentProcess( ) ,
						targetFunction,
						&itr->second.backupData ,
						sizeof( JMPCODE ) ,
						&dwWrite
						);

	// 恢复内存分页属性
	VirtualProtectEx( GetCurrentProcess( ) ,
					  targetFunction ,
					  sizeof( JMPCODE ) ,
					  dwOldProtect ,
					  &dwOldProtect
					  );

	// 删除信息.
	g_hookMap.erase( itr );
	return true;
}



