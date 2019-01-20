#pragma once
#include "windef.h"
#include "stdio.h"
#include "time.h"
#include "SysError.h"

//#define ASSERT(a, b) if(a == false) printf("Kernel Panic : %s\n", b); _asm hlt

#define SKY_ASSERT(Expr, Msg) \
    __SKY_ASSERT(#Expr, Expr, __FILE__, __LINE__, Msg)

void __SKY_ASSERT(const char* expr_str, bool expr, const char* file, int line, const char* msg);

LONG InterlockedAdd(LONG volatile *Addend, LONG Value);
LONG InterlockedAnd(LONG volatile *Destination, LONG Value);
LONG InterlockedCompareExchange(LONG volatile *Destination, LONG ExChange, LONG Comperand);
LONG InterlockedDecrement(LONG volatile *Addend);
LONG InterlockedIncrement(LONG volatile *Addend);
LONG InterlockedOr(LONG volatile *Destination, LONG Value);
LONG InterlockedXor(LONG volatile *Destination, LONG Value);




bool GetLocalTime(LPSYSTEMTIME lpSystemTime);
BYTE SetLocalTime(LPSYSTEMTIME lpSystemTime);
unsigned int GetTickCount();
void msleep(int ms);

DWORD GetLastError();
DWORD SetLastError(DWORD dwErrorCode);

#ifdef __cplusplus
extern "C" {
#endif
void HaltSystem(const char* errMsg);
u32int kmalloc(u32int sz);
void* malloc(size_t sz);
u32int calloc(u32int count, u32int size);
size_t malloc_size(void * ptr);
void* krealloc(void * ptr, size_t size);
void* realloc(void * ptr, size_t size);

int sleep(bigtime_t timeout);
void printf(const char *fmt, ...);
void kfree(void *p);

#ifdef __cplusplus
}
#endif
/**
General deallocation function.
**/


bool ValidatePEImage(void* image);
void PrintCurrentTime();


void panic(const char *fmt, ...);