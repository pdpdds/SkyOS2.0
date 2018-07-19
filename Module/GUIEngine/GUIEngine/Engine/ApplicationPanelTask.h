/**
 *  file    ApplicationPanelTask.h
 *  date    2009/11/03
 *  author  kkamagui 
 *          Copyright(c)2008 All rights reserved by kkamagui 
 *  brief   ���ø����̼� �гο� ���õ� ��� ����
 */

#ifndef __APPLICATIONPANELTASK_H__
#define __APPLICATIONPANELTASK_H__

#include "windef.h"
#include "Window.h"
#include "MintFont.h"


////////////////////////////////////////////////////////////////////////////////
//
// ��ũ��
//
////////////////////////////////////////////////////////////////////////////////
// ���ø����̼� �г� �������� ����
#define APPLICATIONPANEL_HEIGHT             31
// ���ø����̼� �г� �������� ����
#define APPLICATIONPANEL_TITLE              "SYS_APPLICATIONPANNEL"
// ���ø����̼� �г� �����쿡 ǥ���� �ð��� �ʺ�
// 09:00 AM ���·� ǥ���ϹǷ� 8 * ��Ʈ�� �ʺ�� ���
#define APPLICATIONPANEL_CLOCKWIDTH         ( 8 * FONT_ENGLISHWIDTH )

// ���ø����̼� ����Ʈ �����쿡 ��Ÿ�� �������� ����
#define APPLICATIONPANEL_LISTITEMHEIGHT     ( FONT_ENGLISHHEIGHT + 4 )
// ���ø����̼� ����Ʈ �������� ����
#define APPLICATIONPANEL_LISTTITLE          "SYS_APPLICATIONLIST"

// ���ø����̼� �гο��� ����ϴ� ����
#define APPLICATIONPANEL_COLOR_OUTERLINE     RGB( 109, 218, 22 )
#define APPLICATIONPANEL_COLOR_MIDDLELINE    RGB( 183, 249, 171 )
#define APPLICATIONPANEL_COLOR_INNERLINE     RGB( 150, 210, 140 )
#define APPLICATIONPANEL_COLOR_BACKGROUND    RGB( 55, 135, 11 )
#define APPLICATIONPANEL_COLOR_ACTIVE        RGB( 79, 204, 11 )

////////////////////////////////////////////////////////////////////////////////
//
// ����ü
//
////////////////////////////////////////////////////////////////////////////////
// ���ø����̼� �г��� ����ϴ� ������ �����ϴ� �ڷᱸ��
typedef struct kApplicationPanelDataStruct
{
    // ���ø����̼� �г� �������� ID
    QWORD qwApplicationPanelID;
    
    // ���ø����̼� ����Ʈ �������� ID
    QWORD qwApplicationListID;
    
    // ���ø����̼� �г��� ��ư ��ġ
    RECT stButtonArea;
    
    // ���ø����̼� ����Ʈ �������� �ʺ�
    int iApplicationListWidth;
    
    // ���ø����̼� ����Ʈ �����쿡�� ������ ���콺�� ��ġ�� �������� �ε���
    int iPreviousMouseOverIndex;

    // ���ø����̼� ����Ʈ �����찡 ȭ�鿡 ǥ�õǾ����� ����
	bool bApplicationWindowVisible;
} APPLICATIONPANELDATA;

// GUI �½�ũ�� ������ �����ϴ� �ڷᱸ��
typedef struct kApplicationEntryStruct
{   
    // GUI �½�ũ�� �̸�
    char* pcApplicationName;

    // GUI �½�ũ�� ��Ʈ�� ����Ʈ   
	DWORD (WINAPI *pvEntryPoint)(LPVOID parameter);	
} APPLICATIONENTRY;

////////////////////////////////////////////////////////////////////////////////
//
// �Լ�
//
////////////////////////////////////////////////////////////////////////////////
DWORD WINAPI kApplicationPanelGUITask(LPVOID parameter);

static void kDrawClockInApplicationPanel( QWORD qwApplicationPanelID );
bool kCreateApplicationPanelWindow( void );
static void kDrawDigitalClock( QWORD qwApplicationPanelID );
bool kCreateApplicationListWindow( void );
static void kDrawApplicationListItem( int iIndex, bool bSelected );
static bool kProcessApplicationPanelWindowEvent( void );
static bool kProcessApplicationListWindowEvent( void );
static int kGetMouseOverItemIndex( int iMouseY );

#endif /*__APPLICATIONPANELTASK_H__*/
