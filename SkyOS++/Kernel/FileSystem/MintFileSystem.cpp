#include "MintFileSystem.h"
#include "SkyAPI.h"
#include "math.h"
#include "RAMDisk.h"
#include "SkyConsole.h"

// ���� �ý��� �ڷᱸ��
static FILESYSTEMMANAGER   gs_stFileSystemManager;
// ���� �ý��� �ӽ� ����
static BYTE gs_vbTempBuffer[ FILESYSTEM_SECTORSPERCLUSTER * 512 ];

// �ϵ� ��ũ ��� ���õ� �Լ� ������ ����
fReadHDDInformation gs_pfReadHDDInformation = NULL;
fReadHDDSector gs_pfReadHDDSector = NULL;
fWriteHDDSector gs_pfWriteHDDSector = NULL;

/**
 *  ���� �ý����� �ʱ�ȭ
 */
bool kInitializeRDDFileSystem( void )
{	
    if( kInitializeRDD( RDD_TOTALSECTORCOUNT ) == TRUE )
    {
        // �ʱ�ȭ�� �����ϸ� �Լ� �����͸� �� ��ũ�� �Լ��� ����
        gs_pfReadHDDInformation = kReadRDDInformation;
        gs_pfReadHDDSector = kReadRDDSector;
        gs_pfWriteHDDSector = kWriteRDDSector;
        
        // �� ��ũ�� �����Ͱ� �������� �����Ƿ� �Ź� ���� �ý����� ������
        if( kFormat() == false )
        {
            return false;
        }   
    }
    else
    {
        return false;
    }
    
    // ���� �ý��� ����
    if( kMount() == false )
    {
        return false;
    }    

	// �ڵ��� ���� ������ �Ҵ�
	gs_stFileSystemManager.pstHandlePool = (MFILE*) new char[FILESYSTEM_HANDLE_MAXCOUNT * sizeof(MFILE)];

	// �޸� �Ҵ��� �����ϸ� �ϵ� ��ũ�� �νĵ��� ���� ������ ����
	if (gs_stFileSystemManager.pstHandlePool == nullptr)
	{
		gs_stFileSystemManager.bMounted = false;
		return false;
	}

    return TRUE;
}

//==============================================================================
//  ������ �Լ�(Low Level Function)
//==============================================================================
/**
 *  �ϵ� ��ũ�� MBR�� �о MINT ���� �ý������� Ȯ��
 *      MINT ���� �ý����̶�� ���� �ý��ۿ� ���õ� ���� ������ �о
 *      �ڷᱸ���� ����
 */
bool kMount( void )
{
    MBR* pstMBR;
    
    // ����ȭ ó��
	kEnterCriticalSection();

    // MBR�� ����
    if( gs_pfReadHDDSector( TRUE, TRUE, 0, 1, (char*)gs_vbTempBuffer ) == FALSE )
    {
        // ����ȭ ó��
		kLeaveCriticalSection();
        return FALSE;
    }
    
    // �ñ׳�ó�� Ȯ���Ͽ� ���ٸ� �ڷᱸ���� �� ������ ���� ���� ����
    pstMBR = ( MBR* ) gs_vbTempBuffer;
    if( pstMBR->dwSignature != FILESYSTEM_SIGNATURE )
    {
        // ����ȭ ó��
		kLeaveCriticalSection();
        return FALSE;
    }
    
    // ���� �ý��� �ν� ����
    gs_stFileSystemManager.bMounted = TRUE;
    
    // �� ������ ���� LBA ��巹���� ���� ���� ���
    gs_stFileSystemManager.dwReservedSectorCount = pstMBR->dwReservedSectorCount;
    gs_stFileSystemManager.dwClusterLinkAreaStartAddress =
        pstMBR->dwReservedSectorCount + 1;
    gs_stFileSystemManager.dwClusterLinkAreaSize = pstMBR->dwClusterLinkSectorCount;
    gs_stFileSystemManager.dwDataAreaStartAddress = 
        pstMBR->dwReservedSectorCount + pstMBR->dwClusterLinkSectorCount + 1;
    gs_stFileSystemManager.dwTotalClusterCount = pstMBR->dwTotalClusterCount;

    // ����ȭ ó��
	kLeaveCriticalSection();
    return TRUE;
}

/**
 *  �ϵ� ��ũ�� ���� �ý����� ����
 */
bool kFormat( void )
{
    HDDINFORMATION* pstHDD;
    MBR* pstMBR;
    DWORD dwTotalSectorCount, dwRemainSectorCount;
    DWORD dwMaxClusterCount, dwClsuterCount;
    DWORD dwClusterLinkSectorCount;
    DWORD i;
    
    // ����ȭ ó��
	kEnterCriticalSection();

    //==========================================================================
    //  �ϵ� ��ũ ������ �о ��Ÿ ������ ũ��� Ŭ�������� ������ ���
    //==========================================================================
    // �ϵ� ��ũ�� ������ �� �ϵ� ��ũ�� �� ���� ���� ����
    pstHDD = ( HDDINFORMATION* ) gs_vbTempBuffer;
    if( gs_pfReadHDDInformation( TRUE, TRUE, pstHDD ) == FALSE )
    {
        // ����ȭ ó��
		kLeaveCriticalSection();
        return FALSE;
    }    
    dwTotalSectorCount = pstHDD->dwTotalSectors;
    
    // ��ü ���� ���� 4Kbyte, �� Ŭ������ ũ��� ������ �ִ� Ŭ������ ���� ���
    dwMaxClusterCount = dwTotalSectorCount / FILESYSTEM_SECTORSPERCLUSTER;
    
    // �ִ� Ŭ�������� ���� ���߾� Ŭ������ ��ũ ���̺��� ���� ���� ���
    // ��ũ �����ʹ� 4����Ʈ�̹Ƿ�, �� ���Ϳ��� 128���� ��. ���� �� ������
    // 128�� ���� �� �ø��Ͽ� Ŭ������ ��ũ�� ���� ���� ����
    dwClusterLinkSectorCount = ( dwMaxClusterCount + 127 ) / 128;
    
    // ����� ������ ���� ������� �����Ƿ�, ��ũ ��ü �������� MBR ������ Ŭ������
    // ��ũ ���̺� ������ ũ�⸦ �� �������� ���� ������ ������ ��
    // �ش� ������ Ŭ������ ũ��� ������ ���� Ŭ�������� ������ ����
    dwRemainSectorCount = dwTotalSectorCount - dwClusterLinkSectorCount - 1;
    dwClsuterCount = dwRemainSectorCount / FILESYSTEM_SECTORSPERCLUSTER;
    
    // ���� ��� ������ Ŭ������ ���� ���߾� �ٽ� �ѹ� ���
    dwClusterLinkSectorCount = ( dwClsuterCount + 127 ) / 128;

    //==========================================================================
    // ���� ������ MBR�� ���� ����, ��Ʈ ���͸� �������� ��� 0���� �ʱ�ȭ�Ͽ�
    // ���� �ý����� ����
    //==========================================================================
    // MBR ���� �б�
    if( gs_pfReadHDDSector( TRUE, TRUE, 0, 1, (char*)gs_vbTempBuffer ) == FALSE )
    {
        // ����ȭ ó��
		kLeaveCriticalSection();
        return FALSE;
    }        
    
    // ��Ƽ�� ������ ���� �ý��� ���� ����    
    pstMBR = ( MBR* ) gs_vbTempBuffer;
	memset( pstMBR->vstPartition, 0, sizeof( pstMBR->vstPartition ) );
    pstMBR->dwSignature = FILESYSTEM_SIGNATURE;
    pstMBR->dwReservedSectorCount = 0;
    pstMBR->dwClusterLinkSectorCount = dwClusterLinkSectorCount;
    pstMBR->dwTotalClusterCount = dwClsuterCount;
    
    // MBR ������ 1 ���͸� ��
    if( gs_pfWriteHDDSector( TRUE, TRUE, 0, 1, (char*)gs_vbTempBuffer ) == FALSE )
    {
        // ����ȭ ó��
		kLeaveCriticalSection();
        return FALSE;
    }
    
    // MBR ���ĺ��� ��Ʈ ���͸����� ��� 0���� �ʱ�ȭ
    memset( gs_vbTempBuffer, 0, 512 );
    for( i = 0 ; i < ( dwClusterLinkSectorCount + FILESYSTEM_SECTORSPERCLUSTER );
         i++ )
    {
        // ��Ʈ ���͸�(Ŭ������ 0)�� �̹� ���� �ý����� ����ϰ� �����Ƿ�,
        // �Ҵ�� ������ ǥ��
        if( i == 0 )
        {
            ( ( DWORD* ) ( gs_vbTempBuffer ) )[ 0 ] = FILESYSTEM_LASTCLUSTER;
        }
        else
        {
            ( ( DWORD* ) ( gs_vbTempBuffer ) )[ 0 ] = FILESYSTEM_FREECLUSTER;
        }
        
        // 1 ���;� ��
        if( gs_pfWriteHDDSector( TRUE, TRUE, i + 1, 1, (char*)gs_vbTempBuffer ) == FALSE )
        {
            // ����ȭ ó��
			kLeaveCriticalSection();
            return FALSE;
        }
    }    

    
    // ����ȭ ó��
	kLeaveCriticalSection();
    return TRUE;
}

