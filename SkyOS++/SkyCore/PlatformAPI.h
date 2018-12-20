#pragma once

#ifdef WIN32STUB
#include <Windows.h>
#include <stdio.h>
#else
#include "windef.h"
#include "fileio.h"
#endif

//파일 입출력 관련 인터페이스
typedef struct tag_SKY_FILE_INTERFACE
{
	size_t(*sky_fread)(void *ptr, size_t size, size_t nmemb, FILE *stream);
	FILE* (*sky_fopen)(const char *filename, const char *mode);
	size_t(*sky_fwrite)(const void *ptr, size_t size, size_t nmemb, FILE *stream);
	int(*sky_fclose)(FILE *stream);
	int(*sky_feof)(FILE *stream);
	int(*sky_ferror)(FILE *stream);
	int(*sky_fflush)(FILE *stream);
	FILE* (*sky_freopen)(const char *filename, const char *mode, FILE *stream);
	int(*sky_fseek)(FILE *stream, long int offset, int whence);
	long int(*sky_ftell)(FILE *stream);
	int(*sky_fgetc)(FILE * stream);
	char* (*sky_fgets)(char *dst, int max, FILE *fp);
	int(*sky_fputs)(char const* _Buffer, FILE* _Stream);

} SKY_FILE_INTERFACE;

//프로세스 관련 인터페이스
typedef struct tag_SKY_PROCESS_INTERFACE
{
	unsigned int(*sky_kcreate_process_from_memory)(const char* appName, LPTHREAD_START_ROUTINE lpStartAddress, void* param, UINT32 processType);
	unsigned int(*sky_kcreate_process_from_file)(char* appName, void* param, UINT32 processType);
	unsigned int(*sky_kcreate_thread_from_memory)(unsigned int processId, LPTHREAD_START_ROUTINE lpStartAddress, LPVOID param);
	unsigned int(*sky_kcreate_thread_from_file)(unsigned int processId, FILE* pFile, LPVOID param);
	unsigned int(*sky_kdestroy_task)(unsigned int taskId); //task가 하나인 프로세스는 프로세스도 삭제, taskId가 메인 스레드면 역시 프로세스도 삭제
	void(*sky_ksleep)(int ms);
	unsigned int(*sky_kget_proc_address)(HMODULE hModule, LPCSTR lpProcName);
	unsigned int(*sky_kload_library)(LPCSTR lpLibFileName);
	void(*sky_enter_critical_section)();
	void(*sky_leave_critical_section)();
	unsigned int(*sky_get_tick_count)();
	void(*sky_kexit)(unsigned int taskId);


} SKY_PROCESS_INTERFACE;

//메모리 할당관련 인터페이스
typedef struct tag_SKY_ALLOC_INTERFACE
{
	unsigned int(*sky_kmalloc)(unsigned int sz);
	void(*sky_kfree)(void* p);
	unsigned int(*sky_kcalloc)(unsigned int count, unsigned int size);
	void* (*sky_krealloc)(void* ptr, size_t size);

} SKY_ALLOC_INTERFACE;

//데이터 입출력관련 인터페이스
typedef struct tag_SKY_PRINT_INTERFACE
{
	void(*sky_printf)(const char* str, ...);
	FILE* sky_stdin;
	FILE* sky_stdout;
	FILE* sky_stderr;
	int(*sky_getchar)();
	void(*sky_cls)(char* szCls);
} SKY_PRINT_INTERFACE;


//DLL로 넘길 인터페이스  구조체
typedef struct tag_PlatformAPI
{
	SKY_ALLOC_INTERFACE		_allocInterface;
	SKY_FILE_INTERFACE		_fileInterface;
	SKY_PRINT_INTERFACE		_printInterface;
	SKY_PROCESS_INTERFACE   _processInterface;
}PlatformAPI;

typedef void(*pSetPlatformAPI)(PlatformAPI);

extern PlatformAPI platformAPI;
