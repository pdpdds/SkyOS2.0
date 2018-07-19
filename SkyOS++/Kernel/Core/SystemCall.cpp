#include "SkyOS.h"

static void* _syscalls[] = {

	kprintf,
	TerminateProcess,
	MemoryAlloc,
	MemoryFree,
	CreateDefaultHeap,
	GetTickCount,
	CreateThread,
	ksleep,
};

_declspec(naked) void SysCallDispatcher()
{

	//���� API ��ȣ�� IDX�� �����ϰ� ������ �����͸� �����Ѵ�(0X10)
	static uint32_t idx = 0;
	_asm {
		push eax
		mov eax, 0x10
		mov ds, ax
		pop eax
		mov[idx], eax
		pusha
	}


	// ��û�� ���� API�� �ε����� �ִ밪���� ũ�� �ƹ��� ó���� ���� �ʴ´�.
	if (idx >= MAX_SYSCALL) {
		_asm {
			/* restore registers and return */
			popa
			iretd
		}
	}

	//���� �ε��� ��ȣ�� �ش��ϴ� �ý��� �Լ��� ����.
	static void* fnct = 0;
	fnct = _syscalls[idx];

	//�ý��� �Լ��� �����Ѵ�.
	_asm {
		//�������͵��� ������ �� �Լ��� �Ķ���ͷ� ����ִ´�.
		popa
		push edi
		push esi
		push edx
		push ecx
		push ebx
		//�ý��� ��
		call fnct
		//���� ���� å���� �ݷ����� �ִ�. �Ķ���ͷ� 5���� ���ÿ� ����־����Ƿ�
		//���� �������� ���� 20����Ʈ ������Ų��.
		add esp, 20
		//Ŀ�� ������ �����Ͱ��� ���� ������ �����Ͱ����� �����Ѵ�.	
		push eax
		mov eax, 0x23
		mov ds, ax
		pop eax
		iretd
	}
}

void InitializeSysCall()
{
	SetInterruptVector(0x80, SysCallDispatcher, I86_IDT_DESC_RING3);
}
