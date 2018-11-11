#pragma once
#include "windef.h"
#include "2DGraphics.h"
#include "I_GUIEngine.h"

// ������ �Ŵ��� �½�ũ�� ó���� �����ͳ� �̺�Ʈ�� �����ϴ� �ִ� ����
#define WINDOWMANAGER_DATAACCUMULATECOUNT    20
// ������ ũ�� ���� ǥ���� ũ��
#define WINDOWMANAGER_RESIZEMARKERSIZE       20
// ������ ũ�� ���� ǥ���� ����
#define WINDOWMANAGER_COLOR_RESIZEMARKER    RGB( 210, 20, 20 )
// ������ ũ�� ���� ǥ���� �β�
#define WINDOWMANAGER_THICK_RESIZEMARKER    4


void kStartWindowManager(LinearBufferInfo* bufferInfo);
bool kProcessMouseData( void );
bool kProcessKeyData( void );
bool kProcessEventQueueData( void );
void kUpdate(bool isDirectBuffer);

void kDrawResizeMarker( const RECT* pstArea, bool bShowMarker );