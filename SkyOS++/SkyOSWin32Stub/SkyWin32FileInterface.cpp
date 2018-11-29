#include <Windows.h>
#include "PlatformAPI.h"
#include <conio.h>
#include <string>
#include <stdlib.h>

size_t sky_fread(void *ptr, size_t size, size_t nmemb, FILE *stream)
{
	return fread(ptr, size, nmemb, stream);
}

FILE* sky_fopen(const char *filename, const char *mode)
{
	std::string szFileName;
	szFileName += filename;
	return fopen(szFileName.c_str(), mode);
}

size_t sky_fwrite(const void *ptr, size_t size, size_t nmemb, FILE *stream)
{
	return fwrite(ptr, size, nmemb, stream);
}
int sky_fclose(FILE *stream)
{
	return fclose(stream);
}

int sky_feof(FILE *stream)
{
	return feof(stream);
}

int sky_ferror(FILE *stream)
{
	return ferror(stream);
}

int sky_fflush(FILE *stream)
{
	return fflush(stream);
}

FILE* sky_freopen(const char *filename, const char *mode, FILE *stream)
{
	return freopen(filename, mode, stream);
}

int sky_fseek(FILE *stream, long int offset, int whence)
{
	return fseek(stream, offset, whence);
}

long int sky_ftell(FILE *stream)
{
	return ftell(stream);
}

int sky_fgetc(FILE * stream)
{
	return fgetc(stream);
}

char* sky_fgets(char *dst, int max, FILE *fp)
{
	return fgets(dst, max, fp);
}

int sky_fputs(char const* _Buffer, FILE* _Stream)
{
	return fputs(_Buffer, _Stream);
}

SKY_FILE_INTERFACE g_FileInterface =
{
	sky_fread,
	sky_fopen,
	sky_fwrite,
	sky_fclose,
	sky_feof,
	sky_ferror,
	sky_fflush,
	sky_freopen,
	sky_fseek,
	sky_ftell,
	sky_fgetc,
	sky_fgets,
	sky_fputs,
};


//메모리 할당
unsigned int sky_kmalloc(unsigned int sz)
{
	if (sz == 0)
		return 0;

	return (unsigned int)new char[sz];
}

void sky_kfree(void* p)
{
	delete p;
}

unsigned int sky_kcalloc(unsigned int count, unsigned int size)
{
	return (unsigned int)calloc(count, size);
}

void* sky_krealloc(void* ptr, size_t size)
{
	return realloc(ptr, size);
}

SKY_ALLOC_INTERFACE g_allocInterface =
{
	sky_kmalloc,
	sky_kfree,
	sky_kcalloc,
	sky_krealloc,
};

void sky_printf(const char* str, ...)
{
	char *p = new char[128];
	va_list ap;
	
	va_start(ap, str);
	(void)vsnprintf(p, 128, str, ap);
	va_end(ap);
	
	printf(p);
	delete p;
}

int sky_getchar()
{
	return _getch();
}

/* Standard error macro for reporting API errors */
#define PERR(bSuccess, api){if(!(bSuccess)) printf("%s:Error %d from %s on line %d\n", __FILE__, GetLastError(), api, __LINE__);}

void cls(HANDLE hConsole)
{
	COORD coordScreen = { 0, 0 };    /* here's where we'll home the
									 cursor */
	BOOL bSuccess;
	DWORD cCharsWritten;
	CONSOLE_SCREEN_BUFFER_INFO csbi; /* to get buffer info */
	DWORD dwConSize;                 /* number of character cells in
									 the current buffer */

									 /* get the number of character cells in the current buffer */

	bSuccess = GetConsoleScreenBufferInfo(hConsole, &csbi);
	PERR(bSuccess, "GetConsoleScreenBufferInfo");
	dwConSize = csbi.dwSize.X * csbi.dwSize.Y;

	/* fill the entire screen with blanks */

	bSuccess = FillConsoleOutputCharacter(hConsole, (TCHAR) ' ',
		dwConSize, coordScreen, &cCharsWritten);
	PERR(bSuccess, "FillConsoleOutputCharacter");

	/* get the current text attribute */

	bSuccess = GetConsoleScreenBufferInfo(hConsole, &csbi);
	PERR(bSuccess, "ConsoleScreenBufferInfo");

	/* now set the buffer's attributes accordingly */

	bSuccess = FillConsoleOutputAttribute(hConsole, csbi.wAttributes,
		dwConSize, coordScreen, &cCharsWritten);
	PERR(bSuccess, "FillConsoleOutputAttribute");

	/* put the cursor at (0, 0) */

	bSuccess = SetConsoleCursorPosition(hConsole, coordScreen);
	PERR(bSuccess, "SetConsoleCursorPosition");
	return;
}

void sky_cls(char* szCls)
{
	system(szCls);
}

SKY_PRINT_INTERFACE g_printInterface =
{
	sky_printf,	
	stdin,
	stdout,
	stderr,
	sky_getchar,
	sky_cls,
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
	HANDLE hThread = CreateThread(NULL, 0, lpStartAddress, param, 0, &dwThreadId);

	return (unsigned int)hThread;
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
	Sleep(ms);
}

unsigned int sky_kget_proc_address(HMODULE hModule, LPCSTR lpProcName)
{
	return (unsigned int)GetProcAddress(hModule, lpProcName);
}

unsigned int sky_kload_library(LPCSTR lpLibFileName)
{
	return (unsigned int)LoadLibraryA(lpLibFileName);
}

extern CRITICAL_SECTION g_cs;

void sky_enter_critical_section()
{
	EnterCriticalSection(&g_cs);
}

void sky_leave_critical_section()
{
	LeaveCriticalSection(&g_cs);
}

unsigned int sky_get_tick_count()
{
	return GetTickCount();
}

//프로세스 생성 및 삭제
SKY_PROCESS_INTERFACE g_processInterface =
{
	sky_kcreate_process_from_memory,
	sky_kcreate_process_from_file,
	sky_kcreate_thread_from_memory,
	sky_kcreate_thread_from_file,
	sky_kdestroy_task,
	sky_ksleep,
	sky_kget_proc_address,
	sky_kload_library,
	sky_enter_critical_section,
    sky_leave_critical_section,
	sky_get_tick_count,
};

