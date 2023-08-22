/*-
 * Copyright (c) 2020-2022 The FreeBSD Foundation
 * Copyright (c) 2021-2022 Bjoern A. Zeeb
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

/*
 * NOTE: this socket buffer compatibility code is highly EXPERIMENTAL.
 *       Do not rely on the internals of this implementation.  They are highly
 *       likely to change as we will improve the integration to FreeBSD mbufs.
 */

#include <sys/cdefs.h>
__FBSDID("$FreeBSD$");

#include "opt_ddb.h"

#include <sys/param.h>
#include <sys/types.h>
#include <sys/kernel.h>
#include <sys/malloc.h>
#include <sys/socket.h>
#include <sys/sysctl.h>

#include <net/if.h>
#include <net/if_dl.h>
#include <net/if_var.h>
#include <net/if_llatbl.h>
#include <netinet/if_ether.h>

#ifdef DDB
#include <ddb/ddb.h>
#endif

#include <linux/skbuff.h>
#include <linux/slab.h>
#include <linux/gfp.h>
#ifdef __LP64__
#include <linux/log2.h>
#endif
#include <linux/netdevice.h>

SYSCTL_DECL(_compat_linuxkpi);
SYSCTL_NODE(_compat_linuxkpi, OID_AUTO, skb, CTLFLAG_RW | CTLFLAG_MPSAFE, 0,
    "LinuxKPI skbuff");

#ifdef SKB_DEBUG
int linuxkpi_debug_skb;
SYSCTL_INT(_compat_linuxkpi_skb, OID_AUTO, debug, CTLFLAG_RWTUN,
    &linuxkpi_debug_skb, 0, "SKB debug level");
#endif

#ifdef __LP64__
/*
 * Realtek wireless drivers (e.g., rtw88) require 32bit DMA in a single segment.
 * busdma(9) has a hard time providing this currently for 3-ish pages at large
 * quantities (see lkpi_pci_nseg1_fail in linux_pci.c).
 * Work around this for now by allowing a tunable to enforce physical addresses
 * allocation limits on 64bit platforms using "old-school" contigmalloc(9) to
 * avoid bouncing.
 */
static int linuxkpi_skb_memlimit;
SYSCTL_INT(_compat_linuxkpi_skb, OID_AUTO, mem_limit, CTLFLAG_RDTUN,
    &linuxkpi_skb_memlimit, 0, "SKB memory limit: 0=no limit, "
    "1=32bit, 2=36bit, other=undef (currently 32bit)");
#endif

static MALLOC_DEFINE(M_LKPISKB, "lkpiskb", "Linux KPI skbuff compat");

struct sk_buff *
linuxkpi_alloc_skb(size_t size, gfp_t gfp)
{
	struct sk_buff *skb;
	size_t len;

	len = sizeof(*skb) + size + sizeof(struct skb_shared_info);
	/*
	 * Using our own type here not backing my kmalloc.
	 * We assume no one calls kfree directly on the skb.
	 */
#ifdef __LP64__
	if (__predict_true(linuxkpi_skb_memlimit == 0)) {
		skb = malloc(len, M_LKPISKB, linux_check_m_flags(gfp) | M_ZERO);
	} else {
		vm_paddr_t high;

		switch (linuxkpi_skb_memlimit) {
		case 2:
			high = (0xfffffffff);	/* 1<<36 really. */
			break;
		case 1:
		default:
			high = (0xffffffff);	/* 1<<32 really. */
			break;
		}
		len = roundup_pow_of_two(len);
		skb = contigmalloc(len, M_LKPISKB,
		    linux_check_m_flags(gfp) | M_ZERO, 0, high, PAGE_SIZE, 0);
	}
#else
	skb = malloc(len, M_LKPISKB, linux_check_m_flags(gfp) | M_ZERO);
#endif
	if (skb == NULL)
		return (skb);
	skb->_alloc_len = len;
	skb->truesize = size;

	skb->head = skb->data = skb->tail = (uint8_t *)(skb+1);
	skb->end = skb->head + size;

	skb->prev = skb->next = skb;

	skb->shinfo = (struct skb_shared_info *)(skb->end);
	memset(skb->shinfo, 0, sizeof *skb->shinfo);

	SKB_TRACE_FMT(skb, "data %p size %zu", (skb) ? skb->data : NULL, size);
	return (skb);
}

