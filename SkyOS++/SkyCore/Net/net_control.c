/*
** Copyright 2003, Travis Geiselbrecht. All rights reserved.
** Distributed under the terms of the NewOS License.
*/
#include "windef.h"
#include "math.H"
#include "syserror.h"
#include "memory.h"
//#include <kernel/debug.h>
#include "kheap.h"
//#include <kernel/vm.h>
//#include <kernel/fs/devfs.h>
#include "net.h"
#include "net_control.h"
#include "net_timer.h"
#include "if.h"
#include "ethernet.h"
#include "loopback.h"
#include "arp.h"
#include "ipv4.h"
#include "udp.h"
#include "tcp.h"
#include "socket.h"
#include "misc.h"
#include <string.h>

#define NET_CONTROL_TRACE 0

/*#if NET_CONTROL_TRACE > 0
#define TRACE(x...) dprintf("net_control: " x)
#else
#define TRACE(x...)
#endif*/

typedef void * dev_ident;
typedef void * dev_cookie;

typedef enum {
	_SEEK_SET = 0,
	_SEEK_CUR,
	_SEEK_END

} seek_type;
typedef struct iovec {
	void *start;
	size_t len;
} iovec;

typedef struct iovecs {
	size_t num;
	size_t total_len;
	iovec vec[0];
} iovecs;

struct dev_calls {
	int(*dev_open)(dev_ident ident, dev_cookie *cookie);
	int(*dev_close)(dev_cookie cookie);
	int(*dev_freecookie)(dev_cookie cookie);

	int(*dev_seek)(dev_cookie cookie, off_t pos, seek_type st);
	int(*dev_ioctl)(dev_cookie cookie, int op, void *buf, size_t len);

	ssize_t(*dev_read)(dev_cookie cookie, void *buf, off_t pos, ssize_t len);
	ssize_t(*dev_write)(dev_cookie cookie, const void *buf, off_t pos, ssize_t len);

	int(*dev_canpage)(dev_ident ident);
	ssize_t(*dev_readpage)(dev_ident ident, iovecs *vecs, off_t pos);
	ssize_t(*dev_writepage)(dev_ident ident, iovecs *vecs, off_t pos);
};

static int net_control_dev_open(dev_ident ident, dev_cookie *cookie)
{
	*cookie = NULL;

	return 0;
}

static int net_control_dev_close(dev_cookie cookie)
{
	return 0;
}

static int net_control_dev_freecookie(dev_cookie cookie)
{
	return 0;
}

static int net_control_dev_seek(dev_cookie cookie, off_t pos, seek_type st)
{
	return ERR_NOT_ALLOWED;
}

