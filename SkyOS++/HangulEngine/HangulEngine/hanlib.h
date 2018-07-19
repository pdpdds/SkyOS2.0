//-----------------------------------------------------------------------------
//
//	�ѱ� ���̺귯�� 0.17
//
//	2007.07.29
//
//-----------------------------------------------------------------------------
#ifndef hanlibH
#define hanlibH
//-----------------------------------------------------------------------------
#ifdef __cplusplus
extern "C" {
#endif
//-----------------------------------------------------------------------------
#include "windef.h"
#include <stdio.h>
//#include <stdlib.h>
#include <string.h>
#include "sprintf.h"
#include "fileio.h"
#include "memory.h"
//-----------------------------------------------------------------------------
#ifndef __cplusplus
typedef enum {false, true} bool;
#endif

typedef unsigned char byte;
typedef unsigned short int word;
typedef unsigned long int dword;

typedef struct {
    unsigned int F3		: 5;	// ����
    unsigned int F2		: 5;	// �߼�
    unsigned int F1		: 5;	// �ʼ�
    unsigned int MSB	: 1;	// MSB (�ֻ��� ��Ʈ) 1 - �ѱ�, 0 - ����
	unsigned int Dummy	: 16;	// dummy
} THanCode;

typedef struct {
    byte Byte1;		// �ѱ��� ���� ����Ʈ (������)
    byte Byte0;		// �ѱ��� ���� ����Ʈ (����)
	byte Dymmy[2];
} THanByte;

typedef union {
	THanCode HanCode;
	THanByte HanByte;
} THangul;
//-----------------------------------------------------------------------------
typedef struct {
    char FullFileName[512];
    int FileSize, CharCount;
	byte Eng[256][32];
} TEngFont;

#define SPCFONT_KSS     0
#define SPCFONT_KSG     1
#define SPCFONT_SAMBO   2

typedef struct {
    char FullFileName[512];
    int FileSize, CharCount;
	byte Spc[12][94][32];       // 12 x 94 x 32 == 36,096 byte
    byte SamboSpc[128][32];
    int SType;
} TSpcFont;

typedef struct {
    char FullFileName[512];
    int FileSize, CharCount;
	byte Hanja[52][94][32];     // 52 x 94 x 32 == 156,416 byte
} THanjaFont;

#define HANFONT_10X4X4	0
#define HANFONT_8X4X4	1
#define HANFONT_6X2X1	2
#define HANFONT_2X1X2	3

typedef struct {
    char FullFileName[512];
    int FileSize, CharsCount, HanFontType;
    int F_SKIP, F1_SKIP, F2_SKIP, F3_SKIP;
    int F1Count, F2Count, F3Count;

	int F1BulCount;
	int F2BulCount;
	int F3BulCount;

	byte F1[10][1 + 19 + 4][32];
	byte F2[4][1 + 21 + 2][32];
	byte F3[4][1 + 27 + 4][32];
	byte *pHangulJamo[51];

	//byte Hangul[2350][32];      // KS�ϼ���2350
    // 0.55 ���� (17304 -> 17304 + 67
	//byte Unicode[17304 + 67][32];    // �����ڵ� 17304 = 128(0~127) + 128(128~255, ������) + 11172 + 988 + 4888 + ù����67

	byte *pF1B, *pF2B, *pF3B;
} THanFont;

//-----------------------------------------------------------------------------
#include "LoadEngFont.h"
#include "LoadHanFont.h"
#include "LoadSpcFont.h"
#include "LoadHanjaFont.h"
#include "CompleteHan.h"
#include "CompleteKS.h"
#include "CP949Table.h"

#include "Johab.h"
#include "Table10x4x4.h"
#include "Table8x4x4.h"
#include "Table6x2x1.h"
#include "Table2x1x2.h"
//-----------------------------------------------------------------------------
#ifdef __cplusplus
}
#endif
//-----------------------------------------------------------------------------
#endif
