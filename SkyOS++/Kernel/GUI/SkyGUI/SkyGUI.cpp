#include "SkyGUI.h"
#include "SkyOS.h"
#include "FontData.h"
#include "ProcessManager.h"
#include "SkyMouse.h"
#include "fifo.h"
#include "PhysicalMemoryManager.h"
#include "SkySheetController8.h"
#include "SkySheetController32.h"
#include "SkyRenderer.h"
#include "SkyGUIConsole.h"
#include "SkyRenderer8.h"
#include "SkyRenderer32.h"
#include "Scheduler.h"
#include "KernelProcedure.h"
#include "ConsoleIOListener.h"
#include "Process.h"
#include "Thread.h"
#include "SkyConsoleTask.h"
#include "SkyDebugger.h"

extern void EnableMouse(FIFO32 *fifo, int data0, MOUSE_DEC *mdec);
extern void ProcessSkyMouseHandler();
extern void ProcessSkyKeyboardHandler();

extern char skyFontData[4096];

#define DMA_PICU1       0x0020
#define DMA_PICU2       0x00A0

__declspec(naked) void SendEOI()
{
	_asm
	{
		PUSH EBP
		MOV  EBP, ESP
		PUSH EAX

		; [EBP] < -EBP
		; [EBP + 4] < -RET Addr
		; [EBP + 8] < -IRQ ��ȣ

		MOV AL, 20H; EOI ��ȣ�� ������.
		OUT DMA_PICU1, AL

		CMP BYTE PTR[EBP + 8], 7
		JBE END_OF_EOI
		OUT DMA_PICU2, AL; Send to 2 also

		END_OF_EOI :
		POP EAX
		POP EBP
		RET
	}
}
int first = 0;
int second = 0;
static int color = COL8_FF00FF;

__declspec(naked) void kSkyMouseHandler()
{

	_asm {
		PUSHAD
		PUSHFD
		CLI
	}

	_asm
	{
		call ProcessSkyMouseHandler
	}

	
	
	/*second = GetTickCount();
	
	if (second - first >= 10)
	{
		

		if (color == COL8_FF00FF)
			color = COL8_C6C6C6;
		else color = COL8_FF00FF;
		SkyConsole::Print("%d\n", first);
		first = GetTickCount();
		SkyGUI::FillRect8(100, 100, 100, 100, color, 1024, 768);
	}*/

		
	SendEOI();

	_asm
	{

		POPFD
		POPAD
		IRETD
	}
}

static char keytable0[0x80] = {
	0,   0,   '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', 0,   0,
	'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P', '[', ']', 0,   0,   'A', 'S',
	'D', 'F', 'G', 'H', 'J', 'K', 'L', ';', '\'', '`',   0,   '\\', 'Z', 'X', 'C', 'V',
	'B', 'N', 'M', ',', '.', '/', 0,   '*', 0,   ' ', 0,   0,   0,   0,   0,   0,
	0,   0,   0,   0,   0,   0,   0,   '7', '8', '9', '-', '4', '5', '6', '+', '1',
	'2', '3', '0', '.', 0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
	0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
	0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0
};
static char keytable1[0x80] = {
	0,   0,   '!', '@', '#', '$', '%', '&', '^', '*', '(', ')', '_', '+', 0,   0,
	'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P', '{', '}', 0,   0,   'A', 'S',
	'D', 'F', 'G', 'H', 'J', 'K', 'L', ':', '"', '~',   0,   '|', 'Z', 'X', 'C', 'V',
	'B', 'N', 'M', '<', '>', '?', 0,   '*', 0,   ' ', 0,   0,   0,   0,   0,   0,
	0,   0,   0,   0,   0,   0,   0,   '7', '8', '9', '-', '4', '5', '6', '+', '1',
	'2', '3', '0', '.', 0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
	0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
	0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0
};

__declspec(naked) void kSkyKeyboardHandler()
{
	_asm {
		PUSHAD
		PUSHFD
		CLI
	}

	_asm
	{
		call ProcessSkyKeyboardHandler
	}

	SendEOI();

	_asm
	{
		POPFD
		POPAD
		IRETD
	}
}

SkyGUI::SkyGUI()
{
	m_pPressedSheet = nullptr;
	m_RButtonPressed = false;
	m_debugProcessId = -1;
}


SkyGUI::~SkyGUI()
{
}

