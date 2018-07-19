#pragma once
#include "I_VirtualIO.h"

typedef struct tag_WIN32_STUB
{
	void* _fileInterface;
	void* _allocInterface;
	void* _printInterface;
	void* _processInterface;
	unsigned int _virtualAddress;
	unsigned int _virtualAddressSize;
}WIN32_STUB;

typedef struct tag_WIN32_VIDEO
{
	unsigned int _frameBuffer;
	unsigned int _width;
	unsigned int _height;
	unsigned int _bpp;
}WIN32_VIDEO;

typedef struct tag_SKYOS_MODULE
{
	char _name[256];
	unsigned int _startAddress;
	unsigned int _endAddress;	
}SKYOS_MODULE;

typedef struct tag_SKYOS_MODULE_LIST
{
	unsigned int _moduleCount;
	SKYOS_MODULE* _module;
}SKYOS_MODULE_LIST;

extern "C" __declspec(dllexport) WIN32_STUB* GetWin32Stub();
extern "C" __declspec(dllexport) tag_WIN32_VIDEO* InitWin32System(int width, int height, int bpp);
extern "C" __declspec(dllexport) void LoopWin32(I_VirtualIO* pVirtualIO);
extern "C" __declspec(dllexport) SKYOS_MODULE_LIST* InitSkyOSModule();

extern "C" __declspec(dllexport) bool SKY_VirtualProtect(void* address, int size, int attribute, unsigned int* dwOld);
extern "C" __declspec(dllexport) bool SKY_VirtualProtect(void* address, int size, int attribute, unsigned int* dwOld);
extern "C" __declspec(dllexport) bool SKY_VirtualFree(void* lpAddress, unsigned int dwSize, unsigned int  dwFreeType);
extern "C" __declspec(dllexport) void* SKY_VirtualAlloc(void* lpAddress, unsigned int dwSize, unsigned int  flAllocationType, unsigned int  flProtect);
