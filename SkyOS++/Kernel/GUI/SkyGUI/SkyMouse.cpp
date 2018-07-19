#include "SkyMouse.h"
#include "Hal.h"
#include "fifo.h"
#include "SkyGUI.h"
#include "SkyGUIConsole.h"

FIFO32 *mousefifo = nullptr;
int mousedata0 = 0;

void ProcessSkyMouseHandler()
{
	int data;
	OutPortByte(PIC1_OCW2, 0x64);	/* IRQ-12 ���� �ϷḦ PIC1�� ���� */
	OutPortByte(PIC0_OCW2, 0x62);	/* IRQ-02 ���� �ϷḦ PIC0�� ���� */
	data = InPortByte(PORT_KEYDAT);
	int result = fifo32_put(mousefifo, data + mousedata0);

	return;
}

#define KEYCMD_SENDTO_MOUSE		0xd4
#define MOUSECMD_ENABLE			0xf4

#define PORT_KEYSTA				0x0064
#define KEYSTA_SEND_NOTREADY	0x02
#define KEYCMD_WRITE_MODE		0x60
#define KBC_MODE				0x47

void EnableMouse(FIFO32 *fifo, int data0, MOUSE_DEC *mdec)
{
	/* write�� FIFO ���۸� ��� */
	mousefifo = fifo;
	mousedata0 = data0;
	
	return;
}

int DecodeMouseValue(MOUSE_DEC *mdec, unsigned char dat)
{
	if (mdec->phase == 0) {
		/* ���콺�� 0xfa�� ��ٸ��� �ִ� �ܰ� */
		if (dat == 0xfa) {
			mdec->phase = 1;
		}
		return 0;
	}
	if (mdec->phase == 1) {
		/* ���콺�� 1����Ʈ°�� ��ٸ��� �ִ� �ܰ� */
		if ((dat & 0xc8) == 0x08) {
			/* �ùٸ� 1����Ʈ°���� */
			mdec->buf[0] = dat;
			mdec->phase = 2;
		}
		return 0;
	}
	if (mdec->phase == 2) {
		/* ���콺�� 2����Ʈ°�� ��ٸ��� �ִ� �ܰ� */
		mdec->buf[1] = dat;
		mdec->phase = 3;
		return 0;
	}
	if (mdec->phase == 3) {
		/* ���콺�� 3����Ʈ°�� ��ٸ��� �ִ� �ܰ� */
		mdec->buf[2] = dat;
		mdec->phase = 1;
		mdec->btn = mdec->buf[0] & 0x07;
		mdec->x = mdec->buf[1];
		mdec->y = mdec->buf[2];
		if ((mdec->buf[0] & 0x10) != 0) {
			mdec->x |= 0xffffff00;
		}
		if ((mdec->buf[0] & 0x20) != 0) {
			mdec->y |= 0xffffff00;
		}
		mdec->y = - mdec->y; /* ���콺������ y������ ��ȣ�� ȭ��� �ݴ� */
		return 1;
	}

	//SkyGUI::FillRect8(100, 100, 100, 100, 8, 1024, 768);
	return -1; /* ���⿡ �� ���� ���� �� */
}


FIFO32 *keyfifo;
int keydata0;

void ProcessSkyKeyboardHandler()
{
	int data;
	OutPortByte(PIC0_OCW2, 0x61);	/* IRQ-01 ���� �ϷḦ PIC�� ���� */
	data = InPortByte(PORT_KEYDAT);
	fifo32_put(keyfifo, data + keydata0);
	return;
}

#define PORT_KEYSTA				0x0064
#define KEYSTA_SEND_NOTREADY	0x02
#define KEYCMD_WRITE_MODE		0x60
#define KBC_MODE				0x47

void wait_KBC_sendready(void)
{
	/* Ű���� ��Ʈ�ѷ��� ������ �۽��� �����ϰ� �Ǵ� ���� ��ٸ��� */
	for (;;) {
		if ((InPortByte(PORT_KEYSTA) & KEYSTA_SEND_NOTREADY) == 0) {
			break;
		}
	}
	return;
}


void init_keyboard(FIFO32 *fifo, int data0)
{
	/* write�� FIFO ���۸� ��� */
	keyfifo = fifo;
	keydata0 = data0;
	/* Ű������Ʈ�ѷ��� �ʱ�ȭ */
	
	return;
}