struct sk_buff *
linuxkpi_dev_alloc_skb(size_t size, gfp_t gfp)
{
	struct sk_buff *skb;
	size_t len;

	len = size + NET_SKB_PAD;
	skb = linuxkpi_alloc_skb(len, gfp);

	if (skb != NULL)
		skb_reserve(skb, NET_SKB_PAD);

	SKB_TRACE_FMT(skb, "data %p size %zu len %zu",
	    (skb) ? skb->data : NULL, size, len);
	return (skb);
}

struct sk_buff *
linuxkpi_build_skb(void *data, size_t fragsz)
{
	struct sk_buff *skb;

	if (data == NULL || fragsz == 0)
		return (NULL);

	/* Just allocate a skb without data area. */
	skb = linuxkpi_alloc_skb(0, GFP_KERNEL);
	if (skb == NULL)
		return (NULL);

	skb->_flags |= _SKB_FLAGS_SKBEXTFRAG;
	skb->truesize = fragsz;
	skb->head = skb->data = data;
	skb_reset_tail_pointer(skb);	/* XXX is that correct? */
	skb->end = (void *)((uintptr_t)skb->head + fragsz);

	return (skb);
}

static struct sk_buff *
copy_internal(struct sk_buff *new, struct sk_buff *skb)
{
	struct skb_shared_info *shinfo;

	// headroom = skb_headroom(skb);
	// /* Fixup head and end. */
	// skb_reserve(new, headroom);	/* data and tail move headroom forward. */
	// skb_put(new, skb->len);		/* tail and len get adjusted */

	// /* Copy data. */
	// memcpy(new->head, skb->data - headroom, headroom + skb->len);

	/* Deal with fragments. */
	shinfo = skb->shinfo;
	if (shinfo->nr_frags > 0) {
		printf("%s:%d: NOT YET SUPPORTED; missing %d frags\n",
		    __func__, __LINE__, shinfo->nr_frags);
		SKB_TODO();
	}

	/* Deal with header fields. */
	memcpy(new->cb, skb->cb, sizeof(skb->cb));
	SKB_IMPROVE("more header fields to copy?");

	new->mac_header = skb->mac_header;
	new->network_header = skb->network_header;
	new->transport_header = skb->transport_header;

	return (new);
}

struct sk_buff *
linuxkpi_skb_clone(struct sk_buff *skb, gfp_t gfp)
{
	struct sk_buff *new;

	new = linuxkpi_alloc_skb(0, gfp);
	if (new == NULL)
		return (NULL);

	new = copy_internal(skb, new);
	return (new);
}

struct sk_buff *
linuxkpi_skb_copy(struct sk_buff *skb, gfp_t gfp)
{
	struct sk_buff *new;
	size_t len;

	/* Full buffer size + any fragments. */
	len = skb->end - skb->head + skb->data_len;

	new = linuxkpi_alloc_skb(len, gfp);
	if (new == NULL)
		return (NULL);

	new = copy_internal(new, skb);
	if (new == NULL)
		return (NULL);

	new->data = new->head + (skb->data - skb->head);
	new->tail = new->head + (skb->tail - skb->head);

	memcpy(new->head, skb->head, skb->end - skb->head);
	return (new);
}

static int
resolve_addr(struct ifnet *ifp, struct mbuf *m, struct sockaddr const *dst,
	struct route *ro, u_char *phdr, uint32_t *pflags, struct llentry **plle)
{
	/*
	 * XXX This fella is adapted from ether_resolve_addr.
	 * Make this more permanent and less lobotomized.
	 */

	uint32_t lleflags = 0;
	int error = 0;
	struct ether_header *eh = (struct ether_header *)phdr;
	uint16_t etype;

	if (dst == NULL)
		return (ENOTSUP);

