/**
 *  file    Window.h
 *  date    2009/09/28
 *  author  kkamagui
 *          Copyright(c)2008 All rights reserved by kkamagui
 *  brief   GUI �ý��ۿ� ���õ� �Լ��� ������ ��� ����
 */

#ifndef __WINDOW_H__
#define __WINDOW_H__

#include "windef.h"
#include "2DGraphics.h"
#include "List.h"
#include "Queue.h"
#include "Keyboard.h"

////////////////////////////////////////////////////////////////////////////////
//
// ��ũ��
//
////////////////////////////////////////////////////////////////////////////////
// �����츦 ������ �� �ִ� �ִ� ����
#define WINDOW_MAXCOUNT             2048
// ������ ID�� ������ Ǯ ���� �������� ����ϴ� ��ũ��
// ���� 32��Ʈ�� Ǯ ���� �������� ��Ÿ��
#define GETWINDOWOFFSET( x )        ( ( x ) & 0xFFFFFFFF )
// ������ ������ �ִ� ����
#define WINDOW_TITLEMAXLENGTH       40
// ��ȿ���� ���� ������ ID
#define WINDOW_INVALIDID            0xFFFFFFFFFFFFFFFF

// �������� �Ӽ�
// �����츦 ȭ�鿡 ��Ÿ��
#define WINDOW_FLAGS_SHOW               0x00000001
// ������ �׵θ� �׸�
#define WINDOW_FLAGS_DRAWFRAME          0x00000002
// ������ ���� ǥ���� �׸�
#define WINDOW_FLAGS_DRAWTITLE          0x00000004
// ������ ũ�� ���� ��ư�� �׸�
#define WINDOW_FLAGS_RESIZABLE          0x00000008
// ������ �⺻ �Ӽ�, ���� ǥ���ٰ� �������� ��� �׸��� ȭ�鿡 �����츦 ���̰� ����
#define WINDOW_FLAGS_DEFAULT        ( WINDOW_FLAGS_SHOW | WINDOW_FLAGS_DRAWFRAME | \
                                      WINDOW_FLAGS_DRAWTITLE )

// ���� ǥ������ ����
#define WINDOW_TITLEBAR_HEIGHT      21
// �������� �ݱ� ��ư�� ũ��
#define WINDOW_XBUTTON_SIZE         19
// �������� �ּ� �ʺ�, ��ư 2���� �ʺ� 30�ȼ��� ���� ���� Ȯ��
#define WINDOW_WIDTH_MIN            ( WINDOW_XBUTTON_SIZE * 2 + 30 )
// �������� �ּ� ����, ���� ǥ������ ���̿� 30�ȼ��� ���� ���� Ȯ��
#define WINDOW_HEIGHT_MIN           ( WINDOW_TITLEBAR_HEIGHT + 30 )

// �������� ����
#define WINDOW_COLOR_FRAME                      RGB( 109, 218, 22 )
#define WINDOW_COLOR_BACKGROUND                 RGB( 255, 255, 255 )
#define WINDOW_COLOR_TITLEBARTEXT               RGB( 255, 255, 255 )
#define WINDOW_COLOR_TITLEBARACTIVEBACKGROUND   RGB( 79, 204, 11 )
#define WINDOW_COLOR_TITLEBARINACTIVEBACKGROUND RGB( 55, 135, 11 )
#define WINDOW_COLOR_TITLEBARBRIGHT1            RGB( 183, 249, 171 )
#define WINDOW_COLOR_TITLEBARBRIGHT2            RGB( 150, 210, 140 )
#define WINDOW_COLOR_TITLEBARUNDERLINE          RGB( 46, 59, 30 )
#define WINDOW_COLOR_BUTTONBRIGHT               RGB( 229, 229, 229 )
#define WINDOW_COLOR_BUTTONDARK                 RGB( 86, 86, 86 )
#define WINDOW_COLOR_SYSTEMBACKGROUND           RGB( 232, 255, 232 )
#define WINDOW_COLOR_XBUTTONLINECOLOR           RGB( 71, 199, 21 )

// ��� �������� ����
#define WINDOW_BACKGROUNDWINDOWTITLE            "SYS_BACKGROUND"


// ���콺 Ŀ���� �ʺ�� ����
#define MOUSE_CURSOR_WIDTH                  20
#define MOUSE_CURSOR_HEIGHT                 20

