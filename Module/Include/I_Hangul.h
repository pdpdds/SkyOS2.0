#pragma once

//32��Ʈ �ѱ� �׷��� ����
class I_Hangul
{
public:
	virtual bool Initialize() = 0; //�ѱ���Ʈ �ε� �� �ʱ�ȭ
	virtual void PutFonts(char *vram, int xsize, int x, int y, unsigned int, unsigned char *s) = 0; //������ �׷��� ���� ���ۿ� ���ڿ��� ����Ѵ�. 
};