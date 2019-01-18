/*
** Copyright 2001-2002, Travis Geiselbrecht. All rights reserved.
** Distributed under the terms of the NewOS License.
*/
#ifndef _NEWOS_KERNEL_NET_TCP_H
#define _NEWOS_KERNEL_NET_TCP_H

#include "if.h"
#include "ipv4.h"
#include "socket.h"
#include "cbuf.h"

int tcp_input(cbuf *buf, ifnet *i, ipv4_addr source_address, ipv4_addr target_address);
int tcp_open(void **prot_data);
int tcp_bind(void *prot_data, sockaddr *addr);
int tcp_connect(void *prot_data, sockaddr *addr);
int tcp_listen(void *prot_data);
int tcp_accept(void *prot_data, sockaddr *addr, void **new_socket);
int tcp_close(void *prot_data);
ssize_t tcp_recvfrom(void *prot_data, void *buf, ssize_t len, sockaddr *saddr, int flags, bigtime_t timeout);
ssize_t tcp_sendto(void *prot_data, const void *buf, ssize_t len, sockaddr *addr);
int tcp_init(void);

#endif

