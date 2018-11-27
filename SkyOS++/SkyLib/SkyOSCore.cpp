#include "SkyOSCore.h"
#include "windef.h"
#include "stdint.h"
#include "Page.h"
#include "PageCache.h"
#include "physicalmap.h"
#include "AddressSpace.h"
#include "Defines.h"

typedef struct tag_PlatformAPI
{

};
tag_PlatformAPI;

bool BuildPlatform()
{
#ifdef SKY_EMULATOR

#else
#endif
	return true;
}

bool InitKernelSystem()
{
	BuildPlatform();


	Page::Bootstrap();
	PageCache::Bootstrap();
	PhysicalMap::Bootstrap();
	AddressSpace::Bootstrap();

	return true;
}