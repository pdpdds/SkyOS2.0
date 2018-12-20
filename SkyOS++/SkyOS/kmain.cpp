#include "kmain.h"
#include "BasicStruct.h"
#include "Constants.h"
#include "PlatformAPI.h"
#include "MultbootUtil.h"
#include "SkyOSCore.h"
#include "BootParams.h"
#include "SystemAPI.h"
#include "SkyFacade.h"
#include "syscall.h"
#include "KernelProcedure.h"
#include "SkyStartOption.h"
#include "SkyVirtualInput.h"
#include "SkyGUISystem.h"

#include "SkyOSWin32Stub.h"
extern unsigned int g_tickCount;
extern int wsmain();

void kmain(unsigned long magic, unsigned long addr, uint32_t imageBase)
{	
	bool bGraphicMode = false;
#if SKY_CONSOLE_MODE == 0
	bGraphicMode = true;
#endif

	InitOSSystem(magic, addr, imageBase, bGraphicMode, SKY_WIDTH, SKY_HEIGHT, SKY_BPP);
	
	//SystemAPITest();
	//RMETest();

	if (bGraphicMode)
	{
		SkyGUISystem::GetInstance()->Initialize();
		SkyGUISystem::GetInstance()->LoadGUIModule();
		SkyGUISystem::GetInstance()->InitGUI();

		//execThread("Idle", (thread_start_t)SystemIdle, 0, 1);
		//execThread("GUI", (thread_start_t)SystemGUIProc, 0, 1);		

		execThread("GUI", (thread_start_t)wsmain, 0, 1);
	}
		
	else
	{
		execThread("Idle", (thread_start_t)SystemIdle, 0, 1);
		execThread("Console", (thread_start_t)SystemConsoleProc, 0, 1);
	}

	LoopWin32(new SkyVirtualInput(), g_tickCount);	
	for (;;);


	/*exec("/dev/sound/beep");
	exec("/boot/net_server");
	*/	
}


void SystemAPITest()
{
	LONG a = 5; LONG b = 4;
	//InterlockedAdd(&a, b);
	//InterlockedAnd(&a, b);
	//InterlockedOr(&a, b);
	//InterlockedCompareExchange(&a, a, 7);
	//b = InterlockedIncrement(&a);
	//b = InterlockedDecrement(&a);
	InterlockedXor(&a, b);
}