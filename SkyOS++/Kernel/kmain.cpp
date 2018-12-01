#include "kmain.h"
#include "SkyGUILauncher.h"
#include "SkyConsoleLauncher.h"
#include "SkyTest.h"
#include "SkyVirtualInput.h"
#include "PCIManager.h"
#include "SkyDebugger.h"
#include "BasicStruct.h"
#include "Constants.h"
#include "PlatformAPI.h"
#include "MultbootUtil.h"

#ifdef SKY_EMULATOR
#include "SkyOSWin32Stub.h"
extern unsigned int _pitTicks;
#endif

BootParams bootParams;
PlatformAPI platformAPI;

#ifdef SKY_EMULATOR
void kmain()
#else
void kmain(unsigned long magic, unsigned long addr, uint32_t imageBase)
#endif
{
	InitializeConstructors();

	kEnterCriticalSection();	
			
	InitHardware();	

#ifdef SKY_EMULATOR
	multiboot_info* pInfo = 0;
	BuildPlatformAPI(0, 0x01600000);
#else
	multiboot_info* pInfo = (multiboot_info*)addr;
	BuildPlatformAPI(addr, imageBase);
#endif	

	if (false == InitMemoryManager())
	{
		HaltSystem("Init Memory Error!!");
	}	

	InitModules(pInfo);
	InitDisplaySystem();
	
	kLeaveCriticalSection();

	SkyModuleManager::GetInstance()->Initialize();
	SystemProfiler::GetInstance()->Initialize();
	StorageManager::GetInstance()->SetCurrentFileSystemByID('L');
	StorageManager::GetInstance()->Initilaize();
	SkyGUISystem::GetInstance()->LoadGUIModule();
	SkyGUISystem::GetInstance()->InitGUI();
	SkyDebugger::GetInstance()->LoadSymbol("DebugEngine.dll");
	SkyModuleManager::GetInstance()->LoadImplictDLL(bootParams._memoryLayout._kernelBase);	

	SkyLauncher* pSystemLauncher = nullptr;

#if SKY_CONSOLE_MODE == 0	
	pSystemLauncher = new SkyGUILauncher();
#else
	pSystemLauncher = new SkyConsoleLauncher();	
#endif
	
	PrintCurrentTime();
	pSystemLauncher->Launch();

#ifdef SKY_EMULATOR
#if SKY_CONSOLE_MODE == 0
	LoopWin32(new SkyVirtualInput(), _pitTicks);
#endif
#endif
	for (;;);
}

bool InitDisplaySystem()
{
#if SKY_CONSOLE_MODE == 0
#ifdef SKY_EMULATOR
	WIN32_VIDEO* pVideoInfo = InitWin32System(SKY_WIDTH, SKY_HEIGHT, SKY_BPP);
	if (pVideoInfo == nullptr)
		return false;

	bootParams.framebuffer_addr = pVideoInfo->_frameBuffer;
	bootParams.framebuffer_bpp = pVideoInfo->_bpp;
	bootParams.framebuffer_width = pVideoInfo->_width;
	bootParams.framebuffer_height = pVideoInfo->_height;

#endif
	SkyGUISystem::GetInstance()->Initialize();

#endif

	return true;
}

void InitHardware()
{
	SkyConsole::Initialize();
	SkyConsole::Print("*** Sky OS Console System Init ***\n");
	SkyConsole::Print("Boot Loader Name : %s\n", bootParams._szBootLoaderName);

	GDTInitialize();
	IDTInitialize(0x8);

#ifndef SKY_EMULATOR
	PICInitialize(0x20, 0x28);
	PITInitialize();
#endif 

	SkyConsole::Print("Hardware Init Complete\n");

	SetInterruptVector();
	InitializeSysCall();
	//현재 구조에서는 TSS가 필요없음
	//InstallTSS(5, 0x10, 0);

	SkyConsole::Print("Interrput Handler Init Complete\n");

#ifndef SKY_EMULATOR
	if (false == InitFPU())
	{
		SkyConsole::Print("[Warning] Floating Pointer Unit Detection Fail\n");
	}
	else
	{
		EnableFPU();
		SkyConsole::Print("FPU Init..\n");
	}
#endif
}

//물리/가상 메모리 매니저를 초기화한다.
bool InitMemoryManager()
{		
	EnablePaging(false);	
	PhysicalMemoryManager::Initialize();	
	VirtualMemoryManager::Initialize();	
	EnablePaging(true);		
	HeapManager::Initialize();
	
	return true;
}

void SetInterruptVector()
{
	SetInterruptVector(0, (void(__cdecl &)(void))kHandleDivideByZero);
	SetInterruptVector(1, (void(__cdecl &)(void))kHandleSingleStepTrap);
	SetInterruptVector(2, (void(__cdecl &)(void))kHandleNMITrap);
	SetInterruptVector(3, (void(__cdecl &)(void))kHandleBreakPointTrap);
	SetInterruptVector(4, (void(__cdecl &)(void))kHandleOverflowTrap);
	SetInterruptVector(5, (void(__cdecl &)(void))kHandleBoundsCheckFault);
	SetInterruptVector(6, (void(__cdecl &)(void))kHandleInvalidOpcodeFault);
	SetInterruptVector(7, (void(__cdecl &)(void))kHandleNoDeviceFault);
	SetInterruptVector(8, (void(__cdecl &)(void))kHandleDoubleFaultAbort);
	SetInterruptVector(10, (void(__cdecl &)(void))kHandleInvalidTSSFault);
	SetInterruptVector(11, (void(__cdecl &)(void))kHandleSegmentFault);
	SetInterruptVector(12, (void(__cdecl &)(void))kHandleStackFault);
	SetInterruptVector(13, (void(__cdecl &)(void))kHandleGeneralProtectionFault);
	SetInterruptVector(14, (void(__cdecl &)(void))kHandlePageFault);
	SetInterruptVector(16, (void(__cdecl &)(void))kHandlefpu_fault);
	SetInterruptVector(17, (void(__cdecl &)(void))kHandleAlignedCheckFault);
	SetInterruptVector(18, (void(__cdecl &)(void))kHandleMachineCheckAbort);
	SetInterruptVector(19, (void(__cdecl &)(void))kHandleSIMDFPUFault);

	SetInterruptVector(33, (void(__cdecl &)(void))InterrputDefaultHandler);
	SetInterruptVector(38, (void(__cdecl &)(void))InterrputDefaultHandler);
}

