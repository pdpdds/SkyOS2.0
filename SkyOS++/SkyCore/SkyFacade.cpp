#include "SkyFacade.h"
#include "GDT.h"
#include "IDT.h"
#include "PIC.h"
#include "FPU.h"
#include "PIT.h"
#include "TSS.h"
#include "cpu_asm.h"
#include "interrupt.h"
#include "MultiBoot.h"
#include "SystemAPI.h"
#include "PhysicalMemoryManager.h"
#include "VirtualMemoryManager.h"
#include "BasicStruct.h"
#include "HeapManager.h"
#include "PlatformAPI.h"
#include "memory.h"
#include "MultbootUtil.h"
#include "string.h"
#include "SkyConsole.h"
#include "SkyOSCore.h"
#include "BootParams.h"
#include "StorageManager.h"
#include "SkyModuleManager.h"
#include "SkyDebugger.h"
#include "SystemProfiler.h"

#ifdef SKY_EMULATOR
#include "SkyOSWin32Stub.h"
unsigned int g_tickCount = 0;
#endif

extern "C" void InitializeConstructors();
bool InitCPU();
void SetInterruptVectors();
void InitInterrupt();
bool InitMemoryManager();
bool InitModules(multiboot_info* pInfo);
bool BuildPlatformAPI(unsigned long addr, uint32_t imageBase);
bool InitDisplaySystem();

void newEntry();

BootParams bootParams;
PlatformAPI platformAPI;
DWORD g_pte;

void JumpToNewKernelEntry(int entryPoint, unsigned int procStack)
{

#ifdef SKY_EMULATOR
	newEntry();
#else
	__asm
	{
		MOV     AX, 0x10;
		MOV     DS, AX
			MOV     ES, AX
			MOV     FS, AX
			MOV     GS, AX

			MOV     ESP, procStack
			PUSH	0; //파라메터
		PUSH	0; //EBP
		PUSH    0x200; EFLAGS
			PUSH    0x08; CS
			PUSH    entryPoint; EIP
			IRETD
	}
#endif
}

void newEntry()
{
	_BootParams params;
	params.memsize = bootParams._memorySize;
	params.SetAllocated(0, bootParams._memoryLayout._kernelDataTop);
	//params.SetAllocated(bootParams._memoryLayout._bootStackBase, bootParams._memoryLayout._kHeapTop);
	printf("kernel end %x\n", bootParams._memoryLayout._kernelDataTop);

	PageDirectory* curPageDirectory = VirtualMemoryManager::GetKernelPageDirectory();
	InitKernelSystem(&params, (unsigned int)&curPageDirectory->m_entries[0]);

	SkyModuleManager::GetInstance()->LoadImplictDLL(bootParams._memoryLayout._kernelBase);

	SkyModuleManager::GetInstance()->Initialize();
	SystemProfiler::GetInstance()->Initialize();
	StorageManager::GetInstance()->SetCurrentFileSystemByID('L');
	StorageManager::GetInstance()->Initilaize();

	SkyDebugger::GetInstance()->LoadSymbol("DebugEngine.dll");

	PrintCurrentTime();

	InitDisplaySystem();

#ifndef SKY_EMULATOR
	SkyInputHandler::GetInstance()->Initialize(nullptr);
#endif // !SKY_EMULATOR
}

bool InitOSSystem(unsigned long magic, unsigned long addr, uint32_t imageBase, bool bGraphicMode, int width, int height, int bpp)
{
#ifdef SKY_EMULATOR	
	magic = 0;
	addr = 0;
	imageBase = 0x01600000;	
#endif 

	InitializeConstructors();

	InitCPU();

	InitInterrupt();

	BuildPlatformAPI(addr, imageBase);

	SkyConsole::Initialize();
	SkyConsole::Print("*** Sky OS Console System Init ***\n");
	SkyConsole::Print("Boot Loader Name : %s\n", bootParams._szBootLoaderName);

	if (false == InitMemoryManager())
	{
		HaltSystem("Init Memory Error!!");
	}

	multiboot_info* pInfo = (multiboot_info*)addr;
	InitModules(pInfo);

	StartPITCounter(1000, I86_PIT_OCW_COUNTER_0, I86_PIT_OCW_MODE_SQUAREWAVEGEN);


	if (bGraphicMode == true)
	{
		bootParams.framebuffer_width = width;
		bootParams.framebuffer_height = height;
		bootParams.framebuffer_bpp = bpp;
	}

	JumpToNewKernelEntry((int)newEntry, bootParams._memoryLayout._bootPhysicalStackTop);

	return true;
}

bool InitCPU()
{
	InitGDT();

	InitIDT(0x8);
	InitTSS(5, 0x10, 0);

#ifndef SKY_EMULATOR
	InitPIC(0x20, 0x28);
	//InitPIT(); //OLD TIMER INTERRPUT HANDLER.....

	if (false == InitFPU())
		return false;

	EnableFPU();
#endif 

	return true;
}

void InitInterrupt()
{
	SetInterruptVectors();
	//InitializeSysCall();
}

