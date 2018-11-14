/*
** Copyright 2001, Travis Geiselbrecht. All rights reserved.
** Distributed under the terms of the NewOS License.
*/
#ifndef _RTL8139_PRIV_H
#define _RTL8139_PRIV_H

//#include <kernel/kernel.h>
//#include <kernel/vm.h>
//#include <kernel/smp.h>
#include "windef.h"
#include "Net/lock.h"

typedef struct rtl8139 {
	int irq;
	addr_t phys_base;
	addr_t phys_size;
	addr_t virt_base;
	uint16 io_port;
	region_id region;
	uint8 mac_addr[6];
	int txbn;
	int last_txbn;
	region_id rxbuf_region;
	addr_t rxbuf;
	sem_id rx_sem;
	region_id txbuf_region;
	addr_t txbuf;
	sem_id tx_sem;
	mutex lock;
	//spinlock_t reg_spinlock;
} rtl8139;

int rtl8139_detect(rtl8139 **rtl);
int rtl8139_init(rtl8139 *rtl);
void rtl8139_xmit(rtl8139 *rtl, const char *ptr, ssize_t len);
ssize_t rtl8139_rx(rtl8139 *rtl, char *buf, ssize_t buf_len);

#endif
