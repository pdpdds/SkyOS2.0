#pragma once
#include "windef.h"

namespace HeapManager
{
	bool Initialize();
		
	DWORD GetHeapSize();
	DWORD GetUsedHeapSize();
	void Dump();
}