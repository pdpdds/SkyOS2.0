#include "GUIEngine.h"
#include "windef.h"
#include "string.h"
#include "memory.h"

extern "C"
{
	//�� �Լ��� �籸���ؾ� �ϴµ� �ڷᰡ ����...
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