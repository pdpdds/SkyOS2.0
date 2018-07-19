#include "SkyConsoleTask.h"
#include "SkyOS.h"
#include "SkyRenderer.h"
#include "ConsoleIOListener.h"
#include "SkySheet.h"

SkyConsoleTask::SkyConsoleTask()
{
	m_pRenderer = nullptr;
	m_pGUI = nullptr;
	m_pIOListener = nullptr;
	m_pSheet = nullptr;
	m_ProcessId = -1;
}

SkyConsoleTask::~SkyConsoleTask()
{
}

bool SkyConsoleTask::Init(SkyGUI* pGUI, Thread* pTask)
{
	m_cursorX = 16;
	m_cursorY = 28;
	m_cursorCol = COL8_FFFFFF;

	m_pRenderer = pGUI->GetRenderer();
	m_pGUI = pGUI;
	m_pIOListener = new ConsoleIOListener();

	Process* pProcess = pTask->m_pParent;
	m_ProcessId = pProcess->GetProcessId();

	m_pSheet = pGUI->FindSheetByID(m_ProcessId);

	m_pGUI->RegisterIOListener(pProcess->GetProcessId(), m_pIOListener);

	//������Ʈ > ǥ��
	m_pRenderer->PutFontAscToSheet(m_pSheet, 8, 28, COL8_FFFFFF, COL8_000000, ">", 1);

	return true;
}

bool SkyConsoleTask::kGetMessage(LPSKY_MSG lpMsg, HWND hWnd, UINT wMsgFilterMin, UINT wMsgFilterMax)
{
	lpMsg->_msgId = SKY_MSG_NO_MESSAGE;
	lpMsg->_extra = 0;

	kEnterCriticalSection();
	if (m_pIOListener->ReadyStatus() == false)
	{
		Scheduler::GetInstance()->Yield();
	}
	else
	{
		lpMsg->_msgId = SKY_MSG_MESSAGE;
		lpMsg->_extra = m_pIOListener->GetStatus();
	}

	kLeaveCriticalSection();

	return true;
}

bool SkyConsoleTask::kTranslateAccelerator(HWND hWnd, HANDLE hAccTable, LPSKY_MSG lpMsg)
{
	return lpMsg->_msgId != SKY_MSG_MESSAGE;
}

bool SkyConsoleTask::kTranslateMessage(const LPSKY_MSG lpMsg)
{

	if (256 <= lpMsg->_extra && lpMsg->_extra <= 511) //Ű���� ������
	{
		lpMsg->_msgId = SKY_MSG_KEYBOARD;
	}
	else if (512 <= lpMsg->_extra && lpMsg->_extra <= 767) //���콺 ������ ó��
	{

		lpMsg->_msgId = SKY_MSG_MOUSE;
	}

	return true;
}

bool SkyConsoleTask::kDispatchMessage(const LPSKY_MSG lpMsg)
{
	return kWndProc(lpMsg->_hwnd, lpMsg->_msgId, lpMsg->_extra, 0);
}

