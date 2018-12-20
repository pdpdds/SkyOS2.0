#include "SkyRamDiskAdaptor.h"
#include "SkyConsole.h"
#include "SkyStruct.h"
#include "VirtualMemoryManager.h"
#include "SkyModuleManager.h"
#include "SkyRamDisk.h"
#include "Hal.h"

SkyRamDiskAdaptor::SkyRamDiskAdaptor(char* deviceName, DWORD deviceID)
	: FileSysAdaptor(deviceName, deviceID)
{
	m_pRamDisk = nullptr;
}

SkyRamDiskAdaptor::~SkyRamDiskAdaptor()
{
}

bool SkyRamDiskAdaptor::Initialize()
{
	BootModule* rd_module = SkyModuleManager::GetInstance()->FindModule("skyramdisk.pak");	
	if (rd_module == 0) {
		//G_PRETTY_BOOT_FAIL("Ramdisk not found (did you supply enough memory?");
		SkyConsole::Print("%s ramdisk not found (did you supply enough memory?)", "kern");
		return false;
	}
	
	SkyConsole::Print("Loading Ramdisk\n");
	
#ifdef SKY_EMULATOR
	LoadRamDisk(rd_module, rd_module->ModuleStart);
#else
	//20181201
	//LoadRamDisk(rd_module, KERNEL_VIRTUAL_RAMDISK_ADDRESS);
#endif

	return true;
}

void SkyRamDiskAdaptor::LoadRamDisk(BootModule* ramdiskModule, DWORD targetAddress)
{
/*#ifndef  SKY_EMULATOR
	int ramdiskPages = PAGE_ALIGN_UP(ramdiskModule->ModuleEnd - ramdiskModule->ModuleStart) / PAGE_SIZE;	
	VirtualMemoryManager::MapAddress(targetAddress, ramdiskPages);
	memcpy((void*)targetAddress, (void*)ramdiskModule->ModuleStart, ramdiskModule->ModuleEnd - ramdiskModule->ModuleStart);

	ramdiskModule->ModuleEnd = targetAddress + (ramdiskModule->ModuleEnd - ramdiskModule->ModuleStart);
	ramdiskModule->ModuleStart = targetAddress;
#endif*/

	m_pRamDisk = new SkyRamDisk();
	m_pRamDisk->Load(ramdiskModule);
	SkyConsole::Print("%s ramdisk loaded\n", "kern");	
}

int SkyRamDiskAdaptor::GetCount()
{
	return 1;
}

int SkyRamDiskAdaptor::Read(PFILE file, unsigned char* buffer, unsigned int size, int count)
{
	if (file == nullptr)
		return false;

	RamDiskEntry* pEntry = (RamDiskEntry*)file->_id;

	if (pEntry == nullptr)
		return false;

	int remain = file->_fileLength - file->_position;

	if (remain == 0)
	{
		file->_eof = 1;
		return 0;
	}

	int readCount = size * count;

	if (readCount > remain)
	{
		readCount = remain;
		file->_eof = 1;
	}

	memcpy(buffer, ((char*)pEntry->data) + file->_position, readCount);

	//SkyConsole::Print("%c", buffer[0]);

	file->_position += readCount;

	return readCount;
}

bool SkyRamDiskAdaptor::Close(PFILE file)
{
	if (file == nullptr)
		return false;

	delete file;
	return true;
}

PFILE SkyRamDiskAdaptor::Open(const char* fileName, const char *mode)
{
	if (m_pRamDisk == nullptr)
		nullptr;

	RamDiskEntry* pEntry = m_pRamDisk->FindAbsolute(fileName);

	if (pEntry == nullptr)
		nullptr;

	if (pEntry)
	{
		PFILE file = new FILE;
		file->_deviceID = 'P';
		strcpy(file->_name, fileName);
		file->_id = (DWORD)pEntry;
		file->_fileLength = pEntry->datalength;
		file->_eof = 0;
		file->_currentCluster = 0;
		file->_position = 0;
		file->_flags = FS_FILE;
		return file;
	}

	return nullptr;
}

size_t SkyRamDiskAdaptor::Write(PFILE file, unsigned char* buffer, unsigned int size, int count)
{
	return 0;
}

bool SkyRamDiskAdaptor::GetFileList()
{
	if (m_pRamDisk == nullptr)
		return false;

	RamDiskEntry* pEntry = m_pRamDisk->GetFirst();
	if (pEntry == nullptr)
		return false;

	while (pEntry)
	{

		SkyConsole::Print("%s\n", pEntry->name);
		pEntry = pEntry->next;
	}

	return true;
}

