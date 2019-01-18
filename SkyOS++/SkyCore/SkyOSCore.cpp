#include "SkyOSCore.h"
#include "windef.h"
#include "stdint.h"
#include "Page.h"
#include "PageCache.h"
#include "physicalmap.h"
#include "AddressSpace.h"
#include "Defines.h"
#include "BootParams.h"
#include "Timer.h"
#include "Team.h"
#include "datastructure\/Thread.h"
#include "Processor.h"
#include "FileSystem.h"

bool BuildPlatform()
{
#ifdef SKY_EMULATOR

#else
#endif
	return true;
}

_BootParams _bootParams;
extern "C" void printf(const char *fmt, ...);
unsigned int g_kernelPageDirectory;



bool InitKernelSystem(_BootParams* param, unsigned int kernelPageDirectory)
{
	//BuildPlatform();
	g_kernelPageDirectory = kernelPageDirectory;
	_bootParams.memsize = param->memsize;
	for (int i = 0; i < param->rangeCount; i++)
	{
		_bootParams.SetAllocated(param->allocatedRange[i].begin, param->allocatedRange[i].end);
		printf("%x %x\n", param->allocatedRange[i].begin, param->allocatedRange[i].end);
	}	
	
	Thread::Bootstrap();	
	Timer::Bootstrap();		
	
	Page::Bootstrap();	
	PageCache::Bootstrap();	

	PhysicalMap::Bootstrap();
	
	AddressSpace::Bootstrap();	
	
	Team::Bootstrap();
	
	Processor::Bootstrap();	
	FileSystem::Bootstrap();

	printf("complete\n");	
	
	//

	return true;
}

void StartNativeSystem(void* param)
{
	AddressSpace::PageDaemonLoop();
}