/**
 *  file    Console.c
 *  date    2009/01/31
 *  author  kkamagui 
 *          Copyright(c)2008 All rights reserved by kkamagui
 *  brief   �ֿܼ� ���õ� �ҽ� ����
 */

#include <stdarg.h>
#include "Console.h"
#include "Keyboard.h"
//#include "Utility.h"
#include "Console.h"
#include "memory.h"
#include "sprintf.h"
#include "string.h"
#include "Mouse.h"
//#include "AssemblyUtility.h"

// �ܼ��� ������ �����ϴ� �ڷᱸ��
CONSOLEMANAGER gs_stConsoleManager = { 0, };

// �׷��� ���� �������� �� ����ϴ� ȭ�� ���� ����
static CHARACTER gs_vstScreenBuffer[ CONSOLE_WIDTH * CONSOLE_HEIGHT ];

// �׷��� ���� �������� �� GUI �ܼ� �� ������� ���޵� Ű �̺�Ʈ�� �ܼ� �� �½�ũ�� 
// �����ϴ� ť ����
static KEYDATA gs_vstKeyQueueBuffer[ CONSOLE_GUIKEYQUEUE_MAXCOUNT ];

/**
 *  �ܼ� �ʱ�ȭ
 */
void kInitializeConsole( int iX, int iY )
{
    // �ܼ� �ڷᱸ�� �ʱ�ȭ
    memset( &gs_stConsoleManager, 0, sizeof( gs_stConsoleManager ) );
    // ȭ�� ���� �ʱ�ȭ
	memset( &gs_vstScreenBuffer, 0, sizeof( gs_vstScreenBuffer ) );
    
	//if( kIsGraphicMode() == FALSE )
    if( 0 )
    {
        // �׷��� ���� ������ ���� �ƴϸ� ���� �޸𸮸� ȭ�� ���۷� ����
        gs_stConsoleManager.pstScreenBuffer = (CHARACTER* ) CONSOLE_VIDEOMEMORYADDRESS;
    }
    else
    {
        // �׷��� ���� ���������� �׷��� ���� ȭ�� ���۸� ����
        gs_stConsoleManager.pstScreenBuffer = (CHARACTER*)gs_vstScreenBuffer;
        
        // �׷��� ��忡�� ����� Ű ť�� ���ؽ��� �ʱ�ȭ
        kInitializeQueue( &( gs_stConsoleManager.stKeyQueueForGUI ), gs_vstKeyQueueBuffer, 
                CONSOLE_GUIKEYQUEUE_MAXCOUNT, sizeof( KEYDATA ) );
       // kInitializeMutex( &( gs_stConsoleManager.stLock ) );
    }
    
    // Ŀ�� ��ġ ����
    kSetCursor( iX, iY );
}

/**
 *  Ŀ���� ��ġ�� ����
 *      ���ڸ� ����� ��ġ�� ���� ����
 */
