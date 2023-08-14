/*
 * Copyright (c) 2013-2017 Mellanox Technologies, Ltd.
 * Copyright (c) 2023 Aymeric Wibo <obiwac@freebsd.org>
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

#ifndef _LINUXKPI_LINUX_BITMAP_H_
#define	_LINUXKPI_LINUX_BITMAP_H_

#include <linux/pr_debug.h>
#include <linux/bitops.h>
#include <linux/find.h>
#include <linux/slab.h>

static inline void
bitmap_zero(unsigned long *addr, const unsigned int size)
{
	memset(addr, 0, BITS_TO_LONGS(size) * sizeof(long));
}

static inline void
bitmap_fill(unsigned long *addr, const unsigned int size)
{
	const unsigned int tail = size & (BITS_PER_LONG - 1);

	memset(addr, 0xff, BIT_WORD(size) * sizeof(long));

	if (tail)
		addr[BIT_WORD(size)] = BITMAP_LAST_WORD_MASK(tail);
}

static inline int
bitmap_full(const unsigned long *addr, const unsigned int size)
{
	const unsigned int end = BIT_WORD(size);
	const unsigned int tail = size & (BITS_PER_LONG - 1);
	unsigned int i;

	for (i = 0; i != end; i++) {
		if (addr[i] != ~0UL)
			return (0);
	}

	if (tail) {
		const unsigned long mask = BITMAP_LAST_WORD_MASK(tail);

		if ((addr[end] & mask) != mask)
			return (0);
	}
	return (1);
}

static inline int
bitmap_empty(const unsigned long *addr, const unsigned int size)
{
	const unsigned int end = BIT_WORD(size);
	const unsigned int tail = size & (BITS_PER_LONG - 1);
	unsigned int i;

	for (i = 0; i != end; i++) {
		if (addr[i] != 0)
			return (0);
	}

	if (tail) {
		const unsigned long mask = BITMAP_LAST_WORD_MASK(tail);

		if ((addr[end] & mask) != 0)
			return (0);
	}
	return (1);
}

static inline void
bitmap_set(unsigned long *map, unsigned int start, int nr)
{
	const unsigned int size = start + nr;
	int bits_to_set = BITS_PER_LONG - (start % BITS_PER_LONG);
	unsigned long mask_to_set = BITMAP_FIRST_WORD_MASK(start);

	map += BIT_WORD(start);

	while (nr - bits_to_set >= 0) {
		*map |= mask_to_set;
		nr -= bits_to_set;
		bits_to_set = BITS_PER_LONG;
		mask_to_set = ~0UL;
		map++;
	}

	if (nr) {
		mask_to_set &= BITMAP_LAST_WORD_MASK(size);
		*map |= mask_to_set;
	}
}

static inline void
bitmap_clear(unsigned long *map, unsigned int start, int nr)
{
	const unsigned int size = start + nr;
	int bits_to_clear = BITS_PER_LONG - (start % BITS_PER_LONG);
	unsigned long mask_to_clear = BITMAP_FIRST_WORD_MASK(start);

	map += BIT_WORD(start);

	while (nr - bits_to_clear >= 0) {
		*map &= ~mask_to_clear;
		nr -= bits_to_clear;
		bits_to_clear = BITS_PER_LONG;
		mask_to_clear = ~0UL;
		map++;
	}

	if (nr) {
		mask_to_clear &= BITMAP_LAST_WORD_MASK(size);
		*map &= ~mask_to_clear;
	}
}

static inline unsigned int
bitmap_find_next_zero_area_off(const unsigned long *map,
    const unsigned int size, unsigned int start,
    unsigned int nr, unsigned int align_mask,
    unsigned int align_offset)
{
	unsigned int index;
	unsigned int end;
	unsigned int i;

retry:
	index = find_next_zero_bit(map, size, start);

	index = (((index + align_offset) + align_mask) & ~align_mask) - align_offset;

	end = index + nr;
	if (end > size)
		return (end);

	i = find_next_bit(map, end, index);
	if (i < end) {
		start = i + 1;
		goto retry;
	}
	return (index);
}

static inline unsigned int
bitmap_find_next_zero_area(const unsigned long *map,
    const unsigned int size, unsigned int start,
    unsigned int nr, unsigned int align_mask)
{
	return (bitmap_find_next_zero_area_off(map, size,
	    start, nr, align_mask, 0));
}

static inline int
bitmap_find_free_region(unsigned long *bitmap, int bits, int order)
{
	int pos;
	int end;

	for (pos = 0; (end = pos + (1 << order)) <= bits; pos = end) {
		if (!linux_reg_op(bitmap, pos, order, REG_OP_ISFREE))
			continue;
		linux_reg_op(bitmap, pos, order, REG_OP_ALLOC);
		return (pos);
	}
	return (-ENOMEM);
}

static inline int
bitmap_allocate_region(unsigned long *bitmap, int pos, int order)
{
	if (!linux_reg_op(bitmap, pos, order, REG_OP_ISFREE))
		return (-EBUSY);
	linux_reg_op(bitmap, pos, order, REG_OP_ALLOC);
	return (0);
}

static inline void
bitmap_release_region(unsigned long *bitmap, int pos, int order)
{
	linux_reg_op(bitmap, pos, order, REG_OP_RELEASE);
}

static inline unsigned int
bitmap_weight(const unsigned long *addr, const unsigned int size)
{
	const unsigned int end = BIT_WORD(size);
	const unsigned int tail = size & (BITS_PER_LONG - 1);
	unsigned int retval = 0;
	unsigned int i;

	for (i = 0; i != end; i++)
		retval += hweight_long(addr[i]);

	if (tail) {
		const unsigned long mask = BITMAP_LAST_WORD_MASK(tail);

		retval += hweight_long(addr[end] & mask);
	}
	return (retval);
}

static inline unsigned int
bitmap_weight_and(const unsigned long *addr1,
				const unsigned long *addr2, unsigned int size)
{
	const unsigned int end = BIT_WORD(size);
	const unsigned int tail = size & (BITS_PER_LONG - 1);
	unsigned int retval = 0;
	unsigned int i;

	for (i = 0; i != end; i++)
		retval += hweight_long(addr1[i] & addr2[i]);

	if (tail) {
		const unsigned long mask = BITMAP_LAST_WORD_MASK(tail);

		retval += hweight_long((addr1[end] & addr2[end]) & mask);
	}
	return (retval);
}

static inline int
bitmap_equal(const unsigned long *pa,
    const unsigned long *pb, unsigned size)
{
	const unsigned int end = BIT_WORD(size);
	const unsigned int tail = size & (BITS_PER_LONG - 1);
	unsigned int i;

	for (i = 0; i != end; i++) {
		if (pa[i] != pb[i])
			return (0);
	}

	if (tail) {
		const unsigned long mask = BITMAP_LAST_WORD_MASK(tail);

		if ((pa[end] ^ pb[end]) & mask)
			return (0);
	}
	return (1);
}

static inline int
bitmap_subset(const unsigned long *pa,
    const unsigned long *pb, unsigned size)
{
	const unsigned end = BIT_WORD(size);
	const unsigned tail = size & (BITS_PER_LONG - 1);
	unsigned i;

	for (i = 0; i != end; i++) {
		if (pa[i] & ~pb[i])
			return (0);
	}

	if (tail) {
		const unsigned long mask = BITMAP_LAST_WORD_MASK(tail);

		if (pa[end] & ~pb[end] & mask)
			return (0);
	}
	return (1);
}

static inline void
bitmap_complement(unsigned long *dst, const unsigned long *src,
    const unsigned int size)
{
	const unsigned int end = BITS_TO_LONGS(size);
	unsigned int i;

	for (i = 0; i != end; i++)
		dst[i] = ~src[i];
}

static inline void
bitmap_copy(unsigned long *dst, const unsigned long *src,
    const unsigned int size)
{
	const unsigned int end = BITS_TO_LONGS(size);
	unsigned int i;

	for (i = 0; i != end; i++)
		dst[i] = src[i];
}

static inline void
bitmap_to_arr32(uint32_t *dst, const unsigned long *src, unsigned int size)
{
	const unsigned int end = howmany(size, 32);

#ifdef __LP64__
	unsigned int i = 0;
	while (i < end) {
		dst[i++] = (uint32_t)(*src & UINT_MAX);
		if (i < end)
			dst[i++] = (uint32_t)(*src >> 32);
		src++;
	}
#else
	bitmap_copy((unsigned long *)dst, src, size);
#endif
	if ((size % 32) != 0) /* Linux uses BITS_PER_LONG. Seems to be a bug */
		dst[end - 1] &= (uint32_t)(UINT_MAX >> (32 - (size % 32)));
}

