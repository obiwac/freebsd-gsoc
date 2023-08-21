/*-
 * SPDX-License-Identifier: BSD-2-Clause
 * Copyright (c) 2023 Aymeric Wibo <obiwac@freebsd.org>
 */

#ifndef	_LINUXKPI_NET_GENETLINK_H
#define	_LINUXKPI_NET_GENETLINK_H

#include <linux/skbuff.h>
#include <netlink/netlink_ctl.h>

struct genl_info {
	u32	snd_seq;
	u32	snd_portid;
	struct nlattr	**attrs;
	void	*user_ptr[2];
};

struct genl_split_ops {
	u8	internal_flags;
};

struct genl_small_ops {
	u8	cmd;
	u8	validate;
	u8	internal_flags;
	u8	flags;
	int	(*doit)(struct sk_buff *skb, struct genl_info *info);
	int	(*dumpit)(struct sk_buff *skb, struct netlink_callback *cb);
};

struct genl_family {
	char const		*name;
	u16			hdrsize;
	u16			version;
	u16			maxattr;
	struct nla_policy const	*policy;
	u8			netnsok;
	int			(*pre_doit)(struct genl_split_ops const *ops, struct sk_buff *skb, struct genl_info *info);
	void			(*post_doit)(struct genl_split_ops const *ops, struct sk_buff *skb, struct genl_info *info);
	struct module		*module;
	u8			n_small_ops;
	struct genl_small_ops const	*small_ops;
	u8			resv_start_op;
	u8			n_mcgrps;
	struct genl_multicast_group const	*mcgrps;
};

enum genl_validate_flags {
	GENL_DONT_VALIDATE_STRICT	= BIT(0),
	GENL_DONT_VALIDATE_DUMP		= BIT(1),
	GENL_DONT_VALIDATE_DUMP_STRICT	= BIT(2),
};

static inline void *
genlmsg_put(struct sk_buff *skb, uint32_t port, uint32_t seq, struct genl_family const *family, int flags, uint8_t cmd)
{

	pr_debug("%s: TODO hmmmmmmmmmmmmmmmmm\n", __func__);
	return NULL;
}

static inline void
genl_dump_check_consistent(struct netlink_callback *cb, void *user_hdr)
{

	pr_debug("%s: TODO hmmmmmmmmmmmmmmmmm\n", __func__);
}

static inline void
genlmsg_cancel(struct sk_buff *skb, void *hdr)
{

	if (hdr == NULL)
		return;

	// nlmsg_abort();
	pr_debug("%s: TODO how to turn skb to nl_writer?\n", __func__);
}

static inline void
genlmsg_end(struct sk_buff *skb, void *hdr)
{

	// nlmsg_end(skb, hdr - GENL_HDRLEN - NLMSG_HDRLEN);
	pr_debug("%s: TODO how to turn skb to nl_writer?\n", __func__);
}

static inline struct net *
genl_info_net(struct genl_info *info)
{

	pr_debug("%s: TODO\n", __func__);
	return (NULL);
}

static inline int
linux_genl_register_family(struct genl_family *family)
{
	/* XXX Should this be family->maxattr - 1? */
	int const family_id = genl_register_family(family->name,
	    family->hdrsize, family->version, family->maxattr);

	if (family_id == 0)
		return (-1);
	return (0);
}

static inline void
linux_genl_unregister_family(struct genl_family *family)
{

	genl_unregister_family(family->name);
}

#endif	/* _LINUXKPI_NET_GENETLINK_H */
