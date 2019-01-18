#include "SkyGUISystem.h"
#include "SkyModuleManager.h"
#include "SkyWindow.h"
//#include "HangulEngine.h"
#include "SkyStartOption.h"
#include "FileSystem/StorageManager.h"
#include "SystemAPI.h"

SkyGUISystem* SkyGUISystem::m_GUISystem = nullptr;

SkyGUISystem::SkyGUISystem()
{
	m_GUIEnable = false;
	m_pInputEngine = nullptr;
	m_pEngine = nullptr;
	m_pWindow = nullptr;
}

SkyGUISystem::~SkyGUISystem()
{
}

bool SkyGUISystem::InitializeWithConsole()
{
	//VirtualMemoryManager::MapDMAAddress(VirtualMemoryManager::GetCurPageDirectory(), 0xE0000000, 0xE0000000, 0xE0FF0000);
	//VirtualMemoryManager::MapDMAAddress(VirtualMemoryManager::GetCurPageDirectory(), 0xF0000000, 0xF0000000, 0xF0FF0000);
	
	m_videoRamInfo._pVideoRamPtr = (void*)0xF0000000;
	m_videoRamInfo._width = 1024;
	m_videoRamInfo._height = 768;
	m_videoRamInfo._bpp = 8;
	m_videoRamInfo._framebuffer_type = MULTIBOOT_FRAMEBUFFER_TYPE_INDEXED;

	m_pWindow = new SkyWindow<SKY_GUI_SYSTEM>();
	if (m_pWindow == nullptr)
		return false;

	m_pWindow->Initialize(m_videoRamInfo._pVideoRamPtr, m_videoRamInfo._width, m_videoRamInfo._height, m_videoRamInfo._bpp, m_videoRamInfo._framebuffer_type);
	m_GUIEnable = true;

	return true;
}

bool SkyGUISystem::Initialize()
{

	if (bootParams.framebuffer_addr != 0)
	{		
		m_videoRamInfo._pVideoRamPtr = (void*)bootParams.framebuffer_addr;
		m_videoRamInfo._width = bootParams.framebuffer_width;
		m_videoRamInfo._height = bootParams.framebuffer_height;
		m_videoRamInfo._bpp = bootParams.framebuffer_bpp;
		m_videoRamInfo._framebuffer_type = bootParams.framebuffer_type;

		m_GUIEnable = true;

		return true;
	}
	return false;
}

extern void SampleFillRect(ULONG* lfb, int x, int y, int w, int h, int col);
bool SkyGUISystem::InitGUI()
{
	if (m_GUIEnable == false)
		return false;

	m_pWindow = new SkyWindow<SKY_GUI_SYSTEM>();
	if (m_pWindow == nullptr)
		return false;
	
	return m_pWindow->Initialize(m_videoRamInfo._pVideoRamPtr, m_videoRamInfo._width, m_videoRamInfo._height, m_videoRamInfo._bpp, m_videoRamInfo._framebuffer_type);	
}

bool SkyGUISystem::LoadGUIModule()
{
	
	StorageManager::GetInstance()->SetCurrentFileSystemByID('L');

	//Load Hangul Engine
	void* hwnd = SkyModuleManager::GetInstance()->LoadModule("Hangul.dll");		
	PHangulInput HanguleInput = (PHangulInput)SkyModuleManager::GetInstance()->GetModuleFunction(hwnd, "GetHangulEngine");	

	SKY_ASSERT(HanguleInput != nullptr, "Hangul Module Load Fail!!");	

	m_pInputEngine = HanguleInput();
	
	hwnd = SkyModuleManager::GetInstance()->LoadModule("Multilingual.dll");
	PGetHangulEngine HangulEngine = (PGetHangulEngine)SkyModuleManager::GetInstance()->GetModuleFunction(hwnd, "GetHangulEngine");

	SKY_ASSERT(HangulEngine != nullptr, "Multilingual Module Load Fail!!");
	
	m_pEngine = HangulEngine();
	bool result = m_pEngine->Initialize();

	SKY_ASSERT(result != false, "Multilingual Module Initialize Fail!!");
	
	return true;
}

bool SkyGUISystem::Run()
{	
	if(m_pWindow)
		m_pWindow->Run();

	return true;
}

bool SkyGUISystem::Print(char* pMsg)
{
	bool result = false;
	if (m_pWindow)
		result = m_pWindow->Print(pMsg);

	return result;
}

bool SkyGUISystem::Clear()
{
	bool result = false;
	if (m_pWindow)
		result = m_pWindow->Clear();

	return result;
}

bool SkyGUISystem::PutKeyboardQueue(KEYDATA* pData)
{
	bool result = false;
	if (m_pWindow)
		result = m_pWindow->PutKeyboardQueue(pData);

	return result;
}

bool SkyGUISystem::PutMouseQueue(MOUSEDATA* pData)
{
	bool result = false;
	if (m_pWindow)
		result = m_pWindow->PutMouseQueue(pData);

	return result;
}