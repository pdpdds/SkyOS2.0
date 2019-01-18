#pragma once
#include "I_SkyInput.h"

class SkyVirtualInput : public I_SkyInput
{
public:
	SkyVirtualInput();
	~SkyVirtualInput();

	virtual bool PutKeyboardQueue(KEYDATA* pData) override;
	virtual bool PutMouseQueue(MOUSEDATA* pData) override;
	virtual bool Print(char* str) override;
};
