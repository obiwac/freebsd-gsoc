/*-
 * SPDX-License-Identifier: BSD-2-Clause
 * Copyright (c) 2023 Aymeric Wibo <obiwac@freebsd.org>
 */

#ifndef	_LINUXKPI_NET_RTNETLINK_H
#define	_LINUXKPI_NET_RTNETLINK_H

#include <linux/netdevice.h>
#include <net/if_clone.h>

struct net_device;

struct rtnl_link_ops {
	struct net		*(*get_link_net)(struct net_device const *);
	char const		*kind;
	size_t			priv_size;
	void			(*setup)(struct net_device *);
	unsigned int		maxtype;
	struct nla_policy const	*policy;
	int			(*validate)(struct nlattr *[], struct nlattr *[],
	    struct netlink_ext_ack *);
	int			(*newlink)(struct net *, struct net_device *, struct nlattr *[],
	    struct nlattr *[], struct netlink_ext_ack *);
	void			(*dellink)(struct net_device *, struct list_head *);

	/*
	 * FreeBSD specific fields.
	 * When you use this structure, you must populate the ifc_addreq field with a
	 * struct if_clone_addreq_v2 which contains function pointer to wrappers
	 * around the function pointers in this structure.
	 * An example may be found in sys/contrib/dev/batman-adv/soft-interface.c
	 */
	struct if_clone_addreq_v2	*ifc_addreq;
	struct if_clone			*ifc;
};

static inline int
rtnl_is_locked(void)
{

	pr_debug("%s: TODO\n", __func__);
	return (-1);
}

#define	ASSERT_RTNL()								\
	WARN_ONCE(!rtnl_is_locked(), "RTNL: assertion failed at %s:%d\n", 	\
	    __FILE__, __LINE__)

static inline int
rtnl_link_register(struct rtnl_link_ops *ops)
{
	char const *const name = ops->kind;
	struct if_clone *ifc;

	if (ops->ifc_addreq == NULL) {
		pr_debug("ifc_addreq field of struct rtnl_link_ops must be"
		    "populated on FreeBSD\n");
		return (-1);
	}

	/* Locking is done by if_clone_attach; we don't have to worry about it. */
	ifc = ifc_attach_cloner(name, (struct if_clone_addreq *)ops->ifc_addreq);
	if (ifc == NULL)
		return (-1);

	ops->ifc = ifc;
	return (0);
}

static inline void
rtnl_link_unregister(struct rtnl_link_ops *ops)
{

	if (ops->ifc == NULL)
		pr_debug("ifc field of struct rtnl_link_ops is NULL\n");
	else
		ifc_detach_cloner(ops->ifc);
}

#endif	/* _LINUXKPI_NET_RTNETLINK_H */
