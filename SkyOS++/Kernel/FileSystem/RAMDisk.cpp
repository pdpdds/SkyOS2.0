/**
 *  file    RAMDisk.c
 *  date    2009/05/22
 *  author  kkamagui 
 *          Copyright(c)2008 All rights reserved by kkamagui
 *  brief   �� ��ũ�� ���õ� �Լ��� ������ �ҽ� ����
 */

#include "RAMDisk.h"
#include "string.h"
#include "memory.h"
#include "Math.h"

// �� ��ũ�� �����ϴ� �ڷᱸ��
static RDDMANAGER gs_stRDDManager;

/**
 *  �� ��ũ ����̽� ����̹� �ʱ�ȭ �Լ�
 */
bool kInitializeRDD( DWORD dwTotalSectorCount )
{
    // �ڷᱸ�� �ʱ�ȭ
    memset( &gs_stRDDManager, 0, sizeof( gs_stRDDManager ) );
    
    // �� ��ũ�� ����� �޸𸮸� �Ҵ�
    gs_stRDDManager.pbBuffer = new BYTE[dwTotalSectorCount * 512];
    if( gs_stRDDManager.pbBuffer == NULL )
    {
        return FALSE;
    }
    
    // �� ���� ���� ����ȭ ��ü�� ����
    gs_stRDDManager.dwTotalSectorCount = dwTotalSectorCount;
 
    return TRUE;
}

/**
 *  �� ��ũ�� ������ ��ȯ
 */
bool kReadRDDInformation(bool bPrimary, bool bMaster,
        HDDINFORMATION* pstHDDInformation )
{
    // �ڷᱸ�� �ʱ�ȭ
	memset( pstHDDInformation, 0, sizeof( HDDINFORMATION ) );
    
    // �� ���� ���� �ø��� ��ȣ, �׸��� �� ��ȣ�� ����
    pstHDDInformation->dwTotalSectors = gs_stRDDManager.dwTotalSectorCount;
    memcpy( pstHDDInformation->vwSerialNumber, "0000-0001", 9 );
	memcpy( pstHDDInformation->vwModelNumber, "SkyOS RAM Disk v1.01", 20 );

    return TRUE;
}

/**
 *  �� ��ũ���� ���� ���͸� �о ��ȯ
 */
int kReadRDDSector(bool bPrimary, bool bMaster, DWORD dwLBA, int iSectorCount,
        char* pcBuffer )
{
    int iRealReadCount = 0;
    
    // LBA ��巹������ ������ ���� �� �ִ� ���� ���� �о�� �� ���� ���� ���ؼ�
    // ������ ���� �� �ִ� ���� ���
    iRealReadCount = (int)MIN( gs_stRDDManager.dwTotalSectorCount - (dwLBA + (DWORD)iSectorCount), (DWORD)iSectorCount );

    // �� ��ũ �޸𸮿��� �����͸� ������ ���� ���� ����ŭ �����ؼ� ��ȯ
    memcpy( pcBuffer, gs_stRDDManager.pbBuffer + ( dwLBA * 512 ), 
             iRealReadCount * 512 );
    
    return iRealReadCount;
}

/**
 *  �� ��ũ�� ���� ���͸� ��
 */
int kWriteRDDSector(bool bPrimary, bool bMaster, DWORD dwLBA, int iSectorCount,
        char* pcBuffer )
{
    int iRealWriteCount;
    
    // LBA ��巹������ ������ �� �� �ִ� ���� ���� ��� �� ���� ���� ���ؼ�
    // ������ �� �� �ִ� ���� ���
    iRealWriteCount = (int)MIN( gs_stRDDManager.dwTotalSectorCount - (dwLBA + (DWORD)iSectorCount), (DWORD)iSectorCount );

    // �����͸� ������ �� ���� ����ŭ �� ��ũ �޸𸮿� ����
	memcpy( gs_stRDDManager.pbBuffer + ( dwLBA * 512 ), pcBuffer,
             iRealWriteCount * 512 );
    
    return iRealWriteCount;    
}

