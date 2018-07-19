/**
 *  file    GUITask.c
 *  date    2009/10/20
 *  author  kkamagui 
 *          Copyright(c)2008 All rights reserved by kkamagui
 *  brief   GUI �½�ũ�� ���õ� �Լ��� ������ �ҽ� ����
 */
#include "GUITask.h"
#include "Window.h"
#include "memory.h"
#include "string.h"
#include "sprintf.h"
#include "fileio.h"
#include "JPEG.h"
#include "MintFont.h"
//#include "Console.h"
#include "ConsoleShell.h"
#include "SkyInterface.h"
#include "Mouse.h"
#define FILESYSTEM_MAXFILENAMELENGTH        24
//#include "MultiProcessor.h"
//#include "MPConfigurationTable.h"
//------------------------------------------------------------------------------
//  �⺻ GUI �½�ũ
//------------------------------------------------------------------------------
/**
 *  �⺻ GUI �½�ũ�� �ڵ�
 *      GUI �½�ũ�� ���� �� �����Ͽ� �⺻ �ڵ�� ���
 */
extern void printf(const char* str, ...);

DWORD WINAPI kBaseGUITask(LPVOID parameter)
{
    QWORD qwWindowID;
    int iMouseX, iMouseY;
    int iWindowWidth, iWindowHeight;
    EVENT stReceivedEvent;
    MOUSEEVENT* pstMouseEvent;
    KEYEVENT* pstKeyEvent;
    WINDOWEVENT* pstWindowEvent;

    
    //--------------------------------------------------------------------------
    // �����츦 ����
    //--------------------------------------------------------------------------
    // ���콺�� ���� ��ġ�� ��ȯ
    kGetCursorPosition( &iMouseX, &iMouseY );

    // �������� ũ��� ���� ����
    iWindowWidth = 500;
    iWindowHeight = 200;
    
    // ������ ���� �Լ� ȣ��, ���콺�� �ִ� ��ġ�� �������� ����
    qwWindowID = kCreateWindow( iMouseX - 10, iMouseY - WINDOW_TITLEBAR_HEIGHT / 2,
        iWindowWidth, iWindowHeight, WINDOW_FLAGS_DEFAULT | WINDOW_FLAGS_RESIZABLE,
         "Hello World Window" );
    // �����츦 �������� �������� ����
    if( qwWindowID == WINDOW_INVALIDID )
    {
        return 0;
    }
    
    //--------------------------------------------------------------------------
    // GUI �½�ũ�� �̺�Ʈ ó�� ����
    //--------------------------------------------------------------------------
    while( 1 )
    {
        // �̺�Ʈ ť���� �̺�Ʈ�� ����
        if( kReceiveEventFromWindowQueue( qwWindowID, &stReceivedEvent ) == FALSE )
        {

			//20180628
			ksleep( 0 );
            continue;
        }
        
        // ���ŵ� �̺�Ʈ�� Ÿ�Կ� ���� ������ ó��
        switch( stReceivedEvent.qwType )
        {
            // ���콺 �̺�Ʈ ó��
        case EVENT_MOUSE_MOVE:
        case EVENT_MOUSE_LBUTTONDOWN:
        case EVENT_MOUSE_LBUTTONUP:            
        case EVENT_MOUSE_RBUTTONDOWN:
        case EVENT_MOUSE_RBUTTONUP:
        case EVENT_MOUSE_MBUTTONDOWN:
        case EVENT_MOUSE_MBUTTONUP:
            // ���⿡ ���콺 �̺�Ʈ ó�� �ڵ� �ֱ�
            pstMouseEvent = &( stReceivedEvent.stMouseEvent );
            break;

            // Ű �̺�Ʈ ó��
        case EVENT_KEY_DOWN:
        case EVENT_KEY_UP:
            // ���⿡ Ű���� �̺�Ʈ ó�� �ڵ� �ֱ�
            pstKeyEvent = &( stReceivedEvent.stKeyEvent );
            break;

            // ������ �̺�Ʈ ó��
        case EVENT_WINDOW_SELECT:
        case EVENT_WINDOW_DESELECT:
        case EVENT_WINDOW_MOVE:
        case EVENT_WINDOW_RESIZE:
        case EVENT_WINDOW_CLOSE:
            // ���⿡ ������ �̺�Ʈ ó�� �ڵ� �ֱ�
            pstWindowEvent = &( stReceivedEvent.stWindowEvent );

            //------------------------------------------------------------------
            // ������ �ݱ� �̺�Ʈ�̸� �����츦 �����ϰ� ������ �������� �½�ũ�� ����
            //------------------------------------------------------------------
            if( stReceivedEvent.qwType == EVENT_WINDOW_CLOSE )
            {
                // ������ ����
                kDeleteWindow( qwWindowID );
                return 1;
            }
            break;
            
            // �� �� ����
        default:
            // ���⿡ �� �� ���� �̺�Ʈ ó�� �ڵ� �ֱ�
            break;
        }
    }

	return 0;
}

//------------------------------------------------------------------------------
//  Hello World GUI �½�ũ
//------------------------------------------------------------------------------
/**
 *  Hello World GUI �½�ũ
 */
QWORD qwGUIWindowID;
int iWindowWidth, iWindowHeight;
int iY;
MOUSEEVENT* pstMouseEvent;
KEYEVENT* pstKeyEvent;
WINDOWEVENT* pstWindowEvent;
char vcTempBuffer[50];
RECT stButtonArea;
    QWORD qwFindWindowID;
    EVENT stSendEvent;
	static int s_iWindowCount = 0;

	// �̺�Ʈ Ÿ�� ���ڿ�
	char* vpcEventString[] = {
		"Unknown",
		"MOUSE_MOVE       ",
		"MOUSE_LBUTTONDOWN",
		"MOUSE_LBUTTONUP  ",
		"MOUSE_RBUTTONDOWN",
		"MOUSE_RBUTTONUP  ",
		"MOUSE_MBUTTONDOWN",
		"MOUSE_MBUTTONUP  ",
		"WINDOW_SELECT    ",
		"WINDOW_DESELECT  ",
		"WINDOW_MOVE      ",
		"WINDOW_RESIZE    ",
		"WINDOW_CLOSE     ",
		"KEY_DOWN         ",
		"KEY_UP           " };


DWORD WINAPI kHelloWorldGUITask(LPVOID parameter)
{
    
    int iMouseX, iMouseY;
   
  
   
    
   
    
    
    int i;
    
    //--------------------------------------------------------------------------
    // �����츦 ����
    //--------------------------------------------------------------------------
    // ���콺�� ���� ��ġ�� ��ȯ
    kGetCursorPosition( &iMouseX, &iMouseY );

    // �������� ũ��� ���� ����
    iWindowWidth = 500;
    iWindowHeight = 200;
    
    // ������ ���� �Լ� ȣ��, ���콺�� �ִ� ��ġ�� �������� �����ϰ� ��ȣ�� �߰��Ͽ�
    // �����츶�� �������� �̸��� �Ҵ�
    sprintf( vcTempBuffer, "Hello World Window %d", s_iWindowCount++ );
	qwGUIWindowID = kCreateWindow( iMouseX - 10, iMouseY - WINDOW_TITLEBAR_HEIGHT / 2,
        iWindowWidth, iWindowHeight, WINDOW_FLAGS_DEFAULT, vcTempBuffer );
    // �����츦 �������� �������� ����
    if(qwGUIWindowID == WINDOW_INVALIDID )
    {
        return 0;
    }
    
    //--------------------------------------------------------------------------
    // ������ �Ŵ����� ������� �����ϴ� �̺�Ʈ�� ǥ���ϴ� ������ �׸�
    //--------------------------------------------------------------------------
    // �̺�Ʈ ������ ����� ��ġ ����
    iY = WINDOW_TITLEBAR_HEIGHT + 10;
    
    // �̺�Ʈ ������ ǥ���ϴ� ������ �׵θ��� ������ ID�� ǥ��
    kDrawRect(qwGUIWindowID, 10, iY + 8, iWindowWidth - 10, iY + 70, RGB( 0, 0, 0 ),
            FALSE );
    sprintf( vcTempBuffer, "GUI Event Information[Window ID: 0x%Q]", qwGUIWindowID);
    kDrawText(qwGUIWindowID, 20, iY, RGB( 0, 0, 0 ), RGB( 255, 255, 255 ),
               vcTempBuffer, strlen( vcTempBuffer ) );    
    
    //--------------------------------------------------------------------------
    // ȭ�� �Ʒ��� �̺�Ʈ ���� ��ư�� �׸�
    //--------------------------------------------------------------------------
    // ��ư ������ ����
    kSetRectangleData( 10, iY + 80, iWindowWidth - 10, iWindowHeight - 10, 
            &stButtonArea );
    // ����� �������� �������� �����ϰ� ���ڴ� ���������� �����Ͽ� ��ư�� �׸�
    kDrawButton(qwGUIWindowID, &stButtonArea, WINDOW_COLOR_BACKGROUND,
            "User Message Send Button(Up)", RGB( 0, 0, 0 ) );
    // �����츦 ȭ�鿡 ǥ��
    kShowWindow(qwGUIWindowID, TRUE );

	return 1;
       
}