// Ŀ�� �̹����� ����
#define MOUSE_CURSOR_OUTERLINE              RGB(0, 0, 0 )
#define MOUSE_CURSOR_OUTER                  RGB( 79, 204, 11 )
#define MOUSE_CURSOR_INNER                  RGB( 232, 255, 232 )

// �̺�Ʈ ť�� ũ��
#define EVENTQUEUE_WINDOWMAXCOUNT           100
#define EVENTQUEUE_WNIDOWMANAGERMAXCOUNT    WINDOW_MAXCOUNT

// ������� ������ �Ŵ��� �½�ũ ���̿��� ���޵Ǵ� �̺�Ʈ�� ����
// ���콺 �̺�Ʈ
#define EVENT_UNKNOWN                                   0
#define EVENT_MOUSE_MOVE                                1
#define EVENT_MOUSE_LBUTTONDOWN                         2
#define EVENT_MOUSE_LBUTTONUP                           3
#define EVENT_MOUSE_RBUTTONDOWN                         4
#define EVENT_MOUSE_RBUTTONUP                           5
#define EVENT_MOUSE_MBUTTONDOWN                         6
#define EVENT_MOUSE_MBUTTONUP                           7
// ������ �̺�Ʈ
#define EVENT_WINDOW_SELECT                             8
#define EVENT_WINDOW_DESELECT                           9
#define EVENT_WINDOW_MOVE                               10
#define EVENT_WINDOW_RESIZE                             11
#define EVENT_WINDOW_CLOSE                              12
// Ű �̺�Ʈ
#define EVENT_KEY_DOWN                                  13
#define EVENT_KEY_UP                                    14
// ȭ�� ������Ʈ �̺�Ʈ
#define EVENT_WINDOWMANAGER_UPDATESCREENBYID            15
#define EVENT_WINDOWMANAGER_UPDATESCREENBYWINDOWAREA    16
#define EVENT_WINDOWMANAGER_UPDATESCREENBYSCREENAREA    17

// ȭ�鿡 ������Ʈ�� �� ������ ������Ʈ�� ������ �����ص� ����
#define WINDOW_OVERLAPPEDAREALOGMAXCOUNT                20

////////////////////////////////////////////////////////////////////////////////
//
// ����ü
//
////////////////////////////////////////////////////////////////////////////////
// ���콺 �̺�Ʈ �ڷᱸ��
typedef struct kMouseEventStruct
{
    // ������ ID
    QWORD qwWindowID;

    // ���콺 X,Y��ǥ�� ��ư�� ����
    POINT stPoint;
    BYTE bButtonStatus;
} MOUSEEVENT;

// Ű �̺�Ʈ �ڷᱸ��
typedef struct kKeyEventStruct
{
    // ������ ID
    QWORD qwWindowID;
    
    // Ű�� ASCII �ڵ�� ��ĵ �ڵ�
    BYTE bASCIICode;
    BYTE bScanCode;    
    
    // Ű �÷���
    BYTE bFlags;
} KEYEVENT;

// ������ �̺�Ʈ �ڷᱸ��
typedef struct kWindowEventStruct
{
    // ������ ID
    QWORD qwWindowID;
    
    // ���� ����
    RECT stArea;
} WINDOWEVENT;

// �̺�Ʈ �ڷᱸ��
typedef struct kEventStruct
{
    // �̺�Ʈ Ÿ��
    QWORD qwType;
    
    // �̺�Ʈ ������ ������ ������ ����ü
    union
    {
        // ���콺 �̺�Ʈ ���� ������
        MOUSEEVENT stMouseEvent;

        // Ű �̺�Ʈ ���� ������
        KEYEVENT stKeyEvent;

        // ������ �̺�Ʈ ���� ������
        WINDOWEVENT stWindowEvent;

        // ���� �̺�Ʈ �ܿ� ���� �̺�Ʈ�� ���� ������
        QWORD vqwData[ 3 ];
    };
} EVENT;

