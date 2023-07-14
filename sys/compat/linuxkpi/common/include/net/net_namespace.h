/*-
 * SPDX-License-Identifier: BSD-2-Clause
 * Copyright (c) 2023 Aymeric Wibo <obiwac@freebsd.org>
 */

#ifndef	_LINUXKPI_NET_NAMESPACE_H
#define	_LINUXKPI_NET_NAMESPACE_H

static inline int
net_eq(struct net const *net1, struct net const *net2)
{

	return (net1 == net2);
}

#endif	/* _LINUXKPI_NET_NAMESPACE_H */
