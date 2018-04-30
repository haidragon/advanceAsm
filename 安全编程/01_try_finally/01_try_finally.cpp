// 01_try_finally.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include <windows.h>



int _tmain(int argc, _TCHAR* argv[])
{
	__try{
		printf("try块\n");
		//return 0;
		*(int*)0 = 0;
		__leave; // 以正常方式离开try的关键字
	}
	__finally{
		printf("finally块\n");
	}
	printf("main\n");
	return 0;
}

