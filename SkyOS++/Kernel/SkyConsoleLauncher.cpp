#include "SkyConsoleLauncher.h"
#include "SkyConsole.h"
#include "ProcessManager.h"
#include "SkyAPI.h"
#include "KernelProcedure.h"
#include "defines.h"
#include "Exception.h"
#include "Process.h"
#include "Thread.h"
#include "KeyboardController.h"

SkyConsoleLauncher::SkyConsoleLauncher()
{
}


SkyConsoleLauncher::~SkyConsoleLauncher()
{
}

void SkyConsoleLauncher::Launch()
{
//�ֿܼ� Ű���� �ʱ�ȭ
#ifdef SKY_EMULATOR
#else
	KeyboardController::SetupInterrupts();
	SkyConsole::Print("Keyboard Init..\n");
#endif

	Process* pMainProcess = ProcessManager::GetInstance()->CreateProcessFromMemory("ConsoleSystem", SystemConsoleProc, NULL, PROCESS_KERNEL);
	ProcessManager::GetInstance()->CreateProcessFromMemory("WatchDog", WatchDogProc, NULL, PROCESS_KERNEL);
	ProcessManager::GetInstance()->CreateProcessFromMemory("ProcessRemover", ProcessRemoverProc, NULL, PROCESS_KERNEL);

	SkyConsole::Print("Init Console....\n");

	Thread* pThread = pMainProcess->GetMainThread();

	Jump(pThread);
}
