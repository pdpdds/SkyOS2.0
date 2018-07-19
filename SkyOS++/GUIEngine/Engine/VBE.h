/**
 *  file    VBE.h
 *  date    2009/08/29
 *  author  kkamagui 
 *          Copyright(c)2008 All rights reserved by kkamagui
 *  brief   VBE�� ���õ� �Լ��� ������ ��� ����
 */

#ifndef __VBE_H__
#define __VBE_H__

#include "windef.h"

////////////////////////////////////////////////////////////////////////////////
//
// ��ũ��
//
////////////////////////////////////////////////////////////////////////////////
// ��� ���� ����� ����� ��巹��
#define VBE_MODEINFOBLOCKADDRESS            0x7E00
// �׷��� ���� �����ϴ� �÷��װ� ����� ��巹��
#define VBE_STARTGRAPHICMODEFLAGADDRESS     0x7C0A

////////////////////////////////////////////////////////////////////////////////
//
// ����ü
//
////////////////////////////////////////////////////////////////////////////////
#pragma pack( push, 1 )

// VBE���� ������ ��� ���� ���(ModeInfoBlock) �ڷᱸ��, 256����Ʈ
typedef struct kVBEInfoBlockStruct
{
    //==========================================================================
    // ��� VBE ������ ������ �κ�
    //==========================================================================
    WORD wModeAttribute;        // ����� �Ӽ�
    BYTE bWinAAttribute;        // ������ A�� �Ӽ�
    BYTE bWinBAttribute;        // ������ B�� �Ӽ�
    WORD wWinGranulity;         // �������� ����ġ(Granularity)
    WORD wWinSize;              // �������� ũ��
    WORD wWinASegment;          // ������ A�� �����ϴ� ���׸�Ʈ ��巹��
    WORD wWinBSegment;          // ������ B�� �����ϴ� ���׸�Ʈ ��巹��
    DWORD dwWinFuncPtr;         // ������ ���� �Լ��� ������(���� ��� ��)
    WORD wBytesPerScanLine;     // ȭ�� ��ĵ ����(Scan Line) �� ����Ʈ ��
    
    //==========================================================================
    // VBE ���� 1.2 �̻� ������ �κ�
    //==========================================================================
    WORD wXResolution;          // X�� �ȼ� �� �Ǵ� ���� ��
    WORD wYResolution;          // Y�� �ȼ� �� �Ǵ� ���� ��
    BYTE bXCharSize;            // �� ������ X�� �ȼ� ��
    BYTE bYCharSize;            // �� ������ Y�� �ȼ� ��
    BYTE bNumberOfPlane;        // �޸� �÷���(Memory Plane) ��
    BYTE bBitsPerPixel;         // �� �ȼ��� �����ϴ� ��Ʈ ��
    BYTE bNumberOfBanks;        // ��ũ(Bank) ��
    BYTE bMemoryModel;          // ���� �޸� ����
    BYTE bBankSize;             // ��ũ�� ũ��(Kbyte)
    BYTE bNumberOfImagePages;   // �̹��� ������ ����
    BYTE bReserved;             // ������ ����� ���� ����� ����
    
    // ���̷�Ʈ �÷�(Direct Color)�� ���õ� �ʵ�
    BYTE bRedMaskSize;              // ������(Red) �ʵ尡 �����ϴ� ũ��
    BYTE bRedFieldPosition;         // ������ �ʵ��� ��ġ
    BYTE bGreenMaskSize;            // ���(Green) �ʵ尡 �����ϴ� ũ��
    BYTE bGreenFieldPosition;       // ��� �ʵ��� ��ġ
    BYTE bBlueMaskSize;             // �Ķ���(Blue) �ʵ尡 �����ϴ� ũ��
    BYTE bBlueFieldPosition;        // �Ķ��� �ʵ��� ��ġ
    BYTE bReservedMaskSize;         // ����� �ʵ��� ũ��
    BYTE bReservedFieldPosition;    // ����� �ʵ��� ��ġ
    BYTE bDirectColorModeInfo;      // ���̷�Ʈ �÷� ����� ����
    
    //==========================================================================
    // VBE ���� 2.0 �̻� ������ �κ�
    //==========================================================================
    DWORD dwPhysicalBasePointer;    // ���� ������ ���� �޸��� ���� ��巹��
    DWORD dwReserved1;              // ����� �ʵ�
    DWORD dwReserved2;
    
    //==========================================================================
    // VBE ���� 3.0 �̻� ������ �κ�
    //==========================================================================
    WORD wLinearBytesPerScanLine;       // ���� ������ ���� ����� 
                                        // ȭ�� ��ĵ ����(Scan Line) �� ����Ʈ ��
    BYTE bBankNumberOfImagePages;       // ��ũ ����� �� �̹��� ������ ��
    BYTE bLinearNumberOfImagePages;     // ���� ������ ���� ����� �� �̹��� ������ ��
    // ���� ������ ���� ����� �� ���̷�Ʈ �÷�(Direct Color)�� ���õ� �ʵ�
    BYTE bLinearRedMaskSize;            // ������(Red) �ʵ尡 �����ϴ� ũ�� 
    BYTE bLinearRedFieldPosition;       // ������ �ʵ��� ��ġ
    BYTE bLinearGreenMaskSize;          // ���(Green) �ʵ尡 �����ϴ� ũ��
    BYTE bLinearGreenFieldPosition;     // ��� �ʵ��� ��ġ
    BYTE bLinearBlueMaskSize;           // �Ķ���(Blue) �ʵ尡 �����ϴ� ũ��
    BYTE bLinearBlueFieldPosition;      // �Ķ��� �ʵ��� ��ġ
    BYTE bLinearReservedMaskSize;       // ����� �ʵ��� ũ��
    BYTE bLinearReservedFieldPosition;  // ����� �ʵ��� ��ġ
    DWORD dwMaxPixelClock;              // �ȼ� Ŭ���� �ִ� ��(Hz)

    BYTE vbReserved[ 189 ];             // ������ ����
} VBEMODEINFOBLOCK;

#pragma pack( pop )

////////////////////////////////////////////////////////////////////////////////
//
// �Լ�
//
////////////////////////////////////////////////////////////////////////////////
VBEMODEINFOBLOCK* kGetVBEModeInfoBlock( void );

#endif /*__VBE_H__*/
