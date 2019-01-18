#include "SkyVirtualInput.h"
#include "SkyStartOption.h"
#include "SkyGUISystem.h"

SkyVirtualInput::SkyVirtualInput()
{
}

SkyVirtualInput::~SkyVirtualInput()
{
}

bool SkyVirtualInput::PutKeyboardQueue(KEYDATA* pData)
{
#if SKY_CONSOLE_MODE == 0	
	SkyGUISystem::GetInstance()->PutKeyboardQueue(pData);
#endif
	return false;
}

bool SkyVirtualInput::Print(char* str)
{
	SkyGUISystem::GetInstance()->Print(str);
	return true;
}

bool SkyVirtualInput::PutMouseQueue(MOUSEDATA* pData)
{
#if SKY_CONSOLE_MODE == 0	
	SkyGUISystem::GetInstance()->PutMouseQueue(pData);
#endif

	return false;
}
