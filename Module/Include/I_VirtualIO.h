#pragma once

#pragma pack( push, 1 )

// 키 큐에 삽입할 데이터 구조체
typedef struct kKeyDataStruct
{
	// 키보드에서 전달된 스캔 코드
	unsigned char bScanCode;
	// 스캔 코드를 변환한 ASCII 코드
	unsigned char bASCIICode;
	// 키 상태를 저장하는 플래그(눌림/떨어짐/확장 키 여부)
	unsigned char bFlags;
} KEYDATA;

// PS/2 마우스 패킷을 저장하는 자료구조, 마우스 큐에 삽입하는 데이터
typedef struct kMousePacketStruct
{
	// 버튼 상태와 X, Y 값에 관련된 플래그
	unsigned char bButtonStatusAndFlag;
	// X축 이동거리
	int bXMovement;
	// Y축 이동거리
	int bYMovement;
	// 상대좌표값인지 절대좌표값인지를 나타내는 플래그
	unsigned char bAbsoluteCoordinate;
} MOUSEDATA;

// 마우스의 상태를 관리하는 자료구조
typedef struct tag_MouseManagerStruct
{
	// 자료구조 동기화를 위한 스핀락
	//    SPINLOCK stSpinLock;    
	// 현재 수신된 데이터의 개수, 마우스 데이터가 3개이므로 0~2의 범위를 계속 반복함
	int iByteCount;
	// 현재 수신 중인 마우스 데이터
	MOUSEDATA stCurrentData;
} MOUSESTATE;

#pragma pack( pop )


class I_VirtualIO
{
public:
	virtual bool PutKeyboardQueue(KEYDATA* pData) = 0;
	virtual bool PutMouseQueue(MOUSEDATA* pData) = 0;
};