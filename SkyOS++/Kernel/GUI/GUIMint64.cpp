#include "SkyOS.h"
#include "GUIMint64.h"
#include "SkyInputHandler.h"

typedef I_GUIEngine* (*PGUIEngine)();

GUIMint64::GUIMint64()
{
	m_pVideoRamPtr = nullptr;
	m_pEngine = nullptr;
}


GUIMint64::~GUIMint64()
{
}

extern void SampleFillRect(ULONG* lfb, int x, int y, int w, int h, int col);
bool GUIMint64::Initialize(void* pVideoRamPtr, int width, int height, int bpp, uint8_t buffertype)
{
	m_pVideoRamPtr = (ULONG*)pVideoRamPtr;
	m_width = width;
	m_height = height;
	m_bpp = bpp;

	void* hwnd = SkyModuleManager::GetInstance()->LoadModule("SkyGUIMint64.dll");				
	PGUIEngine GUIEngine = (PGUIEngine)SkyModuleManager::GetInstance()->GetModuleFunction(hwnd, "GetGUIEngine");
	
	m_pEngine = GUIEngine();
	LinearBufferInfo info;
	info.pBuffer = (unsigned long*)m_pVideoRamPtr;
	info.width = m_width;
	info.height = m_height;
	info.depth = m_bpp;
	info.type = 0;
	info.isDirectVideoBuffer = true;
	
	m_pEngine->SetLinearBuffer(info);
	m_pEngine->Initialize();

#ifdef SKY_EMULATOR
#else
	SkyInputHandler::GetInstance();	
	SkyInputHandler::GetInstance()->Initialize(nullptr);
	
#endif
	return true;
}

bool GUIMint64::Run()
{
	SampleFillRect(m_pVideoRamPtr, 1004, 0, 20, 20, 0xff000000);	
	//SampleFillRect((ULONG *)m_pVideoRamPtr, 1004, 0, 20, 20, 0x00FF0000);

	int colorStatus[] = { 0x00FF0000, 0x0000FF00, 0x0000FF };
	int pos = 0;

	m_pEngine->Update(0);

	/*while (1)
	{
		if (++pos > 2)
			pos = 0; 

		SampleFillRect(m_pVideoRamPtr, 1004, 0, 20, 20, colorStatus[pos]);		
	}*/
	
	return true;
}

bool GUIMint64::Print(char* pMsg)
{
	if(m_pVideoRamPtr)
	SampleFillRect(m_pVideoRamPtr, 1004, 0, 20, 20, 0x00FF0000);
	return true;
}

bool GUIMint64::Clear()
{
	return true;
}


bool GUIMint64::PutKeyboardQueue(KEYDATA* pData)
{
	if(m_pEngine)
		m_pEngine->PutKeyboardQueue(pData);
	return true;
}

bool GUIMint64::PutMouseQueue(MOUSEDATA* pData)
{
	if (m_pEngine)
		m_pEngine->PutMouseQueue(pData);
	return true;
}