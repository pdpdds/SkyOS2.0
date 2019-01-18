/*
** Copyright 2001-2002, Travis Geiselbrecht. All rights reserved.
** Distributed under the terms of the NewOS License.
*/
#ifndef _NEWOS_KERNEL_NET_IF_H
#define _NEWOS_KERNEL_NET_IF_H

#include "windef.h"
#include "net.h"
#include "cbuf.h"
#include "net/queue2.h"
#include "net/lock.h"
#include "net.h"

typedef struct ifaddr {
	struct ifaddr *next;
	struct ifnet *if_owner;
	netaddr addr;
	netaddr netmask;
	netaddr broadcast;
} ifaddr;

enum {
	IF_TYPE_NULL = 0,
	IF_TYPE_LOOPBACK,
	IF_TYPE_ETHERNET
};

typedef int if_id;

typedef struct ifnet {
	struct ifnet *next;
	if_id id;
	char path[256];
	int type;
	int fd;
	thread_id rx_thread;
	thread_id tx_thread;
	ifaddr *addr_list;
	ifaddr *link_addr;
	size_t mtu;
	int (*link_input)(cbuf *buf, struct ifnet *i);
	int (*link_output)(cbuf *buf, struct ifnet *i, netaddr *target, int protocol_type);
	sem_id tx_queue_sem;
	mutex tx_queue_lock;
	fixed_queue tx_queue;
	uint8 tx_buf[2048];
	uint8 rx_buf[2048];
} ifnet;

int if_init(void);
ifnet *if_id_to_ifnet(if_id id);
ifnet *if_path_to_ifnet(const char *path);
int if_register_interface(const char *path, ifnet **i);
void if_bind_address(ifnet *i, ifaddr *addr);
void if_bind_link_address(ifnet *i, ifaddr *addr);
int if_boot_interface(ifnet *i);
int if_output(cbuf *b, ifnet *i);

#endif

