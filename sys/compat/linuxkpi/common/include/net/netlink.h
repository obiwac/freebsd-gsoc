/*-
 * Copyright (c) 2020,2022 Bjoern A. Zeeb
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
 */

#ifndef	_LINUXKPI_NET_NETLINK_H
#define	_LINUXKPI_NET_NETLINK_H

#include <netlink/netlink.h>

struct nla_policy {
};

struct netlink_skb_parms {
	__u32	portid;
};

struct netlink_callback {
	struct sk_buff	*skb;
	const struct nlmsghdr	*nlh;
	unsigned int	seq;
	int		args[8];
};

static __inline int
nla_put(struct sk_buff *skb, int attr, size_t len, void *data)
{

	pr_debug("%s: TODO -- now native?\n", __func__);
	return (-ENXIO);
}

static __inline int
nla_put_string(struct sk_buff *skb, int attr, char const *str)
{

	return (nla_put(skb, attr, strlen(str) + 1, __DECONST(void*, str)));
}

static __inline int
nla_put_flag(struct sk_buff *skb, int attr)
{

	return (nla_put(skb, attr, 0, NULL));
}

static __inline int
nla_put_u16(struct sk_buff *skb, int attr, uint16_t val)
{

	return (nla_put(skb, attr, sizeof(uint16_t), &val));
}

static __inline int
nla_put_u32(struct sk_buff *skb, int attr, uint32_t val)
{

	return (nla_put(skb, attr, sizeof(uint32_t), &val));
}

#endif	/* _LINUXKPI_NET_NETLINK_H */
