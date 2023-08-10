/*-
 * Copyright (c) 2010 Isilon Systems, Inc.
 * Copyright (c) 2010 iX Systems, Inc.
 * Copyright (c) 2010 Panasas, Inc.
 * Copyright (c) 2013-2019 Mellanox Technologies, Ltd.
 * All rights reserved.
 * Copyright (c) 2020-2021 The FreeBSD Foundation
 * Copyright (c) 2020-2022 Bjoern A. Zeeb
 * Copyright (c) 2023 Aymeric Wibo <obiwac@freebsd.org>
 *
 * Portions of this software were developed by Björn Zeeb
 * under sponsorship from the FreeBSD Foundation.
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
#ifndef	_LINUXKPI_LINUX_NETDEVICE_H
#define	_LINUXKPI_LINUX_NETDEVICE_H

#include <linux/types.h>
#include <linux/netdev_features.h>

#include <sys/param.h>
#include <sys/kernel.h>
#include <sys/lock.h>
#include <sys/mutex.h>
#include <sys/malloc.h>
#include <sys/queue.h>
#include <sys/socket.h>
#include <sys/taskqueue.h>

#include <net/if_types.h>
#include <net/if.h>
#include <net/if_var.h>
#include <net/if_dl.h>

#include <linux/bitops.h>
#include <linux/device.h>
#include <linux/if_ether.h>
#include <linux/kernel.h>
#include <linux/list.h>
#include <linux/net.h>
#include <linux/netlink.h>
#include <linux/notifier.h>
#include <linux/random.h>
#include <linux/rcupdate.h>

#ifdef VIMAGE
#define	init_net *vnet0
#else
#define	init_net *((struct vnet *)0)
#endif

struct sk_buff;
struct net_device;
struct wireless_dev;		/* net/cfg80211.h */

#define	MAX_ADDR_LEN		20

#define	NET_NAME_UNKNOWN	0

enum netdev_tx {
	NETDEV_TX_OK		= 0x00,
	NETDEV_TX_BUSY		= 0x10,
};
typedef	enum netdev_tx		netdev_tx_t;

struct netdev_hw_addr {
	struct list_head	addr_list;
	uint8_t			addr[MAX_ADDR_LEN];
};

struct netdev_hw_addr_list {
	struct list_head	addr_list;
	int			count;
};

enum net_device_reg_state {
	NETREG_DUMMY		= 1,
	NETREG_REGISTERED,
};

struct net_device_ops {
	int			(*ndo_open)(struct net_device *);
	int			(*ndo_stop)(struct net_device *);
	int			(*ndo_set_mac_address)(struct net_device *,  void *);
	netdev_tx_t		(*ndo_start_xmit)(struct sk_buff *, struct net_device *);
	void			(*ndo_set_rx_mode)(struct net_device *);
	int			(*ndo_init)(struct net_device *);
	struct net_device_stats	*(*ndo_get_stats)(struct net_device *);
	int			(*ndo_vlan_rx_add_vid)(struct net_device *, __be16, u16);
	int			(*ndo_vlan_rx_kill_vid)(struct net_device *, __be16, u16);
	int			(*ndo_change_mtu)(struct net_device *, int);
	int			(*ndo_validate_addr)(struct net_device *);
	int			(*ndo_add_slave)(struct net_device *, struct net_device *, struct netlink_ext_ack *);
	int			(*ndo_del_slave)(struct net_device *, struct net_device *);
};

struct net_device_stats {
	unsigned long		multicast;

	unsigned long		rx_bytes;
	unsigned long		rx_errors;
	unsigned long		rx_packets;
	unsigned long		tx_bytes;
	unsigned long		tx_dropped;
	unsigned long		tx_errors;
	unsigned long		tx_packets;
};

struct net_device {
	/*
	 * struct ifnet, with aliases to struct net_device equivalents,
	 * plus struct net_device-only fields.
	 */

