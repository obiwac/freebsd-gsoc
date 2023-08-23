/*-
 * SPDX-License-Identifier: BSD-2-Clause
 * Copyright (c) 2023 Aymeric Wibo <obiwac@freebsd.org>
 */

#ifndef	_LINUXKPI_LINUX_IPV6_H
#define	_LINUXKPI_LINUX_IPV6_H

#include <linux/skbuff.h>

/*
 * (u) unconfirmed structure field names;
 * using FreeBSD's (struct ip6_hdr) meanwhile.
 */
struct ipv6hdr {
#if BYTE_ORDER == BIG_ENDIAN
	uint8_t		version:4, ihl:4;
#else
	uint8_t		ihl:4, version:4;
#endif
	uint32_t	ip6_un1_flow;	/* (u) */
	uint16_t	ip6_un1_plen;	/* (u) */
	uint8_t		nexthdr;
	uint8_t		ip6_un1_hlim;	/* (u) */
	struct in6_addr ip6_src;	/* (u) */
	struct in6_addr ip6_dst;	/* (u) */
};

static inline struct ipv6hdr *
ipv6_hdr(struct sk_buff const *skb)
{

	return ((struct ipv6hdr *)skb_network_header(__DECONST(struct sk_buff *, skb)));
}

#endif	/* _LINUXKPI_LINUX_IPV6_H */