extern "C" {
	void trap0(); void trap1(); void trap2(); void trap3(); void trap4();
	void trap5(); void trap6(); void trap7(); void trap8(); void trap9();
	void trap10(); void trap11(); void trap12(); void trap13(); void trap14();
	void trap16(); void trap17(); void trap18(); void trap32(); void trap33();
	void trap34(); void trap35(); void trap36(); void trap37(); void trap38();
	void trap39(); void trap40(); void trap41(); void trap42(); void trap43();
	void trap44(); void trap45(); void trap46(); void trap47(); void trap50();	
};

void SetInterruptVectors()
{
	SetInterruptVector(0, (void(__cdecl &)(void))trap0);
	SetInterruptVector(1, (void(__cdecl &)(void))trap1);
	SetInterruptVector(2, (void(__cdecl &)(void))trap2);
	SetInterruptVector(3, (void(__cdecl &)(void))trap3);
	SetInterruptVector(4, (void(__cdecl &)(void))trap4);
	SetInterruptVector(5, (void(__cdecl &)(void))trap5);
	SetInterruptVector(6, (void(__cdecl &)(void))trap6);
	SetInterruptVector(7, (void(__cdecl &)(void))trap7);
	SetInterruptVector(8, (void(__cdecl &)(void))trap8);
	SetInterruptVector(9, (void(__cdecl &)(void))trap9);
	SetInterruptVector(10, (void(__cdecl &)(void))trap10);
	SetInterruptVector(11, (void(__cdecl &)(void))trap11);
	SetInterruptVector(12, (void(__cdecl &)(void))trap12);
	SetInterruptVector(13, (void(__cdecl &)(void))trap13);
	SetInterruptVector(14, (void(__cdecl &)(void))trap14);
	SetInterruptVector(16, (void(__cdecl &)(void))trap16);
	SetInterruptVector(17, (void(__cdecl &)(void))trap17);
	SetInterruptVector(18, (void(__cdecl &)(void))trap18);

	SetInterruptVector(32, (void(__cdecl &)(void))trap32);
	SetInterruptVector(33, (void(__cdecl &)(void))trap33);
	SetInterruptVector(34, (void(__cdecl &)(void))trap34);
	SetInterruptVector(35, (void(__cdecl &)(void))trap35);
	SetInterruptVector(36, (void(__cdecl &)(void))trap36);
	SetInterruptVector(37, (void(__cdecl &)(void))trap37);
	SetInterruptVector(38, (void(__cdecl &)(void))trap38);
	SetInterruptVector(39, (void(__cdecl &)(void))trap39);
	SetInterruptVector(40, (void(__cdecl &)(void))trap40);
	SetInterruptVector(41, (void(__cdecl &)(void))trap41);
	SetInterruptVector(42, (void(__cdecl &)(void))trap42);
	SetInterruptVector(43, (void(__cdecl &)(void))trap43);
	SetInterruptVector(44, (void(__cdecl &)(void))trap44);
	SetInterruptVector(45, (void(__cdecl &)(void))trap45);
	SetInterruptVector(46, (void(__cdecl &)(void))trap46);
	SetInterruptVector(47, (void(__cdecl &)(void))trap47);
	SetInterruptVector(50, (void(__cdecl &)(void))trap50);

	/*SetInterruptVector(0, (void(__cdecl &)(void))kHandleDivideByZero);
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
	SetInterruptVector(38, (void(__cdecl &)(void))InterrputDefaultHandler);*/
}




void GetKernalDataEnd()
{
	DWORD kernelEnd = 0;
	DWORD address = (DWORD)PhysicalMemoryManager::AllocBlock();
	bootParams._memoryLayout._kernelDataTop = address;
	PhysicalMemoryManager::FreeBlock((void*)address);
}

//물리/가상 메모리 매니저를 초기화한다.
bool InitMemoryManager()
{
	PhysicalMemoryManager::Initialize();
	VirtualMemoryManager::Initialize();

	HeapManager::Initialize();
	GetKernalDataEnd();

	return true;
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
	extern SKY_PROCESS_INTERFACE _processInterface;
	extern SKY_ALLOC_INTERFACE		_allocInterface;
	extern SKY_FILE_INTERFACE		_fileInterface;
	extern SKY_PRINT_INTERFACE		_printInterface;

	platformAPI._processInterface = _processInterface;
	platformAPI._fileInterface = _fileInterface;
	platformAPI._allocInterface = _allocInterface;
	platformAPI._printInterface = _printInterface;

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

bool InitDisplaySystem()
{
#ifdef SKY_EMULATOR
	WIN32_VIDEO* pVideoInfo = InitWin32System(bootParams.framebuffer_width, bootParams.framebuffer_height, bootParams.framebuffer_bpp);
	if (pVideoInfo == nullptr)
		return false;

	bootParams.framebuffer_addr = pVideoInfo->_frameBuffer;
	bootParams.framebuffer_bpp = pVideoInfo->_bpp;
	bootParams.framebuffer_width = pVideoInfo->_width;
	bootParams.framebuffer_height = pVideoInfo->_height;

#endif
	//SkyGUISystem::GetInstance()->Initialize();

	return true;
}
