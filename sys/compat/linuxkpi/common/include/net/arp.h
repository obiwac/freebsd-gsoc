/*-
 * SPDX-License-Identifier: BSD-2-Clause
 * Copyright (c) 2023 Aymeric Wibo <obiwac@freebsd.org>
 */

#ifndef	_LINUXKPI_NET_ARP_H
#define	_LINUXKPI_NET_ARP_H

#include <linux/netdevice.h>
#include <linux/skbuff.h>

static inline struct sk_buff *
arp_create(int type, int ptype, __be32 dest_ip, struct net_device *dev,
    __be32 src_ip, unsigned char const *dest_hw, unsigned char const *src_hw,
    unsigned char const *target_hw)
{

	pr_debug("%s: TODO\n", __func__);
	return (NULL);
}

static inline unsigned int
arp_hdr_len(struct net_device const *dev)
{

	pr_debug("%s: TODO\n", __func__);
	return (0);
}

#endif	/* _LINUXKPI_NET_ARP_H */
