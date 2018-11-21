#pragma once
#include "kheap.h"
#include "map.h"
#include "stl/list.h"
#include "stl_string.h"
#include "PCI.h"
#include <IDeviceDriver.h>

class DeviceDriverManager
{
public:
	DeviceDriverManager();
	~DeviceDriverManager();

	BYTE GetPCIConfigurationSpace(BYTE Bus, BYTE Device, BYTE Function, struct PCIConfigurationSpace * PCICfg);
	UINT16 InitPCIDevices();
	void EnumeratePCIDevices(BYTE(*CallBackFn) (struct PCIConfigurationSpace *));	
	VOID RequestPCIList();	
	PCIDeviceDetails* FindPCIDevice(unsigned int venderId, unsigned int deviceId);

	bool AddDevice(string deviceName);

	static DeviceDriverManager* GetInstance()
	{
		if (m_pDeviceDriverManager == nullptr)
			m_pDeviceDriverManager = new DeviceDriverManager();

		return m_pDeviceDriverManager;
	}

	bool IOInit();
	

	uint8 ReadIOByte(int mapped_io_addr);
	void WriteIOByte(int mapped_io_addr, uint8 value);
	uint16 ReadIOWord(int mapped_io_addr);
	void WriteIOWord(int mapped_io_addr, uint16 value);
	uint32 ReadIODWord(int mapped_io_addr);
	void WrieIODWord(int mapped_io_addr, uint32 value);

private:
	static DeviceDriverManager* m_pDeviceDriverManager;
	map<string, IDeviceDriver*> m_mapDeviceDriver;
	list<PCIDeviceDetails *>* m_listPCIDevices;
};