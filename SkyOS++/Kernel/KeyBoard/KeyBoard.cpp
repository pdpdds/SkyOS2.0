#include <string.h>
#include <ctype.h>
#include <hal.h>
#include "KeyBoard.h"
#include "SkyConsole.h"

namespace KeyBoard
{
	static KeyBoardState _keyBoardState;
	
	
	//Ű���� ���ͷ�Ʈ �ڵ鷯
	__declspec(naked) void i86_kybrd_irq() 
	{

		_asm {
			cli
			pushad
		}

		_asm
		{
			call ProcessKeyBoardInterrupt
		}

		_asm {
			mov al, 0x20
			out 0x20, al
			popad
			sti
			iretd
		}
	}

	bool	GetScrollLock()
	{

		return _keyBoardState._scrolllock;
	}

	bool	GetNumLock()
	{

		return _keyBoardState._numlock;
	}

	bool	GetCapsLock()
	{

		return _keyBoardState._capslock;
	}

	bool	GetCtrl() 
	{

		return _keyBoardState._ctrl;
	}

	bool	GetAlt() 
	{

		return _keyBoardState._alt;
	}

	bool	GetShift()
	{

		return _keyBoardState._shift;
	}

	uint8_t GetLastScanCode() 
	{

		return _keyBoardState._scancode;
	}

	KEYCODE GetLastKeyCode() 
	{
		return (_keyBoardState._scancode != INVALID_SCANCODE) ? ((KEYCODE)KeyboardScanCode[_keyBoardState._scancode]) : (KEY_UNKNOWN);
	}

	void DiscardLastKeyCode()
	{

		_keyBoardState._scancode = INVALID_SCANCODE;
	}

//Ű���� ���� ���� �޼ҵ�
	void SetLeds(bool num, bool caps, bool scroll) {

		uint8_t data = 0;

		data = (scroll) ? (data | 1) : (data & 1);
		data = (num) ? 3 : 0;
		data = (caps) ? 0b101 : 0;

		SendEncodeCommand(KYBRD_ENC_CMD_SET_LED);
		SendEncodeCommand(data);
	}

	//Ű���� ��Ʈ�ѷ����� ���� Ű�Է� �����͸� ���� �� �ִ����� ��û�Ѵ�
	uint8_t ReadStatus()
	{

		return InPortByte(KYBRD_CTRL_STATS_REG);
	}

	//Ű���� ��Ʈ�ѷ����� Ŀ�ǵ� ����
	void SendCommand(uint8_t cmd)
	{

		//Ű���� ��Ʈ�ѷ��� �Է¹��۰� Ŭ����ɶ����� ����Ѵ�.
		while (1)
			if ((ReadStatus() & KYBRD_CTRL_STATS_MASK_IN_BUF) == 0)
				break;

		OutPortByte(KYBRD_CTRL_CMD_REG, cmd);
	}

	//Ű���� ��Ʈ�ѷ��κ��� ���ڵ��� Ű���� �����͸� �о���δ�.
	uint8_t ReadEncodeBuffer() 
	{

		return InPortByte(KYBRD_ENC_INPUT_BUF);
	}

	//Ű���忡 ����� �����Ѵ�.
	void SendEncodeCommand(uint8_t cmd) 
	{

		//Ű����κ��� �о���� Ű �����Ͱ� ������ Ŭ����ɶ����� ����Ѵ�.
		while (1)
			if ((ReadStatus() & KYBRD_CTRL_STATS_MASK_IN_BUF) == 0)
				break;

		OutPortByte(KYBRD_ENC_CMD_REG, cmd);
	}


	//Ű�ڵ带 �ƽ�Ű�ڵ�� ��ȯ�ؼ� �����Ѵ�.
	char ConvertKeyToAscii(KEYCODE code) 
	{

		uint8_t key = (uint8_t)code;

		//�ƽ�Ű�ڵ����� Ȯ��
		if (isascii(key)) {

			//����ƮŰ�� Caps Ű ���θ� üũ�ؼ� ���ĺ��� �빮�ڷ� �����Ѵ�.
			if (_keyBoardState._shift || _keyBoardState._capslock)
				if (key >= 'a' && key <= 'z')
					key -= 32;

			if (_keyBoardState._shift)
				if (key >= '0' && key <= '9')
					switch (key) {

					case '0':
						key = KEY_RIGHTPARENTHESIS;
						break;
					case '1':
						key = KEY_EXCLAMATION;
						break;
					case '2':
						key = KEY_AT;
						break;
					case '3':
						key = KEY_EXCLAMATION;
						break;
					case '4':
						key = KEY_HASH;
						break;
					case '5':
						key = KEY_PERCENT;
						break;
					case '6':
						key = KEY_CARRET;
						break;
					case '7':
						key = KEY_AMPERSAND;
						break;
					case '8':
						key = KEY_ASTERISK;
						break;
					case '9':
						key = KEY_LEFTPARENTHESIS;
						break;
					}
				else {

					switch (key) {
					case KEY_COMMA:
						key = KEY_LESS;
						break;

					case KEY_DOT:
						key = KEY_GREATER;
						break;

					case KEY_SLASH:
						key = KEY_QUESTION;
						break;

					case KEY_SEMICOLON:
						key = KEY_COLON;
						break;

					case KEY_QUOTE:
						key = KEY_QUOTEDOUBLE;
						break;

					case KEY_LEFTBRACKET:
						key = KEY_LEFTCURL;
						break;

					case KEY_RIGHTBRACKET:
						key = KEY_RIGHTCURL;
						break;

					case KEY_GRAVE:
						key = KEY_TILDE;
						break;

					case KEY_MINUS:
						key = KEY_UNDERSCORE;
						break;

					case KEY_PLUS:
						key = KEY_EQUAL;
						break;

					case KEY_BACKSLASH:
						key = KEY_BAR;
						break;
					}
				}

				return key;
		}

		return 0;
	}

