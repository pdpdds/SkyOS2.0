#include "SkyDebugger.h"
#include "SkyConsole.h"
#include "ProcessManager.h"
#include "Thread.h"
#include "SkyModuleManager.h"
#include "Exception.h"
#include "SkyMockInterface.h"
#include "StorageManager.h"
#include "stdio.h"
#include "kheap.h"
#include "SkyAPI.h"
#include "Process.h"
#include "I_MapFileReader.h"

SkyDebugger* SkyDebugger::m_pDebugger = nullptr;

typedef I_MapFileReader*(*PGetDebugEngineDLL)();

SKY_FILE_Interface g_FileInterface =
{
	fread,
	fopen,
	fwrite,
	fclose,
	feof,
	ferror,
	fflush,
	freopen,
	fseek,
	ftell,
	fgetc,
	fgets,
};

u32int sky_kmalloc(u32int sz)
{
	return kmalloc(sz);
}

void sky_kfree(void *p)
{
	kfree(p);
}

unsigned int sky_kcalloc(unsigned int count, unsigned int size)
{
	return calloc(count, size);
}

void* sky_krealloc(void* ptr, size_t size)
{
	return krealloc(ptr, size);
}

SKY_ALLOC_Interface g_allocInterface =
{
	sky_kmalloc,
	sky_kfree,
	sky_kcalloc,
	sky_krealloc,
};


void sky_printf(const char* str, ...)
{
	char *p = new char[128];
	va_list ap;

	va_start(ap, str);
	(void)vsprintf(p, str, ap);
	va_end(ap);

	SkyConsole::Print(p);
	delete p;
}


SKY_Print_Interface g_printInterface =
{
	sky_printf,
	0,
	0,
	0,
	0,
};

unsigned int sky_kcreate_process_from_memory(const char* appName, LPTHREAD_START_ROUTINE lpStartAddress, void* param, UINT32 processType)
{
	return 0;
}

unsigned int sky_kcreate_process_from_file(char* appName, void* param, UINT32 processType)
{
	return 0;
}

unsigned int sky_kcreate_thread_from_memory(unsigned int processId, LPTHREAD_START_ROUTINE lpStartAddress, LPVOID param)
{
	DWORD dwThreadId = 0;

	kEnterCriticalSection();
	//Process* pProcess = ProcessManager::GetInstance()->FindProcess(processId);
	Process* pProcess = ProcessManager::GetInstance()->GetCurrentTask()->m_pParent;
	Thread* pThread = ProcessManager::GetInstance()->CreateThread(pProcess, lpStartAddress, param);

	SKY_ASSERT(pThread != nullptr, "MainThread is null.");

	if(pProcess->m_threadList.size() > 0)
		pProcess->AddThread(pThread);	

	kLeaveCriticalSection();

	return (unsigned int)pThread;
}

unsigned int sky_kcreate_thread_from_file(unsigned int processId, FILE* pFile, LPVOID param)
{
	return 0;
}


unsigned int sky_kdestroy_task(unsigned int taskId)
{

	return 0;
}

void sky_ksleep(int ms)
{
	ksleep(ms);
}

void sky_enter_critical_section()
{
	kEnterCriticalSection();
}

void sky_leave_critical_section()
{
	kLeaveCriticalSection();
}

extern unsigned int GetTickCount();
unsigned int sky_get_tick_count()
{
	return GetTickCount();
}


//���μ��� ���� �� ����
SKY_PROCESS_INTERFACE g_processInterface =
{
	sky_kcreate_process_from_memory,
	sky_kcreate_process_from_file,
	sky_kcreate_thread_from_memory,
	sky_kcreate_thread_from_file,
	sky_kdestroy_task,
	sky_ksleep,
	0,
	0,
	sky_enter_critical_section,
    sky_leave_critical_section,
	sky_get_tick_count,
};

SkyDebugger::SkyDebugger()
{
	m_pMapReader = nullptr;
	m_symbolInit = false;
}


SkyDebugger::~SkyDebugger()
{
}

void SkyDebugger::TraceStackWithSymbol(unsigned int maxFrames)
{
	//���� ��Ȳ
	//  ù��° �Ķ���� maxFrames
	//  TraceStackWithSymbol �Լ��� �����Ų ȣ���Լ� �����ּ�
	//  ȣ���Լ��� EBP(���� EBP �������Ͱ� �̰��� ����Ű�� �ִ�)
	unsigned int* ebp = &maxFrames - 2;
	SkyConsole::Print("Stack trace:\n");
	//���� �½�ũ�� ����.
	Thread* pTask = ProcessManager::GetInstance()->GetCurrentTask();

	int lineNumber = 0;
	DWORD resultAddress = 0;
	char function[256];
	char objFileName[256];
	char filename[256];
	char undecorateName[256];

	for (unsigned int frame = 0; frame < maxFrames; ++frame)
	{
		unsigned int eip = ebp[1];

		//�Լ� �����ּҰ� 0�̸� �ݽ��� ����� ������.
		if (eip == 0)			
			break;

		// ���� ȣ���Լ��� �������������� �̵��Ѵ�.
		ebp = reinterpret_cast<unsigned int *>(ebp[0]);
		unsigned int * arguments = &ebp[2];

		if (m_symbolInit == true && m_pMapReader != nullptr)
		{
			// �ɹ��������� ���� �ش��ּ��� �Լ��̸� ���� ���� ���´�.
			memset(function, 0, 256);
			memset(objFileName, 0, 256);
			memset(filename, 0, 256);
			memset(undecorateName, 0, 256);

			bool result = m_pMapReader->getAddressInfo(eip, objFileName, filename, lineNumber, function, resultAddress);

			if (result == true)
			{
				result = m_pMapReader->SimpleUndecorateCPP(function, undecorateName, 256);
				if (result == true)
				{
					SkyConsole::Print("  %s + 0x%x, %s", undecorateName, eip - resultAddress, objFileName);
				}
				else
				{
					SkyConsole::Print("  %s + 0x%x, %s", function, eip - resultAddress, objFileName);
				}
				
			}
		}
		else
		{
			SkyConsole::Print("  0x{%x}", eip);
		}		
	}
}