	/* General book keeping of interface lists. */
	CK_STAILQ_ENTRY(ifnet) if_link; 	/* all struct ifnets are chained (CK_) */
	LIST_ENTRY(ifnet) if_clones;	/* interfaces of a cloner */
	CK_STAILQ_HEAD(, ifg_list) if_groups; /* linked list of groups per if (CK_) */
					/* protected by if_addr_lock */
	u_char	if_alloctype;		/* if_type at time of allocation */
	uint8_t	if_numa_domain;		/* NUMA domain of device */
	/* Driver and protocol specific information that remains stable. */
	void	*if_softc;		/* pointer to driver state */
	void	*if_llsoftc;		/* link layer softc */
	void	*if_l2com;		/* pointer to protocol bits */
	const char *if_dname;		/* driver name */
	int	if_dunit;		/* unit or IF_DUNIT_NONE */
	u_short	ifindex;		/* numeric abbreviation for this if  */
	u_short	if_idxgen;		/* ... and its generation count */
	char	name[IFNAMSIZ];		/* external name (name + unit) */
	char	*if_description;	/* interface description */

	/* Variable fields that are touched by the stack and drivers. */
	int	flags;			/* up/down, broadcast, etc. */
	int	priv_flags;		/* driver-managed status flags */
	int	if_capabilities;	/* interface features & capabilities */
	int	if_capabilities2;	/* part 2 */
	int	if_capenable;		/* enabled features & capabilities */
	int	if_capenable2;		/* part 2 */
	void	*if_linkmib;		/* link-type-specific MIB data */
	size_t	if_linkmiblen;		/* length of above data */
	u_int	if_refcount;		/* reference count */

	/* These fields are shared with struct if_data. */
	uint8_t		type;		/* ethernet, tokenring, etc */
	uint8_t		addr_len;	/* media address length */
	uint8_t		if_hdrlen;	/* media header length */
	uint8_t		if_link_state;	/* current link state */
	uint32_t	mtu;		/* maximum transmission unit */
	uint32_t	if_metric;	/* routing metric (external only) */
	uint64_t	if_baudrate;	/* linespeed */
	uint64_t	if_hwassist;	/* HW offload capabilities, see IFCAP */
	time_t		if_epoch;	/* uptime at attach or stat reset */
	struct timeval	if_lastchange;	/* time of last administrative change */

	struct  ifaltq if_snd;		/* output queue (includes altq) */
	struct	task if_linktask;	/* task for link change events */
	struct	task if_addmultitask;	/* task for SIOCADDMULTI */

	struct ifnet	*if_master;	/* master interface */
	int		(*if_slavefn)(struct mbuf *, if_t, if_t); /* slave forwarding function */

	/* Addresses of different protocol families assigned to this if. */
	struct mtx if_addr_lock;	/* lock to protect address lists */
		/*
		 * if_addrhead is the list of all addresses associated to
		 * an interface.
		 * Some code in the kernel assumes that first element
		 * of the list has type AF_LINK, and contains sockaddr_dl
		 * addresses which store the link-level address and the name
		 * of the interface.
		 * However, access to the AF_LINK address through this
		 * field is deprecated. Use if_addr instead.
		 */
	struct	ifaddrhead if_addrhead;	/* linked list of addresses per if */
	struct	ifmultihead if_multiaddrs; /* multicast addresses configured */
	int	if_amcount;		/* number of all-multicast requests */
	struct	ifaddr	*if_addr;	/* pointer to link-level address */
	void	*if_hw_addr;		/* hardware link-level address */
	const u_int8_t *if_broadcastaddr; /* linklevel broadcast bytestring */
	struct	mtx if_afdata_lock;
	void	*if_afdata[AF_MAX];
	int	if_afdata_initialized;

	/* Additional features hung off the interface. */
	u_int	if_fib;			/* interface FIB */
	struct	vnet *if_vnet;		/* pointer to network stack instance */
	struct	vnet *if_home_vnet;	/* where this ifnet originates from */
	struct  ifvlantrunk *if_vlantrunk; /* pointer to 802.1q data */
	struct	bpf_if *if_bpf;		/* packet filter structure */
	int	if_pcount;		/* number of promiscuous listeners */
	void	*if_bridge;		/* bridge glue */
	void	*if_lagg;		/* lagg glue */
	void	*if_pf_kif;		/* pf glue */
	struct	carp_if *if_carp;	/* carp interface structure */
	struct	label *if_label;	/* interface MAC label */
	struct	netmap_adapter *if_netmap; /* netmap(4) softc */

