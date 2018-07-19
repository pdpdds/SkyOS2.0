#include "SkyOS.h"

void __SKY_ASSERT(const char* expr_str, bool expr, const char* file, int line, const char* msg)
{
	if (!expr)
	{
		//SkyConsole::Print("%s %s, %s %d\n", msg, expr_str, file, line);
		//for (;;);
		char buf[256];
		sprintf(buf, "Assert failed: %s Expected: %s %s %d\n", msg, expr_str, file, line);		

		HaltSystem(buf);
	}
}

/////////////////////////////////////////////////////////////////////////////
//������
/////////////////////////////////////////////////////////////////////////////
DWORD SKYAPI kGetCurrentThreadId()
{
	DWORD dwThreadId = 0;
	kEnterCriticalSection();
	Thread* pThread = ProcessManager::GetInstance()->GetCurrentTask();
	if (pThread != nullptr)
		dwThreadId = pThread->GetThreadId();

	kLeaveCriticalSection();

	return dwThreadId;
}

bool GetLocalTime(LPSYSTEMTIME lpSystemTime)
{
	/* Checking whether we can read the time now or not according to some documentation the MSB in Status A will remain 1 (invalid time) only a millisecond*/
	int TimeOut;

	OutPortByte(RTC_INDEX_REG, RTC_STATUS_A);    //check status - read access
	TimeOut = 1000;
	while (InPortByte(RTC_VALUE_REG) & 0x80)
		if (TimeOut < 0)
			return false;
		else
			TimeOut--;

	OutPortByte(RTC_INDEX_REG, RTC_DAY);         //get day value
	lpSystemTime->wDay = InPortByte(RTC_VALUE_REG);
	OutPortByte(RTC_INDEX_REG, RTC_MONTH);       //get month value
	lpSystemTime->wMonth = InPortByte(RTC_VALUE_REG);
	OutPortByte(RTC_INDEX_REG, RTC_YEAR);        //get year
	lpSystemTime->wYear = InPortByte(RTC_VALUE_REG);

	OutPortByte(RTC_INDEX_REG, RTC_DAY_OF_WEEK); //get day of week - **** problem
	lpSystemTime->wDayOfWeek = InPortByte(RTC_VALUE_REG);

	OutPortByte(RTC_INDEX_REG, RTC_SECOND);
	lpSystemTime->wSecond = InPortByte(RTC_VALUE_REG);
	OutPortByte(RTC_INDEX_REG, RTC_MINUTE);
	lpSystemTime->wMinute = InPortByte(RTC_VALUE_REG);
	OutPortByte(RTC_INDEX_REG, RTC_HOUR);
	lpSystemTime->wHour = InPortByte(RTC_VALUE_REG);

	OutPortByte(RTC_INDEX_REG, RTC_STATUS_B);
	OutPortByte(RTC_VALUE_REG, 2);


	lpSystemTime->wYear = (lpSystemTime->wYear / 16) * 10 + (lpSystemTime->wYear % 16);
	lpSystemTime->wMonth = (lpSystemTime->wMonth / 16) * 10 + (lpSystemTime->wMonth % 16);
	lpSystemTime->wDay = (lpSystemTime->wDay / 16) * 10 + (lpSystemTime->wDay % 16);
	lpSystemTime->wHour = (lpSystemTime->wHour / 16) * 10 + (lpSystemTime->wHour % 16);
	lpSystemTime->wMinute = (lpSystemTime->wMinute / 16) * 10 + (lpSystemTime->wMinute % 16);
	lpSystemTime->wSecond = (lpSystemTime->wSecond / 16) * 10 + (lpSystemTime->wSecond % 16);

	return true;
}
/* Returns 1 on success and 0 on failue */
BYTE SetLocalTime(LPSYSTEMTIME lpSystemTime)
{
	/* Checking whether we can read the time now or not according to some documentation the MSB in Status A will   remain 1 (invalid time) only a millisecond*/
	int TimeOut;

	OutPortByte(RTC_INDEX_REG, RTC_STATUS_A);    //checking status -read access
	TimeOut = 1000;
	while (InPortByte(RTC_VALUE_REG) & 0x80)
		if (TimeOut < 0)
			return 0;
		else
			TimeOut--;

	OutPortByte(RTC_INDEX_REG, RTC_DAY);
	OutPortByte(RTC_VALUE_REG, (uchar)lpSystemTime->wDay);
	OutPortByte(RTC_INDEX_REG, RTC_MONTH);
	OutPortByte(RTC_VALUE_REG, (uchar)lpSystemTime->wMonth);
	OutPortByte(RTC_INDEX_REG, RTC_YEAR);
	OutPortByte(RTC_VALUE_REG, (uchar)lpSystemTime->wYear);

	OutPortByte(RTC_INDEX_REG, RTC_SECOND);
	OutPortByte(RTC_VALUE_REG, (uchar)lpSystemTime->wSecond);
	OutPortByte(RTC_INDEX_REG, RTC_MINUTE);
	OutPortByte(RTC_VALUE_REG, (uchar)lpSystemTime->wMinute);
	OutPortByte(RTC_INDEX_REG, RTC_HOUR);
	OutPortByte(RTC_VALUE_REG, (uchar)lpSystemTime->wHour);

	return 1;
}

