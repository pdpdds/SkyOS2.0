#include "./pci/pci_private.h"
#include "Errors.h"
#include "Hal.h"


status_t
pci_io_init()
{
	// nothing to do on x86 hardware
	return B_OK;
}


uint8
pci_read_io_8(int mapped_io_addr)
{
	return InPortByte(mapped_io_addr);
}


void
pci_write_io_8(int mapped_io_addr, uint8 value)
{
	OutPortByte(value, mapped_io_addr);
}


uint16
pci_read_io_16(int mapped_io_addr)
{
	return InPortWord(mapped_io_addr);
}


void
pci_write_io_16(int mapped_io_addr, uint16 value)
{
	OutPortWord(value, mapped_io_addr);
}


uint32
pci_read_io_32(int mapped_io_addr)
{
	return InPortDWord(mapped_io_addr);
}


void
pci_write_io_32(int mapped_io_addr, uint32 value)
{
	OutPortDWord(value, mapped_io_addr);
}
