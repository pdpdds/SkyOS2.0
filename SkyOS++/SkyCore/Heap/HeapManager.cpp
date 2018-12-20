#include "HeapManager.h"
#include "kheap.h"
#include "BasicStruct.h"
#include "memory_layout.h"
#include "X86Arch.h"

using namespace VirtualMemoryManager;

extern DWORD g_usedHeapSize;

namespace HeapManager
{
	int m_heapFrameCount = 0;

	DWORD GetHeapSize() { return m_heapFrameCount * PAGE_SIZE; }

	DWORD GetUsedHeapSize() { return g_usedHeapSize; }

	bool Initialize()
	{		
		m_heapFrameCount = KERNEL_HEAP_FRAME_COUNT;

		//�䱸�Ǵ� ���� ũ�Ⱑ ������������ ũ�ٸ� �� ũ�⸦ �������� ũ��� ������� ������ ���δ�.
		uint32_t memorySize = bootParams._memorySize;

		
		PageDirectory* curPageDirectory = GetKernelPageDirectory();
		
		//���� �����ּ�
#ifdef SKY_EMULATOR
		void* pVirtualHeap = (void*)(bootParams.allocatedRange[0].begin + 0x2000000);
#else
		DWORD pVirtualHeap = bootParams._memoryLayout._kHeapBase;
		VirtualMemoryManager::MapAddress(GetKernelPageDirectory(), pVirtualHeap, m_heapFrameCount);
#endif // SKY_EMULATOR		
		
#ifdef _HEAP_DEBUG
		printf("kernel heap allocation success. frame count : %d\n", m_heapFrameCount);
#endif

		int virtualEndAddress = (uint32_t)pVirtualHeap + m_heapFrameCount * PAGE_SIZE;

		EnablePaging(true);

		//���� �Ҵ�� ���� �ּ� ������ ����ؼ� �� �ڷᱸ���� �����Ѵ�. 
		create_kernel_heap((u32int)pVirtualHeap, (uint32_t)virtualEndAddress, (uint32_t)virtualEndAddress, 0, 0);
			
		return true;
	}

	void Dump()
	{
#ifdef _HEAP_DEBUG
		SkyConsole::Print("Heap Physical Start Address 0x%x\n", m_pKernelHeapPhysicalMemory);
		SkyConsole::Print("Heap Physical End Address 0x%x\n", (int)m_pKernelHeapPhysicalMemory + m_heapFrameCount * PMM_BLOCK_SIZE);

		SkyConsole::Print("Heap Virtual Start Address 0x%x\n", pVirtualHeap);
		SkyConsole::Print("Heap Virtual End Address 0x%x\n", virtualEndAddress);
#endif
	}
}