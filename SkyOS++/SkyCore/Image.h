#pragma once
#include "windef.h"
#include "PEImage.h"

class Team;

class Image {
public:
	Image();
	virtual ~Image();
	int Open(const char path[]);
	int Load(Team&);
	unsigned int GetEntryAddress() const;
	const char* GetPath() const;

private:
	int ReadHeader();
	void PrintSections() const;
	void PrintSymbols() const;

	int fFileHandle;
	void *fBaseAddress;
	char *fPath;
	IMAGE_DOS_HEADER *fDosHeader;
	IMAGE_NT_HEADERS *fNTHeader;		
};