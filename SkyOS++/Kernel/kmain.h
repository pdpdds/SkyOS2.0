#pragma once
#include "SkyOS.h"

extern "C" void __cdecl InitializeConstructors();
void SetInterruptVector();
void InitHardware();
bool InitMemoryManager();
bool BuildPlatformAPI(unsigned long addr, uint32_t imageBase);
bool InitModules(multiboot_info* pInfo);
bool InitDisplaySystem();

#ifdef SKY_EMULATOR
void kmain();
#else
void kmain(unsigned long, unsigned long, uint32_t imageBase);
#endif