DWORD g_dwSysLastError = 0;
CPUID_DATA sysCPUBasicInformation[CPUID_MAX_BASIC_LEVEL];
CPUID_DATA sysCPUExtendedInformation[CPUID_MAX_EXTENDED_LEVEL];

DWORD GetLastError()
{
	return g_dwSysLastError;
}
DWORD SetLastError(DWORD dwErrorCode)
{
	return g_dwSysLastError = dwErrorCode;
}

void ksleep(int millisecond)
{
	msleep(millisecond);
}

void printf(const char* str, ...)
{
	if (!str)
		return;

	va_list		args;
	va_start(args, str);
	size_t i;
	for (i = 0; i < strlen(str); i++) {

		switch (str[i]) {

		case '%':

			switch (str[i + 1]) {

				/*** characters ***/
			case 'c': {
				char c = va_arg(args, char);
				SkyConsole::WriteChar(c);
				i++;		// go to next character
				break;
			}

					  /*** address of ***/
			case 's': {
				int c = (int&)va_arg(args, char);
				char str[256];
				strcpy(str, (const char*)c);
				SkyConsole::Write(str);
				i++;		// go to next character
				break;
			}

					  /*** integers ***/
			case 'd':
			case 'i': {
				int c = va_arg(args, int);
				char str[32] = { 0 };
				itoa_s(c, 10, str);
				SkyConsole::Write(str);
				i++;		// go to next character
				break;
			}

					  /*** display in hex ***/
					  /*int*/
			case 'X': 
			case 'p':
			{
				int c = va_arg(args, int);
				char str[32] = { 0 };
				itoa_s(c, 16, str);
				SkyConsole::Write(str);
				i++;		// go to next character
				break;
			}
					  /*unsigned int*/
			case 'x': {
				unsigned int c = va_arg(args, unsigned int);
				char str[32] = { 0 };
				itoa_s(c, 16, str);
				SkyConsole::Write(str);
				i++;		// go to next character
				break;
			}

			default:
				va_end(args);
				return;
			}

			break;

		default:
			SkyConsole::WriteChar(str[i]);
			break;
		}

	}

	va_end(args);
	return;
}

void PauseSystem(const char* msg)
{
	for (;;);
}

