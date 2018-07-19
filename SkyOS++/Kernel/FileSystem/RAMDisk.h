#pragma once
#include "windef.h"
#include "MintFileSystem.h"


////////////////////////////////////////////////////////////////////////////////
//
// ��ũ��
//
////////////////////////////////////////////////////////////////////////////////
// �� ��ũ�� �� ���� ��. 8Mbyte ũ��� ����
#define RDD_TOTALSECTORCOUNT        ( 8 * 1024 * 1024 / 512)

////////////////////////////////////////////////////////////////////////////////
//
// ����ü
//
////////////////////////////////////////////////////////////////////////////////
// 1����Ʈ�� ����
#pragma pack( push, 1 )

// �� ��ũ�� �ڷᱸ���� �����ϴ� ����ü
typedef struct kRDDManagerStruct
{
    // �� ��ũ������ �Ҵ� ���� �޸� ����
    BYTE* pbBuffer;
    
    // �� ���� ��
    DWORD dwTotalSectorCount;
} RDDMANAGER;

#pragma pack( pop)

////////////////////////////////////////////////////////////////////////////////
//
//  �Լ�
//
////////////////////////////////////////////////////////////////////////////////
bool kInitializeRDD( DWORD dwTotalSectorCount );
bool kReadRDDInformation(bool bPrimary, bool bMaster,
        HDDINFORMATION* pstHDDInformation );
int kReadRDDSector(bool bPrimary, bool bMaster, DWORD dwLBA, int iSectorCount,
        char* pcBuffer );
int kWriteRDDSector(bool bPrimary, bool bMaster, DWORD dwLBA, int iSectorCount,
        char* pcBuffer );