	/* Various procedures of the layer2 encapsulation and drivers. */
	if_output_fn_t if_output;	/* output routine (enqueue) */
	if_input_fn_t if_input;		/* input routine (from h/w driver) */
	struct mbuf *(*if_bridge_input)(struct ifnet *, struct mbuf *);
	int	(*if_bridge_output)(struct ifnet *, struct mbuf *, struct sockaddr *,
		    struct rtentry *);
	void (*if_bridge_linkstate)(struct ifnet *ifp);
	if_start_fn_t	if_start;	/* initiate output routine */
	if_ioctl_fn_t	if_ioctl;	/* ioctl routine */
	if_init_fn_t	if_init;	/* Init routine */
	int	(*if_resolvemulti)	/* validate/resolve multicast */
		(struct ifnet *, struct sockaddr **, struct sockaddr *);
	if_qflush_fn_t	if_qflush;	/* flush any queue */
	if_transmit_fn_t if_transmit;   /* initiate output routine */

	if_reassign_fn_t if_reassign;		/* reassign to vnet routine */
	if_get_counter_t if_get_counter; /* get counter values */
	int	(*if_requestencap)	/* make link header from request */
		(struct ifnet *, struct if_encap_req *);

	/* Statistics. */
	counter_u64_t	if_counters[IFCOUNTERS];

	/* Stuff that's only temporary and doesn't belong here. */

	/*
	 * Network adapter TSO limits:
	 * ===========================
	 *
	 * If the "if_hw_tsomax" field is zero the maximum segment
	 * length limit does not apply. If the "if_hw_tsomaxsegcount"
	 * or the "if_hw_tsomaxsegsize" field is zero the TSO segment
	 * count limit does not apply. If all three fields are zero,
	 * there is no TSO limit.
	 *
	 * NOTE: The TSO limits should reflect the values used in the
	 * BUSDMA tag a network adapter is using to load a mbuf chain
	 * for transmission. The TCP/IP network stack will subtract
	 * space for all linklevel and protocol level headers and
	 * ensure that the full mbuf chain passed to the network
	 * adapter fits within the given limits.
	 */
	u_int	if_hw_tsomax;		/* TSO maximum size in bytes */
	u_int	if_hw_tsomaxsegcount;	/* TSO maximum segment count */
	u_int	if_hw_tsomaxsegsize;	/* TSO maximum segment size in bytes */

	/*
	 * Network adapter send tag support:
	 */
	if_snd_tag_alloc_t *if_snd_tag_alloc;

	/* Ratelimit (packet pacing) */
	if_ratelimit_query_t *if_ratelimit_query;
	if_ratelimit_setup_t *if_ratelimit_setup;

	/* Ethernet PCP */
	uint8_t if_pcp;

	/*
	 * Debugnet (Netdump) hooks to be called while in db/panic.
	 */
	struct debugnet_methods *if_debugnet_methods;
	struct epoch_context	if_epoch_ctx;

	/*
	 * Spare fields to be added before branching a stable branch, so
	 * that structure can be enhanced without changing the kernel
	 * binary interface.
	 */
	int	if_ispare[4];		/* general use */

	/*
	 * Extra fields only in struct net_device, not struct ifnet.
	 */
	netdev_features_t		features;
	struct net_device_stats		stats;
	enum net_device_reg_state	reg_state;

	unsigned short			hard_header_len;
	int				needed_headroom, needed_tailroom;

	uint8_t				dev_addr[ETH_ALEN];
	struct netdev_hw_addr_list	mc;
	uint8_t				broadcast[ETH_ALEN];

	const struct rtnl_link_ops	*rtnl_link_ops;
	const struct ethtool_ops	*ethtool_ops;
	const struct net_device_ops	*netdev_ops;

	bool				needs_free_netdev;
	void (*priv_destructor)(struct net_device *);

	/* net_device internal. */
	struct device			dev;

