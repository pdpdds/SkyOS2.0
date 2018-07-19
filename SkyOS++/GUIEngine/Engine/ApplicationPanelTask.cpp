/**
 *  file    ApplicationPanelTask.c
 *  date    2009/11/03
 *  author  kkamagui 
 *          Copyright(c)2008 All rights reserved by kkamagui 
 *  brief   ���ø����̼� �гο� ���õ� �ҽ� ����
 */

#include "ApplicationPanelTask.h"
#include "string.h"
//#include "RTC.h"
//#include "Task.h"
#include "GUITask.h"
#include "SkyInterface.h"

extern SKY_PROCESS_INTERFACE g_processInterface;

// ���ø����̼� ���̺�
APPLICATIONENTRY gs_vstApplicationTable[] =
{
        { "Base GUI Task", kBaseGUITask },
        { "Hello World GUI Task",  kHelloWorldGUITask },
        //{ "System Monitor Task", kSystemMonitorTask },
       // { "Console Shell for GUI", kGUIConsoleShellTask },
        { "Image Viewer Task", kImageViewerTask },
};

// ���ø����̼� �гο��� ����ϴ� �ڷᱸ��
APPLICATIONPANELDATA gs_stApplicationPanelData;

/**
 *  ���ø����̼� �г� �½�ũ
 */
DWORD WINAPI kApplicationPanelGUITask(LPVOID parameter)
{
    EVENT stReceivedEvent;
	bool bApplicationPanelEventResult;
	bool bApplicationListEventResult;    

    //--------------------------------------------------------------------------
    // �����츦 ����
    //--------------------------------------------------------------------------
    // ���ø����̼� �г� ������� �������α׷� ����Ʈ �����츦 ����
    if( ( kCreateApplicationPanelWindow() == FALSE ) ||
        ( kCreateApplicationListWindow() == FALSE ) )
    {
        return 0;
    }

    //--------------------------------------------------------------------------
    // GUI �½�ũ�� �̺�Ʈ ó�� ����
    //--------------------------------------------------------------------------
    while( 1 )
    {
        // ������ �̺�Ʈ�� ó��
        bApplicationPanelEventResult = kProcessApplicationPanelWindowEvent();
        bApplicationListEventResult = kProcessApplicationListWindowEvent();
        
        // ó���� �̺�Ʈ�� ������ ���μ����� ��ȯ
        if( ( bApplicationPanelEventResult == FALSE ) &&
            ( bApplicationListEventResult == FALSE ) )
        {
			int j = 1;
			//20180628
            ksleep( 0 );
        }            
    }

	return 1;
}

/**
 *  ���ø����̼� �г� �����츦 ����
 */