void UpdateHelloGUITask()
{
	EVENT stReceivedEvent;
	//--------------------------------------------------------------------------
	// GUI �½�ũ�� �̺�Ʈ ó�� ����
	//--------------------------------------------------------------------------
	while (1)
	{
		// �̺�Ʈ ť���� �̺�Ʈ�� ����
		if (kReceiveEventFromWindowQueue(qwGUIWindowID, &stReceivedEvent) == FALSE)
		{
			//20180628
			ksleep( 0 );
			continue;
		}

		// ������ �̺�Ʈ ������ ǥ�õ� ������ �������� ĥ�Ͽ� ������ ǥ���� �����͸�
		// ��� ����
		kDrawRect(qwGUIWindowID, 11, iY + 20, iWindowWidth - 11, iY + 69,
			WINDOW_COLOR_BACKGROUND, TRUE);

		// ���ŵ� �̺�Ʈ�� Ÿ�Կ� ���� ������ ó��
		switch (stReceivedEvent.qwType)
		{
			// ���콺 �̺�Ʈ ó��
		case EVENT_MOUSE_MOVE:
		case EVENT_MOUSE_LBUTTONDOWN:
		case EVENT_MOUSE_LBUTTONUP:
		case EVENT_MOUSE_RBUTTONDOWN:
		case EVENT_MOUSE_RBUTTONUP:
		case EVENT_MOUSE_MBUTTONDOWN:
		case EVENT_MOUSE_MBUTTONUP:
			// ���⿡ ���콺 �̺�Ʈ ó�� �ڵ� �ֱ�
			pstMouseEvent = &(stReceivedEvent.stMouseEvent);

			// ���콺 �̺�Ʈ�� Ÿ���� ���
			sprintf(vcTempBuffer, "Mouse Event: %s",
				vpcEventString[stReceivedEvent.qwType]);
			kDrawText(qwGUIWindowID, 20, iY + 20, RGB(0, 0, 0),
				WINDOW_COLOR_BACKGROUND, vcTempBuffer, strlen(vcTempBuffer));

			// ���콺 �����͸� ���
			sprintf(vcTempBuffer, "Data: X = %d, Y = %d, Button = %X",
				pstMouseEvent->stPoint.iX, pstMouseEvent->stPoint.iY,
				pstMouseEvent->bButtonStatus);
			kDrawText(qwGUIWindowID, 20, iY + 40, RGB(0, 0, 0),
				WINDOW_COLOR_BACKGROUND, vcTempBuffer, strlen(vcTempBuffer));

			//------------------------------------------------------------------
			// ���콺 ���� �Ǵ� ������ �̺�Ʈ�̸� ��ư�� ������ �ٽ� �׸�
			//------------------------------------------------------------------
			// ���콺 ���� ��ư�� ������ �� ��ư ó��
			if (stReceivedEvent.qwType == EVENT_MOUSE_LBUTTONDOWN)
			{
				// ��ư ������ ���콺 ���� ��ư�� ���ȴ����� �Ǵ�
				if (kIsInRectangle(&stButtonArea, pstMouseEvent->stPoint.iX,
					pstMouseEvent->stPoint.iY) == TRUE)
				{
					// ��ư�� ����� ���� ������� �����Ͽ� �������� ǥ��
					kDrawButton(qwGUIWindowID, &stButtonArea,
						RGB(79, 204, 11), "User Message Send Button(Down)",
						RGB(255, 255, 255));
					kUpdateScreenByID(qwGUIWindowID);

					//----------------------------------------------------------
					// �ٸ� ������� ���� �̺�Ʈ�� ����
					//----------------------------------------------------------
					// ������ ��� �����츦 ã�Ƽ� �̺�Ʈ�� ����
					stSendEvent.qwType = EVENT_USER_TESTMESSAGE;
					stSendEvent.vqwData[0] = qwGUIWindowID;
					stSendEvent.vqwData[1] = 0x1234;
					stSendEvent.vqwData[2] = 0x5678;

					// ������ �������� �� ��ŭ ������ �����ϸ鼭 �̺�Ʈ�� ����
					for (int i = 0; i < s_iWindowCount; i++)
					{
						// ������ �������� �����츦 �˻�
						sprintf(vcTempBuffer, "Hello World Window %d", i);
						qwFindWindowID = kFindWindowByTitle(vcTempBuffer);
						// �����찡 �����ϸ� ������ �ڽ��� �ƴ� ���� �̺�Ʈ�� ����
						if ((qwFindWindowID != WINDOW_INVALIDID) &&
							(qwFindWindowID != qwGUIWindowID))
						{
							// ������� �̺�Ʈ ����
							kSendEventToWindow(qwFindWindowID, &stSendEvent);
						}
					}
				}
			}
			// ���콺 ���� ��ư�� �������� �� ��ư ó��
			else if (stReceivedEvent.qwType == EVENT_MOUSE_LBUTTONUP)
			{
				// ��ư�� ����� ������� �����Ͽ� ������ �ʾ����� ǥ��
				kDrawButton(qwGUIWindowID, &stButtonArea,
					WINDOW_COLOR_BACKGROUND, "User Message Send Button(Up)",
					RGB(0, 0, 0));
			}
			break;

			// Ű �̺�Ʈ ó��
		case EVENT_KEY_DOWN:
		case EVENT_KEY_UP:
			// ���⿡ Ű���� �̺�Ʈ ó�� �ڵ� �ֱ�
			pstKeyEvent = &(stReceivedEvent.stKeyEvent);

			// Ű �̺�Ʈ�� Ÿ���� ���
			sprintf(vcTempBuffer, "Key Event: %s",
				vpcEventString[stReceivedEvent.qwType]);
			kDrawText(qwGUIWindowID, 20, iY + 20, RGB(0, 0, 0),
				WINDOW_COLOR_BACKGROUND, vcTempBuffer, strlen(vcTempBuffer));

			// Ű �����͸� ���
			sprintf(vcTempBuffer, "Data: Key = %c, Flag = %X",
				pstKeyEvent->bASCIICode, pstKeyEvent->bFlags);
			kDrawText(qwGUIWindowID, 20, iY + 40, RGB(0, 0, 0),
				WINDOW_COLOR_BACKGROUND, vcTempBuffer, strlen(vcTempBuffer));
			break;

			// ������ �̺�Ʈ ó��
		case EVENT_WINDOW_SELECT:
		case EVENT_WINDOW_DESELECT:
		case EVENT_WINDOW_MOVE:
		case EVENT_WINDOW_RESIZE:
		case EVENT_WINDOW_CLOSE:
			// ���⿡ ������ �̺�Ʈ ó�� �ڵ� �ֱ�
			pstWindowEvent = &(stReceivedEvent.stWindowEvent);

			// ������ �̺�Ʈ�� Ÿ���� ���
			sprintf(vcTempBuffer, "Window Event: %s",
				vpcEventString[stReceivedEvent.qwType]);
			kDrawText(qwGUIWindowID, 20, iY + 20, RGB(0, 0, 0),
				WINDOW_COLOR_BACKGROUND, vcTempBuffer, strlen(vcTempBuffer));

			// ������ �����͸� ���
			sprintf(vcTempBuffer, "Data: X1 = %d, Y1 = %d, X2 = %d, Y2 = %d",
				pstWindowEvent->stArea.iX1, pstWindowEvent->stArea.iY1,
				pstWindowEvent->stArea.iX2, pstWindowEvent->stArea.iY2);
			kDrawText(qwGUIWindowID, 20, iY + 40, RGB(0, 0, 0),
				WINDOW_COLOR_BACKGROUND, vcTempBuffer, strlen(vcTempBuffer));

			//------------------------------------------------------------------
			// ������ �ݱ� �̺�Ʈ�̸� �����츦 �����ϰ� ������ �������� �½�ũ�� ����
			//------------------------------------------------------------------
			if (stReceivedEvent.qwType == EVENT_WINDOW_CLOSE)
			{
				// ������ ����
				kDeleteWindow(qwGUIWindowID);
				return;
			}
			break;

			// �� �� ����
		default:
			// ���⿡ �� �� ���� �̺�Ʈ ó�� �ڵ� �ֱ�
			// �� �� ���� �̺�Ʈ�� ���
			sprintf(vcTempBuffer, "Unknown Event: 0x%X", stReceivedEvent.qwType);
			kDrawText(qwGUIWindowID, 20, iY + 20, RGB(0, 0, 0), WINDOW_COLOR_BACKGROUND,
				vcTempBuffer, strlen(vcTempBuffer));

			// �����͸� ���
			sprintf(vcTempBuffer, "Data0 = 0x%x, Data1 = 0x%x, Data2 = 0x%x",
				stReceivedEvent.vqwData[0], stReceivedEvent.vqwData[1],
				stReceivedEvent.vqwData[2]);
			kDrawText(qwGUIWindowID, 20, iY + 40, RGB(0, 0, 0),
				WINDOW_COLOR_BACKGROUND, vcTempBuffer, strlen(vcTempBuffer));
			break;
		}

		// �����츦 ȭ�鿡 ������Ʈ
		kShowWindow(qwGUIWindowID, TRUE);
	}
}

