#include "HangulEngine.h"
#include "HangulInput.h"
#include "memory.h"
#include "string.h"
#include "2DGraphics.h"

#define KEY_BACKSPACE 8
#define KEY_LALT 0x85

BUFFERMANAGER stBufferManager;

HangulEngine::HangulEngine()
{
	m_bHangulMode = false;
	memset(&stBufferManager, 0, sizeof(BUFFERMANAGER));
}

void HangulEngine::Reset()
{
	memset(&stBufferManager, 0, sizeof(BUFFERMANAGER));
}

HangulEngine::~HangulEngine()
{
}

bool HangulEngine::SwitchMode()
{
	if (m_bHangulMode == false)
		m_bHangulMode = true;
	else
		m_bHangulMode = false;

	return m_bHangulMode;
}

bool HangulEngine::InputAscii(unsigned char letter)
{
	switch (letter)
	{
		//------------------------------------------------------------------
		// ��/�� ��� ��ȯ ó��
		//------------------------------------------------------------------
	case KEY_LALT:
		// �ѱ� �Է� ��� �߿� Alt Ű�� �������� �ѱ� ������ ����
		if (m_bHangulMode == TRUE)
		{
			// Ű �Է� ���۸� �ʱ�ȭ
			stBufferManager.iInputBufferLength = 0;

			if ((stBufferManager.vcOutputBufferForProcessing[0] != '\0') &&
				(stBufferManager.iOutputBufferLength + 2 < MAXOUTPUTLENGTH))
			{
				// ���� ���� �ѱ��� ������ ȭ�鿡 ����ϴ� ���۷� ����
				memcpy(stBufferManager.vcOutputBuffer +
					stBufferManager.iOutputBufferLength,
					stBufferManager.vcOutputBufferForProcessing, 2);
				stBufferManager.iOutputBufferLength += 2;

				// ���� ���� �ѱ��� �����ϴ� ���۸� �ʱ�ȭ
				stBufferManager.vcOutputBufferForProcessing[0] = '\0';
			}
		}
		// ���� �Է� ��� �߿� Alt Ű�� �������� �ѱ� ���տ� ���۸� �ʱ�ȭ
		else
		{
			stBufferManager.iInputBufferLength = 0;
			stBufferManager.vcOutputBufferForComplete[0] = '\0';
			stBufferManager.vcOutputBufferForProcessing[0] = '\0';
		}
		m_bHangulMode = true - m_bHangulMode;
		break;

		//------------------------------------------------------------------
		// �齺���̽� Ű ó��
		//------------------------------------------------------------------
	case KEY_BACKSPACE:
		// �ѱ��� �����ϴ� ���̸� �Է� ������ ������ �����ϰ� ����
		// Ű �Է� ������ �������� �ѱ��� ����
		if ((m_bHangulMode == true) &&
			(stBufferManager.iInputBufferLength > 0))
		{
			// Ű �Է� ������ ������ �ϳ� �����ϰ� �ѱ��� �ٽ� ����
			stBufferManager.iInputBufferLength--;
			ComposeHangul(stBufferManager.vcInputBuffer,
				&stBufferManager.iInputBufferLength,
				stBufferManager.vcOutputBufferForProcessing,
				stBufferManager.vcOutputBufferForComplete);
		}
		// �ѱ� ���� ���� �ƴϸ� ������ ȭ�鿡 ����ϴ� ������ ������ ����
		else
		{
			if (stBufferManager.iOutputBufferLength > 0)
			{
				// ȭ�� ��� ���ۿ� ����ִ� ������ 2����Ʈ �̻��̰� ���ۿ�
				// ����� ���� �ֻ��� ��Ʈ�� ���� ������ �ѱ۷� �����ϰ�
				// ������ 2����Ʈ�� ��� ����
				if ((stBufferManager.iOutputBufferLength >= 2) &&
					(stBufferManager.vcOutputBuffer[
						stBufferManager.iOutputBufferLength - 1] & 0x80))
				{
					stBufferManager.iOutputBufferLength -= 2;
					memset(stBufferManager.vcOutputBuffer +
						stBufferManager.iOutputBufferLength, 0, 2);
				}
						// �ѱ��� �ƴϸ� ������ 1����Ʈ�� ����
				else
				{
					stBufferManager.iOutputBufferLength--;
					stBufferManager.vcOutputBuffer[
						stBufferManager.iOutputBufferLength] = '\0';
				}
			}
		}
		break;

		//------------------------------------------------------------------
		// ������ Ű���� ���� �Է� ��忡 ���� �ѱ��� �����ϰų�
		// ������ ȭ�鿡 ����ϴ� ���۷� ���� ����
		//------------------------------------------------------------------
	default:
		// Ư�� Ű���� ��� ����
		if (letter & 0x80)
		{
			break;
		}

		// �ѱ� ���� ���̸� �ѱ� ���� ó��(�ѱ��� ȭ�� ��� ���ۿ� ������
		// ������ ��������� Ȯ��)
		if ((m_bHangulMode == TRUE) &&
			(stBufferManager.iOutputBufferLength + 2 <= MAXOUTPUTLENGTH))
		{
			// ��/������ ����Ʈ�� ���յǴ� ��츦 ����Ͽ� �������̳�
			// �ָ����� ������ �������� �ҹ��ڷ� ��ȯ
			ConvertJaumMoumToLowerCharactor((BYTE*)&letter);

			// �Է� ���ۿ� Ű �Է� ���� ä��� �������� ���̸� ����
			stBufferManager.vcInputBuffer[
				stBufferManager.iInputBufferLength] = letter;
			stBufferManager.iInputBufferLength++;

			// �ѱ� ���տ� �ʿ��� ���۸� �Ѱ��༭ �ѱ��� ����
			if (ComposeHangul(stBufferManager.vcInputBuffer,
				&stBufferManager.iInputBufferLength,
				stBufferManager.vcOutputBufferForProcessing,
				stBufferManager.vcOutputBufferForComplete) == TRUE)
			{
				// ������ �Ϸ�� ���ۿ� ���� �ִ°� Ȯ���Ͽ� ������ ȭ�鿡
				// ����� ���۷� ����
				if (stBufferManager.vcOutputBufferForComplete[0] != '\0')
				{
					memcpy(stBufferManager.vcOutputBuffer +
						stBufferManager.iOutputBufferLength,
						stBufferManager.vcOutputBufferForComplete, 2);
					stBufferManager.iOutputBufferLength += 2;

					// ���յ� �ѱ��� ������ �ڿ� ���� ���� ���� �ѱ��� �����
					// ������ ���ٸ� Ű �Է� ���ۿ� ���� ���� �ѱ� ���۸� ��� �ʱ�ȭ
					if (stBufferManager.iOutputBufferLength + 2 > MAXOUTPUTLENGTH)
					{
						stBufferManager.iInputBufferLength = 0;
						stBufferManager.vcOutputBufferForProcessing[0] = '\0';
					}
				}
			}
			// ���տ� �����ϸ� �Է� ���ۿ� ���������� �Էµ� ���� �ѱ� ��/������
			// �ƴϹǷ� �ѱ� ������ �Ϸ�� ������ ���� �Է� ���ۿ� �ִ� ���� ���
			// ��� ���۷� ����
			else
			{
				// ������ �Ϸ�� ���ۿ� ���� �ִ°� Ȯ���Ͽ� ������ ȭ�鿡
				// ����� ���۷� ����
				if (stBufferManager.vcOutputBufferForComplete[0] != '\0')
				{
					// �ϼ��� �ѱ��� ��� ���۷� ����
					memcpy(stBufferManager.vcOutputBuffer +
						stBufferManager.iOutputBufferLength,
						stBufferManager.vcOutputBufferForComplete, 2);
					stBufferManager.iOutputBufferLength += 2;
				}

				// ������ ȭ�鿡 ����ϴ� ������ ������ ����ϸ� Ű �Է� ���ۿ�
				// ���������� �Էµ� �ѱ� ��/�� �ƴ� ���� ����
				if (stBufferManager.iOutputBufferLength < MAXOUTPUTLENGTH)
				{
					stBufferManager.vcOutputBuffer[
						stBufferManager.iOutputBufferLength] =
						stBufferManager.vcInputBuffer[0];
						stBufferManager.iOutputBufferLength++;
				}

				// Ű �Է� ���۸� ���
				stBufferManager.iInputBufferLength = 0;
			}
		}
		// �ѱ� �Է� ��尡 �ƴ� ���
		else if ((m_bHangulMode == false) &&
			(stBufferManager.iOutputBufferLength + 1 <= MAXOUTPUTLENGTH))
		{
			// Ű �Է��� �״�� ������ ȭ�鿡 ����ϴ� ���۷� ����
			stBufferManager.vcOutputBuffer[
				stBufferManager.iOutputBufferLength] = letter;
			stBufferManager.iOutputBufferLength++;
		}
		break;
	}

	return true;
}

