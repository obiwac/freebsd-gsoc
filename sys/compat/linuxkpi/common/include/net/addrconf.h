/*-
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2020 The FreeBSD Foundation
 * Copyright (c) 2023 Aymeric Wibo <obiwac@freebsd.org>
 *
 * This software was developed by Björn Zeeb under sponsorship from
 * the FreeBSD Foundation.
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

#ifndef	_LINUXKPI_NET_ADDRCONF_H
#define	_LINUXKPI_NET_ADDRCONF_H

#include <linux/skbuff.h>
#include <netinet/in.h>
#include <sys/types.h>

static __inline void
addrconf_addr_solict_mult(struct in6_addr *ia6, struct in6_addr *sol)
{

	sol->s6_addr16[0] = IPV6_ADDR_INT16_MLL;
	sol->s6_addr16[1] = 0;
	sol->s6_addr32[1] = 0;
	sol->s6_addr32[2] = IPV6_ADDR_INT32_ONE;
	sol->s6_addr32[3] = ia6->s6_addr32[3];
	sol->s6_addr8[12] = 0xff;
}

static inline int
ipv6_mc_check_mld(struct sk_buff *skb)
{

	pr_debug("%s: TODO\n", __func__);
	return (-1);
}

static inline bool
ipv6_addr_is_ll_all_nodes(struct in6_addr const *addr)
{

	pr_debug("%s: TODO\n", __func__);
	return (false);
}

#endif	/* _LINUXKPI_NET_ADDRCONF_H */
