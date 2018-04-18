// PE文件Day002.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include <windows.h>

BYTE* g_pFileImageBase = 0;
PIMAGE_NT_HEADERS g_pNt = 0;
void ReadFileToMem(WCHAR* szFilePath)
{
	//打开文件获取文件句柄
	HANDLE hFile = CreateFile(szFilePath, GENERIC_READ, FALSE,
		NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hFile == INVALID_HANDLE_VALUE)
	{
		printf("文件打开失败\n");
		return;
	}
	//获取文件大小
	DWORD dwFileSize = GetFileSize(hFile, NULL);

	// 	HANDLE hMap = CreateFileMapping(hFile, NULL, PAGE_READONLY, 0, dwFileSize, NULL);
	// 	char* lpBuf = (char*)MapViewOfFile(hMap, FILE_MAP_READ, NULL,  NULL,dwFileSize);
	// 	char* p = lpBuf++;
	g_pFileImageBase = new BYTE[dwFileSize]{};
	DWORD dwRead;
	//将文件读取到内存中
	bool bRet =
		ReadFile(hFile, g_pFileImageBase, dwFileSize, &dwRead, NULL);
	if (!bRet)
	{
		delete[] g_pFileImageBase;
	}
	//关闭句柄
	CloseHandle(hFile);
}
bool IsPEFile()
{
	//使用PIMAGE_DOS_HEADER（占64字节）解释前64个字节
	PIMAGE_DOS_HEADER pDos = (PIMAGE_DOS_HEADER)g_pFileImageBase;
	//判断PE文件的标识是否正确，有一个不对，那么它就不是PE文件
	if (pDos->e_magic != IMAGE_DOS_SIGNATURE)//0x5A4D('MZ')
	{
		return false;
	}
	g_pNt = (PIMAGE_NT_HEADERS)(pDos->e_lfanew + g_pFileImageBase);
	if (g_pNt->Signature != IMAGE_NT_SIGNATURE)//0x00004550('PE')
	{
		return false;
	}
	return true;
}

void ShowNtImportance()
{
	printf("入口点RVA：%08X", g_pNt->OptionalHeader.AddressOfEntryPoint);
	printf("文件默认加载：%08X", g_pNt->OptionalHeader.ImageBase);

	printf("文件区段个数：%d", g_pNt->FileHeader.NumberOfSections);
	//....
}

void ShowDirTable()
{
	//获取目录表个数
	int nCountOfDirTable = g_pNt->OptionalHeader.NumberOfRvaAndSizes;
	printf("数据目录表个数：%d", g_pNt->OptionalHeader.NumberOfRvaAndSizes);
	for (int i = 0; i < nCountOfDirTable; i++)
	{
		//如果VirtualAddress不为0，说明此表存在
		if (g_pNt->OptionalHeader.DataDirectory[i].VirtualAddress)
		{
			//....
		}
	}
}

void ShowSectionTable()
{
	//获取区段个数
	int nCountOfSection = g_pNt->FileHeader.NumberOfSections;
	//得到首个区段的位置
	PIMAGE_SECTION_HEADER pSec = IMAGE_FIRST_SECTION(g_pNt);
	//保存区段名字（区段名字可能不是以0为结尾的）
	char strName[9] = {};
	for (int i = 0; i < nCountOfSection; i++)
	{
		memcpy(strName, pSec->Name, 8);
		printf("第%d个区段名：%s", i + 1, strName);
		printf("区段RVA：%08X", pSec->VirtualAddress);
		//....
		//下一个区段地址
		pSec++;
	}
}

DWORD RVAtoFOA(DWORD dwRVA)
{
	//此RVA落在哪个区段中
	//找到所在区段后，
	//减去所在区段的起始位置，加上在文件中的起始位置
	int nCountOfSection = g_pNt->FileHeader.NumberOfSections;
	PIMAGE_SECTION_HEADER pSec = IMAGE_FIRST_SECTION(g_pNt);

	DWORD dwSecAligment = g_pNt->OptionalHeader.SectionAlignment;
	for (int i = 0; i < nCountOfSection; i++)
	{
		//求在内存中的真实大小
		DWORD dwRealVirSize = pSec->Misc.VirtualSize % dwSecAligment ?
			pSec->Misc.VirtualSize / dwSecAligment * dwSecAligment + dwSecAligment
			: pSec->Misc.VirtualSize;
		if (dwRVA >= pSec->VirtualAddress &&
			dwRVA < pSec->VirtualAddress + dwRealVirSize)
		{
			//FOA = RVA - 内存中区段的起始位置 + 在文件中区段的起始位置 
			return dwRVA - pSec->VirtualAddress + pSec->PointerToRawData;
		}
		//下一个区段地址
		pSec++;
	}
}
// DWORD FOAtoRVA(DWORD dwFOA)
// {
// 
// }