int cons_newline2(int cursor_y, SkySheet *sheet)
{
	int x, y;
	unsigned char* buf = sheet->GetBuf();
	int bxsize = sheet->GetXSize();
	if (cursor_y < 28 + 112) {
		cursor_y += 16; /* ���� �࿡ */
	}
	else {
		/* ��ũ�� */
		for (y = 28; y < 28 + 112; y++) {
			for (x = 8; x < 8 + 340; x++) {
				buf[x + y * bxsize] = buf[x + (y + 16) * bxsize];
			}
		}
		for (y = 28 + 112; y < 28 + 628; y++) {
			for (x = 8; x < 8 + 340; x++) {
				buf[x + y * bxsize] = COL8_000000;
			}
		}
		sheet->Refresh(8, 28, 8 + 340, 28 + 628);
	}
	return cursor_y;
}

int g_cursory = 0;
bool errorOuccored = false;
bool SkyGUI::Print(char* pMsg)
{
	int len = strlen(pMsg);

	if (len <= 0 || len > 256)
		return false;

	if (m_pRenderer && m_mainSheet)
	{
		char* pMessage = new char[len + 1];
		memset(pMessage, 0, len + 1);
		memcpy(pMessage, pMsg, len);
		
		kEnterCriticalSection();
		SendToMessage(m_debugProcessId, pMessage);
		kLeaveCriticalSection();
		
	}

	if (errorOuccored == true)
	{
		m_pRenderer->PutFontAscToSheet(sht_back, 8, g_cursory, COL8_FFFFFF, COL8_000000, pMsg, strlen(pMsg));
		g_cursory += 16;
		
	}
	

	return true;
}

bool SkyGUI::Clear()
{
	memset(m_pVideoRamPtr, 0, (m_width * m_height) * sizeof(char));
	errorOuccored = true;
	return true;
}

bool SkyGUI::PutKeyboardQueue(KEYDATA* pData)
{
	return false;
}

bool SkyGUI::PutMouseQueue(MOUSEDATA* pData)
{
	return false;
}

bool SkyGUI::Initialize(void* pVideoRamPtr, int width, int height, int bpp, uint8_t buffertype)
{
	m_pVideoRamPtr = (ULONG*)pVideoRamPtr;
	m_width = width;
	m_height = height;
	m_bpp = bpp;

	LoadFontFromMemory();

	//�ʱ�ȭ�� ����
	//ȭ�� ��������� ��׶���, ���콺�� ǥ���� ��Ʈ�� �����ϰ� ������ ������ �°� �½�ũ�ٳ� �ؽ�Ʈ�ٸ� ���δ�.
	MakeInitScreen();

	//����� �ý��� �籸��
	MakeIOSystem();

	CreateGUIDebugProcess();
	CreateGUIConsoleProcess(300, 4);
	//ProcessManager::GetInstance()->CreateProcessFromMemory("GUIWatchDog", GUIWatchDogProc, NULL, PROCESS_KERNEL);

	//SkyDebugger::GetInstance()->TraceStackWithSymbol();

	return true;
}

bool SkyGUI::MakeInitScreen()
{
	m_pRenderer = new SkyRenderer8();
	m_pRenderer->Initialize();

	m_mainSheet = new SkySheetController8();
	m_mainSheet->Initialize((unsigned char*)m_pVideoRamPtr, m_width, m_height);;

	//��׶��� ��Ʈ�� ����
	sht_back = m_mainSheet->Alloc();
	unsigned char* buf_back = m_mainSheet->AllocBuffer(m_width, m_height);

	sht_back->SetBuf(buf_back, m_width, m_height, -1);
	sht_back->m_movable = false;
	m_pRenderer->InitScreen((unsigned char *)buf_back, m_width, m_height);

	//���콺 ��Ʈ�� ����
	sht_mouse = m_mainSheet->Alloc();
	unsigned char* buf_mouse = m_mainSheet->AllocBuffer(16, 16);
	sht_mouse->SetBuf(buf_mouse, 16, 16, 99);
	sht_mouse->m_movable = false;
	m_pRenderer->InitMouseCursor((char *)buf_mouse, 99);


	mx = (m_width - 16) / 2;
	my = (m_height - 28 - 16) / 2;

	sht_back->Slide(0, 0);
	sht_back->Updown(0);

	sht_mouse->Slide(mx, my);
	sht_mouse->Updown(1);

	return true;
}

