/*-
 * SPDX-License-Identifier: BSD-2-Clause
 * Copyright (c) 2023 Aymeric Wibo <obiwac@freebsd.org>
 */

#ifndef	_LINUXKPI_LINUX_INETDEVICE_H_
#define	_LINUXKPI_LINUX_INETDEVICE_H_

#include <linux/netdevice.h>

struct in_device {
	struct ip_mc_list __rcu	*mc_list;
};

static inline struct in_device *
__in_dev_get_rcu(struct net_device const *dev)
{

	/* return (rcu_dereference(dev->ip_ptr)); */
	pr_debug("%s: TODO\n", __func__);
	return (NULL);
}

#define	IN_DEV_MFORWARD(in_dev)	({		\
	pr_debug("IN_DEV_MFORWARD: TODO\n");	\
	0;					\
})

#endif	/* _LINUXKPI_LINUX_INETDEVICE_H_ */
