/*-
 * Copyright (c) 2010 Isilon Systems, Inc.
 * Copyright (c) 2010 iX Systems, Inc.
 * Copyright (c) 2010 Panasas, Inc.
 * Copyright (c) 2013-2016 Mellanox Technologies, Ltd.
 * All rights reserved.
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

#ifndef _LINUXKPI_LINUX_RANDOM_H_
#define	_LINUXKPI_LINUX_RANDOM_H_

#include <linux/kernel.h>
#include <linux/types.h>
#include <sys/random.h>
#include <sys/libkern.h>

#define	get_random_u32() get_random_int()

static inline void
get_random_bytes(void *buf, int nbytes)
{

	arc4random_buf(buf, nbytes);
}

static inline u_int
get_random_int(void)
{
	u_int val;

	get_random_bytes(&val, sizeof(val));
	return (val);
}

static inline u_long
get_random_long(void)
{
	u_long val;

	get_random_bytes(&val, sizeof(val));
	return (val);
}

static __inline uint32_t
prandom_u32(void)
{
	uint32_t val;

	get_random_bytes(&val, sizeof(val));
	return (val);
}

static inline u32
prandom_u32_max(u32 max)
{
	return (arc4random_uniform(max));
}

static inline u32
get_random_u32_below(u32 ceil)
{

	return (arc4random_uniform(ceil));
}

#endif /* _LINUXKPI_LINUX_RANDOM_H_ */
