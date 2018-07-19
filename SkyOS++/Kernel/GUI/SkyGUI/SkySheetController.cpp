#include "SkySheetController.h"
#include "memory.h"

SkySheetController::SkySheetController()
{
	memset(m_pSheets, 0, sizeof(SkySheet*) * MAX_SHEETS);
}


SkySheetController::~SkySheetController()
{
}

SkySheet *SkySheetController::Alloc()
{
	SkySheet* sheet;
	int i;
	for (i = 0; i < MAX_SHEETS; i++)
	{
		if (sheets0[i].m_flags == 0)
		{
			sheet = &sheets0[i];
			sheet->m_flags = SHEET_USE; /* ����� ��ũ */
			sheet->m_height = -1; /* ��ǥ���� */
			sheet->SetOwner(this);

			return sheet;
		}
	}
	return nullptr;	//����� �� �ִ� ��Ʈ�� ����
}

SkySheet* SkySheetController::FindSheet(int x, int y)
{
	for (int h = m_sheetTop; h >= 0; h--)
	{
		if (m_pSheets[h] == nullptr)
			continue;

		if (m_pSheets[h]->m_movable == false)
			continue;

		if (m_pSheets[h]->InRange(x, y))
			return m_pSheets[h];
	}

	return nullptr;
}

SkySheet* SkySheetController::FindSheetById(int processId)
{
	for (int h = m_sheetTop; h >= 0; h--)
	{
		if (m_pSheets[h] == nullptr)
			continue;

		if (m_pSheets[h]->m_ownerProcess == processId)
			return m_pSheets[h];
	}

	return nullptr;
}

void SkySheetController::UpdateSheets(int old, int height, SkySheet* sheet)
{
	if (sheet == nullptr || height >= MAX_SHEETS)
		return;

	/* ���ϴ� �ַ� sheets[]�� �þ���� ��ü */
	if (old > height)  /* �������� �������� */
	{
		if (height >= 0) {
			/* ������ ���� ����ø��� */
			for (int h = old; h > height; h--)
			{
				m_pSheets[h] = m_pSheets[h - 1];
				m_pSheets[h]->m_height = h;
			}

			m_pSheets[height] = sheet;
			RefreshMap(sheet->m_vx0, sheet->m_vy0, sheet->m_vx0 + sheet->m_bxsize, sheet->m_vy0 + sheet->m_bysize, height + 1);
			RefreshSub(sheet->m_vx0, sheet->m_vy0, sheet->m_vx0 + sheet->m_bxsize, sheet->m_vy0 + sheet->m_bysize, height + 1, old);
		}
		else
		{	/* ��ǥ��ȭ */
			if (m_sheetTop > old) {
				/* ���� �Ǿ� �ִ� ���� ������ */
				for (int h = old; h < m_sheetTop; h++) {
					m_pSheets[h] = m_pSheets[h + 1];
					m_pSheets[h]->m_height = h;
				}
			}
			m_sheetTop--;

			RefreshMap(sheet->m_vx0, sheet->m_vy0, sheet->m_vx0 + sheet->m_bxsize, sheet->m_vy0 + sheet->m_bysize, 0);
			RefreshSub(sheet->m_vx0, sheet->m_vy0, sheet->m_vx0 + sheet->m_bxsize, sheet->m_vy0 + sheet->m_bysize, 0, old - 1);
		}
	}
	else if (old < height) /* �������� �������� */
	{
		
		if (old >= 0) {
			/* ������ ���� ���� ������ */
			for (int h = old; h < height; h++) {

				if (m_pSheets[h + 1])
				{
					m_pSheets[h] = m_pSheets[h + 1];
					m_pSheets[h]->m_height = h;
				}
				
			}
			m_pSheets[height] = sheet;
		}
		else {	/* ��ǥ�� ���¿��� ǥ�� ���·� */
				/* ���� �Ǿ� �ִ� ���� ��� �ø��� */
			for (int h = m_sheetTop; h >= height; h--)
			{
				if (m_pSheets[h])
				{
					m_pSheets[h + 1] = m_pSheets[h];
					m_pSheets[h + 1]->m_height = h + 1;
				}
				
			}

			m_pSheets[height] = sheet;

			m_sheetTop++;/* ǥ������ ���̾ 1�� �����ϹǷ�, �� ���� ���̰� �����Ѵ� */
		}
		RefreshMap(sheet->m_vx0, sheet->m_vy0, sheet->m_vx0 + sheet->m_bxsize, sheet->m_vy0 + sheet->m_bysize, height);
		RefreshSub(sheet->m_vx0, sheet->m_vy0, sheet->m_vx0 + sheet->m_bxsize, sheet->m_vy0 + sheet->m_bysize, height, height);
	}
}


void SkySheetController::RefreshMap(int vx0, int vy0, int vx1, int vy1, int h0)
{
	int h, bx, by, vx, vy, bx0, by0, bx1, by1;
	unsigned char *buf, sid;
	SkySheet *sht;

	if (vx0 < 0)
	{
		vx0 = 0;
	}

	if (vy0 < 0)
	{
		vy0 = 0;
	}

	if (vx1 > m_xsize - 1)
	{
		vx1 = m_xsize - 1;
	}

	if (vy1 > m_ysize)
	{
		vy1 = m_ysize;
	}

	for (h = h0; h <= m_sheetTop; h++)
	{
		sht = m_pSheets[h];

		if (sht == nullptr)
			continue;

		sid = sht - sheets0; /* ������ ���� �װ��� ���̾� ��ȣ�� �̿� */
		buf = sht->m_buf;

		if (buf == nullptr)
			continue;


		bx0 = vx0 - sht->m_vx0;
		by0 = vy0 - sht->m_vy0;
		bx1 = vx1 - sht->m_vx0;
		by1 = vy1 - sht->m_vy0;

		if (bx0 < 0)
		{
			bx0 = 0;
		}

		if (by0 < 0)
		{
			by0 = 0;
		}

		if (bx1 > sht->m_bxsize)
		{
			bx1 = sht->m_bxsize;
		}

		if (by1 > sht->m_bysize)
		{
			by1 = sht->m_bysize;
		}

		for (by = by0; by < by1; by++)
		{
			vy = sht->m_vy0 + by;
			for (bx = bx0; bx < bx1; bx++)
			{
				vx = sht->m_vx0 + bx;
				if (buf[by * sht->m_bxsize + bx] != sht->m_col_inv)
				{
					//if (vy * m_xsize + vx < 200000)
						m_map[vy * m_xsize + vx] = sid;
				}
			}
		}
	}
	return;
}