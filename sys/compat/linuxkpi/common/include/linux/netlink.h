/*-
* SPDX-License-Identifier: BSD-2-Clause
* Copyright (c) 2023 Aymeric Wibo <obiwac@freebsd.org>
*/
#ifndef _LINUXKPI_LINUX_NETLINK_H_
#define _LINUXKPI_LINUX_NETLINK_H_

#include <net/netlink.h>

#define NETLINK_CB(skb)		(*(struct netlink_skb_parms*)&((skb)->cb))

#endif /* _LINUXKPI_LINUX_NETLINK_H_ */
