/*
** Copyright 2001-2002, Travis Geiselbrecht. All rights reserved.
** Distributed under the terms of the NewOS License.
*/
#ifndef _NEWOS_KERNEL_NET_NET_H
#define _NEWOS_KERNEL_NET_NET_H

#include "windef.h"

enum {
	// called on partition device to get info
	IOCTL_DEVFS_GET_PARTITION_INFO = 1000,
	// called on raw device to declare one partition
	IOCTL_DEVFS_SET_PARTITION,

	// framebuffer ops
	IOCTL_DEVFS_GET_FRAMEBUFFER_INFO = 2000,
	IOCTL_DEVFS_MAP_FRAMEBUFFER,

	// network ops
	IOCTL_DEVFS_NETWORK_OPS_BASE = 3000,
};

typedef struct netaddr {
	uint8 len;
	uint8 type;
	uint8 pad0;
	uint8 pad1;
	uint8 addr[12];
} netaddr;

enum {
	SOCK_PROTO_NULL = 0,
	SOCK_PROTO_UDP,
	SOCK_PROTO_TCP
};

enum {
	ADDR_TYPE_NULL = 0,
	ADDR_TYPE_ETHERNET,
	ADDR_TYPE_IP
};

#define SOCK_FLAG_TIMEOUT 1

typedef struct sockaddr {
	netaddr addr;
	int port;
} sockaddr;

enum {
	IP_PROT_ICMP = 1,
	IP_PROT_TCP = 6,
	IP_PROT_UDP = 17,
};

typedef uint32 ipv4_addr;
#define NETADDR_TO_IPV4(naddr) (*(ipv4_addr *)(&((&(naddr))->addr[0])))
#define IPV4_DOTADDR_TO_ADDR(a, b, c, d) \
	(((ipv4_addr)(a) << 24) | (((ipv4_addr)(b) & 0xff) << 16) | (((ipv4_addr)(c) & 0xff) << 8) | ((ipv4_addr)(d) & 0xff))

/* network control ioctls */
enum {
	IOCTL_NET_CONTROL_IF_CREATE = IOCTL_DEVFS_NETWORK_OPS_BASE,
	IOCTL_NET_CONTROL_IF_DELETE,
	IOCTL_NET_CONTROL_IF_ADDADDR,
	IOCTL_NET_CONTROL_IF_RMADDR,
	IOCTL_NET_CONTROL_IF_LIST,
	IOCTL_NET_CONTROL_ROUTE_ADD,
	IOCTL_NET_CONTROL_ROUTE_DELETE,
	IOCTL_NET_CONTROL_ROUTE_LIST,
	IOCTL_NET_IF_GET_ADDR,
	IOCTL_NET_IF_GET_TYPE,
};

/* used in all of the IF control messages */
struct _ioctl_net_if_control_struct {
	char if_name[256];
	netaddr if_addr;
	netaddr mask_addr;
	netaddr broadcast_addr;
};

/* used in all of the route control messages */
struct _ioctl_net_route_struct {
	netaddr net_addr;
	netaddr mask_addr;
	netaddr if_addr;
	char if_name[256];
};

// kernel args
typedef struct {
	// architecture specific
	int foo;
} kernel_args;

int net_init(kernel_args *ka);
int net_init_postdev(kernel_args *ka);

#define NET_CHATTY 0

#endif

