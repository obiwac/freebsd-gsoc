/*-
 * SPDX-License-Identifier: BSD-2-Clause
 * Copyright (c) 2010 Isilon Systems, Inc.
 * Copyright (c) 2010 iX Systems, Inc.
 * Copyright (c) 2010 Panasas, Inc.
 * Copyright (c) 2013-2016 Mellanox Technologies, Ltd.
 * Copyright (c) 2014-2015 François Tigeot
 * Copyright (c) 2023 Aymeric Wibo <obiwac@freebsd.org>
 */

#ifndef	_LINUXKPI_LINUX_PR_DEBUG_H
#define	_LINUXKPI_LINUX_PR_DEBUG_H

#include <sys/syslog.h>

/*
 * The "pr_debug()" and "pr_devel()" macros should produce zero code
 * unless DEBUG is defined:
 */

#ifdef DEBUG
extern int linuxkpi_debug;
#define pr_debug(fmt, ...)					\
	do {							\
		if (linuxkpi_debug)				\
			printf(fmt, ##__VA_ARGS__);		\
	} while (0)
#define pr_devel(fmt, ...) \
	log(LOG_DEBUG, pr_fmt(fmt), ##__VA_ARGS__)
#else
#define pr_debug(fmt, ...) \
	({ if (0) log(LOG_DEBUG, fmt, ##__VA_ARGS__); 0; })
#define pr_devel(fmt, ...) \
	({ if (0) log(LOG_DEBUG, pr_fmt(fmt), ##__VA_ARGS__); 0; })
#endif

#endif	/* _LINUXKPI_LINUX_PR_DEBUG_H */
