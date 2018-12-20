#include "idt.h"
#include "string.h"
#include "memory.h"
#include "hal.h"


//���ͷ�Ʈ ��ũ���� ���̺�
static struct idt_descriptor	_idt [I86_MAX_INTERRUPTS];

//CPU�� IDTR �������͸� �ʱ�ȭ�ϴµ� ������ �ִ� IDTR ����ü
static struct idtr				_idtr;

//IDTR �������Ϳ� IDT�� �ּҰ��� �ִ´�.
static void IDTInstall() 
{
#ifndef SKY_EMULATOR
	_asm lidt [_idtr]
#endif
}

//�ٷ�� �ִ� �ڵ鷯�� �������� ������ ȣ��Ǵ� �⺻ �ڵ鷯
__declspec(naked) void InterrputDefaultHandler () {
	
	_asm {
		cli
		pushad
	}

	_asm {
		mov al, 0x20
		out 0x20, al
		popad
		sti
		iretd
	}
}

//i��° ���ͷ�Ʈ ��ũ��Ʈ�� ���´�.
idt_descriptor* GetInterruptDescriptor(uint32_t i) {

	if (i>I86_MAX_INTERRUPTS)
		return 0;

	return &_idt[i];
}

//���ͷ�Ʈ �ڵ鷯 ��ġ
bool InstallInterrputHandler(uint32_t i, uint16_t flags, uint16_t sel, I86_IRQ_HANDLER irq) {

	if (i>I86_MAX_INTERRUPTS)
		return false;

	if (!irq)
		return false;

	//���ͷ�Ʈ�� ���̽� �ּҸ� ���´�.
	uint64_t		uiBase = (uint64_t)&(*irq);
	
	if ((flags & 0x0500) == 0x0500) {
		_idt[i].sel = sel;
		_idt[i].flags = uint8_t(flags);
	}
	else
	{
		//���˿� �°� ���ͷ�Ʈ �ڵ鷯�� �÷��� ���� ��ũ���Ϳ� �����Ѵ�.
		_idt[i].baseLo = uint16_t(uiBase & 0xffff);
		_idt[i].baseHi = uint16_t((uiBase >> 16) & 0xffff);
		_idt[i].reserved = 0;
		_idt[i].flags = uint8_t(flags);
		_idt[i].sel = sel;
	}

	return	true;
}

//IDT�� �ʱ�ȭ�ϰ� ����Ʈ �ڵ鷯�� ����Ѵ�
bool InitIDT(uint16_t codeSel) {

	//IDTR �������Ϳ� �ε�� ����ü �ʱ�ȭ
	_idtr.limit = sizeof(struct idt_descriptor) * I86_MAX_INTERRUPTS - 1;
	_idtr.base = (uint32_t)&_idt[0];

	//NULL ��ũ����
	memset((void*)&_idt[0], 0, sizeof(idt_descriptor) * I86_MAX_INTERRUPTS - 1);

	//����Ʈ �ڵ鷯 ���
	for (int i = 0; i<I86_MAX_INTERRUPTS; i++)
		InstallInterrputHandler(i, I86_IDT_DESC_PRESENT | I86_IDT_DESC_BIT32,
			codeSel, (I86_IRQ_HANDLER)InterrputDefaultHandler);

	//IDTR �������͸� �¾��Ѵ�
	IDTInstall();

	return true;
}

//���ͷ�Ʈ ���� ����
void SetInterruptVector(int intno, void(&vect) (), int flags) {

	//! install interrupt handler! This overwrites prev interrupt descriptor
	InstallInterrputHandler(intno, (uint16_t)(I86_IDT_DESC_PRESENT | I86_IDT_DESC_BIT32 | flags), 0x8, vect);
}


