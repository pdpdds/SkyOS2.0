#pragma once

#pragma pack( push, 1 )

// Ű ť�� ������ ������ ����ü
typedef struct kKeyDataStruct
{
	// Ű���忡�� ���޵� ��ĵ �ڵ�
	unsigned char bScanCode;
	// ��ĵ �ڵ带 ��ȯ�� ASCII �ڵ�
	unsigned char bASCIICode;
	// Ű ���¸� �����ϴ� �÷���(����/������/Ȯ�� Ű ����)
	unsigned char bFlags;
} KEYDATA;

// PS/2 ���콺 ��Ŷ�� �����ϴ� �ڷᱸ��, ���콺 ť�� �����ϴ� ������
typedef struct kMousePacketStruct
{
	// ��ư ���¿� X, Y ���� ���õ� �÷���
	unsigned char bButtonStatusAndFlag;
	// X�� �̵��Ÿ�
	int bXMovement;
	// Y�� �̵��Ÿ�
	int bYMovement;
	// �����ǥ������ ������ǥ�������� ��Ÿ���� �÷���
	unsigned char bAbsoluteCoordinate;
} MOUSEDATA;

// ���콺�� ���¸� �����ϴ� �ڷᱸ��
typedef struct tag_MouseManagerStruct
{
	// �ڷᱸ�� ����ȭ�� ���� ���ɶ�
	//    SPINLOCK stSpinLock;    
	// ���� ���ŵ� �������� ����, ���콺 �����Ͱ� 3���̹Ƿ� 0~2�� ������ ��� �ݺ���
	int iByteCount;
	// ���� ���� ���� ���콺 ������
	MOUSEDATA stCurrentData;
} MOUSESTATE;

#pragma pack( pop )


class I_VirtualIO
{
public:
	virtual bool PutKeyboardQueue(KEYDATA* pData) = 0;
	virtual bool PutMouseQueue(MOUSEDATA* pData) = 0;
};