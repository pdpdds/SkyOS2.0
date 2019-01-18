/*
** Copyright 2001-2006, Travis Geiselbrecht. All rights reserved.
** Distributed under the terms of the NewOS License.
*/
#ifndef _NEWOS_KERNEL_NET_MISC_H
#define _NEWOS_KERNEL_NET_MISC_H


#include "net.h"

#if BYTE_ORDER == LITTLE_ENDIAN
#if defined(_ARCH_BSWAP16)
#define ntohs(n) _ARCH_BSWAP16(n)
#define htons(n) _ARCH_BSWAP16(n)
#else
#define ntohs(n) \
	((((uint16)(n) & 0xff) << 8) | ((uint16)(n) >> 8))
#define htons(h) \
	((((uint16)(h) & 0xff) << 8) | ((uint16)(h) >> 8))
#endif

#if defined(_ARCH_BSWAP32)
#define ntohl(n) _ARCH_BSWAP32(n)
#define htonl(n) _ARCH_BSWAP32(n)
#else
#define ntohl(n) \
	(((uint32)(n) << 24) | (((uint32)(n) & 0xff00) << 8) |(((uint32)(n) & 0x00ff0000) >> 8) | ((uint32)(n) >> 24))
#define htonl(h) \
	(((uint32)(h) << 24) | (((uint32)(h) & 0xff00) << 8) |(((uint32)(h) & 0x00ff0000) >> 8) | ((uint32)(h) >> 24))
#endif
#elif BYTE_ORDER == BIG_ENDIAN
#define ntohs(n) \
	((uint16)(n))
#define htons(h) \
	((uint16)(h))
#define ntohl(n) \
	((uint32)(n))
#define htonl(h) \
	((uint32)(h))
#else
#error need to define BYTE_ORDER
#endif

uint16 ones_sum16(uint32 sum, const void *_buf, int len);
uint16 cksum16(void *_buf, int len);
uint16 cksum16_2(void *buf1, int len1, void *buf2, int len2);
int cmp_netaddr(netaddr *addr1, netaddr *addr2);

#endif

