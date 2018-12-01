#pragma once
#include "windef.h"

#define MEMORY_REGION_AVAILABLE 1
#define MEMORY_REGION_HIBERNATE 2

#define KERNEL_HEAP_FRAME_COUNT 12800

typedef struct tag_BootModule
{
	UINT32 ModuleStart;
	UINT32 ModuleEnd;
	char *Name;
	unsigned int Reserved;
}BootModule;

typedef struct tag_MemoryLayout
{
	UINT32 _kernelBase;		
	UINT32 _kernelDataBase;
	UINT32 _kernelDataTop;
	UINT32 _bootStackBase;
	UINT32 _bootStackTop;
	UINT32 _kHeapBase;
	UINT32 _kHeapTop;

	UINT32 _kernelIOAreaBase;
	UINT32 _kernelIOAreaTop;
	UINT32 _kernelTop;	
}MemoryLayout;

/*#define KERNEL_LOAD_BASE_ADDRESS 0x01600000
#define STACK_BASE_ADDRESS 0x08000000
#define USER_VIRTUAL_STACK_ADDRESS				0x00F00000

#define KERNEL_VIRTUAL_HEAP_ADDRESS				0x10000000
#define KERNEL_VIRTUAL_RAMDISK_ADDRESS			0x30000000*/

typedef struct tag_BootParams
{		
	UINT64 _kernelSize;
	MemoryLayout _memoryLayout;	

	char _szBootLoaderName[256];
	int  _moduleCount;
	BootModule *Modules;

	UINT64 framebuffer_addr;
	UINT32 framebuffer_pitch;
	UINT32 framebuffer_width;
	UINT32 framebuffer_height;
	uint8 framebuffer_bpp;

#define MULTIBOOT_FRAMEBUFFER_TYPE_INDEXED 0
#define MULTIBOOT_FRAMEBUFFER_TYPE_RGB     1
#define MULTIBOOT_FRAMEBUFFER_TYPE_EGA_TEXT     2
		uint8 framebuffer_type;

	UINT64 _memorySize;
	int _rangeCount;

	struct Range {
		UINT64 begin;
		UINT64 end;
		int type;		
	} allocatedRange[64];

	tag_BootParams()
	{		
		_memorySize = 0;
		_rangeCount = 0;		
		_moduleCount = 0;
	}

	inline void SetAllocated(UINT64 begin, UINT64 end, int type)
	{
		allocatedRange[_rangeCount].begin = begin;
		allocatedRange[_rangeCount].end = end;
		allocatedRange[_rangeCount].type = type;
		_rangeCount++;		
	}

}BootParams;

extern BootParams bootParams;