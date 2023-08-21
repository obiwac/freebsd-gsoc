/*-
 * Copyright (c) 2020,2022 Bjoern A. Zeeb
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
 */

#ifndef	_LINUXKPI_NET_NETLINK_H
#define	_LINUXKPI_NET_NETLINK_H

#include <linux/skbuff.h>
#include <netlink/netlink.h>
#include <netlink/netlink_ctl.h>

struct nla_policy {
	u8	type;
	u16	len;
};

struct netlink_skb_parms {
	__u32	portid;
};

#define	NETLINK_CB(skb)	(*(struct netlink_skb_parms *) &((skb)->cb))

struct netlink_callback {
	struct sk_buff	*skb;
	const struct nlmsghdr	*nlh;
	unsigned int	seq;
	long		args[8];
};

static inline int
nla_put(struct sk_buff *skb, int attr, size_t len, void const *data)
{

	pr_debug("%s: TODO -- now native?\n", __func__);
	return (-ENXIO);
}

static inline int
nla_put_string(struct sk_buff *skb, int attr, char const *str)
{

	return (nla_put(skb, attr, strlen(str) + 1, __DECONST(void*, str)));
}

static inline int
nla_put_flag(struct sk_buff *skb, int attr)
{

	return (nla_put(skb, attr, 0, NULL));
}

static inline int
nla_put_u8(struct sk_buff *skb, int attr, u8 val)
{

	return (nla_put(skb, attr, sizeof(val), &val));
}

static inline int
nla_put_u16(struct sk_buff *skb, int attr, u16 val)
{

	return (nla_put(skb, attr, sizeof(val), &val));
}

static inline int
nla_put_u32(struct sk_buff *skb, int attr, u32 val)
{

	return (nla_put(skb, attr, sizeof(val), &val));
}

static inline int
nla_put_u64_64bit(struct sk_buff *skb, int attr, u64 val, int padaddr)
{

	pr_debug("%s: TODO\n", __func__);
	return (-1);
}

static inline int
nla_put_be32(struct sk_buff *skb, int attr, __be32 val)
{

	return (nla_put(skb, attr, sizeof(val), &val));
}

static inline int
nla_put_in_addr(struct sk_buff *skb, int attr, __be32 addr)
{

	return (nla_put_be32(skb, attr, addr));
}

static inline u8
nla_get_u8(struct nlattr const *nla)
{

	pr_debug("%s: TODO\n", __func__);
	return (0);
}

static inline u16
nla_get_u16(struct nlattr const *nla)
{

	pr_debug("%s: TODO\n", __func__);
	return (0);
}

static inline u32
nla_get_u32(struct nlattr const *nla)
{

	pr_debug("%s: TODO\n", __func__);
	return (0);
}

enum {
	NLA_STRING,
	NLA_NUL_STRING,
	NLA_FLAG,
	NLA_U8,
	NLA_U16,
	NLA_U32,
	NLA_U64,
};

static inline struct nlattr *
nlmsg_find_attr(struct nlmsghdr const *nlh, int hdrlen, int attrtype)
{

	pr_debug("%s: TODO\n", __func__);
	return (NULL);
}

static inline int
nla_len(struct nlattr const *nla)
{

	return nla->nla_len - NLA_HDRLEN;
}

static inline int
nlmsg_msg_size(int payload)
{

	return NLMSG_HDRLEN + payload;
}

static inline int
nlmsg_total_size(int payload)
{

	return NLMSG_ALIGN(nlmsg_msg_size(payload));
}

static inline struct sk_buff *
nlmsg_new(size_t payload, gfp_t flags)
{

	return alloc_skb(nlmsg_total_size(payload), flags);
}

static inline void
nlmsg_free(struct sk_buff *skb)
{

	kfree_skb(skb);
}

#if PAGE_SIZE < 8192
#define	NLMSG_GOODSIZE	SKB_WITH_OVERHEAD(PAGE_SIZE)
#else
#define	NLMSG_GOODSIZE	SKB_WITH_OVERHEAD(8192)
#endif

#define	NLMSG_DEFAULT_SIZE	(NLMSG_GOODSIZE - NLMSG_HDRLEN)

static inline void *
nla_data(struct nlattr const *nla)
{
	return __DECONST(char *, nla) + NLA_HDRLEN;
}

#endif	/* _LINUXKPI_NET_NETLINK_H */
