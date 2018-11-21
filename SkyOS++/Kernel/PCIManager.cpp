#include "PCIManager.h"
#include "Hal.h"
#include "SkyConsole.h"
#include "SkyAPI.h"
#include "haiku_pci.h"
#include "Errors.h"
#include "pci_irq.h"
#include "pci_info.h"
#include "es1370\es1370.h"

#define PCI_MECH1_REQ_PORT				0xCF8
#define PCI_MECH1_DATA_PORT 			0xCFC
#define PCI_MECH1_REQ_DATA(bus, device, func, offset) \
	(0x80000000 | (bus << 16) | (device << 11) | (func << 8) | (offset & ~3))

#define PCI_MECH2_ENABLE_PORT			0x0cf8
#define PCI_MECH2_FORWARD_PORT			0x0cfa
#define PCI_MECH2_CONFIG_PORT(dev, offset) \
	(uint16)(0xC00 | (dev << 8) | offset)


static status_t
pci_mech1_read_config(void *cookie, uint8 bus, uint8 device, uint8 function,
	uint16 offset, uint8 size, uint32 *value)
{
	cpu_status cpu;
	status_t status = B_OK;

	if (offset > 0xff)
		return B_BAD_VALUE;

	kEnterCriticalSection();
	//PCI_LOCK_CONFIG(cpu);
	OutPortDWord(PCI_MECH1_REQ_PORT, PCI_MECH1_REQ_DATA(bus, device, function, offset));
	switch (size) {
	case 1:
		*value = InPortByte(PCI_MECH1_DATA_PORT + (offset & 3));
		break;
	case 2:
		*value = InPortWord(PCI_MECH1_DATA_PORT + (offset & 3));
		break;
	case 4:
		*value = InPortDWord(PCI_MECH1_DATA_PORT);
		break;
	default:
		status = B_ERROR;
		break;
	}
	kLeaveCriticalSection();
	//PCI_UNLOCK_CONFIG(cpu);

	return status;
}


static status_t
pci_mech1_write_config(void *cookie, uint8 bus, uint8 device, uint8 function,
	uint16 offset, uint8 size, uint32 value)
{
	cpu_status cpu;
	status_t status = B_OK;

	if (offset > 0xff)
		return B_BAD_VALUE;

	kEnterCriticalSection();
	//PCI_LOCK_CONFIG(cpu);
	OutPortDWord(PCI_MECH1_REQ_PORT, PCI_MECH1_REQ_DATA(bus, device, function, offset));
	switch (size) {
	case 1:
		OutPortByte(PCI_MECH1_DATA_PORT + (offset & 3), value);
		break;
	case 2:
		OutPortWord(PCI_MECH1_DATA_PORT + (offset & 3), value);
		break;
	case 4:
		OutPortDWord(PCI_MECH1_DATA_PORT, value);
		break;
	default:
		status = B_ERROR;
		break;
	}
	kLeaveCriticalSection();
	//PCI_UNLOCK_CONFIG(cpu);

	return status;
}

static status_t
pci_mech1_get_max_bus_devices(void *cookie, int32 *count)
{
	*count = 32;
	return B_OK;
}


static status_t
pci_mech2_read_config(void *cookie, uint8 bus, uint8 device, uint8 function,
	uint16 offset, uint8 size, uint32 *value)
{
	cpu_status cpu;
	status_t status = B_OK;
	
	if (offset > 0xff)
		return B_BAD_VALUE;

	

	kEnterCriticalSection();
	//PCI_LOCK_CONFIG(cpu);
	OutPortByte(PCI_MECH2_ENABLE_PORT, (uint8)(0xf0 | (function << 1)));
	OutPortByte(PCI_MECH2_FORWARD_PORT, bus);
	switch (size) {
	case 1:
		*value = InPortByte(PCI_MECH2_CONFIG_PORT(device, offset));
		break;
	case 2:
		*value = InPortWord(PCI_MECH2_CONFIG_PORT(device, offset));
		break;
	case 4:
		*value = InPortDWord(PCI_MECH2_CONFIG_PORT(device, offset));
		break;
	default:
		status = B_ERROR;
		TRACE("SDFDFSFD %d %d\n", status, *value);
		break;
	}
	OutPortByte(PCI_MECH2_ENABLE_PORT, 0);
	//PCI_UNLOCK_CONFIG(cpu);
	kLeaveCriticalSection();
	
	return status;
}

