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
//���� IO���� ���콺 ��ǥ�� ������ ���밪�� ����.
	bool GetMouseDataFromMouseQueue(BYTE& buttonStatus, int& xpos, int& ypos);
	bool PutMouseueue(MOUSEDATA* pstData);

private:
	SkyQueue* m_pKeyQueue;
	SkyQueue* m_pMouseQueue;
};

