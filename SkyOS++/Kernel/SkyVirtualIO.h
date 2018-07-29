#pragma once
#include "I_GUIEngine.h"

class SkyVirtualIO : public I_VirtualIO
{
public:
	SkyVirtualIO();
	~SkyVirtualIO();

	virtual bool PutKeyboardQueue(KEYDATA* pData) override;
	virtual bool PutMouseQueue(MOUSEDATA* pData) override;
};

class Test
{
public:
	Test(){}
	
};

