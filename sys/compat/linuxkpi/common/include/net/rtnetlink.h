/*-
 * SPDX-License-Identifier: BSD-2-Clause
 * Copyright (c) 2023 Aymeric Wibo <obiwac@freebsd.org>
 */

#ifndef	_LINUXKPI_NET_GENETLINK_H
#define	_LINUXKPI_NET_GENETLINK_H

struct net_device;

struct rtnl_link_ops {
	struct net	*(*get_link_net)(struct net_device const *dev);
};

static inline int
rtnl_is_locked(void)
{

	pr_debug("%s: TODO\n", __func__);
	return (-1);
}

#define	ASSERT_RTNL()	\
	WARN_ONCE(!rtnl_is_locked(), "RTNL: assertion failed at %s:%d\n", __FILE__, __LINE__)

#endif	/* _LINUXKPI_NET_GENETLINK_H */
