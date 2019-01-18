#include "kmain.h"
#include "../RME/rme.h"
#include "../SkyCore/PCI/DeviceDriverManager.h"
#include "SkyConsole.h"

extern unsigned int g_tickCount;
extern int wsmain();
void TestSkySDL(int width, int height, int bpp);
void DevelopDeviceInterface();

#ifdef SKY_EMULATOR
DWORD WINAPI StartVirtualFramework(LPVOID parameter);
#endif

extern void SkyOSEntry(bool bGraphicMode);

void StartDaemon(void* param)
{
#ifdef SKY_EMULATOR	
	StartVirtualFramework(0);
#else
	StartNativeSystem(0);

#endif 
}

int TEST()
{
	printf("Kernel up and running\n");

	tRME_State *emu = RME_CreateState();

	{
		uint16_t *zeroptr = (uint16_t *)(void*)0;
		uint16_t *lowCache = (uint16_t *)new char[RME_BLOCK_SIZE];
		memcpy(lowCache, zeroptr, RME_BLOCK_SIZE);

		printf("%x %x\n", zeroptr[0], zeroptr[1]);
		printf("%x %x\n", lowCache[0], lowCache[1]);

		emu->Memory[0] = lowCache;	// The RME has NULL checks
		for (int i = 1; i < 0x100000 / RME_BLOCK_SIZE; i++)
			emu->Memory[i] = (void*)(i*RME_BLOCK_SIZE);
	}


	int ret = 0;
#if 1
	emu->AX.W = (0x00 << 8) | 0x11;	// Set Mode 0x11
	ret = RME_CallInt(emu, 0x10);
#endif

	// VESA
#if 0
	{
		struct VesaControllerInfo {
			char	Signature[4];	// == "VBE2"
			uint16_t	Version;	// == 0x0300 for Vesa 3.0
			t_farptr	OemString;	// isa vbeFarPtr
			uint8_t	Capabilities[4];
			t_farptr	Videomodes;	// isa vbeParPtr
			uint16_t	TotalMemory;// as # of 64KB blocks
		}	*info = (VesaControllerInfo*)0x10000;

		struct ModeInfo {
			uint16_t	attributes;
			uint8_t	winA, winB;
			uint16_t	granularity;
			uint16_t	winsize;
			uint16_t	segmentA, segmentB;
			t_farptr	realFctPtr;
			uint16_t	pitch; // bytes per scanline

			uint16_t	Xres, Yres;
			uint8_t	Wchar, Ychar, planes, bpp, banks;
			uint8_t	memory_model, bank_size, image_pages;
			uint8_t	reserved0;

			uint8_t	red_mask, red_position;
			uint8_t	green_mask, green_position;
			uint8_t	blue_mask, blue_position;
			uint8_t	rsv_mask, rsv_position;
			uint8_t	directcolor_attributes;

			uint32_t	physbase;  // your LFB address ;)
			uint32_t	reserved1;
			uint16_t	reserved2;
		}	*modeinfo = (ModeInfo*)0x9000;

		uint16_t	*modes;

		memcpy(info->Signature, "VBE2", 4);
		emu->AX.W = 0x4F00;
		emu->ES = 0x1000;
		emu->DI.W = 0;
		ret = RME_CallInt(emu, 0x10);
		printf("emu->AX = 0x%04x\n", emu->AX.W);
		printf("info->Videomodes = {Segment:0x%04x,Offset:0x%04x}\n",
			info->Videomodes.Segment, info->Videomodes.Offset);
		modes = (uint16_t*)((void*)((info->Videomodes.Segment * 16) + info->Videomodes.Offset));
		for (int i = 1; modes[i] != 0xFFFF; i++)
		{
			emu->AX.W = 0x4F01;
			emu->CX.W = modes[i];
			emu->ES = 0x0900;
			emu->DI.W = 0x0000;
			RME_CallInt(emu, 0x10);
			printf("modes[%i] = 0x%04x\n", i, modes[i]);
			printf("modeinfo = {\n");
			printf("  .attributes = 0x%04x\n", modeinfo->attributes);
			printf("  .pitch = 0x%04x\n", modeinfo->pitch);
			printf("  .Xres = %i\n", modeinfo->Xres);
			printf("  .Yres = %i\n", modeinfo->Yres);
			printf("  .bpp = %i\n", modeinfo->bpp);
			printf("  .physbase = 0x%08x\n", modeinfo->physbase);
			printf("}\n");

			/*
			printf("  .width = %i\n", modes[i].width);
			printf("  .height = %i\n", modes[i].height);
			printf("  .pitch = 0x%04x\n", modes[i].pitch);
			printf("  .bpp = %i\n", modes[i].bpp);
			printf("  .flags = 0x%04x\n", modes[i].flags);
			printf("  .fbSize = 0x%04x\n", modes[i].fbSize);
			printf("  .framebuffer = 0x%08x\n", modes[i].framebuffer);
			*/
			//break;
		}

		emu->AX.W = 0x4F02;
		emu->BX.W = 0x0115;	// Qemu 800x600x24
		RME_CallInt(emu, 0x10);
	}
#endif

#if 0
	emu->AX.W = (0x0B << 8) | 0x00;	// Set Border Colour
	emu->BX.W = (0x00 << 0) | 0x02;	// Colour 1
	ret = RME_CallInt(emu, 0x10);
#endif

#if 0
	emu->AX.W = (0x0F << 8) | 0;	// Function 0xF?
	ret = RME_CallInt(emu, 0x10);
#endif

	// Read Sector
#if 0
	emu->AX.W = 0x0201;	// Function 2, 1 sector
	emu->CX.W = 1;	// Cylinder 0, Sector 1
	emu->DX.W = 0x10;	// Head 0, HDD 1
	emu->ES = 0x1000;
	emu->BX.W = 0x0;
	ret = RME_CallInt(emu, 0x13);
	printf("\n%02x %02x",
		*(uint8_t*)(0x10000 + 510),
		*(uint8_t*)(0x10000 + 511)
	);
#endif

	//ret = RME_CallInt(emu, 0x11);	// Equipment Test

	switch (ret)
	{
	case RME_ERR_OK:
		printf("\n--- Emulator exited successfully!\n");
		printf("emu->AX = 0x%04x\n", emu->AX.W);
		break;
	case RME_ERR_INVAL:
		printf("\n--- ERROR: Invalid parameters\n");
		break;
	case RME_ERR_BADMEM:
		printf("\n--- ERROR: Emulator accessed bad memory\n");
		break;
	case RME_ERR_UNDEFOPCODE:
		printf("\n--- ERROR: Emulator hit an undefined opcode\n");
		break;
	case RME_ERR_DIVERR:
		printf("\n--- ERROR: Division Fault\n");
		break;
	default:
		printf("\n--- ERROR: Unknown error %i\n", ret);
		break;
	}

	return 0;
}


