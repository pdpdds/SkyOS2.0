#include "pci_arch_info.h"

void
pci_read_arch_info(PCIDev *dev)
{
	pci_read_msi_info(dev);
	pci_read_msix_info(dev);
	pci_read_ht_mapping_info(dev);
}