void kSetCursor( int iX, int iY ) 
{
    int iLinearValue;
    int iOldX;
    int iOldY;
    int i;
    
    // Ŀ���� ��ġ�� ���
    iLinearValue = iY * CONSOLE_WIDTH + iX;

    // �ؽ�Ʈ ���� ���������� CRT ��Ʈ�ѷ��� Ŀ�� ��ġ�� ����
    //if( kIsGraphicMode() == FALSE )
	if(0)
    {/*
        // CRTC ��Ʈ�� ��巹�� ��������(��Ʈ 0x3D4)�� 0x0E�� �����Ͽ�
        // ���� Ŀ�� ��ġ �������͸� ����
        kOutPortByte( VGA_PORT_INDEX, VGA_INDEX_UPPERCURSOR );
        // CRTC ��Ʈ�� ������ ��������(��Ʈ 0x3D5)�� Ŀ���� ���� ����Ʈ�� ���
        kOutPortByte( VGA_PORT_DATA, iLinearValue >> 8 );
    
        // CRTC ��Ʈ�� ��巹�� ��������(��Ʈ 0x3D4)�� 0x0F�� �����Ͽ�
        // ���� Ŀ�� ��ġ �������͸� ����
        kOutPortByte( VGA_PORT_INDEX, VGA_INDEX_LOWERCURSOR );
        // CRTC ��Ʈ�� ������ ��������(��Ʈ 0x3D5)�� Ŀ���� ���� ����Ʈ�� ���
        kOutPortByte( VGA_PORT_DATA, iLinearValue & 0xFF );*/
    }
    // �׷��� ���� ���������� ȭ�� ���ۿ� ����� Ŀ���� ��ġ�� �Ű���
    else
    {
        // ������ Ŀ���� �ִ� ��ġ�� �״�� Ŀ���� ���������� Ŀ���� ����
        for( i = 0 ; i < CONSOLE_WIDTH * CONSOLE_HEIGHT ; i++ )
        {
            // Ŀ���� ������ ����
            if( ( gs_stConsoleManager.pstScreenBuffer[ i ].bCharactor == '_' ) &&
                ( gs_stConsoleManager.pstScreenBuffer[ i ].bAttribute == 0x00 ) )
            {
                gs_stConsoleManager.pstScreenBuffer[ i ].bCharactor = ' ';
                gs_stConsoleManager.pstScreenBuffer[ i ].bAttribute = 
                    CONSOLE_DEFAULTTEXTCOLOR;
                break;
            }
        }
        
        // ���ο� ��ġ�� Ŀ���� ���
        gs_stConsoleManager.pstScreenBuffer[ iLinearValue ].bCharactor = '_';
        gs_stConsoleManager.pstScreenBuffer[ iLinearValue ].bAttribute = 0x00;
    }
    // ���ڸ� ����� ��ġ ������Ʈ
    gs_stConsoleManager.iCurrentPrintOffset = iLinearValue;
}

/**
 *  ���� Ŀ���� ��ġ�� ��ȯ
 */
void kGetCursor( int *piX, int *piY )
{
    *piX = gs_stConsoleManager.iCurrentPrintOffset % CONSOLE_WIDTH;
    *piY = gs_stConsoleManager.iCurrentPrintOffset / CONSOLE_WIDTH;
}



/**
*  ���ڿ��� ������ ������
*/
void kReverseString(char* pcBuffer)
{
	int iLength;
	int i;
	char cTemp;


	// ���ڿ��� ����� �߽����� ��/�츦 �ٲ㼭 ������ ������
	iLength = strlen(pcBuffer);
	for (i = 0; i < iLength / 2; i++)
	{
		cTemp = pcBuffer[i];
		pcBuffer[i] = pcBuffer[iLength - 1 - i];
		pcBuffer[iLength - 1 - i] = cTemp;
	}
}

/**
*  16���� ���� ���ڿ��� ��ȯ
*/
int kHexToString(QWORD qwValue, char* pcBuffer)
{
	QWORD i;
	QWORD qwCurrentValue;

	// 0�� ������ �ٷ� ó��
	if (qwValue == 0)
	{
		pcBuffer[0] = '0';
		pcBuffer[1] = '\0';
		return 1;
	}

	// ���ۿ� 1�� �ڸ����� 16, 256, ...�� �ڸ� ������ ���� ����
	for (i = 0; qwValue > 0; i++)
	{
		qwCurrentValue = qwValue % 16;
		if (qwCurrentValue >= 10)
		{
			pcBuffer[i] = 'A' + (qwCurrentValue - 10);
		}
		else
		{
			pcBuffer[i] = '0' + qwCurrentValue;
		}

		qwValue = qwValue / 16;
	}
	pcBuffer[i] = '\0';

	// ���ۿ� ����ִ� ���ڿ��� ����� ... 256, 16, 1�� �ڸ� ������ ����
	kReverseString(pcBuffer);
	return i;
}