//------------------------------------------------------------------
// ȭ�� ��� ���ۿ� �ִ� ���ڿ��� ���� ���� �ѱ� ���� ��Ʈ���� ����	
//------------------------------------------------------------------
int HangulEngine::GetString(char* buffer)
{		
	int length = stBufferManager.iOutputBufferLength;

	if (length != 0)
	{
		strcpy(buffer, stBufferManager.vcOutputBuffer);
	}

	// ���� ���� ���� �ѱ��� �ִٸ� ȭ�� ��� ������ ������ ��µ�
	// ���� ��ġ�� �������� �ѱ��� ���
	if (stBufferManager.vcOutputBufferForProcessing[0] != '\0')
	{
		buffer[length] = stBufferManager.vcOutputBufferForProcessing[0];
		buffer[length + 1] = stBufferManager.vcOutputBufferForProcessing[1];

		length += 2;
	}

	return length;	
}

int HangulEngine::DrawText(int iX1, int iY1, int iX2, int iY2, DWORD* pstMemoryAddress, int iX, int iY,
	DWORD stTextColor, DWORD stBackgroundColor, const char* pcString, int iLength)
{
	RECT memoryArea;
	memoryArea.iX1 = iX1;
	memoryArea.iY1 = iY1;
	memoryArea.iX2 = iX2;
	memoryArea.iY2 = iY2;

	return kInternalDrawText(&memoryArea, pstMemoryAddress, iX, iY, stTextColor, stBackgroundColor, pcString, iLength);
}
