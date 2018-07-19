#pragma once
#include "windef.h"

// MINT ���� �ý��� �ñ׳�ó(Signature)
#define FILESYSTEM_SIGNATURE                0x7E38CF10
// Ŭ�������� ũ��(���� ��), 4Kbyte
#define FILESYSTEM_SECTORSPERCLUSTER        8
// ���� Ŭ�������� ������ ǥ��
#define FILESYSTEM_LASTCLUSTER              0xFFFFFFFF
// �� Ŭ������ ǥ��
#define FILESYSTEM_FREECLUSTER              0x00
// ��Ʈ ���͸��� �ִ� �ִ� ���͸� ��Ʈ���� ��
#define FILESYSTEM_MAXDIRECTORYENTRYCOUNT   ( ( FILESYSTEM_SECTORSPERCLUSTER * 512 ) / \
        sizeof( DIRECTORYENTRY ) )
// Ŭ�������� ũ��(����Ʈ ��)
#define FILESYSTEM_CLUSTERSIZE              ( FILESYSTEM_SECTORSPERCLUSTER * 512 )

// �ڵ��� �ִ� ����, �ִ� �½�ũ ���� 3��� ����
#define FILESYSTEM_HANDLE_MAXCOUNT          ( 10 * 3 )

// ���� �̸��� �ִ� ����
#define FILESYSTEM_MAXFILENAMELENGTH        24

// �ڵ��� Ÿ���� ����
#define FILESYSTEM_TYPE_FREE                0
#define FILESYSTEM_TYPE_FILE                1
#define FILESYSTEM_TYPE_DIRECTORY           2

// SEEK �ɼ� ����
#define FILESYSTEM_SEEK_SET                 0
#define FILESYSTEM_SEEK_CUR                 1
#define FILESYSTEM_SEEK_END                 2



#pragma pack( push, 1 )

// ��Ƽ�� �ڷᱸ��
typedef struct kPartitionStruct
{
	// ���� ���� �÷���. 0x80�̸� ���� ������ ��Ÿ���� 0x00�� ���� �Ұ�
	BYTE bBootableFlag;
	// ��Ƽ���� ���� ��巹��. ����� ���� ������� ������ �Ʒ��� LBA ��巹���� ��� ���
	BYTE vbStartingCHSAddress[3];
	// ��Ƽ�� Ÿ��
	BYTE bPartitionType;
	// ��Ƽ���� ������ ��巹��. ����� ���� ��� �� ��
	BYTE vbEndingCHSAddress[3];
	// ��Ƽ���� ���� ��巹��. LBA ��巹���� ��Ÿ�� ��
	DWORD dwStartingLBAAddress;
	// ��Ƽ�ǿ� ���Ե� ���� ��
	DWORD dwSizeInSector;
} _PARTITION;


// MBR �ڷᱸ��
typedef struct kMBRStruct
{
	// ��Ʈ �δ� �ڵ尡 ��ġ�ϴ� ����
	BYTE vbBootCode[430];

	// ���� �ý��� �ñ׳�ó, 0x7E38CF10
	DWORD dwSignature;
	// ����� ������ ���� ��
	DWORD dwReservedSectorCount;
	// Ŭ������ ��ũ ���̺� ������ ���� ��
	DWORD dwClusterLinkSectorCount;
	// Ŭ�������� ��ü ����
	DWORD dwTotalClusterCount;

	// ��Ƽ�� ���̺�
	_PARTITION vstPartition[4];

	// ��Ʈ �δ� �ñ׳�ó, 0x55, 0xAA
	BYTE vbBootLoaderSignature[2];
} MBR;