// �������� ������ �����ϴ� �ڷᱸ��
typedef struct kWindowStruct
{
    // ���� �������� ��ġ�� ���� �������� ID
    LISTLINK stLink;

    // �ڷᱸ�� ����ȭ�� ���� ���ؽ�
    //MUTEX stLock;

    // ������ ���� ����
    RECT stArea;

    // �������� ȭ�� ���� ��巹��
    COLOR* pstWindowBuffer;

    // �����츦 ������ �ִ� �½�ũ�� ID
    QWORD qwTaskID;

    // ������ �Ӽ�
    DWORD dwFlags;

    // �̺�Ʈ ť�� ť���� ����� ����
    QUEUE stEventQueue;
    EVENT* pstEventBuffer;
    
    // ������ ����
    char vcWindowTitle[ WINDOW_TITLEMAXLENGTH + 1 ];
} WINDOW;

// ������ Ǯ�� ���¸� �����ϴ� �ڷᱸ��
typedef struct kWindowPoolManagerStruct
{
    // �ڷᱸ�� ����ȭ�� ���� ���ؽ�
    //MUTEX stLock;

    // ������ Ǯ�� ���� ����
    WINDOW* pstStartAddress;
    int iMaxCount;
    int iUseCount;

    // �����찡 �Ҵ�� Ƚ��
    int iAllocatedCount;
} WINDOWPOOLMANAGER;

// ������ �Ŵ��� �ڷᱸ��
typedef struct kWindowManagerStruct
{
    // �ڷᱸ�� ����ȭ�� ���� ���ؽ�
   // MUTEX stLock;

    // ������ ����Ʈ
    LIST stWindowList;

    // ���� ���콺 Ŀ���� X, Y��ǥ
    int iMouseX;
    int iMouseY;

    // ȭ�� ���� ����
    RECT stScreenArea;

    // ���� �޸��� ��巹��
    COLOR* pstVideoMemory;

    // ��� �������� ID
    QWORD qwBackgoundWindowID;
    
    // �̺�Ʈ ť�� ť���� ����� ����
    QUEUE stEventQueue;
    EVENT* pstEventBuffer;

    // ���콺 ��ư�� ���� ����
    BYTE bPreviousButtonStatus;
    
    // �̵� ���� �������� ID�� ������ �̵� ���
    QWORD qwMovingWindowID;
	bool bWindowMoveMode;
    
    // ������ ũ�� ���� ���� ũ�� ���� ���� �������� ID, ���� ���� �������� ����
    bool bWindowResizeMode;    
    QWORD qwResizingWindowID;
    RECT stResizingWindowArea;
    
    // ȭ�� ������Ʈ�� ��Ʈ�� ������ ��巹��
    BYTE* pbDrawBitmap;
} WINDOWMANAGER;


// ȭ�鿡 ������Ʈ�� ������ ��Ʈ�� ������ �����ϴ� �ڷᱸ��
typedef struct kDrawBitmapStruct
{
    // ������Ʈ�� ȭ�� ����
    RECT stArea;
    // ȭ�� ������ ������ ����� ��Ʈ���� ��巹��
    BYTE* pbBitmap;
} DRAWBITMAP;

////////////////////////////////////////////////////////////////////////////////
//
// �Լ�
//
////////////////////////////////////////////////////////////////////////////////
// ������ Ǯ ����
static void kInitializeWindowPool( void );
static WINDOW* kAllocateWindow( void );
static void kFreeWindow( QWORD qwID );

// ������� ������ �Ŵ��� ����
void kInitializeGUISystem( void );
WINDOWMANAGER* kGetWindowManager( void );
QWORD kGetBackgroundWindowID( void );
void kGetScreenArea( RECT* pstScreenArea );
QWORD kCreateWindow( int iX, int iY, int iWidth, int iHeight, DWORD dwFlags,
        const char* pcTitle );
bool kDeleteWindow( QWORD qwWindowID );
bool kDeleteAllWindowInTaskID( QWORD qwTaskID );
WINDOW* kGetWindow( QWORD qwWindowID );
WINDOW* kGetWindowWithWindowLock( QWORD qwWindowID );
bool kShowWindow( QWORD qwWindowID, bool bShow );
bool kRedrawWindowByArea( const RECT* pstArea, QWORD qwDrawWindowID );
static void kCopyWindowBufferToFrameBuffer( const WINDOW* pstWindow,
        DRAWBITMAP* pstDrawBitmap );
