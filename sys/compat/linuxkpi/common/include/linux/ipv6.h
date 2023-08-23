/*-
 * SPDX-License-Identifier: BSD-2-Clause
 * Copyright (c) 2023 Aymeric Wibo <obiwac@freebsd.org>
 */

#ifndef	_LINUXKPI_LINUX_IPV6_H
#define	_LINUXKPI_LINUX_IPV6_H

#include <linux/skbuff.h>

struct ipv6hdr {
#if BYTE_ORDER == BIG_ENDIAN
	uint8_t		version:4, priority:4;
#else
	uint8_t		priority:4, version:4;
#endif
	__u8		flow_lbl[3];
	__be16		payload_len;
	__u8		nexthdr;
	__u8		hop_limit;
	struct in6_addr	saddr;
	struct in6_addr	daddr;
};

static inline struct ipv6hdr *
ipv6_hdr(struct sk_buff const *skb)
{

	return ((struct ipv6hdr *)skb_network_header(__DECONST(struct sk_buff *, skb)));
}

#endif	/* _LINUXKPI_LINUX_IPV6_H */
