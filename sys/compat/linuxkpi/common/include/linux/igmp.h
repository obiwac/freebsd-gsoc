/*-
 * SPDX-License-Identifier: BSD-2-Clause
 * Copyright (c) 2023 Aymeric Wibo <obiwac@freebsd.org>
 */

#ifndef	_LINUXKPI_LINUX_IGMP_H
#define	_LINUXKPI_LINUX_IGMP_H

#include <linux/skbuff.h>

#define	IGMP_HOST_MEMBERSHIP_REPORT	0x12
#define	IGMPV2_HOST_MEMBERSHIP_REPORT	0x16
#define	IGMPV3_HOST_MEMBERSHIP_REPORT	0x22

struct ip_mc_list {
	__be32	multiaddr;
	struct ip_mc_list __rcu	*next_rcu;
};

struct igmphdr {
	__u8	type;
};

static inline struct igmphdr *
igmp_hdr(struct sk_buff const *skb)
{

	return (struct igmphdr *)skb_transport_header(__DECONST(struct sk_buff *, skb));
}

static inline int
ip_mc_check_igmp(struct sk_buff *skb)
{

	pr_debug("%s: TODO\n", __func__);
	return (-1);
}

#endif	/* _LINUXKPI_LINUX_IGMP_H */
