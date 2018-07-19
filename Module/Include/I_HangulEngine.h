#pragma once
#include "windef.h"

class I_HangulEngine
{
public:
	virtual bool SwitchMode() = 0; //�ѿ���ȯ
	virtual void Reset() = 0; //�� ���ڿ��� ���� �غ� �Ѵ�.
	virtual bool InputAscii(unsigned char letter) = 0; //�ƽ�Ű ���ڸ� ����ִ´�.
	virtual int GetString(char* buffer) = 0; //���� ���ڿ��� ��´�.
	virtual int DrawText(int iX1, int iY1, int iX2, int iY2, DWORD* pstMemoryAddress, int iX, int iY,
		DWORD stTextColor, DWORD stBackgroundColor, const char* pcString, int iLength) = 0;

};