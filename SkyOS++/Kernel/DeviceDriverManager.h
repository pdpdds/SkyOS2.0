#pragma once
#include "kheap.h"
#include "map.h"
#include "list.h"
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

private:
	static DeviceDriverManager* m_pDeviceDriverManager;
	map<string, IDeviceDriver*> m_mapDeviceDriver;
	list<PCIDeviceDetails *>* m_listPCIDevices;
};