bool kCreateApplicationPanelWindow( void )
{
    WINDOWMANAGER* pstWindowManager;
    QWORD qwWindowID;
    
    // ������ �Ŵ����� ��ȯ
    pstWindowManager = kGetWindowManager();
    
    // ȭ�� ���ʿ� ���ø����̼� �г� �����츦 ����, ���η� ���� ������ ����
    qwWindowID = kCreateWindow( 0, 0, pstWindowManager->stScreenArea.iX2 + 1,
        APPLICATIONPANEL_HEIGHT, NULL, APPLICATIONPANEL_TITLE );
    // �����츦 �������� �������� ����
    if( qwWindowID == WINDOW_INVALIDID )
    {
        return FALSE;
    }
    
    // ���ø����̼� �г� �������� �׵θ��� ���θ� ǥ��
    kDrawRect( qwWindowID, 0, 0, pstWindowManager->stScreenArea.iX2, 
            APPLICATIONPANEL_HEIGHT - 1, APPLICATIONPANEL_COLOR_OUTERLINE, FALSE );   
    kDrawRect( qwWindowID, 1, 1, pstWindowManager->stScreenArea.iX2 - 1, 
            APPLICATIONPANEL_HEIGHT - 2, APPLICATIONPANEL_COLOR_MIDDLELINE, FALSE );   
    kDrawRect( qwWindowID, 2, 2, pstWindowManager->stScreenArea.iX2 - 2, 
            APPLICATIONPANEL_HEIGHT - 3, APPLICATIONPANEL_COLOR_INNERLINE, FALSE );  
    kDrawRect( qwWindowID, 3, 3, pstWindowManager->stScreenArea.iX2 - 3, 
            APPLICATIONPANEL_HEIGHT - 4, APPLICATIONPANEL_COLOR_BACKGROUND, TRUE );
    
    // ���ø����̼� �г��� ���ʿ� GUI �½�ũ�� ����Ʈ�� �����ִ� ��ư�� ǥ��
    kSetRectangleData( 5, 5, 120, 25, &( gs_stApplicationPanelData.stButtonArea ) );
    kDrawButton( qwWindowID, &( gs_stApplicationPanelData.stButtonArea ), 
                 APPLICATIONPANEL_COLOR_ACTIVE, "Application", RGB( 255, 255, 255 ) );
    
    // ���ø����̼� �г� �������� �����ʿ� �ð踦 ǥ��
    kDrawDigitalClock( qwWindowID );
    
    // ���ø����̼� �г��� ȭ�鿡 ǥ��
    kShowWindow( qwWindowID, TRUE );    
    
    // ���ø����̼� �г� �ڷᱸ���� ������ ID ����
    gs_stApplicationPanelData.qwApplicationPanelID = qwWindowID;
    
    return TRUE;
}

/**
 *  ���ø����̼� �гο� �ð踦 ǥ��
 */
static void kDrawDigitalClock( QWORD qwWindowID )
{
    RECT stWindowArea;
    RECT stUpdateArea;
    static BYTE s_bPreviousHour, s_bPreviousMinute, s_bPreviousSecond;
    BYTE bHour, bMinute, bSecond;
    char vcBuffer[ 10 ] = "00:00 AM";

    // ���� �ð��� RTC���� ��ȯ
	//20180628
    //kReadRTCTime( &bHour, &bMinute, &bSecond );
    
    // ���� �ð��� ��ȭ�� ������ �ð踦 ǥ���� �ʿ� ����
    if( ( s_bPreviousHour == bHour ) && ( s_bPreviousMinute == bMinute ) &&
        ( s_bPreviousSecond == bSecond ) )
    {
        return ;
    }
    
    // ���� �񱳸� ���� ��, ��, �ʸ� ������Ʈ
    s_bPreviousHour = bHour;
    s_bPreviousMinute = bMinute;
    s_bPreviousSecond = bSecond;

    // �ð��� 12�ð� ������ PM���� ����
    if( bHour >= 12 )
    {
        if( bHour > 12 )
        {
            bHour -= 12;
        }
        vcBuffer[ 6 ] = 'P';
    }
    
    // �ð� ����
    vcBuffer[ 0 ] = '0' + bHour / 10;
    vcBuffer[ 1 ] = '0' + bHour % 10;
    // �� ����
    vcBuffer[ 3 ] = '0' + bMinute / 10;
    vcBuffer[ 4 ] = '0' + bMinute % 10;
    
    // �ʿ� ���� ��� :�� ������
    if( ( bSecond % 2 ) == 1 )
    {
        vcBuffer[ 2 ] = ' ';
    }
    else
    {
        vcBuffer[ 2 ] = ':';
    }
    
    // ���ø����̼� �г� �������� ��ġ�� ��ȯ
    kGetWindowArea( qwWindowID, &stWindowArea );
    
    // �ð� ������ �׵θ��� ǥ��
    kSetRectangleData( stWindowArea.iX2 - APPLICATIONPANEL_CLOCKWIDTH - 13, 5,
                       stWindowArea.iX2 - 5, 25, &stUpdateArea );    
    kDrawRect( qwWindowID, stUpdateArea.iX1, stUpdateArea.iY1, 
            stUpdateArea.iX2, stUpdateArea.iY2, APPLICATIONPANEL_COLOR_INNERLINE, 
            FALSE );
    
    // �ð踦 ǥ��
    kDrawText( qwWindowID, stUpdateArea.iX1 + 4, stUpdateArea.iY1 + 3, 
            RGB( 255, 255, 255 ), APPLICATIONPANEL_COLOR_BACKGROUND, vcBuffer, 
            strlen( vcBuffer ) );
    
    // �ð谡 �׷��� ������ ȭ�鿡 ������Ʈ
    kUpdateScreenByWindowArea( qwWindowID, &stUpdateArea );
}

