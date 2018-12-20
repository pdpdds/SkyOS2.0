#include "cpu_asm.h"

void read_tsc(int *cl_Low, int *cl_High)
{
	__asm {
		_emit 0Fh
		_emit 31h
		mov ecx, dword ptr cl_Low
		mov[ecx], eax
		mov ecx, dword ptr cl_High
		mov[ecx], edx
	}
}
extern unsigned int g_kernelPageDirectory;
/// Return the physical address of the current page directory
unsigned int GetCurrentPageDir()
{
#ifdef SKY_EMULATOR
	return g_kernelPageDirectory;
#endif
	unsigned int val;
	__asm
	{
		mov eax, cr3
		mov val, eax
	}

	return val;
}