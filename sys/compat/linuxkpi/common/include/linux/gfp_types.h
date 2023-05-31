/*-
* SPDX-License-Identifier: BSD-2-Clause
* Copyright (c) 2022 Aymeric Wibo <obiwac@freebsd.org>
*/
#ifndef _LINUXKPI_LINUX_GFP_TYPES_H_
#define _LINUXKPI_LINUX_GFP_TYPES_H_

#include <sys/param.h>

#define	__GFP_NOWARN	0
#define	__GFP_HIGHMEM	0
#define	__GFP_ZERO	M_ZERO
#define	__GFP_NORETRY	0
#define	__GFP_NOMEMALLOC 0
#define	__GFP_RECLAIM   0
#define	__GFP_RECLAIMABLE   0
#define	__GFP_RETRY_MAYFAIL 0
#define	__GFP_MOVABLE	0
#define	__GFP_COMP	0
#define	__GFP_KSWAPD_RECLAIM 0

#define	__GFP_IO	0
#define	__GFP_NO_KSWAPD	0
#define	__GFP_KSWAPD_RECLAIM	0
#define	__GFP_WAIT	M_WAITOK
#define	__GFP_DMA32	(1U << 24) /* LinuxKPI only */
#define	__GFP_BITS_SHIFT 25
#define	__GFP_BITS_MASK	((1 << __GFP_BITS_SHIFT) - 1)
#define	__GFP_NOFAIL	M_WAITOK

#define	GFP_NOWAIT	M_NOWAIT
#define	GFP_ATOMIC	(M_NOWAIT | M_USE_RESERVE)
#define	GFP_KERNEL	M_WAITOK
#define	GFP_USER	M_WAITOK
#define	GFP_HIGHUSER	M_WAITOK
#define	GFP_HIGHUSER_MOVABLE	M_WAITOK
#define	GFP_IOFS	M_NOWAIT
#define	GFP_NOIO	M_NOWAIT
#define	GFP_NOFS	M_NOWAIT
#define	GFP_DMA32	__GFP_DMA32
#define	GFP_TEMPORARY	M_NOWAIT
#define	GFP_NATIVE_MASK	(M_NOWAIT | M_WAITOK | M_USE_RESERVE | M_ZERO)
#define	GFP_TRANSHUGE	0
#define	GFP_TRANSHUGE_LIGHT	0

CTASSERT((__GFP_DMA32 & GFP_NATIVE_MASK) == 0);
CTASSERT((__GFP_BITS_MASK & GFP_NATIVE_MASK) == GFP_NATIVE_MASK);


#endif /* __LINUX_GFP_TYPES_H */
