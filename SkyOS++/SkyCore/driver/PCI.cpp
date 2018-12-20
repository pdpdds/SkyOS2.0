// 
// Copyright 1998-2012 Jeff Bush
// 
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// 
//     http://www.apache.org/licenses/LICENSE-2.0
// 
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
// 

#include "cpu_asm.h"
#include "PCI.h"
#include "types.h"
#include "intrinsic.h"

#define CONFIG_ADDRESS 0xcf8
#define CONFIG_DATA 0xcfc

struct ConfigAddress {
	unsigned reg : 8,
		function : 3,
		unit : 5,
		bus : 8,
		reserved : 7,
		enable : 1;
} PACKED;

PCIInterface* PCIInterface::fDevices[255];
int PCIInterface::fDeviceCount = 0;

PCIInterface::PCIInterface()
{
}

PCIInterface::PCIInterface(int bus, int unit)
	:	fBus(bus),
		fUnit(unit)
{
}

PCIInterface* PCIInterface::QueryDevice(int basicClass, int subClass)
{
	if (fDeviceCount == 0)
		Scan();
		
	for (int i = 0; i < fDeviceCount; i++)
		if ((fDevices[i]->fClass >> 24) == basicClass
			&& ((fDevices[i]->fClass >> 16) & 0xff) == subClass)
			return fDevices[i];

	return 0;
}

unsigned PCIInterface::ReadConfig(int bus, int unit, int function, int reg, int bytes)
{
	ConfigAddress address;
	address.enable = 1;
	address.reserved = 0;
	address.bus = bus;
	address.unit = unit;
	address.function = function;
	address.reg = reg & 0xfc;
	
	OutPortDWord(CONFIG_ADDRESS, *(unsigned*)&address);
	switch (bytes) {
	case 1:
		return InPortByte(CONFIG_DATA + (reg & 3));
	case 2:
		return InPortWord(CONFIG_DATA + (reg & 3));
	case 4:
		return InPortDWord(CONFIG_DATA + (reg & 3));
	}

	return 0;
}

void PCIInterface::WriteConfig(int bus, int unit, int function, int reg, unsigned data,
	int bytes)
{
	ConfigAddress address;
	address.enable = 1;
	address.reserved = 0;
	address.bus = bus;
	address.unit = unit;
	address.function = function;
	address.reg = reg & 0xfc;
	
	OutPortDWord(*(unsigned*)&address, CONFIG_ADDRESS);
	switch (bytes) {
	case 1:
		OutPortByte(CONFIG_DATA + (reg & 3), data);
	case 2:
		OutPortWord(CONFIG_DATA + (reg & 3), data);
	case 4:
		OutPortDWord(CONFIG_DATA + (reg & 3), data);
	}
}

void PCIInterface::Scan()
{
	for (int bus = 0; bus < 255; bus++) {
		for (int unit = 0; unit < 32; unit++) {
			if (ReadConfig(bus, unit, 0, 0, 2) == 0xffff)
				continue;

			PCIInterface *interface = new PCIInterface(bus, unit);
			interface->fClass = ReadConfig(bus, unit, 0, 8, 4);
			fDevices[fDeviceCount++] = interface;
		}
	}
}

unsigned PCIInterface::ReadConfig(int reg)
{
	return ReadConfig(fBus, fUnit, 0, reg, 4);
}

void PCIInterface::WriteConfig(int reg, unsigned data)
{
	WriteConfig(fBus, fUnit, 0, reg, data, 4);
}


