#include "SkyModuleManager.h"
#include "PlatformAPI.h"
#include "stdio.h"
#include "MultiBoot.h"
#include "SystemAPI.h"
#include "StorageManager.h"
#include "LoadDLL.h"
#include "PEImage.h"
#include "PEFile.h"

SkyModuleManager* SkyModuleManager::m_pModuleManager = nullptr;

extern "C" FILE* g_stdOut;
extern "C" FILE* g_stdIn;
extern "C" FILE* g_stdErr;

#ifdef SKY_EMULATOR
#include "SkyOSWin32Stub.h"
#endif // #ifdef SKY_EMULATOR

SkyModuleManager::SkyModuleManager()
{
}

SkyModuleManager::~SkyModuleManager()
{
}

#include "../../../SkyOS/SkyStartOption.h"
bool SkyModuleManager::Initialize()
{
	g_stdOut = new FILE;
	g_stdIn = new FILE;
	g_stdErr = new FILE;
	strcpy(g_stdOut->_name, "STDOUT");
	strcpy(g_stdIn->_name, "STDIN");
	strcpy(g_stdErr->_name, "STDERR");

#ifdef SKY_EMULATOR
	
#else
	platformAPI._printInterface.sky_stdin = g_stdIn;
	platformAPI._printInterface.sky_stdout = g_stdOut;
	platformAPI._printInterface.sky_stderr = g_stdErr;
#endif // !SKY_EMULATOR

	return true;
}

void SkyModuleManager::PrintMoudleList()
{

	uint32_t mods_count = bootParams._moduleCount;
	uint32_t mods_addr = (uint32_t)bootParams.Modules;

	for (uint32_t mod = 0; mod < mods_count; mod++)
	{
		Module* module = (Module*)(mods_addr + (mod * sizeof(Module)));

		const char* module_string = (const char*)module->Name;

		printf(" %s\n", module_string);
	}

}

BootModule* SkyModuleManager::FindModule(const char* moduleName)
{
	uint32_t mods_count = bootParams._moduleCount;
	uint32_t mods_addr = (uint32_t)bootParams.Modules;

	for (uint32_t mod = 0; mod < mods_count; mod++)
	{
		BootModule* module = (BootModule*)(mods_addr + (mod * sizeof(BootModule)));

		const char* module_string = (const char*)module->Name;

		//SkyConsole::Print("Module Name : %s 0x%x 0x%x\n", module_string, module->ModuleStart, module->ModuleEnd);

		if (strcmp(module_string, moduleName) == 0)
		{
			return module;
		}
	}


	return nullptr;
}

LOAD_DLL_INFO* SkyModuleManager::FindLoadedModule(const char* dll_path)
{
	auto iter = m_moduleList.begin();

	for (; iter != m_moduleList.end(); iter++)
	{
		if (strcmp((*iter)->moduleName, dll_path) == 0)
			return (*iter);
	}

	return nullptr;
}

void* SkyModuleManager::LoadModule(const char* moduleName, bool fromMemory)
{
#ifdef SKY_EMULATOR_DLL
	void* hwnd = (void*)platformAPI._processInterface.sky_kload_library(moduleName);
#else
	MODULE_HANDLE hwnd = SkyModuleManager::GetInstance()->LoadModuleFromMemory(moduleName);
#endif // SKY_EMULATOR_DLL	

	if (hwnd == nullptr)
	{
		HaltSystem("Memory Module Load Fail!!");
	}

#ifdef SKY_EMULATOR_DLL
	pSetPlatformAPI SetPlatformAPI = (pSetPlatformAPI)platformAPI._processInterface.sky_kget_proc_address(hwnd, "SetPlatformAPI");
#else

	if (hwnd->refCount > 1)
		return hwnd;
	
	pSetPlatformAPI SetPlatformAPI = (pSetPlatformAPI)SkyModuleManager::GetInstance()->GetModuleFunction(hwnd, "SetPlatformAPI");
#endif

	//����� ������ �÷��� �������� �������̽��� �ѱ��.
	if (SetPlatformAPI != nullptr)
		SetPlatformAPI(platformAPI);


	return hwnd;
}

MODULE_HANDLE SkyModuleManager::LoadModuleFromFile(const char* dll_path)
{
	LOAD_DLL_INFO *p = FindLoadedModule(dll_path);
	if (p)
	{
		p->refCount++;
		return p;
	}

	p = new LOAD_DLL_INFO;
	strcpy(p->moduleName, dll_path);
	p->refCount = 1;

	DWORD res = LoadDLLFromFileName(dll_path, 0, p);
	if (res != ELoadDLLResult_OK)
	{
		delete p;
		return NULL;
	}

	m_moduleList.push_back(p);
	return p;
}

