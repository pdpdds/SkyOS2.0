//---------------------------------------------------------------------------
#ifndef CP949CodeTableH
#define CP949CodeTableH
//-----------------------------------------------------------------------------
#ifdef __cplusplus
extern "C" {
#endif 
//---------------------------------------------------------------------------
typedef struct {
	unsigned cp949;		// ���� �ϼ��� (=Ȯ�� �ϼ���)
	unsigned KSSM;		// ��� ������
	unsigned Unicode;	// �����ڵ�
	int CodeType;
} TCodeTable;

extern TCodeTable CP949CodeTable[17304 + 67];	// 0.55 ����

#define CODETABLE_ASCII_128			0
#define CODETABLE_DBCS_LEAD_BYTE	1
#define CODETABLE_SPC				2
#define CODETABLE_CJK				3
#define CODETABLE_HANGUL			4
#define CODETABLE_HANGUL_SYLLABLE   5
//-----------------------------------------------------------------------------
#ifdef __cplusplus
}
#endif 
//-----------------------------------------------------------------------------
#endif
