#pragma once
#include "stdint.h"
#include "svgagui.h"
#include "lua.h"
#include "lualib.h"
#include "luatinker.h"
#include "I_VirtualIO.h"

class VirtualIOManager;

class SkySVGA : public I_VirtualIO
{
public:
	SkySVGA();
	~SkySVGA();

	bool Initialize(void* pVideoRamPtr, int width, int height, int bpp, uint8_t buffertype);
	bool Run();
	bool Print(char* pMsg);
	bool Clear();

	bool PutKeyboardQueue(KEYDATA* pData);
	bool PutMouseQueue(MOUSEDATA* pData);

protected:
	int StartGui();

private:
	bool m_exit_program;
	GuiWinThread *m_pWinThread;
	VirtualIOManager* m_pVirtualIOManager;
	
};