/**
 *  ���� �ý��ۿ� ����� �ϵ� ��ũ�� ������ ��ȯ
 */
bool kGetHDDInformation( HDDINFORMATION* pstInformation)
{
	bool bResult;
    
    // ����ȭ ó��
	kEnterCriticalSection();
    
    bResult = gs_pfReadHDDInformation( TRUE, TRUE, pstInformation );
    
    // ����ȭ ó��
	kLeaveCriticalSection();
    
    return bResult;
}

/**
 *  Ŭ������ ��ũ ���̺� ���� �����¿��� �� ���͸� ����
 */
static bool kReadClusterLinkTable(DWORD dwOffset, BYTE* pbBuffer)
{

	return kInternalReadClusterLinkTableWithoutCache(dwOffset, pbBuffer);	
}

/**
 *  Ŭ������ ��ũ ���̺� ���� �����¿��� �� ���͸� ����
 *      ���������� ����ϴ� �Լ�, ĳ�� ��� ����
 */
static bool kInternalReadClusterLinkTableWithoutCache( DWORD dwOffset,
        BYTE* pbBuffer )
{
    // Ŭ������ ��ũ ���̺� ������ ���� ��巹���� ����
    return gs_pfReadHDDSector( TRUE, TRUE, dwOffset + 
              gs_stFileSystemManager.dwClusterLinkAreaStartAddress, 1, (char*)pbBuffer );
}

/**
 *  Ŭ������ ��ũ ���̺� ���� �����¿� �� ���͸� ��
 */
static bool kWriteClusterLinkTable( DWORD dwOffset, BYTE* pbBuffer )
{
   return kInternalWriteClusterLinkTableWithoutCache( dwOffset, pbBuffer );
}

/**
 *  Ŭ������ ��ũ ���̺� ���� �����¿� �� ���͸� ��
 *      ���������� ����ϴ� �Լ�, ĳ�� ��� �� ��
 */
static bool kInternalWriteClusterLinkTableWithoutCache( DWORD dwOffset,
        BYTE* pbBuffer )
{
    // Ŭ������ ��ũ ���̺� ������ ���� ��巹���� ����
    return gs_pfWriteHDDSector( TRUE, TRUE, dwOffset + 
               gs_stFileSystemManager.dwClusterLinkAreaStartAddress, 1, (char*)pbBuffer );
}


/**
*  ������ ������ �����¿��� �� Ŭ�����͸� ����
*      ���������� ����ϴ� �Լ�, ĳ�� ��� �� ��
*/
static bool kInternalReadCluster(DWORD dwOffset, BYTE* pbBuffer)
{	
	// ������ ������ ���� ��巹���� ����
	return gs_pfReadHDDSector(TRUE, TRUE, (dwOffset * FILESYSTEM_SECTORSPERCLUSTER) +
		gs_stFileSystemManager.dwDataAreaStartAddress,
		FILESYSTEM_SECTORSPERCLUSTER, (char*)pbBuffer);
}

/**
 *  ������ ������ �����¿��� �� Ŭ�����͸� ����
 */
static bool kReadCluster( DWORD dwOffset, BYTE* pbBuffer )
{    
     return kInternalReadCluster( dwOffset, pbBuffer );	 
}



/**
 *  ������ ������ �����¿� �� Ŭ�����͸� ��
 */
static bool kWriteCluster(DWORD dwOffset, BYTE* pbBuffer)
{
	return kInternalWriteClusterWithoutCache(dwOffset, pbBuffer);	
}

/**
 *  ������ ������ �����¿� �� Ŭ�����͸� ��
 *      ���������� ����ϴ� �Լ�, ĳ�� ��� �� ��
 */
static bool kInternalWriteClusterWithoutCache( DWORD dwOffset, BYTE* pbBuffer )
{
    // ������ ������ ���� ��巹���� ����
    return  gs_pfWriteHDDSector( TRUE, TRUE, ( dwOffset * FILESYSTEM_SECTORSPERCLUSTER ) + 
              gs_stFileSystemManager.dwDataAreaStartAddress, 
              FILESYSTEM_SECTORSPERCLUSTER, (char*)pbBuffer );
}

/**
 *  Ŭ������ ��ũ ���̺� �������� �� Ŭ�����͸� �˻���
 */
static DWORD kFindFreeCluster( void )
{
    DWORD dwLinkCountInSector;
    DWORD dwLastSectorOffset, dwCurrentSectorOffset;
    DWORD i, j;
    
    // ���� �ý����� �ν����� �������� ����
    if( gs_stFileSystemManager.bMounted == FALSE )
    {
        return FILESYSTEM_LASTCLUSTER;
    }
    
    // ���������� Ŭ�����͸� �Ҵ��� Ŭ������ ��ũ ���̺��� ���� �������� ������
    dwLastSectorOffset = gs_stFileSystemManager.dwLastAllocatedClusterLinkSectorOffset;

    // ���������� �Ҵ��� ��ġ���� ������ ���鼭 �� Ŭ�����͸� �˻�
    for( i = 0 ; i < gs_stFileSystemManager.dwClusterLinkAreaSize ; i++ )
    {
        // Ŭ������ ��ũ ���̺��� ������ �����̸� ��ü ���͸�ŭ ���� ���� �ƴ϶�
        // ���� Ŭ�������� ����ŭ ������ ���ƾ� ��
        if( ( dwLastSectorOffset + i ) == 
            ( gs_stFileSystemManager.dwClusterLinkAreaSize - 1 ) )
        {
            dwLinkCountInSector = gs_stFileSystemManager.dwTotalClusterCount % 128; 
        }
        else
        {
            dwLinkCountInSector = 128;
        }
        
        // �̹��� �о�� �� Ŭ������ ��ũ ���̺��� ���� �������� ���ؼ� ����
        dwCurrentSectorOffset = ( dwLastSectorOffset + i ) % 
            gs_stFileSystemManager.dwClusterLinkAreaSize;
        if( kReadClusterLinkTable( dwCurrentSectorOffset, gs_vbTempBuffer ) == FALSE )
        {
            return FILESYSTEM_LASTCLUSTER;
        }
        
        // ���� ������ ������ ���鼭 �� Ŭ�����͸� �˻�
        for( j = 0 ; j < dwLinkCountInSector ; j++ )
        {
            if( ( ( DWORD* ) gs_vbTempBuffer )[ j ] == FILESYSTEM_FREECLUSTER )
            {
                break;
            }
        }
            
        // ã�Ҵٸ� Ŭ������ �ε����� ��ȯ
        if( j != dwLinkCountInSector )
        {
            // ���������� Ŭ�����͸� �Ҵ��� Ŭ������ ��ũ ���� ���� �������� ����
            gs_stFileSystemManager.dwLastAllocatedClusterLinkSectorOffset = 
                dwCurrentSectorOffset;
            
            // ���� Ŭ������ ��ũ ���̺��� �������� �����Ͽ� Ŭ������ �ε����� ���
            return ( dwCurrentSectorOffset * 128 ) + j;
        }
    }
    
    return FILESYSTEM_LASTCLUSTER;
}