	/*
	 * In case we delete the net_device we need to be able to clear all
	 * NAPI consumers.
	 */
	struct mtx			napi_mtx;
	TAILQ_HEAD(, napi_struct)	napi_head;
	struct taskqueue		*napi_tq;

	/* Must stay last. */
	uint8_t				drv_priv[0] __aligned(CACHE_LINE_SIZE);
};

#define	SET_NETDEV_DEV(_ndev, _dev)	(_ndev)->dev.parent = _dev;

/* -------------------------------------------------------------------------- */
/* According to linux::ipoib_main.c. */
struct netdev_notifier_info {
	struct net_device	*dev;
	struct ifnet		*ifp;
};

static inline struct net_device *
netdev_notifier_info_to_dev(struct netdev_notifier_info *ni)
{
	return (ni->dev);
}

static inline struct ifnet *
netdev_notifier_info_to_ifp(struct netdev_notifier_info *ni)
{
	return (ni->ifp);
}

int	register_netdevice_notifier(struct notifier_block *);
int	register_inetaddr_notifier(struct notifier_block *);
int	unregister_netdevice_notifier(struct notifier_block *);
int	unregister_inetaddr_notifier(struct notifier_block *);

/* -------------------------------------------------------------------------- */

#define	NAPI_POLL_WEIGHT			64	/* budget */

/*
 * There are drivers directly testing napi state bits, so we need to publicly
 * expose them.  If you ask me, those accesses should be hid behind an
 * inline function and the bit flags not be directly exposed.
 */
enum napi_state_bits {
	/*
	 * Official Linux flags encountered.
	 */
	NAPI_STATE_SCHED = 1,

	/*
	 * Our internal versions (for now).
	 */
	/* Do not schedule new things while we are waiting to clear things. */
	LKPI_NAPI_FLAG_DISABLE_PENDING = 0,
	/* To synchronise that only one poll is ever running. */
	LKPI_NAPI_FLAG_IS_SCHEDULED = 1,
	/* If trying to schedule while poll is running. Need to re-schedule. */
	LKPI_NAPI_FLAG_LOST_RACE_TRY_AGAIN = 2,
	/* When shutting down forcefully prevent anything from running task/poll. */
	LKPI_NAPI_FLAG_SHUTDOWN = 3,
};

struct napi_struct {
	TAILQ_ENTRY(napi_struct)	entry;

	struct list_head	rx_list;
	struct net_device	*dev;
	int			(*poll)(struct napi_struct *, int);
	int			budget;
	int			rx_count;


	/*
	 * These flags mostly need to be checked/changed atomically
	 * (multiple together in some cases).
	 */
	volatile unsigned long	state;

	/* FreeBSD internal. */
	/* Use task for now, so we can easily switch between direct and task. */
	struct task		napi_task;
};

void linuxkpi_init_dummy_netdev(struct net_device *);
void linuxkpi_netif_napi_add(struct net_device *, struct napi_struct *,
    int(*napi_poll)(struct napi_struct *, int));
void linuxkpi_netif_napi_del(struct napi_struct *);
bool linuxkpi_napi_schedule_prep(struct napi_struct *);
void linuxkpi___napi_schedule(struct napi_struct *);
void linuxkpi_napi_schedule(struct napi_struct *);
void linuxkpi_napi_reschedule(struct napi_struct *);
bool linuxkpi_napi_complete_done(struct napi_struct *, int);
bool linuxkpi_napi_complete(struct napi_struct *);
void linuxkpi_napi_disable(struct napi_struct *);
void linuxkpi_napi_enable(struct napi_struct *);
void linuxkpi_napi_synchronize(struct napi_struct *);

#define	init_dummy_netdev(_n)						\
	linuxkpi_init_dummy_netdev(_n)
#define	netif_napi_add(_nd, _ns, _p)					\
	linuxkpi_netif_napi_add(_nd, _ns, _p)
#define	netif_napi_del(_n)						\
	linuxkpi_netif_napi_del(_n)
#define	napi_schedule_prep(_n)						\
	linuxkpi_napi_schedule_prep(_n)
#define	__napi_schedule(_n)						\
	linuxkpi___napi_schedule(_n)