//------------------------------------------------------------------------------
//  �ý��� ����� �½�ũ
//------------------------------------------------------------------------------
/**
 *  �ý����� ���¸� �����Ͽ� ȭ�鿡 ǥ���ϴ� �½�ũ
 */
/*void kSystemMonitorTask( void )
{
    QWORD qwWindowID;
    int i;
    int iWindowWidth;
    int iProcessorCount;
    DWORD vdwLastCPULoad[ MAXPROCESSORCOUNT ];
    int viLastTaskCount[ MAXPROCESSORCOUNT ];
    QWORD qwLastTickCount;
    EVENT stReceivedEvent;
    WINDOWEVENT* pstWindowEvent;
    bool bChanged;
    RECT stScreenArea;
    QWORD qwLastDynamicMemoryUsedSize;
    QWORD qwDynamicMemoryUsedSize;
    QWORD qwTemp;
    
    //--------------------------------------------------------------------------
    // �����츦 ����
    //--------------------------------------------------------------------------
    // ȭ�� ������ ũ�⸦ ��ȯ
    kGetScreenArea( &stScreenArea );
    
    // ���� ���μ����� ������ �������� �ʺ� ���
    iProcessorCount = kGetProcessorCount();
    iWindowWidth = iProcessorCount * ( SYSTEMMONITOR_PROCESSOR_WIDTH + 
            SYSTEMMONITOR_PROCESSOR_MARGIN ) + SYSTEMMONITOR_PROCESSOR_MARGIN;
    
    // �����츦 ȭ�� ����� ������ �� ȭ�鿡 ǥ������ ����. ���μ��� ������ �޸� ������ 
    // ǥ���ϴ� ������ �׸� �� ȭ�鿡 ǥ��
    qwWindowID = kCreateWindow( ( stScreenArea.iX2 - iWindowWidth ) / 2, 
        ( stScreenArea.iY2 - SYSTEMMONITOR_WINDOW_HEIGHT ) / 2, 
        iWindowWidth, SYSTEMMONITOR_WINDOW_HEIGHT, WINDOW_FLAGS_DEFAULT & 
        ~WINDOW_FLAGS_SHOW, "System Monitor" );
    // �����츦 �������� �������� ����
    if( qwWindowID == WINDOW_INVALIDID )
    {
        return ;
    }

    // ���μ��� ������ ǥ���ϴ� ������ 3�ȼ� �β��� ǥ���ϰ� ���ڿ��� ���
    kDrawLine( qwWindowID, 5, WINDOW_TITLEBAR_HEIGHT + 15, iWindowWidth - 5, 
            WINDOW_TITLEBAR_HEIGHT + 15, RGB( 0, 0, 0 ) );
    kDrawLine( qwWindowID, 5, WINDOW_TITLEBAR_HEIGHT + 16, iWindowWidth - 5, 
            WINDOW_TITLEBAR_HEIGHT + 16, RGB( 0, 0, 0 ) );
    kDrawLine( qwWindowID, 5, WINDOW_TITLEBAR_HEIGHT + 17, iWindowWidth - 5, 
            WINDOW_TITLEBAR_HEIGHT + 17, RGB( 0, 0, 0 ) );
    kDrawText( qwWindowID, 9, WINDOW_TITLEBAR_HEIGHT + 8, RGB( 0, 0, 0 ), 
            WINDOW_COLOR_BACKGROUND, "Processor Information", 21 );


    // �޸� ������ ǥ���ϴ� ������ 3�ȼ� �β��� ǥ���ϰ� ���ڿ��� ���
    kDrawLine( qwWindowID, 5, WINDOW_TITLEBAR_HEIGHT + SYSTEMMONITOR_PROCESSOR_HEIGHT + 50, 
            iWindowWidth - 5, WINDOW_TITLEBAR_HEIGHT + SYSTEMMONITOR_PROCESSOR_HEIGHT + 50, 
            RGB( 0, 0, 0 ) );
    kDrawLine( qwWindowID, 5, WINDOW_TITLEBAR_HEIGHT + SYSTEMMONITOR_PROCESSOR_HEIGHT + 51, 
                iWindowWidth - 5, WINDOW_TITLEBAR_HEIGHT + SYSTEMMONITOR_PROCESSOR_HEIGHT + 51, 
                RGB( 0, 0, 0 ) );
    kDrawLine( qwWindowID, 5, WINDOW_TITLEBAR_HEIGHT + SYSTEMMONITOR_PROCESSOR_HEIGHT + 52, 
                iWindowWidth - 5, WINDOW_TITLEBAR_HEIGHT + SYSTEMMONITOR_PROCESSOR_HEIGHT + 52, 
                RGB( 0, 0, 0 ) );
    kDrawText( qwWindowID, 9, WINDOW_TITLEBAR_HEIGHT + SYSTEMMONITOR_PROCESSOR_HEIGHT + 43, 
            RGB( 0, 0, 0 ), WINDOW_COLOR_BACKGROUND, "Memory Information", 18 );
    // �����츦 ȭ�鿡 ǥ��
    kShowWindow( qwWindowID, TRUE );
    
    // ������ ���鼭 �ý��� ������ �����Ͽ� ȭ�鿡 ǥ��
    qwLastTickCount = 0;
    
    // ���������� ������ ���μ����� ���Ͽ� �½�ũ ��, �׸��� �޸� ��뷮�� ��� 0���� ����
    memset( vdwLastCPULoad, 0, sizeof( vdwLastCPULoad ) );
	memset( viLastTaskCount, 0, sizeof( viLastTaskCount ) );
    qwLastDynamicMemoryUsedSize = 0;
    
    //--------------------------------------------------------------------------
    // GUI �½�ũ�� �̺�Ʈ ó�� ����
    //--------------------------------------------------------------------------
    while( 1 )
    {
        //----------------------------------------------------------------------
        // �̺�Ʈ ť�� �̺�Ʈ ó��
        //----------------------------------------------------------------------
        // �̺�Ʈ ť���� �̺�Ʈ�� ����
        if( kReceiveEventFromWindowQueue( qwWindowID, &stReceivedEvent ) == TRUE )
        {
            // ���ŵ� �̺�Ʈ�� Ÿ�Կ� ���� ������ ó��
            switch( stReceivedEvent.qwType )
            {
                // ������ �̺�Ʈ ó��
            case EVENT_WINDOW_CLOSE:
                //--------------------------------------------------------------
                // ������ �ݱ� �̺�Ʈ�̸� �����츦 �����ϰ� ������ �������� �½�ũ�� ����
                //--------------------------------------------------------------
                // ������ ����
                kDeleteWindow( qwWindowID );
                return ;
                break;
                
                // �� �� ����
            default:
                break;
            }
        }
        
        // 0.5�ʸ��� �ѹ��� �ý��� ���¸� Ȯ��
        if( ( kGetTickCount() - qwLastTickCount ) < 500 )
        {
            ksleep( 1 );
            continue;
        }

        // ���������� ������ �ð��� �ֽ����� ������Ʈ
        qwLastTickCount = kGetTickCount();

        //----------------------------------------------------------------------
        // ���μ��� ���� ���
        //----------------------------------------------------------------------
        // ���μ��� ����ŭ ���Ͽ� �½�ũ ���� Ȯ���Ͽ� �޶��� ���� ������ ȭ�鿡 ������Ʈ
        for( i = 0 ; i < iProcessorCount ; i++ )
        {
            bChanged = FALSE;
            
            // ���μ��� ���� �˻�
            if( vdwLastCPULoad[ i ] != kGetProcessorLoad( i ) )
            {
                vdwLastCPULoad [ i ] = kGetProcessorLoad( i );
                bChanged = TRUE;
            }
            // �½�ũ �� �˻�
            else if( viLastTaskCount[ i ] != kGetTaskCount( i ) )
            {
                viLastTaskCount[ i ] = kGetTaskCount( i );
                bChanged = TRUE;
            }
            
            // ������ ���ؼ� ���� ������ ������ ȭ�鿡 ������Ʈ
            if( bChanged == TRUE )
            {
                // ȭ�鿡 ���� ���μ����� ���ϸ� ǥ�� 
                kDrawProcessorInformation( qwWindowID, i * SYSTEMMONITOR_PROCESSOR_WIDTH + 
                    ( i + 1 ) * SYSTEMMONITOR_PROCESSOR_MARGIN, WINDOW_TITLEBAR_HEIGHT + 28,
                    i );
            }
        }
        
        //----------------------------------------------------------------------
        // ���� �޸� ���� ���
        //----------------------------------------------------------------------
        // ���� �޸��� ������ ��ȯ
        kGetDynamicMemoryInformation( &qwTemp, &qwTemp, &qwTemp, 
                &qwDynamicMemoryUsedSize );
        
        // ���� ���� �Ҵ� �޸� ��뷮�� ������ �ٸ��ٸ� �޸� ������ ���
        if( qwDynamicMemoryUsedSize != qwLastDynamicMemoryUsedSize )
        {
            qwLastDynamicMemoryUsedSize = qwDynamicMemoryUsedSize;
            
            // �޸� ������ ���
            kDrawMemoryInformation( qwWindowID, WINDOW_TITLEBAR_HEIGHT + 
                    SYSTEMMONITOR_PROCESSOR_HEIGHT + 60, iWindowWidth );
        }
    }
}*/