//������� ������ ���� ���������� ȣ��� �Լ��� ǥ����� �ʴ´�.
//������ TraceStackWithSymbol �޼ҵ�� �����غ��� �� �� �ִ�.
//�ذ�å
//1. Ÿ�� ���μ����� ������ ���丮�� ��ü�� �� EIP �������͸� �����Ѵ�.
//2. ������ ���丮�� ������� �����ѵ� EIP �ּҿ� �ش��ϴ� �ɹ��� ��´�.
void SkyDebugger::TraceStackWithProcessId(int processId)
{
	kEnterCriticalSection();

#ifdef SKY_EMULATOR
	Process* pProcess = ProcessManager::GetInstance()->GetCurrentTask()->m_pParent;
#else
	Process* pProcess = ProcessManager::GetInstance()->FindProcess(processId);	
#endif
	Thread* pTask = ProcessManager::GetInstance()->GetCurrentTask();

	if (pProcess == nullptr)
	{
		SkyConsole::Print("process not found!!\n");
	}
	else
	{
		if (pTask->m_pParent == pProcess)
		{
			kLeaveCriticalSection();
			TraceStackWithSymbol();
			return;
		}

		SkyConsole::Print("Stack trace:\n");
		Thread* pThread = pProcess->GetMainThread();
		unsigned int* ebp = (unsigned int*)pThread->m_contextSnapshot.ebp;		

		int lineNumber = 0;
		DWORD resultAddress = 0;
		char function[256];
		char objFileName[256];
		char filename[256];
		char undecorateName[256];

		//SkyConsole::Print("  0x%x 0x%x 0x%x\n", ebp, ebp[0], ebp[1]);

		for (unsigned int frame = 0; frame < 20; ++frame)
		{
			unsigned int eip = ebp[1];
			if (eip == 0)
				//�Լ� �����ּҰ� 0�̸� �ݽ��� ����� ������.
				break;

			// ���� ȣ���Լ��� �������������� �̵��Ѵ�.
			ebp = reinterpret_cast<unsigned int *>(ebp[0]);
			unsigned int * arguments = &ebp[2];

			if (m_symbolInit == true && m_pMapReader != nullptr)
			{
				// �ɹ��������� ���� �ش��ּ��� �Լ��̸� ���� ���� ���´�.
				memset(function, 0, 256);
				memset(objFileName, 0, 256);
				memset(filename, 0, 256);
				memset(undecorateName, 0, 256);
				
				bool result = m_pMapReader->getAddressInfo(eip, objFileName, filename, lineNumber, function, resultAddress);
								

				if (result == true)
				{
					result = m_pMapReader->SimpleUndecorateCPP(function, undecorateName, 256);
					if (result == true)
					{
						SkyConsole::Print("  %s + 0x%x, %s", undecorateName, eip - resultAddress, objFileName);
					}
					else
					{
						SkyConsole::Print("  %s + 0x%x, %s", function, eip - resultAddress, objFileName);
					}

				}
			}
			else
			{
				SkyConsole::Print("  0x{%x}", eip);
			}
		}
	}

	kLeaveCriticalSection();
}

//����׿��� ����� �ε��Ѵ�.
bool SkyDebugger::LoadSymbol(const char* moduleName)
{	
	
	void* hwnd = SkyModuleManager::GetInstance()->LoadModule(moduleName);
	PGetDebugEngineDLL GetDebugEngineDLLInterface = (PGetDebugEngineDLL)SkyModuleManager::GetInstance()->GetModuleFunction(hwnd, "GetDebugEngineDLL");

	if (!GetDebugEngineDLLInterface)
	{
		HaltSystem("Memory Module Load Fail!!");
	}
	
	m_pMapReader = GetDebugEngineDLLInterface();
	
	//m_pMapReader = new MapFileReader();

	if (m_pMapReader == nullptr)
	{
		HaltSystem("Map Reader Creation Fail!!");
	}
	
	StorageManager::GetInstance()->SetCurrentFileSystemByID('L');

	bool result = m_pMapReader->readFile("SkyOS32.map");	
	
	if (result == false)
	{
		HaltSystem("Map Read Fail!!");
	}
	
	unsigned int preferredAddress = m_pMapReader->getPreferredLoadAddress();
	m_pMapReader->setLoadAddress(preferredAddress);

	m_symbolInit = true;
	return true;
}
