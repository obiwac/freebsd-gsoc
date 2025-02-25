/*-
 * SPDX-License-Identifier: BSD-2-Clause
 * Copyright (c) 2023 Aymeric Wibo <obiwac@freebsd.org>
 */

#ifndef	_LINUXKPI_LINUX_PERCPU_H
#define	_LINUXKPI_LINUX_PERCPU_H

#include <linux/percpu-defs.h>

static inline void
free_percpu(void __percpu *__pdata)
{

	pr_debug("TODO: %s\n", __func__);
}

static inline void __percpu *
__alloc_percpu(size_t size, size_t align)
{

	pr_debug("TODO: %s\n", __func__);
	return (NULL);
}

#endif	/* _LINUXKPI_LINUX_PERCPU_H */