/**
 *  ���� ���μ����� ������ ȭ�鿡 ǥ��
 */
/*static void kDrawProcessorInformation( QWORD qwWindowID, int iX, int iY, 
        BYTE bAPICID )
{
    char vcBuffer[ 100 ];
    RECT stArea;
    QWORD qwProcessorLoad;
    QWORD iUsageBarHeight;
    int iMiddleX;
    
    // ���μ��� ID�� ǥ��
	sprintf( vcBuffer, "Processor ID: %d", bAPICID );
    kDrawText( qwWindowID, iX + 10, iY, RGB( 0, 0, 0 ), WINDOW_COLOR_BACKGROUND, 
            vcBuffer, strlen( vcBuffer ) );
    
    // ���μ����� �½�ũ ������ ǥ��
	sprintf( vcBuffer, "Task Count: %d   ", kGetTaskCount( bAPICID ) );
    kDrawText( qwWindowID, iX + 10, iY + 18, RGB( 0, 0, 0 ), WINDOW_COLOR_BACKGROUND,
            vcBuffer, strlen( vcBuffer ) );

    //--------------------------------------------------------------------------
    // ���μ��� ���ϸ� ��Ÿ���� ���븦 ǥ��
    //--------------------------------------------------------------------------
    // ���μ��� ���ϸ� ǥ��
    qwProcessorLoad = kGetProcessorLoad( bAPICID );
    if( qwProcessorLoad > 100 )
    {
        qwProcessorLoad = 100;
    }
    
    // ���ϸ� ǥ���ϴ� ������ ��ü�� �׵θ��� ǥ��
    kDrawRect( qwWindowID, iX, iY + 36, iX + SYSTEMMONITOR_PROCESSOR_WIDTH, 
            iY + SYSTEMMONITOR_PROCESSOR_HEIGHT, RGB( 0, 0, 0 ), FALSE );

    // ���μ��� ��뷮�� ��Ÿ���� ������ ����, ( ���� ��ü�� ���� * ���μ��� ���� / 100 ) 
    iUsageBarHeight = ( SYSTEMMONITOR_PROCESSOR_HEIGHT - 40 ) * qwProcessorLoad / 100;

    // ���ϸ� ǥ���ϴ� ������ ���� ���θ� ǥ��
    // ä���� ���븦 ǥ��, �׵θ��� 2�ȼ� ���� ���� ������ �� 
    kDrawRect( qwWindowID, iX + 2,
        iY + ( SYSTEMMONITOR_PROCESSOR_HEIGHT - iUsageBarHeight ) - 2, 
        iX + SYSTEMMONITOR_PROCESSOR_WIDTH - 2, 
        iY + SYSTEMMONITOR_PROCESSOR_HEIGHT - 2, SYSTEMMONITOR_BAR_COLOR, TRUE );
    // �� ���븦 ǥ��, �׵θ��� 2�ȼ� ���� ���� ������ ��
    kDrawRect( qwWindowID, iX + 2, iY + 38, iX + SYSTEMMONITOR_PROCESSOR_WIDTH - 2,
            iY + ( SYSTEMMONITOR_PROCESSOR_HEIGHT - iUsageBarHeight ) - 1, 
            WINDOW_COLOR_BACKGROUND, TRUE );
    
    // ���μ����� ���ϸ� ǥ��, ������ ����� ���ϰ� ǥ�õǵ��� ��
	sprintf( vcBuffer, "Usage: %d%%", qwProcessorLoad );
    iMiddleX = ( SYSTEMMONITOR_PROCESSOR_WIDTH - 
            (strlen( vcBuffer ) * FONT_ENGLISHWIDTH ) ) / 2;
    kDrawText( qwWindowID, iX + iMiddleX, iY + 80, RGB( 0, 0, 0 ), 
            WINDOW_COLOR_BACKGROUND, vcBuffer, strlen( vcBuffer ) );
    
    // ���μ��� ������ ǥ�õ� ������ �ٽ� ȭ�鿡 ������Ʈ
    kSetRectangleData( iX, iY, iX + SYSTEMMONITOR_PROCESSOR_WIDTH, 
            iY + SYSTEMMONITOR_PROCESSOR_HEIGHT, &stArea );    
    kUpdateScreenByWindowArea( qwWindowID, &stArea );
}*/

/**
 *  �޸� ������ ���
 */
