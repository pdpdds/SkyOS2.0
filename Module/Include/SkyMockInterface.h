#pragma once
#ifdef WIN32
#include <stdio.h>
#include <Windows.h>
#else
#include "windef.h"
#include "fileio.h"
#endif

#pragma pack (push, 1)
//����� ���� �������̽�
typedef struct SKY_FILE_Interface
{
	size_t (*sky_fread)(void *ptr, size_t size, size_t nmemb, FILE *stream);
	FILE* (*sky_fopen)(const char *filename, const char *mode);
	size_t (*sky_fwrite)(const void *ptr, size_t size, size_t nmemb, FILE *stream);
	int (*sky_fclose)(FILE *stream);
	int (*sky_feof)(FILE *stream);
	int (*sky_ferror)(FILE *stream);
	int (*sky_fflush)(FILE *stream);
	FILE* (*sky_freopen)(const char *filename, const char *mode, FILE *stream);
	int (*sky_fseek)(FILE *stream, long int offset, int whence);
	long int (*sky_ftell)(FILE *stream);
	int (*sky_fgetc)(FILE * stream);
	char* (*sky_fgets)(char *dst, int max, FILE *fp);	
} SKY_FILE_Interface;

//���μ��� ���� �� ���� �������̽�
//������ �������� LPTHREAD_START_ROUTINE �Լ������ʹ� WIN32�� SKYOS�� �Ȱ���...
typedef struct tag_SKY_PROCESS_INTERFACE
{
	unsigned int (*sky_kcreate_process_from_memory)(const char* appName, LPTHREAD_START_ROUTINE lpStartAddress, void* param, UINT32 processType);
	unsigned int (*sky_kcreate_process_from_file)(char* appName, void* param, UINT32 processType);
	unsigned int (*sky_kcreate_thread_from_memory)(unsigned int processId, LPTHREAD_START_ROUTINE lpStartAddress, LPVOID param);	
	unsigned int (*sky_kcreate_thread_from_file)(unsigned int processId, FILE* pFile, LPVOID param);
	unsigned int (*sky_kdestroy_task)(unsigned int taskId); //task�� �ϳ��� ���μ����� ���μ����� ����, taskId�� ���� ������� ���� ���μ����� ����
	void (*sky_ksleep)(int ms);
	unsigned int(*sky_kget_proc_address)(HMODULE hModule, LPCSTR lpProcName);
	unsigned int(*sky_kload_library)(LPCSTR lpLibFileName);
	void(*sky_enter_critical_section)();
	void(*sky_leave_critical_section)();
	unsigned int(*sky_get_tick_count)();
	void(*sky_kexit)(unsigned int taskId);


} SKY_PROCESS_INTERFACE;

//�޸� �Ҵ���� �������̽�
typedef struct SKY_ALLOC_Interface
{
	unsigned int (*sky_kmalloc)(unsigned int sz);
	void (*sky_kfree)(void* p);	
	unsigned int (*sky_kcalloc)(unsigned int count, unsigned int size);
	void* (*sky_krealloc)(void* ptr, size_t size);

} SKY_ALLOC_Interface;

//��°��� �������̽�
typedef struct SKY_Print_Interface
{	
	void(*sky_printf)(const char* str, ...);
	FILE* sky_stdin;
	FILE* sky_stdout;
	FILE* sky_stderr;
	int(*sky_getchar)();
	void(*sky_cls)(char* szCls);
} SKY_Print_Interface;

//DLL�� �ѱ� �������̽� Ŭ����
typedef struct SkyMockInterface
{
	SKY_ALLOC_Interface g_allocInterface;
	SKY_FILE_Interface g_fileInterface;
	SKY_Print_Interface g_printInterface;
}SkyMockInterface;

#pragma pack (pop, 1)

typedef void(*PSetSkyMockInterface)(SKY_ALLOC_Interface, SKY_FILE_Interface, SKY_Print_Interface);
typedef void(*PSetSkyProcessInterface)(SKY_PROCESS_INTERFACE);