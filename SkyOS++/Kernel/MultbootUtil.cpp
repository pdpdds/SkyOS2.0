#include "MultbootUtil.h"
#include "windef.h"
#include "MultiBoot.h"
#include "KernelAPI.h"
#include "SkyStartOption.h"
#include "Constants.h"

bool GetMemoryInfo(multiboot_info* bootinfo, BootParams* pParam)
{
	uint32_t mmapEntryNum = bootinfo->mmap_length / sizeof(multiboot_memory_map_t);

	multiboot_mmap_entry* mmapAddr = (multiboot_mmap_entry*)bootinfo->mmap_addr;

	uint64_t memorySize = 0;

#ifdef _SKY_DEBUG
	SkyConsole::Print("Memory Map Entry Num : %d\n", mmapEntryNum);
#endif
	
	for (uint32_t i = 0; i < mmapEntryNum; i++)
	{
		uint64_t areaStart = (uint64_t)mmapAddr[i].baseAddressLower | ((uint64_t)mmapAddr[i].baseAddressHigher << 32);
		uint64_t areaEnd = areaStart + ((uint64_t)mmapAddr[i].lengthLower | ((uint64_t)mmapAddr[i].lengthHigher << 32));
		
		SkyConsole::Print("%q, %q, %d, type : %d\n", areaStart, areaEnd, mmapAddr[i].size, mmapAddr[i].type);
		pParam->SetAllocated(areaStart, areaEnd, mmapAddr[i].type);
		
		if (mmapAddr[i].type == MEMORY_REGION_AVAILABLE && areaEnd > memorySize)
			memorySize = areaEnd;
	}

	pParam->_memorySize = memorySize;
	
	return true;
}

uint32_t GetKernelSize(multiboot_info* bootinfo)
{
	uint64_t endAddress = 0;
	uint32_t mods_count = bootinfo->mods_count;   /* Get the amount of modules available */
	uint32_t mods_addr = (uint32_t)bootinfo->Modules;     /* And the starting address of the modules */
	for (uint32_t i = 0; i < mods_count; i++) {
		Module* module = (Module*)(mods_addr + (i * sizeof(Module)));     /* Loop through all modules */

		if (strcmp(module->Name, KERNEL32_NAME) == 0)
		{
			uint32_t moduleStart = PAGE_ALIGN_DOWN((uint32_t)module->ModuleStart);
			uint32_t moduleEnd = PAGE_ALIGN_UP((uint32_t)module->ModuleEnd);

			return moduleEnd - moduleStart;
		}
	}

	return 0;
}