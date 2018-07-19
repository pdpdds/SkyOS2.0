#include "SkySheet.h"
#include "SkySheetController.h"

SkySheet::SkySheet()
{
	Init();
}

void SkySheet::Init()
{
	m_ownerProcess = -1;
	m_movable = true;
	m_vx0 = 0;
	m_vy0 = 0;
	m_buf = nullptr;
	m_bxsize = 0;
	m_bysize = 0;
	m_col_inv = 0;
	m_ctl = nullptr;
	m_height = -1;
	m_flags = 0;/* �̻�� ��ũ */
}


SkySheet::~SkySheet()
{
}


void SkySheet::SetBuf(unsigned char *buf, int xsize, int ysize, int col_inv)
{
	m_buf = buf;
	m_bxsize = xsize;
	m_bysize = ysize;
	m_col_inv = col_inv;
	
	return;
}



void SkySheet::Updown(int height)
{
	SkySheetController* ctl = GetOwner();
	int top = ctl->GetTop();

	int old = m_height; /* ���� ���� ���̸� ����Ѵ� */
						  /* ������ �ʹ� ���ų� �ʹ� ������ �����Ѵ� */
	if (height > top + 1) {
		height = top + 1;
	}
	if (height < -1) 
	{
		height = -1;
	}

	m_height = height; /* ���̸� ���� */

	ctl->UpdateSheets(old, height, this);
	
	return;
}

void SkySheet::Refresh(int bx0, int by0, int bx1, int by1)
{
	SkySheetController* ctl = GetOwner();

	if (m_height >= 0) { /* ���� ǥ�����̶��, ���ο� ���̾��� ������ ���� ȭ���� �ٽ� �׸��� */
		ctl->RefreshSub(m_vx0 + bx0, m_vy0 + by0, m_vx0 + bx1, m_vy0 + by1, m_height, m_height);
	}
	return;
}
#include "SkyConsole.h"
void SkySheet::Slide(int vx0, int vy0)
{
	int old_vx0 = m_vx0, old_vy0 = m_vy0;
	m_vx0 = vx0;
	m_vy0 = vy0;

	SkySheetController* ctl = GetOwner();
	//SkyConsole::Print("SkySheet height %d %d\n", m_height, m_ownerProcess);
	if (m_height >= 0) { /* ���� ǥ�����̶��, ���ο� ���̾��� ������ ���� ȭ���� �ٽ� �׸��� */
		ctl->RefreshMap(old_vx0, old_vy0, old_vx0 + m_bxsize, old_vy0 + m_bysize, 0);
		ctl->RefreshMap(vx0, vy0, vx0 + m_bxsize, vy0 + m_bysize, m_height);
		ctl->RefreshSub(old_vx0, old_vy0, old_vx0 + m_bxsize, old_vy0 + m_bysize, 0, m_height - 1);
		ctl->RefreshSub(vx0, vy0, vx0 + m_bxsize, vy0 + m_bysize, m_height, m_height);
	}
	return;
}

void SkySheet::Free()
{
	if (m_height >= 0) 
	{
		Updown(-1); /* ǥ�����̶�� �켱 ��ǥ�÷� �Ѵ� */
	}
	
	m_flags = 0; /* �̻�� ��ũ */
	return;
}

bool SkySheet::InRange(int x, int y)
{
	if (m_vx0 < x && x < m_vx0 + m_bxsize)
		if (m_vy0 < y && y < m_vy0 + m_bysize)
			return true;

	return false;
}