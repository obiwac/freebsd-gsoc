/*-
 * SPDX-License-Identifier: BSD-2-Clause
 * Copyright (c) ???
 * Copyright (c) 2023 Aymeric Wibo <obiwac@freebsd.org>
 */

#ifndef	_LINUXKPI_LINUX_PR_DEBUG_H
#define	_LINUXKPI_LINUX_PR_DEBUG_H

#include <sys/syslog.h>

// TODO who wrote this?

/*
 * The "pr_debug()" and "pr_devel()" macros should produce zero code
 * unless DEBUG is defined:
 */

// #ifdef DEBUG
extern int linuxkpi_debug;
#define pr_debug(fmt, ...)					\
	do {							\
		/*if (linuxkpi_debug)*/				\
			printf(fmt, ##__VA_ARGS__);		\
	} while (0)
#define pr_devel(fmt, ...) \
	log(LOG_DEBUG, pr_fmt(fmt), ##__VA_ARGS__)
// #else
// #define pr_debug(fmt, ...) \
// 	({ if (0) log(LOG_DEBUG, fmt, ##__VA_ARGS__); 0; })
// #define pr_devel(fmt, ...) \
// 	({ if (0) log(LOG_DEBUG, pr_fmt(fmt), ##__VA_ARGS__); 0; })
// #endif

#endif	/* _LINUXKPI_LINUX_PR_DEBUG_H */
