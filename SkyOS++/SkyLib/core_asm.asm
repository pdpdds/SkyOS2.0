[BITS 32]      

global _CopyUserInternal1
global _GetDR6

SECTION .text

;   extern "C" int CopyUserInternal1(void *dest, const void *src, unsigned int size, unsigned int *handler);
_CopyUserInternal:
	push edi
		push esi
		push ebx
		mov ebx, [esp + 28]
		mov ebx, on_fault
		xor eax, eax				; Clear success flag
		mov ecx, [esp + 24]
		mov edi, [esp + 16]
		mov esi, [esp + 20]
		rep movsb
		mov eax, 1					; Set success flag
on_fault:
		mov ebx, 0	 				; Clear fault handler
		pop ebx
		pop esi
		pop edi
		ret

_GetDR6:
		mov eax, dr6
		ret