/**
*  10���� ���� ���ڿ��� ��ȯ
*/
int kDecimalToString(long lValue, char* pcBuffer)
{
	long i;

	// 0�� ������ �ٷ� ó��
	if (lValue == 0)
	{
		pcBuffer[0] = '0';
		pcBuffer[1] = '\0';
		return 1;
	}

	// ���� �����̸� ��� ���ۿ� '-'�� �߰��ϰ� ����� ��ȯ
	if (lValue < 0)
	{
		i = 1;
		pcBuffer[0] = '-';
		lValue = -lValue;
	}
	else
	{
		i = 0;
	}

	// ���ۿ� 1�� �ڸ����� 10, 100, 1000 ...�� �ڸ� ������ ���� ����
	for (; lValue > 0; i++)
	{
		pcBuffer[i] = '0' + lValue % 10;
		lValue = lValue / 10;
	}
	pcBuffer[i] = '\0';

	// ���ۿ� ����ִ� ���ڿ��� ����� ... 1000, 100, 10, 1�� �ڸ� ������ ����
	if (pcBuffer[0] == '-')
	{
		// ������ ���� ��ȣ�� �����ϰ� ���ڿ��� ������
		kReverseString(&(pcBuffer[1]));
	}
	else
	{
		kReverseString(pcBuffer);
	}

	return i;
}

int kIToA(long lValue, char* pcBuffer, int iRadix)
{
	int iReturn;

	switch (iRadix)
	{
		// 16����
	case 16:
		iReturn = kHexToString(lValue, pcBuffer);
		break;

		// 10���� �Ǵ� ��Ÿ
	case 10:
	default:
		iReturn = kDecimalToString(lValue, pcBuffer);
		break;
	}

	return iReturn;
}

