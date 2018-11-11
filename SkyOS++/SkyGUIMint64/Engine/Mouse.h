
#pragma once
#include "windef.h"
#include "I_GUIEngine.h"
#include "SkyMockInterface.h"

extern SKY_PROCESS_INTERFACE g_processInterface;
// ���콺 ť�� ���� ��ũ��
#define MOUSE_MAXQUEUECOUNT 100

// ��ư�� ���¸� ��Ÿ���� ��ũ��
#define MOUSE_LBUTTONDOWN   0x01
#define MOUSE_RBUTTONDOWN   0x02
#define MOUSE_MBUTTONDOWN   0x04

// ���콺�� ���¸� �����ϴ� �ڷᱸ��
typedef struct kMouseManagerStruct
{
	// �ڷᱸ�� ����ȭ�� ���� ���ɶ�
	//    SPINLOCK stSpinLock;    
	// ���� ���ŵ� �������� ����, ���콺 �����Ͱ� 3���̹Ƿ� 0~2�� ������ ��� �ݺ���
	int iByteCount;
	// ���� ���� ���� ���콺 ������
	MOUSEDATA stCurrentData;
} MOUSEMANAGER;

#define kEnterCriticalSection()	g_processInterface.sky_enter_critical_section()
#define kLeaveCriticalSection()	g_processInterface.sky_leave_critical_section()

bool kInitializeMouse( void );
bool kAccumulateMouseDataAndPutQueue( BYTE bMouseData );
bool kGetMouseDataFromMouseQueue( BYTE* pbButtonStatus, int* piRelativeX,int* piRelativeY, bool& bAbsoluteCoordinate);