_declspec(naked) void multiboot_entry(void)
{
	__asm {
		align 4

		multiboot_header:
		//멀티부트 헤더 사이즈 : 0X30
		dd(MULTIBOOT_HEADER_MAGIC); magic number

#if SKY_CONSOLE_MODE == 0
		dd(MULTIBOOT_HEADER_FLAGS_GUI); flags
		dd(CHECKSUM_GUI); checksum
#else
		dd(MULTIBOOT_HEADER_FLAGS); flags
		dd(CHECKSUM); checksum
#endif		
		dd(HEADER_ADRESS); //헤더 주소 KERNEL_LOAD_ADDRESS+ALIGN(0x100400)
		dd(GRUB_KERNEL_LOAD_ADDRESS); //커널이 로드된 주소
		dd(00); //사용되지 않음
		dd(00); //사용되지 않음
		dd(HEADER_ADRESS + 0x30); //커널 시작 주소 : 멀티부트 헤더 주소 + 0x30, kernel_entry

		dd(SKY_CONSOLE_MODE);
		dd(SKY_WIDTH);
		dd(SKY_HEIGHT);
		dd(SKY_BPP)

	kernel_entry:
		MOV     ESP, 0x40000; //스택 설정

		PUSH    0; //플래그 레지스터 초기화
		POPF

		PUSH    GRUB_KERNEL_LOAD_ADDRESS;
		//GRUB에 의해 담겨 있는 정보값을 스택에 푸쉬한다.
		PUSH    EBX; //멀티부트 구조체 포인터
		PUSH    EAX; //매직 넘버

					 //위의 두 파라메터와 함께 kmain 함수를 호출한다.
		CALL    kmain; //C++ 메인 함수 호출

	//루프를 돈다. kmain이 리턴되지 않으면 아래 코드는 수행되지 않는다.
	halt:
		jmp halt;
	}
}

bool BuildPlatformAPI(unsigned long addr, uint32_t imageBase)
{
#ifdef SKY_EMULATOR	
	WIN32_STUB* pStub = GetWin32Stub();

	platformAPI._processInterface = *(SKY_PROCESS_INTERFACE*)pStub->_processInterface;
	platformAPI._fileInterface = *(SKY_FILE_INTERFACE*)pStub->_fileInterface;
	platformAPI._allocInterface = *(SKY_ALLOC_INTERFACE*)pStub->_allocInterface;
	platformAPI._printInterface = *(SKY_PRINT_INTERFACE*)pStub->_printInterface;

	bootParams._memoryLayout._kernelBase = imageBase;
	bootParams._memoryLayout._bootStackBase = 0x30000;
	bootParams._memoryLayout._bootStackTop = 0x40000;

	strcpy(bootParams._szBootLoaderName, "SkyOS Emulator");

	uint32_t startAddress = PAGE_ALIGN_UP((uint32_t)pStub->_virtualAddress);
	uint32_t endAddress = PAGE_ALIGN_DOWN((uint32_t)pStub->_virtualAddress + pStub->_virtualAddressSize);

	bootParams.SetAllocated(startAddress, endAddress, MEMORY_REGION_AVAILABLE);
	bootParams._memorySize = endAddress - startAddress;	
	
#else
	multiboot_info* pBootInfo = (multiboot_info*)addr;
	bootParams._memoryLayout._kernelBase = imageBase;
	bootParams._kernelSize = GetKernelSize(pBootInfo);	
	strcpy(bootParams._szBootLoaderName, pBootInfo->boot_loader_name);
	GetMemoryInfo(pBootInfo, &bootParams);	
#endif

	return true;
}

bool InitModules(multiboot_info* pInfo)
{
#ifdef SKY_EMULATOR
	SKYOS_MODULE_LIST* pModule = InitSkyOSModule();

	bootParams._moduleCount = pModule->_moduleCount;
	if (bootParams._moduleCount > 0)
	{
		bootParams.Modules = new BootModule[pModule->_moduleCount];

		for (int i = 0; i < pModule->_moduleCount; i++)
		{
			bootParams.Modules[i].ModuleStart = pModule->_module[i]._startAddress;
			bootParams.Modules[i].ModuleEnd = pModule->_module[i]._endAddress;
			bootParams.Modules[i].Name = pModule->_module[i]._name;
		}
	}
#else
	bootParams._moduleCount = pInfo->mods_count;
	if (bootParams._moduleCount > 0)
	{
		bootParams.Modules = new BootModule[pInfo->mods_count];

		for (int i = 0; i < pInfo->mods_count; i++)
		{
			bootParams.Modules[i].ModuleStart = pInfo->Modules[i].ModuleStart;
			bootParams.Modules[i].ModuleEnd = pInfo->Modules[i].ModuleEnd;
			bootParams.Modules[i].Name = pInfo->Modules[i].Name;
		}
	}
#endif
	return true;
}
