#include "GUIEngine.h"
#include "windef.h"
#include "string.h"
#include "memory.h"
#include "Queue.h"




GUIEngine::GUIEngine()
{
	
}


GUIEngine::~GUIEngine()
{
}

bool GUIEngine::Initialize() 
{

	return true;
}
void GUIEngine::Update(float deltaTime) 
{
	//wnd_update();
	
	//kUpdate(m_linearBufferInfo.isDirectVideoBuffer);
}

void GUIEngine::SetLinearBuffer(LinearBufferInfo& linearBufferInfo)
{
	m_linearBufferInfo = linearBufferInfo;
	//init_lfb(linearBufferInfo.pBuffer, linearBufferInfo.width, linearBufferInfo.height, linearBufferInfo.depth, linearBufferInfo.type);
}

extern QUEUE gs_stKeyQueue;
extern QUEUE gs_stMouseQueue;
bool GUIEngine::PutKeyboardQueue(KEYDATA* pData)
{
	return kPutQueue(&gs_stKeyQueue, pData);
}

bool  GUIEngine::PutMouseQueue(MOUSEDATA* pData)
{
	return kPutQueue(&gs_stMouseQueue, pData);
}


//int colorStatus[] = { 0x00FF0000, 0x0000FF00, 0x0000FF };
//ULONG* lfAb = (ULONG*)SkyGUISystem::GetInstance()->GetVideoRamInfo()._pVideoRamPtr;
//SampleFillRect(lfAb, 0, 0, 20, 20, 0x00ff0000);