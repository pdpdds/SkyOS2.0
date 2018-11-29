#pragma once
#include "FileSysAdaptor.h"
#include "BasicStruct.h"
#include "HDDAdaptor.h"
#include "MemoryResourceAdaptor.h"
#include "RamDiskAdaptor.h"
#include "FloppyDiskAdaptor.h"

//������ġ�� �ִ� 26��
#define STORAGE_DEVICE_MAX 26

class StorageManager
{
public:	
	~StorageManager();

	static StorageManager* GetInstance()
	{
		if (m_pStorageManager == nullptr)
			m_pStorageManager = new StorageManager();

		return m_pStorageManager;
	}

	bool Initilaize();

//�������̽�
	bool RegisterFileSystem(FileSysAdaptor* fsys, DWORD deviceID);
	bool UnregisterFileSystem(FileSysAdaptor* fsys);
	bool UnregisterFileSystemByID(DWORD deviceID);

	bool SetCurrentFileSystemByID(DWORD deviceID);
	bool SetCurrentFileSystem(FileSysAdaptor* fsys);

//���� �޼ҵ�
	PFILE OpenFile(const char* fname, const char *mode);
	int ReadFile(PFILE file, unsigned char* Buffer, unsigned int size, int count);
	int WriteFile(PFILE file, unsigned char* Buffer, unsigned int size, int count);
	bool CloseFile(PFILE file);

	bool GetFileList();
	int GetCurrentDriveId()
	{
		if (m_pCurrentFileSystem)
			return m_pCurrentFileSystem->m_deviceID;

		return 0;
	}
	bool ConstructFileSystem();
protected:
	

private:
	StorageManager();
	static StorageManager* m_pStorageManager;

	FileSysAdaptor* m_fileSystems[STORAGE_DEVICE_MAX];
	int m_stroageCount;
	FileSysAdaptor* m_pCurrentFileSystem;
};