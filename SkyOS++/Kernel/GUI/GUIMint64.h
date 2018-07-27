#pragma once
#include "stdint.h"
#include "windef.h"
#include "I_VirtualIO.h"

class I_GUIEngine;

class GUIMint64
{
public:
	GUIMint64();
	~GUIMint64();

	bool Initialize(void* pVideoRamPtr, int width, int height, int bpp, uint8_t buffertype);
	bool Run();
	bool Print(char* pMsg);
	bool Clear();

	bool PutKeyboardQueue(KEYDATA* pData);
	bool PutMouseQueue(MOUSEDATA* pData);

	ULONG* m_pVideoRamPtr;
	int m_width;
	int m_height;
	int m_bpp;

	I_GUIEngine* m_pEngine;
};

