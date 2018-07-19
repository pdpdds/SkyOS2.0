/**
 *  file    2DGraphics.h
 *  date    2009/09/5
 *  author  kkamagui 
 *          Copyright(c)2008 All rights reserved by kkamagui
 *  brief   2D Graphic�� ���� ��� ����
 */

#ifndef __2DGRAPHICS_H__
#define __2DGRAPHICS_H__

#include "windef.h"

////////////////////////////////////////////////////////////////////////////////
//
// ��ũ��
//
////////////////////////////////////////////////////////////////////////////////
// ���� �����ϴµ� ����� �ڷᱸ��, 16��Ʈ ���� ����ϹǷ� WORD�� ����
typedef WORD                COLOR;

// 0~255 ������ R, G, B�� 16��Ʈ �� �������� ��ȯ�ϴ� ��ũ��
// 0~255�� ������ 0~31, 0~63���� ����Ͽ� ����ϹǷ� ���� 8�� 4�� ��������� ��
// ������ 8�� ������ 4�� >> 3�� >> 2�� ��ü
#define RGB( r, g, b )      ( ( ( BYTE )( r ) >> 3 ) << 11 | \
                ( ( ( BYTE )( g ) >> 2 ) ) << 5 |  ( ( BYTE )( b ) >> 3 ) )


////////////////////////////////////////////////////////////////////////////////
//
// ����ü
//
////////////////////////////////////////////////////////////////////////////////
// �簢���� ������ ��� �ڷᱸ��
typedef struct kRectangleStruct
{
    // ���� ��(������)�� X ��ǥ
    int iX1;
    // ���� ��(������)�� Y ��ǥ
    int iY1;
    
    // ������ �Ʒ�(����)�� X ��ǥ
    int iX2;
    // ������ �Ʒ�(����)�� Y��ǥ
    int iY2;
} RECT;

// ���� ������ ��� �ڷᱸ��
typedef struct kPointStruct
{
    // X�� Y�� ��ǥ
    int iX;
    int iY;
} POINT;

////////////////////////////////////////////////////////////////////////////////
//
// �Լ�
//
////////////////////////////////////////////////////////////////////////////////
inline void kInternalDrawPixel( const RECT* pstMemoryArea, COLOR* pstMemoryAddress, 
        int iX, int iY, COLOR stColor );
void kInternalDrawLine( const RECT* pstMemoryArea, COLOR* pstMemoryAddress, 
        int iX1, int iY1, int iX2, int iY2, COLOR stColor );
void kInternalDrawRect( const RECT* pstMemoryArea, COLOR* pstMemoryAddress,
        int iX1, int iY1, int iX2, int iY2, COLOR stColor, bool bFill );
void kInternalDrawCircle( const RECT* pstMemoryArea, COLOR* pstMemoryAddress,
        int iX, int iY, int iRadius, COLOR stColor, bool bFill );
int kInternalDrawText( const RECT* pstMemoryArea, COLOR* pstMemoryAddress,
        int iX, int iY, COLOR stTextColor, COLOR stBackgroundColor, 
        const char* pcString, int iLength );
int kInternalDrawEnglishText( const RECT* pstMemoryArea, COLOR* pstMemoryAddress,
        int iX, int iY, COLOR stTextColor, COLOR stBackgroundColor,
        const char* pcString, int iLength );
int kInternalDrawHangulText( const RECT* pstMemoryArea, COLOR* pstMemoryAddress,
        int iX, int iY, COLOR stTextColor, COLOR stBackgroundColor,
        const char* pcString, int iLength );

inline bool kIsInRectangle( const RECT* pstArea, int iX, int iY );
inline int kGetRectangleWidth( const RECT* pstArea );
inline int kGetRectangleHeight( const RECT* pstArea );
inline void kSetRectangleData( int iX1, int iY1, int iX2, int iY2, RECT* pstRect );
inline bool kGetOverlappedRectangle( const RECT* pstArea1, const RECT* pstArea2,
        RECT* pstIntersection  );
inline bool kIsRectangleOverlapped( const RECT* pstArea1, 
        const RECT* pstArea2 );

#endif /*__2DGRAPHICS_H__*/
