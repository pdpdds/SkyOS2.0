#pragma once
#include "VirtualMemoryManager.h"

namespace HeapManager
{
	bool Initialize();
		
	DWORD GetHeapSize();
	DWORD GetUsedHeapSize();
	void Dump();
}