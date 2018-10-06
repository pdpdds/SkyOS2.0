#pragma once
#include "windef.h"

class SkyQueue
{
public:
	SkyQueue();
	~SkyQueue();

	void Initialize(void* pvQueueBuffer, int iMaxDataCount, int iDataSize);
	bool IsQueueFull();
	bool IsQueueEmpty();
	bool PutQueue(const void* pvData);
	bool GetQueue(void* pvData);

private:
	int m_iDataSize;
	int m_iMaxDataCount;

	// ť ������ �����Ϳ� ����/���� �ε���
	void* m_pvQueueArray;
	int m_iPutIndex;
	int m_iGetIndex;

	// ť�� ����� ������ ����� ���������� ����
	bool m_bLastOperationPut;
	

	void* m_pQueueBuffer;
};

