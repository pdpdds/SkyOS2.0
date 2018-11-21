/*
 * Copyright 2006, Marcus Overhagen. All rights reserved.
 *
 * Distributed under the terms of the MIT License.
 */
#ifndef __PCI_X86_IRQ_H
#define __PCI_X86_IRQ_H

#include "windef.h"

status_t pci_x86_irq_init(void);

status_t pci_x86_irq_read(void *cookie,
						  uint8 bus, uint8 device, uint8 function, 
						  uint8 pin, uint8 *irq);

status_t pci_x86_irq_write(void *cookie,
						   uint8 bus, uint8 device, uint8 function, 
						   uint8 pin, uint8 irq);


#endif
