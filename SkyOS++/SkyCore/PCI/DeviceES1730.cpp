#include "DeviceES1730.h"
#include "DeviceDriverManager.h"
#include "audio/es1370/es1370reg.h"
#include "SystemAPI.h"
#include "intrinsic.h"
#include "IDT.h"

extern void SendEOI();

DeviceES1730::DeviceES1730()
{
	m_interrupt_mask = 0;
}


DeviceES1730::~DeviceES1730()
{
}

bool DeviceES1730::InitDriver()
{	
	PCIDeviceDetails* pciInfo = DeviceDriverManager::GetInstance()->FindPCIDevice(0x1274, 0x5000);

	if (pciInfo == nullptr)
		return false;

	m_pDev = pciInfo;

	return SetupES1370();
}

typedef struct tag_PCICfg
{
	UINT16  wVendorID;
	UINT16	wDeviceID;
	UINT16  wCmdRegister;
	UINT16  wStatusRegister;
	UCHAR	byRevisionID;
	UCHAR	class_code[3];
	UCHAR	byCacheLineSize;
	UCHAR	byLatencyTimer;
	UCHAR	byHeaderType;
	UCHAR	byBIST;
	DWORD	base_addr[6];
	DWORD	dwCISPointer;
	UINT16	wSubVendorID;
	UINT16	wSubSystemID;
	DWORD	dwRomBaseAddr;
	UCHAR	byCapabilityBase;
	UCHAR	rsv0[3];
	UCHAR	rsv1[4];
	UCHAR	byIntLine;
	UCHAR	byIntPin;
	UCHAR	byMinGnt;
	UCHAR	byMaxLat;
}PCICfg;

static int read_pci_port_dword(int nBus, int nDevFn, int nIndex, DWORD *pDword)
{
	DWORD dwX;

	dwX = (DWORD)((DWORD)0x80000000 | (DWORD)(nBus << 16) | (DWORD)(nDevFn << 8) |
		(DWORD)(nIndex << 2));

	// write pci address register
	OutPortDWord((DWORD)0xCF8, dwX);

	// read pci data register
	*pDword = (DWORD)InPortDWord((DWORD)0xCFC);

	return(0);
}

static int read_pci_config_data(PCICfg *pCfg, int nBus, int nDevFn)
{
	int		nI;
	DWORD	*pX;

	memset(pCfg, 0, sizeof(PCICfg));

	pX = (DWORD*)pCfg;
	for (nI = 0; nI < 16; nI++)
	{
		read_pci_port_dword(nBus, nDevFn, nI, &pX[nI]);
		if (nI == 0)
		{
			if (pX[0] == (DWORD)0xFFFFFFFF || pX[0] == 0 || pX[0] == (DWORD)0xFFFF0000 || pX[0] == (DWORD)0x0000FFFF)
				return(-1);
		}
	}

	//kdbg_printf( "bus(%d) devfn(%d), device id (%X), revision %d , class ( %d, %d, %d )\n", nBus, nDevFn, pCfg->wDeviceID, pCfg->byRevisionID, pCfg->class_code[0], pCfg->class_code[1], pCfg->class_code[2] );

	return(0);
}


__declspec(naked) void es1370_int()
{
	//레지스터를 저장하고 인터럽트를 끈다.
	_asm
	{
		PUSHAD
		PUSHFD
		CLI
	}

	// 스택상태가 변경되는 것을 막기 위해 함수를 호출한다. 
	//_asm call KeyboardController::HandleKeyboardInterrupt
	printf("es1370 interrupt!!!!!!!!!!!!!!!!\n");

	SendEOI();

	// 레지스터를 복원하고 원래 수행하던 곳으로 돌아간다.
	_asm
	{
		POPFD
		POPAD
		IRETD
	}
}