/*static void kDrawMemoryInformation( QWORD qwWindowID, int iY, int iWindowWidth )
{
    char vcBuffer[ 100 ];
    QWORD qwTotalRAMKbyteSize;
    QWORD qwDynamicMemoryStartAddress;
    QWORD qwDynamicMemoryUsedSize;
    QWORD qwUsedPercent;
    QWORD qwTemp;
    int iUsageBarWidth;
    RECT stArea;
    int iMiddleX;
    
    // Mbyte ������ �޸𸮸� Kbyte ������ ��ȯ
    qwTotalRAMKbyteSize = kGetTotalRAMSize() * 1024;
    
    // �޸� ������ ǥ��
	sprintf( vcBuffer, "Total Size: %d KB        ", qwTotalRAMKbyteSize );
    kDrawText( qwWindowID, SYSTEMMONITOR_PROCESSOR_MARGIN + 10, iY + 3, RGB( 0, 0, 0 ), 
            WINDOW_COLOR_BACKGROUND, vcBuffer, strlen( vcBuffer ) );
    
    // ���� �޸��� ������ ��ȯ
    kGetDynamicMemoryInformation( &qwDynamicMemoryStartAddress, &qwTemp, 
            &qwTemp, &qwDynamicMemoryUsedSize );
    
	sprintf( vcBuffer, "Used Size: %d KB        ", ( qwDynamicMemoryUsedSize +
            qwDynamicMemoryStartAddress ) / 1024 );
    kDrawText( qwWindowID, SYSTEMMONITOR_PROCESSOR_MARGIN + 10, iY + 21, RGB( 0, 0, 0 ), 
            WINDOW_COLOR_BACKGROUND, vcBuffer, strlen( vcBuffer ) );
    
    //--------------------------------------------------------------------------
    // �޸� ��뷮�� ��Ÿ���� ���븦 ǥ��
    //--------------------------------------------------------------------------
    // �޸� ��뷮�� ǥ���ϴ� ������ ��ü�� �׵θ��� ǥ��
    kDrawRect( qwWindowID, SYSTEMMONITOR_PROCESSOR_MARGIN, iY + 40,
            iWindowWidth - SYSTEMMONITOR_PROCESSOR_MARGIN, 
            iY + SYSTEMMONITOR_MEMORY_HEIGHT - 32, RGB( 0, 0, 0 ), FALSE );
    // �޸� ��뷮(%)�� ���
    qwUsedPercent = ( qwDynamicMemoryStartAddress + qwDynamicMemoryUsedSize ) * 
        100 / 1024 / qwTotalRAMKbyteSize;
    if( qwUsedPercent > 100 )
    {
        qwUsedPercent = 100;
    }
    
    // �޸� ��뷮�� ��Ÿ���� ������ ����, ( ���� ��ü�� ���� * �޸� ��뷮 / 100 )     
    iUsageBarWidth = ( iWindowWidth - 2 * SYSTEMMONITOR_PROCESSOR_MARGIN ) * 
        qwUsedPercent / 100;
    
    // �޸� ��뷮�� ǥ���ϴ� ������ ���� ���θ� ǥ��
    // ��ĥ�� ���븦 ǥ��, �׵θ��� 2�ȼ� ���� ���� ������ �� 
    kDrawRect( qwWindowID, SYSTEMMONITOR_PROCESSOR_MARGIN + 2, iY + 42, 
            SYSTEMMONITOR_PROCESSOR_MARGIN + 2 + iUsageBarWidth, 
            iY + SYSTEMMONITOR_MEMORY_HEIGHT - 34, SYSTEMMONITOR_BAR_COLOR, TRUE );  
    // �� ���븦 ǥ��, �׵θ��� 2�ȼ� ���� ���� ������ ��
    kDrawRect( qwWindowID, SYSTEMMONITOR_PROCESSOR_MARGIN + 2 + iUsageBarWidth, 
        iY + 42, iWindowWidth - SYSTEMMONITOR_PROCESSOR_MARGIN - 2,
        iY + SYSTEMMONITOR_MEMORY_HEIGHT - 34, WINDOW_COLOR_BACKGROUND, TRUE );    
    
    // ��뷮�� ��Ÿ���� �ؽ�Ʈ ǥ��, ������ ����� ��뷮�� ǥ�õǵ��� ��
	sprintf( vcBuffer, "Usage: %d%%", qwUsedPercent );
    iMiddleX = ( iWindowWidth - (strlen( vcBuffer ) * FONT_ENGLISHWIDTH ) ) / 2;
    kDrawText( qwWindowID, iMiddleX, iY + 45, RGB( 0, 0, 0 ), WINDOW_COLOR_BACKGROUND, 
            vcBuffer, strlen( vcBuffer ) );
    
    // �޸� ������ ǥ�õ� ������ ȭ�鿡 �ٽ� ������Ʈ
    kSetRectangleData(0, iY, iWindowWidth, iY + SYSTEMMONITOR_MEMORY_HEIGHT, &stArea );
    kUpdateScreenByWindowArea( qwWindowID, &stArea );
}*/

//------------------------------------------------------------------------------
//  GUI ������ �ܼ� �� �½�ũ
//------------------------------------------------------------------------------
// ���� ȭ�� ������ ���� �����ϴ� ����
#define CONSOLE_WIDTH                       80
#define CONSOLE_HEIGHT                      25
#include "Console.h"
static CHARACTER gs_vstPreviousScreenBuffer[ CONSOLE_WIDTH * CONSOLE_HEIGHT ];

/**
 *  GUI ������ �ܼ� �� �½�ũ
 */
#include "SkyInterface.h"
extern SKY_PROCESS_INTERFACE g_processInterface;

DWORD WINAPI kGUIConsoleShellTask(LPVOID parameter)
{
    static QWORD s_qwWindowID = WINDOW_INVALIDID;
    int iWindowWidth, iWindowHeight;
    EVENT stReceivedEvent;
    KEYEVENT* pstKeyEvent;
    RECT stScreenArea;
    KEYDATA stKeyData;
   // TCB* pstConsoleShellTask;
    QWORD qwConsoleShellTaskID;


    // GUI �ܼ� �� �����찡 �����ϸ� ������ �����츦 �ֻ����� ����� �½�ũ ����
    if( s_qwWindowID != WINDOW_INVALIDID )
    {
        kMoveWindowToTop( s_qwWindowID );
        return 0;
    }
    
    //--------------------------------------------------------------------------
    // �����츦 ����
    //--------------------------------------------------------------------------
    // ��ü ȭ�� ������ ũ�⸦ ��ȯ
    kGetScreenArea( &stScreenArea );
    
    // �������� ũ�� ����, ȭ�� ���ۿ� ���� ������ �ִ� �ʺ�� ���̸� ����ؼ� ���
    iWindowWidth = CONSOLE_WIDTH * FONT_ENGLISHWIDTH + 4;
    iWindowHeight = CONSOLE_HEIGHT * FONT_ENGLISHHEIGHT + WINDOW_TITLEBAR_HEIGHT + 2;
    
    // ������ ���� �Լ� ȣ��, ȭ�� ����� ����
    s_qwWindowID = kCreateWindow( ( stScreenArea.iX2 - iWindowWidth ) / 2, 
        ( stScreenArea.iY2 - iWindowHeight ) / 2, iWindowWidth, iWindowHeight, 
        WINDOW_FLAGS_DEFAULT, "Console Shell for GUI" );
    // �����츦 �������� �������� ����
    if( s_qwWindowID == WINDOW_INVALIDID )
    {
        return 0;
    }

	g_processInterface.sky_kcreate_thread_from_memory(1, kStartConsoleShell, NULL);

    // �� Ŀ�ǵ带 ó���ϴ� �ܼ� �� �½�ũ�� ����
   // kSetConsoleShellExitFlag( FALSE );
   // pstConsoleShellTask = kCreateTask( TASK_FLAGS_LOW | TASK_FLAGS_THREAD, 0, 0, 
     //   ( QWORD ) kStartConsoleShell, TASK_LOADBALANCINGID );
    //if( pstConsoleShellTask == NULL )
    {
        // �ܼ� �� �½�ũ ������ �����ϸ� �����츦 �����ϰ� ����
//        kDeleteWindow( s_qwWindowID );
      //  return ;
    }
    // �ܼ� �� �½�ũ�� ID�� ����
    //qwConsoleShellTaskID = pstConsoleShellTask->stLink.qwID;

    // ���� ȭ�� ���۸� �ʱ�ȭ
    memset( gs_vstPreviousScreenBuffer, 0xFF, sizeof( gs_vstPreviousScreenBuffer ) );
    
    //--------------------------------------------------------------------------
    // GUI �½�ũ�� �̺�Ʈ ó�� ����
    //--------------------------------------------------------------------------
    while( 1 )
    {
        // ȭ�� ������ ����� ������ �����쿡 ������Ʈ
        kProcessConsoleBuffer( s_qwWindowID );
        
        // �̺�Ʈ ť���� �̺�Ʈ�� ����
        if( kReceiveEventFromWindowQueue( s_qwWindowID, &stReceivedEvent ) == FALSE )
        {
			//20180628
            ksleep( 0 );
            continue;
        }
        
        // ���ŵ� �̺�Ʈ�� Ÿ�Կ� ���� ������ ó��
        switch( stReceivedEvent.qwType )
        {
            // Ű �̺�Ʈ ó��
        case EVENT_KEY_DOWN:
        case EVENT_KEY_UP:
            // ���⿡ Ű���� �̺�Ʈ ó�� �ڵ� �ֱ�
            pstKeyEvent = &( stReceivedEvent.stKeyEvent );
            stKeyData.bASCIICode = pstKeyEvent->bASCIICode;
            stKeyData.bFlags = pstKeyEvent->bFlags;
            stKeyData.bScanCode = pstKeyEvent->bScanCode;

            // Ű�� �׷��� ���� Ű ť�� �����Ͽ� �� �½�ũ�� �Է����� ����
            kPutKeyToGUIKeyQueue( &stKeyData );
            break;

            // ������ �̺�Ʈ ó��
        case EVENT_WINDOW_CLOSE:
            // ������ �� �½�ũ�� ����ǵ��� ���� �÷��׸� �����ϰ� ����� ������ ���
           // kSetConsoleShellExitFlag( TRUE );
            //while( kIsTaskExist( qwConsoleShellTaskID ) == TRUE )
            {
				//20180628
			ksleep( 1 );
            }
            
            // �����츦 �����ϰ� ������ ID�� �ʱ�ȭ
            kDeleteWindow( s_qwWindowID );
            s_qwWindowID = WINDOW_INVALIDID;            
            return 0;
            
            break;
            
            // �� �� ����
        default:
            // ���⿡ �� �� ���� �̺�Ʈ ó�� �ڵ� �ֱ�
            break;
        }
    }

	return 0;
}

