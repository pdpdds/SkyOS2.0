#include "intrinsic.h"
#include "kheap.h"
#include "stdio.h"

extern "C" int _outp(unsigned short, int);
extern "C" unsigned long _outpd(unsigned int, int);
extern "C" unsigned short _outpw(unsigned short, unsigned short);
extern "C" int _inp(unsigned short);
extern "C" unsigned short _inpw(unsigned short);
extern "C" unsigned long _inpd(unsigned int shor);

void *operator new(size_t size)
{
	return (void *)kmalloc(size);
}

void *operator new(size_t, void *p)
{
	return p;
}

void *operator new[](size_t, void *p)
{
	return p;
}

void * __cdecl operator new[](size_t size)
{
	return (void *)kmalloc(size);
}

void __cdecl operator delete(void *p)
{
	kfree(p);
}

void operator delete(void *p, size_t size)
{
	kfree(p);
}

void operator delete[](void *p)
{
	kfree(p);
}

void operator delete[](void *p, size_t size)
{
	kfree(p);
}

int __cdecl _purecall()
{
	printf("Pure Function Called!!\n");
	return 0;
}

#define DMA_PICU1       0x0020
#define DMA_PICU2       0x00A0

__declspec(naked) void SendEOI()
{
	_asm
	{
		PUSH EBP
		MOV  EBP, ESP
		PUSH EAX

		; [EBP] < -EBP
		; [EBP + 4] < -RET Addr
		; [EBP + 8] < -IRQ 번호

		MOV AL, 20H; EOI 신호를 보낸다.
		OUT DMA_PICU1, AL

		CMP BYTE PTR[EBP + 8], 7
		JBE END_OF_EOI
		OUT DMA_PICU2, AL; Send to 2 also

		END_OF_EOI :
		POP EAX
		POP EBP
		RET
	}
}

extern "C" {

	void OutPortByte(ushort port, uchar value)
	{
		_outp(port, value);
	}

	void OutPortWord(ushort port, ushort value)
	{
		_outpw(port, value);
	}

	void OutPortDWord(ushort port, unsigned int value)
	{
		_outpd(port, value);
	}

	long InPortDWord(unsigned int port)
	{
		return _inpd(port);
	}

	uchar InPortByte(ushort port)
	{

		return (uchar)_inp(port);
	}

	ushort InPortWord(ushort port)
	{
		return _inpw(port);
	}
}