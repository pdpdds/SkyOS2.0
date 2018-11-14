#include "SkyOS.h"
#include "SkyRenderer.h"
#include "nic.h"
#include "SkyGUISystem.h"
#include "SkyTest.h"
#include "SkyDebugger.h"

bool systemOn = false;

void NativeConsole()
{
	ConsoleManager manager;

	char	commandBuffer[MAXPATH];

	while (1)
	{
		int driveId = StorageManager::GetInstance()->GetCurrentDriveId();
		char driveLetter = 'a' + driveId;
		std::string driveName;
		driveName += toupper(driveLetter);
		driveName += ":> ";

		SkyConsole::Print("%s", (char*)driveName.c_str());		
		memset(commandBuffer, 0, MAXPATH);

		SkyConsole::GetCommand(commandBuffer, MAXPATH - 2);
		SkyConsole::Print("\n");

		if (manager.RunCommand(commandBuffer) == true)
			break;
	}
}

DWORD WINAPI SystemIdle(LPVOID parameter)
{
	while (1)
	{
		ksleep(10);
	}

	return 0;
}

DWORD WINAPI SystemConsoleProc(LPVOID parameter)
{
	SkyConsole::Print("Console Mode Start!!\n");
	multiboot_info* pBootInfo = SkyModuleManager::GetInstance()->GetMultiBootInfo();
	
	systemOn = true;

#ifdef SKY_EMULATOR

#else		
	StartPITCounter(100, I86_PIT_OCW_COUNTER_0, I86_PIT_OCW_MODE_SQUAREWAVEGEN);
#endif // SKY_EMULATOR	

	StorageManager::GetInstance()->SetCurrentFileSystemByID('L');
	StorageManager::GetInstance()->Initilaize(pBootInfo);
	SkyDebugger::GetInstance()->LoadSymbol("DebugEngine.dll");

#ifdef SKY_EMULATOR
	SkyModuleManager::GetInstance()->LoadModule("Lua5.dll");
	SkyModuleManager::GetInstance()->LoadModule("SkySDL.dll");
	SkyModuleManager::GetInstance()->LoadModule("freetyped.dll");
#else
	SkyModuleManager::GetInstance()->LoadImplictDLL(0x001600000);
#endif
	TestSkySDL(1024, 768, 32);

	NativeConsole();

	SkyConsole::Print("Bye!!");

	return 0;
}
extern void SampleFillRect(ULONG* lfb0, int x, int y, int w, int h, int col);

DWORD WINAPI SystemGUIProc(LPVOID parameter)
{
	unsigned int* ebp = (unsigned int*)&parameter - 1;
	SkyConsole::Print("start ebp : %x\n", *ebp);	
	SkyConsole::Print("parameter : %x\n", parameter);

	multiboot_info* pBootInfo = SkyModuleManager::GetInstance()->GetMultiBootInfo();

	kEnterCriticalSection();
#ifdef SKY_EMULATOR
#else	
	StartPITCounter(1000, I86_PIT_OCW_COUNTER_0, I86_PIT_OCW_MODE_SQUAREWAVEGEN);	
#endif
	
	StorageManager::GetInstance()->Initilaize(pBootInfo);
	StorageManager::GetInstance()->SetCurrentFileSystemByID('L');

	SkyDebugger::GetInstance()->LoadSymbol("DebugEngine.dll");
	
	SkyGUISystem::GetInstance()->InitGUIModule();

#ifdef SKY_EMULATOR
	SkyModuleManager::GetInstance()->LoadModule("Lua5.dll");
	SkyModuleManager::GetInstance()->LoadModule("SkySDL.dll");
	SkyModuleManager::GetInstance()->LoadModule("freetyped.dll");
	TestSkySDL(1024,768,32);
	//TestFreeType("Gabriola.ttf", "hello world");
#else
	SkyModuleManager::GetInstance()->LoadImplictDLL(0x001600000);
#endif

	//SampleFillRect((ULONG*)SkyGUISystem::GetInstance()->GetVideoRamInfo()._pVideoRamPtr, 1004, 0, 20, 20, 0x0000FF00);

	systemOn = true;
	kLeaveCriticalSection();

	SkyGUISystem::GetInstance()->Run();

	return 0;
}

#define TS_WATCHDOG_CLOCK_POS		(0xb8000+(80-1)*2)
#define TIMEOUT_PER_SECOND		1000
static bool m_bShowTSWatchdogClock = true;

DWORD WINAPI WatchDogProc(LPVOID parameter)
{
	Process* pProcess = (Process*)parameter;
	int pos = 0;
	char *addr = (char *)TS_WATCHDOG_CLOCK_POS, status[] = { '-', '\\', '|', '/', '-', '\\', '|', '/' };
	int first = GetTickCount();

	//SkyConsole::Print("\nWatchDogProc Start. Thread Id : %d\n", kGetCurrentThreadId());

	while (1)
	{

		int second = GetTickCount();
		if (second - first >= TIMEOUT_PER_SECOND)
		{
			if (++pos > 7)
				pos = 0;

			if (m_bShowTSWatchdogClock)
				*addr = status[pos];

			first = GetTickCount();
		}
#ifdef SKY_EMULATOR
#else
		kEnterCriticalSection();
		Scheduler::GetInstance()->Yield();
		kLeaveCriticalSection();
#endif // SKY_EMULATOR
	}

	return 0;
}

void SampleFillRect(ULONG* lfb0, int x, int y, int w, int h, int col)
{
	for (int k = 0; k < h; k++)
		for (int j = 0; j < w; j++)
		{
			int index = ((j + x) + (k + y) * 1024);
			lfb0[index] = col;
		}
}

void WatchDogLoop(Process* pProcess)
{
	int pos = 0;

	int colorStatus[] = { 0x00FF0000, 0x0000FF00, 0x0000FF };
	int first = GetTickCount();

	//그래픽 버퍼 주소를 얻는다.
	ULONG* lfb = (ULONG*)SkyGUISystem::GetInstance()->GetVideoRamInfo()._pVideoRamPtr;

	while (1)
	{
		int second = GetTickCount();
		//1초 단위로 색상을 변경한다.

		if (second - first >= TIMEOUT_PER_SECOND)
		{
			if (++pos > 2)
				pos = 0;

			if (m_bShowTSWatchdogClock)
			{
				SampleFillRect(lfb, 1004, 0, 20, 20, colorStatus[pos]);
			}

			first = GetTickCount();
		}

		//빠르게 실행될 필요가 없으므로 실행시간을 타 프로세스에 양보한다.
		Scheduler::GetInstance()->Yield();
	}
}

DWORD WINAPI GUIWatchDogProc(LPVOID parameter)
{
	Process* pProcess = (Process*)parameter;	

	//루프를 돌면서 오른쪽 상단에 사각형을 그린다.
	WatchDogLoop(pProcess);

	return 0;
}

void LoopProcessRemove(Process* pProcess)
{
	int static id = 0;
	int temp = id++;
	int first = GetTickCount();

	while (1)
	{

		//페이징 기능 전환 테스트		
		kEnterCriticalSection();
		//PhysicalMemoryManager::EnablePaging(false);
		//PhysicalMemoryManager::EnablePaging(true);

		ProcessManager::GetInstance()->RemoveTerminatedProcess();

		//Scheduler::GetInstance()->Yield();
		kLeaveCriticalSection();

		int second = GetTickCount();
		if (second - first >= 400)
		{
			first = GetTickCount();
		}
	}
}

DWORD WINAPI ProcessRemoverProc(LPVOID parameter)
{
	Process* pProcess = (Process*)parameter;

	LoopProcessRemove(pProcess);

	return 0;
}