bool SkyModuleManager::LoadImplictDLL(DWORD moduleAddress)
{
	if (false == ValidatePEImage((void*)moduleAddress))
	{
		printf("LoadImplictDLL Fail\n");		
		return false;
	}

	FixIAT((void*)moduleAddress);

	return true;
}

bool SkyModuleManager::RelocateExe(DWORD imageBase, UINT64 _kernelSize, DWORD relocationBase)
{
	PeImgStt					pe;
	DWORD						dwDestAddr;			// �����ذ� �ּҸ� ����ϱ� ���� ����.
	DWORD						*pT, dwTemp;
	DWORD						dwLastImageByte;
	DWORD						dwDebugPosition;
	LONG						lBase, lDestBase;
	DWORD						dwBuiltInV86Lib, dwBuiltInV86LibSize;

	pe.pBuff = (UCHAR*)imageBase;
	pe.pDosHd = (MY_IMAGE_DOS_HEADER*)(pe.pBuff);
	// ��Ÿ ���̽� �������� �����Ѵ�.
	pe.lPeBase = pe.lIfBase = pe.pDosHd->e_lfanew;
	pe.lIoBase = pe.lPeBase + sizeof(MY_IMAGE_FILE_HEADER);
	// �� HEADER�� �ּҸ� ����Ѵ�.
	pe.pPeHd = (MY_IMAGE_PE_HEADER*)&pe.pBuff[pe.pDosHd->e_lfanew];
	pe.pIfHd = (MY_IMAGE_FILE_HEADER*)pe.pPeHd;
	pe.pIoHd = (MY_IMAGE_OPTIONAL_HEADER*)&pe.pBuff[pe.pDosHd->e_lfanew + sizeof(MY_IMAGE_FILE_HEADER)];

	// ù��° ������ �����ġ�� ���Ѵ�.
	lBase = pe.lIoBase + sizeof(MY_IMAGE_OPTIONAL_HEADER);

	// �Űܰ� �� ���.
	lDestBase = 0;
	//pe.pBase = (UCHAR*)relocationBase;  // �Űܰ� ���� �ּ�.
	pe.pBase = (UCHAR*)new char[_kernelSize + 4096];

									// 2002-12-13 Ŀ�� �̹��� �� ���� 0���� Ŭ����.
	memset(pe.pBase, 0, 512);

	// ���� �ٷ� ���κб����� �׳� �Űܹ�����.
	memcpy(pe.pBase, pe.pBuff, lBase);

	for (int nI = 0; nI < pe.pIfHd->NumberOfSections && nI < BDF_MAX_PE_SECTION - 1; nI++)
	{
		// ���� ����� ��ġ	 ���
		pe.sect[nI].lBase = lBase;
		// ���� ����� ������ ���
		pe.sect[nI].pPtr = (MY_IMAGE_SECTION_HEADER*)&pe.pBuff[lBase];
		// ���Ǹ� ����
		memset(pe.sect[nI].szName, 0, 8);
		memcpy(pe.sect[nI].szName, pe.sect[nI].pPtr, 8);
		pT = (DWORD*)pe.sect[nI].szName;

		// ���Ǹ��� ȭ�鿡 �����Ѵ�.
		printf("%s\n", pe.sect[nI].szName);

		// �ʿ信 ���� ������ Ÿ���� �����Ѵ�.
		pe.sect[nI].nType = nI;

		// ������ �ٵ� ������ ��ġ�� �Ű� ������ �ȴ�.
		dwTemp = (DWORD)&pe.pBuff[pe.sect[nI].pPtr->PointerToRawData];
		memset(&pe.pBase[pe.sect[nI].pPtr->VirtualAddress], 0, pe.sect[nI].pPtr->VirtualSize);  // 0���� Ŭ����.
		memcpy(&pe.pBase[pe.sect[nI].pPtr->VirtualAddress], (char*)dwTemp, pe.sect[nI].pPtr->SizeOfRawData);
		dwTemp += pe.sect[nI].pPtr->SizeOfRawData;
		dwTemp = (DWORD)(((dwTemp + 511) / 512) * 512);

		lBase += sizeof(MY_IMAGE_SECTION_HEADER);

		// �̹����� ������ ����Ʈ ��ġ
		dwLastImageByte = (DWORD)&pe.pBase[pe.sect[nI].pPtr->VirtualAddress] + pe.sect[nI].pPtr->VirtualSize;
	}

	// ����� ������ �ű��. (VC6 with CODEMAP utility)
	if (pe.pIoHd->dd_Debug_dwVAddr != 0)
	{
		char	*pS;
		DWORD   *pX;

		// dd_Debug_dwVAddr�� �׳� ���������� ����Ѵ�.
		pS = (char*)&pe.pBuff[pe.pIoHd->dd_Debug_dwVAddr];
		pX = (DWORD*)pS;

		if (pX[0] == (DWORD)0x46464F43)
		{
			dwDebugPosition = dwLastImageByte;
			dwLastImageByte += pe.pIoHd->dd_Debug_dwSize;

			// ����� ������ �����Ѵ�.
			memcpy((UCHAR*)dwDebugPosition, pS, pe.pIoHd->dd_Debug_dwSize);
		}
		else
			dwDebugPosition = 0;
	}

	// ���� ��� �ٷ� ���� ��ġ�� MAIGIC���� V86Lib �������� �ִ��� Ȯ��.
	dwBuiltInV86Lib = 0;
	dwBuiltInV86LibSize = 0;
	/*pT = (DWORD*)((DWORD)pe.pDosHd + sizeof(MY_IMAGE_DOS_HEADER));
	if (pT[0] == V86PARAM_MAGIC)
	{
		dwBuiltInV86Lib = dwLastImageByte;
		dwBuiltInV86LibSize = pT[2]; // ������
									 // DOS STUB�� �ű��.
		memcpy((BYTE*)dwBuiltInV86Lib, (BYTE*)pT[1] + (DWORD)pe.pDosHd, dwBuiltInV86LibSize);
		dwLastImageByte += dwBuiltInV86LibSize;
	}*/

	// �̹����� ������ ����Ʈ �������� 4096���� �ø��Ѵ�.
	dwLastImageByte = (DWORD)(((dwLastImageByte + 4095) / 4096) * 4096);

	return true;
}



