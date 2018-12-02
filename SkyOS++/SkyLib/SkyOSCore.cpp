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

bool BuildPlatform()
{
#ifdef SKY_EMULATOR

#else
#endif
	return true;
}

_BootParams _bootParams;
extern "C" void printf(const char *fmt, ...);
bool InitKernelSystem()
{
	//BuildPlatform();

	Timer::Bootstrap();
	__asm {sti}	
	for (;;);
	Page::Bootstrap();
	PageCache::Bootstrap();
	PhysicalMap::Bootstrap();
	AddressSpace::Bootstrap();

	return true;
}