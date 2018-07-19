#pragma once
#include "FileSysAdaptor.h"
#include "MintFileSystem.h"

#pragma pack( push, 1 )

// ��Ű���� �ñ׳�ó
#define PACKAGESIGNATURE    "SKYOS32PACKAGE_  "

// ���� �̸��� �ִ� ����
#define MAXFILENAMELENGTH   24

// ��Ű�� ���ο� ���� ������ �����ϱ� ���� ����ü
// ���� �̸�
// ������ ũ��
typedef struct tag_PACKAGEITEM
{
	char vcFileName[MAXFILENAMELENGTH];
	DWORD dwFileLength;
} PACKAGEITEM;

// ��Ű�� ��� �ڷᱸ��
// �ñ׳���
// ���ũ��
typedef struct tag_PACKAGEHEADER
{
	char vcSignature[16];
	DWORD dwHeaderSize;
	
} PACKAGEHEADER;

#pragma pack( pop )

class RamDiskAdaptor : public FileSysAdaptor
{
public:
	RamDiskAdaptor(char* deviceName, DWORD deviceID);
	~RamDiskAdaptor();
	
	virtual bool Initialize() override;
	virtual int GetCount() override;
	virtual int Read(PFILE file, unsigned char* buffer, unsigned int size, int count) override;
	virtual bool Close(PFILE file)  override;
	virtual PFILE Open(const char* FileName, const char *mode)  override;
	virtual size_t Write(PFILE file, unsigned char* buffer, unsigned int size, int count) override;

	bool InstallPackage();

private:
	void PrintRamDiskInfo();	
	PACKAGEHEADER* FindPackageSignature(UINT32 startAddress, UINT32 endAddress);

private:
	HDDINFORMATION* m_pInformation;
};