/**
 *  Ŭ������ ��ũ ���̺� ���� ����
 */
static bool kSetClusterLinkData( DWORD dwClusterIndex, DWORD dwData )
{
    DWORD dwSectorOffset;
    
    // ���� �ý����� �ν����� �������� ����
    if( gs_stFileSystemManager.bMounted == FALSE )
    {
        return FALSE;
    }
    
    // �� ���Ϳ� 128���� Ŭ������ ��ũ�� ���Ƿ� 128�� ������ ���� �������� 
    // ���� �� ����
    dwSectorOffset = dwClusterIndex / 128;

    // �ش� ���͸� �о ��ũ ������ ������ ��, �ٽ� ����
    if( kReadClusterLinkTable( dwSectorOffset, gs_vbTempBuffer ) == FALSE )
    {
        return FALSE;
    }    
    
    ( ( DWORD* ) gs_vbTempBuffer )[ dwClusterIndex % 128 ] = dwData;

    if( kWriteClusterLinkTable( dwSectorOffset, gs_vbTempBuffer ) == FALSE )
    {
        return FALSE;
    }

    return TRUE;
}

/**
 *  Ŭ������ ��ũ ���̺��� ���� ��ȯ
 */
static bool kGetClusterLinkData( DWORD dwClusterIndex, DWORD* pdwData )
{
    DWORD dwSectorOffset;
    
    // ���� �ý����� �ν����� �������� ����
    if( gs_stFileSystemManager.bMounted == FALSE )
    {
        return FALSE;
    }
    
    // �� ���Ϳ� 128���� Ŭ������ ��ũ�� ���Ƿ� 128�� ������ ���� �������� 
    // ���� �� ����
    dwSectorOffset = dwClusterIndex / 128;
    
    if( dwSectorOffset > gs_stFileSystemManager.dwClusterLinkAreaSize )
    {
        return FALSE;
    }
    
    
    // �ش� ���͸� �о ��ũ ������ ��ȯ
    if( kReadClusterLinkTable( dwSectorOffset, gs_vbTempBuffer ) == FALSE )
    {
        return FALSE;
    }    

    *pdwData = ( ( DWORD* ) gs_vbTempBuffer )[ dwClusterIndex % 128 ];
    return TRUE;
}


/**
 *  ��Ʈ ���͸����� �� ���͸� ��Ʈ���� ��ȯ
 */
static int kFindFreeDirectoryEntry( void )
{
    DIRECTORYENTRY* pstEntry;
    int i;

    // ���� �ý����� �ν����� �������� ����
    if( gs_stFileSystemManager.bMounted == FALSE )
    {
        return -1;
    }

    // ��Ʈ ���͸��� ����
    if( kReadCluster( 0, gs_vbTempBuffer ) == FALSE )
    {
        return -1;
    }
    
    // ��Ʈ ���͸� �ȿ��� ������ ���鼭 �� ��Ʈ��, �� ���� Ŭ������ ��ȣ�� 0��
    // ��Ʈ���� �˻�
    pstEntry = ( DIRECTORYENTRY* ) gs_vbTempBuffer;
    for( i = 0 ; i < FILESYSTEM_MAXDIRECTORYENTRYCOUNT ; i++ )
    {
        if( pstEntry[ i ].dwStartClusterIndex == 0 )
        {
            return i;
        }
    }
    return -1;
}

/**
 *  ��Ʈ ���͸��� �ش� �ε����� ���͸� ��Ʈ���� ����
 */
static bool kSetDirectoryEntryData( int iIndex, DIRECTORYENTRY* pstEntry )
{
    DIRECTORYENTRY* pstRootEntry;
    
    // ���� �ý����� �ν����� ���߰ų� �ε����� �ùٸ��� ������ ����
    if( ( gs_stFileSystemManager.bMounted == FALSE ) ||
        ( iIndex < 0 ) || ( iIndex >= FILESYSTEM_MAXDIRECTORYENTRYCOUNT ) )
    {
        return FALSE;
    }

    // ��Ʈ ���͸��� ����
    if( kReadCluster( 0, gs_vbTempBuffer ) == FALSE )
    {
        return FALSE;
    }    
    
    // ��Ʈ ���͸��� �ִ� �ش� �����͸� ����
    pstRootEntry = ( DIRECTORYENTRY* ) gs_vbTempBuffer;
    memcpy( pstRootEntry + iIndex, pstEntry, sizeof( DIRECTORYENTRY ) );

    // ��Ʈ ���͸��� ��
    if( kWriteCluster( 0, gs_vbTempBuffer ) == FALSE )
    {
        return FALSE;
    }    
    return TRUE;
}

/**
 *  ��Ʈ ���͸��� �ش� �ε����� ��ġ�ϴ� ���͸� ��Ʈ���� ��ȯ
 */
static bool kGetDirectoryEntryData( int iIndex, DIRECTORYENTRY* pstEntry )
{
    DIRECTORYENTRY* pstRootEntry;
    
    // ���� �ý����� �ν����� ���߰ų� �ε����� �ùٸ��� ������ ����
    if( ( gs_stFileSystemManager.bMounted == FALSE ) ||
        ( iIndex < 0 ) || ( iIndex >= FILESYSTEM_MAXDIRECTORYENTRYCOUNT ) )
    {
        return FALSE;
    }

    // ��Ʈ ���͸��� ����
    if( kReadCluster( 0, gs_vbTempBuffer ) == FALSE )
    {
        return FALSE;
    }    
    
    // ��Ʈ ���͸��� �ִ� �ش� �����͸� ����
    pstRootEntry = ( DIRECTORYENTRY* ) gs_vbTempBuffer;
    memcpy( pstEntry, pstRootEntry + iIndex, sizeof( DIRECTORYENTRY ) );
    return TRUE;
}

/**
 *  ��Ʈ ���͸����� ���� �̸��� ��ġ�ϴ� ��Ʈ���� ã�Ƽ� �ε����� ��ȯ
 */
