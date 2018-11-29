/*
 * Realmode Emulator
 * - Composite File
 */
#include "rme.c"
#include "prefixes.c"
#include "ops_alu.c"
#include "ops_call.c"
#include "ops_flags.c"
#include "ops_io.c"
#include "ops_jump.c"
#include "ops_misc.c"
#include "ops_mov.c"
#include "ops_stack.c"
#include "ops_string.c"

void
__declspec(naked)
_alldiv()
{
	/* *INDENT-OFF* */
	__asm {
		push        edi
		push        esi
		push        ebx
		xor         edi, edi
		mov         eax, dword ptr[esp + 14h]
		or eax, eax
		jge         L1
		inc         edi
		mov         edx, dword ptr[esp + 10h]
		neg         eax
		neg         edx
		sbb         eax, 0
		mov         dword ptr[esp + 14h], eax
		mov         dword ptr[esp + 10h], edx
		L1 :
		mov         eax, dword ptr[esp + 1Ch]
			or eax, eax
			jge         L2
			inc         edi
			mov         edx, dword ptr[esp + 18h]
			neg         eax
			neg         edx
			sbb         eax, 0
			mov         dword ptr[esp + 1Ch], eax
			mov         dword ptr[esp + 18h], edx
			L2 :
		or eax, eax
			jne         L3
			mov         ecx, dword ptr[esp + 18h]
			mov         eax, dword ptr[esp + 14h]
			xor edx, edx
			div         ecx
			mov         ebx, eax
			mov         eax, dword ptr[esp + 10h]
			div         ecx
			mov         edx, ebx
			jmp         L4
			L3 :
		mov         ebx, eax
			mov         ecx, dword ptr[esp + 18h]
			mov         edx, dword ptr[esp + 14h]
			mov         eax, dword ptr[esp + 10h]
			L5 :
			shr         ebx, 1
			rcr         ecx, 1
			shr         edx, 1
			rcr         eax, 1
			or ebx, ebx
			jne         L5
			div         ecx
			mov         esi, eax
			mul         dword ptr[esp + 1Ch]
			mov         ecx, eax
			mov         eax, dword ptr[esp + 18h]
			mul         esi
			add         edx, ecx
			jb          L6
			cmp         edx, dword ptr[esp + 14h]
			ja          L6
			jb          L7
			cmp         eax, dword ptr[esp + 10h]
			jbe         L7
			L6 :
		dec         esi
			L7 :
		xor         edx, edx
			mov         eax, esi
			L4 :
		dec         edi
			jne         L8
			neg         edx
			neg         eax
			sbb         edx, 0
			L8 :
			pop         ebx
			pop         esi
			pop         edi
			ret         10h
	}
	/* *INDENT-ON* */
}

void
__declspec(naked)
_allrem()
{
	/* *INDENT-OFF* */
	__asm {
		push        ebx
		push        edi
		xor         edi, edi
		mov         eax, dword ptr[esp + 10h]
		or eax, eax
		jge         L1
		inc         edi
		mov         edx, dword ptr[esp + 0Ch]
		neg         eax
		neg         edx
		sbb         eax, 0
		mov         dword ptr[esp + 10h], eax
		mov         dword ptr[esp + 0Ch], edx
		L1 :
		mov         eax, dword ptr[esp + 18h]
			or eax, eax
			jge         L2
			mov         edx, dword ptr[esp + 14h]
			neg         eax
			neg         edx
			sbb         eax, 0
			mov         dword ptr[esp + 18h], eax
			mov         dword ptr[esp + 14h], edx
			L2 :
		or eax, eax
			jne         L3
			mov         ecx, dword ptr[esp + 14h]
			mov         eax, dword ptr[esp + 10h]
			xor edx, edx
			div         ecx
			mov         eax, dword ptr[esp + 0Ch]
			div         ecx
			mov         eax, edx
			xor         edx, edx
			dec         edi
			jns         L4
			jmp         L8
			L3 :
		mov         ebx, eax
			mov         ecx, dword ptr[esp + 14h]
			mov         edx, dword ptr[esp + 10h]
			mov         eax, dword ptr[esp + 0Ch]
			L5 :
			shr         ebx, 1
			rcr         ecx, 1
			shr         edx, 1
			rcr         eax, 1
			or ebx, ebx
			jne         L5
			div         ecx
			mov         ecx, eax
			mul         dword ptr[esp + 18h]
			xchg        eax, ecx
			mul         dword ptr[esp + 14h]
			add         edx, ecx
			jb          L6
			cmp         edx, dword ptr[esp + 10h]
			ja          L6
			jb          L7
			cmp         eax, dword ptr[esp + 0Ch]
			jbe         L7
			L6 :
		sub         eax, dword ptr[esp + 14h]
			sbb         edx, dword ptr[esp + 18h]
			L7 :
			sub         eax, dword ptr[esp + 0Ch]
			sbb         edx, dword ptr[esp + 10h]
			dec         edi
			jns         L8
			L4 :
		neg         edx
			neg         eax
			sbb         edx, 0
			L8 :
			pop         edi
			pop         ebx
			ret         10h
	}
	/* *INDENT-ON* */
}

void
__declspec(naked)
_allshr()
{
	/* *INDENT-OFF* */
	__asm {
		cmp         cl, 40h
		jae         RETZERO
		cmp         cl, 20h
		jae         MORE32
		shrd        eax, edx, cl
		sar         edx, cl
		ret
		MORE32 :
		mov         eax, edx
			xor         edx, edx
			and         cl, 1Fh
			sar         eax, cl
			ret
			RETZERO :
		xor         eax, eax
			xor         edx, edx
			ret
	}
	/* *INDENT-ON* */
}
