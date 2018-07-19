#include "Page.h"

/**
 *	IA-32e ��� Ŀ���� ���� ������ ���̺� ����
 */
void InitializePageTables(int pml4EntryAddress)
{
	PML4TENTRY* pstPML4TEntry;
	PDPTENTRY* pstPDPTEntry;
	PDENTRY* pstPDEntry;
	DWORD dwMappingAddress;
	int i;

	// PML4 ���̺� ����
	// ù ��° ��Ʈ�� �ܿ� �������� ��� 0���� �ʱ�ȭ
	pstPML4TEntry = (PML4TENTRY*)pml4EntryAddress;
	SetPageEntryData(&(pstPML4TEntry[0]), 0x00, (pml4EntryAddress + 0x1000), PAGE_FLAGS_DEFAULT,
		0);
	for (i = 1; i < PAGE_MAXENTRYCOUNT; i++)
	{
		SetPageEntryData(&(pstPML4TEntry[i]), 0, 0, 0, 0);
	}

	// ������ ���͸� ������ ���̺� ����
	// �ϳ��� PDPT�� 512GByte���� ���� �����ϹǷ� �ϳ��� �����
	// 64���� ��Ʈ���� �����Ͽ� 64GByte���� ������
	pstPDPTEntry = (PDPTENTRY*)(pml4EntryAddress + 0x1000);
	for (i = 0; i < 64; i++)
	{
		SetPageEntryData(&(pstPDPTEntry[i]), 0, (pml4EntryAddress + 0x2000) + (i * PAGE_TABLESIZE),
			PAGE_FLAGS_DEFAULT, 0);
	}
	for (i = 64; i < PAGE_MAXENTRYCOUNT; i++)
	{
		SetPageEntryData(&(pstPDPTEntry[i]), 0, 0, 0, 0);
	}

	// ������ ���͸� ���̺� ����
	// �ϳ��� ������ ���͸��� 1GByte���� ���� ���� 
	// �����ְ� 64���� ������ ���͸��� �����Ͽ� �� 64GB���� ����
	pstPDEntry = (PDENTRY*)(pml4EntryAddress + 0x2000);
	dwMappingAddress = 0;
	for (i = 0; i < PAGE_MAXENTRYCOUNT * 64; i++)
	{
		// 32��Ʈ�δ� ���� ��巹���� ǥ���� �� �����Ƿ�, Mbyte ������ ����� ����
		// ���� ����� �ٽ� 4Kbyte�� ������ 32��Ʈ �̻��� ��巹���� �����
		SetPageEntryData(&(pstPDEntry[i]),
			(i * (PAGE_DEFAULTSIZE >> 20)) >> 12, dwMappingAddress,
			PAGE_FLAGS_G | PAGE_FLAGS_DEFAULT | PAGE_FLAGS_PS, 0);
		dwMappingAddress += PAGE_DEFAULTSIZE;
	}
}

/**
 *	������ ��Ʈ���� ���� �ּҿ� �Ӽ� �÷��׸� ����
 */
void SetPageEntryData(PTENTRY* pstEntry, DWORD dwUpperBaseAddress,
	DWORD dwLowerBaseAddress, DWORD dwLowerFlags, DWORD dwUpperFlags)
{
	pstEntry->dwAttributeAndLowerBaseAddress = dwLowerBaseAddress | dwLowerFlags;
	pstEntry->dwUpperBaseAddressAndEXB = (dwUpperBaseAddress & 0xFF) |
		dwUpperFlags;
}
