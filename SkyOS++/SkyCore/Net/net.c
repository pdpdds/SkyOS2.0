/*
** Copyright 2001, Travis Geiselbrecht. All rights reserved.
** Distributed under the terms of the NewOS License.
*/
#include "windef.h"
#include "syserror.h"
#include "memory.h"
#include "math.h"
//#include "thread.h>
//#include "vfs.h>
#include "cbuf.h"
#include "kheap.h"
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
//#include <boot/stage2.h>
#include <string.h>
#include "SystemAPI.h"

thread_id rx_thread_id;
int net_fd;

#define NET_TEST1 0
#define NET_TEST2 0
#define NET_TEST3 0
#define NET_TEST4 0

#if NET_TEST2
static int net_test_thread2(void *unused)
{
	sock_id id;
	sockaddr addr;
	sockaddr saddr;
	char buf[64];

	thread_snooze(2000000);

	memset(&addr, 0, sizeof(addr));
	addr.addr.len = 4;
	addr.addr.type = ADDR_TYPE_IP;
	addr.port = 9998;
	id = socket_create(SOCK_PROTO_UDP, 0);
	socket_bind(id, &addr);
	dprintf("net_test_thread: id %d\n", id);

	saddr.port = 9999;
	saddr.addr.len = 4;
	saddr.addr.type = ADDR_TYPE_IP;
	NETADDR_TO_IPV4(saddr.addr) = 0x7f000001; // loopback

	strcpy(buf, "loopback test");

	for(;;) {
		ssize_t bytes_written;

		// send it back
		bytes_written = socket_sendto(id, buf, strlen(buf), &saddr);
	}
}
#endif

#if NET_TEST3
static int net_test_thread3(void *unused)
{
	sock_id id;
	sockaddr addr;

	thread_snooze(2000000);

	memset(&addr, 0, sizeof(addr));
	addr.addr.len = 4;
	addr.addr.type = ADDR_TYPE_IP;
	addr.port = 9999;
	id = socket_create(SOCK_PROTO_UDP, 0);
	socket_bind(id, &addr);
	dprintf("net_test_thread: id %d\n", id);

	for(;;) {
		ssize_t bytes_read;
		sockaddr saddr;
		char buf[64];

		bytes_read = socket_recvfrom(id, buf, sizeof(buf), &saddr);
		dprintf("net_test_thread3: read %Ld bytes from host 0x%x, port %d: '%s'\n",
			(long long)bytes_read, NETADDR_TO_IPV4(saddr.addr), saddr.port, buf);
	}
}
#endif

#if NET_TEST4
static uint64 transferred_data = 0;

static int net_test_thread4_watcher(void *unused)
{
	uint64 old_transferred_data =0;
	uint64 now;

	for(;;) {
		thread_snooze(1000000);
		now = transferred_data;
		dprintf("moved %Ld bytes in the last second (%Ld total)\n", now - old_transferred_data, now);
		old_transferred_data = now;
	}
}

static int net_test_thread4(void *unused)
{
	sock_id id;
	sockaddr addr;
	int err;

	thread_snooze(2000000);

	id = socket_create(SOCK_PROTO_TCP, 0);
	dprintf("net_test_thread4: id %d\n", id);

	memset(&addr, 0, sizeof(addr));
	addr.addr.len = 4;
	addr.addr.type = ADDR_TYPE_IP;
	addr.port = 1900;
//	NETADDR_TO_IPV4(addr.addr) = IPV4_DOTADDR_TO_ADDR(192,168,0,3);
	NETADDR_TO_IPV4(addr.addr) = IPV4_DOTADDR_TO_ADDR(63,203,215,73);

retry:
	err = socket_connect(id, &addr);
	if(err < 0) {
		dprintf("net_test_thread: error %d opening socket, retrying...\n", err);
		thread_snooze(5000000);
		goto retry;
	}

	for(;;) {
		char buf2[1024];
		ssize_t len;

		len = socket_sendto(id, buf2, sizeof(buf2), NULL);
//		dprintf("*** net test 4: len %d\n", len);
		transferred_data += len;

#if 0
		len = socket_recvfrom(id, buf2, sizeof(buf2), NULL);
//		dprintf("*** net test 4: len %d\n", len);
		transferred_data += len;
#endif
#if 0
		len = socket_recvfrom(id, buf2, sizeof(buf2), NULL);
		dprintf("*** net test 4: len %d\n", len);
		len = socket_recvfrom(id, buf2, sizeof(buf2), NULL);
		dprintf("*** net test 4: len %d\n", len);
		len = socket_recvfrom(id, buf2, sizeof(buf2), NULL);
		dprintf("*** net test 4: len %d\n", len);

		thread_snooze(4000000);
#endif
	}

	return 0;
}
#endif

#if NET_TEST1
static int net_test_thread(void *unused)
{
	sock_id id;
	sockaddr addr;

	thread_snooze(2000000);

	memset(&addr, 0, sizeof(addr));
	addr.addr.len = 4;
	addr.addr.type = ADDR_TYPE_IP;
	addr.port = 9999;
	id = socket_create(SOCK_PROTO_UDP, 0);
	socket_bind(id, &addr);
	dprintf("net_test_thread: id %d\n", id);

	for(;;) {
		ssize_t bytes_read;
		ssize_t bytes_written;
		sockaddr saddr;
		char buf[64];

		bytes_read = socket_recvfrom(id, buf, sizeof(buf), &saddr);
		dprintf("net_test_thread: read %Ld bytes from host 0x%x, port %d: '%s'\n",
			(long long)bytes_read, NETADDR_TO_IPV4(saddr.addr), saddr.port, buf);

		// send it back
		bytes_written = socket_sendto(id, buf, bytes_read, &saddr);
	}
}
#endif

int net_init(kernel_args *ka)
{
//	dprintf("net_init: entry\n");

	net_timer_init();
	if_init();
	ethernet_init();
	arp_init();
	ipv4_init();
	loopback_init();
	udp_init();
	tcp_init();
	socket_init();
	net_control_init();

	return 0;
}

int net_init_postdev(kernel_args *ka)
{
	printf("net_init_postdev: entry\n");

#if NET_TEST
	// start the test thread
{
	thread_id id;

	id = thread_create_kernel_thread("net tester", &net_test_thread, NULL);
	thread_resume_thread(id);
}
#endif
#if NET_TEST4
	// start the test thread
{
	thread_id id;

	id = thread_create_kernel_thread("net tester 4", &net_test_thread4, NULL);
	thread_resume_thread(id);
	id = thread_create_kernel_thread("net tester 4 watcher", &net_test_thread4_watcher, NULL);
	thread_resume_thread(id);
}
#endif
#if NET_TEST2 || NET_TEST3
	// start the other test threads
{
	thread_id id;

	id = thread_create_kernel_thread("net tester2", &net_test_thread2, NULL);
	thread_resume_thread(id);
	id = thread_create_kernel_thread("net tester3", &net_test_thread3, NULL);
	thread_resume_thread(id);
}
#endif
	return 0;
}