static status_t
pci_mech2_write_config(void *cookie, uint8 bus, uint8 device, uint8 function,
	uint16 offset, uint8 size, uint32 value)
{
	cpu_status cpu;
	status_t status = B_OK;

	if (offset > 0xff)
		return B_BAD_VALUE;

	kEnterCriticalSection();
	//PCI_LOCK_CONFIG(cpu);
	OutPortByte(PCI_MECH2_ENABLE_PORT, (uint8)(0xf0 | (function << 1)));
	OutPortByte(PCI_MECH2_FORWARD_PORT, bus);
	switch (size) {
	case 1:
		OutPortByte(PCI_MECH2_CONFIG_PORT(device, offset), value);
		break;
	case 2:
		OutPortWord(PCI_MECH2_CONFIG_PORT(device, offset), value);
		break;
	case 4:
		OutPortDWord(PCI_MECH2_CONFIG_PORT(device, offset), value);
		break;
	default:
		status = B_ERROR;
		break;
	}
	OutPortByte(PCI_MECH2_ENABLE_PORT, 0);
	//PCI_UNLOCK_CONFIG(cpu);
	kLeaveCriticalSection();

	return status;
}


static status_t
pci_mech2_get_max_bus_devices(void *cookie, int32 *count)
{
	*count = 16;
	return B_OK;
}

pci_controller pci_controller_x86_mech1 =
{
	pci_mech1_read_config,
	pci_mech1_write_config,
	pci_mech1_get_max_bus_devices,
	pci_x86_irq_read,
	pci_x86_irq_write,
};

pci_controller pci_controller_x86_mech2 =
{
	pci_mech2_read_config,
	pci_mech2_write_config,
	pci_mech2_get_max_bus_devices,
	pci_x86_irq_read,
	pci_x86_irq_write,
};

PCIManager::PCIManager()
{
}


PCIManager::~PCIManager()
{
}

PCI *gPCI;
extern "C" status_t es1370_open(const char *name, uint32 flags, void** cookie);
bool PCIManager::Initialize()
{
	gPCI = new PCI;

	InitPCI();

	gPCI->InitDomainData();
	gPCI->InitBus();

	init_hardware();
	init_driver();

	es1370_open(0,0,0);
	//pci_print_info();

	return true;
}

bool PCIManager::InitPCI()
{
	bool search_mech1 = true;
	bool search_mech2 = true;
	bool search_mechpcie = true;

	/*if (search_mechpcie) 
	{
		acpi_init();
		struct acpi_table_mcfg* mcfg =
			(struct acpi_table_mcfg*)acpi_find_table("MCFG");
		if (mcfg != NULL) {
			struct acpi_mcfg_allocation* end = (struct acpi_mcfg_allocation*)
				((char*)mcfg + mcfg->Header.Length);
			struct acpi_mcfg_allocation* alloc = (struct acpi_mcfg_allocation*)
				(mcfg + 1);
			for (; alloc < end; alloc++) {
				//dprintf("PCI: mechanism addr: %" B_PRIx64 ", seg: %x, start: "
					"%x, end: %x\n", alloc->Address, alloc->PciSegment,
					alloc->StartBusNumber, alloc->EndBusNumber);
				if (alloc->PciSegment == 0) {
					area_id mcfgArea = map_physical_memory("acpi mcfg",
						alloc->Address, (alloc->EndBusNumber + 1) << 20,
						B_ANY_KERNEL_ADDRESS, B_KERNEL_READ_AREA
						| B_KERNEL_WRITE_AREA, (void **)&sPCIeBase);
					if (mcfgArea < 0)
						break;
					sStartBusNumber = alloc->StartBusNumber;
					sEndBusNumber = alloc->EndBusNumber;
					//dprintf("PCI: mechanism pcie controller found\n");
					return pci_controller_add(&pci_controller_x86_mechpcie,
						NULL);
				}
			}
		}
	}*/

	if (search_mech1) {
		// check for mechanism 1
		OutPortDWord(PCI_MECH1_REQ_PORT, 0x80000000);
		if (0x80000000 == InPortDWord(PCI_MECH1_REQ_PORT)) {
			TRACE("PCI: mechanism 1 controller found\n");
			return pci_controller_add(&pci_controller_x86_mech1, NULL);
		}
	}

	if (search_mech2) {
		// check for mechanism 2
		OutPortByte(0x00, 0xCFB);
		OutPortByte(0x00, 0xCF8);
		OutPortByte(0x00, 0xCFA);
		if (InPortByte(0xCF8) == 0x00 && InPortByte(0xCFA) == 0x00) {
			TRACE("PCI: mechanism 2 controller found\n");
			return pci_controller_add(&pci_controller_x86_mech2, NULL);
		}
	}

	TRACE("PCI: no configuration mechanism found\n");
	return false;
}