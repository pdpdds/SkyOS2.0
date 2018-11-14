#pragma once
#include "SkyOS.h"

extern "C" void __cdecl InitializeConstructors();
void SetInterruptVector();
void InitContext(multiboot_info* bootinfo);
void InitHardware();
bool InitMemoryManager(multiboot_info* bootinfo);

#ifdef SKY_EMULATOR
void kmain();
#else
void kmain(unsigned long, unsigned long, uint32_t imageBase);
#endif
