#include <string.h>
#include "PEImage32.h"
#include "ProcessUtil32.h"
#include "SkyConsole.h"
#include "defines.h"

//32��Ʈ PE���� �̹��� ��ȿ�� �˻�
bool ValidatePEImage(void* image)
{
	IMAGE_DOS_HEADER* dosHeader = 0;
	IMAGE_NT_HEADERS* ntHeaders = 0;

	dosHeader = (IMAGE_DOS_HEADER*)image;
	if (dosHeader->e_magic != IMAGE_DOS_SIGNATURE)
		return false;

	if (dosHeader->e_lfanew == 0)
		return false;

	//NT Header üũ
	ntHeaders = (IMAGE_NT_HEADERS*)(dosHeader->e_lfanew + (uint32_t)image);
	if (ntHeaders->Signature != IMAGE_NT_SIGNATURE)
		return false;

	/* only supporting for i386 archs */
	if (ntHeaders->FileHeader.Machine != IMAGE_FILE_MACHINE_I386)
		return false;

	/* only support 32 bit executable images */
	if (!(ntHeaders->FileHeader.Characteristics & (IMAGE_FILE_EXECUTABLE_IMAGE | IMAGE_FILE_32BIT_MACHINE)))
		return false;

	//if ((ntHeaders->OptionalHeader.ImageBase < 0x400000) || (ntHeaders->OptionalHeader.ImageBase > 0x80000000))
		//return false;

	/* only support 32 bit optional header format */
	if (ntHeaders->OptionalHeader.Magic != IMAGE_NT_OPTIONAL_HDR32_MAGIC)
		return false;

	//��ȿ�� 32��Ʈ PE �����̴�.
	return true;
}

uint32_t FindKernel32Entry(const char* szFileName, char* buf, uint32_t& imageBase)
{
	if (!ValidatePEImage(buf)) {
		SkyConsole::Print("Invalid PE Format!! %s\n", szFileName);
		return 0;
	}

	IMAGE_DOS_HEADER* dosHeader = 0;
	IMAGE_NT_HEADERS* ntHeaders = 0;

	SkyConsole::Print("Valid PE Format %s\n", szFileName);

	dosHeader = (IMAGE_DOS_HEADER*)buf;
	ntHeaders = (IMAGE_NT_HEADERS*)(dosHeader->e_lfanew + (uint32_t)buf);	

	imageBase = ntHeaders->OptionalHeader.ImageBase;
	uint32_t entryPoint = (uint32_t)ntHeaders->OptionalHeader.AddressOfEntryPoint + imageBase;
	

	SkyConsole::Print("image base : 0x%x, entry : 0x%x\n", imageBase, entryPoint);

	return 	entryPoint;
}

