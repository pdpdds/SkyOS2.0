#pragma once
#include "windef.h"
#include "I_VirtualIO.h"

// Ű ť�� ���� ��ũ��
// Ű ť�� �ִ� ũ��
#define KEY_MAXQUEUECOUNT	100

#pragma pack( push, 1 )

////////////////////////////////////////////////////////////////////////////////
// ���콺 ť�� ���� ��ũ��
#define MOUSE_MAXQUEUECOUNT 100

// ��ư�� ���¸� ��Ÿ���� ��ũ��
#define MOUSE_LBUTTONDOWN   0x01
#define MOUSE_RBUTTONDOWN   0x02
#define MOUSE_MBUTTONDOWN   0x04

// ��ĵ �ڵ� ���̺��� �����ϴ� �׸�
typedef struct kKeyMappingEntryStruct
{
	// Shift Ű�� Caps Lock Ű�� ���յ��� �ʴ� ASCII �ڵ�
	BYTE bNormalCode;

	// Shift Ű�� Caps Lock Ű�� ���յ� ASCII �ڵ�
	BYTE bCombinedCode;
} KEYMAPPINGENTRY;

typedef struct kKeycodeAsciiMappingEntryStruct
{	
	unsigned int sdlKeycode;
	BYTE bNormalCode;
	BYTE bCombinedCode;
} KEYCODEASCIIMAPPINGENTRY;

// Ű������ ���¸� �����ϴ� �ڷᱸ��
typedef struct tag_KEYBOARDSTATE
{
	// �ڷᱸ�� ����ȭ�� ���� ���ɶ�
	//SPINLOCK stSpinLock;

	// ���� Ű ����
	bool bShiftDown;
	bool bCapsLockOn;
	bool bNumLockOn;
	bool bScrollLockOn;

	// Ȯ�� Ű�� ó���ϱ� ���� ����
	bool bExtendedCodeIn;
	int iSkipCountForPause;
} KEYBOARDSTATE;

typedef struct tag_LinearBufferInfo
{
	unsigned long* pBuffer;
	unsigned long width;
	unsigned long height;
	unsigned long depth;
	unsigned type;
	bool isDirectVideoBuffer;

} LinearBufferInfo;

#pragma pack( pop )

class I_GUIEngine
{
public:
	virtual bool Initialize() = 0;
	virtual void Update(float deltaTime) = 0;
	virtual void SetLinearBuffer(LinearBufferInfo& linearBufferInfo) = 0;
	virtual bool PutKeyboardQueue(KEYDATA* pData) = 0;
	virtual bool PutMouseQueue(MOUSEDATA* pData) = 0;

	LinearBufferInfo m_linearBufferInfo;

};