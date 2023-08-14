/*-
 * Copyright (c) 2015-2016 Mellanox Technologies, Ltd. All rights reserved.
 * Copyright (c) 2023 Aymeric Wibo <obiwac@freebsd.org>
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
#ifndef _LINUXKPI_LINUX_ETHERDEVICE_H_
#define	_LINUXKPI_LINUX_ETHERDEVICE_H_

#include <linux/device.h>
#include <linux/skbuff.h>
#include <linux/types.h>
#include <linux/netdevice.h>

#include <sys/random.h>
#include <sys/libkern.h>

#define	ETH_MODULE_SFF_8079		1
#define	ETH_MODULE_SFF_8079_LEN		256
#define	ETH_MODULE_SFF_8472		2
#define	ETH_MODULE_SFF_8472_LEN		512
#define	ETH_MODULE_SFF_8636		3
#define	ETH_MODULE_SFF_8636_LEN		256
#define	ETH_MODULE_SFF_8436		4
#define	ETH_MODULE_SFF_8436_LEN		256

struct ethtool_eeprom {
	u32	offset;
	u32	len;
};

struct ethtool_modinfo {
	u32	type;
	u32	eeprom_len;
};

static inline bool
is_zero_ether_addr(const u8 * addr)
{
	return ((addr[0] + addr[1] + addr[2] + addr[3] + addr[4] + addr[5]) == 0x00);
}

static inline bool
is_multicast_ether_addr(const u8 * addr)
{
	return (0x01 & addr[0]);
}

static inline bool
is_broadcast_ether_addr(const u8 * addr)
{
	return ((addr[0] + addr[1] + addr[2] + addr[3] + addr[4] + addr[5]) == (6 * 0xff));
}

static inline bool
is_valid_ether_addr(const u8 * addr)
{
	return !is_multicast_ether_addr(addr) && !is_zero_ether_addr(addr);
}

static inline void
ether_addr_copy(u8 * dst, const u8 * src)
{
	memcpy(dst, src, 6);
}

static inline bool
ether_addr_equal_unaligned(const u8 *pa, const u8 *pb)
{
	return (memcmp(pa, pb, 6) == 0);
}
#define	ether_addr_equal(_pa, _pb)	ether_addr_equal_unaligned(_pa, _pb)

static inline bool
ether_addr_equal_64bits(const u8 *pa, const u8 *pb)
{
	return (memcmp(pa, pb, 6) == 0);
}

static inline void
eth_broadcast_addr(u8 *pa)
{
	memset(pa, 0xff, 6);
}

static inline void
eth_zero_addr(u8 *pa)
{
	memset(pa, 0, 6);
}

static inline void
random_ether_addr(u8 *dst)
{
	arc4random_buf(dst, 6);

	dst[0] &= 0xfe;
	dst[0] |= 0x02;
}

static inline void
eth_random_addr(u8 *dst)
{

	random_ether_addr(dst);
}

static inline int
device_get_mac_address(struct device *dev, char *dst)
{

	/* XXX get mac address from FDT? */
	return (-ENOENT);
}

static inline __be16
eth_type_trans(struct sk_buff *skb, struct net_device *dev)
{
	struct ethhdr *ethhdr;

	/*
	 * XXX eth_type_trans is a little more complicated than what I'm doing
	 * here; some protocols aren't simply what's in ethhdr->h_proto.
	 * There's surely a FreeBSD equivalent I'm not aware of though.
	 */
	skb_reset_mac_header(skb);
	ethhdr = (struct ethhdr *)skb->data;

	return (ethhdr->h_proto);
}

static inline void
eth_hw_addr_set(struct net_device *dev, u8 const *addr)
{

	dev->addr_len = ETHER_ADDR_LEN;
	ether_addr_copy(dev->dev_addr, addr);
}

static inline int
eth_validate_addr(struct net_device *dev)
{

	pr_debug("%s: TODO\n", __func__);
	return (-1);
}

static inline void
eth_hw_addr_random(struct net_device *dev)
{
	uint8_t ether[ETHER_ADDR_LEN];

	random_ether_addr(ether);
	eth_hw_addr_set(dev, ether);
}

#endif					/* _LINUXKPI_LINUX_ETHERDEVICE_H_ */
