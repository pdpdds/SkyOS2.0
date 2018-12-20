#pragma once
#include "windef.h"
#include "stdint.h"

bool InitOSSystem(unsigned long magic, unsigned long addr, uint32_t imageBase, bool bGraphicMode, int width, int height, int bpp);