static int kFindDirectoryEntry( const char* pcFileName, DIRECTORYENTRY* pstEntry )
{
    DIRECTORYENTRY* pstRootEntry;
    int i;
    int iLength;

    // ���� �ý����� �ν����� �������� ����
    if( gs_stFileSystemManager.bMounted == FALSE )
    {
        return -1;
    }

    // ��Ʈ ���͸��� ����
    if( kReadCluster( 0, gs_vbTempBuffer ) == FALSE )
    {
        return -1;
    }
    
    iLength = strlen( pcFileName );
    // ��Ʈ ���͸� �ȿ��� ������ ���鼭 ���� �̸��� ��ġ�ϴ� ��Ʈ���� ��ȯ
    pstRootEntry = ( DIRECTORYENTRY* ) gs_vbTempBuffer;
    for( i = 0 ; i < FILESYSTEM_MAXDIRECTORYENTRYCOUNT ; i++ )
    {
        if( memcmp( pstRootEntry[ i ].vcFileName, pcFileName, iLength ) == 0 )
        {
			memcpy( pstEntry, pstRootEntry + i, sizeof( DIRECTORYENTRY ) );
            return i;
        }
    }
    return -1;
}

/**
 *  ���� �ý����� ������ ��ȯ
 */
void kGetFileSystemInformation( FILESYSTEMMANAGER* pstManager )
{
	memcpy( pstManager, &gs_stFileSystemManager, sizeof( gs_stFileSystemManager ) );
}

//==============================================================================
//  ����� �Լ�(High Level Function)
//==============================================================================
/**
 *  ����ִ� �ڵ��� �Ҵ�
 */
static void* kAllocateFileDirectoryHandle( void )
{
    int i;
	MFILE* pstFile;
    
    // �ڵ� Ǯ(Handle Pool)�� ��� �˻��Ͽ� ����ִ� �ڵ��� ��ȯ
    pstFile = gs_stFileSystemManager.pstHandlePool;
    for( i = 0 ; i < FILESYSTEM_HANDLE_MAXCOUNT ; i++ )
    {
        // ����ִٸ� ��ȯ
        if( pstFile->bType == FILESYSTEM_TYPE_FREE )
        {
            pstFile->bType = FILESYSTEM_TYPE_FILE;
            return pstFile;
        }
        
        // �������� �̵�
        pstFile++;
    }
    
    return NULL;
}

/**
 *  ����� �ڵ��� ��ȯ
 */
static void kFreeFileDirectoryHandle(MFILE* pstFile )
{
    // ��ü ������ �ʱ�ȭ
    memset( pstFile, 0, sizeof(MFILE) );
    
    // ����ִ� Ÿ������ ����
    pstFile->bType = FILESYSTEM_TYPE_FREE;
}

/**
 *  ������ ����
 */
static bool kCreateFile( const char* pcFileName, DIRECTORYENTRY* pstEntry,
        int* piDirectoryEntryIndex )
{
    DWORD dwCluster;
    
    // �� Ŭ�����͸� ã�Ƽ� �Ҵ�� ������ ����
    dwCluster = kFindFreeCluster();
    if( ( dwCluster == FILESYSTEM_LASTCLUSTER ) ||
        ( kSetClusterLinkData( dwCluster, FILESYSTEM_LASTCLUSTER ) == FALSE ) )
    {
        return FALSE;
    }

    // �� ���͸� ��Ʈ���� �˻�
    *piDirectoryEntryIndex = kFindFreeDirectoryEntry();
    if( *piDirectoryEntryIndex == -1 )
    {
        // ������ ��� �Ҵ� ���� Ŭ�����͸� ��ȯ�ؾ� ��
        kSetClusterLinkData( dwCluster, FILESYSTEM_FREECLUSTER );
        return FALSE;
    }
    
    // ���͸� ��Ʈ���� ����
    memcpy( pstEntry->vcFileName, pcFileName, strlen( pcFileName ) + 1 );
    pstEntry->dwStartClusterIndex = dwCluster;
    pstEntry->dwFileSize = 0;
    
    // ���͸� ��Ʈ���� ���
    if( kSetDirectoryEntryData( *piDirectoryEntryIndex, pstEntry ) == FALSE )
    {
        // ������ ��� �Ҵ� ���� Ŭ�����͸� ��ȯ�ؾ� ��
        kSetClusterLinkData( dwCluster, FILESYSTEM_FREECLUSTER );
        return FALSE;
    }
    return TRUE;
}

/**
 *  �Ķ���ͷ� �Ѿ�� Ŭ�����ͺ��� ������ ������ ����� Ŭ�����͸� ��� ��ȯ
 */
static bool kFreeClusterUntilEnd( DWORD dwClusterIndex )
{
    DWORD dwCurrentClusterIndex;
    DWORD dwNextClusterIndex;
    
    // Ŭ������ �ε����� �ʱ�ȭ
    dwCurrentClusterIndex = dwClusterIndex;
    
    while( dwCurrentClusterIndex != FILESYSTEM_LASTCLUSTER )
    {
        // ���� Ŭ�������� �ε����� ������
        if( kGetClusterLinkData( dwCurrentClusterIndex, &dwNextClusterIndex )
                == FALSE )
        {
            return FALSE;
        }
        
        // ���� Ŭ�����͸� �� ������ �����Ͽ� ����
        if( kSetClusterLinkData( dwCurrentClusterIndex, FILESYSTEM_FREECLUSTER )
                == FALSE )
        {
            return FALSE;
        }
        
        // ���� Ŭ������ �ε����� ���� Ŭ�������� �ε����� �ٲ�
        dwCurrentClusterIndex = dwNextClusterIndex;
    }
    
    return TRUE;
}

/**
 *  ������ ���ų� ���� 
 */
MFILE* kOpenFile( const char* pcFileName, const char* pcMode )
{
    DIRECTORYENTRY stEntry;
    int iDirectoryEntryOffset;
    int iFileNameLength;
    DWORD dwSecondCluster;
	MFILE* pstFile;

    // ���� �̸� �˻�
    iFileNameLength = strlen( pcFileName );
    if( ( iFileNameLength > ( sizeof( stEntry.vcFileName ) - 1 ) ) || 
        ( iFileNameLength == 0 ) )
    {
        return NULL;
    }
    
    // ����ȭ
	kEnterCriticalSection();
    
    //==========================================================================
    // ������ ���� �����ϴ°� Ȯ���ϰ�, ���ٸ� �ɼ��� ���� ������ ����
    //==========================================================================
    iDirectoryEntryOffset = kFindDirectoryEntry( pcFileName, &stEntry );
    if( iDirectoryEntryOffset == -1 )
    {
        // ������ ���ٸ� �б�(r, r+) �ɼ��� ����
        if( pcMode[ 0 ] == 'r' )
        {
            // ����ȭ
			kLeaveCriticalSection();
            return NULL;
        }
        
        // ������ �ɼǵ��� ������ ����
        if( kCreateFile( pcFileName, &stEntry, &iDirectoryEntryOffset ) == FALSE )
        {
            // ����ȭ
			kLeaveCriticalSection();
            return NULL;
        }
    }    
    //==========================================================================
    // ������ ������ ����� �ϴ� �ɼ��̸� ���Ͽ� ����� Ŭ�����͸� ��� �����ϰ�
    // ���� ũ�⸦ 0���� ����
    //==========================================================================
    else if( pcMode[ 0 ] == 'w' )
    {
        // ���� Ŭ�������� ���� Ŭ�����͸� ã��
        if( kGetClusterLinkData( stEntry.dwStartClusterIndex, &dwSecondCluster )
                == FALSE )
        {
            // ����ȭ
			kLeaveCriticalSection();
            return NULL;
        }
        
        // ���� Ŭ�����͸� ������ Ŭ�����ͷ� ����
        if( kSetClusterLinkData( stEntry.dwStartClusterIndex, 
               FILESYSTEM_LASTCLUSTER ) == FALSE )
        {
            // ����ȭ
			kLeaveCriticalSection();
            return NULL;
        }
        
        // ���� Ŭ�����ͺ��� ������ Ŭ�����ͱ��� ��� ����
        if( kFreeClusterUntilEnd( dwSecondCluster ) == FALSE )
        {
            // ����ȭ
			kLeaveCriticalSection();
            return NULL;
        }
       
        // ������ ������ ��� ��������Ƿ�, ũ�⸦ 0���� ����
        stEntry.dwFileSize = 0;
        if( kSetDirectoryEntryData( iDirectoryEntryOffset, &stEntry ) == FALSE )
        {
            // ����ȭ
			kLeaveCriticalSection();
            return NULL;
        }
    }
    
    //==========================================================================
    // ���� �ڵ��� �Ҵ� �޾� �����͸� ������ �� ��ȯ
    //==========================================================================
    // ���� �ڵ��� �Ҵ� �޾� ������ ����
    pstFile = (MFILE*)kAllocateFileDirectoryHandle();
    if( pstFile == NULL )
    {
        // ����ȭ
		kLeaveCriticalSection();
        return NULL;
    }
    
    // ���� �ڵ鿡 ���� ������ ����
    pstFile->bType = FILESYSTEM_TYPE_FILE;
    pstFile->stFileHandle.iDirectoryEntryOffset = iDirectoryEntryOffset;
    pstFile->stFileHandle.dwFileSize = stEntry.dwFileSize;
    pstFile->stFileHandle.dwStartClusterIndex = stEntry.dwStartClusterIndex;
    pstFile->stFileHandle.dwCurrentClusterIndex = stEntry.dwStartClusterIndex;
    pstFile->stFileHandle.dwPreviousClusterIndex = stEntry.dwStartClusterIndex;
    pstFile->stFileHandle.dwCurrentOffset = 0;
       
    // ���� �߰� �ɼ�(a)�� �����Ǿ� ������, ������ ������ �̵�
    if( pcMode[ 0 ] == 'a' )
    {
        kSeekFile( pstFile, 0, FILESYSTEM_SEEK_END );
    }

    // ����ȭ
	kLeaveCriticalSection();
    return pstFile;
}