/**
*  vsprintf() �Լ��� ���� ����
*      ���ۿ� ���� ���ڿ��� ���� �����͸� ����
*/
int kVSPrintf(char* pcBuffer, const char* pcFormatString, va_list ap)
{
	QWORD i, j, k;
	int iBufferIndex = 0;
	int iFormatLength, iCopyLength;
	char* pcCopyString;
	QWORD qwValue;
	int iValue;
	double dValue;

	// ���� ���ڿ��� ���̸� �о ���ڿ��� ���̸�ŭ �����͸� ��� ���ۿ� ���
	iFormatLength = strlen(pcFormatString);
	for (i = 0; i < iFormatLength; i++)
	{
		// %�� �����ϸ� ������ Ÿ�� ���ڷ� ó��
		if (pcFormatString[i] == '%')
		{
			// % ������ ���ڷ� �̵�
			i++;
			switch (pcFormatString[i])
			{
				// ���ڿ� ���  
			case 's':
				// ���� ���ڿ� ����ִ� �Ķ���͸� ���ڿ� Ÿ������ ��ȯ
				pcCopyString = (char*)(va_arg(ap, char*));
				iCopyLength = strlen(pcCopyString);
				// ���ڿ��� ���̸�ŭ�� ��� ���۷� �����ϰ� ����� ���̸�ŭ 
				// ������ �ε����� �̵�
				memcpy(pcBuffer + iBufferIndex, pcCopyString, iCopyLength);
				iBufferIndex += iCopyLength;
				break;

				// ���� ���
			case 'c':
				// ���� ���ڿ� ����ִ� �Ķ���͸� ���� Ÿ������ ��ȯ�Ͽ� 
				// ��� ���ۿ� �����ϰ� ������ �ε����� 1��ŭ �̵�
				pcBuffer[iBufferIndex] = (char)(va_arg(ap, int));
				iBufferIndex++;
				break;

				// ���� ���
			case 'd':
			case 'i':
				// ���� ���ڿ� ����ִ� �Ķ���͸� ���� Ÿ������ ��ȯ�Ͽ�
				// ��� ���ۿ� �����ϰ� ����� ���̸�ŭ ������ �ε����� �̵�
				iValue = (int)(va_arg(ap, int));
				iBufferIndex += kIToA(iValue, pcBuffer + iBufferIndex, 10);
				break;

				// 4����Ʈ Hex ���
			case 'x':
			case 'X':
				// ���� ���ڿ� ����ִ� �Ķ���͸� DWORD Ÿ������ ��ȯ�Ͽ�
				// ��� ���ۿ� �����ϰ� ����� ���̸�ŭ ������ �ε����� �̵�
				qwValue = (DWORD)(va_arg(ap, DWORD)) & 0xFFFFFFFF;
				iBufferIndex += kIToA(qwValue, pcBuffer + iBufferIndex, 16);
				break;

				// 8����Ʈ Hex ���
			case 'q':
			case 'Q':
			case 'p':
				// ���� ���ڿ� ����ִ� �Ķ���͸� QWORD Ÿ������ ��ȯ�Ͽ�
				// ��� ���ۿ� �����ϰ� ����� ���̸�ŭ ������ �ε����� �̵�
				qwValue = (QWORD)(va_arg(ap, QWORD));
				iBufferIndex += kIToA(qwValue, pcBuffer + iBufferIndex, 16);
				break;

				// �Ҽ��� ��° �ڸ����� �Ǽ��� ���
			case 'f':
				dValue = (double)(va_arg(ap, double));
				// ��° �ڸ����� �ݿø� ó��
				dValue += 0.005;
				// �Ҽ��� ��° �ڸ����� ���ʷ� �����Ͽ� ���۸� ������
				pcBuffer[iBufferIndex] = '0' + (QWORD)(dValue * 100) % 10;
				pcBuffer[iBufferIndex + 1] = '0' + (QWORD)(dValue * 10) % 10;
				pcBuffer[iBufferIndex + 2] = '.';
				for (k = 0; ; k++)
				{
					// ���� �κ��� 0�̸� ����
					if (((QWORD)dValue == 0) && (k != 0))
					{
						break;
					}
					pcBuffer[iBufferIndex + 3 + k] = '0' + ((QWORD)dValue % 10);
					dValue = dValue / 10;
				}
				pcBuffer[iBufferIndex + 3 + k] = '\0';
				// ���� ����� ���̸�ŭ ������ ���̸� ������Ŵ
				kReverseString(pcBuffer + iBufferIndex);
				iBufferIndex += 3 + k;
				break;

				// ���� �ش����� ������ ���ڸ� �״�� ����ϰ� ������ �ε�����
				// 1��ŭ �̵�
			default:
				pcBuffer[iBufferIndex] = pcFormatString[i];
				iBufferIndex++;
				break;
			}
		}
		// �Ϲ� ���ڿ� ó��
		else
		{
			// ���ڸ� �״�� ����ϰ� ������ �ε����� 1��ŭ �̵�
			pcBuffer[iBufferIndex] = pcFormatString[i];
			iBufferIndex++;
		}
	}

	// NULL�� �߰��Ͽ� ������ ���ڿ��� ����� ����� ������ ���̸� ��ȯ
	pcBuffer[iBufferIndex] = '\0';
	return iBufferIndex;
}

void kPrintf( const char* pcFormatString, ... )
{
    va_list ap;
    char vcBuffer[ 1024 ];
    int iNextPrintOffset;

    // ���� ���� ����Ʈ�� ����ؼ� vsprintf()�� ó��
    va_start( ap, pcFormatString );
	kVSPrintf( vcBuffer, pcFormatString, ap );
    va_end( ap );
    
    // ���� ���ڿ��� ȭ�鿡 ���
    iNextPrintOffset = kConsolePrintString( vcBuffer );
    
    // Ŀ���� ��ġ�� ������Ʈ
    kSetCursor( iNextPrintOffset % CONSOLE_WIDTH, iNextPrintOffset / CONSOLE_WIDTH );
}

/**
 *  \n, \t�� ���� ���ڰ� ���Ե� ���ڿ��� ����� ��, ȭ����� ���� ����� ��ġ�� 
 *  ��ȯ
 */
