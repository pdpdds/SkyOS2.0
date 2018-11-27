#ifndef _PCI_ARCH_INFO_H
#define _PCI_ARCH_INFO_H

#include "pci_msi.h"

typedef struct pci_arch_info {
	msi_info	msi;
	msix_info	msix;
	ht_mapping_info	ht_mapping;
} pci_arch_info;


void pci_read_arch_info(PCIDev *device);

#endif // _PCI_ARCH_INFO_H