QWORD kFindWindowByPoint( int iX, int iY );
QWORD kFindWindowByTitle( const char* pcTitle );
bool kIsWindowExist( QWORD qwWindowID );
QWORD kGetTopWindowID( void );
bool kMoveWindowToTop( QWORD qwWindowID );
bool kIsInTitleBar( QWORD qwWindowID, int iX, int iY );
bool kIsInCloseButton( QWORD qwWindowID, int iX, int iY );
bool kMoveWindow( QWORD qwWindowID, int iX, int iY );
static bool kUpdateWindowTitle( QWORD qwWindowID, bool bSelectedTitle );
bool kResizeWindow( QWORD qwWindowID, int iX, int iY, int iWidth, int iHeight );
bool kIsInResizeButton(QWORD qwWindowID, int iX, int iY);

// ��ǥ ��ȯ ����
bool kGetWindowArea( QWORD qwWindowID, RECT* pstArea );
bool kConvertPointScreenToClient( QWORD qwWindowID, const POINT* pstXY,
        POINT* pstXYInWindow );
bool kConvertPointClientToScreen( QWORD qwWindowID, const POINT* pstXY,
        POINT* pstXYInScreen );
bool kConvertRectScreenToClient( QWORD qwWindowID, const RECT* pstArea,
        RECT* pstAreaInWindow );
bool kConvertRectClientToScreen( QWORD qwWindowID, const RECT* pstArea,
        RECT* pstAreaInScreen );

// ȭ�� ������Ʈ ����
bool kUpdateScreenByID( QWORD qwWindowID );
bool kUpdateScreenByWindowArea( QWORD qwWindowID, const RECT* pstArea );
bool kUpdateScreenByScreenArea( const RECT* pstArea );

//  �̺�Ʈ ť ����
bool kSendEventToWindow( QWORD qwWindowID, const EVENT* pstEvent );
bool kReceiveEventFromWindowQueue( QWORD qwWindowID, EVENT* pstEvent );
bool kSendEventToWindowManager( const EVENT* pstEvent );
bool kReceiveEventFromWindowManagerQueue( EVENT* pstEvent );
bool kSetMouseEvent( QWORD qwWindowID, QWORD qwEventType, int iMouseX, int iMouseY,
        BYTE bButtonStatus, EVENT* pstEvent );
bool kSetWindowEvent( QWORD qwWindowID, QWORD qwEventType, EVENT* pstEvent );
void kSetKeyEvent( QWORD qwWindow, const KEYDATA* pstKeyData, EVENT* pstEvent );

// ������ ���ο� �׸��� �Լ��� ���콺 Ŀ�� ����
bool kDrawWindowFrame( QWORD qwWindowID );
bool kDrawWindowBackground( QWORD qwWindowID );
bool kDrawWindowTitle( QWORD qwWindowID, const char* pcTitle, bool bSelectedTitle );
bool kDrawButton( QWORD qwWindowID, RECT* pstButtonArea, COLOR stBackgroundColor,
        const char* pcText, COLOR stTextColor );
bool kDrawPixel( QWORD qwWindowID, int iX, int iY, COLOR stColor );
bool kDrawLine( QWORD qwWindowID, int iX1, int iY1, int iX2, int iY2, COLOR stColor );
bool kDrawRect( QWORD qwWindowID, int iX1, int iY1, int iX2, int iY2,
        COLOR stColor, bool bFill );
bool kDrawCircle( QWORD qwWindowID, int iX, int iY, int iRadius, COLOR stColor,
	bool bFill );
bool kDrawText( QWORD qwWindowID, int iX, int iY, COLOR stTextColor,
        COLOR stBackgroundColor, const char* pcString, int iLength );
static void kDrawCursor( int iX, int iY );
void kMoveCursor( int iX, int iY );
void kGetCursorPosition( int* piX, int* piY );
bool kBitBlt( QWORD qwWindowID, int iX, int iY, COLOR* pstBuffer, int iWidth,
        int iHeight );
void kDrawBackgroundImage( void );

//  ȭ�� ������Ʈ�� ����ϴ� ȭ�� ������Ʈ ��Ʈ�� ����
bool kCreateDrawBitmap( const RECT* pstArea, DRAWBITMAP* pstDrawBitmap );
static bool kFillDrawBitmap( DRAWBITMAP* pstDrawBitmap, RECT* pstArea, bool bFill );
inline bool kGetStartPositionInDrawBitmap( const DRAWBITMAP* pstDrawBitmap,
        int iX, int iY, int* piByteOffset, int* piBitOffset );
inline bool kIsDrawBitmapAllOff( const DRAWBITMAP* pstDrawBitmap );
#endif /*__WINDOW_H__*/