#define	napi_schedule(_n)						\
	linuxkpi_napi_schedule(_n)
#define	napi_reschedule(_n)						\
	linuxkpi_napi_reschedule(_n)
#define	napi_complete_done(_n, _r)					\
	linuxkpi_napi_complete_done(_n, _r)
#define	napi_complete(_n)						\
	linuxkpi_napi_complete(_n)
#define	napi_disable(_n)						\
	linuxkpi_napi_disable(_n)
#define	napi_enable(_n)							\
	linuxkpi_napi_enable(_n)
#define	napi_synchronize(_n)						\
	linuxkpi_napi_synchronize(_n)


static inline void
netif_napi_add_tx(struct net_device *dev, struct napi_struct *napi,
    int(*napi_poll)(struct napi_struct *, int))
{

	netif_napi_add(dev, napi, napi_poll);
}

/* -------------------------------------------------------------------------- */

static inline void
netdev_rss_key_fill(uint32_t *buf, size_t len)
{

	/*
	 * Remembering from a previous life there was discussions on what is
	 * a good RSS hash key.  See end of rss_init() in net/rss_config.c.
	 * iwlwifi is looking for a 10byte "secret" so stay with random for now.
	 */
	get_random_bytes(buf, len);
}

static inline int
netdev_hw_addr_list_count(struct netdev_hw_addr_list *list)
{

	return (list->count);
}

static inline int
netdev_mc_count(struct net_device *ndev)
{

	return (netdev_hw_addr_list_count(&ndev->mc));
}

#define	netdev_hw_addr_list_for_each(_addr, _list)			\
	list_for_each_entry((_addr), &(_list)->addr_list, addr_list)

#define	netdev_for_each_mc_addr(na, ndev)				\
	netdev_hw_addr_list_for_each(na, &(ndev)->mc)

static __inline void
synchronize_net(void)
{

	/* We probably cannot do that unconditionally at some point anymore. */
	synchronize_rcu();
}

static __inline void
netif_receive_skb_list(struct list_head *head)
{

	pr_debug("%s: TODO\n", __func__);
}

static __inline int
napi_gro_receive(struct napi_struct *napi, struct sk_buff *skb)
{

	pr_debug("%s: TODO\n", __func__);
	return (-1);
}

static __inline void
ether_setup(struct net_device *ndev)
{

	pr_debug("%s: TODO\n", __func__);
}

static __inline void
dev_net_set(struct net_device *ndev, void *p)
{

	pr_debug("%s: TODO\n", __func__);
}

static __inline int
dev_set_threaded(struct net_device *ndev, bool threaded)
{

	pr_debug("%s: TODO\n", __func__);
	return (-ENODEV);
}

/* -------------------------------------------------------------------------- */

static __inline bool
netif_carrier_ok(struct net_device *ndev)
{
	pr_debug("%s: TODO\n", __func__);
	return (false);
}

static __inline void
netif_carrier_off(struct net_device *ndev)
{
	pr_debug("%s: TODO\n", __func__);
}

static __inline void
netif_carrier_on(struct net_device *ndev)
{
	pr_debug("%s: TODO\n", __func__);
}

/* -------------------------------------------------------------------------- */

static __inline bool
netif_queue_stopped(struct net_device *ndev)
{
	pr_debug("%s: TODO\n", __func__);
	return (false);
}

static __inline void
netif_stop_queue(struct net_device *ndev)
{
	pr_debug("%s: TODO\n", __func__);
}

static __inline void
netif_wake_queue(struct net_device *ndev)
{
	pr_debug("%s: TODO\n", __func__);
}

/* -------------------------------------------------------------------------- */

static __inline int
register_netdevice(struct net_device *ndev)
{

	/* assert rtnl_locked? */
	pr_debug("%s: TODO\n", __func__);
	return (0);
}

static __inline int
register_netdev(struct net_device *ndev)
{
	int error;

	/* lock */
	error = register_netdevice(ndev);
	/* unlock */
	pr_debug("%s: TODO\n", __func__);
	return (error);
}

static __inline void
unregister_netdev(struct net_device *ndev)
{
	pr_debug("%s: TODO\n", __func__);
}

