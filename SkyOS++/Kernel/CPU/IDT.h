#ifndef _IDT_H
#define _IDT_H
#include <stdint.h>

//Interrupt Descriptor Table : IDT�� ���ͷ�Ʈ�� �ٷ�� �������̽��� ������ å���� �ִ�.
//�������̽� ��ġ, ��û, �׸��� ���ͷ�Ʈ�� ó���ϴ� �ݹ� ��ƾ

//���ͷ�Ʈ �ڵ鷯�� �ִ� ���� : 256
#define I86_MAX_INTERRUPTS		256

#define I86_IDT_DESC_BIT16		0x06	//00000110
#define I86_IDT_DESC_BIT32		0x0E	//00001110
#define I86_IDT_DESC_RING1		0x40	//01000000
#define I86_IDT_DESC_RING2		0x20	//00100000
#define I86_IDT_DESC_RING3		0x60	//01100000
#define I86_IDT_DESC_PRESENT	0x80	//10000000

//���ͷ�Ʈ �ڵ鷯 ���� �Լ�
//���ͷ�Ʈ �ڵ鷯�� ���μ����� ȣ���Ѵ�. �̶� ���� ������ ���ϴµ�
//���ͷ�Ʈ�� ó���ϰ� �����Ҷ� ������ ���ͷ�Ʈ �ڵ鷯 ȣ�������� ������ �Ȱ����� �����ؾ� �Ѵ�.
typedef void (_cdecl *I86_IRQ_HANDLER)(void);

#ifdef _MSC_VER
#pragma pack (push, 1)
#endif

//IDTR ����ü
struct idtr {

	//IDT�� ũ��
	uint16_t limit;

	//IDT�� ���̽� �ּ�
	uint32_t base;
};

//���ͷ�Ʈ ��ũ����
struct idt_descriptor {

	//���ͷ�Ʈ �ڵ鷯 �ּ��� 0-16 ��Ʈ
	uint16_t		baseLo;

	//GDT�� �ڵ� ������
	uint16_t		sel;

	//����� �� 0�̾�� �Ѵ�.
	uint8_t			reserved;

	//8��Ʈ ��Ʈ �÷���
	uint8_t			flags;

	//���ͷ�Ʈ �ڵ鷯 �ּ��� 16-32 ��Ʈ
	uint16_t		baseHi;
};

#ifdef _MSC_VER
#pragma pack (pop)
#endif

//i��° ���ͷ�Ʈ ��ũ��Ʈ�� ���´�.
idt_descriptor* GetInterruptDescriptor(uint32_t i);

//���ͷ�Ʈ �ڵ鷯 ��ġ.
bool InstallInterrputHandler(uint32_t i, uint16_t flags, uint16_t sel, I86_IRQ_HANDLER);
bool IDTInitialize(uint16_t codeSel);
void InterrputDefaultHandler();

#endif
