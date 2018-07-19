/**
 *  file    WindowManager.h
 *  date    2009/10/04
 *  author  kkamagui 
 *          Copyright(c)2008 All rights reserved by kkamagui
 *  brief   ������ �Ŵ����� ���õ� �Լ��� ������ ��� ����
 */

#ifndef __WINDOWMANAGER_H__
#define __WINDOWMANAGER_H__
#include "windef.h"
#include "2DGraphics.h"
////////////////////////////////////////////////////////////////////////////////
//
// ��ũ��
//
////////////////////////////////////////////////////////////////////////////////
// ������ �Ŵ��� �½�ũ�� ó���� �����ͳ� �̺�Ʈ�� �����ϴ� �ִ� ����
#define WINDOWMANAGER_DATAACCUMULATECOUNT    20
// ������ ũ�� ���� ǥ���� ũ��
#define WINDOWMANAGER_RESIZEMARKERSIZE       20
// ������ ũ�� ���� ǥ���� ����
#define WINDOWMANAGER_COLOR_RESIZEMARKER    RGB( 210, 20, 20 )
// ������ ũ�� ���� ǥ���� �β�
#define WINDOWMANAGER_THICK_RESIZEMARKER    4

////////////////////////////////////////////////////////////////////////////////
//
// �Լ�
//
////////////////////////////////////////////////////////////////////////////////
void kStartWindowManager( void );
bool kProcessMouseData( void );
bool kProcessKeyData( void );
bool kProcessEventQueueData( void );
void kUpdate(bool isDirectBuffer);

void kDrawResizeMarker( const RECT* pstArea, bool bShowMarker );

#endif /*__WINDOWMANAGER_H__*/
