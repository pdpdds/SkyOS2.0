#pragma once
#include "windef.h"
#include "stdint.h"
#include "MultiBoot.h"
#include "Hal.h"
	
#define PMM_BLOCK_SIZE	4096	
#define PMM_BLOCK_ALIGN	BLOCK_SIZE
#define PMM_BITS_PER_INDEX	32

namespace PhysicalMemoryManager
{
	void	Initialize();

///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
	void*	AllocBlock();
	void	FreeBlock(void* pa);

	void*	AllocBlocks(size_t);
	void	FreeBlocks(void* pa, size_t);
	
///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
	UINT32	GetFreeMemorySize();	
	void	Dump(); //Debug
}	