int kConsolePrintString( const char* pcBuffer )
{
    CHARACTER* pstScreen;
    int i, j;
    int iLength;
    int iPrintOffset;

    // ȭ�� ���۸� ����
    pstScreen = gs_stConsoleManager.pstScreenBuffer;
    
    // ���ڿ��� ����� ��ġ�� ����
    iPrintOffset = gs_stConsoleManager.iCurrentPrintOffset;

    // ���ڿ��� ���̸�ŭ ȭ�鿡 ���
    iLength = strlen( pcBuffer );    
    for( i = 0 ; i < iLength ; i++ )
    {
        // ���� ó��
        if( pcBuffer[ i ] == '\n' )
        {
            // ����� ��ġ�� 80�� ��� �÷����� �ű�
            // ���� ������ ���� ���ڿ��� ����ŭ ���ؼ� ���� �������� ��ġ��Ŵ
            iPrintOffset += ( CONSOLE_WIDTH - ( iPrintOffset % CONSOLE_WIDTH ) );
        }
        // �� ó��
        else if( pcBuffer[ i ] == '\t' )
        {
            // ����� ��ġ�� 8�� ��� �÷����� �ű�
            iPrintOffset += ( 8 - ( iPrintOffset % 8 ) );
        }
        // �Ϲ� ���ڿ� ���
        else
        {
            // ���� �޸𸮿� ���ڿ� �Ӽ��� �����Ͽ� ���ڸ� ����ϰ�
            // ����� ��ġ�� �������� �̵�
            pstScreen[ iPrintOffset ].bCharactor = pcBuffer[ i ];
            pstScreen[ iPrintOffset ].bAttribute = CONSOLE_DEFAULTTEXTCOLOR;
            iPrintOffset++;
        }
        
        // ����� ��ġ�� ȭ���� �ִ�(80 * 25)�� ������� ��ũ�� ó��
        if( iPrintOffset >= ( CONSOLE_HEIGHT * CONSOLE_WIDTH ) )
        {
            // ���� ������ ������ �������� �� �� ���� ����
            memcpy( pstScreen, pstScreen + CONSOLE_WIDTH,
                     ( CONSOLE_HEIGHT - 1 ) * CONSOLE_WIDTH * sizeof( CHARACTER ) );

            // ���� ������ ������ �������� ä��
            for( j = ( CONSOLE_HEIGHT - 1 ) * ( CONSOLE_WIDTH ) ; 
                 j < ( CONSOLE_HEIGHT * CONSOLE_WIDTH ) ; j++ )
            {
                // ���� ���
                pstScreen[ j ].bCharactor = ' ';
                pstScreen[ j ].bAttribute = CONSOLE_DEFAULTTEXTCOLOR;
            }
            
            // ����� ��ġ�� ���� �Ʒ��� ������ ó������ ����
            iPrintOffset = ( CONSOLE_HEIGHT - 1 ) * CONSOLE_WIDTH;
        }
    }
    return iPrintOffset;
}

/**
 *  ��ü ȭ���� ����
 */
void kClearScreen( void )
{
    CHARACTER* pstScreen;
    int i;
    
    // ȭ�� ���۸� ����
    pstScreen = gs_stConsoleManager.pstScreenBuffer;
    
    // ȭ�� ��ü�� �������� ä���, Ŀ���� ��ġ�� 0, 0���� �ű�
    for( i = 0 ; i < CONSOLE_WIDTH * CONSOLE_HEIGHT ; i++ )
    {
        pstScreen[ i ].bCharactor = ' ';
        pstScreen[ i ].bAttribute = CONSOLE_DEFAULTTEXTCOLOR;
    }
    
    // Ŀ���� ȭ�� ������� �̵�
    kSetCursor( 0, 0 );
}

/**
 *  getch() �Լ��� ����
 */