bool SkyModuleManager::FixIAT(void* image)
{
	StorageManager::GetInstance();

	IMAGE_DOS_HEADER* dosHeader = 0;
	IMAGE_NT_HEADERS* ntHeaders = 0;

	dosHeader = (IMAGE_DOS_HEADER*)image;
	if (dosHeader->e_magic != IMAGE_DOS_SIGNATURE)
		return false;

	if (dosHeader->e_lfanew == 0)
		return false;

	//NT Header üũ
	ntHeaders = (IMAGE_NT_HEADERS*)(dosHeader->e_lfanew + (uint32_t)image);
	if (ntHeaders->Signature != IMAGE_NT_SIGNATURE)
		return false;

	/* only supporting for i386 archs */
	if (ntHeaders->FileHeader.Machine != IMAGE_FILE_MACHINE_I386)
		return false;

	/* only support 32 bit executable images */
	if (!(ntHeaders->FileHeader.Characteristics & (IMAGE_FILE_EXECUTABLE_IMAGE | IMAGE_FILE_32BIT_MACHINE)))
		return false;

	/*
	Note: 1st 4 MB remains idenitity mapped as kernel pages as it contains
	kernel stack and page directory. If you want to support loading below 1MB,
	make sure to move these into kernel land
	*/

	//�ε�Ǵ� ���μ����� ���̽� �ּҴ� 0x00400000��. 
	//����� ��Ʃ������� �Ӽ�=> ��Ŀ => ����� �����ּ� �׸񿡼� Ȯ�� �����ϴ�
	if ((ntHeaders->OptionalHeader.ImageBase < 0x400000) || (ntHeaders->OptionalHeader.ImageBase > 0x80000000))
		return false;

	/* only support 32 bit optional header format */
	if (ntHeaders->OptionalHeader.Magic != IMAGE_NT_OPTIONAL_HDR32_MAGIC)
		return false;

	auto importDir = ntHeaders->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT];

	if (!importDir.VirtualAddress || !importDir.Size)
		return false;

	auto importDescriptor = PIMAGE_IMPORT_DESCRIPTOR(ULONG_PTR(image) + importDir.VirtualAddress);
	auto fixIATCount = 0;

	for (; importDescriptor->FirstThunk; importDescriptor++)
	{
		printf("OriginalFirstThunk: %x\n", importDescriptor->OriginalFirstThunk);
		printf("     TimeDateStamp: %x\n", importDescriptor->TimeDateStamp);
		printf("    ForwarderChain: %x\n", importDescriptor->ForwarderChain);
		//if (!IsBadReadPtr((char*)image + importDescriptor->Name, 2))
		printf("              Name: %x %s\n", importDescriptor->Name, (char*)image + importDescriptor->Name);

		if (strcmp("SkyOSWin32Stub.dll", (char*)image + importDescriptor->Name) == 0)
			continue;

		char* dllName = (char*)image + importDescriptor->Name;
		void* hwnd = (void*)LoadModule(dllName);
		
#ifdef SKY_EMULATOR_DLL
		//if (hwnd != nullptr) SKY_EMULATOR_DLL;
		LoadImplictDLL((DWORD)hwnd);
		continue;
#else
		if (((MODULE_HANDLE)(hwnd))->refCount > 1)
			continue;
#endif

		auto thunkData = PIMAGE_THUNK_DATA32(ULONG_PTR(image) + importDescriptor->FirstThunk);

		PIMAGE_THUNK_DATA32 pthunk;
		if (importDescriptor->OriginalFirstThunk == 0)
			pthunk = PIMAGE_THUNK_DATA32(ULONG_PTR(image) + importDescriptor->FirstThunk);
		else
			pthunk = PIMAGE_THUNK_DATA32(ULONG_PTR(image) + importDescriptor->OriginalFirstThunk);
		PIMAGE_THUNK_DATA32 nextthunk;
		for (int i = 0; pthunk->u1.Function != 0; i++, pthunk++) 
		{
			nextthunk = PIMAGE_THUNK_DATA32(ULONG_PTR(image) + importDescriptor->FirstThunk);
			if ((pthunk->u1.Ordinal & IMAGE_ORDINAL_FLAG) == 0) 
			{
				PIMAGE_IMPORT_BY_NAME pname = (PIMAGE_IMPORT_BY_NAME)((PCHAR)image + pthunk->u1.AddressOfData);
				
				void* p = GetModuleFunction(hwnd, (char*)pname->Name);

				if (p)
				{					
					nextthunk[i].u1.Function = reinterpret_cast<DWORD>(p);
					//g_printInterface.sky_printf("Function: %x %s\n", nextthunk[i].u1.Function, (char*)pname->Name);
					printf("Function: %x %s\n", nextthunk[i].u1.Function, (char*)pname->Name);
					fixIATCount++;
				}				
			}			
		}
	}

	
	
	printf("%d imports parsed!\n", fixIATCount);

	return true;
}

