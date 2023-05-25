# SPDX-License-Identifier: BSD-2-Clause
# Copyright (c) 2022 Aymeric Wibo <obiwac@freebsd.org>

DEVBATMANADVDIR=	${SRCTOP}/sys/contrib/dev/batman-adv

.PATH: ${DEVBATMANADVDIR}

WITH_DEBUGFS=	1

KMOD=	batman_adv

# batman-adv sources/flags.

SRCS=	bat_v.c

CFLAGS+=	-DKBUILD_MODNAME='"batman-adv"'
CFLAGS+=	-I${DEVBATMANADVDIR}

# Linux KPI sources/flags.

SRCS+=	${LINUXKPI_GENSRCS}
CFLAGS+=	${LINUXKPI_INCLUDES}

# DebugFS source files.

.if defined(WITH_DEBUGFS) && ${WITH_DEBUGFS} > 0
# XXX no debugfs?
.endif

.include <bsd.kmod.mk>
