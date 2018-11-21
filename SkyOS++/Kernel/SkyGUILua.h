#pragma once
#include "windef.h"
#include "I_GUIEngine.h"

class SkyInputManager;

class SkyGUILua
{
public:
	SkyGUILua();
	~SkyGUILua();

	bool Initialize(void* pVideoRamPtr, int width, int height, int bpp, uint8_t buffertype);
	bool Run();
	bool Print(char* pMsg);
	bool PrintUnicode(char* pMsg);
	bool PrintCommand(char* pMsg, bool backspace);

	bool PutKeyboardQueue(KEYDATA* pData);
	bool PutMouseQueue(MOUSEDATA* pData);

	void GetCommandForGUI(char* commandBuffer, int bufSize);

	VOID GetNewLine();

	bool Clear();
	void FillRect(int x, int y, int w, int h, int col);

	void Update(unsigned long *buf);	
	void PutPixel(ULONG x, ULONG y, ULONG col);
	void PutPixel(ULONG i, ULONG col);
	void PutPixel(ULONG i, unsigned char r, unsigned char g, unsigned char b);

	ULONG GetPixel(ULONG i);
	ULONG GetBPP();
	
protected:
	void PutCursor();

private:
	static ULONG* m_pVideoRamPtr;
	int m_width;
	int m_height;
	int m_bpp;

	SkyRenderer* m_pRenderer;
	int m_yPos;
	int m_xPos;
	int m_lastCommandLength;

	SkyInputManager* m_pSkyInputManager;
};