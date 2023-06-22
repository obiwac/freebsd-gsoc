/*-
 * Copyright (c) 2010 Isilon Systems, Inc.
 * Copyright (c) 2010 iX Systems, Inc.
 * Copyright (c) 2010 Panasas, Inc.
 * Copyright (c) 2013-2016 Mellanox Technologies, Ltd.
 * Copyright (c) 2023 Aymeric Wibo <obiwac@freebsd.org>
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice unmodified, this list of conditions, and the following
 *    disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 * NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * $FreeBSD$
 */
#ifndef	_LINUXKPI_LINUX_IF_VLAN_H_
#define	_LINUXKPI_LINUX_IF_VLAN_H_

#include <linux/if_ether.h>
#include <net/ethernet.h>
#include <net/if.h>
#include <net/if_types.h>
#include <net/if_var.h>
#include <net/if_vlan_var.h>
#include <sys/endian.h>
#include <sys/socket.h>
#include <sys/types.h>

#define VLAN_VID_MASK	0x0fff
#define VLAN_N_VID	4096

static inline int
is_vlan_dev(struct ifnet *ifp)
{
	return (if_gettype(ifp) == IFT_L2VLAN);
}

static inline uint16_t
vlan_dev_vlan_id(struct ifnet *ifp)
{
	uint16_t vtag;
	if (VLAN_TAG(ifp, &vtag) == 0)
		return (vtag);
	return (0);
}

static inline struct sk_buff *
vlan_insert_tag(struct sk_buff *skb, __be16 vlan_proto, uint16_t vlan_tci)
{

	pr_debug("%s: TODO\n", __func__);
	return (NULL);
}

struct vlan_hdr {
	__be16	h_vlan_TCI;
	__be16	h_vlan_encapsulated_proto;
};

struct vlan_ethhdr {
	unsigned char	h_dest[ETH_ALEN];
	unsigned char	h_source[ETH_ALEN];
	__be16		h_vlan_proto;
	__be16		h_vlan_TCI;
	__be16		h_vlan_encapsulated_proto;
};

static inline struct vlan_ethhdr *
vlan_eth_hdr(struct sk_buff const *skb)
{

	return (struct vlan_ethhdr *)skb_mac_header(skb);
}

#endif	/* _LINUXKPI_LINUX_IF_VLAN_H_ */
