/*-
 * SPDX-License-Identifier: BSD-2-Clause
 * Copyright (c) 2023 Aymeric Wibo <obiwac@freebsd.org>
 */

#ifndef	_LINUXKPI_NET_RTNETLINK_H
#define	_LINUXKPI_NET_RTNETLINK_H

#include <linux/netdevice.h>

struct net_device;

struct rtnl_link_ops {
	struct net		*(*get_link_net)(struct net_device const *);
	char const		*kind;
	size_t			priv_size;
	void			(*setup)(struct net_device *);
	unsigned int		maxtype;
	struct nla_policy const	*policy;
	int			(*validate)(struct nlattr *[], struct nlattr *[], struct netlink_ext_ack *);
	int			(*newlink)(struct net *, struct net_device *, struct nlattr *[], struct nlattr *[], struct netlink_ext_ack *);
	void			(*dellink)(struct net_device *, struct list_head *);
};

static inline int
rtnl_is_locked(void)
{

	pr_debug("%s: TODO\n", __func__);
	return (-1);
}

#define	ASSERT_RTNL()	\
	WARN_ONCE(!rtnl_is_locked(), "RTNL: assertion failed at %s:%d\n", __FILE__, __LINE__)

static inline int
rtnl_link_register(struct rtnl_link_ops *ops)
{

	pr_debug("%s: TODO\n", __func__);
	return (-1);
}

static inline void
rtnl_link_unregister(struct rtnl_link_ops *ops)
{

	pr_debug("%s: TODO\n", __func__);
}

#endif	/* _LINUXKPI_NET_RTNETLINK_H */
