#include "GUIEngine.h"
#include "windef.h"
#include "string.h"
#include "memory.h"
#include "WindowManagerTask.h"
#include "Queue.h"

extern void kStartWindowManager(void);
extern QUEUE gs_stKeyQueue;
extern QUEUE gs_stMouseQueue;

GUIEngine::GUIEngine()
{
	
}

GUIEngine::~GUIEngine()
{
}

bool GUIEngine::Initialize() 
{
	
	kStartWindowManager(&m_linearBufferInfo);

	return true;
}
void GUIEngine::Update(float deltaTime) 
{
	kUpdate(m_linearBufferInfo.isDirectVideoBuffer);
}

void GUIEngine::SetLinearBuffer(LinearBufferInfo& linearBufferInfo)
{
	m_linearBufferInfo = linearBufferInfo;
}

bool GUIEngine::PutKeyboardQueue(KEYDATA* pData)
{
	return kPutQueue(&gs_stKeyQueue, pData);
}

bool  GUIEngine::PutMouseQueue(MOUSEDATA* pData)
{
	return kPutQueue(&gs_stMouseQueue, pData);
}