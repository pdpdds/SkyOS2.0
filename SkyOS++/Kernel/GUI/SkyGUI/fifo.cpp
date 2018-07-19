/* FIFO ���̺귯�� */

#include "fifo.h"

#define FLAGS_OVERRUN		0x0001

void fifo32_init(FIFO32 *fifo, int size, int *buf)
/* FIFO ������ �ʱ�ȭ */
{
	fifo->size = size;
	fifo->buf = buf;
	fifo->free = size; /* �� ���� */
	fifo->flags = 0;
	fifo->p = 0; /* write ��ġ */
	fifo->q = 0; /* read ��ġ */
	//fifo->task = task; /* �����Ͱ� ���� ���� ����Ű�� �½�ũ */
	return;
}

int fifo32_put(FIFO32 *fifo, int data)
/* FIFO�� �����͸� ���� �״´� */
{
	if (fifo->free == 0) {
		/* �� ������ ��� ���ƴ� */
		fifo->flags |= FLAGS_OVERRUN;
		return -1;
	}
	fifo->buf[fifo->p] = data;
	fifo->p++;
	if (fifo->p == fifo->size) {
		fifo->p = 0;
	}
	fifo->free--;
	
	return 0;
}

int fifo32_get(FIFO32 *fifo)
/* FIFO�κ��� �����͸� 1�� �����´� */
{
	int data;
	if (fifo->free == fifo->size) {
		/* ���۰� ����� ���� �켱 -1�� �־����� */
		return -1;
	}
	data = fifo->buf[fifo->q];
	fifo->q++;
	if (fifo->q == fifo->size) {
		fifo->q = 0;
	}
	fifo->free++;
	return data;
}

int fifo32_status(FIFO32 *fifo)
/* ��� ���� �����Ͱ� �� ����� �����Ѵ� */
{
	if (fifo == nullptr)
		return 0;

	return fifo->size - fifo->free;
}
