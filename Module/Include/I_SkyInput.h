#pragma once

#define MOUSE_LBUTTONDOWN   0x01
#define MOUSE_RBUTTONDOWN   0x02
#define MOUSE_MBUTTONDOWN   0x04

#pragma pack( push, 1 )

typedef struct tag_KEYDATA
{	
	unsigned char bScanCode;
	unsigned char bASCIICode;	
	unsigned char bFlags;
} KEYDATA;

typedef struct tag_MouseData
{	
	unsigned char bButtonStatusAndFlag;	
	int bXMovement;
	int bYMovement;
	// �����ǥ������ ������ǥ�������� ��Ÿ���� �÷���
	bool bAbsoluteCoordinate;
} MOUSEDATA;

typedef struct tag_MouseState
{
	int iByteCount;
	MOUSEDATA stCurrentData;
} MOUSESTATE;

#pragma pack( pop )


class I_SkyInput
{
public:
	virtual bool PutKeyboardQueue(KEYDATA* pData) = 0;
	virtual bool PutMouseQueue(MOUSEDATA* pData) = 0;
	virtual bool Print(char* str) { return false; }
};