#include "Image.h"
#include "_types.h"
#include "syscall.h"
#include "string.h"
#include "SystemAPI.h"
#include "memory.h"
#include "SystemCall.h"

Image::Image()
	: fFileHandle(-1),
	fBaseAddress(0),
	fPath(0),
	fDosHeader(0),
	fNTHeader(0)	
{
}

Image::~Image()
{
	close_handle(fFileHandle);
	delete[] fPath;
	delete fDosHeader;
	delete fNTHeader;	
}

int Image::Open(const char path[])
{
	status_t error = E_NO_ERROR;
	fFileHandle = open(path, 0);
	if (fFileHandle < 0)
		return fFileHandle;

	fPath = new char[strlen(path) + 1];
	strcpy(fPath, path);

	error = ReadHeader();
	if (error < 0)
		return error;

	return 0;
}

int Image::Load(Team &team)
{
	const char *filename = fPath;
	for (const char *c = fPath; *c; c++)
		if (*c == '/')
			filename = c + 1;

	char areaName[OS_NAME_LENGTH];
	snprintf(areaName, OS_NAME_LENGTH, "%.12s", filename);

	status_t error = CreateFileArea(areaName, fPath, fNTHeader->OptionalHeader.ImageBase
		& ~(PAGE_SIZE - 1), 0,
		((fNTHeader->OptionalHeader.SizeOfImage + PAGE_SIZE - 1) / PAGE_SIZE) * PAGE_SIZE,
		EXACT_ADDRESS | MAP_PRIVATE, USER_READ | USER_EXEC | SYSTEM_READ,

		team);
	if (error < 0) {
		printf("Failed to map image file\n");
		return error;
	}

	
	fBaseAddress = reinterpret_cast<void*>(fNTHeader->OptionalHeader.ImageBase);
	return 0;
}

unsigned int Image::GetEntryAddress() const
{
	return fNTHeader->OptionalHeader.AddressOfEntryPoint;
}

const char* Image::GetPath() const
{
	return fPath;
}

int Image::ReadHeader()
{
	unsigned char buf[512];

	fDosHeader = new IMAGE_DOS_HEADER;

	if (fDosHeader == 0)
		return E_NO_MEMORY;

	if (read_pos(fFileHandle, 0, buf, 512) != 512) {
		printf("incomplete header\n");
		return E_NOT_IMAGE;
	}
	
	//유효하지 않은 PE파일이면 파일 핸들을 닫고 종료한다.
	if (!ValidatePEImage(buf))
	{
		printf("Invalid PE Format!! %s\n", fPath);
		return -1;
	}

	
	memcpy(fDosHeader, buf, sizeof(IMAGE_DOS_HEADER));
	fNTHeader = new IMAGE_NT_HEADERS;
	memcpy(fNTHeader, (void*)(fDosHeader->e_lfanew + (uint32_t)buf), sizeof(IMAGE_NT_HEADERS));	

	return 0;
}

void Image::PrintSections() const
{
	
}

void Image::PrintSymbols() const
{
	
}