static inline void
unregister_netdevice_queue(struct net_device *dev, struct list_head *head)
{

	pr_debug("%s: TODO\n", __func__);
}

static __inline void
unregister_netdevice(struct net_device *ndev)
{
	pr_debug("%s: TODO\n", __func__);
}

/* -------------------------------------------------------------------------- */

static __inline void
netif_rx(struct sk_buff *skb)
{
	struct net_device *const dev = skb->dev;
	if_t const ifp = __DECONST(if_t, dev);
	size_t const len = skb->tail - skb->data;
	struct mbuf *m;

	/* Create mbuf from skbuff. */
	/* TODO Should this be a M_EXT? What is M_PKTHDR for? */

	m = m_get3(len, M_WAITOK, MT_DATA, M_PKTHDR);
	if (m == NULL)
		return;
	m->m_pkthdr.rcvif = ifp;
	m->m_pkthdr.len = len;
	m->m_len = len;

	memcpy(mtod(m, uint8_t *), skb->data, len);

	ifp->if_input(ifp, m);
}

static __inline void
netif_rx_ni(struct sk_buff *skb)
{
	pr_debug("%s: TODO\n", __func__);
}

/* -------------------------------------------------------------------------- */

struct net_device *linuxkpi_alloc_netdev(size_t, const char *, uint32_t,
	void(*)(struct net_device *));
struct net_device *linuxkpi_alloc_netdev_ifp(size_t, u_char,
	void(*)(struct net_device *));
void linuxkpi_free_netdev(struct net_device *);

#define	alloc_netdev(_l, _n, _f, _func)						\
	linuxkpi_alloc_netdev(_l, _n, _f, _func)
#define	free_netdev(_n)								\
	linuxkpi_free_netdev(_n)

static inline void *
netdev_priv(const struct net_device *ndev)
{

	return (__DECONST(void *, ndev->drv_priv));
}

static __inline void
dev_put(struct net_device *dev)
{

	pr_debug("%s: TODO\n", __func__);
}

/* -------------------------------------------------------------------------- */
/* This is really rtnetlink and probably belongs elsewhere. */

#define	rtnl_lock()		do { } while(0)
#define	rtnl_unlock()		do { } while(0)
#define	rcu_dereference_rtnl(x)	READ_ONCE(x)

typedef struct {} netdevice_tracker;

struct packet_type {
	__be16			type;	/* This is really htons(ether_type). */
	bool			ignore_outgoing;
	struct net_device	*dev;	/* NULL is wildcarded here	     */
	netdevice_tracker	dev_tracker;
	int			(*func) (struct sk_buff *,
					 struct net_device *,
					 struct packet_type *,
					 struct net_device *);
	void			(*list_func) (struct list_head *,
					      struct packet_type *,
					      struct net_device *);
	struct net		*af_packet_net;
	void			*af_packet_priv;
	struct list_head	list;
};

#define	NET_RX_SUCCESS	0
#define	NET_RX_DROP	1

static inline struct net_device *
linux_dev_get_by_index(struct net *net, int ifindex)
{
	struct epoch_tracker et;
	struct ifnet *ifp;

	NET_EPOCH_ENTER(et);
	ifp = ifnet_byindex(ifindex);
	NET_EPOCH_EXIT(et);

	/* TODO explain why we just cast. */
	/* https://github.com/luigirizzo/netmap/blob/master/LINUX/bsd_glue.h */

	pr_debug("TODO: %s\n", __func__);
	return ((struct net_device *)ifp);
}

#define	NET_XMIT_SUCCESS	0x00
#define	NET_XMIT_DROP	0x01
#define	NET_XMIT_CN	0x02
#define	NET_XMIT_MASK	0x0F

#define	HH_DATA_MOD	16

// TODO rewrite this

#define	LL_RESERVED_SPACE(dev)						\
	((((dev)->hard_header_len + READ_ONCE((dev)->needed_headroom))	\
	& ~(HH_DATA_MOD - 1)) + HH_DATA_MOD)