HANDLE CreateThread(SIZE_T dwStackSize, LPTHREAD_START_ROUTINE lpStartAddress, LPVOID lpParameter, DWORD dwCreateionFlags, LPDWORD lpThreadId)
{
	Process* cur = ProcessManager::GetInstance()->GetCurrentTask()->m_pParent;

	if (cur->GetProcessId() == PROC_INVALID_ID)
	{
		SkyConsole::Print("CreateThread Fail. Id : d\n", ProcessManager::GetInstance()->GetCurrentTask()->GetThreadId());
		return 0;
	}

	Thread* newThread = ProcessManager::GetInstance()->CreateThread(cur, lpStartAddress, lpParameter);

	if (newThread == nullptr)
	{
		SkyConsole::Print("Thread Create Fail!!\n");
		return 0;
	}

	bool result = cur->AddThread(newThread);

	SKY_ASSERT(result == true, "CreateThread Faill");

	return (HANDLE)newThread;
}
extern "C"
{
	uint32_t MemoryAlloc(size_t size)
	{
		Process* pProcess = ProcessManager::GetInstance()->GetCurrentTask()->m_pParent;
		void *addr = memory_alloc(size, (u8int)0, (heap_t*)pProcess->m_lpHeap);

#ifdef _ORANGE_DEBUG
		SkyConsole::Print("process heap alloc, %d %x\n", size, pProcess->m_lpHeap);
#endif			
		return (u32int)addr;
	}

	void MemoryFree(void* p)
	{
		//���� �����尡 ��� �����Ѵ�.
		//���� �޸𸮸� �����ҽ� ���ؽ�Ʈ ����Ī�� �Ͼ�� �ٸ� �����尡 ���� �ڿ�(��)�� ������ �� �ִ� ���ɼ��� ����Ƿ�
		//���ͷ�Ʈ�� �Ͼ�� �ʰ� ó���Ѵ�.
		kEnterCriticalSection();
		Thread* pTask = ProcessManager::GetInstance()->GetCurrentTask();
		Process* pProcess = pTask->m_pParent;
		free(p, (heap_t*)pProcess->m_lpHeap);
		kLeaveCriticalSection();
	}

	//���μ��� ������ ����Ʈ ���� �����Ѵ�
	void CreateDefaultHeap()
	{
		kEnterCriticalSection();

		Thread* pTask = ProcessManager::GetInstance()->GetCurrentTask();
		Process* pProcess = pTask->m_pParent;

		//1�ް� ����Ʈ�� ���� ����
		void* pHeapPhys = PhysicalMemoryManager::AllocBlocks(DEFAULT_HEAP_PAGE_COUNT);
		u32int heapAddess = pProcess->m_imageBase + pProcess->m_imageSize + PAGE_SIZE + PAGE_SIZE * 2;

		//�� �ּҸ� 4K�� ���� Align	
		heapAddess -= (heapAddess % PAGE_SIZE);

		//#ifdef _ORANGE_DEBUG
		SkyConsole::Print("%d : V(%x) P(%x)\n", pProcess->GetProcessId(), heapAddess, pHeapPhys);
		//#endif // _ORANGE_DEBUG

		for (int i = 0; i < DEFAULT_HEAP_PAGE_COUNT; i++)
		{
			VirtualMemoryManager::MapPhysicalAddressToVirtualAddresss2(pProcess->GetPageDirectory(),
				(uint32_t)heapAddess + i * PAGE_SIZE,
				(uint32_t)pHeapPhys + i * PAGE_SIZE,
				I86_PTE_PRESENT | I86_PTE_WRITABLE | I86_PTE_USER);
		}

		memset((void*)heapAddess, 0, DEFAULT_HEAP_PAGE_COUNT * PAGE_SIZE);

		pProcess->m_lpHeap = create_heap((u32int)heapAddess, (uint32_t)heapAddess + DEFAULT_HEAP_PAGE_COUNT * PAGE_SIZE,
			(uint32_t)heapAddess + DEFAULT_HEAP_PAGE_COUNT * PAGE_SIZE, 0, 0);

		kLeaveCriticalSection();

		//#ifdef _ORANGE_DEBUG
		SkyConsole::Print("CreateDefaultHeap End\n");
		//#endif // _ORANGE_DEBUG

	}

	//���μ��� ����	
	extern "C" void TerminateProcess()
	{
		kEnterCriticalSection();

		Thread* pTask = ProcessManager::GetInstance()->GetCurrentTask();
		Process* pProcess = pTask->m_pParent;

		if (pProcess == nullptr || pProcess->GetProcessId() == PROC_INVALID_ID)
		{
			SkyConsole::Print("Invailid Process Termination\n");
			kLeaveCriticalSection();
			return;
		}

		//���μ��� �Ŵ������� �ش� ���μ����� ������ �����Ѵ�.
		//�½�ũ ��Ͽ����� ���ŵǾ� �ش� ���μ����� ���̻� �����층 ���� �ʴ´�.		
		//ProcessManager::GetInstance()->RemoveProcess(pProcess->GetProcessId());

		ProcessManager::GetInstance()->ReserveRemoveProcess(pProcess);

		kLeaveCriticalSection();
	}
}

