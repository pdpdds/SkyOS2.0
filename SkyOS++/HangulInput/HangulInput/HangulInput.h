/**
 *  file    Main.c
 *  date    2010/03/30
 *  author  kkamagui
 *          Copyright(c)2008 All rights reserved by kkamagui
 *  brief   �ѱ� �Է¿� ���õ� ��� ����
 */

#ifndef __HANGULINPUT_H__
#define __HANGULINPUT_H__
#include "windef.h"

#define MAXOUTPUTLENGTH 256
////////////////////////////////////////////////////////////////////////////////
//
// ��ũ��
//
////////////////////////////////////////////////////////////////////////////////
// ���� �빮�ڸ� �ҹ��ڷ� �ٲ��ִ� ��ũ��
#define TOLOWER( x ) ( ( ( 'A' <= ( x ) ) && ( ( x ) <= 'Z' ) ) ? \
    ( ( x ) - 'A' + 'a' ) : ( x ) )

////////////////////////////////////////////////////////////////////////////////
//
// ����ü
//
////////////////////////////////////////////////////////////////////////////////
// �ѱ� �Է� ���̺��� �����ϴ� ����
typedef struct HangulInputTableItemStruct
{
    // �ѱ�
    char* pcHangul;

    // �ѱۿ� �����ϴ� Ű �Է�
    char* pcInput;
} HANGULINPUTITEM;

// �ѱ� �Է� ���̺��� �ε��� ���̺��� �����ϴ� ����
typedef struct HangulIndexTableItemStruct
{
    // �ѱ� ���ڸ� �Է��� �� ����ϴ� ù ��° Ű
    char cStartCharactor;

    // �ѱ� �Է� ���̺��� ���� �ε���
    DWORD dwIndex;
} HANGULINDEXITEM;

typedef struct BufferManagerStruct
{
	//--------------------------------------------------------------------------
	// �ѱ��� �����ϴµ� �ʿ��� �ʵ�
	//--------------------------------------------------------------------------
	// �ѱ� ������ ���� Ű �Է��� �����ϴ� ����
	char vcInputBuffer[20];
	int iInputBufferLength;

	// ���� ���� �ѱ۰� ������ �Ϸ�� �ѱ��� �����ϴ� ����
	char vcOutputBufferForProcessing[3];
	char vcOutputBufferForComplete[3];

	//--------------------------------------------------------------------------
	// ó���� �Ϸ�� ������ ������ ȭ�鿡 ����ϴµ� �ʿ��� �ʵ�
	//--------------------------------------------------------------------------
	// ������ ȭ�鿡 ����ϴ� ������ ����ִ� ����
	char vcOutputBuffer[MAXOUTPUTLENGTH];
	int iOutputBufferLength;
} BUFFERMANAGER;

////////////////////////////////////////////////////////////////////////////////
//
// �Լ�
//
////////////////////////////////////////////////////////////////////////////////
void ConvertJaumMoumToLowerCharactor(BYTE* pbInput);


////////////////////////////////////////////////////////////////////////////////
//
// �Լ�
//
////////////////////////////////////////////////////////////////////////////////
bool IsJaum( char cInput );
bool IsMoum( char cInput );
bool FindLongestHangulInTable( const char* pcInputBuffer, int iBufferCount,
                        int* piMatchIndex, int* piMatchLength );
bool ComposeHangul( char* pcInputBuffer, int* piInputBufferLength,
    char* pcOutputBufferForProcessing, char* pcOutputBufferForComplete );

#endif /* __HANGULINPUT_H__ */
