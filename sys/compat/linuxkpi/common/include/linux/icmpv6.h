/*-
 * SPDX-License-Identifier: BSD-2-Clause
 * Copyright (c) 2023 Aymeric Wibo <obiwac@freebsd.org>
 */

#ifndef	_LINUXKPI_LINUX_ICMPV6_H
#define	_LINUXKPI_LINUX_ICMPV6_H

#include <linux/skbuff.h>

#define	ICMPV6_MGM_REPORT	131
#define	ICMPV6_MLD2_REPORT	143

struct icmp6hdr {
	__u8	icmp6_type;
};

static inline struct icmp6hdr *
icmp6_hdr(struct sk_buff const *skb)
{

	return (struct icmp6hdr *)skb_transport_header(__DECONST(struct sk_buff *, skb));
}

#endif	/* _LINUXKPI_LINUX_ICMPV6_H */
