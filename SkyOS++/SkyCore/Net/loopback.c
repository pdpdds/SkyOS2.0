/*
** Copyright 2001, Travis Geiselbrecht. All rights reserved.
** Distributed under the terms of the NewOS License.
*/
#include "windef.h"
#include "memory.h"
#include "syserror.h"
//#include <kernel/debug.h>
#include "cbuf.h"
#include "kheap.h"
#include "loopback.h"
#include "ethernet.h"
#include "ipv4.h"
#include "arp.h"

static int _loopback_input(cbuf *buf, ifnet *i, int protocol_type)
{
	int err;

	switch(protocol_type) {
		case PROT_TYPE_IPV4:
			err = ipv4_input(buf, i);
			break;
		case PROT_TYPE_ARP:
			err = arp_input(buf, i);
			break;
		default:
			err = -1;
	}

	return err;
}

int loopback_input(cbuf *buf, ifnet *i)
{
	// What? you can't call this directly
	cbuf_free_chain(buf);
	return NO_ERROR;
}

int loopback_output(cbuf *buf, ifnet *i, netaddr *target, int protocol_type)
{
	_loopback_input(buf, i, protocol_type);

	return NO_ERROR;
}

int loopback_init(void)
{
	ifnet *i;
	ifaddr *address;
	int err;

	// set up an initial device
	err = if_register_interface("loopback", &i);
	if(err < 0)
		return err;

	address = (ifaddr *)kmalloc(sizeof(ifaddr));
	address->addr.type = ADDR_TYPE_NULL;
	address->broadcast.type = ADDR_TYPE_NULL;
	address->netmask.type = ADDR_TYPE_NULL;
	if_bind_link_address(i, address);

	// set the ip address for this net interface
	address = (ifaddr *)kmalloc(sizeof(ifaddr));
	address->addr.len = 4;
	address->addr.type = ADDR_TYPE_IP;
	NETADDR_TO_IPV4(address->addr) = 0x7f000001; // 127.0.0.1
	address->netmask.len = 4;
	address->netmask.type = ADDR_TYPE_IP;
	NETADDR_TO_IPV4(address->netmask) = 0xff000000; // 255.255.255.0
	address->broadcast.len = 4;
	address->broadcast.type = ADDR_TYPE_IP;
	NETADDR_TO_IPV4(address->broadcast) = 0x7fffffff; // 127.255.255.255
	if_bind_address(i, address);

	// set up an initial routing table
	ipv4_route_add(0x7f000000, 0xff000000, 0x7f000001, i->id);

	return 0;
}