/**
 *  ������ �о� ���۷� ����
 */
DWORD kReadFile( void* pvBuffer, DWORD dwSize, DWORD dwCount, MFILE* pstFile )
{
    DWORD dwTotalCount;
    DWORD dwReadCount;
    DWORD dwOffsetInCluster;
    DWORD dwCopySize;
    FILEHANDLE* pstFileHandle;
    DWORD dwNextClusterIndex;    

    // �ڵ��� ���� Ÿ���� �ƴϸ� ����
    if( ( pstFile == NULL ) ||
        ( pstFile->bType != FILESYSTEM_TYPE_FILE ) )
    {
        return 0;
    }

    pstFileHandle = &( pstFile->stFileHandle );
    
    // ������ ���̰ų� ������ Ŭ�������̸� ����
    if( ( pstFileHandle->dwCurrentOffset == pstFileHandle->dwFileSize ) ||
        ( pstFileHandle->dwCurrentClusterIndex == FILESYSTEM_LASTCLUSTER ) )
    {
        return 0;
    }

    // ���� ���� ���ؼ� ������ ���� �� �ִ� ���� ���
    dwTotalCount = MIN( dwSize * dwCount, pstFileHandle->dwFileSize - 
                        pstFileHandle->dwCurrentOffset );
    
    // ����ȭ
	kEnterCriticalSection();
    
    // ���� ����ŭ �� ���� ������ �ݺ�
    dwReadCount = 0;
    while( dwReadCount != dwTotalCount )
    {
        //======================================================================
        // Ŭ�����͸� �о ���ۿ� ����
        //======================================================================
        // ���� Ŭ�����͸� ����
        if( kReadCluster( pstFileHandle->dwCurrentClusterIndex, gs_vbTempBuffer )
                == FALSE )
        {
            break;
        }

        // Ŭ������ ������ ���� �����Ͱ� �����ϴ� �������� ���
        dwOffsetInCluster = pstFileHandle->dwCurrentOffset % FILESYSTEM_CLUSTERSIZE;
        
        // ���� Ŭ�����Ϳ� ������ �ִٸ� ���� Ŭ�����Ϳ��� ���� ��ŭ �а� ���� 
        // Ŭ�����ͷ� �̵�
        dwCopySize = MIN( FILESYSTEM_CLUSTERSIZE - dwOffsetInCluster, 
                          dwTotalCount - dwReadCount );
        memcpy( ( char* ) pvBuffer + dwReadCount, 
                gs_vbTempBuffer + dwOffsetInCluster, dwCopySize );

        // ���� ����Ʈ ���� ���� �������� ��ġ�� ����
        dwReadCount += dwCopySize;
        pstFileHandle->dwCurrentOffset += dwCopySize;

        //======================================================================
        // ���� Ŭ�����͸� ������ �� �о����� ���� Ŭ�����ͷ� �̵�
        //======================================================================
        if( ( pstFileHandle->dwCurrentOffset % FILESYSTEM_CLUSTERSIZE ) == 0 )
        {
            // ���� Ŭ�������� ��ũ �����͸� ã�� ���� Ŭ�����͸� ����
            if( kGetClusterLinkData( pstFileHandle->dwCurrentClusterIndex, 
                                     &dwNextClusterIndex ) == FALSE )
            {
                break;
            }
            
            // Ŭ������ ������ ����
            pstFileHandle->dwPreviousClusterIndex = 
                pstFileHandle->dwCurrentClusterIndex;
            pstFileHandle->dwCurrentClusterIndex = dwNextClusterIndex;
        }
    }
    
    // ����ȭ
	kLeaveCriticalSection();	
    // ���� ���ڵ� ���� ��ȯ
    return dwReadCount;
}

/**
 *  ��Ʈ ���͸����� ���͸� ��Ʈ�� ���� ����
 */
static bool kUpdateDirectoryEntry( FILEHANDLE* pstFileHandle )
{
    DIRECTORYENTRY stEntry;
    
    // ���͸� ��Ʈ�� �˻�
    if( ( pstFileHandle == NULL ) ||
        ( kGetDirectoryEntryData( pstFileHandle->iDirectoryEntryOffset, &stEntry)
            == FALSE ) )
    {
        return FALSE;
    }
    
    // ���� ũ��� ���� Ŭ�����͸� ����
    stEntry.dwFileSize = pstFileHandle->dwFileSize;
    stEntry.dwStartClusterIndex = pstFileHandle->dwStartClusterIndex;

    // ����� ���͸� ��Ʈ���� ����
    if( kSetDirectoryEntryData( pstFileHandle->iDirectoryEntryOffset, &stEntry )
            == FALSE )
    {
        return FALSE;
    }
    return TRUE;
}

/**
 *  ������ �����͸� ���Ͽ� ��
 */