static int net_control_dev_ioctl(dev_cookie cookie, int op, void *buf, size_t len)
{
	int err;

	union {
		struct _ioctl_net_if_control_struct if_control;
		struct _ioctl_net_route_struct route_control;
	} u;

//	TRACE("ioctl: op %d, buf %p, len %ld\n", op, buf, len);

	/* copy the args over */
	switch(op) {
		case IOCTL_NET_CONTROL_IF_CREATE:
		case IOCTL_NET_CONTROL_IF_DELETE:
		case IOCTL_NET_CONTROL_IF_ADDADDR:
		case IOCTL_NET_CONTROL_IF_RMADDR:
			// copy the interface args over
			err = user_memcpy(&u.if_control, buf, sizeof(u.if_control));
			if(err < 0)
				goto out;

//			TRACE("ioctl: args name '%s', if_addr 0x%x, mask_addr 0x%x, b_addr 0x%x\n",
			//	u.if_control.if_name, NETADDR_TO_IPV4(u.if_control.if_addr),
			//	NETADDR_TO_IPV4(u.if_control.mask_addr), NETADDR_TO_IPV4(u.if_control.broadcast_addr));
			break;
		case IOCTL_NET_CONTROL_ROUTE_ADD:
		case IOCTL_NET_CONTROL_ROUTE_DELETE:
			// copy the route args over
			err = user_memcpy(&u.route_control, buf, sizeof(u.route_control));
			if(err < 0)
				goto out;

//			TRACE("ioctl: args name '%s', if_addr 0x%x, mask_addr 0x%x, net_addr 0x%x\n",
		//		u.route_control.if_name, NETADDR_TO_IPV4(u.route_control.if_addr),
		//		NETADDR_TO_IPV4(u.route_control.mask_addr), NETADDR_TO_IPV4(u.route_control.net_addr));
			break;
	}

	/* do the operation */
	switch(op) {
		case IOCTL_NET_CONTROL_IF_CREATE: {
			ifnet *i;

			err = if_register_interface(u.if_control.if_name, &i);
			if(err < 0)
				goto out;

			err = NO_ERROR;
			break;
		}
		case IOCTL_NET_CONTROL_IF_DELETE:
			err = ERR_UNIMPLEMENTED;
			break;
		case IOCTL_NET_CONTROL_IF_ADDADDR: {
			ifnet *i;
			ifaddr *address;

			/* look up the structure describing the interface */
			i = if_path_to_ifnet(u.if_control.if_name);
			if(i == NULL) {
				err = ERR_NOT_FOUND;
				goto out;
			}

			/* set up the address on the interface */
			address = (ifaddr *)kmalloc(sizeof(ifaddr));
			if(address == NULL) {
				err = ERR_NO_MEMORY;
				goto out;
			}
			memcpy(&address->addr, &u.if_control.if_addr, sizeof(address->addr));
			memcpy(&address->netmask, &u.if_control.mask_addr, sizeof(address->netmask));
			memcpy(&address->broadcast, &u.if_control.broadcast_addr, sizeof(address->broadcast));
 			if_bind_address(i, address);

			err = NO_ERROR;
			break;
		}
		case IOCTL_NET_CONTROL_IF_RMADDR:
		case IOCTL_NET_CONTROL_IF_LIST:
			err = ERR_UNIMPLEMENTED;
			break;
		case IOCTL_NET_CONTROL_ROUTE_ADD: {

			/* see what type of network we are dealing with here */
			if(u.route_control.net_addr.type == ADDR_TYPE_IP) {
				ifnet *i;

				i = if_path_to_ifnet(u.route_control.if_name);
				if(i == NULL) {
					err = ERR_NET_BAD_ADDRESS;
					goto out;
				}

				if(NETADDR_TO_IPV4(u.route_control.mask_addr) == 0) {
					/* this is a default gateway route */
					ipv4_route_add_gateway(0, 0,
						NETADDR_TO_IPV4(u.route_control.if_addr), i->id,
						NETADDR_TO_IPV4(u.route_control.net_addr));
				} else {
					/* regular ol' route */
					ipv4_route_add(NETADDR_TO_IPV4(u.route_control.net_addr),
						NETADDR_TO_IPV4(u.route_control.mask_addr),
						NETADDR_TO_IPV4(u.route_control.if_addr), i->id);
				}
			} else {
				err = ERR_UNIMPLEMENTED;
				break;
			}

			err = NO_ERROR;
			break;
		}
		case IOCTL_NET_CONTROL_ROUTE_DELETE:
		case IOCTL_NET_CONTROL_ROUTE_LIST:
			err = ERR_UNIMPLEMENTED;
			break;
 		default:
			err = ERR_INVALID_ARGS;
	}

out:
	return err;
}

static ssize_t net_control_dev_read(dev_cookie cookie, void *buf, off_t pos, ssize_t len)
{
	return ERR_NOT_ALLOWED;
}

static ssize_t net_control_dev_write(dev_cookie cookie, const void *buf, off_t pos, ssize_t len)
{
	return ERR_NOT_ALLOWED;
}

static struct dev_calls net_control_dev_hooks = {
	&net_control_dev_open,
	&net_control_dev_close,
	&net_control_dev_freecookie,
	&net_control_dev_seek,
	&net_control_dev_ioctl,
	&net_control_dev_read,
	&net_control_dev_write,
	NULL,
	NULL,
	NULL
};

int net_control_init(void)
{
	// create device node
	devfs_publish_device("net/ctrl", NULL, &net_control_dev_hooks);

	return 0;
}

