/**
 *  file    VBE.h
 *  date    2009/08/29
 *  author  kkamagui 
 *          Copyright(c)2008 All rights reserved by kkamagui
 *  brief   VBE�� ���õ� �Լ��� ������ �ҽ� ����
 */

#include "VBE.h"

// ��� ���� ��� �ڷᱸ��
static VBEMODEINFOBLOCK* gs_pstVBEModeBlockInfo = 
    ( VBEMODEINFOBLOCK* ) VBE_MODEINFOBLOCKADDRESS;

VBEMODEINFOBLOCK* g_pstVBEModeBlockInfo = nullptr;

/**
 *  VBE ��� ���� ����� ��ȯ
 */
VBEMODEINFOBLOCK* kGetVBEModeInfoBlock( void )
{
	if (g_pstVBEModeBlockInfo == nullptr)
		g_pstVBEModeBlockInfo = new VBEMODEINFOBLOCK;

	return g_pstVBEModeBlockInfo;
 //   return gs_pstVBEModeBlockInfo;
}
