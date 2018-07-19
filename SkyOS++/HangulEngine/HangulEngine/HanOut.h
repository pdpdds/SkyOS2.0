#pragma once
#include "hanlib.h" // �ѱ� ���̺귯�� 0.17

#define OVERWRITE   0
#define OVERLAP     1

extern TEngFont *pDefEngFont;
extern THanFont *pDefHanFont;
extern TSpcFont *pDefSpcFont;
extern THanjaFont *pDefHanjaFont;

void SetOutputMode(int mode);
void HanTextOut(int left, int top, byte *s);

