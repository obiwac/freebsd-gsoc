/*-
 * SPDX-License-Identifier: BSD-2-Clause
 * Copyright (c) 2023 Aymeric Wibo <obiwac@freebsd.org>
 */

#ifndef	_LINUXKPI_NET_DSFIELD_H
#define	_LINUXKPI_NET_DSFIELD_H

#include <linux/ip.h>
#include <linux/ipv6.h>
#include <netinet/in.h>

static inline __u8
ipv4_get_dsfield(struct iphdr const *iph)
{

	return (iph->tos);
}

static inline __u8
ipv6_get_dsfield(struct ipv6hdr const *ipv6h)
{

	pr_debug("%s: TODO\n", __func__);
	return (0);
}

#endif	/* _LINUXKPI_NET_DSFIELD_H */
