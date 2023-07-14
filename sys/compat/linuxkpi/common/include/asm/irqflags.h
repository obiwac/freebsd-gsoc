/*-
 * SPDX-License-Identifier: BSD-2-Clause
 * Copyright (c) 2023 Aymeric Wibo <obiwac@freebsd.org>
 */

#ifndef	_LINUXKPI_ASM_GENERIC_IRQFLAGS_H
#define	_LINUXKPI_ASM_GENERIC_IRQFLAGS_H

static inline unsigned long
arch_local_irq_disable(void)
{

	pr_debug("%s: TODO\n", __func__);
	return 0;
}

static inline unsigned long
arch_local_irq_save(void)
{

	pr_debug("%s: TODO\n", __func__);
	return 0;
}

static inline void
arch_local_irq_restore(unsigned long flags)
{

	pr_debug("%s(0x%lx): TODO\n", __func__, flags);
}

#endif	/* _LINUXKPI_ASM_GENERIC_IRQFLAGS_H */
