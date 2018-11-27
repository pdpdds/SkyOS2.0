#ifndef _PCI_FIXUP_H
#define _PCI_FIXUP_H

class PCI;


void pci_fixup_device(PCI *pci, uint8 domain, uint8 bus, uint8 device,
	uint8 function);

#endif