	switch (dst->sa_family) {
	case AF_INET:
		if ((m->m_flags & (M_BCAST | M_MCAST)) == 0)
			error = arpresolve(ifp, 0, m, dst, phdr, &lleflags,
			    NULL);
		else {
			if (m->m_flags & M_BCAST)
				memcpy(eh->ether_dhost, ifp->if_broadcastaddr,
				    ETHER_ADDR_LEN);
			else {
				const struct in_addr *a;
				a = &(((const struct sockaddr_in *)dst)->sin_addr);
				ETHER_MAP_IP_MULTICAST(a, eh->ether_dhost);
			}
			etype = htons(ETHERTYPE_IP);
			memcpy(&eh->ether_type, &etype, sizeof(etype));
			memcpy(eh->ether_shost, IF_LLADDR(ifp), ETHER_ADDR_LEN);
		}
		break;
	default:
		if_printf(ifp, "can't handle af%d\n", dst->sa_family);
		if (m != NULL)
			m_freem(m);
		return (EAFNOSUPPORT);
	}

	if (error == EHOSTDOWN) {
		if (ro != NULL && (ro->ro_flags & RT_HAS_GW) != 0)
			error = EHOSTUNREACH;
	}

	if (error != 0)
		return (error);

	*pflags = RT_MAY_LOOP;
	if (lleflags & LLE_IFADDR)
		*pflags |= RT_L2_ME;

	return (error);
}

struct sk_buff *
linuxkpi_skb_from_mbuf(struct net_device *dev, struct mbuf *m,
	struct sockaddr const *dst, struct route *ro, int headroom)
{
	if_t const ifp = (if_t)dev;
	size_t payload_len;
	struct sk_buff *skb;
	char linkhdr[ETHER_HDR_LEN], *phdr = NULL;
	uint32_t pflags;
	size_t hlen = 0;
	struct llentry *lle = NULL;
	int addref = 0;
	int error;

	/*
	 * We first gotta see if we're provided a link layer header.
	 * XXX Take a look at ether_output. This is probably what we wanna do.
	 */
	if (ro != NULL) {
		if (ro->ro_prepend != NULL) {
			phdr = ro->ro_prepend;
			hlen = ro->ro_plen;
		} else if (!(m->m_flags & (M_BCAST | M_MCAST))) {
			if ((ro->ro_flags & RT_LLE_CACHE) != 0) {
				lle = ro->ro_lle;
				if (lle != NULL &&
				    (lle->la_flags & LLE_VALID) == 0) {
					LLE_FREE(lle);
					lle = NULL;	/* redundant */
					ro->ro_lle = NULL;
				}
				if (lle == NULL) {
					/* if we lookup, keep cache */
					addref = 1;
				} else
					/*
					 * Notify LLE code that
					 * the entry was used
					 * by datapath.
					 */
					llentry_provide_feedback(lle);
			}
			if (lle != NULL) {
				phdr = lle->r_linkdata;
				hlen = lle->r_hdrlen;
				pflags = lle->r_flags;
			}
		}
	}

	/* If not, figure one out. */
	if (phdr == NULL) {
		error = resolve_addr(ifp, m, dst, ro, linkhdr, &pflags,
		    addref ? &lle : NULL);
		if (error == 0) {
			phdr = linkhdr;
			hlen = sizeof linkhdr;
		}
		else if (error == EWOULDBLOCK)
			return (NULL);
	}

	/* Add header once/if we've got one. */
	M_PREPEND(m, hlen, M_NOWAIT);
	if (m == NULL)
		return (NULL);
	if ((pflags & RT_HAS_HEADER) == 0)
		memcpy(mtod(m, void *), phdr, hlen);
recv:
	payload_len = m_length(m, NULL);
	skb = dev_alloc_skb(headroom + payload_len);
	if (skb == NULL)
		return (NULL);

	skb->data = skb->head + headroom;
	skb->tail = skb->data + payload_len;
	skb->len = skb->tail - skb->data;
	m_copydata(m, 0, payload_len, skb->data);

	/*
	 * TODO this should really be done only in linuxkpi_alloc_skb, not sure why
	 * it's not working correctly...
	 */
	memset(skb->shinfo, 0, sizeof *skb->shinfo);
	return (skb);
}

