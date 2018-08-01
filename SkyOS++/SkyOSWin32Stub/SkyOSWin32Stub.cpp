// TestEngine.cpp : 콘솔 응용 프로그램에 대한 진입점을 정의합니다.
//

#include "stdafx.h"
#include <windows.h> 
#include <iostream>
#include <stdio.h> 
#include "SDL.h"
#include "SkyOSWin32Stub.h"
#include "SkyMockInterface.h"
#include "I_VirtualIO.h"
#include "SkyIOHandlerWin32.h"

WIN32_VIDEO g_win32Video;
CRITICAL_SECTION g_cs;

int screen_w;
int screen_h;
SDL_Surface *screen;
SDL_Window *pWindow;
SDL_Renderer *pRenderer;
SDL_Texture *pTexture;

#pragma comment(lib, "SDL2.LIB")
FILE _iob[] = { *stdin, *stdout, *stderr };
extern "C" FILE * __cdecl __iob_func(void) { return _iob; }

extern SKY_FILE_Interface g_FileInterface;
extern SKY_ALLOC_Interface g_allocInterface;
extern SKY_Print_Interface g_printInterface;
extern SKY_PROCESS_INTERFACE g_processInterface;

#define SKY_PHYSICAL_MEMORY_SIZE 100000000

WIN32_STUB* GetWin32Stub()
{
	InitializeCriticalSection(&g_cs);

	char* pPhysicalMemory = new char[SKY_PHYSICAL_MEMORY_SIZE];
	WIN32_STUB* pStub = new WIN32_STUB;
	pStub->_allocInterface = &g_allocInterface;
	pStub->_fileInterface = &g_FileInterface;
	pStub->_printInterface = &g_printInterface;
	pStub->_processInterface = &g_processInterface;
	pStub->_virtualAddress = (unsigned int)pPhysicalMemory;
	pStub->_virtualAddressSize = SKY_PHYSICAL_MEMORY_SIZE;
	return pStub;
}



extern "C" WIN32_VIDEO* InitWin32System(int width, int height, int bpp)
{
	//윈도우와 렌더러를 생성
	if (SDL_CreateWindowAndRenderer(width, height, 0, &pWindow, &pRenderer) < 0)
	{
		std::cout << "SDL_CreateWindowAndRenderer Error: " << SDL_GetError() << std::endl;
		return nullptr;
	}
	SDL_GetWindowSize(pWindow, &screen_w, &screen_h);
	screen = SDL_CreateRGBSurface(0, screen_w, screen_h, bpp,
		0,
		0,
		0,
		0);

	if (screen == 0)
	{
		std::cout << "SDL_CreateRGBSurface Error: " << SDL_GetError() << std::endl;
		return nullptr;
	}

	if (bpp == 32)
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
	else if(bpp == 8)
	{
		pTexture = SDL_CreateTexture(pRenderer,
			SDL_PIXELFORMAT_RGB332,
			SDL_TEXTUREACCESS_STREAMING,
			screen_w, screen_h);
	}

	if (pRenderer == 0)
	{
		std::cout << "SDL_CreateTexture Error: " << SDL_GetError() << std::endl;
		return nullptr;
	}

	if (pTexture == 0)
	{
		SDL_DestroyRenderer(pRenderer);
		SDL_DestroyWindow(pWindow);
		std::cout << "SDL_CreateTextureFromSurface Error: " << SDL_GetError() << std::endl;
		return 0;
	}


	g_win32Video._frameBuffer = (unsigned int)screen->pixels;
	g_win32Video._width = width;
	g_win32Video._height = height;
	g_win32Video._bpp = bpp;

	return &g_win32Video;
}