DWORD kWriteFile( const void* pvBuffer, DWORD dwSize, DWORD dwCount, MFILE* pstFile )
{
    DWORD dwWriteCount;
    DWORD dwTotalCount;
    DWORD dwOffsetInCluster;
    DWORD dwCopySize;
    DWORD dwAllocatedClusterIndex;
    DWORD dwNextClusterIndex;
    FILEHANDLE* pstFileHandle;

    // �ڵ��� ���� Ÿ���� �ƴϸ� ����
    if( ( pstFile == NULL ) ||
        ( pstFile->bType != FILESYSTEM_TYPE_FILE ) )
    {
        return 0;
    }
    pstFileHandle = &( pstFile->stFileHandle );

    // �� ����Ʈ ��
    dwTotalCount = dwSize * dwCount;

    // ����ȭ
	kEnterCriticalSection();

    // �� �� ������ �ݺ�
    dwWriteCount = 0;
    while( dwWriteCount != dwTotalCount )
    {
        //======================================================================
        // ���� Ŭ�����Ͱ� ������ ���̸� Ŭ�����͸� �Ҵ��Ͽ� ����
        //======================================================================
        if( pstFileHandle->dwCurrentClusterIndex == FILESYSTEM_LASTCLUSTER )
        {					
            // �� Ŭ������ �˻�
            dwAllocatedClusterIndex = kFindFreeCluster();
            if( dwAllocatedClusterIndex == FILESYSTEM_LASTCLUSTER )
            {
                break;
            }
            
            // �˻��� Ŭ�����͸� ������ Ŭ�����ͷ� ����
            if( kSetClusterLinkData( dwAllocatedClusterIndex, FILESYSTEM_LASTCLUSTER )
                    == FALSE )
            {
                break;
            }

            
            // ������ ������ Ŭ�����Ϳ� �Ҵ�� Ŭ�����͸� ����
            if( kSetClusterLinkData( pstFileHandle->dwPreviousClusterIndex, 
                                     dwAllocatedClusterIndex ) == FALSE )
            {
                // ������ ��� �Ҵ�� Ŭ�����͸� ����
                kSetClusterLinkData( dwAllocatedClusterIndex, FILESYSTEM_FREECLUSTER );
                break;
            }
            
            // ���� Ŭ�����͸� �Ҵ�� ������ ����
            pstFileHandle->dwCurrentClusterIndex = dwAllocatedClusterIndex;
            
            // ���� �Ҵ�޾����� �ӽ� Ŭ������ ���۸� 0���� ä��
			memset( gs_vbTempBuffer, 0, 4096 );

        }        
        //======================================================================
        // �� Ŭ�����͸� ä���� ���ϸ� Ŭ�����͸� �о �ӽ� Ŭ������ ���۷� ����
        //======================================================================
        else if( ( ( pstFileHandle->dwCurrentOffset % FILESYSTEM_CLUSTERSIZE ) != 0 ) ||
                 ( ( dwTotalCount - dwWriteCount ) < FILESYSTEM_CLUSTERSIZE ) )
        {					
            // ��ü Ŭ�����͸� ����� ��찡 �ƴϸ� �κи� ������ �ϹǷ� 
            // ���� Ŭ�����͸� ����
            if( kReadCluster( pstFileHandle->dwCurrentClusterIndex, 
                              gs_vbTempBuffer ) == FALSE )
            {
                break;
            }
        }		

        // Ŭ������ ������ ���� �����Ͱ� �����ϴ� �������� ���
        dwOffsetInCluster = pstFileHandle->dwCurrentOffset % FILESYSTEM_CLUSTERSIZE;
        
        // ���� Ŭ�����Ϳ� ������ �ִٸ� ���� Ŭ�����Ϳ��� ���� ��ŭ ���� ���� 
        // Ŭ�����ͷ� �̵�
        dwCopySize = MIN( FILESYSTEM_CLUSTERSIZE - dwOffsetInCluster, 
                          dwTotalCount - dwWriteCount );
        memcpy( gs_vbTempBuffer + dwOffsetInCluster, ( char* ) pvBuffer + 
                 dwWriteCount, dwCopySize );
        
        // �ӽ� ���ۿ� ���Ե� ���� ��ũ�� ��
        if( kWriteCluster( pstFileHandle->dwCurrentClusterIndex, gs_vbTempBuffer ) 
                == FALSE )
        {
            break;
        }
        
        // �� ����Ʈ ���� ���� �������� ��ġ�� ����
        dwWriteCount += dwCopySize;
        pstFileHandle->dwCurrentOffset += dwCopySize;

        //======================================================================
        // ���� Ŭ�������� ������ �� ������ ���� Ŭ�����ͷ� �̵�
        //======================================================================
        if( ( pstFileHandle->dwCurrentOffset % FILESYSTEM_CLUSTERSIZE ) == 0 )
        {
            // ���� Ŭ�������� ��ũ �����ͷ� ���� Ŭ�����͸� ����
            if( kGetClusterLinkData( pstFileHandle->dwCurrentClusterIndex, 
                                     &dwNextClusterIndex ) == FALSE )
            {
                break;
            }
            
            // Ŭ������ ������ ����
            pstFileHandle->dwPreviousClusterIndex = 
                pstFileHandle->dwCurrentClusterIndex;
            pstFileHandle->dwCurrentClusterIndex = dwNextClusterIndex;
        }			
    }

    //==========================================================================
    // ���� ũ�Ⱑ ���ߴٸ� ��Ʈ ���͸��� �ִ� ���͸� ��Ʈ�� ������ ����
    //==========================================================================
    if( pstFileHandle->dwFileSize < pstFileHandle->dwCurrentOffset )
    {
        pstFileHandle->dwFileSize = pstFileHandle->dwCurrentOffset;
        kUpdateDirectoryEntry( pstFileHandle );
    }
    
    // ����ȭ
	kLeaveCriticalSection();
    
    // �� ���ڵ� ���� ��ȯ
    return ( dwWriteCount / dwSize );
}

/**
 *  ������ Count��ŭ 0���� ä��
 */
bool kWriteZero(MFILE* pstFile, DWORD dwCount )
{
    BYTE* pbBuffer;
    DWORD dwRemainCount;
    DWORD dwWriteCount;
    
    // �ڵ��� NULL�̸� ����
    if( pstFile == NULL )
    {
        return FALSE;
    }
    
    // �ӵ� ����� ���� �޸𸮸� �Ҵ� �޾� Ŭ������ ������ ���� ����
    // �޸𸮸� �Ҵ�
    pbBuffer = ( BYTE* ) new BYTE[FILESYSTEM_CLUSTERSIZE];
    if( pbBuffer == NULL )
    {
        return FALSE;
    }
    
    // 0���� ä��
	memset( pbBuffer, 0, FILESYSTEM_CLUSTERSIZE );
    dwRemainCount = dwCount;
    
    // Ŭ������ ������ �ݺ��ؼ� ���� ����
    while( dwRemainCount != 0 )
    {
        dwWriteCount = MIN( dwRemainCount , FILESYSTEM_CLUSTERSIZE );
        if( kWriteFile( pbBuffer, 1, dwWriteCount, pstFile ) != dwWriteCount )
        {
			delete pbBuffer;
            return FALSE;
        }
        dwRemainCount -= dwWriteCount;
    }
    delete pbBuffer;
    return TRUE;
}

/**
 *  ���� �������� ��ġ�� �̵�
 */
