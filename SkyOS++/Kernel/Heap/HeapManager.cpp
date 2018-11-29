#include "HeapManager.h"
#include "SkyConsole.h"
#include "kheap.h"
#include "BasicStruct.h"

using namespace VirtualMemoryManager;

bool g_heapInit = false;
extern DWORD g_usedHeapSize;

namespace HeapManager
{
	int m_heapFrameCount = 0;
	
	//Physical Heap Address
	void* m_pKernelHeapPhysicalMemory = 0;

	DWORD GetHeapSize() { return m_heapFrameCount * PAGE_SIZE; }

	DWORD GetUsedHeapSize() { return g_usedHeapSize; }

	bool InitKernelHeap(int heapFrameCount)
	{
		PageDirectory* curPageDirectory = GetKernelPageDirectory();

		//���� �����ּ�
#ifdef SKY_EMULATOR
		void* pVirtualHeap = (void*)(bootParams.allocatedRange[0].begin + 0x2000000);
#else
		void* pVirtualHeap = (void*)(KERNEL_VIRTUAL_HEAP_ADDRESS);
#endif // SKY_EMULATOR		

		//������ ����ŭ ���� �޸� �Ҵ��� ��û�Ѵ�.
		m_heapFrameCount = heapFrameCount;
		m_pKernelHeapPhysicalMemory = PhysicalMemoryManager::AllocBlocks(m_heapFrameCount);
		
		if (m_pKernelHeapPhysicalMemory == NULL)
		{
#ifdef _HEAP_DEBUG
			SkyConsole::Print("kernel heap allocation fail. frame count : %d\n", m_heapFrameCount);
#endif			
			return false;
		}

//������ �ý��ۿ� �� �����ּҿ� �����ּҸ� ��������.
		MapHeapToAddressSpace(curPageDirectory);

#ifdef _HEAP_DEBUG
		SkyConsole::Print("kernel heap allocation success. frame count : %d\n", m_heapFrameCount);
#endif

		int virtualEndAddress = (uint32_t)pVirtualHeap + m_heapFrameCount * PMM_BLOCK_SIZE;

#ifdef _HEAP_DEBUG
		SkyConsole::Print("Heap Physical Start Address 0x%x\n", m_pKernelHeapPhysicalMemory);
		SkyConsole::Print("Heap Physical End Address 0x%x\n", (int)m_pKernelHeapPhysicalMemory + m_heapFrameCount * PMM_BLOCK_SIZE);

		SkyConsole::Print("Heap Virtual Start Address 0x%x\n", pVirtualHeap);
		SkyConsole::Print("Heap Virtual End Address 0x%x\n", virtualEndAddress);
#endif
		
		//���� �Ҵ�� ���� �ּ� ������ ����ؼ� �� �ڷᱸ���� �����Ѵ�. 
		create_kernel_heap((u32int)pVirtualHeap, (uint32_t)virtualEndAddress, (uint32_t)virtualEndAddress, 0, 0);

		g_heapInit = true;
		return true;
	}

	bool MapHeapToAddressSpace(PageDirectory* curPageDirectory)
	{
		
		int endAddress = (uint32_t)KERNEL_VIRTUAL_HEAP_ADDRESS + m_heapFrameCount * PMM_BLOCK_SIZE;
		//int frameCount = (endAddress - KERNEL_VIRTUAL_HEAP_ADDRESS) / PAGE_SIZE;

		for (int i = 0; i < m_heapFrameCount; i++)
		{
			uint32_t virt = (uint32_t)KERNEL_VIRTUAL_HEAP_ADDRESS + i * PAGE_SIZE;
			uint32_t phys = (uint32_t)m_pKernelHeapPhysicalMemory + i * PAGE_SIZE;
			
			MapPhysicalAddressToVirtualAddresss(curPageDirectory, virt, phys, I86_PTE_PRESENT | I86_PTE_WRITABLE);
			
		}

		return true;
	}
}