void
linuxkpi_kfree_skb(struct sk_buff *skb)
{
	struct skb_shared_info *shinfo;
	uint16_t fragno, count;

	SKB_TRACE(skb);
	if (skb == NULL)
		return;

	/*
	 * XXX TODO this will go away once we have skb backed by mbuf.
	 * currently we allow the mbuf to stay around and use a private
	 * free function to allow secondary resources to be freed along.
	 */
	if (skb->m != NULL) {
		void *m;

		m = skb->m;
		skb->m = NULL;

		KASSERT(skb->m_free_func != NULL, ("%s: skb %p has m %p but no "
		    "m_free_func %p\n", __func__, skb, m, skb->m_free_func));
		skb->m_free_func(m);
	}
	KASSERT(skb->m == NULL,
	    ("%s: skb %p m %p != NULL\n", __func__, skb, skb->m));

	shinfo = skb->shinfo;
	for (count = fragno = 0;
	    count < shinfo->nr_frags && fragno < nitems(shinfo->frags);
	    fragno++) {

		if (shinfo->frags[fragno].page != NULL) {
			struct page *p;

			p = shinfo->frags[fragno].page;
			shinfo->frags[fragno].size = 0;
			shinfo->frags[fragno].offset = 0;
			shinfo->frags[fragno].page = NULL;
			__free_page(p);
			count++;
		}
	}

	if ((skb->_flags & _SKB_FLAGS_SKBEXTFRAG) != 0) {
		void *p;

		p = skb->head;
		skb_free_frag(p);
	}

#ifdef __LP64__
	if (__predict_true(linuxkpi_skb_memlimit == 0))
		free(skb, M_LKPISKB);
	else
		contigfree(skb, skb->_alloc_len, M_LKPISKB);
#else
	free(skb, M_LKPISKB);
#endif
}

#ifdef DDB
DB_SHOW_COMMAND(skb, db_show_skb)
{
	struct sk_buff *skb;
	int i;

	if (!have_addr) {
		db_printf("usage: show skb <addr>\n");
			return;
	}

	skb = (struct sk_buff *)addr;

	db_printf("skb %p\n", skb);
	db_printf("\tnext %p prev %p\n", skb->next, skb->prev);
	db_printf("\tlist %p\n", &skb->list);
	db_printf("\t_alloc_len %u len %u data_len %u truesize %u mac_len %u\n",
	    skb->_alloc_len, skb->len, skb->data_len, skb->truesize,
	    skb->mac_len);
	db_printf("\tcsum %#06x network_header %u transport_header %u priority %u qmap %u\n",
	    skb->csum, skb->network_header, skb->transport_header, skb->priority, skb->qmap);
	db_printf("\tpkt_type %d dev %p sk %p\n",
	    skb->pkt_type, skb->dev, skb->sk);
	db_printf("\tcsum_offset %d csum_start %d ip_summed %d protocol %d\n",
	    skb->csum_offset, skb->csum_start, skb->ip_summed, skb->protocol);
	db_printf("\t_flags %#06x\n", skb->_flags);		/* XXX-BZ print names? */
	db_printf("\thead %p data %p tail %p end %p\n",
	    skb->head, skb->data, skb->tail, skb->end);
	db_printf("\tshinfo %p m %p m_free_func %p\n",
	    skb->shinfo, skb->m, skb->m_free_func);

	if (skb->shinfo != NULL) {
		struct skb_shared_info *shinfo;

		shinfo = skb->shinfo;
		db_printf("\t\tgso_type %d gso_size %u nr_frags %u\n",
		    shinfo->gso_type, shinfo->gso_size, shinfo->nr_frags);
		for (i = 0; i < nitems(shinfo->frags); i++) {
			struct skb_frag *frag;

			frag = &shinfo->frags[i];
			if (frag == NULL || frag->page == NULL)
				continue;
			db_printf("\t\t\tfrag %p fragno %d page %p %p "
			    "offset %ju size %zu\n",
			    frag, i, frag->page, linux_page_address(frag->page),
			    (uintmax_t)frag->offset, frag->size);
		}
	}
	db_printf("\tcb[] %p {", skb->cb);
	for (i = 0; i < nitems(skb->cb); i++) {
		db_printf("%#04x%s",
		    skb->cb[i], (i < (nitems(skb->cb)-1)) ? ", " : "");
	}
	db_printf("}\n");

	db_printf("\t__scratch[0] %p\n", skb->__scratch);
};
#endif
