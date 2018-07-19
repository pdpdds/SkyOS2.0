/**
 *  file    ConsoleShell.h
 *  date    2009/01/31
 *  author  kkamagui 
 *          Copyright(c)2008 All rights reserved by kkamagui
 *  brief   �ܼ� �п� ���õ� ��� ����
 */

#ifndef __CONSOLESHELL_H__
#define __CONSOLESHELL_H__

#include "windef.h"

////////////////////////////////////////////////////////////////////////////////
//
// ��ũ��
//
////////////////////////////////////////////////////////////////////////////////
#define CONSOLESHELL_MAXCOMMANDBUFFERCOUNT  300
#define CONSOLESHELL_PROMPTMESSAGE          "MINT64>"

// ���ڿ� �����͸� �Ķ���ͷ� �޴� �Լ� ������ Ÿ�� ����
typedef void ( * CommandFunction ) ( const char* pcParameter );

// ��Ű���� �ñ׳�ó
#define PACKAGESIGNATURE    "MINT64OSPACKAGE "

// ���� �̸��� �ִ� ����, Ŀ���� FILESYSTEM_MAXFILENAMELENGTH�� ����
#define MAXFILENAMELENGTH   24

////////////////////////////////////////////////////////////////////////////////
//
// ����ü
//
////////////////////////////////////////////////////////////////////////////////
// 1����Ʈ�� ����
#pragma pack( push, 1 )

// ���� Ŀ�ǵ带 �����ϴ� �ڷᱸ��
typedef struct kShellCommandEntryStruct
{
    // Ŀ�ǵ� ���ڿ�
    char* pcCommand;
    // Ŀ�ǵ��� ����
    char* pcHelp;
    // Ŀ�ǵ带 �����ϴ� �Լ��� ������
    CommandFunction pfFunction;
} SHELLCOMMANDENTRY;

// �Ķ���͸� ó���ϱ����� ������ �����ϴ� �ڷᱸ��
typedef struct kParameterListStruct
{
    // �Ķ���� ������ ��巹��
    const char* pcBuffer;
    // �Ķ������ ����
    int iLength;
    // ���� ó���� �Ķ���Ͱ� �����ϴ� ��ġ
    int iCurrentPosition;
} PARAMETERLIST;

// ��Ű�� ��� ������ �� ���� ������ �����ϴ� �ڷᱸ��
typedef struct PackageItemStruct
{
    // ���� �̸�
    char vcFileName[ MAXFILENAMELENGTH ];

    // ������ ũ��
    DWORD dwFileLength;
} PACKAGEITEM;

// ��Ű�� ��� �ڷᱸ��
typedef struct PackageHeaderStruct
{
    // MINT64 OS�� ��Ű�� ������ ��Ÿ���� �ñ׳�ó
    char vcSignature[ 16 ];

    // ��Ű�� ����� ��ü ũ��
    DWORD dwHeaderSize;

    // ��Ű�� �������� ���� ��ġ
    PACKAGEITEM vstItem[ 0 ];
} PACKAGEHEADER;

#pragma pack( pop )

////////////////////////////////////////////////////////////////////////////////
//
// �Լ�
//
////////////////////////////////////////////////////////////////////////////////
// ���� �� �ڵ�
DWORD WINAPI kStartConsoleShell(LPVOID parameter);
void kExecuteCommand( const char* pcCommandBuffer );
void kInitializeParameter( PARAMETERLIST* pstList, const char* pcParameter );
int kGetNextParameter( PARAMETERLIST* pstList, char* pcParameter );

// Ŀ�ǵ带 ó���ϴ� �Լ�
static void kHelp( const char* pcParameterBuffer );
static void kCls( const char* pcParameterBuffer );
static void kShowTotalRAMSize( const char* pcParameterBuffer );
static void kShutdown( const char* pcParamegerBuffer );


#endif /*__CONSOLESHELL_H__*/
