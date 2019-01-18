#pragma once
#include "IDeviceDriver.h"

struct PCIDeviceDetails;

enum PCIType
{
	NonBridge = 0,
	Bridge,
	PCCard
};

class DeviceES1730 : public IDeviceDriver
{
public:
	DeviceES1730();
	~DeviceES1730();

	virtual bool InitDriver() override;
	void es1370_reg_write_32(int regno, uint32 value);
	DWORD es1370_reg_read_32(int regno);

	bool es1370_codec_wait();
	void es1370_codec_write(int regno, uint16 value);

protected:
	bool SetupES1370();	

private:
	bool es1370_init();

private:
	PCIDeviceDetails* m_pDev;
	PCIType m_type;
	int m_interrupt_mask;
	
};

