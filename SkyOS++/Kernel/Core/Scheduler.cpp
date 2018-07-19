#include "SkyOS.h"

Scheduler* Scheduler::m_pScheduler = 0;
extern bool systemOn;
registers_t g_registers;

uint32_t lastTickCount = 0;
extern int g_esp;
extern uint32_t g_pageDirectory;
extern uint32_t g_pageDirectory1;

int entryPoint = 0;
unsigned int procStack = 0;
LPVOID startParam = 0;

void SwitchTask(int tick, registers_t& registers)
{
	if (systemOn == false)
		return;

	Scheduler::GetInstance()->DoSchedule(tick, registers);
}

Scheduler::Scheduler()
{
}

Scheduler::~Scheduler()
{
}

bool  Scheduler::DoSchedule(int tick, registers_t& registers)
{

#ifdef _ORANGE_DEBUG
	/*uint32_t currentTickCount = GetTickCount();

	if (currentTickCount - lastTickCount > 300)
	{
		SkyConsole::Print("\nSwitch Stack Report\n");

		SkyConsole::Print("EDI : %x\n", registers.edi);
		SkyConsole::Print("ESI : %x\n", registers.esi);
		SkyConsole::Print("EBP : %x\n", registers.ebp);
		SkyConsole::Print("ESP : %x\n", registers.esp);
		SkyConsole::Print("EBX : %x\n", registers.ebx);
		SkyConsole::Print("EDX : %x\n", registers.edx);
		SkyConsole::Print("ECX : %x\n", registers.ecx);
		SkyConsole::Print("EAX : %x\n", registers.eax);

		SkyConsole::Print("EIP : %x\n", registers.eip);
		SkyConsole::Print("CS : %x\n", registers.cs);
		SkyConsole::Print("EFLAGS : %x\n", registers.eflags);
		SkyConsole::Print("USERESP : %x\n", registers.useresp);
		SkyConsole::Print("SS : %x\n", registers.ss);

		lastTickCount = currentTickCount;
	}*/
#endif	
			
	ProcessManager::TaskList* pTaskList = ProcessManager::GetInstance()->GetTaskList();

	int taskCount = pTaskList->size();

	if (taskCount == 0)
		HaltSystem("Task Count Is 0\n");

	//�½�ũ�� �ϳ����̶�� �����층�� �� �ʿ䰡 ����.
	if (taskCount == 1)
		return true;

	ProcessManager::TaskList::iterator iter = pTaskList->begin();

	Thread* pThread = *iter;

	pThread->m_waitingTime--;
	
	//�½�ũ ����Ʈ���� ���ο� �ִ� �½�ũ�� ���� ����ǰ� �ִ� �½�ũ��.
	//����ð��� ���� ���� ������ �½�ũ ����Ī�� ���� �ʴ´�.
	if (pThread->m_waitingTime > 0)
	{
		
		g_pageDirectory = (uint32_t)pThread->m_pParent->GetPageDirectory();
		VirtualMemoryManager::SetCurPageDirectory(pThread->m_pParent->GetPageDirectory());
		return true;
	}

	//�½�ũ�� ����ð��� ����Ǿ����� �½�ũ�� ���¸� �����·� �����ϰ�
	//���� �½�ũ�� �������Ͱ����� �������Ѵ�.
	pThread->m_taskState = TASK_STATE_WAIT;
	pThread->m_contextSnapshot = registers;
	pThread->m_esp = g_esp;
	
	//SkyConsole::Print("\n%s, %s %d, %d\n", __FILE__, __func__, __LINE__, pTaskList->size());
	
	//����� �½�ũ�� ����Ʈ ���ʿ� �ִ´�.
	pTaskList->remove(pThread);
	pTaskList->push_back(pThread);	

	Thread* pNextThread = pTaskList->front();

	Process* pProcess = pNextThread->m_pParent;

	//ó������ ���۵Ǵ� �½�ũ�ΰ�?
	if (pNextThread->m_taskState == TASK_STATE_INIT)
	{		
		
		//�½�ũ�� ���¸� ���������� �����ϰ� ����ð��� Ȯ���Ѵ�.
		pNextThread->m_waitingTime = TASK_RUNNING_TIME;
		pNextThread->m_taskState = TASK_STATE_RUNNING;

		//�½�ũ�� ��Ʈ�� �����Ϳ� �����ּ�, �Ķ���Ͱ��� ��´�.
		entryPoint = (int)pNextThread->frame.eip;
		procStack = pNextThread->frame.esp;
		startParam = pNextThread->m_startParam;

		//������ ���丮�� ���´�.
		PageDirectory* pageDirectory = pNextThread->m_pParent->GetPageDirectory();
		VirtualMemoryManager::SetCurPageDirectory(pageDirectory);
		ProcessManager::GetInstance()->SetCurrentTask(pNextThread);
		{

			_asm
			{				
				mov	eax, [pageDirectory]
				mov	cr3, eax	 	// CR3 �������Ϳ� ������ ���丮 ����
				//�������� ����Ǿ entryPoint procStack startParam�� Ŀ�ο����� ����� ������
				//�������� ��밡���ϴ�.
				mov ecx, [entryPoint]
				mov esp, procStack
				xor ebp, ebp
				push ebp
				mov ebx, [startParam]
			}
			
			__asm
			{
				mov     ax, 0x10;������ ���׸�Ʈ ������ �ʱ�ȭ
				mov     ds, ax
				mov     es, ax
				mov     fs, ax
				mov     gs, ax			

				push    ebx;
				push    0; //EBP
				push    0x200; EFLAGS
				push    0x08; CS
				push    ecx; EIP
				//���ͷ�Ʈ Ȱ��ȭ�� �� �½�ũ�� �����Ѵ�.
				mov al, 0x20
				out 0x20, al
				sti

				iretd
			}
		}		
	}
	else//�̹� �������� �½�ũ�� ���
	{
#ifdef _ORANGE_DEBUG
		/*SkyConsole::Print("EDI : %x\n", pNextThread->m_regs.edi);
		SkyConsole::Print("ESI : %x\n", pNextThread->m_regs.esi);
		SkyConsole::Print("EBP : %x\n", pNextThread->m_regs.ebp);
		SkyConsole::Print("ESP : %x\n", pNextThread->m_regs.esp);
		SkyConsole::Print("EBX : %x\n", pNextThread->m_regs.ebx);
		SkyConsole::Print("EDX : %x\n", pNextThread->m_regs.edx);
		SkyConsole::Print("ECX : %x\n", pNextThread->m_regs.ecx);
		SkyConsole::Print("EAX : %x\n", pNextThread->m_regs.eax);

		SkyConsole::Print("EIP : %x\n", pNextThread->m_regs.eip);
		SkyConsole::Print("CS : %x\n", pNextThread->m_regs.cs);
		SkyConsole::Print("EFLAGS : %x\n", pNextThread->m_regs.eflags);

		SkyConsole::Print("ds : %x\n", pNextThread->m_regs.ds);
		SkyConsole::Print("gs : %x\n", pNextThread->m_regs.gs);
		SkyConsole::Print("es : %x\n", pNextThread->m_regs.es);
		SkyConsole::Print("fs : %x\n", pNextThread->m_regs.fs);*/
#endif		

		//�½�ũ�� ESP�� ������ ���丮 ���� g_esp, g_pageDirectory�� ����
		pNextThread->m_waitingTime = TASK_RUNNING_TIME;
		pNextThread->m_taskState = TASK_STATE_RUNNING;

		g_esp = pNextThread->m_esp;
		
		g_pageDirectory = (uint32_t)pNextThread->m_pParent->GetPageDirectory();

		VirtualMemoryManager::SetCurPageDirectory(pNextThread->m_pParent->GetPageDirectory());
		ProcessManager::GetInstance()->SetCurrentTask(pNextThread);

		//��������
		g_registers = pNextThread->m_contextSnapshot;		
	}

	return true;
}

bool Scheduler::Yield()
{
	Thread* pCurrentThread = ProcessManager::GetInstance()->GetCurrentTask();	

	kEnterCriticalSection();

	ProcessManager::TaskList* pTaskList = ProcessManager::GetInstance()->GetTaskList();
	ProcessManager::TaskList::iterator iter = pTaskList->begin();

	for (; iter != pTaskList->end(); iter++)
	{
		Thread* pThread = *iter;
		if (pThread->GetThreadId() == pCurrentThread->GetThreadId())
			pCurrentThread->m_waitingTime = 0;
	}

	kLeaveCriticalSection();

	return true;
	
}