bool CALLBACK SkyConsoleTask::kWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{

	switch (message)
	{
	case SKY_MSG_KEYBOARD:
	{
		ProcessKeyboard(wParam);
	}
	break;
	case SKY_MSG_MOUSE:
	{
		// Not Implemented
		//ProcessMouse(wParam);
	}
	break;
	case SKY_MSG_EXIT:
		//Not Implemented
		break;
		//default:
		//return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}


bool IsEnter(int value)
{
	return value == 10 + 256;
}

bool IsBackSpace(int value)
{
	return value == 8 + 256;
}

void SkyConsoleTask::ProcessKeyboard(int value)
{
	if (IsBackSpace(value))
	{
		//�齺���̽��� ������� �ִٸ�
		if (m_cursorX > 16) 
		{
			m_pRenderer->PutFontAscToSheet(m_pSheet, m_cursorX, m_cursorY, COL8_FFFFFF, COL8_000000, " ", 1);
			m_cursorX -= 8;
		}
	}
	else if (IsEnter(value))
	{
		//Ŀ���� �����̽��� ����� ���� ����
		m_pRenderer->PutFontAscToSheet(m_pSheet, m_cursorX, m_cursorY, COL8_FFFFFF, COL8_000000, " ", 1);
		cmdline[m_cursorX / 8 - 2] = 0;
		GetNewLine();

		if (strcmp(cmdline, "mem") == 0)
		{
			size_t totalMemory = PhysicalMemoryManager::GetMemorySize();
			sprintf(s, "total   %dMB", totalMemory / (1024 * 1024));
			m_pRenderer->PutFontAscToSheet(m_pSheet, 8, m_cursorY, COL8_FFFFFF, COL8_000000, s, 30);
			GetNewLine();
			sprintf(s, "free %dKB", PhysicalMemoryManager::GetFreeMemory() / 1024);
			m_pRenderer->PutFontAscToSheet(m_pSheet, 8, m_cursorY, COL8_FFFFFF, COL8_000000, s, 30);
			GetNewLine();
		}
		else if (strcmp(cmdline, "cls") == 0) /* cls Ŀ�ǵ� */
		{

			unsigned char* buf = m_pSheet->GetBuf();
			int bxsize = m_pSheet->GetXSize();
			for (int y = 28; y < 28 + 128; y++) {
				for (int x = 8; x < 8 + 240; x++) {
					buf[x + y * bxsize] = COL8_000000;
				}
			}
			m_pSheet->Refresh(8, 28, 8 + 240, 28 + 128);
			m_cursorY = 28;
		}

		else if (cmdline[0] != 0) {
			/* Ŀ��嵵 �ƴϰ� �� �൵ �ƴϴ� */
			m_pRenderer->PutFontAscToSheet(m_pSheet, 8, m_cursorY, COL8_FFFFFF, COL8_000000, "Bad command.", 12);
			GetNewLine();
		}
		//������Ʈ ǥ��
		m_pRenderer->PutFontAscToSheet(m_pSheet, 8, m_cursorY, COL8_FFFFFF, COL8_000000, ">", 1);
		m_cursorX = 16;
	}
	else 
	{
		//�Ϲݹ����̰� ������ ����� ���� ��� ȭ�鿡 ����Ѵ�.
		if (m_cursorX < 240) 
		{			
			s[0] = value - 256;
			s[1] = 0;
			cmdline[m_cursorX / 8 - 2] = value - 256;
			m_pRenderer->PutFontAscToSheet(m_pSheet, m_cursorX, m_cursorY, COL8_FFFFFF, COL8_000000, s, 1);
			m_cursorX += 8;
		}
	}

	//Ŀ�� ��ǥ��
	if (m_cursorCol >= 0) {
		m_pRenderer->BoxFill(m_pSheet->GetBuf(), m_pSheet->GetXSize(), m_cursorCol, m_cursorX, m_cursorY, m_cursorX + 7, m_cursorY + 15);
	}
	m_pSheet->Refresh(m_cursorX, m_cursorY, m_cursorX + 8, m_cursorY + 16);
}

bool SkyConsoleTask::Run()
{
	SKY_MSG msg;
	while (kGetMessage(&msg, nullptr, 0, 0))
	{
		if (!kTranslateAccelerator(msg._hwnd, nullptr, &msg))
		{
			kTranslateMessage(&msg);
			kDispatchMessage(&msg);
		}
	}

	return msg._msgId == SKY_MSG_EXIT;
}


int SkyConsoleTask::GetNewLine()
{
	int x, y;
	unsigned char* buf = m_pSheet->GetBuf();
	int bxsize = m_pSheet->GetXSize();
	if (m_cursorY < 28 + 112) {
		m_cursorY += 16; /* ���� �࿡ */
	}
	else 
	{
		/* ��ũ�� */
		for (y = 28; y < 28 + 112; y++) {
			for (x = 8; x < 8 + 240; x++) {
				buf[x + y * bxsize] = buf[x + (y + 16) * bxsize];
			}
		}
		for (y = 28 + 112; y < 28 + 128; y++) {
			for (x = 8; x < 8 + 240; x++) {
				buf[x + y * bxsize] = COL8_000000;
			}
		}

		m_pSheet->Refresh(8, 28, 8 + 240, 28 + 128);
	}
	return m_cursorY;
}