#pragma once
#include "windef.h"

void *operator new(size_t size);
void *operator new[](size_t size);
void *operator new(size_t, void *p);
void *operator new[](size_t, void *p);

void operator delete(void *p);
void operator delete(void *p, size_t size);
void operator delete[](void *p);
void operator delete[](void *p, size_t size);

int __cdecl _purecall();

#ifdef SKY_EMULATOR
#define kEnterCriticalSection()	platformAPI._processInterface.sky_enter_critical_section()
#define kLeaveCriticalSection()	platformAPI._processInterface.sky_leave_critical_section()
#else
#define kEnterCriticalSection()	__asm	PUSHFD	__asm CLI
#define kLeaveCriticalSection()		__asm	POPFD
#endif

#ifdef  __cplusplus
extern "C" {
#endif
	void OutPortByte(ushort port, uchar value);
	void OutPortWord(ushort port, ushort value);
	void OutPortDWord(ushort port, unsigned int value);
	uchar InPortByte(ushort port);
	ushort InPortWord(ushort port);
	long InPortDWord(unsigned int port);
#ifdef  __cplusplus
}
#endif