void TravseralExportTable()
{
	//找到导出表
	DWORD dwExportRVA = 
		g_pNt->OptionalHeader.DataDirectory[0].VirtualAddress;
	//获取在文件中的位置
	PIMAGE_EXPORT_DIRECTORY pExport = 
		(PIMAGE_EXPORT_DIRECTORY)(RVAtoFOA(dwExportRVA) + g_pFileImageBase);

	//模块名字
	char* pName = (char*)(RVAtoFOA(pExport->Name) + g_pFileImageBase);
	printf("%s\n", pName);
	//地址表中的个数
	DWORD dwCountOfFuntions = pExport->NumberOfFunctions;
	//名称表中的个数
	DWORD dwCountOfNames = pExport->NumberOfNames;

	//地址表地址
	PDWORD pAddrOfFuntion = (PDWORD)(RVAtoFOA(pExport->AddressOfFunctions) + g_pFileImageBase);
	//名称表地址
	PDWORD pAddrOfName = (PDWORD)(RVAtoFOA(pExport->AddressOfNames) + g_pFileImageBase);
	//序号表地址
	PWORD pAddrOfOrdial = (PWORD)(RVAtoFOA(pExport->AddressOfNameOrdinals) + g_pFileImageBase);
	//base值
	DWORD dwBase = pExport->Base;
	//遍历地址表中的元素
	for (int i = 0; i < dwCountOfFuntions;i++)
	{
		//地址表中可能存在无用的值（就是为0的值）
		if (pAddrOfFuntion[i] == 0)
		{
			continue;
		}
		//根据序号表中是否有值地址表的下标值，
		//来判断是否是名称导出
		bool bRet = false;
		for (int j = 0; j < dwCountOfNames;j++)
		{
			if (i == pAddrOfOrdial[j])
			{
				//取出名称表中的名称地址RVA
				DWORD dwNameRVA = pAddrOfName[j];
				char* pFunName = (char*)(RVAtoFOA(dwNameRVA) + g_pFileImageBase);
				printf("%04d  %s  0x%08x\n", i + dwBase, pFunName, pAddrOfFuntion[i]);
				bRet = true;
				break;
			}
		}
		if (!bRet)
		{
			//序号表中没有，说明是以序号导出的
			printf("%04d  %08X\n", i + dwBase, pAddrOfFuntion[i]);
		}
				
	}

}
void TravseralImportTable()
{
	//找到导入表
	DWORD dwImpotRVA = g_pNt->OptionalHeader.DataDirectory[1].VirtualAddress;
	//在文件中的位置
	DWORD dwImportInFile = (DWORD)(RVAtoFOA(dwImpotRVA) + g_pFileImageBase);
	PIMAGE_IMPORT_DESCRIPTOR pImport = (PIMAGE_IMPORT_DESCRIPTOR)dwImportInFile;
	
	//遍历每一个导入表
	while (pImport->Name)
	{
		//函数名称地址
		PIMAGE_THUNK_DATA pFirsThunk =
			(PIMAGE_THUNK_DATA)(RVAtoFOA(pImport->FirstThunk) + g_pFileImageBase);
		//模块名
		char* pName = (char*)(RVAtoFOA(pImport->Name) + g_pFileImageBase);
		printf("导入模块名字%s\n", pName);
		while (pFirsThunk->u1.AddressOfData)
		{
			//判断导入方式
			if (IMAGE_SNAP_BY_ORDINAL32(pFirsThunk->u1.AddressOfData))
			{
				//说明是序号导入(低16位是其序号)
				printf("\t\t%04X \n", pFirsThunk->u1.Ordinal & 0xFFFF);
			}
			else
			{
				//名称导入
				PIMAGE_IMPORT_BY_NAME pImportName =
					(PIMAGE_IMPORT_BY_NAME)(RVAtoFOA(pFirsThunk->u1.AddressOfData) + g_pFileImageBase);
				printf("\t\t%04X %s \n", pImportName->Hint, pImportName->Name);
			}
			//
			pFirsThunk++;
		}
		pImport++;
	}
}

int _tmain(int argc, _TCHAR* argv[])
{
	//ReadFileToMem(L"TestDll.dll");
	//IsPEFile();
	//TravseralExportTable();
	//TravseralImportTable();
// 	typedef void(*PF)();
// 	HMODULE hMod = LoadLibrary(L"TestDll.dll");
// 	PF fA = (PF)GetProcAddress(hMod, "funA");
// 	PF fB = (PF)GetProcAddress(hMod, "funB");
// 
// 	fA();
// 	fB();
	return 0;
}