static inline void
bitmap_or(unsigned long *dst, const unsigned long *src1,
    const unsigned long *src2, const unsigned int size)
{
	const unsigned int end = BITS_TO_LONGS(size);
	unsigned int i;

	for (i = 0; i != end; i++)
		dst[i] = src1[i] | src2[i];
}

static inline bool
bitmap_and(unsigned long *dst, const unsigned long *src1,
    const unsigned long *src2, const unsigned int size)
{
	unsigned int k;
	unsigned int lim = size/BITS_PER_LONG;
	unsigned long result = 0;

	for (k = 0; k < lim; k++)
		result |= (dst[k] = src1[k] & src2[k]);
	if (size % BITS_PER_LONG)
		result |= (dst[k] = src1[k] & src2[k] &
			   BITMAP_LAST_WORD_MASK(size));
	return result != 0;
}

static inline bool
bitmap_andnot(unsigned long *dst, const unsigned long *src1,
    const unsigned long *src2, const unsigned int size)
{
	unsigned int k;
	unsigned int lim = size/BITS_PER_LONG;
	unsigned long result = 0;

	for (k = 0; k < lim; k++)
		result |= (dst[k] = src1[k] & ~src2[k]);
	if (size % BITS_PER_LONG)
		result |= (dst[k] = src1[k] & ~src2[k] &
			   BITMAP_LAST_WORD_MASK(size));
	return result != 0;
}

