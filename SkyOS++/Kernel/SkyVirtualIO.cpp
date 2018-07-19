#include "SkyVirtualIO.h"
#include "SkyStartOption.h"
#include "SkyGUISystem.h"

SkyVirtualIO::SkyVirtualIO()
{
}

SkyVirtualIO::~SkyVirtualIO()
{
}

bool SkyVirtualIO::PutKeyboardQueue(KEYDATA* pData)
{
#if SKY_CONSOLE_MODE == 0	
	SkyGUISystem::GetInstance()->PutKeyboardQueue(pData);
#endif
	return false;
}

bool SkyVirtualIO::PutMouseQueue(MOUSEDATA* pData)
{
#if SKY_CONSOLE_MODE == 0	
	SkyGUISystem::GetInstance()->PutMouseQueue(pData);
#endif

	return false;
}
