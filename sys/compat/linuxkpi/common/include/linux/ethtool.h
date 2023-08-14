/*-
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2022 Bjoern A. Zeeb
 * Copyright (c) 2023 Aymeric Wibo <obiwac@freebsd.org>
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 * $FreeBSD$
 */

#ifndef _LINUXKPI_LINUX_ETHTOOL_H_
#define	_LINUXKPI_LINUX_ETHTOOL_H_

#include <linux/types.h>
#include <net/rtnetlink.h>

#define	ETH_GSTRING_LEN	(2 * IF_NAMESIZE)	/* Increase if not large enough */

#define	ETHTOOL_FWVERS_LEN	32

struct ethtool_stats {
	uint8_t __dummy[0];
};

enum ethtool_ss {
	ETH_SS_STATS,
};

struct ethtool_drvinfo {
	char			driver[32];
	char			version[32];
	char			fw_version[ETHTOOL_FWVERS_LEN];
	char			bus_info[32];
};

struct net_device;
struct ethtool_ops {
	void	(*get_drvinfo)(struct net_device *, struct ethtool_drvinfo *);
	u32	(*get_link)(struct net_device *);
	void	(*get_strings)(struct net_device *, u32, u8 *);
	void	(*get_ethtool_stats)(struct net_device *, struct ethtool_stats *, u64 *);
	int	(*get_sset_count)(struct net_device *, int);
};

#define	SPEED_UNKNOWN	-1

#define	DUPLEX_HALF	0x00
#define	DUPLEX_FULL	0x01
#define	DUPLEX_UNKNOWN	0xFF

struct ethtool_link_settings {
	uint32_t	speed;
	uint8_t		duplex;
};

struct ethtool_link_ksettings {
	struct ethtool_link_settings	base;
};

static inline int
__ethtool_get_link_ksettings(struct net_device *dev, struct ethtool_link_ksettings *link_ksettings)
{

	/* XXX Normally we'd wanna call dev->ethtool_ops->get_link_ksettings for
	 * this, but we're not guaranteed dev is a Linux net_device (i.e. could
	 * be a FreeBSD ifp), so we can't just do that.
	 */

	return (-EOPNOTSUPP);
}

static inline u32
ethtool_op_get_link(struct net_device *dev)
{

	pr_debug("%s: TODO\n", __func__);
	return (0);
}

#endif	/* _LINUXKPI_LINUX_ETHTOOL_H_ */
