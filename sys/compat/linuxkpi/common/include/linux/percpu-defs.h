/*-
 * SPDX-License-Identifier: BSD-2-Clause
 * Copyright (c) 2023 Aymeric Wibo <obiwac@freebsd.org>
 */

#ifndef	_LINUXKPI_LINUX_PERCPU_DEFS_H
#define	_LINUXKPI_LINUX_PERCPU_DEFS_H

#define	this_cpu_add(pcp, val)	({		\
	pr_debug("this_cpu_add: TODO\n");	\
	0;					\
})

#define	per_cpu_ptr(ptr, cpu)	({		\
	pr_debug("per_cpu_ptr: TODO\n");	\
	NULL;					\
})

#endif	/* _LINUXKPI_LINUX_PERCPU_DEFS_H */