static inline void
bitmap_xor(unsigned long *dst, const unsigned long *src1,
    const unsigned long *src2, const unsigned int size)
{
	const unsigned int end = BITS_TO_LONGS(size);
	unsigned int i;

	for (i = 0; i != end; i++)
		dst[i] = src1[i] ^ src2[i];
}

static inline unsigned long *
bitmap_alloc(unsigned int size, gfp_t flags)
{
	return (kmalloc_array(BITS_TO_LONGS(size),
	    sizeof(unsigned long), flags));
}

static inline unsigned long *
bitmap_zalloc(unsigned int size, gfp_t flags)
{
	return (bitmap_alloc(size, flags | __GFP_ZERO));
}

static inline void
bitmap_free(const unsigned long *bitmap)
{
	kfree(bitmap);
}

static inline void
bitmap_shift_left(unsigned long *dst, unsigned long const *src,
	unsigned int shift, unsigned int nbits)
{
	/* XXX-GPL This comes from lib/bitmap.c, I did not write this. */

	int k;
	unsigned int lim = BITS_TO_LONGS(nbits);
	unsigned int off = shift/BITS_PER_LONG, rem = shift % BITS_PER_LONG;

	for (k = lim - off - 1; k >= 0; --k) {
		unsigned long upper, lower;

		/*
		 * If shift is not word aligned, take upper rem bits of
		 * word below and make them the bottom rem bits of result.
		 */
		if (rem && k > 0)
			lower = src[k - 1] >> (BITS_PER_LONG - rem);
		else
			lower = 0;
		upper = src[k] << rem;
		dst[k + off] = lower | upper;
	}
	if (off)
		memset(dst, 0, off*sizeof(unsigned long));
}

#endif					/* _LINUXKPI_LINUX_BITMAP_H_ */
