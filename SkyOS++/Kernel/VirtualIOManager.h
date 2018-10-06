#pragma once
#include "SkyQueue.h"
#include "I_GUIEngine.h"

class VirtualIOManager
{
public:
	VirtualIOManager();
	~VirtualIOManager();

	bool Initialize();
	bool GetKeyFromKeyQueue(KEYDATA* pstData);
	bool PutKeyQueue(KEYDATA* pstData);
//가상 IO에서 마우스 좌표는 무조건 절대값을 얻어낸다.
	bool GetMouseDataFromMouseQueue(BYTE& buttonStatus, int& xpos, int& ypos);
	bool PutMouseueue(MOUSEDATA* pstData);

private:
	SkyQueue* m_pKeyQueue;
	SkyQueue* m_pMouseQueue;
};

