#include "GUIEngine.h"
#include "windef.h"
#include "string.h"
#include "memory.h"

extern "C"
{
	//이 함수를 재구성해야 하는데 자료가 없음...
	void _ftoui3()
	{
	
	}
};


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
	
}

void GUIEngine::SetLinearBuffer(unsigned long* pBuffer, unsigned long width, unsigned long height, unsigned long depth, unsigned type)
{
	
}


bool GUIEngine::PutKeyboardQueue(KEYDATA* pData)
{
	return false;
}

bool  GUIEngine::PutMouseQueue(MOUSEDATA* pData)
{
	return false;
}