/**
 *  ���ø����̼� �гο� ���ŵ� �̺�Ʈ�� ó��
 */
static bool kProcessApplicationPanelWindowEvent( void )
{
    EVENT stReceivedEvent;
    MOUSEEVENT* pstMouseEvent;
    bool bProcessResult;
    QWORD qwApplicationPanelID;
    QWORD qwApplicationListID;

    // ������ ID ����
    qwApplicationPanelID = gs_stApplicationPanelData.qwApplicationPanelID;
    qwApplicationListID = gs_stApplicationPanelData.qwApplicationListID;
    bProcessResult = FALSE;
    
    // �̺�Ʈ�� ó���ϴ� ����
    while( 1 )
    {
        // ���ø����̼� �г� �������� �����ʿ� �ð踦 ǥ��
        kDrawDigitalClock( gs_stApplicationPanelData.qwApplicationPanelID );
        
        // �̺�Ʈ ť���� �̺�Ʈ�� ����
        if( kReceiveEventFromWindowQueue( qwApplicationPanelID, &stReceivedEvent ) 
                == FALSE )
        {
            break;
        }

        bProcessResult = TRUE;
        
        // ���ŵ� �̺�Ʈ�� Ÿ�Կ� ���� ������ ó��
        switch( stReceivedEvent.qwType )
        {
            // ���콺 ���� ��ư ó��
        case EVENT_MOUSE_LBUTTONDOWN:
            pstMouseEvent = &( stReceivedEvent.stMouseEvent );
            // ���콺 ���� ��ư�� ���ø����̼� �г��� ��ư ���ο��� ��������
            // ���ø����̼� ����Ʈ �����츦 ǥ��
            if( kIsInRectangle( &( gs_stApplicationPanelData.stButtonArea ), 
                    pstMouseEvent->stPoint.iX, pstMouseEvent->stPoint.iY ) == FALSE )
            {
                break;
            }
            
            // ��ư�� ������ ���¿��� ������ ���
            if( gs_stApplicationPanelData.bApplicationWindowVisible == FALSE )
            {
                // ��ư�� ���� ���·� ǥ��
                kDrawButton( qwApplicationPanelID, &( gs_stApplicationPanelData.stButtonArea ), 
                             APPLICATIONPANEL_COLOR_BACKGROUND, "Application", 
                             RGB( 255, 255, 255 ) );
                // ��ư�� �ִ� ������ ȭ�� ������Ʈ
                kUpdateScreenByWindowArea( qwApplicationPanelID, 
                        &( gs_stApplicationPanelData.stButtonArea ) );
        
                // ���ø����̼� ����Ʈ �����쿡 �ƹ��͵� ���õ��� ���� ������ �ʱ�ȭ�ϰ� 
                // �����츦 ȭ�鿡 �ֻ����� ǥ��
                if( gs_stApplicationPanelData.iPreviousMouseOverIndex != -1 )
                {
                    kDrawApplicationListItem( 
                        gs_stApplicationPanelData.iPreviousMouseOverIndex, FALSE );
                    gs_stApplicationPanelData.iPreviousMouseOverIndex = -1;
                }
                kMoveWindowToTop( gs_stApplicationPanelData.qwApplicationListID );
                kShowWindow( gs_stApplicationPanelData.qwApplicationListID, TRUE );
                // �÷��״� ȭ�鿡 ǥ�õ� ������ ����
                gs_stApplicationPanelData.bApplicationWindowVisible = TRUE;
            }
            // ��ư�� ���� ���¿��� ������ ���
            else
            {
                // ���ø����̼� �г��� ��ư�� ������ ���·� ǥ��
                kDrawButton( qwApplicationPanelID, 
                         &( gs_stApplicationPanelData.stButtonArea ), 
                         APPLICATIONPANEL_COLOR_ACTIVE, "Application", 
                         RGB( 255, 255, 255 ) );
                // ��ư�� �ִ� ������ ȭ�� ������Ʈ
                kUpdateScreenByWindowArea( qwApplicationPanelID, 
                         &( gs_stApplicationPanelData.stButtonArea ) );

                // ���ø����̼� ����Ʈ �����츦 ����
                kShowWindow( qwApplicationListID, FALSE );
                // �÷��״� ȭ�鿡 ǥ�õ��� ���� ������ ����
                gs_stApplicationPanelData.bApplicationWindowVisible = FALSE;            
            }
            break;
            
            // �� �� �̺�Ʈ ó��
        default:
            break;
        }
    }
    
    return bProcessResult;
}

