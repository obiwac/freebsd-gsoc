/*-
 * SPDX-License-Identifier: BSD-2-Clause
 * Copyright (c) 2023 Aymeric Wibo <obiwac@freebsd.org>
 */

#include <err.h>
#include <string.h>
#include <sys/param.h>

#include "ifconfig.h"
#include "ifconfig_netlink.h"

/*
 * Since batman_adv anyway depends on netlink, there's no point supporting the
 * ioctl way.
 */

struct batadvreq {
	char const*	routing_algo;
};

static struct batadvreq params = {
	.routing_algo	= NULL,
};

/*
 * XXX Taken from sys/compat/linuxkpi/common/include/uapi/linux/batman_adv.h.
 * How do we make this includable by userspace programs?
 */
enum batadv_ifla_attrs {
	IFLA_BATADV_UNSPEC,
	IFLA_BATADV_ALGO_NAME,
	__IFLA_BATADV_MAX,
};

static void
batadv_create(int s, if_ctx *ctx, struct ifreq *req)
{
	struct snl_writer nw;
	struct nlmsghdr *hdr;
	uint32_t seq;
	struct snl_errmsg_data e;

	/* Create the interface (link). */
	snl_init_writer(ctx->io_ss, &nw);
	hdr = snl_create_msg_request(&nw, RTM_NEWLINK);
	snl_reserve_msg_object(&nw, struct ifinfomsg);
	hdr->nlmsg_flags |= NLM_F_EXCL | NLM_F_CREATE | NLM_F_ACK |
	    NLM_F_REQUEST;

	snl_add_msg_attr_string(&nw, IFLA_IFNAME, req->ifr_name);
	{
		int const off = snl_add_msg_attr_nested(&nw, IFLA_LINKINFO);
		snl_add_msg_attr_string(&nw, IFLA_INFO_KIND, req->ifr_name);
		if (params.routing_algo != NULL) {
			int const off = snl_add_msg_attr_nested(&nw,
			    IFLA_INFO_DATA);
			snl_add_msg_attr_string(&nw, IFLA_BATADV_ALGO_NAME,
			    params.routing_algo);
			snl_end_attr_nested(&nw, off);
		}
		snl_end_attr_nested(&nw, off);
	}

	hdr = snl_finalize_msg(&nw);
	if (hdr == NULL)
		errx(EXIT_FAILURE, "%s: snl_finalize_msg", req->ifr_name);
	seq = hdr->nlmsg_seq;
	if (snl_send_message(ctx->io_ss, hdr) == 0)
		errx(EXIT_FAILURE, "%s: snl_send_message", req->ifr_name);

	/* Read created interface name in reply. */
	hdr = snl_read_reply(ctx->io_ss, seq);
	if (hdr->nlmsg_type == NLMSG_ERROR) {
		if (snl_parse_errmsg(ctx->io_ss, hdr, &e) == 0)
			errx(EXIT_FAILURE, "%s: snl_parse_errmsg", req->ifr_name);
		if (e.error != 0)
			errx(EXIT_FAILURE, "%s: %d", req->ifr_name, e.error);
	}

	/* TODO It seems like RTM_NEWLINK never replies?
	struct snl_parsed_link_simple link;
	if (hdr->nlmsg_type != RTM_NEWLINK)
		errx(EXIT_FAILURE, "snl_read_reply: %s", req->ifr_name);
	if (snl_parse_nlmsg(ctx->io_ss, hdr, &snl_rtm_link_parser_simple, &link) == 0)
		errx(EXIT_FAILURE, "%s: snl_parse_nlmsg", req->ifr_name);
	printf("%s: NEWWWWWWWWWWWW %s\n", __func__, link.ifla_ifname);
	*/
}

static void
setra(if_ctx *ctx, char const *val, int dummy __unused)
{

	params.routing_algo = val;
}

static struct cmd batadv_cmds[] = {
	DEF_CLONE_CMD_ARG("ra", 	setra),
};

static __constructor void
batadv_ctor(void)
{
	size_t i;

	for (i = 0; i < nitems(batadv_cmds); i++)
		cmd_register(&batadv_cmds[i]);
	clone_nl_setdefcallback_prefix("batadv", batadv_create);
}