int kSeekFile(MFILE* pstFile, int iOffset, int iOrigin )
{
    DWORD dwRealOffset = 0;
    DWORD dwClusterOffsetToMove = 0;
    DWORD dwCurrentClusterOffset = 0;
    DWORD dwLastClusterOffset = 0;
    DWORD dwMoveCount = 0;
    DWORD i = 0;
    DWORD dwStartClusterIndex = 0;
    DWORD dwPreviousClusterIndex = 0;
    DWORD dwCurrentClusterIndex = 0;
    FILEHANDLE* pstFileHandle = 0;

    // �ڵ��� ���� Ÿ���� �ƴϸ� ����
    if( ( pstFile == NULL ) ||
        ( pstFile->bType != FILESYSTEM_TYPE_FILE ) )
    {
        return 0;
    }
    pstFileHandle = &( pstFile->stFileHandle );
    
    //==========================================================================
    // Origin�� Offset�� �����Ͽ� ���� ������ �������� ���� �����͸� �Űܾ� �� ��ġ�� 
    // ���
    //==========================================================================
    // �ɼǿ� ���� ���� ��ġ�� ���
    // �����̸� ������ ���� �������� �̵��ϸ� ����̸� ������ �� �������� �̵�
    switch( iOrigin )
    {
    // ������ ������ �������� �̵�
    case FILESYSTEM_SEEK_SET:
        // ������ ó���̹Ƿ� �̵��� �������� �����̸� 0���� ����
        if( iOffset <= 0 )
        {
            dwRealOffset = 0;
        }
        else
        {
            dwRealOffset = iOffset;
        }
        break;

    // ���� ��ġ�� �������� �̵�
    case FILESYSTEM_SEEK_CUR:
        // �̵��� �������� �����̰� ���� ���� �������� ������ ũ�ٸ�
        // �� �̻� �� �� �����Ƿ� ������ ó������ �̵�
        if( ( iOffset < 0 ) && 
            ( pstFileHandle->dwCurrentOffset <= ( DWORD ) -iOffset ) )
        {
            dwRealOffset = 0;
        }
        else
        {
            dwRealOffset = pstFileHandle->dwCurrentOffset + iOffset;
        }
        break;

    // ������ ���κ��� �������� �̵�
    case FILESYSTEM_SEEK_END:
        // �̵��� �������� �����̰� ���� ���� �������� ������ ũ�ٸ� 
        // �� �̻� �� �� �����Ƿ� ������ ó������ �̵�
        if( ( iOffset < 0 ) && 
            ( pstFileHandle->dwFileSize <= ( DWORD ) -iOffset ) )
        {
            dwRealOffset = 0;
        }
        else
        {
            dwRealOffset = pstFileHandle->dwFileSize + iOffset;
        }
        break;
    }

    //==========================================================================
    // ������ �����ϴ� Ŭ�������� ������ ���� ���� �������� ��ġ�� ����Ͽ�
    // �Ű��� ���� �����Ͱ� ��ġ�ϴ� Ŭ�����ͱ��� Ŭ������ ��ũ�� Ž��
    //==========================================================================
    // ������ ������ Ŭ�������� ������
    dwLastClusterOffset = pstFileHandle->dwFileSize / FILESYSTEM_CLUSTERSIZE;
    // ���� �����Ͱ� �Ű��� ��ġ�� Ŭ������ ������
    dwClusterOffsetToMove = dwRealOffset / FILESYSTEM_CLUSTERSIZE;
    // ���� ���� �����Ͱ� �ִ� Ŭ�������� ������
    dwCurrentClusterOffset = pstFileHandle->dwCurrentOffset / FILESYSTEM_CLUSTERSIZE;

    // �̵��ϴ� Ŭ�������� ��ġ�� ������ ������ Ŭ�������� �������� �Ѿ��
    // ���� Ŭ�����Ϳ��� ���������� �̵��� ��, Write �Լ��� �̿��ؼ� �������� ��������
    // ä��
    if( dwLastClusterOffset < dwClusterOffsetToMove )
    {
        dwMoveCount = dwLastClusterOffset - dwCurrentClusterOffset;
        dwStartClusterIndex = pstFileHandle->dwCurrentClusterIndex;
    }
    // �̵��ϴ� Ŭ�������� ��ġ�� ���� Ŭ�����Ϳ� ���ų� ������ ��ġ��
    // �ִٸ� ���� Ŭ�����͸� �������� ���̸�ŭ�� �̵��ϸ� �ȴ�.
    else if( dwCurrentClusterOffset <= dwClusterOffsetToMove )
    {
        dwMoveCount = dwClusterOffsetToMove - dwCurrentClusterOffset;
        dwStartClusterIndex = pstFileHandle->dwCurrentClusterIndex;
    }
    // �̵��ϴ� Ŭ�������� ��ġ�� ���� Ŭ������ ������ �ִٸ�, ù ��° Ŭ�����ͺ���
    // �̵��ϸ鼭 �˻�
    else
    {
        dwMoveCount = dwClusterOffsetToMove;
        dwStartClusterIndex = pstFileHandle->dwStartClusterIndex;
    }

    // ����ȭ
	kEnterCriticalSection();

    // Ŭ�����͸� �̵�
    dwCurrentClusterIndex = dwStartClusterIndex;
    for( i = 0 ; i < dwMoveCount ; i++ )
    {
        // ���� ����
        dwPreviousClusterIndex = dwCurrentClusterIndex;
        
        // ���� Ŭ�������� �ε����� ����
        if( kGetClusterLinkData( dwPreviousClusterIndex, &dwCurrentClusterIndex ) ==
            FALSE )
        {
            // ����ȭ
			kLeaveCriticalSection();
            return -1;
        }
    }

    // Ŭ�����͸� �̵������� Ŭ������ ������ ����
    if( dwMoveCount > 0 )
    {
        pstFileHandle->dwPreviousClusterIndex = dwPreviousClusterIndex;
        pstFileHandle->dwCurrentClusterIndex = dwCurrentClusterIndex;
    }
    // ù ��° Ŭ�����ͷ� �̵��ϴ� ���� �ڵ��� Ŭ������ ���� ���� Ŭ�����ͷ� ����
    else if( dwStartClusterIndex == pstFileHandle->dwStartClusterIndex )
    {
        pstFileHandle->dwPreviousClusterIndex = pstFileHandle->dwStartClusterIndex;
        pstFileHandle->dwCurrentClusterIndex = pstFileHandle->dwStartClusterIndex;
    }
    
    //==========================================================================
    // ���� �����͸� �����ϰ� ���� �������� ������ ũ�⸦ �Ѿ��ٸ� ������ �κ���
    // 0���� ä���� ������ ũ�⸦ �ø�
    //==========================================================================
    // ���� ������ ũ�⸦ �Ѿ ���� �����Ͱ� �̵��ߴٸ�, ���� ���������� 
    // ���� ũ�⸸ŭ 0���� ä����
    if( dwLastClusterOffset < dwClusterOffsetToMove )
    {
        pstFileHandle->dwCurrentOffset = pstFileHandle->dwFileSize;
        // ����ȭ
		kLeaveCriticalSection();

        // ���� ũ�⸸ŭ 0���� ä��
        if( kWriteZero( pstFile, dwRealOffset - pstFileHandle->dwFileSize )
                == FALSE )
        {
            return 0;
        }
    }

    pstFileHandle->dwCurrentOffset = dwRealOffset;

    // ����ȭ
	kLeaveCriticalSection();

    return 0;    
}

/**
 *  ������ ����
 */
int kCloseFile(MFILE* pstFile )
{
    // �ڵ� Ÿ���� ������ �ƴϸ� ����
    if( ( pstFile == NULL ) ||
        ( pstFile->bType != FILESYSTEM_TYPE_FILE ) )
    {
        return -1;
    }
    
    // �ڵ��� ��ȯ
    kFreeFileDirectoryHandle( pstFile );
    return 0;
}

/**
 *  �ڵ� Ǯ�� �˻��Ͽ� ������ �����ִ����� Ȯ��
 */
bool kIsFileOpened( const DIRECTORYENTRY* pstEntry )
{
    int i;
	MFILE* pstFile;
    
    // �ڵ� Ǯ�� ���� ��巹������ ������ ���� ���ϸ� �˻�
    pstFile = gs_stFileSystemManager.pstHandlePool;
    for( i = 0 ; i < FILESYSTEM_HANDLE_MAXCOUNT ; i++ )
    {
        // ���� Ÿ�� �߿��� ���� Ŭ�����Ͱ� ��ġ�ϸ� ��ȯ
        if( ( pstFile[ i ].bType == FILESYSTEM_TYPE_FILE ) &&
            ( pstFile[ i ].stFileHandle.dwStartClusterIndex == 
              pstEntry->dwStartClusterIndex ) )
        {
            return TRUE;
        }
    }
    return FALSE;
}

