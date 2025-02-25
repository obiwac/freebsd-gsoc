/*-
 * SPDX-License-Identifier: BSD-2-Clause
 * Copyright (c) 2023 Aymeric Wibo <obiwac@freebsd.org>
 */

#ifndef	_LINUXKPI_LINUX_CRC32C_H
#define	_LINUXKPI_LINUX_CRC32C_H

#include <sys/gsb_crc32.h>

static inline uint32_t
crc32c(uint32_t crc, void const *data, size_t len)
{

	return (crc32_raw(data, len, crc));
}

#endif	/* _LINUXKPI_LINUX_CRC32C_H */