	//Ű���带 ��Ȱ��ȭ��Ų��.
	void Disable() 
	{

		SendCommand(KYBRD_CTRL_CMD_DISABLE);
		_keyBoardState._disable = true;
	}

	//Ű���带 Ȱ��ȭ��Ų��.
	void Enable() 
	{

		SendCommand(KYBRD_CTRL_CMD_ENABLE);
		_keyBoardState._disable = false;
	}

	//Ű������ Ȱ��ȭ���θ� �˷��ش�.
	bool IsDisabled() 
	{

		return _keyBoardState._disable;
	}

	//Ű���� ��Ʈ�ѷ��� ����
	void ResetSystem() 
	{
		SendCommand(KYBRD_CTRL_CMD_WRITE_OUT_PORT);
		SendEncodeCommand(0xfe);
	}

	//Ű���� ��Ʈ�ѷ� �׽�Ʈ
	bool SelfTest() {

		SendCommand(KYBRD_CTRL_CMD_SELF_TEST);

		while (1)
			if (ReadStatus() & KYBRD_CTRL_STATS_MASK_OUT_BUF)
				break;

		return (ReadEncodeBuffer() == 0x55) ? true : false;
	}

	//Ű���带 ��ġ�Ѵ�.
	void Install(int irq) {

		//Ű���� ���ͷ�Ʈ �ڵ鷯 ��ġ
		SetInterruptVector(irq, i86_kybrd_irq);

		_keyBoardState._alt = false;
		_keyBoardState._ctrl = false;
		_keyBoardState._shift = false;

		_keyBoardState._scancode = 0;
		_keyBoardState._error = 0;

		_keyBoardState._batFailed = false;
		_keyBoardState._diagnosticsFailed = false;
		_keyBoardState._disable = false;
		_keyBoardState._resendCmd = false;

		SetLeds(false, false, false);
	}

//Ű���� ���ͷ�Ʈ �ڵ鷯
	void ProcessKeyBoardInterrupt()
	{
		static bool _extended = false;
		uint8_t code = 0;

		//���ۿ� Ű�Է� �����Ͱ� ���Դ��� Ȯ���Ѵ�.
		if (ReadStatus() & KYBRD_CTRL_STATS_MASK_OUT_BUF)
		{

			//��ĵ�ڵ带 �о���δ�.
			code = ReadEncodeBuffer();

			//Ȯ�� ��ĵ �ڵ�� �ٷ��� �ʴ´�.
			if (code == 0xE0 || code == 0xE1)
				_extended = true;
			else 
			{
				_extended = false;

				//�� ���� ���̸� Ű���� ��ȿȭ��Ų��.
				//�Էµ� ��ĵ�ڵ尪�� �÷��׿� �����Ѵ�.
				if (code & 0x80) //0b10000000
				{	

					code -= 0x80;
					
					int key = KeyboardScanCode[code];
					switch (key) {

					case KEY_LCTRL:
					case KEY_RCTRL:
						_keyBoardState._ctrl = false;
						break;

					case KEY_LSHIFT:
					case KEY_RSHIFT:
						_keyBoardState._shift = false;
						break;

					case KEY_LALT:
					case KEY_RALT:
						_keyBoardState._alt = false;
						break;
					}
				}
				else 
				{

					

					//Ư��Ű�� ���������� Ȯ���Ѵ�.
					int key = KeyboardScanCode[code];

					switch (key) {

					case KEY_LCTRL:
					case KEY_RCTRL:
						_keyBoardState._ctrl = true;
						break;

					case KEY_LSHIFT:
					case KEY_RSHIFT:
						_keyBoardState._shift = true;
						break;

					case KEY_LALT:
					case KEY_RALT:
						_keyBoardState._alt = true;
						break;

					case KEY_CAPSLOCK:
						_keyBoardState._capslock = (_keyBoardState._capslock) ? false : true;
						SetLeds(_keyBoardState._numlock, _keyBoardState._capslock, _keyBoardState._scrolllock);
						break;

					case KEY_KP_NUMLOCK:
						_keyBoardState._numlock = (_keyBoardState._numlock) ? false : true;
						SetLeds(_keyBoardState._numlock, _keyBoardState._capslock, _keyBoardState._scrolllock);
						break;

					case KEY_SCROLLLOCK:
						_keyBoardState._scrolllock = (_keyBoardState._scrolllock) ? false : true;
						SetLeds(_keyBoardState._numlock, _keyBoardState._capslock, _keyBoardState._scrolllock);
						break;
					default:
						//��ĵ �ڵ� ����
						_keyBoardState._scancode = code;

					}
				}
			}

			//������ �߻������� �÷��׿� ����
			switch (code)
			{

			case KYBRD_ERR_BAT_FAILED:
				_keyBoardState._batFailed = true;
				break;

			case KYBRD_ERR_DIAG_FAILED:
				_keyBoardState._diagnosticsFailed = true;
				break;

			case KYBRD_ERR_RESEND_CMD:
				_keyBoardState._resendCmd = true;
				break;
			}
		}
	}
}