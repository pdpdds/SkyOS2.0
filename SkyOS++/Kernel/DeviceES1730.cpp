#include "DeviceES1730.h"
#include "DeviceDriverManager.h"
#include "es1370/es1370reg.h"

DeviceES1730::DeviceES1730()
{
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

	SetupES1370();

	return true;
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

static int read_pci_config_data(PCICfg *pCfg, int nBus, int nDevFn)
{
	int		nI;
	DWORD	*pX;

	memset(pCfg, 0, sizeof(PCICfg));

	pX = (DWORD*)pCfg;
	for (nI = 0; nI < 16; nI++)
	{
	//	read_pci_port_dword(nBus, nDevFn, nI, &pX[nI]);
		if (nI == 0)
		{
			if (pX[0] == (DWORD)0xFFFFFFFF || pX[0] == 0 || pX[0] == (DWORD)0xFFFF0000 || pX[0] == (DWORD)0x0000FFFF)
				return(-1);
		}
	}

	//kdbg_printf( "bus(%d) devfn(%d), device id (%X), revision %d , class ( %d, %d, %d )\n", nBus, nDevFn, pCfg->wDeviceID, pCfg->byRevisionID, pCfg->class_code[0], pCfg->class_code[1], pCfg->class_code[2] );

	return(0);
}

bool DeviceES1730::SetupES1370()
{	
	uint8	bus = m_pDev->Bus;		//Bus Number
	uint8	device = m_pDev->Device;		//Device Numbeer On Bus
	uint8	function;	/* function # in device */
	uint16	offset;		/* offset in configuration space */
	uint8	size;		/* # bytes to read (1, 2 or 4) */

	/*PCICfg cfg;
	uint8 cmd = read_pci_config_data(&cfg, m_pDev->Bus, m_pDev->Function);
	printf("PCI command before: %x\n", cmd);
	(*pci->write_pci_config)(card->info.bus, card->info.device, card->info.function, PCI_command, 2, cmd | PCI_command_io);
	cmd = (*pci->read_pci_config)(card->info.bus, card->info.device, card->info.function, PCI_command, 2);
	printf("PCI command after: %x\n", cmd);*/

	es1370_reg_write_32(ES1370_REG_SERIAL_CONTROL, SCTRL_P2INTEN | SCTRL_R1INTEN);
	es1370_reg_write_32(ES1370_REG_CONTROL, CTRL_CDC_EN);

	return true;
}



void DeviceES1730::es1370_reg_write_32(int regno, uint32 value)
{	
	DeviceDriverManager::GetInstance()->WrieIODWord(m_pDev->PCIConfDetails.PreDefinedHeader.Bridge.BaseAddress0 + regno, value);
}


/*
	
	printf("codec reset\n");
	es1370_codec_write(&card->config, CODEC_RESET_PWRDWN, 0x2);
	//snooze (20);
	es1370_codec_write(&card->config, CODEC_RESET_PWRDWN, 0x3);
	//snooze (20);
	es1370_codec_write(&card->config, CODEC_CLOCK_SEL, 0x0);

	
	es1370_codec_write(&card->config, CODEC_MASTER_VOL_L, 0x0);
	es1370_codec_write(&card->config, CODEC_MASTER_VOL_R, 0x0);
	es1370_codec_write(&card->config, CODEC_VOICE_VOL_L, 0x0);
	es1370_codec_write(&card->config, CODEC_VOICE_VOL_R, 0x0);

	
	es1370_codec_write(&card->config, CODEC_OUTPUT_MIX1, ES1370_OUTPUT_MIX1_CDL | ES1370_OUTPUT_MIX1_CDR);
	
	es1370_codec_write(&card->config, CODEC_OUTPUT_MIX2, ES1370_OUTPUT_MIX2_VOICEL | ES1370_OUTPUT_MIX2_VOICER);
	*/
	//snooze(50000); // 50 ms
	/*
	printf("installing interrupt : %lx\n", card->config.irq);
	//err = install_io_interrupt_handler(card->config.irq, es1370_int, card, 0);
	if (err != B_OK) {
	printf("failed to install interrupt\n");
	return err;
	}

	if ((err = es1370_init(card)))
	return (err);

	printf("init_driver done\n");

	return err;
}*/