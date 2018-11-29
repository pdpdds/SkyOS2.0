#pragma once
#include "BasicStruct.h"
#include "LoadDLL.h"
#include "kheap.h"
#include "stl/list.h"

typedef LOAD_DLL_INFO* MODULE_HANDLE;

typedef struct DLLInterface
{
	int(*AddNumbers)(int a, int b);
} DLLInterface;


typedef const DLLInterface*(*PGetDLLInterface)();

class SkyModuleManager
{
public:	
	~SkyModuleManager();

	static SkyModuleManager* GetInstance()
	{
		if (m_pModuleManager == 0)
			m_pModuleManager = new SkyModuleManager();

		return m_pModuleManager;
	}

	bool Initialize();
	
	void* LoadModule(const char* moduleName, bool fromMemory = true);
	bool LoadImplictDLL(DWORD moduleAaddress);
	bool FixIAT(void* image);

	LOAD_DLL_INFO* FindLoadedModule(const char* dll_path);
	BootModule* FindModule(const char* moduleName);
	void PrintMoudleList();
	
	bool UnloadModule(MODULE_HANDLE handle);
	void* GetModuleFunction(void* handle, const char* func_name);

protected:
	MODULE_HANDLE LoadModuleFromFile(const char* dll_path);
	MODULE_HANDLE LoadModuleFromMemory(const char* moduleName);

private:
	SkyModuleManager();
	static SkyModuleManager* m_pModuleManager;
	list<LOAD_DLL_INFO*> m_moduleList;
};