bool SkyGUI::LoadFontFromMemory()
{
	unsigned char* buffer = (unsigned char*)skyFontData; 
	int bufferIndex = 0;
	int charIndex = 0;

	memset(buffer, 0, 4096);

	int readIndex = 0;

	//fontData�� ���ǵ� ��Ʈ �迭�� Ŀ�ο��� ����� �� �ֵ��� ��ȯ�ؼ� skyFontData�� �����Ѵ�.
	//��Ʈ ������ 255��, �� ��Ʈ�� 8 * 16 = 128����Ʈ�̹Ƿ� fontData �迭�� ũ��� 32768
	while (readIndex < 32768) {

		//�ѹ��ڸ� ǥ���ϴ� ��Ʈ�� 8 * 16 = 128����Ʈ
		for (int i = 0; i < 128; i++)
		{
			//'*'�� �ȼ���, '.'�� �� ������ �ǹ��Ѵ�.
			//fontdata���� �ѹ����� ���� �� ������ 8����Ʈ�̹Ƿ�
			//�� 8����Ʈ�� �����鼭 1����Ʈ ������ ��´�.
			//skyFontData�� 4096 ����Ʈ �迭�̴�. => 32768 / 8 = 4096
			if (fontData[readIndex + i] == '*')
			{

				if (charIndex < 8)
				{
					char a = (char)(1 << (8 - 1 - charIndex));
					buffer[bufferIndex] |= a;
				}

			}

			if ((fontData[readIndex + i] == '*') || (fontData[readIndex + i] == '.'))
			{
				charIndex++;

				if (charIndex >= 8)
				{
					//8����Ʈ�� üũ�����Ƿ� bufferIndex�� �ϳ� ������Ų��.
					bufferIndex++;
					charIndex = 0;
				}
			}
		}

		readIndex += 128;

	}

	return true;
}
#define KEYCMD_SENDTO_MOUSE		0xd4
#define MOUSECMD_ENABLE			0xf4

#define PORT_KEYSTA				0x0064
#define KEYSTA_SEND_NOTREADY	0x02
#define KEYCMD_WRITE_MODE		0x60
#define KBC_MODE				0x47
bool SkyGUI::MakeIOSystem()
{
	//Ű����, ���콺 �ڵ鷯 ����
	kEnterCriticalSection();

	fifo32_init(&m_fifo, 1024, fifobuf);
	fifo32_init(&keycmd, 32, keycmd_buf);

	init_keyboard(&m_fifo, 256);
	EnableMouse(&m_fifo, 512, &mdec);

	SetInterruptVector(0x21, kSkyKeyboardHandler);
	SetInterruptVector(0x2c, kSkyMouseHandler);

	fifo32_put(&keycmd, KEYCMD_LED);
	fifo32_put(&keycmd, key_leds);

	kLeaveCriticalSection();

	wait_KBC_sendready();
	OutPortByte(PORT_KEYCMD, KEYCMD_WRITE_MODE);
	wait_KBC_sendready();
	OutPortByte(PORT_KEYDAT, KBC_MODE);

	/* ���콺 ��ȿ */
	wait_KBC_sendready();
	OutPortByte(PORT_KEYCMD, KEYCMD_SENDTO_MOUSE);
	wait_KBC_sendready();
	OutPortByte(PORT_KEYDAT, MOUSECMD_ENABLE);
	/* �ߵǸ� ACK(0xfa)�� �۽ŵǾ� �´� */
	mdec.phase = 0; /* ���콺�� 0xfa�� ��ٸ��� �ִ� �ܰ� */

	OutPortByte(PIC0_IMR, 0xf8); /* PIT�� PIC1�� Ű���带 �㰡(11111000) */
	OutPortByte(PIC1_IMR, 0xef); /* ���콺�� �㰡(11101111) */

	return true;
}

bool SkyGUI::kGetMessage(LPSKY_MSG lpMsg, HWND hWnd, UINT wMsgFilterMin, UINT wMsgFilterMax)
{
	lpMsg->_msgId = SKY_MSG_NO_MESSAGE;
	lpMsg->_extra = 0;

	kEnterCriticalSection();
	if (fifo32_status(&keycmd) > 0 && keycmd_wait < 0)
	{
		//Ű���� ��Ʈ�ѷ��� ���� �����Ͱ� ������ ������
		keycmd_wait = fifo32_get(&keycmd);
		kLeaveCriticalSection();
		wait_KBC_sendready();
		OutPortByte(PORT_KEYDAT, keycmd_wait);
	}
	else
		kLeaveCriticalSection();

	kEnterCriticalSection();
	if (fifo32_status(&m_fifo) != 0)
	{
		lpMsg->_msgId = SKY_MSG_MESSAGE;
		lpMsg->_extra = fifo32_get(&m_fifo);
	}
	else
	{
		
		//	Scheduler::GetInstance()->Yield();
	}

	kLeaveCriticalSection();

	return true;
}