MODULE_HANDLE SkyModuleManager::LoadModuleFromMemory(const char* moduleName)
{
	LOAD_DLL_INFO *p = FindLoadedModule(moduleName);
	if (p)
	{
		p->refCount++;
		return p;
	}

	BootModule* pModule = FindModule(moduleName);

	if (pModule == nullptr)
		return nullptr;

	printf("Found %s\n", pModule->Name);

	if (false == ValidatePEImage((void*)pModule->ModuleStart))
	{
		printf("invalid %s\n", pModule->Name);
		return nullptr;
	}

	p = new LOAD_DLL_INFO;
	strcpy(p->moduleName, moduleName);
	p->refCount = 1;

	DWORD res = LoadDLLFromMemory((void*)pModule->ModuleStart, ((size_t)(pModule->ModuleEnd) - (size_t)pModule->ModuleStart), 0, p);
	if (res != ELoadDLLResult_OK)
	{
		delete p;
		printf("LoadModuleFromMemory Fail. ModuleName : %s Error : %d\n", pModule->Name, res);
		p = nullptr;
		return nullptr;
	}

	printf("%s Module Loaded\n", moduleName);
	m_moduleList.push_back(p);

	return p;
}

bool SkyModuleManager::UnloadModule(MODULE_HANDLE handle)
{
	bool findModule = false;
	auto iter = m_moduleList.begin();

	for (; iter != m_moduleList.end(); iter++)
	{
		if ((*iter) == handle)
		{
			findModule = true;
			break;
		}
	}

	if (findModule == false)
	{
		printf("Module UnloadModule Fail!!\n");
		return false;
	}
	
	if (handle->refCount > 1)
	{
		handle->refCount--;
		return true;
	}

	bool res = FALSE != UnloadDLL(handle);
	delete handle;
	return res;
}

void* SkyModuleManager::GetModuleFunction(void* handle, const char* func_name)
{

#ifdef SKY_EMULATOR_DLL
	return (void*)platformAPI._processInterface.sky_kget_proc_address(handle, func_name);
#else
	return (void*)myGetProcAddress_LoadDLLInfo((MODULE_HANDLE)handle, func_name);
#endif
}