/**
 *  ������ ����
 */
int kRemoveFile( const char* pcFileName )
{
    DIRECTORYENTRY stEntry;
    int iDirectoryEntryOffset;
    int iFileNameLength;

    // ���� �̸� �˻�
    iFileNameLength = strlen( pcFileName );
    if( ( iFileNameLength > ( sizeof( stEntry.vcFileName ) - 1 ) ) || 
        ( iFileNameLength == 0 ) )
    {
        return NULL;
    }
    
    // ����ȭ
	kEnterCriticalSection();
    
    // ������ �����ϴ°� Ȯ��
    iDirectoryEntryOffset = kFindDirectoryEntry( pcFileName, &stEntry );
    if( iDirectoryEntryOffset == -1 ) 
    { 
        // ����ȭ
		kLeaveCriticalSection();
        return -1;
    }
    
    // �ٸ� �½�ũ���� �ش� ������ ���� �ִ��� �ڵ� Ǯ�� �˻��Ͽ� Ȯ��
    // ������ ���� ������ ������ �� ����
    if( kIsFileOpened( &stEntry ) == TRUE )
    {
        // ����ȭ
		kLeaveCriticalSection();
        return -1;
    }
    
    // ������ �����ϴ� Ŭ�����͸� ��� ����
    if( kFreeClusterUntilEnd( stEntry.dwStartClusterIndex ) == FALSE )
    { 
        // ����ȭ
		kLeaveCriticalSection();
        return -1;
    }

    // ���͸� ��Ʈ���� �� ������ ����
    memset( &stEntry, 0, sizeof( stEntry ) );
    if( kSetDirectoryEntryData( iDirectoryEntryOffset, &stEntry ) == FALSE )
    {
        // ����ȭ
		kLeaveCriticalSection();
        return -1;
    }
    
    // ����ȭ
	kLeaveCriticalSection();
    return 0;
}

/**
 *  ���͸��� ��
 */
DIR* kOpenDirectory( const char* pcDirectoryName )
{
    DIR* pstDirectory;
    DIRECTORYENTRY* pstDirectoryBuffer;
    
    // ����ȭ
	kEnterCriticalSection();
    
    // ��Ʈ ���͸� �ۿ� �����Ƿ� ���͸� �̸��� �����ϰ� �ڵ鸸 �Ҵ�޾Ƽ� ��ȯ
    pstDirectory = (DIR*)kAllocateFileDirectoryHandle();
    if( pstDirectory == NULL )
    {
        // ����ȭ
		kLeaveCriticalSection();
        return NULL;
    }
    
    // ��Ʈ ���͸��� ������ ���۸� �Ҵ�
    pstDirectoryBuffer = ( DIRECTORYENTRY* ) new char[FILESYSTEM_CLUSTERSIZE];
    if( pstDirectory == NULL )
    {
        // �����ϸ� �ڵ��� ��ȯ�ؾ� ��
        kFreeFileDirectoryHandle( pstDirectory );
        // ����ȭ
		kLeaveCriticalSection();
        return NULL;
    }
    
    // ��Ʈ ���͸��� ����
    if( kReadCluster( 0, ( BYTE* ) pstDirectoryBuffer ) == FALSE )
    {
        // �����ϸ� �ڵ�� �޸𸮸� ��� ��ȯ�ؾ� ��
        kFreeFileDirectoryHandle( pstDirectory );
		delete pstDirectoryBuffer;
        
        // ����ȭ
		kLeaveCriticalSection();
        return NULL;
        
    }
    
    // ���͸� Ÿ������ �����ϰ� ���� ���͸� ��Ʈ���� �������� �ʱ�ȭ
    pstDirectory->bType = FILESYSTEM_TYPE_DIRECTORY;
    pstDirectory->stDirectoryHandle.iCurrentOffset = 0;
    pstDirectory->stDirectoryHandle.pstDirectoryBuffer = pstDirectoryBuffer;

    // ����ȭ
	kLeaveCriticalSection();
    return pstDirectory;
}

/**
 *  ���͸� ��Ʈ���� ��ȯ�ϰ� �������� �̵�
 */
struct kDirectoryEntryStruct* kReadDirectory( DIR* pstDirectory )
{
    DIRECTORYHANDLE* pstDirectoryHandle;
    DIRECTORYENTRY* pstEntry;
    
    // �ڵ� Ÿ���� ���͸��� �ƴϸ� ����
    if( ( pstDirectory == NULL ) ||
        ( pstDirectory->bType != FILESYSTEM_TYPE_DIRECTORY ) )
    {
        return NULL;
    }
    pstDirectoryHandle = &( pstDirectory->stDirectoryHandle );
    
    // �������� ������ Ŭ�����Ϳ� �����ϴ� �ִ��� �Ѿ�� ����
    if( ( pstDirectoryHandle->iCurrentOffset < 0 ) ||
        ( pstDirectoryHandle->iCurrentOffset >= FILESYSTEM_MAXDIRECTORYENTRYCOUNT ) )
    {
        return NULL;
    }

    // ����ȭ
	kEnterCriticalSection();
    
    // ��Ʈ ���͸��� �ִ� �ִ� ���͸� ��Ʈ���� ������ŭ �˻�
    pstEntry = pstDirectoryHandle->pstDirectoryBuffer;
    while( pstDirectoryHandle->iCurrentOffset < FILESYSTEM_MAXDIRECTORYENTRYCOUNT )
    {
        // ������ �����ϸ� �ش� ���͸� ��Ʈ���� ��ȯ
        if( pstEntry[ pstDirectoryHandle->iCurrentOffset ].dwStartClusterIndex
                != 0 )
        {
            // ����ȭ
			kLeaveCriticalSection();
            return &( pstEntry[ pstDirectoryHandle->iCurrentOffset++ ] );
        }
        
        pstDirectoryHandle->iCurrentOffset++;
    }

    // ����ȭ
	kLeaveCriticalSection();
    return NULL;
}

/**
 *  ���͸� �����͸� ���͸��� ó������ �̵�
 */
void kRewindDirectory( DIR* pstDirectory )
{
    DIRECTORYHANDLE* pstDirectoryHandle;
    
    // �ڵ� Ÿ���� ���͸��� �ƴϸ� ����
    if( ( pstDirectory == NULL ) ||
        ( pstDirectory->bType != FILESYSTEM_TYPE_DIRECTORY ) )
    {
        return ;
    }
    pstDirectoryHandle = &( pstDirectory->stDirectoryHandle );
    
    // ����ȭ
	kEnterCriticalSection();
    
    // ���͸� ��Ʈ���� �����͸� 0���� �ٲ���
    pstDirectoryHandle->iCurrentOffset = 0;
    
    // ����ȭ
	kLeaveCriticalSection();
}


/**
 *  ���� ���͸��� ����
 */
int kCloseDirectory( DIR* pstDirectory )
{
    DIRECTORYHANDLE* pstDirectoryHandle;
    
    // �ڵ� Ÿ���� ���͸��� �ƴϸ� ����
    if( ( pstDirectory == NULL ) ||
        ( pstDirectory->bType != FILESYSTEM_TYPE_DIRECTORY ) )
    {
        return -1;
    }
    pstDirectoryHandle = &( pstDirectory->stDirectoryHandle );

    // ����ȭ
	kEnterCriticalSection();
    
    // ��Ʈ ���͸��� ���۸� �����ϰ� �ڵ��� ��ȯ
    delete pstDirectoryHandle->pstDirectoryBuffer;
    kFreeFileDirectoryHandle( pstDirectory );    
    
    // ����ȭ
	kLeaveCriticalSection();

    return 0;
}