// HDD�� ���� ������ ��Ÿ���� ����ü
typedef struct kHDDInformationStruct
{
	// ������
	WORD wConfiguation;

	// �Ǹ��� ��
	WORD wNumberOfCylinder;
	WORD wReserved1;

	// ��� ��
	WORD wNumberOfHead;
	WORD wUnformattedBytesPerTrack;
	WORD wUnformattedBytesPerSector;

	// �Ǹ����� ���� ��
	WORD wNumberOfSectorPerCylinder;
	WORD wInterSectorGap;
	WORD wBytesInPhaseLock;
	WORD wNumberOfVendorUniqueStatusWord;

	// �ϵ� ��ũ�� �ø��� �ѹ�
	WORD vwSerialNumber[10];
	WORD wControllerType;
	WORD wBufferSize;
	WORD wNumberOfECCBytes;
	WORD vwFirmwareRevision[4];

	// �ϵ� ��ũ�� �� ��ȣ
	WORD vwModelNumber[20];
	WORD vwReserved2[13];

	// ��ũ�� �� ���� ��
	DWORD dwTotalSectors;
	WORD vwReserved3[196];
} HDDINFORMATION;

#pragma pack( pop )


// �ϵ� ��ũ ��� ���õ� �Լ� ������ Ÿ�� ����
typedef bool (* fReadHDDInformation ) (bool bPrimary, bool bMaster, HDDINFORMATION* pstHDDInformation );
typedef int (* fReadHDDSector ) (bool bPrimary, bool bMaster, DWORD dwLBA,
        int iSectorCount, char* pcBuffer );
typedef int (* fWriteHDDSector ) (bool bPrimary, bool bMaster, DWORD dwLBA,
        int iSectorCount, char* pcBuffer );

#pragma pack( push, 1 )
// ���͸� ��Ʈ�� �ڷᱸ��
typedef struct kDirectoryEntryStruct
{
    // ���� �̸�
    char vcFileName[ FILESYSTEM_MAXFILENAMELENGTH ];
    // ������ ���� ũ��
    DWORD dwFileSize;
    // ������ �����ϴ� Ŭ������ �ε���
    DWORD dwStartClusterIndex;
} DIRECTORYENTRY;

#pragma pack( pop )

// ������ �����ϴ� ���� �ڵ� �ڷᱸ��
typedef struct kFileHandleStruct
{
    // ������ �����ϴ� ���͸� ��Ʈ���� ������
    int iDirectoryEntryOffset;
    // ���� ũ��
    DWORD dwFileSize;
    // ������ ���� Ŭ������ �ε���
    DWORD dwStartClusterIndex;
    // ���� I/O�� �������� Ŭ�������� �ε���
    DWORD dwCurrentClusterIndex;
    // ���� Ŭ�������� �ٷ� ���� Ŭ�������� �ε���
    DWORD dwPreviousClusterIndex;
    // ���� �������� ���� ��ġ
    DWORD dwCurrentOffset;
} FILEHANDLE;

// ���͸��� �����ϴ� ���͸� �ڵ� �ڷᱸ��
typedef struct kDirectoryHandleStruct
{
    // ��Ʈ ���͸��� �����ص� ����
    DIRECTORYENTRY* pstDirectoryBuffer;
    
    // ���͸� �������� ���� ��ġ
    int iCurrentOffset;
} DIRECTORYHANDLE;

// ���ϰ� ���͸��� ���� ������ ����ִ� �ڷᱸ��
typedef struct kFileDirectoryHandleStruct
{
    // �ڷᱸ���� Ÿ�� ����. ���� �ڵ��̳� ���͸� �ڵ�, �Ǵ� �� �ڵ� Ÿ�� ���� ����
    BYTE bType;

    // bType�� ���� ���� ���� �Ǵ� ���͸��� ���
    union
    {
        // ���� �ڵ�
        FILEHANDLE stFileHandle;
        // ���͸� �ڵ�
        DIRECTORYHANDLE stDirectoryHandle;
    };    
} MFILE, DIR;

