#pragma once
#include "windef.h"

typedef WORD    COLOR;

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
// ������ ���̺�
typedef struct tag_HUFF
{
    int elem; // ��� ����
    unsigned short code[256];
    unsigned char  size[256];
    unsigned char  value[256];
}HUFF;

// JPEG ���ڵ��� ���� �ڷᱸ��
typedef struct tag_JPEG
{
    // SOF
    int width;
    int height;
    // MCU
    int mcu_width;
    int mcu_height;

    int max_h,max_v;
    int compo_count;
    int compo_id[3];
    int compo_sample[3];
    int compo_h[3];
    int compo_v[3];
    int compo_qt[3];

    // SOS
    int scan_count;
    int scan_id[3];
    int scan_ac[3];
    int scan_dc[3];
    int scan_h[3];  // ���ø� ��� ��
    int scan_v[3];  // ���ø� ��� ��
    int scan_qt[3]; // ����ȭ ���̺� �ε���
    
    // DRI
    int interval;

    int mcu_buf[32*32*4]; // ����
    int *mcu_yuv[4];
    int mcu_preDC[3];
    
    // DQT
    int dqt[3][64];
    int n_dqt;
    
    // DHT
    HUFF huff[2][3];
    
    
    // i/o
    unsigned char *data;
    int data_index;
    int data_size;
    
    unsigned long bit_buff;
    int bit_remain;
    
}JPEG;


bool kJPEGInit(JPEG *jpeg, BYTE* pbFileBuffer, DWORD dwFileSize);
bool kJPEGDecode(JPEG *jpeg, COLOR* rgb);

