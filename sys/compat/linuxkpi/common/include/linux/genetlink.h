/*-
 * SPDX-License-Identifier: BSD-2-Clause
 * Copyright (c) 2023 Aymeric Wibo <obiwac@freebsd.org>
 */

#ifndef	_LINUXKPI_LINUX_GENETLINK_H
#define	_LINUXKPI_LINUX_GENETLINK_H

#include <linux/skbuff.h>
#include <net/genetlink.h>
#include <netlink/netlink_generic.h>

struct genl_multicast_group {
	char	name[GENL_NAMSIZ];
};

static inline int
genlmsg_multicast_netns(struct genl_family const *family, struct net *net, struct sk_buff *skb, u32 portid, unsigned int group, gfp_t flags)
{

	pr_debug("%s: TODO\n", __func__);
	return (-1);
}

static inline int
genlmsg_reply(struct sk_buff *skb, struct genl_info *info)
{

	pr_debug("%s: TODO\n", __func__);
	return (-1);
}

#endif	/* _LINUXKPI_LINUX_GENETLINK_H */
