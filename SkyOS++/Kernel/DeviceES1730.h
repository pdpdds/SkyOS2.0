#pragma once
#include <IDeviceDriver.h>

struct PCIDeviceDetails;

class DeviceES1730 : public IDeviceDriver
{
public:
	DeviceES1730();
	~DeviceES1730();

	virtual bool InitDriver() override;
	void es1370_reg_write_32(int regno, uint32 value);

protected:
	bool SetupES1370();	

private:
	PCIDeviceDetails* m_pDev;
	
};

