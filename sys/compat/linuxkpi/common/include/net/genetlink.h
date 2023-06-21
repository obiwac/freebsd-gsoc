/*-
 * SPDX-License-Identifier: BSD-2-Clause
 * Copyright (c) 2023 Aymeric Wibo <obiwac@freebsd.org>
 */

#ifndef	_LINUXKPI_NET_GENETLINK_H
#define	_LINUXKPI_NET_GENETLINK_H

#include <netlink/netlink_ctl.h>
#include <netlink/netlink_var.h>

static __inline void *
genlmsg_put(struct sk_buff *skb, uint32_t port, uint32_t seq, struct genl_family const *family, int flags, uint8_t cmd)
{

	pr_debug("%s: TODO hmmmmmmmmmmmmmmmmm\n", __func__);
	return NULL;
}

static __inline void
genl_dump_check_consistent(struct netlink_callback *cb, void *user_hdr)
{

	pr_debug("%s: TODO hmmmmmmmmmmmmmmmmm\n", __func__);
}

static __inline void
genlmsg_cancel(struct sk_buff *skb, void *hdr)
{

	if (hdr == NULL)
		return;

	// nlmsg_abort();
	pr_debug("%s: TODO how to turn skb to nl_writer?\n", __func__);
}

static __inline void
genlmsg_end(struct sk_buff *skb, void *hdr)
{

	// nlmsg_end(skb, hdr - GENL_HDRLEN - NLMSG_HDRLEN);
	pr_debug("%s: TODO how to turn skb to nl_writer?\n", __func__);
}

#endif	/* _LINUXKPI_NET_GENETLINK_H */
