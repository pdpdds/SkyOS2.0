#include "SkyLauncher.h"
#include "Thread.h"
#include "SkyConsole.h"
#include "Exception.h"
#include "ProcessManager.h"
#include "SkyAPI.h"
#include "PlatformAPI.h"

SkyLauncher::SkyLauncher()
{
}


SkyLauncher::~SkyLauncher()
{
}

void SkyLauncher::Jump(Thread* pThread)
{
	if (pThread == nullptr)
		HaltSystem("Main Thread is Null!!");

	pThread->m_taskState = TASK_STATE_RUNNING;

	kLeaveCriticalSection();

	int entryPoint = (int)pThread->frame.eip;
	unsigned int procStack = pThread->frame.esp;

	SkyConsole::Print("Sky System : Entry Point : (0x%x)\n", entryPoint);
	SkyConsole::Print("Sky System : MainThread Stack : (0x%x)\n", procStack);

	ProcessManager::GetInstance()->SetCurrentTask(pThread);

	JumpToNewKernelEntry(entryPoint, procStack);
}

#ifdef SKY_EMULATOR
void SkyLauncher::JumpToNewKernelEntry(int entryPoint, unsigned int procStack)
{
	platformAPI._processInterface.sky_kcreate_thread_from_memory(1, (LPTHREAD_START_ROUTINE)entryPoint, 0);
}

#else
void SkyLauncher::JumpToNewKernelEntry(int entryPoint, unsigned int procStack)
{
	__asm
	{
		MOV     AX, 0x10;
		MOV     DS, AX
		MOV     ES, AX
		MOV     FS, AX
		MOV     GS, AX

		MOV     ESP, procStack
		PUSH	0; //�Ķ����
		PUSH	0; //EBP
		PUSH    0x200; EFLAGS
		PUSH    0x08; CS
		PUSH    entryPoint; EIP		
		IRETD
	}
}
#endif
