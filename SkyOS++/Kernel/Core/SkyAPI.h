#pragma once
#include "windef.h"
#include "SkyStruct.h"
#include "SysError.h"
#include "time.h"
#include "SkyMockInterface.h"

extern SKY_PROCESS_INTERFACE g_processInterface;

#define ASSERT(a, b) if(a == false) SkyConsole::Print("Kernel Panic : %s\n", b); _asm hlt

#define SKY_ASSERT(Expr, Msg) \
    __SKY_ASSERT(#Expr, Expr, __FILE__, __LINE__, Msg)

void __SKY_ASSERT(const char* expr_str, bool expr, const char* file, int line, const char* msg);

#define kprintf SkyConsole::Print

#ifdef SKY_EMULATOR
#define kEnterCriticalSection()	g_processInterface.sky_enter_critical_section()
#define kLeaveCriticalSection()	g_processInterface.sky_leave_critical_section()
#else
#define kEnterCriticalSection()	__asm	PUSHFD	__asm CLI
#define kLeaveCriticalSection()		__asm	POPFD
#endif

/////////////////////////////////////////////////////////////////////////////
//µ¿±âÈ­
/////////////////////////////////////////////////////////////////////////////
typedef struct _CRITICAL_SECTION {
	
	LONG LockRecursionCount;
	HANDLE OwningThread;        // from the thread's ClientId->UniqueThread
	      
} CRITICAL_SECTION, *LPCRITICAL_SECTION;;


DWORD SKYAPI kGetCurrentThreadId();

bool GetLocalTime(LPSYSTEMTIME lpSystemTime);
BYTE SetLocalTime(LPSYSTEMTIME lpSystemTime);

DWORD GetLastError();
DWORD SetLastError(DWORD dwErrorCode);

#ifdef __cplusplus
extern "C" {
#endif
	void printf(const char* str, ...);
#ifdef __cplusplus
}
#endif

#define TRACE printf


void DumpMemory(void *data, size_t nbytes, size_t bytes_per_line);
void ksleep(int millisecond);
HANDLE CreateThread(SIZE_T dwStackSize, LPTHREAD_START_ROUTINE lpStartAddress, LPVOID lpParameter, DWORD dwCreateionFlags, LPDWORD lpThreadId);
void PauseSystem(const char* msg);

extern "C"
{
	int kdbg_printf(char *pFmt, ...);
	void TerminateProcess();
	uint32_t MemoryAlloc(size_t size);
	void MemoryFree(void* p);
	void CreateDefaultHeap();

	void* GetSkyAPIEntryPoint(char * strAPIName);
	void RegisterSkyAPI(char* strAPIName, void * ptrAPIFunction);
}