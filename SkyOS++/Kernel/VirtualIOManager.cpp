#include "VirtualIOManager.h"
#include "SkyAPI.h"

KEYDATA g_keyQueueBuffer[KEY_MAXQUEUECOUNT];
MOUSEDATA g_mouseQueueBuffer[MOUSE_MAXQUEUECOUNT];

VirtualIOManager::VirtualIOManager()
{
}

VirtualIOManager::~VirtualIOManager()
{
}

bool VirtualIOManager::Initialize()
{
	m_pKeyQueue = new SkyQueue();
	m_pKeyQueue->Initialize(g_keyQueueBuffer, KEY_MAXQUEUECOUNT, sizeof(KEYDATA));

	m_pMouseQueue = new SkyQueue();
	m_pMouseQueue->Initialize(g_mouseQueueBuffer, MOUSE_MAXQUEUECOUNT, sizeof(MOUSEDATA));

	return true;
}

bool VirtualIOManager::GetKeyFromKeyQueue(KEYDATA* pstData)
{
	bool bResult;
	
	kEnterCriticalSection();
	bResult = m_pKeyQueue->GetQueue(pstData);
	kLeaveCriticalSection();
	
	return bResult;
}

bool VirtualIOManager::PutKeyQueue(KEYDATA* pstData)
{
	bool bResult;

	kEnterCriticalSection();
	bResult = m_pKeyQueue->PutQueue(pstData);
	kLeaveCriticalSection();

	return bResult;
}

bool VirtualIOManager::PutMouseueue(MOUSEDATA* pstData)
{
	bool bResult;

	kEnterCriticalSection();
	bResult = m_pMouseQueue->PutQueue(pstData);
	kLeaveCriticalSection();

	return bResult;
}

bool VirtualIOManager::GetMouseDataFromMouseQueue(BYTE& buttonStatus, int& xpos, int& ypos)
{
	MOUSEDATA stData;
	bool bResult;

	if (m_pMouseQueue->IsQueueEmpty() == true)
	{
		return false;
	}

	kEnterCriticalSection();
	bResult = m_pMouseQueue->GetQueue(&stData);
	kLeaveCriticalSection();

	if (bResult == false)
	{
		return false;
	}

	// 마우스 데이터 분석
	// 마우스 버튼 플래그는 첫 번째 바이트의 하위 3비트에 존재함
	buttonStatus = stData.bButtonStatusAndFlag & 0x7;


	xpos = stData.bXMovement;
	ypos = stData.bYMovement;
	return true;
}

