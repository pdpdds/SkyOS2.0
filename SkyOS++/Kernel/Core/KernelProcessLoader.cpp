#include "SkyOS.h"
#include "KernelProcessLoader.h"

KernelProcessLoader::KernelProcessLoader()
{
}


KernelProcessLoader::~KernelProcessLoader()
{

}

Process* KernelProcessLoader::CreateProcessFromMemory(const char* appName, LPTHREAD_START_ROUTINE lpStartAddress, void* param)
{
	//���μ����� �����ϰ� ���μ��� ���̵� �Ҵ��Ѵ�.
	Process* pProcess = new Process();
	pProcess->SetProcessId(GetNextProcessId());
	
	PageDirectory* pPageDirectory = VirtualMemoryManager::GetKernelPageDirectory();

	//����¡ ����� ���� ������ ���丮�� �����Ѵ�.
	/*PhysicalMemoryManager::EnablePaging(false);

	pPageDirectory = VirtualMemoryManager::CreateCommonPageDirectory();

	if (pPageDirectory == nullptr)
	{
		PhysicalMemoryManager::EnablePaging(true);
		return nullptr;
	}

	//�������� ������ ���丮�� �����Ѵ�.
	HeapManager::MapHeapToAddressSpace(pPageDirectory);

	//�׷��� ���� �ּҸ� ������ ���丮�� �����Ѵ�.
	if (SkyGUISystem::GetInstance()->GUIEnable() == true)
	{	
		uintptr_t videoAddress = (uintptr_t)SkyGUISystem::GetInstance()->GetVideoRamInfo()._pVideoRamPtr;
		VirtualMemoryManager::CreateVideoDMAVirtualAddress(pPageDirectory, videoAddress, videoAddress, videoAddress + VIDEO_RAM_LOGICAL_ADDRESS_OFFSET);
	}
	
	//����¡�� Ȱ��ȭ�Ѵ�.
	PhysicalMemoryManager::EnablePaging(true);*/

	pProcess->SetPageDirectory(pPageDirectory);

	pProcess->m_dwRunState = TASK_STATE_INIT;
	strcpy(pProcess->m_processName, appName);

	pProcess->m_dwProcessType = PROCESS_KERNEL;
	pProcess->m_dwPriority = 1;

	return pProcess;
}

Process* KernelProcessLoader::CreateProcessFromFile(char* appName, void* param)
{
	//Not Implemented
	return nullptr;
}
