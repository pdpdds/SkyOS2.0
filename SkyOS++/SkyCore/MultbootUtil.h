#pragma once
#include "MultiBoot.h"
#include "BasicStruct.h"

bool GetMemoryInfo(multiboot_info* bootinfo, BootParams* pParam);
uint32_t GetKernelSize(multiboot_info* bootinfo);