//
 //  ȭ�� ������ ����� ������ GUI �ܼ� �� ������ ȭ�鿡 ������Ʈ
 //
static void kProcessConsoleBuffer(QWORD qwWindowID)
{
	int i;
	int j;
	CONSOLEMANAGER* pstManager;
	CHARACTER* pstScreenBuffer;
	CHARACTER* pstPreviousScreenBuffer;
	RECT stLineArea;
	bool bChanged;
	static QWORD s_qwLastTickCount = 0;
	bool bFullRedraw;

	// �ܼ��� �����ϴ� �ڷᱸ���� ��ȯ �޾� ȭ�� ������ ��巹���� �����ϰ� 
	// ���� ȭ�� ������ ��巹���� ����
	pstManager = kGetConsoleManager();
	pstScreenBuffer = pstManager->pstScreenBuffer;
	pstPreviousScreenBuffer = gs_vstPreviousScreenBuffer;

	// ȭ���� ��ü�� ������Ʈ �� �� 5�ʰ� �������� ������ ȭ�� ��ü�� �ٽ� �׸�
	/*if (kGetTickCount() - s_qwLastTickCount > 5000)
	{
		s_qwLastTickCount = kGetTickCount();
		bFullRedraw = TRUE;
	}
	else*/
	{
		bFullRedraw = FALSE;
	}

	// ȭ�� ������ ���̸�ŭ �ݺ�
	for (j = 0; j < CONSOLE_HEIGHT; j++)
	{
		// ó������ ������� ���� ������ �÷��� ����
		bChanged = FALSE;

		// ���� ���ο� ��ȭ�� �ִ��� ���Ͽ� ���� ���� �÷��׸� ó��
		for (i = 0; i < CONSOLE_WIDTH; i++)
		{
			// ���ڸ� ���Ͽ� �ٸ��ų� ��ü�� ���� �׷��� �ϸ� ���� ȭ�� ���ۿ�
			// ������Ʈ�ϰ� ���� ���� �÷��׸� ����
			if ((pstScreenBuffer->bCharactor != pstPreviousScreenBuffer->bCharactor) ||
				(bFullRedraw == TRUE))
			{
				// ���ڸ� ȭ�鿡 ���
				kDrawText(qwWindowID, i * FONT_ENGLISHWIDTH + 2,
					j * FONT_ENGLISHHEIGHT + WINDOW_TITLEBAR_HEIGHT,
					RGB(0, 255, 0), RGB(0, 0, 0),
					(const char*)&(pstScreenBuffer->bCharactor), 1);

				// ���� ȭ�� ���۷� ���� �Ű� ���� ���� ���ο� ������
				// �ٸ� �����Ͱ� ������ ǥ��
				memcpy(pstPreviousScreenBuffer, pstScreenBuffer, sizeof(CHARACTER));
				bChanged = TRUE;
			}

			pstScreenBuffer++;
			pstPreviousScreenBuffer++;
		}

		// ���� ���ο��� ����� �����Ͱ� �ִٸ� ���� ���θ� ȭ�鿡 ������Ʈ
		if (bChanged == TRUE)
		{
			// ���� ������ ������ ���
			kSetRectangleData(2, j * FONT_ENGLISHHEIGHT + WINDOW_TITLEBAR_HEIGHT,
				5 + FONT_ENGLISHWIDTH * CONSOLE_WIDTH, (j + 1) * FONT_ENGLISHHEIGHT +
				WINDOW_TITLEBAR_HEIGHT - 1, &stLineArea);
			// �������� �Ϻθ� ȭ�� ������Ʈ
			kUpdateScreenByWindowArea(qwWindowID, &stLineArea);
		}
	}
}

//------------------------------------------------------------------------------
//  �̹��� ���(Image Viewer)
//------------------------------------------------------------------------------
/**
 *  �̹��� ��� �½�ũ
 */
 // GUI �ܼ� �� �½�ũ �Լ�
DWORD WINAPI kImageViewerTask(LPVOID parameter)
{
    QWORD qwWindowID;
    int iMouseX, iMouseY;
    int iWindowWidth, iWindowHeight;
    int iEditBoxWidth;
    RECT stEditBoxArea;
    RECT stButtonArea;
    RECT stScreenArea;
    EVENT stReceivedEvent;
    EVENT stSendEvent;
    char vcFileName[FILESYSTEM_MAXFILENAMELENGTH + 1 ];
    int iFileNameLength;
    MOUSEEVENT* pstMouseEvent;
    KEYEVENT* pstKeyEvent;
    POINT stScreenXY;
    POINT stClientXY;
    
    //--------------------------------------------------------------------------
    // �����츦 ����
    //--------------------------------------------------------------------------
    // ��ü ȭ�� ������ ũ�⸦ ��ȯ
    kGetScreenArea( &stScreenArea );
    
    // �������� ũ�� ����, ȭ�� ���ۿ� ���� ������ �ִ� �ʺ�� ���̸� ����ؼ� ���
    iWindowWidth = FONT_ENGLISHWIDTH * FILESYSTEM_MAXFILENAMELENGTH + 165;
    iWindowHeight = 35 + WINDOW_TITLEBAR_HEIGHT + 5;
    
    // ������ ���� �Լ� ȣ��, ȭ�� ����� ����
    qwWindowID = kCreateWindow( ( stScreenArea.iX2 - iWindowWidth ) / 2, 
        ( stScreenArea.iY2 - iWindowHeight ) / 2, iWindowWidth, iWindowHeight, 
        WINDOW_FLAGS_DEFAULT & ~WINDOW_FLAGS_SHOW, "Image Viewer" );
    // �����츦 �������� �������� ����
    if( qwWindowID == WINDOW_INVALIDID )
    {
        return 0;
    }
    
    // ���� �̸��� �Է��ϴ� ����Ʈ �ڽ� ������ ǥ��
    kDrawText( qwWindowID, 5, WINDOW_TITLEBAR_HEIGHT + 6, RGB( 0, 0, 0 ), 
            WINDOW_COLOR_BACKGROUND, "FILE NAME", 9 );
    iEditBoxWidth = FONT_ENGLISHWIDTH * FILESYSTEM_MAXFILENAMELENGTH + 4;
    kSetRectangleData( 85, WINDOW_TITLEBAR_HEIGHT + 5, 85 + iEditBoxWidth, 
                       WINDOW_TITLEBAR_HEIGHT + 25, &stEditBoxArea );
    kDrawRect( qwWindowID, stEditBoxArea.iX1, stEditBoxArea.iY1, 
            stEditBoxArea.iX2, stEditBoxArea.iY2, RGB( 0, 0, 0 ), FALSE );

    // ���� �̸� ���۸� ���� ����Ʈ �ڽ��� �� ���� �̸��� ǥ��
    iFileNameLength = 0;
    memset( vcFileName, 0, sizeof( vcFileName ) );
    kDrawFileName( qwWindowID, &stEditBoxArea, vcFileName, iFileNameLength );
    
    // �̹��� ��� ��ư ������ ����
    kSetRectangleData( stEditBoxArea.iX2 + 10, stEditBoxArea.iY1, 
                       stEditBoxArea.iX2 + 70, stEditBoxArea.iY2, &stButtonArea );
    kDrawButton( qwWindowID, &stButtonArea, WINDOW_COLOR_BACKGROUND, "Show", 
            RGB( 0, 0, 0 ) );
    
    // �����츦 ǥ��
    kShowWindow( qwWindowID, TRUE );
    
    //--------------------------------------------------------------------------
    // GUI �½�ũ�� �̺�Ʈ ó�� ����
    //--------------------------------------------------------------------------
    while( 1 )
    {
        // �̺�Ʈ ť���� �̺�Ʈ�� ����
        if( kReceiveEventFromWindowQueue( qwWindowID, &stReceivedEvent ) == FALSE )
        {
			//20180628
            ksleep( 0 );
            continue;
        }
        
        // ���ŵ� �̺�Ʈ�� Ÿ�Կ� ���� ������ ó��
        switch( stReceivedEvent.qwType )
        {
            // ���콺 �̺�Ʈ ó��
        case EVENT_MOUSE_LBUTTONDOWN:
            pstMouseEvent = &( stReceivedEvent.stMouseEvent );

            // ���콺 ���� ��ư�� �̹��� ��� ��ư ������ ���������� ����� ���� �̸��� 
            // �̿��Ͽ� �̹����� ȭ�鿡 ǥ��
            if( kIsInRectangle( &stButtonArea, pstMouseEvent->stPoint.iX, 
                                pstMouseEvent->stPoint.iY ) == TRUE )
            {
                // ��ư�� ���� ������ ǥ��
                kDrawButton( qwWindowID, &stButtonArea, RGB( 79, 204, 11 ), "Show", 
                            RGB( 255, 255, 255 ) );
                // ��ư�� �ִ� ������ ȭ�� ������Ʈ
                kUpdateScreenByWindowArea( qwWindowID, &( stButtonArea ) );

                // �̹��� ��� �����츦 �����ϰ� �̺�Ʈ�� ó��
                if( kCreateImageViewerWindowAndExecute( qwWindowID, vcFileName ) 
                        == FALSE )
                {
                    // ������ ������ �����ϸ� ��ư�� �������ٰ� �������� ȿ���� �ַ���
                    // 200ms ���
					//20180628	
					ksleep( 200 );
                }
                
                // ��ư�� ������ ������ ǥ��
                kDrawButton( qwWindowID, &stButtonArea, WINDOW_COLOR_BACKGROUND,
                        "Show", RGB( 0, 0, 0 ) );
                // ��ư�� �ִ� ������ ȭ�� ������Ʈ
                kUpdateScreenByWindowArea( qwWindowID, &( stButtonArea ) );
            }
            break;

            // Ű �̺�Ʈ ó��
        case EVENT_KEY_DOWN:
            pstKeyEvent = &( stReceivedEvent.stKeyEvent );
            
            // �齺���̽�(Backspace) Ű�� ���Ե� ���ڸ� ����
            if( ( pstKeyEvent->bASCIICode == KEY_BACKSPACE ) &&
                ( iFileNameLength > 0 ) )
            {
                // ���ۿ� ���Ե� ������ ���ڸ� ����
                vcFileName[ iFileNameLength ] = '\0';
                iFileNameLength--;
                
                // �Էµ� ������ ����Ʈ �ڽ��� ǥ��
                kDrawFileName( qwWindowID, &stEditBoxArea, vcFileName, 
                        iFileNameLength );
            }
            // ����(Enter) Ű�� �̹��� ��� ��ư�� ���� ������ ó��
            else if( ( pstKeyEvent->bASCIICode == KEY_ENTER ) &&
                     ( iFileNameLength > 0 ) )
            {
                // ��ư�� XY ��ǥ�� ȭ�� ��ǥ�� ��ȯ�Ͽ� ���콺 �̺�Ʈ�� ��ǥ�� ���
                stClientXY.iX = stButtonArea.iX1 + 1;
                stClientXY.iY = stButtonArea.iY1 + 1;                
                kConvertPointClientToScreen( qwWindowID, &stClientXY, &stScreenXY );
                
                // �̹��� ��� ��ư�� ���콺 ���� ��ư�� ���� ��ó�� ���콺 �̺�Ʈ�� ����
                kSetMouseEvent( qwWindowID, EVENT_MOUSE_LBUTTONDOWN, 
                                stScreenXY.iX + 1, stScreenXY.iY + 1, 0, &stSendEvent );
                kSendEventToWindow( qwWindowID, &stSendEvent );
            }
            // ESC Ű�� ������ ���� ��ư�� ���� ������ ó��
            else if( pstKeyEvent->bASCIICode == KEY_ESC )
            {
                // ������ �ݱ� �̺�Ʈ�� ������� ����
                kSetWindowEvent( qwWindowID, EVENT_WINDOW_CLOSE, &stSendEvent );
                kSendEventToWindow( qwWindowID, &stSendEvent );
            }
            // �� �� Ű�� ���� �̸� ���ۿ� ������ �ִ� ��츸 ���ۿ� ����
            else if( ( pstKeyEvent->bASCIICode <= 128 ) && 
                     ( pstKeyEvent->bASCIICode != KEY_BACKSPACE ) &&
                     ( iFileNameLength < FILESYSTEM_MAXFILENAMELENGTH) )
            {
                // �Էµ� Ű�� ���� �̸� ������ �������� ����
                vcFileName[ iFileNameLength ] = pstKeyEvent->bASCIICode;
                iFileNameLength++;
                
                // �Էµ� ������ ����Ʈ �ڽ��� ǥ��
                kDrawFileName( qwWindowID, &stEditBoxArea, vcFileName, 
                        iFileNameLength );
            }            
            break;

            // ������ �̺�Ʈ ó��
        case EVENT_WINDOW_CLOSE:
            if( stReceivedEvent.qwType == EVENT_WINDOW_CLOSE )
            {
                // ������ ����
                kDeleteWindow( qwWindowID );
                return 0;
            }
            break;
            
            // �� �� ����
        default:
            // ���⿡ �� �� ���� �̺�Ʈ ó�� �ڵ� �ֱ�
            break;
        }
    }

	return 1;
}

/**
 *  ����Ʈ �ڽ� ������ ���ڸ� ���
 */
static void kDrawFileName( QWORD qwWindowID, RECT* pstArea, char *pcFileName, 
        int iNameLength )
{
    // ����Ʈ �ڽ��� ����� ��� ������� ä��
    kDrawRect( qwWindowID, pstArea->iX1 + 1, pstArea->iY1 + 1, pstArea->iX2 - 1, 
               pstArea-> iY2 - 1, WINDOW_COLOR_BACKGROUND, TRUE );
    
    // ���� �̸��� ���
    kDrawText( qwWindowID, pstArea->iX1 + 2, pstArea->iY1 + 2, RGB( 0, 0, 0 ), 
            WINDOW_COLOR_BACKGROUND, pcFileName, iNameLength );
    
    // ���� �̸��� ���̰� ���� �ý����� ������ �ִ� ���̰� �ƴϸ� Ŀ���� ���
    if( iNameLength < FILESYSTEM_MAXFILENAMELENGTH)
    {
        kDrawText( qwWindowID, pstArea->iX1 + 2 + FONT_ENGLISHWIDTH * iNameLength,
            pstArea->iY1 + 2, RGB( 0, 0, 0 ), WINDOW_COLOR_BACKGROUND, "_", 1 );
    }
    
    // ����Ʈ �ڽ� ������ ȭ�� ������Ʈ
    kUpdateScreenByWindowArea( qwWindowID, pstArea );
}