/**
 *  ���ø����̼� ����Ʈ �����츦 ����
 */
bool kCreateApplicationListWindow( void )
{
    int i;
    int iCount;
    int iMaxNameLength;
    int iNameLength;
    QWORD qwWindowID;
    int iX;
    int iY;
    int iWindowWidth;
    
    // ���ø����̼� ���̺� ���ǵ� �̸� �߿��� ���� �� ���� �˻�
    iMaxNameLength = 0;
    iCount = sizeof( gs_vstApplicationTable ) / sizeof( APPLICATIONENTRY );
    for( i = 0 ; i < iCount ; i++ )
    {
        iNameLength = strlen( gs_vstApplicationTable[ i ].pcApplicationName );
        if( iMaxNameLength < iNameLength )
        {
            iMaxNameLength = iNameLength;
        }
    }
    
    // �������� �ʺ� ���, 20�� �¿� 10�ȼ��� ��������
    iWindowWidth = iMaxNameLength * FONT_ENGLISHWIDTH + 20;
    
    // �������� ��ġ�� ���ø����̼� �г��� ��ư �Ʒ��� ����
    iX = gs_stApplicationPanelData.stButtonArea.iX1;
    iY = gs_stApplicationPanelData.stButtonArea.iY2 + 5;
    
    // �������� ������ �ִ� ���̷� ���ø����̼� ����Ʈ �����츦 ����
    // ���ø����̼� ������� ������ ���� ǥ������ �ʿ� �����Ƿ� �Ӽ��� NULL�� ����
    qwWindowID = kCreateWindow( iX, iY, iWindowWidth, 
        iCount * APPLICATIONPANEL_LISTITEMHEIGHT + 1, NULL, 
        APPLICATIONPANEL_LISTTITLE );
    // �����츦 �������� �������� ����
    if( qwWindowID == WINDOW_INVALIDID )
    {
        return FALSE;
    }
    
    // ���ø����̼� �г� �ڷᱸ���� �������� �ʺ� ����
    gs_stApplicationPanelData.iApplicationListWidth = iWindowWidth;
    
    // ������ �� ���ø����̼� ����Ʈ�� ���ܳ���
    gs_stApplicationPanelData.bApplicationWindowVisible = FALSE;

    // ���ø����̼� �г� �ڷᱸ���� ������ ID�� �����ϰ� ������ ���콺�� ��ġ�� �������� 
    // ���� ������ ����
    gs_stApplicationPanelData.qwApplicationListID = qwWindowID;
    gs_stApplicationPanelData.iPreviousMouseOverIndex = -1;

    // ������ ���ο� �������α׷� �̸��� ������ ǥ��
    for( i = 0 ; i < iCount ; i++ )
    {
        kDrawApplicationListItem( i, FALSE );
    }
    
    kMoveWindow( qwWindowID, gs_stApplicationPanelData.stButtonArea.iX1, 
            gs_stApplicationPanelData.stButtonArea.iY2 + 5 );
    return TRUE;
}