// ���� �ý����� �����ϴ� ����ü
typedef struct kFileSystemManagerStruct
{
    // ���� �ý����� ���������� �νĵǾ����� ����
    bool bMounted;
    
    // �� ������ ���� ���� ���� LBA ��巹��
    DWORD dwReservedSectorCount;
    DWORD dwClusterLinkAreaStartAddress;
    DWORD dwClusterLinkAreaSize;
    DWORD dwDataAreaStartAddress;   
    // ������ ������ Ŭ�������� �� ����
    DWORD dwTotalClusterCount;
    
    // ���������� Ŭ�����͸� �Ҵ��� Ŭ������ ��ũ ���̺��� ���� �������� ����
    DWORD dwLastAllocatedClusterLinkSectorOffset;
        
    // �ڵ� Ǯ(Handle Pool)�� ��巹��
	MFILE* pstHandlePool;
    
    // ĳ�ø� ����ϴ��� ����
    bool bCacheEnable;
} FILESYSTEMMANAGER;


////////////////////////////////////////////////////////////////////////////////
//
// �Լ�
//
////////////////////////////////////////////////////////////////////////////////
bool kInitializeRDDFileSystem( void );
bool kFormat( void );
bool kMount( void );
bool kGetHDDInformation( HDDINFORMATION* pstInformation);

//  ������ �Լ�(Low Level Function)
static bool kReadClusterLinkTable( DWORD dwOffset, BYTE* pbBuffer );
static bool kWriteClusterLinkTable( DWORD dwOffset, BYTE* pbBuffer );
static bool kReadCluster( DWORD dwOffset, BYTE* pbBuffer );
static bool kWriteCluster( DWORD dwOffset, BYTE* pbBuffer );
static DWORD kFindFreeCluster( void );
static bool kSetClusterLinkData( DWORD dwClusterIndex, DWORD dwData );
static bool kGetClusterLinkData( DWORD dwClusterIndex, DWORD* pdwData );
static int kFindFreeDirectoryEntry( void );
static bool kSetDirectoryEntryData( int iIndex, DIRECTORYENTRY* pstEntry );
static bool kGetDirectoryEntryData( int iIndex, DIRECTORYENTRY* pstEntry );
static int kFindDirectoryEntry( const char* pcFileName, DIRECTORYENTRY* pstEntry );
void kGetFileSystemInformation( FILESYSTEMMANAGER* pstManager );

static bool kInternalReadClusterLinkTableWithoutCache( DWORD dwOffset,
        BYTE* pbBuffer );
static bool kInternalWriteClusterLinkTableWithoutCache( DWORD dwOffset,
        BYTE* pbBuffer );
static bool kInternalWriteClusterWithoutCache( DWORD dwOffset, BYTE* pbBuffer );


//  ����� �Լ�(High Level Function)
MFILE* kOpenFile( const char* pcFileName, const char* pcMode );
DWORD kReadFile( void* pvBuffer, DWORD dwSize, DWORD dwCount, MFILE* pstFile );
DWORD kWriteFile( const void* pvBuffer, DWORD dwSize, DWORD dwCount, MFILE* pstFile );
int kSeekFile(MFILE* pstFile, int iOffset, int iOrigin );
int kCloseFile(MFILE* pstFile );
int kRemoveFile( const char* pcFileName );
DIR* kOpenDirectory( const char* pcDirectoryName );
struct kDirectoryEntryStruct* kReadDirectory( DIR* pstDirectory );
void kRewindDirectory( DIR* pstDirectory );
int kCloseDirectory( DIR* pstDirectory );
bool kWriteZero(MFILE* pstFile, DWORD dwCount );
bool kIsFileOpened( const DIRECTORYENTRY* pstEntry );

static void* kAllocateFileDirectoryHandle( void );
static void kFreeFileDirectoryHandle(MFILE* pstFile );
static bool kCreateFile( const char* pcFileName, DIRECTORYENTRY* pstEntry,
        int* piDirectoryEntryIndex );
static bool kFreeClusterUntilEnd( DWORD dwClusterIndex );
static bool kUpdateDirectoryEntry( FILEHANDLE* pstFileHandle );