BYTE kGetCh( void )
{
    KEYDATA stData;
    
    // Ű�� ������ ������ �����
    while( 1 )
    {
        // �׷��� ��尡 �ƴ� ���� Ŀ���� Ű ť���� ���� ������
       // if( kIsGraphicMode() == FALSE )
		if (0)
        {
            // Ű ť�� �����Ͱ� ���ŵ� ������ ���
            while( kGetKeyFromKeyQueue( &stData ) == FALSE )
            {
               // kSchedule();
            }
        }
        // �׷��� ����� ���� �׷��� ���� Ű ť���� ���� ������
        else
        {
            while( kGetKeyFromGUIKeyQueue( &stData ) == FALSE )
            {
                // �׷��� ��忡�� �����ϴ� �߿� �� �½�ũ�� �����ؾߵ� ��� ������ ����
                if( gs_stConsoleManager.bExit == TRUE )
                {
                    return 0xFF;
                }
                //kSchedule();
            }
        }
        
        // Ű�� ���ȴٴ� �����Ͱ� ���ŵǸ� ASCII �ڵ带 ��ȯ
        if( stData.bFlags & KEY_FLAGS_DOWN )
        {
            return stData.bASCIICode;
        }
    }
}

/**
 *  ���ڿ��� X, Y ��ġ�� ���
 */
void kPrintStringXY( int iX, int iY, const char* pcString )
{
    CHARACTER* pstScreen;
    int i;
    
    // ȭ�� ���۸� ����
    pstScreen = gs_stConsoleManager.pstScreenBuffer;
  
    // ���� ����� ��ġ�� ���
    pstScreen += ( iY * CONSOLE_WIDTH ) + iX;
    // ���ڿ��� ���̸�ŭ ������ ���鼭 ���ڿ� �Ӽ��� ����
    for( i = 0 ; pcString[ i ] != 0 ; i++ )
    {
        pstScreen[ i ].bCharactor = pcString[ i ];
        pstScreen[ i ].bAttribute = CONSOLE_DEFAULTTEXTCOLOR;
    }
}

/**
 *  �ܼ��� �����ϴ� �ڷᱸ���� ��ȯ
 */
CONSOLEMANAGER* kGetConsoleManager( void )
{
    return &gs_stConsoleManager;
}

/**
 *  �׷��� ���� Ű ť���� Ű �����͸� ����
 */
bool kGetKeyFromGUIKeyQueue( KEYDATA* pstData )
{
    bool bResult;
    
    // ť�� �����Ͱ� ������ ����
    if( kIsQueueEmpty( &( gs_stConsoleManager.stKeyQueueForGUI ) ) == TRUE )
    {
        return FALSE;
    }
    
    // ����ȭ ó��
   // kLock( &( gs_stConsoleManager.stLock ) );

    // ť���� �����͸� ����
	kEnterCriticalSection();
    bResult = kGetQueue( &( gs_stConsoleManager.stKeyQueueForGUI ), pstData );
	kLeaveCriticalSection();
    // ����ȭ ó��
   // kUnlock( &( gs_stConsoleManager.stLock ) );
    
    return bResult;
}

/**
 *  �׷��� ���� Ű ť�� �����͸� ����
 */
bool kPutKeyToGUIKeyQueue( KEYDATA* pstData )
{
    bool bResult;
    
    // ť�� �����Ͱ� ���� á���� ����
    if( kIsQueueFull( &( gs_stConsoleManager.stKeyQueueForGUI ) ) == TRUE )
    {
        return FALSE;
    }
    
    // ����ȭ ó��
    //kLock( &( gs_stConsoleManager.stLock ) );

    // ť�� �����͸� ����
	kEnterCriticalSection();
    bResult = kPutQueue( &( gs_stConsoleManager.stKeyQueueForGUI ), pstData );
	kLeaveCriticalSection();
    // ����ȭ ó��
    //kUnlock( &( gs_stConsoleManager.stLock ) );
    
    return bResult;
}

/**
 *  �ܼ� �� �½�ũ ���� �÷��׸� ����
 */
void kSetConsoleShellExitFlag( bool bFlag )
{
    gs_stConsoleManager.bExit = bFlag;
}
