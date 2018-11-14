/*
** Copyright 2001-2002, Travis Geiselbrecht. All rights reserved.
** Distributed under the terms of the NewOS License.
*/
#include "windef.h"
#include "math.h"
#include "memory.h"
#include "kheap.h"
//#include <kernel/fs/devfs.h>
#include <string.h>
#include "syserror.h"
#include "net/ethernet.h"

#include "rtl8139_priv.h"

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

static int rtl8139_open(dev_ident ident, dev_cookie *cookie)
{
	rtl8139 *rtl = (rtl8139 *)ident;

	*cookie = rtl;

	return 0;
}

static int rtl8139_freecookie(dev_cookie cookie)
{
	return 0;
}

static int rtl8139_seek(dev_cookie cookie, off_t pos, seek_type st)
{
	return ERR_NOT_ALLOWED;
}

static int rtl8139_close(dev_cookie cookie)
{
	return 0;
}

static ssize_t rtl8139_read(dev_cookie cookie, void *buf, off_t pos, ssize_t len)
{
	rtl8139 *rtl = (rtl8139 *)cookie;

	if(len < ETHERNET_MAX_SIZE)
		return ERR_VFS_INSUFFICIENT_BUF;
	return rtl8139_rx(rtl, buf, len);
}

static ssize_t rtl8139_write(dev_cookie cookie, const void *buf, off_t pos, ssize_t len)
{
	rtl8139 *rtl = (rtl8139 *)cookie;

	if(len > ETHERNET_MAX_SIZE)
		return ERR_VFS_INSUFFICIENT_BUF;
	if(len < 0)
		return ERR_INVALID_ARGS;

	rtl8139_xmit(rtl, buf, len);
	return len;
}

static int rtl8139_ioctl(dev_cookie cookie, int op, void *buf, size_t len)
{
	rtl8139 *rtl = (rtl8139 *)cookie;
	int err = NO_ERROR;

//	dprintf("rtl8139_ioctl: op %d, buf %p, len %Ld\n", op, buf, (long long)len);

	if(!rtl)
		return ERR_IO_ERROR;

	switch(op) {
		case IOCTL_NET_IF_GET_ADDR: // get the ethernet MAC address
			if(len >= sizeof(rtl->mac_addr)) {
				memcpy(buf, rtl->mac_addr, sizeof(rtl->mac_addr));
			} else {
				err = ERR_VFS_INSUFFICIENT_BUF;
			}
			break;
		case IOCTL_NET_IF_GET_TYPE: // return the type of interface (ethernet, loopback, etc)
			if(len >= sizeof(int)) {
				*(int *)buf = IF_TYPE_ETHERNET;
			} else {
				err = ERR_VFS_INSUFFICIENT_BUF;
			}
			break;
		default:
			err = ERR_INVALID_ARGS;
	}

	return err;
}

static struct dev_calls rtl8139_hooks = {
	&rtl8139_open,
	&rtl8139_close,
	&rtl8139_freecookie,
	&rtl8139_seek,
	&rtl8139_ioctl,
	&rtl8139_read,
	&rtl8139_write,
	/* no paging here */
	NULL,
	NULL,
	NULL
};

int dev_bootstrap(void);

int dev_bootstrap(void)
{
	rtl8139 *rtl;

//	dprintf("rtl8139_dev_init: entry\n");

	// detect and setup the device
	if(rtl8139_detect(&rtl) < 0) {
		// no rtl8139 here
//		dprintf("rtl8139_dev_init: no device found\n");
		return ERR_GENERAL;
	}

	rtl8139_init(rtl);

	// create device node
	devfs_publish_indexed_device("net/rtl8139", rtl, &rtl8139_hooks);

	return 0;
}

