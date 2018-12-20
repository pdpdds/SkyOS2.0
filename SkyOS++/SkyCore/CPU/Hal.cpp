#include "Hal.h"
#include "PIC.h"
#include "IDT.h"
#include "intrinsic.h"
#include "PlatformAPI.h"

//! notifies hal interrupt is done
void InterruptDone(unsigned int intno) {

	//! insure its a valid hardware irq
	if (intno > 16)
		return;

	//! test if we need to send end-of-interrupt to second pic
	if (intno >= 8)
		SendCommandToPIC(I86_PIC_OCW2_MASK_EOI, 1);

	//! always send end-of-interrupt to primary pic
	SendCommandToPIC(I86_PIC_OCW2_MASK_EOI, 0);
}



DWORD kReadFLAGS()
{
	UINT flag = 0;
	__asm
	{
		pushf; RFLAGS 레지스터를 스택에 저장
		pop eax; 스택에 저장된 RFLAGS 레지스터를 RAX 레지스터에 저장하여		
		mov [flag], eax
	}

	return flag;
}

bool kSetInterruptFlag(bool bEnableInterrupt)
{
	DWORD dwRFLAGS;

	// 이전의 RFLAGS 레지스터 값을 읽은 뒤에 인터럽트 가능/불가 처리
	dwRFLAGS = kReadFLAGS();
	if (bEnableInterrupt == TRUE)
	{
		kLeaveCriticalSection();
	}
	else
	{
		kEnterCriticalSection();
	}

	// 이전 RFLAGS 레지스터의 IF 비트(비트 9)를 확인하여 이전의 인터럽트 상태를 반환
	if (dwRFLAGS & 0x0200)
	{
		return TRUE;
	}
	return FALSE;
}