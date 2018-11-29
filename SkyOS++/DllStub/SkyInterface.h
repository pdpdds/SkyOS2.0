#pragma once
#include "PlatformAPI.h"

#ifdef  __cplusplus
extern "C" {
#endif
	size_t fread(void *ptr, size_t size, size_t nmemb, FILE *stream);
	FILE *fopen(const char *filename, const char *mode);
	size_t fwrite(const void *ptr, size_t size, size_t nmemb, FILE *stream);
	int fclose(FILE *stream);
	int feof(FILE *stream);
	int ferror(FILE *stream);
	int fflush(FILE *stream);
	FILE *freopen(const char *filename, const char *mode, FILE *stream);
	int fseek(FILE *stream, long int offset, int whence);
	long int ftell(FILE *stream);
	int fgetc(FILE * stream);
	char* fgets(char *dst, int max, FILE *fp);
	int fscanf(FILE *fp, const char *fmt, ...);
	char*	strerr(int errnum);
	//extern FILE*	stderr;
	int fprintf(FILE * stream, const char * format, ...);
	char *	strerror(int errnum);

#ifdef  __cplusplus
}
#endif

#ifdef SKY_DLL
extern PlatformAPI platformAPI;
#define kmalloc(a) platformAPI._allocInterface.sky_kmalloc(a)
#define kfree(p) platformAPI._allocInterface.sky_kfree(p)
#define kcalloc(a, b) platformAPI._allocInterface.sky_kcalloc(a, b)
#define krealloc(a, b) platformAPI._allocInterface.sky_krealloc(a, b)
#define ksleep(a) platformAPI._processInterface.sky_ksleep(a)
#define kexit(a) platformAPI._processInterface.sky_kexit(a)
#endif

#ifdef  __cplusplus
extern "C" {
#endif
	void* malloc(size_t size);
	void free(void* p);
	//unsigned int calloc(unsigned int count, unsigned int size);
	void* calloc(size_t nmemb, size_t size);
	void* realloc(void* ptr, size_t size);
	
#ifdef  __cplusplus
}
#endif