void DumpMemory(void *data, size_t nbytes, size_t bytes_per_line)
{
	uint8_t *mem = (uint8_t *)data;
	for (size_t y = 0; y < nbytes; y++)
	{
		if (y == 0 || y % bytes_per_line == 0)
		{
			if (y % bytes_per_line == 0 && y)
			{
				printf("%c[%uG", 0x1B, 10 + bytes_per_line * 3);
				printf("| ");
				for (size_t i = 0; i < bytes_per_line; i++)
				{
					char c = mem[y - bytes_per_line + i];
					printf("%c", c >= ' ' && c <= '~' ? c : '.');
				}
				printf("\n");
			}
			printf("%08x: ", (uint32_t)data + y);

		}

		printf("%02x ", mem[y]);

	}

	printf("%c[%uG", 0x1B, 10 + bytes_per_line * 3);
	printf("| ");

	size_t x = (nbytes / bytes_per_line) * bytes_per_line;
	if (x == nbytes) {
		x = nbytes - bytes_per_line;
	}

	for (size_t y = x; y < nbytes; y++) {
		char c = mem[y];
		printf("%c", c >= ' ' && c <= '~' ? c : '.');
	}

	printf("\n");
}

int kdbg_printf(char *pFmt, ...)
{
	va_list 	va;
	int 		nI;
	char		szTX[1024];

	va_start(va, pFmt);
	nI = vsprintf(szTX, pFmt, va);
	va_end(va);

	SkyConsole::Print("%s", szTX);

	return(nI);
}

typedef struct tag_SKY_APIStruct
{
	char * strAPIName;		//�Լ� �̸�
	void* ptrAPIFunction;		//�Լ� ������
	tag_SKY_APIStruct * Next;
}SKY_APIStruct;
SKY_APIStruct* g_pRegisteredSkyAPIEntries = nullptr;

void RegisterSkyAPI(char* strAPIName, void * ptrAPIFunction)
{
	SKY_APIStruct* newAPIStruct;
	char *strName = nullptr;

	SkyConsole::Print("Reg : %s %x\n", strAPIName, ptrAPIFunction);

	newAPIStruct = new SKY_APIStruct;
	strName = new char[strlen(strAPIName) + 1];

	strcpy(strName, strAPIName);

	//�Լ��̸��� �Լ� �����͸� ����
	newAPIStruct->strAPIName = strName;
	newAPIStruct->ptrAPIFunction = ptrAPIFunction;
	newAPIStruct->Next = nullptr;

	if (g_pRegisteredSkyAPIEntries) //��Ʈ ��Ʈ���� �����ϸ� �˻��� �ؼ� �������� �� ��Ʈ���� �����ϰ� �׷��� ������ �� ��Ʈ���� ��Ʈ ��Ʈ����
	{
		SKY_APIStruct* curAPIStruct = g_pRegisteredSkyAPIEntries;
		while (curAPIStruct->Next)
		{
			if (strcmp(curAPIStruct->strAPIName, strAPIName) == 0) //already the function exists update the pointer value
				break;
			curAPIStruct = curAPIStruct->Next;
		}
		curAPIStruct->Next = newAPIStruct;
	}
	else
		g_pRegisteredSkyAPIEntries = newAPIStruct;
}

void* GetSkyAPIEntryPoint(char * strAPIName)
{
	SKY_APIStruct* curAPIStruct = g_pRegisteredSkyAPIEntries;
	while (curAPIStruct)
	{
		SkyConsole::Print("[%s] [%s]\n", strAPIName, curAPIStruct->strAPIName);

		if (strcmp(curAPIStruct->strAPIName, strAPIName) == 0)
			return curAPIStruct->ptrAPIFunction;

		curAPIStruct = curAPIStruct->Next;
	}

	return 0;
}