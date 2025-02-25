# SPDX-License-Identifier: BSD-2-Clause
# Copyright (c) 2023 Aymeric Wibo <obiwac@freebsd.org>

DEVBATMANADVDIR=	${SRCTOP}/sys/contrib/dev/batman-adv

.PATH: ${DEVBATMANADVDIR}

WITH_DEBUGFS=	1

KMOD=	batman_adv

# batman-adv sources/flags.

SRCS=	bat_algo.c \
	bat_iv_ogm.c \
	bat_v.c \
	bat_v_elp.c \
	bat_v_ogm.c \
	bitarray.c \
	fragmentation.c \
	gateway_client.c \
	gateway_common.c \
	hard-interface.c \
	hash.c \
	log.c \
	main.c \
	netlink.c \
	originator.c \
	routing.c \
	send.c \
	soft-interface.c \
	tp_meter.c \
	trace.c \
	translation-table.c \
	tvlv.c

CFLAGS+=	-DKBUILD_MODNAME='"batman-adv"'
CFLAGS+=	-I${DEVBATMANADVDIR}

CFLAGS+=	-DCONFIG_BATMAN_ADV_BATMAN_V
CFLAGS+=	-DCONFIG_BATMAN_ADV_DEBUG
# CFLAGS+=	-DCONFIG_BATMAN_ADV_BLA # bridge loop avoidance
# CFLAGS+=	-DCONFIG_BATMAN_ADV_DAT # distributed ARP table
# CFLAGS+=	-DCONFIG_BATMAN_ADV_MCAST # multicast
# CFLAGS+=	-DCONFIG_BATMAN_ADV_NC # XXX network coding (need to add network-coding.c to $SRCS)

# LinuxKPI sources/flags.

SRCS+=		${LINUXKPI_GENSRCS}
CFLAGS+=	${LINUXKPI_INCLUDES}
CFLAGS+=	-O0

# DebugFS source files.

.if defined(WITH_DEBUGFS) && ${WITH_DEBUGFS} > 0
# XXX no debugfs?
.endif

.include <bsd.kmod.mk>