bool SkyGUI::kTranslateAccelerator(HWND hWnd, HANDLE hAccTable, LPSKY_MSG lpMsg)
{
	return lpMsg->_msgId != SKY_MSG_MESSAGE;
}

bool SkyGUI::Run()
{			
	uintptr_t videoAddress = (uintptr_t)SkyGUISystem::GetInstance()->GetVideoRamInfo()._pVideoRamPtr;
	SkyConsole::Print("PhysBasePtr : 0x%x\n", videoAddress);

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

bool SkyGUI::kTranslateMessage(const LPSKY_MSG lpMsg)
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

bool SkyGUI::kDispatchMessage(const LPSKY_MSG lpMsg)
{
	return kWndProc(lpMsg->_hwnd, lpMsg->_msgId, lpMsg->_extra, 0);
}

bool CALLBACK SkyGUI::kWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
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

		ProcessMouse(wParam);
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

void SkyGUI::ProcessKeyboard(int value)
{
	if (value < 0x80 + 256) { /* Ű�ڵ带 ���� �ڵ�� ��ȯ */
		if (key_shift == 0) {
			s[0] = keytable0[value - 256];
		}
		else {
			s[0] = keytable1[value - 256];
		}
	}
	else {
		s[0] = 0;
	}

	if ('A' <= s[0] && s[0] <= 'Z') {	/* �Է� ���ڰ� ���ĺ� */
		if (((key_leds & 4) == 0 && key_shift == 0) ||
			((key_leds & 4) != 0 && key_shift != 0)) {
			s[0] += 0x20;	/* �빮�ڸ� �ҹ��ڿ� ��ȯ */
		}
	}
	kEnterCriticalSection();
	if (s[0] != 0)
	{
		SendToMessage(m_pFocusSheet, SKY_MSG_KEYBOARD, s[0] + 256);
	}
	if (value == 256 + 0x0e) // �� �����̽�
	{
		SendToMessage(m_pFocusSheet, SKY_MSG_KEYBOARD, 8 + 256);
	}
	if (value == 256 + 0x1c)// Enter 
	{
		SendToMessage(m_pFocusSheet, SKY_MSG_KEYBOARD, 10 + 256);
	}
	if (value == 256 + 0x2a) {	/* ���� ����Ʈ ON */
		key_shift |= 1;
	}
	if (value == 256 + 0x36) {	/* ������ ����Ʈ ON */
		key_shift |= 2;
	}
	if (value == 256 + 0xaa) {	/* ���� ����Ʈ OFF */
		key_shift &= ~1;
	}
	if (value == 256 + 0xb6) {	/* ������ ����Ʈ OFF */
		key_shift &= ~2;
	}
	if (value == 256 + 0x3a) {	/* CapsLock */
		key_leds ^= 4;
		fifo32_put(&keycmd, KEYCMD_LED);
		fifo32_put(&keycmd, key_leds);
	}
	if (value == 256 + 0x45) {	/* NumLock */
		key_leds ^= 2;
		fifo32_put(&keycmd, KEYCMD_LED);
		fifo32_put(&keycmd, key_leds);
	}
	if (value == 256 + 0x46) {	/* ScrollLock */
		key_leds ^= 1;
		fifo32_put(&keycmd, KEYCMD_LED);
		fifo32_put(&keycmd, key_leds);
	}

	if (value == 256 + 0xfa) {	/* Ű���尡 �����͸� �����ϰ� �޾Ҵ� */
		keycmd_wait = -1;
	}
	kLeaveCriticalSection();

	if (value == 256 + 0xfe) {	/* Ű���尡 �����͸� �����ϰ� ���� �� ������ */
		wait_KBC_sendready();
		OutPortByte(PORT_KEYDAT, keycmd_wait);
	}
}

void SkyGUI::ProcessMouse(int value)
{
	if (DecodeMouseValue(&mdec, value - 512) != 0)
	{
		/* ���콺 Ŀ���� �̵� */
		mx += mdec.x;
		my += mdec.y;
		if (mx < 0) {
			mx = 0;
		}
		if (my < 0) {
			my = 0;
		}
		if (mx > m_width - 1) {
			mx = m_width - 1;
		}
		if (my > m_height - 1) {
			my = m_height - 1;
		}

		
		//SkyConsole::Print("%d %d %d\n", mx, my, m_height);
		sht_mouse->Slide(mx, my);
		kEnterCriticalSection();
		
		
		//sht_mouse->Updown(3);

		if ((mdec.btn & 0x01) != 0)  //���� ��ư�� �����ٸ� ���콺 �ٷ� �Ʒ��� �����츦 �巡�� ó���Ѵ�.
		{
			if (m_pPressedSheet == nullptr)
				ProcessMouseLButton(mx, my);
		}
		else
		{
			if (m_pPressedSheet)
			{
				m_pPressedSheet->Slide(mx, my);

			}
			m_pPressedSheet = nullptr;
		}
		kLeaveCriticalSection();
		/*if ((mdec.btn & 0x02) != 0) //������ ��ư�� �����ٸ� �ܼ� ���μ����� �����Ѵ�.
		{
			if (m_RButtonPressed == false)
			{
				m_RButtonPressed = true;
				CreateGUIConsoleProcess();
			}
			//SkyGUIConsole::FillRect8(100, 100, 100, 100, COL8_C6C6C6, 1024, 768);
		}
		else
		{
			m_RButtonPressed = false;
		}*/
		
		

	}
}

void SkyGUI::ProcessMouseLButton(int x, int y)
{
	SkySheet* pSheet = m_mainSheet->FindSheet(x, y);
	if (pSheet == nullptr)
	{
		m_pFocusSheet = nullptr;
		m_pPressedSheet = nullptr;
		return;
	}

	SendToMessage(pSheet, SKY_MSG_MOUSE, 0);

	m_pPressedSheet = pSheet;
	m_pFocusSheet = pSheet;
}

bool SkyGUI::SendToMessage(SkySheet* pSheet, int message, int value)
{
	if (pSheet == nullptr)
		return false;

	if (pSheet->m_ownerProcess == m_debugProcessId)
		return false;

	map<int, ConsoleIOListener*>::iterator iter = m_mapIOListener.find(pSheet->m_ownerProcess);

	if (iter == m_mapIOListener.end())
		return false;

	//pSheet->Slide(x, y);

	ConsoleIOListener* listener = (*iter).second;
	listener->PushMessage(message, value);

	return true;
}

bool SkyGUI::SendToMessage(int processID, char* pMsg)
{
	Process* pProcess = ProcessManager::GetInstance()->FindProcess(processID);

	if (pProcess == nullptr)
		return false;

	pProcess->AddMessage(pMsg);

	return true;
}

void SkyGUI::CreateGUIConsoleProcess(int xPos, int yPos)
{
	kEnterCriticalSection();

	Process* pProcess = nullptr;

	pProcess = ProcessManager::GetInstance()->CreateProcessFromMemory("GUIConsole", ConsoleGUIProc, this, PROCESS_KERNEL);
	if (pProcess != nullptr)
	{
		//�ܼ� �½�ũ ��Ʈ�� ����
		SkySheet* console = m_mainSheet->Alloc();
		unsigned char* buf = m_mainSheet->AllocBuffer(256, 165);
		console->SetBuf(buf, 256, 165, -1);
		m_pRenderer->MakeWindow(buf, 256, 165, "Sky Console", 0);
		m_pRenderer->MakeTextBox(console, 8, 28, 240, 128, COL8_000000);
		console->Slide(xPos, yPos);
		console->Updown(2);
		console->m_ownerProcess = pProcess->GetProcessId();
	}
	kLeaveCriticalSection();
}

bool SkyGUI::CreateGUIDebugProcess()
{
	kEnterCriticalSection();
	Process* pProcess = nullptr;
	bool result = false;
	
	pProcess = ProcessManager::GetInstance()->CreateProcessFromMemory("DEBUGGUI", ConsoleDebugGUIProc, this, PROCESS_KERNEL);
	if (pProcess != nullptr)
	{
		//�ܼ� �½�ũ ��Ʈ�� ����
		SkySheet* console = m_mainSheet->Alloc();
		unsigned char* buf = m_mainSheet->AllocBuffer(256, 165);
		console->SetBuf(buf, 256, 165, -1);
		m_pRenderer->MakeWindow(buf, 256, 165, "DEBUG GUI", 0);
		m_pRenderer->MakeTextBox(console, 8, 28, 240, 128, COL8_000000);
		console->Slide(32, 4);
		console->Updown(1);
		console->m_ownerProcess = pProcess->GetProcessId();

		m_debugProcessId = pProcess->GetProcessId();
		result = true;
	}

	kLeaveCriticalSection();

	return result;
}

void SkyGUI::RegisterIOListener(int processID, ConsoleIOListener* listener)
{
	kEnterCriticalSection();

	m_mapIOListener[processID] = listener;

	kLeaveCriticalSection();
}

SkySheet* SkyGUI::FindSheetByID(int processId)
{
	return m_mainSheet->FindSheetById(processId);
}

ULONG* SkyGUI::m_pVideoRamPtr = nullptr;

void SkyGUI::FillRect8(int x, int y, int w, int h, char col, int actualX, int actualY)
{
	char* lfb = (char*)m_pVideoRamPtr;

	for (int k = 0; k < h; k++)
		for (int j = 0; j < w; j++)
		{
			int index = ((j + x) + (k + y) * actualX);
			lfb[index] = col;
			index++;
		}

}






//�Է�â �½�ũ(task_a) ����
/*sht_win = shtctl->Alloc();
buf_win = new unsigned char[160 * 52];
sht_win->SetBuf(buf_win, 144, 52, -1);
SkyRenderer::MakeWindow8(buf_win, 144, 52, "task_a", 1);
SkyRenderer::MakeTextBox8(sht_win, 8, 28, 128, 16, COL8_FFFFFF);
*/

//sht_win->Slide(64, 56);
//sht_win->Updown(2);
int cons_newline(int cursor_y, SkySheet *sheet)
{
	int x, y;
	unsigned char* buf = sheet->GetBuf();
	int bxsize = sheet->GetXSize();
	if (cursor_y < 28 + 112) {
		cursor_y += 16; /* ���� �࿡ */
	}
	else {
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
		sheet->Refresh(8, 28, 8 + 240, 28 + 128);
	}
	return cursor_y;
}

DWORD WINAPI ConsoleDebugGUIProc(LPVOID parameter)
{
	kEnterCriticalSection();
	Thread* pThread = ProcessManager::GetInstance()->GetCurrentTask();
	Process* pProcess = pThread->m_pParent;

	SkyGUI* pGUI = (SkyGUI*)parameter;
	SkyRenderer* pRenderer = pGUI->GetRenderer();

	SkySheet *sheet = pGUI->FindSheetByID(pProcess->GetProcessId());
	int  cursor_x = 16, cursor_y = 28, cursor_c = -1;

	kLeaveCriticalSection();

	cursor_c = COL8_FFFFFF;

	//if(sheet == 0 || pProcess == 0 || pThread == 0 || pGUI == 0)
		//SkyGUI::FillRect8(100, 100, 100, 100, COL8_C6C6C6, 1024, 768);

	for (;;)
	{
		kEnterCriticalSection();
		
		list<char*>& messages = pProcess->GetMessageList();
		if (messages.size() > 0)
		{
		
			auto iter = messages.begin();
			for (; iter != messages.end(); iter++)
			{
				if (sheet)
				{
					pRenderer->PutFontAscToSheet(sheet, 8, cursor_y, COL8_FFFFFF, COL8_000000, *iter, strlen(*iter));
					cursor_y = cons_newline(cursor_y, sheet);
				}
			}

			iter = messages.begin();
			for (; iter != messages.end(); iter++)
			{
				delete *iter;
			}

			messages.clear();			

			if(sheet)
				sheet->Refresh(cursor_x, cursor_y, cursor_x + 8, cursor_y + 16);
		}

		Scheduler::GetInstance()->Yield();

		kLeaveCriticalSection();
		
	}

	return 0;
}

DWORD WINAPI ConsoleGUIProc(LPVOID parameter)
{
	kEnterCriticalSection();
	Thread* pThread = ProcessManager::GetInstance()->GetCurrentTask();
	SkyGUI* pGUI = (SkyGUI*)parameter;

	SkyConsoleTask* pTask = new SkyConsoleTask();
	pTask->Init(pGUI, pThread);

	kLeaveCriticalSection();

	pTask->Run();

	return 0;
}