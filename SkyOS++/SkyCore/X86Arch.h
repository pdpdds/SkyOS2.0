#pragma once
#include "windef.h"

void	LoadPDBR(UINT32 physicalAddr);
UINT32	GetPDBR();

void	EnablePaging(bool state);
bool	IsPaging();

//ĳ���� TLB�� ����.
void FlushTranslationLockBufferEntry(UINT32 addr);

//������ ���丮�� PDTR �������Ϳ� ��Ʈ�Ѵ�
void SetPageDirectory(UINT32 dir);