/**
 *  ���ø����̼� ����Ʈ �����쿡 GUI �½�ũ �������� ǥ��
 */
static void kDrawApplicationListItem( int iIndex, bool bMouseOver )
{
    QWORD qwWindowID;
    int iWindowWidth;
    COLOR stColor;
    RECT stItemArea;
    
    // ���ø����̼� ����Ʈ �������� ID�� �ʺ�
    qwWindowID = gs_stApplicationPanelData.qwApplicationListID;
    iWindowWidth = gs_stApplicationPanelData.iApplicationListWidth;
    
    // ���콺�� ���� �ִ��� ���ο� ���� ���� ���� �ٸ��� ǥ��
    if( bMouseOver == TRUE )
    {
        stColor = APPLICATIONPANEL_COLOR_ACTIVE;
    }
    else
    {
        stColor = APPLICATIONPANEL_COLOR_BACKGROUND;        
    }
    
    // ����Ʈ �����ۿ� �׵θ��� ǥ��
    kSetRectangleData( 0, iIndex * APPLICATIONPANEL_LISTITEMHEIGHT,
        iWindowWidth - 1, ( iIndex + 1 ) * APPLICATIONPANEL_LISTITEMHEIGHT,
        &stItemArea );
    kDrawRect( qwWindowID, stItemArea.iX1, stItemArea.iY1, stItemArea.iX2, 
            stItemArea.iY2, APPLICATIONPANEL_COLOR_INNERLINE, FALSE );
    
    // ����Ʈ �������� ���θ� ä��
    kDrawRect( qwWindowID, stItemArea.iX1 + 1, stItemArea.iY1 + 1, 
               stItemArea.iX2 - 1, stItemArea.iY2 - 1, stColor, TRUE );
    
    // GUI �½�ũ�� �̸��� ǥ��
    kDrawText( qwWindowID, stItemArea.iX1 + 10, stItemArea.iY1 + 2, 
            RGB( 255, 255, 255 ), stColor,
            gs_vstApplicationTable[ iIndex ].pcApplicationName,
            strlen( gs_vstApplicationTable[ iIndex ].pcApplicationName ) );    
    
    // ������Ʈ�� �������� ȭ�鿡 ������Ʈ
    kUpdateScreenByWindowArea( qwWindowID, &stItemArea );
}

/**
 *  ���ø����̼� ����Ʈ�� ���ŵ� �̺�Ʈ�� ó��
 */
