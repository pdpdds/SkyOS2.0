#include "PlatformAPI.h"
#include "SkyAPI.h"
#include "kheap.h"
#include "stdio.h"
#include "SkyConsole.h"
#include "ProcessManager.h"
#include "Thread.h"
#include "Process.h"


SKY_FILE_INTERFACE _fileInterface =
{
	fread,
	fopen,
	fwrite,
	fclose,
	feof,
	ferror,
	fflush,
	freopen,
	fseek,
	ftell,
	fgetc,
	fgets,
	fputs,
};

u32int sky_kmalloc(u32int sz)
{
	return kmalloc(sz);
}

void sky_kfree(void *p)
{
	kfree(p);
}

unsigned int sky_kcalloc(unsigned int count, unsigned int size)
{
	return calloc(count, size);
}

void* sky_krealloc(void* ptr, size_t size)
{
	return krealloc(ptr, size);
}

SKY_ALLOC_INTERFACE _allocInterface =
{
	sky_kmalloc,
	sky_kfree,
	sky_kcalloc,
	sky_krealloc,
};


void sky_printf(const char* str, ...)
{
	char buf[1024];

	va_list arglist;
	va_start(arglist, str);
	vsnprintf(buf, 1024, str, arglist);

	SkyConsole::Print(str);
}

SKY_PRINT_INTERFACE _printInterface =
{
	sky_printf,
	0,
	0,
	0,
	0,
};

unsigned int sky_kcreate_process_from_memory(const char* appName, LPTHREAD_START_ROUTINE lpStartAddress, void* param, UINT32 processType)
{
	return 0;
}

unsigned int sky_kcreate_process_from_file(char* appName, void* param, UINT32 processType)
{
	return 0;
}

unsigned int sky_kcreate_thread_from_memory(unsigned int processId, LPTHREAD_START_ROUTINE lpStartAddress, LPVOID param)
{
	DWORD dwThreadId = 0;

	kEnterCriticalSection();
	//Process* pProcess = ProcessManager::GetInstance()->FindProcess(processId);
	Process* pProcess = ProcessManager::GetInstance()->GetCurrentTask()->m_pParent;
	Thread* pThread = ProcessManager::GetInstance()->CreateThread(pProcess, lpStartAddress, param);

	SKY_ASSERT(pThread != nullptr, "MainThread is null.");

	if (pProcess->m_threadList.size() > 0)
		pProcess->AddThread(pThread);

	kLeaveCriticalSection();

	return (unsigned int)pThread;
}

unsigned int sky_kcreate_thread_from_file(unsigned int processId, FILE* pFile, LPVOID param)
{
	return 0;
}


unsigned int sky_kdestroy_task(unsigned int taskId)
{

	return 0;
}

void sky_ksleep(int ms)
{
	ksleep(ms);
}

void sky_enter_critical_section()
{
	kEnterCriticalSection();
}

void sky_leave_critical_section()
{
	kLeaveCriticalSection();
}

extern unsigned int GetTickCount();
unsigned int sky_get_tick_count()
{
	return GetTickCount();
}

//프로세스 생성 및 삭제
SKY_PROCESS_INTERFACE _processInterface =
{
	sky_kcreate_process_from_memory,
	sky_kcreate_process_from_file,
	sky_kcreate_thread_from_memory,
	sky_kcreate_thread_from_file,
	sky_kdestroy_task,
	sky_ksleep,
	0,
	0,
	sky_enter_critical_section,
	sky_leave_critical_section,
	sky_get_tick_count,
};