static inline int
dev_queue_xmit(struct sk_buff *skb)
{
	struct net_device *const dev = skb->dev;
	if_t const ifp = __DECONST(if_t, dev);
	struct ethhdr *ethhdr;
	struct sockaddr dst;
	struct route ro = {0};
	size_t const len = skb->tail - skb->data;
	struct mbuf *m;

	/* Create mbuf from skbuff. */
	/* TODO Should this be a M_EXT? What is M_PKTHDR for? */

	m = m_get3(len, M_WAITOK, MT_DATA, M_PKTHDR);
	if (m == NULL)
		return (EIO);
	m->m_pkthdr.len = len;
	m->m_len = len;

	skb_reset_mac_header(skb);
	memcpy(mtod(m, uint8_t *), skb->data, len);
	// m->m_ext.ext_size = MCLBYTES;
	// memcpy(m->m_ext.ext_buf, skb->data, len);

	/* Create destination struct. */

	ethhdr = eth_hdr(skb);

	dst.sa_len = sizeof ethhdr->h_dest;
	memcpy(dst.sa_data, ethhdr->h_dest, dst.sa_len);
	dst.sa_family = AF_INET;

	/* Create route struct. */
	/*
	 * XXX I don't know how it works atm, so just pass through original.
	 * I think it's quite simple; look at how bpfwrite does it.
	 * Not sure this is necessary at all, try just passing in NULL for ro.
	 */

	ro.ro_plen = 0;
	ro.ro_prepend = (void *)0xdeadc0de;
	ro.ro_flags = RT_HAS_HEADER;

	/* XXX Printing for testing.

	ssize_t const l = skb->tail - skb->data;
	printf("%s: skbuff of size %zd\n", __func__, l);
	for (ssize_t i = 0; i < l; i++)
		printf("%x ", ((uint8_t*) skb->data)[i]);
	printf("\n");
	*/

	/* Actually call output function. */

	return (ifp->if_output(ifp, m, &dst, &ro));
}

static inline int
net_xmit_eval(int e)
{

	if (e == NET_XMIT_CN)
		return (0);
	return (e);
}

static inline int
dev_get_iflink(struct net_device const *dev)
{

	pr_debug("%s: TODO\n", __func__);
	return (-1);
}

static inline struct net *
dev_net(struct net_device const *dev)
{

	pr_debug("%s: TODO\n", __func__);
	return (NULL);
}

static inline struct net_device *
__dev_get_by_index(struct net *net, int ifindex)
{

	pr_debug("%s: TODO\n", __func__);
	return (NULL);
}

static inline void
dev_hold(struct net_device *dev)
{

	pr_debug("%s: TODO\n", __func__);
}

static inline int
netdev_master_upper_dev_link(struct net_device *dev, struct net_device *upper_dev, void *upper_priv, void *upper_info, struct netlink_ext_ack *extack)
{

	pr_debug("%s: TODO\n", __func__);
	return (0);
}

static inline void
netdev_upper_dev_unlink(struct net_device *dev, struct net_device *upper_dev)
{

	pr_debug("%s: TODO\n", __func__);
}

static inline struct net_device *
netdev_master_upper_dev_get_rcu(struct net_device *dev)
{

	pr_debug("%s: TODO\n", __func__);
	return (NULL);
}

static inline void
dev_add_pack(struct packet_type *pt)
{

	pr_debug("%s: TODO\n", __func__);
}

static inline void
dev_remove_pack(struct packet_type *pt)
{

	pr_debug("%s: TODO\n", __func__);
}

static inline void
netif_start_queue(struct net_device *dev)
{

	pr_debug("%s: TODO\n", __func__);
}

static inline void
netif_trans_update(struct net_device *dev)
{

	pr_debug("%s: TODO\n", __func__);
}

struct netdev_queue {
};

static inline void
netdev_for_each_tx_queue(struct net_device *dev, void (*fn)(struct net_device *, struct netdev_queue *, void *), void *arg)
{

	pr_debug("%s: TODO -- tx_queues\n", __func__);
}

#define	IFF_NO_QUEUE	IFF_DRV_OACTIVE

#endif	/* _LINUXKPI_LINUX_NETDEVICE_H */
