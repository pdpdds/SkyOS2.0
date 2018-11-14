/*
** Copyright 2002, Travis Geiselbrecht. All rights reserved.
** Distributed under the terms of the NewOS License.
*/
#include "windef.h"
#include "syserror.h"
#include "math.h"
#include "memory.h"
#include "khash.h"
//#include <kernel/lock.h>
#include "kheap.h"
//#include <kernel/vm.h>
//#include <kernel/fs/devfs.h>
#include "socket.h"
#include "socket_api.h"
#include <string.h>

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



typedef struct socket_dev {
	sock_id id;
} socket_dev;

static int socket_dev_open(dev_ident ident, dev_cookie *cookie)
{
	socket_dev *s;

	s = (socket_dev *)kmalloc(sizeof(socket_dev));
	if(!s)
		return ERR_NO_MEMORY;

	s->id = -1;

	*cookie = s;

	return 0;
}

static int socket_dev_close(dev_cookie cookie)
{
	socket_dev *s = (socket_dev *)cookie;

	if(s->id >= 0)
		return socket_close(s->id);
	else
		return 0;
}

static int socket_dev_freecookie(dev_cookie cookie)
{
	kfree(cookie);
	return 0;
}

static int socket_dev_seek(dev_cookie cookie, off_t pos, seek_type st)
{
	return ERR_NOT_ALLOWED;
}

static int socket_dev_ioctl(dev_cookie cookie, int op, void *buf, size_t len)
{
	socket_dev *s = (socket_dev *)cookie;
	_socket_api_args_t args;
	int err;

	// copy the args over from user space
	err = user_memcpy(&args, buf, MIN(sizeof(args), len));
	if(err < 0)
		return err;

	if(s->id < 0) {
		switch(op) {
			case _SOCKET_API_CREATE:
				err = s->id = socket_create(args.u.create.type, args.u.create.flags);
				break;
			case _SOCKET_API_ASSOCIATE_SOCKET:
				s->id = args.u.associate.id;
				err = NO_ERROR;
				break;
			default:
				err = ERR_INVALID_ARGS;
		}
	} else {
		switch(op) {
			case _SOCKET_API_BIND:
				err = socket_bind(s->id, args.u.bind.saddr);
				break;
			case _SOCKET_API_CONNECT:
				err = socket_connect(s->id, args.u.connect.saddr);
				break;
			case _SOCKET_API_LISTEN:
				err = socket_listen(s->id);
				break;
			case _SOCKET_API_ACCEPT: {
				// this one is a little tricky, we have a new socket we need to associate with a file descriptor
				sock_id id;
				int fd;
				_socket_api_args_t new_args;
				char socket_dev_path[256];

				id = socket_accept(s->id, args.u.accept.saddr);
				if(id < 0) {
					err = id;
					break;
				}

				// we have the new id, open a new file descriptor in user space
				strcpy(socket_dev_path, "/dev/net/socket");
				fd = vfs_open(socket_dev_path, 0, 0);
				if(fd < 0) {
					socket_close(id);
					err = fd;
					break;
				}

				// now do a special call on this file descriptor that associates it with this socket
				new_args.u.associate.id = id;
				err = vfs_ioctl(fd, _SOCKET_API_ASSOCIATE_SOCKET, &new_args, sizeof(new_args), 0);
				if(err < 0) {
					socket_close(id);
					break;
				}

				err = fd;
				break;
			}
			case _SOCKET_API_RECVFROM:
				err = socket_recvfrom(s->id, args.u.transfer.buf, args.u.transfer.len, args.u.transfer.saddr);
				break;
			case _SOCKET_API_RECVFROM_ETC:
				err = socket_recvfrom_etc(s->id, args.u.transfer.buf, args.u.transfer.len, args.u.transfer.saddr, args.u.transfer.flags, args.u.transfer.timeout);
				break;
			case _SOCKET_API_SENDTO:
				err = socket_sendto(s->id, args.u.transfer.buf, args.u.transfer.len, args.u.transfer.saddr);
				break;
			default:
				err = ERR_INVALID_ARGS;
		}
	}

	return err;
}

static ssize_t socket_dev_read(dev_cookie cookie, void *buf, off_t pos, ssize_t len)
{
	socket_dev *s = (socket_dev *)cookie;

	if(s->id >= 0)
		return socket_read(s->id, buf, len);
	else
		return ERR_NET_NOT_CONNECTED;
}

static ssize_t socket_dev_write(dev_cookie cookie, const void *buf, off_t pos, ssize_t len)
{
	socket_dev *s = (socket_dev *)cookie;

	if(s->id >= 0)
		return socket_write(s->id, buf, len);
	else
		return ERR_NET_NOT_CONNECTED;
}

static struct dev_calls socket_dev_hooks = {
	&socket_dev_open,
	&socket_dev_close,
	&socket_dev_freecookie,
	&socket_dev_seek,
	&socket_dev_ioctl,
	&socket_dev_read,
	&socket_dev_write,
	/* no paging from /dev/null */
	NULL,
	NULL,
	NULL
};

int socket_dev_init(void)
{
	// create device node
	devfs_publish_device("net/socket", NULL, &socket_dev_hooks);

	return 0;
}

