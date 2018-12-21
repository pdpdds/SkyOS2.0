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
void TestSkySDL(int width, int height, int bpp);
DWORD WINAPI StartVirtualFramework(LPVOID parameter);

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

		//execThread("GUI", (thread_start_t)wsmain, 0, 1);
		execThread("TestSkySDL", (thread_start_t)StartVirtualFramework, 0, 1);
		TestSkySDL(1024, 768, 32);
	}
		
	else
	{
		execThread("Idle", (thread_start_t)SystemIdle, 0, 1);
		execThread("Console", (thread_start_t)SystemConsoleProc, 0, 1);
	}

	for (;;);
	


	/*exec("/dev/sound/beep");
	exec("/boot/net_server");
	*/	
}

DWORD WINAPI StartVirtualFramework(LPVOID parameter)
{

	LoopWin32(new SkyVirtualInput(), g_tickCount);
	return 0;
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