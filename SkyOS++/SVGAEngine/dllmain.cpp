// dllmain.cpp: DLL 응용 프로그램의 진입점을 정의합니다.

#include "windef.h"
#include "va_list.h"
#include "stdarg.h"
#include "SkyInterface.h"
#include "string.h"
#include "stl/IOStream.h"
#include "sprintf.h"
#include "GUIEngine.h"

#define DLL_PROCESS_ATTACH 1
#define DLL_PROCESS_DETACH 0
#define DLL_THREAD_ATTACH 2
#define DLL_THREAD_DETACH 3

extern "C" void _cdecl InitializeConstructors();
extern "C" void _cdecl Exit();

bool __stdcall DllMain(HMODULE hModule,
	DWORD  ul_reason_for_call,
	LPVOID lpReserved
)
{
	//gzFile file;
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
	{
		
		

	}
	break;
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
		break;
	}
	return TRUE;
}


extern "C" bool WINAPI _DllMainCRTStartup(
	HANDLE  hDllHandle,
	DWORD   dwReason,
	LPVOID  lpreserved
)
{
	if (dwReason == DLL_PROCESS_ATTACH)
	{
		// set up our minimal cheezy atexit table
		//_atexit_init();

		// Call C++ constructors
		//_initterm(__xc_a, __xc_z);

		InitializeConstructors();
	}

	bool retcode = DllMain(hDllHandle, dwReason, lpreserved);

	if (dwReason == DLL_PROCESS_DETACH)
	{
		Exit();
	}

	return retcode;
}


extern "C" __declspec(dllexport) I_GUIEngine* GetGUIEngine()
{
	I_GUIEngine* pEngine = new GUIEngine();
	return pEngine;
}