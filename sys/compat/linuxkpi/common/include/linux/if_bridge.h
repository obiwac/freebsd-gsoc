/*-
 * SPDX-License-Identifier: BSD-2-Clause
 * Copyright (c) 2023 Aymeric Wibo <obiwac@freebsd.org>
 */

#ifndef	_LINUXKPI_LINUX_IF_BRIDGE_H
#define	_LINUXKPI_LINUX_IF_BRIDGE_H

#include <linux/netdevice.h>

struct br_ip {
	__be16	proto;
	union {
		__be32	ip4;
	} dst;
};

struct br_ip_list {
	struct list_head	list;
	struct br_ip		addr;
};

static inline bool
netif_is_bridge_master(struct net_device const *dev)
{

	pr_debug("%s: TODO\n", __func__);
	return (false);
}

static inline int
br_multicast_list_adjacent(struct net_device *dev, struct list_head *br_ip_list)
{

	pr_debug("%s: TODO\n", __func__);
	return (-1);
}

static inline bool
br_multicast_has_querier_anywhere(struct net_device *dev, int proto)
{

	pr_debug("%s: TODO\n", __func__);
	return (false);
}

static inline bool
br_multicast_has_querier_adjacent(struct net_device *dev, int proto)
{

	pr_debug("%s: TODO\n", __func__);
	return (false);
}

static inline bool
br_multicast_has_router_adjacent(struct net_device *dev, int proto)
{

	pr_debug("%s: TODO\n", __func__);
	return (false);
}

#endif	/* _LINUXKPI_LINUX_IF_BRIDGE_H */
