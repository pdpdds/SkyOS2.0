#pragma once
#include "windef.h"
#include "I_VirtualIO.h"

// 키 큐에 대한 매크로
// 키 큐의 최대 크기
#define KEY_MAXQUEUECOUNT	100

#pragma pack( push, 1 )

////////////////////////////////////////////////////////////////////////////////
// 마우스 큐에 대한 매크로
#define MOUSE_MAXQUEUECOUNT 100

// 버튼의 상태를 나타내는 매크로
#define MOUSE_LBUTTONDOWN   0x01
#define MOUSE_RBUTTONDOWN   0x02
#define MOUSE_MBUTTONDOWN   0x04

// 스캔 코드 테이블을 구성하는 항목
typedef struct kKeyMappingEntryStruct
{
	// Shift 키나 Caps Lock 키와 조합되지 않는 ASCII 코드
	BYTE bNormalCode;

	// Shift 키나 Caps Lock 키와 조합된 ASCII 코드
	BYTE bCombinedCode;
} KEYMAPPINGENTRY;

typedef struct kKeycodeAsciiMappingEntryStruct
{	
	unsigned int sdlKeycode;
	BYTE bNormalCode;
	BYTE bCombinedCode;
} KEYCODEASCIIMAPPINGENTRY;

// 키보드의 상태를 관리하는 자료구조
typedef struct tag_KEYBOARDSTATE
{
	// 자료구조 동기화를 위한 스핀락
	//SPINLOCK stSpinLock;

	// 조합 키 정보
	bool bShiftDown;
	bool bCapsLockOn;
	bool bNumLockOn;
	bool bScrollLockOn;

	// 확장 키를 처리하기 위한 정보
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