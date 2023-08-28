/*-
 * SPDX-License-Identifier: BSD-2-Clause
 * Copyright (c) 2023 Aymeric Wibo <obiwac@freebsd.org>
 */

#ifndef	_LINUXKPI_NET_SOCK_H
#define	_LINUXKPI_NET_SOCK_H

struct sock {
};

static inline struct net *
sock_net(struct sock const *sk)
{

	pr_debug("%s: TODO\n", __func__);
	return (NULL);
}

#endif	/* _LINUXKPI_NET_SOCK_H */