static bool kProcessApplicationListWindowEvent( void )
{
    EVENT stReceivedEvent;
    MOUSEEVENT* pstMouseEvent;
	bool bProcessResult;
    QWORD qwApplicationPanelID;
    QWORD qwApplicationListID;
    int iMouseOverIndex;
    EVENT stEvent;
    
    // ������ ID ����
    qwApplicationPanelID = gs_stApplicationPanelData.qwApplicationPanelID;
    qwApplicationListID = gs_stApplicationPanelData.qwApplicationListID;
    bProcessResult = FALSE;
    
    // �̺�Ʈ�� ó���ϴ� ����
    while( 1 )
    {
        // �̺�Ʈ ť���� �̺�Ʈ�� ����
        if( kReceiveEventFromWindowQueue( qwApplicationListID, &stReceivedEvent ) 
                == FALSE )
        {
            break;
        }

        bProcessResult = TRUE;
        
        // ���ŵ� �̺�Ʈ�� Ÿ�Կ� ���� ������ ó��
        switch( stReceivedEvent.qwType )
        {
            // ���콺 �̵� ó��
        case EVENT_MOUSE_MOVE:
            pstMouseEvent = &( stReceivedEvent.stMouseEvent );

            // ���콺�� ��ġ�� ������ ���
            iMouseOverIndex = kGetMouseOverItemIndex( pstMouseEvent->stPoint.iY );
            
            // ���� ���콺�� ��ġ�� �����۰� ������ ��ġ�� �������� �ٸ� ���� ����
            if( ( iMouseOverIndex == gs_stApplicationPanelData.iPreviousMouseOverIndex ) ||
                ( iMouseOverIndex == -1 ) )
            {
                break;
            }
            
            // ������ ���콺�� ��ġ�� �������� �⺻ ���·� ǥ��
            if( gs_stApplicationPanelData.iPreviousMouseOverIndex != -1 )
            {
                kDrawApplicationListItem( 
                    gs_stApplicationPanelData.iPreviousMouseOverIndex, FALSE );
            }
            
            // ���� ���콺 Ŀ���� �ִ� ��ġ�� ���콺�� ��ġ�� ���·� ǥ��
            kDrawApplicationListItem( iMouseOverIndex, TRUE );
            
            // ���콺�� ��ġ�� �������� �����ص�
            gs_stApplicationPanelData.iPreviousMouseOverIndex = iMouseOverIndex;            
            break;
            
            // ���콺 ���� ��ư ó��
        case EVENT_MOUSE_LBUTTONDOWN:
            pstMouseEvent = &( stReceivedEvent.stMouseEvent );
            
            // ���� ���콺 Ŀ���� �ִ� ��ġ�� ���õ� ������ ǥ��
            iMouseOverIndex = kGetMouseOverItemIndex( pstMouseEvent->stPoint.iY );
            if( iMouseOverIndex == -1 )
            {
                break;
            }

            // ���õ� �������� ����
			//20180628
            //kCreateTask( TASK_FLAGS_LOW | TASK_FLAGS_THREAD, 0, 0, 
              //      ( QWORD ) gs_vstApplicationTable[ iMouseOverIndex ].pvEntryPoint, 
                //    TASK_LOADBALANCINGID );   

			g_processInterface.sky_kcreate_thread_from_memory(1, gs_vstApplicationTable[iMouseOverIndex].pvEntryPoint, NULL);

            // ���ø����̼� �гο� ���콺 ���� ��ư�� ���ȴٴ� �޽����� �����Ͽ� ó��
            kSetMouseEvent( qwApplicationPanelID, EVENT_MOUSE_LBUTTONDOWN,
                    gs_stApplicationPanelData.stButtonArea.iX1 + 1, 
                    gs_stApplicationPanelData.stButtonArea.iY1 + 1, 
                    NULL, &stEvent );
            kSendEventToWindow( qwApplicationPanelID, &stEvent );            
            break;
            
            // �� �� �̺�Ʈ ó��
        default:
            break;
        }
    }
    
    return bProcessResult;
}

/**
 *  ���콺 Ŀ���� ��ġ�� ���ø����̼� ����Ʈ �������� ������ �ε����� ��ȯ
 */
static int kGetMouseOverItemIndex( int iMouseY )
{
    int iCount;
    int iItemIndex;
    
    // ���ø����̼� ���̺��� �� ������ ��
    iCount = sizeof( gs_vstApplicationTable ) / sizeof( APPLICATIONENTRY );
    
    // ���콺 ��ǥ�� �������� �ε����� ���
    iItemIndex = iMouseY / APPLICATIONPANEL_LISTITEMHEIGHT;
    // ������ ����� -1�� ��ȯ
    if( ( iItemIndex < 0 ) || ( iItemIndex >= iCount ) )
    {
        return -1;
    }
    
    return iItemIndex;
}