extern "C" void LoopWin32(I_VirtualIO* pVirtualIO, unsigned int& tickCount)
{
	bool running = true;
	
	SkyIOHandlerWin32* pInputHandler = new SkyIOHandlerWin32();
	pInputHandler->Initialize(pVirtualIO);

	//루프를 돌며 화면을 그린다.
	while (running)
	{
		tickCount = GetTickCount();
		//이벤트를 가져온다.
		SDL_Event event;
		while (SDL_PollEvent(&event))
		{
			if (event.type == SDL_MOUSEMOTION)
			{
				MOUSEDATA data;

				if (event.button.button == SDL_BUTTON_LEFT)
				{
					data.bButtonStatusAndFlag = MOUSE_LBUTTONDOWN;
				}
				else if (event.button.button == SDL_BUTTON_RIGHT)
				{
					data.bButtonStatusAndFlag = MOUSE_RBUTTONDOWN;
				}
				else if (event.button.button == SDL_BUTTON_RIGHT)
				{
					data.bButtonStatusAndFlag = MOUSE_MBUTTONDOWN;
				}
				else
				{
					data.bButtonStatusAndFlag = 0;
				}

				data.bXMovement = event.motion.x;
				data.bYMovement = event.motion.y;
				data.bAbsoluteCoordinate = 1;
				pVirtualIO->PutMouseQueue(&data);
			}
			else if (event.type == SDL_MOUSEBUTTONDOWN)
			{
				MOUSEDATA data;

				if (event.button.button == SDL_BUTTON_LEFT)
				{
					data.bButtonStatusAndFlag = MOUSE_LBUTTONDOWN;
				}
				else if (event.button.button == SDL_BUTTON_RIGHT)
				{
					data.bButtonStatusAndFlag = MOUSE_RBUTTONDOWN;
				}
				else if (event.button.button == SDL_BUTTON_RIGHT)
				{
					data.bButtonStatusAndFlag = MOUSE_MBUTTONDOWN;
				}
				else
				{
					data.bButtonStatusAndFlag = 0;
				}

				data.bXMovement = event.motion.x;
				data.bYMovement = event.motion.y;
				data.bAbsoluteCoordinate = 1;
				pVirtualIO->PutMouseQueue(&data);
			}
			else if (event.type == SDL_MOUSEBUTTONUP)
			{
				MOUSEDATA data;
				data.bButtonStatusAndFlag = 0;
				data.bXMovement = event.motion.x;
				data.bYMovement = event.motion.y;
				data.bAbsoluteCoordinate = 1;
				pVirtualIO->PutMouseQueue(&data);
			}
			//키보드 이벤트가 발생했다면
			else if (event.type == SDL_KEYDOWN)
			{       
				
				unsigned int keycode = SDL_GetKeyFromScancode(event.key.keysym.scancode);

				BYTE bScancode = pInputHandler->ConvertKeycodeToScancode(keycode);

				if (bScancode != 0)
					pInputHandler->ConvertScanCodeAndPutQueue(bScancode);

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

		SDL_RenderClear(pRenderer);
		
		SDL_UpdateTexture(pTexture, NULL, screen->pixels, screen->pitch);
		SDL_RenderCopy(pRenderer, pTexture, NULL, NULL);
		//렌더러의 내용을 화면에 뿌린다.
		SDL_RenderPresent(pRenderer);
	}
	//텍스처, 렌더러, 윈도우 객체를 제거하고 SDL을 종료한다.
	SDL_DestroyTexture(pTexture);
	SDL_DestroyRenderer(pRenderer);
	SDL_DestroyWindow(pWindow);
	SDL_Quit();

	delete pInputHandler;

	exit(0);
}

#include <iostream>
#include <fstream>
#include <string>

using namespace std;

static char * ReadAllBytes(const char * filename, int * read)
{
	ifstream ifs(filename, ios::binary | ios::ate);
	ifstream::pos_type pos = ifs.tellg();

	// What happens if the OS supports really big files.
	// It may be larger than 32 bits?
	// This will silently truncate the value/
	int length = pos;

	// Manuall memory management.
	// Not a good idea use a container/.
	char *pChars = new char[length];
	ifs.seekg(0, ios::beg);
	ifs.read(pChars, length);

	// No need to manually close.
	// When the stream goes out of scope it will close the file
	// automatically. Unless you are checking the close for errors
	// let the destructor do it.
	ifs.close();
	*read = length;
	return pChars;
}

#include <stdio.h>
#include <io.h>
#include <conio.h>
SKYOS_MODULE_LIST g_module_list;
extern "C" SKYOS_MODULE_LIST* InitSkyOSModule()
{
	memset(&g_module_list, 0, sizeof(SKYOS_MODULE_LIST));

	_finddata_t fd;
	long handle;
	int result = 1;
	handle = _findfirst(".\\*.*", &fd);  //현재 폴더 내 모든 파일을 찾는다.

	if (handle == -1)
	{		
		return &g_module_list;
	}

	while (result != -1)
	{
		printf("File: %s\n", fd.name);

		if (fd.size > 0)
		{			
			g_module_list._moduleCount++;
		}

		result = _findnext(handle, &fd);
	}

	_findclose(handle);

	if(g_module_list._moduleCount == 0)
		return &g_module_list;

	g_module_list._module = new SKYOS_MODULE[g_module_list._moduleCount];

	handle = _findfirst(".\\*.*", &fd);  //현재 폴더 내 모든 파일을 찾는다.
	result = 1;

	int index = 0;
	while (result != -1)
	{				
		if(fd.size > 0)
		{
			int readCount = 0;
			string fileName;
			fileName += fd.name;
			char* pBuffer = ReadAllBytes(fileName.c_str(), &readCount);

			strcpy(g_module_list._module[index]._name, fd.name);
			g_module_list._module[index]._startAddress = (unsigned int)pBuffer;
			g_module_list._module[index]._endAddress = (unsigned int)pBuffer + readCount;
			index++;

		}
		
		result = _findnext(handle, &fd);
	}

	_findclose(handle);

	return &g_module_list;
}

extern "C" bool SKY_VirtualProtect(void* address, int size, int attribute, unsigned int* dwOld)
{
	return VirtualProtect(address, size, PAGE_EXECUTE_READWRITE, (DWORD*)dwOld);	
}

extern "C" bool SKY_VirtualFree(void* lpAddress, unsigned int dwSize, unsigned int  dwFreeType)
{
	return VirtualFree(lpAddress, dwSize, dwFreeType);
}

extern "C" void* SKY_VirtualAlloc(void* lpAddress, unsigned int dwSize, unsigned int  flAllocationType, unsigned int  flProtect)
{
	return VirtualAlloc(lpAddress, dwSize, flAllocationType, flProtect);
}