/**
 *  JPEG ������ �о ���� ������ �����쿡 ǥ���ϰ� �̺�Ʈ�� ó��
 */
static bool kCreateImageViewerWindowAndExecute( QWORD qwMainWindowID, 
        const char* pcFileName )
{    
    struct dirent* pstEntry;
    DWORD dwFileSize;
    RECT stScreenArea;
    QWORD qwWindowID;
    WINDOW* pstWindow;
    BYTE* pbFileBuffer;
    COLOR* pstOutputBuffer;
    int iWindowWidth;
    FILE* fp;
    JPEG* pstJpeg;
    EVENT stReceivedEvent;
    KEYEVENT* pstKeyEvent;
    bool bExit;
    
    // �ʱ�ȭ
    fp = NULL;
    pbFileBuffer = NULL;
    pstOutputBuffer = NULL;
    qwWindowID = WINDOW_INVALIDID;
    
    //--------------------------------------------------------------------------
    //  ������ ���� �� �̹��� ���ڵ�
    //--------------------------------------------------------------------------
    // ���� �б�
    fp = fopen( pcFileName, "rb" );
    if( fp == NULL )
    {
        printf( "[ImageViewer] %s file open fail\n", pcFileName );
        return FALSE;
    }

	fseek(fp, 0, SEEK_END);
	dwFileSize = ftell(fp);
	fseek(fp, 0, SEEK_SET);

	if (dwFileSize == 0)
	{
		printf("[ImageViewer] %s file doesn't exist or size is zero\n", pcFileName);
		return FALSE;
	}
    
    // �޸𸮸� ���� ũ�⸸ŭ �Ҵ��ϰ� JPEG �ڷᱸ���� �Ҵ�
    pbFileBuffer = ( BYTE* ) new char[ dwFileSize ];
    pstJpeg = ( JPEG* ) new char[ sizeof( JPEG ) ];
    if( ( pbFileBuffer == NULL ) || ( pstJpeg == NULL ) )
    {
        printf( "[ImageViewer] Buffer allocation fail\n" );
        delete ( pbFileBuffer );
		delete ( pstJpeg );
        fclose( fp );
        return FALSE;
    }
    
    // ������ ���� �� JPEG ���� �������� Ȯ��
    if( ( fread( pbFileBuffer, 1, dwFileSize, fp ) != dwFileSize ) ||
        ( kJPEGInit( pstJpeg, pbFileBuffer, dwFileSize ) == FALSE ) )
    {
		printf( "[ImageViewer] Read fail or file is not JPEG format\n" );
		delete ( pbFileBuffer );
		delete ( pstJpeg );
        fclose( fp );
        return FALSE;
    }

    // ���ڵ� ��� ��¿� ���۸� ����
    pstOutputBuffer = (COLOR*)new char[ pstJpeg->width * pstJpeg->height * sizeof( COLOR ) ];
    // ���ڵ带 ������ �� ���������� ó���Ǿ��ٸ� �����츦 ����
    if( ( pstOutputBuffer != NULL ) &&
        ( kJPEGDecode( pstJpeg, pstOutputBuffer ) == TRUE ) )
    {
        // ��ü ȭ�� ������ ũ�⸦ ��ȯ
        kGetScreenArea( &stScreenArea );
        // �����츦 ����, �̹����� ũ��� ���� ǥ������ ũ�⸦ ���
        qwWindowID = kCreateWindow( ( stScreenArea.iX2 - pstJpeg->width ) / 2, 
                    ( stScreenArea.iY2 - pstJpeg->height ) / 2, pstJpeg->width, 
                    pstJpeg->height + WINDOW_TITLEBAR_HEIGHT, 
                    WINDOW_FLAGS_DEFAULT & ~WINDOW_FLAGS_SHOW | WINDOW_FLAGS_RESIZABLE, 
                    pcFileName );
    }
    
    // ������ ������ �����ϰų� ��� ���� �Ҵ� �Ǵ� ���ڵ��� �����ϸ� ����
    if( ( qwWindowID == WINDOW_INVALIDID ) || ( pstOutputBuffer == NULL ) )
    {
        printf ( "[ImageViewer] Window create fail or output buffer allocation fail\n" );
		delete ( pbFileBuffer );
		delete ( pstJpeg );
		delete ( pstOutputBuffer );
        kDeleteWindow( qwWindowID );
        return FALSE;
    }

    // �������� �ʺ� ���Ͽ� ���� ǥ���� ������ ������ ������ ȭ�� ���� ������ ���ڵ���
    // �̹����� ����
	kEnterCriticalSection();
    pstWindow = kGetWindowWithWindowLock( qwWindowID );
    if( pstWindow != NULL )
    {
        iWindowWidth = kGetRectangleWidth( &( pstWindow->stArea ) );
        memcpy( pstWindow->pstWindowBuffer + ( WINDOW_TITLEBAR_HEIGHT * 
                iWindowWidth ), pstOutputBuffer, pstJpeg->width * 
                pstJpeg->height * sizeof( COLOR ) );

        // ����ȭ ó��
        //kUnlock( &( pstWindow->stLock ) );
		
    }
	kLeaveCriticalSection();
    
    // ���� ���۸� �����ϰ� �����츦 ȭ�鿡 ǥ��
    delete ( pbFileBuffer );
    kShowWindow( qwWindowID, TRUE );
    
    //--------------------------------------------------------------------------
    //  ESC Ű�� ������ �ݱ� ��ư�� ó���ϴ� ������ �̺�Ʈ ����
    //--------------------------------------------------------------------------
    // ���������� �����츦 �����Ͽ� ǥ�������� ���� �̸� �Է� ������� ����
    kShowWindow( qwMainWindowID, FALSE );
    
    bExit = FALSE;
    while( bExit == FALSE )
    {
        // �̺�Ʈ ť���� �̺�Ʈ�� ����
        if( kReceiveEventFromWindowQueue( qwWindowID, &stReceivedEvent ) == FALSE )
        {
			//20180628
            ksleep( 0 );
            continue;
        }
        
        // ���ŵ� �̺�Ʈ�� Ÿ�Կ� ���� ������ ó��
        switch( stReceivedEvent.qwType )
        {
            // Ű �̺�Ʈ ó��
        case EVENT_KEY_DOWN:
            pstKeyEvent = &( stReceivedEvent.stKeyEvent );
            // ESC Ű�� ������ �׸��� ǥ���ϴ� �����츦 �����ϰ� ���� �̸� �Է� �����츦
            // ǥ���� �� ����
            if( pstKeyEvent->bASCIICode == KEY_ESC )
            {
                kDeleteWindow( qwWindowID );
                kShowWindow( qwMainWindowID, TRUE );
                bExit = TRUE;
            }                
            break;

            // ������ �̺�Ʈ ó��
            // ������ ũ�� ���� �̺�Ʈ�� ó��
        case EVENT_WINDOW_RESIZE:
            // ����� �����쿡 ���ڵ��� �̹����� ����
            kBitBlt( qwWindowID, 0, WINDOW_TITLEBAR_HEIGHT, pstOutputBuffer, 
                     pstJpeg->width, pstJpeg->height );
            // �����츦 �ѹ� �� ǥ���Ͽ� ������ ���ο� ���۵� �̹����� ȭ�鿡 ������Ʈ
            kShowWindow( qwWindowID, TRUE );
            break;
            
            // ������ �ݱ� �̺�Ʈ�� ó��
        case EVENT_WINDOW_CLOSE:
            // �ݱ� ��ư�� ������ �̹��� ��� �����츦 �����ϰ� ���� �̸� �Է� �����츦
            // ǥ���� �� ����
            if( stReceivedEvent.qwType == EVENT_WINDOW_CLOSE )
            {
                kDeleteWindow( qwWindowID );
                kShowWindow( qwMainWindowID, TRUE );
                bExit = TRUE;
            }
            break;
            
            // �� �� ����
        default:
            // ���⿡ �� �� ���� �̺�Ʈ ó�� �ڵ� �ֱ�
            break;
        }
    }

    // JPEG �̹��� ���� ���ڵ��� ����ߴ� ���۸� ��ȯ
    delete ( pstJpeg );
	delete ( pstOutputBuffer );
    
    return TRUE;
}