void kmain(unsigned long magic, unsigned long addr, uint32_t imageBase)
{
	bool bGraphicMode = false;
#if SKY_CONSOLE_MODE == 0
	bGraphicMode = true;
#endif

#ifdef SKY_EMULATOR	
	magic = 0;
	addr = 0;
	imageBase = 0x01600000;
#endif 

	InitOSSystem(magic, addr, imageBase, bGraphicMode, SKY_WIDTH, SKY_HEIGHT, SKY_BPP);

	SkyOSEntry(bGraphicMode);
}

void GUICallback(char* str)
{
	SkyGUISystem::GetInstance()->Print(str);
}

void SkyOSEntry(bool bGraphicMode)
{
#ifndef SKY_EMULATOR
	SkyInputHandler::GetInstance()->Initialize(nullptr);
#endif

	//SystemAPITest();
	//RMETest();

	if (bGraphicMode)
	{		
		SkyGUISystem::GetInstance()->Initialize();
#ifndef  SKY_EMULATOR
		SkyConsole::SetCallback(GUICallback);
#endif
		SkyGUISystem::GetInstance()->LoadGUIModule();
		SkyGUISystem::GetInstance()->InitGUI();

		ExecThread("Idle", (thread_start_t)SystemIdle, 0, 1);
		ExecThread("GUI", (thread_start_t)SystemGUIProc, 0, 1);
		ExecThread("GUIWatchDog", (thread_start_t)GUIWatchDogProc, 0, 1);
#ifdef  SKY_EMULATOR

		//ExecThread("GUI", (thread_start_t)wsmain, 0, 1);		
		//TestSkySDL(1024, 768, 32);
#else
		
#endif
	}
	else
	{
		ExecThread("Idle", (thread_start_t)SystemIdle, 0, 1);
		ExecThread("WatchDog", (thread_start_t)WatchDogProc, 0, 1);
		ExecThread("Console", (thread_start_t)SystemConsoleProc, 0, 1);
	}

	DevelopDeviceInterface();	

	printf("damon start!!\n");
	//mount("", "/dev/display/matrox", "devfs", 0, 0);
	//open("/dev/display/matrox", 0);
	StartDaemon(0);

	printf("reached end!!\n");
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

#include "SDL.H"
#include "I_ImageInterface.h"
#include "stl/OStream.h"
//extern "C" __declspec(dllimport) I_ImageInterface* GetImageInterface();


#define DUMMY_SURFACE   "_SDL_DummySurface"
extern BootParams bootParams;
void TestSkySDL(int width, int height, int bpp)
{
	int screen_w;
	int screen_h;
	SDL_Surface *screen;
	SDL_Window *pWindow;
	SDL_Renderer *pRenderer;
	//SDL_Texture *pTexture;
	//GetImageInterface();
	//윈도우와 렌더러를 생성

	if (SDL_CreateWindowAndRenderer(width, height, 0, &pWindow, &pRenderer) < 0)
	{
		//std::cout << "SDL_CreateWindowAndRenderer Error: " << SDL_GetError() << std::endl;
		return;
	}


	SDL_GetWindowSize(pWindow, &screen_w, &screen_h);
	//screen = (SDL_Surface *)SDL_GetWindowData(pWindow, DUMMY_SURFACE);
//	screen->pixels = (void*)bootParams.framebuffer_addr;

	/*screen = SDL_CreateRGBSurface(0, screen_w, screen_h, bpp,
		0,
		0,
		0,
		0);

	if (screen == 0)
	{
		std::cout << "SDL_CreateRGBSurface Error: " << SDL_GetError() << std::endl;
		return;
	}*/

	/*if (bpp == 32)
	{
		pTexture = SDL_CreateTexture(pRenderer,
			SDL_PIXELFORMAT_ARGB8888,
			SDL_TEXTUREACCESS_STREAMING,
			screen_w, screen_h);
	}
	else if (bpp == 24)
	{
		pTexture = SDL_CreateTexture(pRenderer,
			SDL_PIXELFORMAT_RGB888,
			SDL_TEXTUREACCESS_STREAMING,
			screen_w, screen_h);
	}
	else if (bpp == 16)
	{
		pTexture = SDL_CreateTexture(pRenderer,
			SDL_PIXELFORMAT_RGB565,
			SDL_TEXTUREACCESS_STREAMING,
			screen_w, screen_h);
	}
	else if (bpp == 8)
	{
		pTexture = SDL_CreateTexture(pRenderer,
			SDL_PIXELFORMAT_RGB332,
			SDL_TEXTUREACCESS_STREAMING,
			screen_w, screen_h);
	}

	if (pRenderer == 0)
	{
		std::cout << "SDL_CreateTexture Error: " << SDL_GetError() << std::endl;
		return;
	}

	if (pTexture == 0)
	{
		SDL_DestroyRenderer(pRenderer);
		SDL_DestroyWindow(pWindow);
		std::cout << "SDL_CreateTextureFromSurface Error: " << SDL_GetError() << std::endl;
		return;
	}*/

	SDL_Surface *pHellowBMP = SDL_LoadBMP("gui-chan.bmp");
	if (pHellowBMP == 0)
	{
		SDL_DestroyRenderer(pRenderer);
		SDL_DestroyWindow(pWindow);
		std::cout << "SDL_LoadBMP Error: " << SDL_GetError() << std::endl;
		return;
	}

	SDL_Texture *pTexture = SDL_CreateTextureFromSurface(pRenderer, pHellowBMP);
	// SDL_SaveBMP(pHellowBMP, "1.bmp");
	bool running = true;

	while (running)
	{
		SDL_Event event;
		while (SDL_PollEvent(&event))
		{
			if (event.type == SDL_KEYDOWN)
			{
				if (event.key.keysym.sym == SDLK_ESCAPE)
				{
					running = false;
				}
			}
			else if (event.type == SDL_QUIT)
			{
				running = false;
			}
		}
		
		SDL_SetRenderDrawColor(pRenderer, 255, 0, 0, 255);
		SDL_RenderClear(pRenderer);		
		SDL_RenderFillRect(pRenderer, NULL);
		//SDL_UpdateTexture(pTexture, NULL, screen->pixels, screen->pitch);
		SDL_RenderCopy(pRenderer, pTexture, NULL, NULL);			
		SDL_RenderPresent(pRenderer);
	}

	SDL_DestroyTexture(pTexture);
	SDL_DestroyRenderer(pRenderer);
	SDL_DestroyWindow(pWindow);
	SDL_Quit();
}

#ifdef SKY_EMULATOR
DWORD WINAPI StartVirtualFramework(LPVOID parameter)
{
	LoopWin32(new SkyVirtualInput(), g_tickCount);
	return 0;
}
#endif

#include "MP3\openmp3.h"
#include "stl/vector.h"
#include "Heap/HeapManager.h"

void LoadMP3(const void * mp3_file, unsigned file_size)
{
	OpenMP3::Library openmp3;

	OpenMP3::Iterator itr(openmp3, (OpenMP3::UInt8*)mp3_file, file_size);


	float buffer[2][1152];

	vector <float> channels[2];

	bool mono = true;


	OpenMP3::Decoder decoder(openmp3);

	OpenMP3::Frame frame;

	int size = 0;
	int count = 0;

	while (itr.GetNext(frame))
	{
		
		OpenMP3::UInt nsamples = decoder.ProcessFrame(frame, buffer);

		for (int ch = 0; ch < 1; ++ch)
		{
			auto & channel = channels[ch];

			auto * data = buffer[ch];

			for (OpenMP3::UInt idx = 0; idx < nsamples; ++idx) 
				channel.push_back(*data++);
		}
		size += channels[0].size();
		mono = mono && (frame.GetMode() == OpenMP3::kModeMono);
		count++;
	}
	printf("Heap %d\n", HeapManager::GetUsedHeapSize());
	OpenMP3::UInt length = size;
}


#include "../SkyCore/PCI/DeviceES1730.h"
#include "../SkyCore/PCI/SkyPCI.h"
void DevelopDeviceInterface()
{
	vector<float> vec;

	FILE* fp = fopen("test.mp3", "rb");

	if (fp)
	{
		fseek(fp, 0, SEEK_END);
		off_t fileSize = ftell(fp);
		fseek(fp, 0, SEEK_SET);
		char* buffer = new char[fileSize];
		fread(buffer, fileSize, 1, fp);
		fclose(fp);
		delete buffer;

		//LoadMP3(buffer, fileSize);	
	}
	
	//TEST();
#ifndef SKY_EMULATOR
	DeviceDriverManager::GetInstance()->InitPCIDevices();
	DeviceDriverManager::GetInstance()->RequestPCIList();

	DeviceES1730* pSound = new DeviceES1730();
	bool result = pSound->InitDriver();

	
	//ScanPCIDevices();

	//printf("Device ES1370 Init %d\n", result);
#endif // !SKY_EMULATOR
}