bool DeviceES1730::SetupES1370()
{	
	uint8	bus = m_pDev->Bus;		//Bus Number
	uint8	device = m_pDev->Device;		//Device Numbeer On Bus
	uint8	function = m_pDev->Function;	/* function # in device */
	uint8   irq = m_pDev->PCIConfDetails.PreDefinedHeader.NonBridge.InterruptLine;
	uint16	offset;		/* offset in configuration space */
	uint8	size;		/* # bytes to read (1, 2 or 4) */

	es1370_reg_write_32(ES1370_REG_SERIAL_CONTROL, SCTRL_P2INTEN | SCTRL_R1INTEN);
	es1370_reg_write_32(ES1370_REG_CONTROL, CTRL_CDC_EN);
	
	/* reset the codec */	
	es1370_codec_write(CODEC_RESET_PWRDWN, 0x2);
	sleep (20);
	es1370_codec_write(CODEC_RESET_PWRDWN, 0x3);
	sleep(20);
	es1370_codec_write(CODEC_CLOCK_SEL, 0x0);

	// set max volume on master and mixer outputs 
	es1370_codec_write(CODEC_MASTER_VOL_L, 0x0);
	es1370_codec_write(CODEC_MASTER_VOL_R, 0x0);
	es1370_codec_write(CODEC_VOICE_VOL_L, 0x0);
	es1370_codec_write(CODEC_VOICE_VOL_R, 0x0);

	// unmute CD playback 
	es1370_codec_write(CODEC_OUTPUT_MIX1, ES1370_OUTPUT_MIX1_CDL | ES1370_OUTPUT_MIX1_CDR);
	// unmute mixer output
	es1370_codec_write(CODEC_OUTPUT_MIX2, ES1370_OUTPUT_MIX2_VOICEL | ES1370_OUTPUT_MIX2_VOICER);
	
	SetInterruptVector(32+irq, es1370_int);

	sleep(50000); // 50 ms
	/*
	PRINT(("installing interrupt : %lx\n", card->config.irq));
	err = install_io_interrupt_handler(card->config.irq, es1370_int, card, 0);
	if (err != B_OK) {
		PRINT(("failed to install interrupt\n"));
		return err;
	}*/

	if (false == es1370_init())
		return false;

	printf("init_driver done. irq %d\n", irq);	
	uint32 sctrl = 0, ctrl = 0;
	sctrl = es1370_reg_read_32(ES1370_REG_SERIAL_CONTROL);
	ctrl = es1370_reg_read_32(ES1370_REG_CONTROL);

	printf("%d %d\n", sctrl, ctrl);

	es1370_reg_write_32(ES1370_REG_SERIAL_CONTROL, sctrl & ~SCTRL_R1INTEN);
	es1370_reg_write_32(ES1370_REG_SERIAL_CONTROL, sctrl | SCTRL_R1INTEN);

	es1370_reg_write_32(ES1370_REG_SERIAL_CONTROL, sctrl);
	es1370_reg_write_32( ES1370_REG_CONTROL, ctrl);

	return true;
}


bool DeviceES1730::es1370_init()
{
	m_interrupt_mask = STAT_DAC2 | STAT_ADC;

	/* Init streams list */
	//LIST_INIT(&(card->streams));

	/* Init mems list */
	//LIST_INIT(&(card->mems));

	return true;
}

void DeviceES1730::es1370_codec_write(int regno, uint16 value)
{
	//ASSERT(regno >= 0);
	if (es1370_codec_wait() != true) {
		printf("codec busy (4)\n");
		return;
	}
	DeviceDriverManager::GetInstance()->WrieIODWord(ES1370_REG_CODEC, (regno << 8) | value);
}



bool DeviceES1730::es1370_codec_wait()
{
	for (int i = 0; i < 1100; i++) {
		if ((es1370_reg_read_32(ES1370_REG_STATUS) & STAT_CWRIP) == 0)
			return true;		
	}	

	return false;
}

DWORD DeviceES1730::es1370_reg_read_32(int regno)
{
	return DeviceDriverManager::GetInstance()->ReadIODWord(regno);
}

void DeviceES1730::es1370_reg_write_32(int regno, uint32 value)
{	
	DeviceDriverManager::GetInstance()->WrieIODWord(regno, value);
}