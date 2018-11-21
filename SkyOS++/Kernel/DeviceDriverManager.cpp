#include "DeviceDriverManager.h"
#include "SkyOS.h"
#include "Hal.h"

DeviceDriverManager* DeviceDriverManager::m_pDeviceDriverManager = nullptr;

DeviceDriverManager::DeviceDriverManager()
{
}

DeviceDriverManager::~DeviceDriverManager()
{
}

/* This function fills the PCICfg structure using the configuration method 1
*/
BYTE DeviceDriverManager::GetPCIConfigurationSpace(BYTE Bus, BYTE Device, BYTE Function, struct PCIConfigurationSpace * PCICfg)
{
	int Reg;
	int First = 1;
	for (Reg = 0; Reg < sizeof(struct PCIConfigurationSpace) / sizeof(DWORD); Reg++)
	{
		DWORD Addr, Value;
		Addr = 0x80000000L |
			(((DWORD)Bus) << 16) |
			(((DWORD)(Device << 3) | Function) << 8) |
			(Reg << 2);

		OutPortDWord(PCI_CONFIG_ADDRESS, Addr);

		Value = InPortDWord(PCI_CONFIG_DATA);

		Addr = InPortDWord(PCI_CONFIG_ADDRESS);
		((DWORD *)PCICfg)[Reg] = Value;
		OutPortDWord(PCI_CONFIG_ADDRESS, 0);
		if (First)
		{
			if (Value == 0xFFFFFFFF)
				return 0;
			First = 0;
		}
	}
	return Reg;
}

UINT16 DeviceDriverManager::InitPCIDevices()
{
	UINT16 TotDev = 0;
	BYTE Bus, Device, Function;
	struct PCIConfigurationSpace PCICfg;
	struct PCIDeviceDetails *newPCIDev;

	m_listPCIDevices = new list<struct PCIDeviceDetails *>();

	for (Bus = 0; Bus < 0xFF; Bus++)
		for (Device = 0; Device < 32; Device++)
			for (Function = 0; Function < 8; Function++)
			{
				BYTE Ret = GetPCIConfigurationSpace(Bus, Device, Function, &PCICfg);
				if (!Ret)
					continue;
				if (!(PCICfg.VendorID == 0xFFFF || PCICfg.DeviceID == 0xFFFFFFFF))
				{
					newPCIDev = new PCIDeviceDetails;
					newPCIDev->Bus = Bus;
					newPCIDev->Device = Device;
					newPCIDev->Function = Function;
					memcpy(&newPCIDev->PCIConfDetails, &PCICfg, sizeof(struct PCIConfigurationSpace));
					m_listPCIDevices->push_back(newPCIDev);
					TotDev++;
				}
			}
	return TotDev;
}

void DeviceDriverManager::EnumeratePCIDevices(BYTE(*CallBackFn) (PCIConfigurationSpace *))
{
	list<struct PCIDeviceDetails *>::iterator iter = m_listPCIDevices->begin();
	for (; iter != m_listPCIDevices->end(); iter++)
	{
		PCIConfigurationSpace* pPCIDet = &(*iter)->PCIConfDetails;

		if (nullptr != pPCIDet)
		{
			CallBackFn(pPCIDet);
		}
	}
}

PCIDeviceDetails* DeviceDriverManager::FindPCIDevice(unsigned int venderId, unsigned int deviceId)
{
	auto iter = m_listPCIDevices->begin();
	for (; iter != m_listPCIDevices->end(); iter++)
	{
		PCIConfigurationSpace* pPCIDetails = &(*iter)->PCIConfDetails;

		if (pPCIDetails->DeviceID == deviceId && pPCIDetails->VendorID == deviceId)
		{

		}		
	}

	return nullptr;
}

BYTE PrintPCIDeviceList(PCIConfigurationSpace * ptrPCIDet)
{
	SkyConsole::Print("%x %x ", ptrPCIDet->VendorID, ptrPCIDet->DeviceID);
	BYTE SClCode = 0;
	if (ptrPCIDet->ClassCode < __PCI_MaxClass)
	{
		SkyConsole::Print(" %s", PCIClassDetails[ptrPCIDet->ClassCode].Description);
		if (ptrPCIDet->SubClass != 0)
		{
			struct PCISubClass * PCISubCl = PCIClassDetails[ptrPCIDet->ClassCode].SubClass;

			while (!(PCISubCl[SClCode].SubClassCode == 0 && PCISubCl[SClCode].Description == 0))
			{
				if (ptrPCIDet->SubClass == SClCode)

				{
					if (PCISubCl != NULL)
						SkyConsole::Print(" - %s\n", PCISubCl[SClCode].Description);
					else SkyConsole::Print("\n");
					break;
				}
				SClCode++;
			}
		}
		else SkyConsole::Print("\n");
	}

	return SClCode;
}

void DeviceDriverManager::RequestPCIList()
{
	if (SystemProfiler::GetInstance()->GetGlobalState()._pciDevices > 0)
	{
		SkyConsole::Print("Device Vendor Class SubClass\n");
		EnumeratePCIDevices(PrintPCIDeviceList);
	}
	else
		SkyConsole::Print("Device not detected\n");
}

bool DeviceDriverManager::AddDevice(string deviceName)
{
	if (strcmp(deviceName.c_str(), "es1370") == 0)
	{
		PCIDeviceDetails* pciInfo = FindPCIDevice(0x1274, 0x5000);

		if (pciInfo != nullptr)
			return true;		
	}

	return false;
}

bool DeviceDriverManager::IOInit()
{
	// nothing to do on x86 hardware
	return true;
}


uint8 DeviceDriverManager::ReadIOByte(int mapped_io_addr)
{
	return InPortByte(mapped_io_addr);
}


void DeviceDriverManager::WriteIOByte(int mapped_io_addr, uint8 value)
{
	OutPortByte(value, mapped_io_addr);
}


uint16 DeviceDriverManager::ReadIOWord(int mapped_io_addr)
{
	return InPortWord(mapped_io_addr);
}


void DeviceDriverManager::WriteIOWord(int mapped_io_addr, uint16 value)
{
	OutPortWord(value, mapped_io_addr);
}


uint32 DeviceDriverManager::ReadIODWord(int mapped_io_addr)
{
	return InPortDWord(mapped_io_addr);
}


void DeviceDriverManager::WrieIODWord(int mapped_io_addr, uint32 value)
{
	OutPortDWord(value, mapped_io_addr);
}
