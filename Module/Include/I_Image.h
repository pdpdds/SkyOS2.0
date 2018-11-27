#prgma once

#ifdef SKY_OS
typedef struct tag_FILE_STRUCT
{
	struct Elf32_Ehdr *fHeader;
	struct Elf32_Shdr *fSectionTable;
	struct Elf32_Sym *fSymbolTable;
	struct Elf32_Phdr *fProgramHeaderTable;
	
}FILE_STRUCT;
#else
typedef struct tag_FILE_STRUCT
{
	struct Elf32_Ehdr *fHeader;
	struct Elf32_Shdr *fSectionTable;
	struct Elf32_Sym *fSymbolTable;
	struct Elf32_Phdr *fProgramHeaderTable;
	
}FILE_STRUCT;
#endif

class I_Image 
{
public:
	I_Image();
	virtual ~I_Image();
	int Open(const char path[]);
	int Load();
	unsigned int GetEntryAddress() const;
	const char* GetPath() const;

protected:
	void* FindSection(const char name[]) const;
	int ReadHeader();
	void PrintSections() const;
	void PrintSymbols() const;
	
private:	
	int fFileHandle;
	void *fBaseAddress;
	char *fPath;	
	char *fStringTable;
	int fStringTableSize;
	char *fSectionStringTable;
	int fSectionStringTableSize;	
};