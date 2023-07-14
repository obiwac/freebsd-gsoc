/*-
 * SPDX-License-Identifier: BSD-2-Clause
 * Copyright (c) 2023 Aymeric Wibo <obiwac@freebsd.org>
 */

#ifndef	_LINUXKPI_LINUX_CRC32C_H
#define	_LINUXKPI_LINUX_CRC32C_H

#include <sys/gsb_crc32.h>

static inline u32
crc32c(u32 crc, void const *address, unsigned int length)
{

	pr_debug("%s: TODO\n", __func__);
	return (-1);
}

#endif	/* _LINUXKPI_LINUX_CRC32C_H */
