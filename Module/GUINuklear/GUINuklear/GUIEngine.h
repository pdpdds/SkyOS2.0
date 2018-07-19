#pragma once
#include "I_GUIEngine.h"

class GUIEngine : public I_GUIEngine
{
public:
	GUIEngine();
	virtual ~GUIEngine();

	virtual bool Initialize() override;
	virtual void Update(float deltaTime) override;
	virtual void SetLinearBuffer(unsigned long* pBuffer, unsigned long width, unsigned long height, unsigned long depth, unsigned type) override;
	
	virtual bool PutKeyboardQueue(KEYDATA* pData) override;
	virtual bool PutMouseQueue(MOUSEDATA* pData) override;
};

