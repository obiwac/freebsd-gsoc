/*-
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2021 Alstom Group.
 * Copyright (c) 2021 Semihalf.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#ifndef _FELIX_REG_H_
#define _FELIX_REG_H_

#define	BIT(x)						(1UL << (x))

#define FELIX_MDIO_BASE					0x1C00

#define	FELIX_DEVCPU_GCB_RST				0x70004
#define	FELIX_DEVCPU_GCB_RST_EN				BIT(0)

#define	FELIX_ANA_VT					0x287F34
#define	FELIX_ANA_VT_PORTMASK_SHIFT			2
#define	FELIX_ANA_VT_PORTMASK_MASK			0x7F
#define	FELIX_ANA_VT_STS				(BIT(0) | BIT(1))
#define	FELIX_ANA_VT_RESET				(BIT(0) | BIT(1))
#define	FELIX_ANA_VT_WRITE				BIT(1)
#define	FELIX_ANA_VT_READ				BIT(0)
#define	FELIX_ANA_VT_IDLE				0
#define	FELIX_ANA_VTIDX					0x287F38

#define	FELIX_ANA_PORT_BASE				0x287800
#define	FELIX_ANA_PORT_OFFSET				0x100
#define	FELIX_ANA_PORT_VLAN_CFG				0x0
#define	FELIX_ANA_PORT_VLAN_CFG_VID_MASK		0xFFF
#define	FELIX_ANA_PORT_VLAN_CFG_POP			BIT(18)
#define	FELIX_ANA_PORT_VLAN_CFG_VID_AWARE		BIT(20)
#define	FELIX_ANA_PORT_DROP_CFG				0x4
#define	FELIX_ANA_PORT_DROP_CFG_MULTI			BIT(0)
#define	FELIX_ANA_PORT_DROP_CFG_NULL			BIT(1)	/* SRC, or DST MAC == 0 */
#define	FELIX_ANA_PORT_DROP_CFG_CTAGGED_PRIO		BIT(2)	/* 0x8100, VID == 0 */
#define	FELIX_ANA_PORT_DROP_CFG_STAGGED_PRIO		BIT(3)	/* 0x88A8, VID == 0 */
#define	FELIX_ANA_PORT_DROP_CFG_CTAGGED			BIT(4)	/* 0x8100 */
#define	FELIX_ANA_PORT_DROP_CFG_STAGGED			BIT(5)	/* 0x88A8 */
#define	FELIX_ANA_PORT_DROP_CFG_UNTAGGED		BIT(6)
#define FELIX_ANA_PORT_DROP_CFG_TAGGED	\
	(FELIX_ANA_PORT_DROP_CFG_CTAGGED_PRIO | \
	 FELIX_ANA_PORT_DROP_CFG_STAGGED_PRIO | \
	 FELIX_ANA_PORT_DROP_CFG_CTAGGED | \
	 FELIX_ANA_PORT_DROP_CFG_STAGGED)

#define	FELIX_DEVGMII_BASE				0x100000
#define	FELIX_DEVGMII_PORT_OFFSET			0x010000

#define	FELIX_DEVGMII_CLK_CFG				0x0
#define	FELIX_DEVGMII_CLK_CFG_SPEED_1000		1
#define	FELIX_DEVGMII_CLK_CFG_SPEED_100			2
#define	FELIX_DEVGMII_CLK_CFG_SPEED_10			3

#define	FELIX_DEVGMII_MAC_CFG				0x1c
#define	FELIX_DEVGMII_MAC_CFG_TX_ENA			BIT(0)
#define	FELIX_DEVGMII_MAC_CFG_RX_ENA			BIT(4)

#define FELIX_DEVGMII_VLAN_CFG				0x28
#define FELIX_DEVGMII_VLAN_CFG_ENA			BIT(0)	/* Accept 0x8100 only. */
#define FELIX_DEVGMII_VLAN_CFG_DOUBLE_ENA		BIT(1)	/* Inner tag can only be 0x8100. */
#define FELIX_DEVGMII_VLAN_CFG_LEN_ENA			BIT(2)	/* Enable VLANMTU. */

#define FELIX_REW_PORT_BASE				0x030000
#define FELIX_REW_PORT_OFFSET				0x80
#define FELIX_REW_PORT_TAG_CFG				0x4
#define FELIX_REW_PORT_TAG_CFG_MASK			(BIT(7) | BIT(8))
#define FELIX_REW_PORT_TAG_CFG_DIS			(0 << 7) /* Port tagging disabled */
#define FELIX_REW_PORT_TAG_CFG_ALL			(2 << 7) /* Tag frames if pvid != 0 */

#define	FELIX_SYS_RAM_CTRL				0x10F24
#define	FELIX_SYS_RAM_CTRL_INIT				BIT(1)

#define	FELIX_SYS_CFG					0x10E00
#define	FELIX_SYS_CFG_CORE_EN				BIT(0)

#define	FELIX_QSYS_PORT_MODE(port)			(0x20F480 + 4*(port))
#define	FELIX_QSYS_PORT_MODE_PORT_ENA			BIT(14)

#endif
