#pragma once
#include "windef.h"

#ifdef _MSC_VER
#define interrupt __declspec (naked)
#else
#define interrupt
#endif

#define far
#define near

void InterruptDone(unsigned int intno);

#pragma pack (push, 1)
typedef struct registers
{
	u32int ds, es, fs, gs;                  // ������ ���׸�Ʈ ������
	u32int edi, esi, ebp, esp, ebx, edx, ecx, eax; // PUSHAD
	u32int eip, cs, eflags, useresp, ss; //���μ����� ���� �ڵ������� Ǫ���Ǵ� ������
} registers_t;
#pragma pack (pop)

bool kSetInterruptFlag(bool bEnableInterrupt);
extern void _cdecl msleep(int ms);
