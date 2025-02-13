/*
  PIM for Quagga
  Copyright (C) 2008  Everton da Silva Marques

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful, but
  WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program; see the file COPYING; if not, write to the
  Free Software Foundation, Inc., 51 Franklin St, Fifth Floor, Boston,
  MA 02110-1301 USA
*/

#include <zebra.h>
#include "zebra/rib.h"

#include "log.h"
#include "prefix.h"
#include "zclient.h"
#include "stream.h"
#include "network.h"
#include "thread.h"
#include "prefix.h"
#include "vty.h"

#include "pimd.h"
#include "pim_iface.h"
#include "pim_pim.h"
#include "pim_str.h"
#include "pim_oil.h"
#include "pim_zlookup.h"

static struct zclient *zlookup = NULL;

static void zclient_lookup_sched(struct zclient *zlookup, int delay);

/* Connect to zebra for nexthop lookup. */
static int zclient_lookup_connect(struct thread *t)
{
	struct zclient *zlookup;

	zlookup = THREAD_ARG(t);
	zlookup->t_connect = NULL;

	if (zlookup->sock >= 0) {
		return 0;
	}

	if (zclient_socket_connect(zlookup) < 0) {
		++zlookup->fail;
		zlog_warn("%s: failure connecting zclient socket: failures=%d",
			  __PRETTY_FUNCTION__, zlookup->fail);
	} else {
		zlookup->fail = 0; /* reset counter on connection */
	}

	zassert(!zlookup->t_connect);
	if (zlookup->sock < 0) {
		/* Since last connect failed, retry within 10 secs */
		zclient_lookup_sched(zlookup, 10);
		return -1;
	}

	return 0;
}

/* Schedule connection with delay. */
static void zclient_lookup_sched(struct zclient *zlookup, int delay)
{
	zassert(!zlookup->t_connect);

	THREAD_TIMER_ON(master, zlookup->t_connect, zclient_lookup_connect,
			zlookup, delay);

	zlog_notice("%s: zclient lookup connection scheduled for %d seconds",
		    __PRETTY_FUNCTION__, delay);
}

/* Schedule connection for now. */
static void zclient_lookup_sched_now(struct zclient *zlookup)
{
	zassert(!zlookup->t_connect);

	zlookup->t_connect =
		thread_add_event(master, zclient_lookup_connect, zlookup, 0);

	zlog_notice("%s: zclient lookup immediate connection scheduled",
		    __PRETTY_FUNCTION__);
}

/* Schedule reconnection, if needed. */
static void zclient_lookup_reconnect(struct zclient *zlookup)
{
	if (zlookup->t_connect) {
		return;
	}

	zclient_lookup_sched_now(zlookup);
}

static void zclient_lookup_failed(struct zclient *zlookup)
{
	if (zlookup->sock >= 0) {
		if (close(zlookup->sock)) {
			zlog_warn("%s: closing fd=%d: errno=%d %s", __func__,
				  zlookup->sock, errno, safe_strerror(errno));
		}
		zlookup->sock = -1;
	}

	zclient_lookup_reconnect(zlookup);
}

void zclient_lookup_free(void)
{
	zclient_free(zlookup);
	zlookup = NULL;
}

void zclient_lookup_new(void)
{
	zlookup = zclient_new(master);
	if (!zlookup) {
		zlog_err("%s: zclient_new() failure", __PRETTY_FUNCTION__);
		return;
	}

	zlookup->sock = -1;
	zlookup->t_connect = NULL;

	zclient_lookup_sched_now(zlookup);

	zlog_notice("%s: zclient lookup socket initialized",
		    __PRETTY_FUNCTION__);
}

static int zclient_read_nexthop(struct zclient *zlookup,
				struct pim_zlookup_nexthop nexthop_tab[],
				const int tab_size, struct in_addr addr)
{
	int num_ifindex = 0;
	struct stream *s;
	const uint16_t MIN_LEN = 10; /* getipv4=4 getc=1 getl=4 getc=1 */
	uint16_t length;
	u_char marker;
	u_char version;
	vrf_id_t vrf_id;
	uint16_t command = 0;
	struct in_addr raddr;
	uint8_t distance;
	uint32_t metric;
	int nexthop_num;
	int i, err;

	if (PIM_DEBUG_PIM_TRACE_DETAIL) {
		char addr_str[INET_ADDRSTRLEN];
		pim_inet4_dump("<addr?>", addr, addr_str, sizeof(addr_str));
		zlog_debug("%s: addr=%s", __PRETTY_FUNCTION__, addr_str);
	}

	s = zlookup->ibuf;

	while (command != ZEBRA_IPV4_NEXTHOP_LOOKUP_MRIB) {
		stream_reset(s);
		err = zclient_read_header(s, zlookup->sock, &length, &marker,
					  &version, &vrf_id, &command);
		if (err < 0) {
			zlog_err("%s %s: zclient_read_header() failed",
				 __FILE__, __PRETTY_FUNCTION__);
			zclient_lookup_failed(zlookup);
			return -1;
		}

		if (length < MIN_LEN) {
			zlog_err(
				"%s %s: failure reading zclient lookup socket: len=%d < MIN_LEN=%d",
				__FILE__, __PRETTY_FUNCTION__, length, MIN_LEN);
			zclient_lookup_failed(zlookup);
			return -2;
		}
	}

	raddr.s_addr = stream_get_ipv4(s);

	if (raddr.s_addr != addr.s_addr) {
		char addr_str[INET_ADDRSTRLEN];
		char raddr_str[INET_ADDRSTRLEN];
		pim_inet4_dump("<addr?>", addr, addr_str, sizeof(addr_str));
		pim_inet4_dump("<raddr?>", raddr, raddr_str, sizeof(raddr_str));
		zlog_warn("%s: address mismatch: addr=%s raddr=%s",
			  __PRETTY_FUNCTION__, addr_str, raddr_str);
		/* warning only */
	}

	distance = stream_getc(s);
	metric = stream_getl(s);
	nexthop_num = stream_getc(s);

	if (nexthop_num < 1) {
		zlog_err("%s: socket %d bad nexthop_num=%d", __func__,
			 zlookup->sock, nexthop_num);
		return -6;
	}

	for (i = 0; i < nexthop_num; ++i) {
		enum nexthop_types_t nexthop_type;
		struct pim_neighbor *nbr;

		nexthop_type = stream_getc(s);
		if (num_ifindex >= tab_size) {
			char addr_str[INET_ADDRSTRLEN];
			pim_inet4_dump("<addr?>", addr, addr_str,
				       sizeof(addr_str));
			zlog_warn(
				"%s %s: found too many nexthop ifindexes (%d > %d) for address %s",
				__FILE__, __PRETTY_FUNCTION__,
				(num_ifindex + 1), tab_size, addr_str);
			return num_ifindex;
		}
		switch (nexthop_type) {
		case NEXTHOP_TYPE_IFINDEX:
		case NEXTHOP_TYPE_IPV4_IFINDEX:
		case NEXTHOP_TYPE_IPV4:
			nexthop_tab[num_ifindex].nexthop_addr.family = AF_INET;
			if (nexthop_type == NEXTHOP_TYPE_IPV4_IFINDEX
			    || nexthop_type == NEXTHOP_TYPE_IPV4) {
				nexthop_tab[num_ifindex]
					.nexthop_addr.u.prefix4.s_addr =
					stream_get_ipv4(s);
			} else {
				nexthop_tab[num_ifindex]
					.nexthop_addr.u.prefix4.s_addr =
					PIM_NET_INADDR_ANY;
			}
			nexthop_tab[num_ifindex].ifindex = stream_getl(s);
			nexthop_tab[num_ifindex].protocol_distance = distance;
			nexthop_tab[num_ifindex].route_metric = metric;
			++num_ifindex;
			break;
		case NEXTHOP_TYPE_IPV6_IFINDEX:
			nexthop_tab[num_ifindex].nexthop_addr.family = AF_INET6;
			stream_get(&nexthop_tab[num_ifindex]
					    .nexthop_addr.u.prefix6,
				   s, 16);
			nexthop_tab[num_ifindex].ifindex = stream_getl(s);
			nbr = pim_neighbor_find_if(if_lookup_by_index(
				nexthop_tab[num_ifindex].ifindex, VRF_DEFAULT));
			if (nbr) {
				nexthop_tab[num_ifindex].nexthop_addr.family =
					AF_INET;
				nexthop_tab[num_ifindex]
					.nexthop_addr.u.prefix4 =
					nbr->source_addr;
			}
			++num_ifindex;
			break;
		default:
			/* do nothing */
			{
				char addr_str[INET_ADDRSTRLEN];
				pim_inet4_dump("<addr?>", addr, addr_str,
					       sizeof(addr_str));
				zlog_warn(
					"%s %s: found non-ifindex nexthop type=%d for address %s",
					__FILE__, __PRETTY_FUNCTION__,
					nexthop_type, addr_str);
			}
			break;
		}
	}

	return num_ifindex;
}

static int zclient_lookup_nexthop_once(struct pim_zlookup_nexthop nexthop_tab[],
				       const int tab_size, struct in_addr addr)
{
	struct stream *s;
	int ret;

	if (PIM_DEBUG_PIM_TRACE_DETAIL) {
		char addr_str[INET_ADDRSTRLEN];
		pim_inet4_dump("<addr?>", addr, addr_str, sizeof(addr_str));
		zlog_debug("%s: addr=%s", __PRETTY_FUNCTION__, addr_str);
	}

	/* Check socket. */
	if (zlookup->sock < 0) {
		zlog_err("%s %s: zclient lookup socket is not connected",
			 __FILE__, __PRETTY_FUNCTION__);
		zclient_lookup_failed(zlookup);
		return -1;
	}

	s = zlookup->obuf;
	stream_reset(s);
	zclient_create_header(s, ZEBRA_IPV4_NEXTHOP_LOOKUP_MRIB, VRF_DEFAULT);
	stream_put_in_addr(s, &addr);
	stream_putw_at(s, 0, stream_get_endp(s));

	ret = writen(zlookup->sock, s->data, stream_get_endp(s));
	if (ret < 0) {
		zlog_err(
			"%s %s: writen() failure: %d writing to zclient lookup socket",
			__FILE__, __PRETTY_FUNCTION__, errno);
		zclient_lookup_failed(zlookup);
		return -2;
	}
	if (ret == 0) {
		zlog_err("%s %s: connection closed on zclient lookup socket",
			 __FILE__, __PRETTY_FUNCTION__);
		zclient_lookup_failed(zlookup);
		return -3;
	}

	return zclient_read_nexthop(zlookup, nexthop_tab, tab_size, addr);
}

int zclient_lookup_nexthop(struct pim_zlookup_nexthop nexthop_tab[],
			   const int tab_size, struct in_addr addr,
			   int max_lookup)
{
	int lookup;
	uint32_t route_metric = 0xFFFFFFFF;
	uint8_t protocol_distance = 0xFF;

	qpim_nexthop_lookups++;

	for (lookup = 0; lookup < max_lookup; ++lookup) {
		int num_ifindex;
		int first_ifindex;
		struct prefix nexthop_addr;

		num_ifindex = zclient_lookup_nexthop_once(nexthop_tab, tab_size,
							  addr);
		if (num_ifindex < 1) {
			if (PIM_DEBUG_ZEBRA) {
				char addr_str[INET_ADDRSTRLEN];
				pim_inet4_dump("<addr?>", addr, addr_str,
					       sizeof(addr_str));
				zlog_debug(
					"%s %s: lookup=%d/%d: could not find nexthop ifindex for address %s",
					__FILE__, __PRETTY_FUNCTION__, lookup,
					max_lookup, addr_str);
			}
			return -1;
		}

		if (lookup < 1) {
			/* this is the non-recursive lookup - save original
			 * metric/distance */
			route_metric = nexthop_tab[0].route_metric;
			protocol_distance = nexthop_tab[0].protocol_distance;
		}

		/*
		 * FIXME: Non-recursive nexthop ensured only for first ifindex.
		 * However, recursive route lookup should really be fixed in
		 * zebra daemon.
		 * See also TODO T24.
		 *
		 * So Zebra for NEXTHOP_TYPE_IPV4 returns the ifindex now since
		 * it was being stored.  This Doesn't solve all cases of
		 * recursive lookup but for the most common types it does.
		 */
		first_ifindex = nexthop_tab[0].ifindex;
		nexthop_addr = nexthop_tab[0].nexthop_addr;
		if (first_ifindex > 0) {
			/* found: first ifindex is non-recursive nexthop */

			if (lookup > 0) {
				/* Report non-recursive success after first
				 * lookup */
				if (PIM_DEBUG_ZEBRA) {
					char addr_str[INET_ADDRSTRLEN];
					pim_inet4_dump("<addr?>", addr,
						       addr_str,
						       sizeof(addr_str));
					zlog_debug(
						"%s %s: lookup=%d/%d: found non-recursive ifindex=%d for address %s dist=%d met=%d",
						__FILE__, __PRETTY_FUNCTION__,
						lookup, max_lookup,
						first_ifindex, addr_str,
						nexthop_tab[0]
							.protocol_distance,
						nexthop_tab[0].route_metric);
				}

				/* use last address as nexthop address */
				nexthop_tab[0].nexthop_addr.u.prefix4 = addr;

				/* report original route metric/distance */
				nexthop_tab[0].route_metric = route_metric;
				nexthop_tab[0].protocol_distance =
					protocol_distance;
			}

			return num_ifindex;
		}

		if (PIM_DEBUG_ZEBRA) {
			char addr_str[INET_ADDRSTRLEN];
			char nexthop_str[PREFIX_STRLEN];
			pim_inet4_dump("<addr?>", addr, addr_str,
				       sizeof(addr_str));
			pim_addr_dump("<nexthop?>", &nexthop_addr, nexthop_str,
				      sizeof(nexthop_str));
			zlog_debug(
				"%s %s: lookup=%d/%d: zebra returned recursive nexthop %s for address %s dist=%d met=%d",
				__FILE__, __PRETTY_FUNCTION__, lookup,
				max_lookup, nexthop_str, addr_str,
				nexthop_tab[0].protocol_distance,
				nexthop_tab[0].route_metric);
		}

		addr =
			nexthop_addr.u.prefix4; /* use nexthop addr for
						   recursive lookup */

	} /* for (max_lookup) */

	if (PIM_DEBUG_ZEBRA) {
		char addr_str[INET_ADDRSTRLEN];
		pim_inet4_dump("<addr?>", addr, addr_str, sizeof(addr_str));
		zlog_warn(
			"%s %s: lookup=%d/%d: failure searching recursive nexthop ifindex for address %s",
			__FILE__, __PRETTY_FUNCTION__, lookup, max_lookup,
			addr_str);
	}

	return -2;
}

void pim_zlookup_show_ip_multicast(struct vty *vty)
{
	vty_out(vty, "Zclient lookup socket: ");
	if (zlookup) {
		vty_out(vty, "%d failures=%d%s", zlookup->sock, zlookup->fail,
			VTY_NEWLINE);
	} else {
		vty_out(vty, "<null zclient>%s", VTY_NEWLINE);
	}
}

int pim_zlookup_sg_statistics(struct channel_oil *c_oil)
{
	struct stream *s = zlookup->obuf;
	uint16_t command = 0;
	unsigned long long lastused;
	struct prefix_sg sg;
	int count = 0;
	int ret;
	struct interface *ifp =
		pim_if_find_by_vif_index(c_oil->oil.mfcc_parent);

	if (PIM_DEBUG_ZEBRA) {
		struct prefix_sg more;

		more.src = c_oil->oil.mfcc_origin;
		more.grp = c_oil->oil.mfcc_mcastgrp;
		zlog_debug(
			"Sending Request for New Channel Oil Information(%s) VIIF %d",
			pim_str_sg_dump(&more), c_oil->oil.mfcc_parent);
	}

	if (!ifp)
		return -1;

	stream_reset(s);
	zclient_create_header(s, ZEBRA_IPMR_ROUTE_STATS, VRF_DEFAULT);
	stream_put_in_addr(s, &c_oil->oil.mfcc_origin);
	stream_put_in_addr(s, &c_oil->oil.mfcc_mcastgrp);
	stream_putl(s, ifp->ifindex);
	stream_putw_at(s, 0, stream_get_endp(s));

	count = stream_get_endp(s);
	ret = writen(zlookup->sock, s->data, count);
	if (ret <= 0) {
		zlog_err(
			"%s %s: writen() failure: %d writing to zclient lookup socket",
			__FILE__, __PRETTY_FUNCTION__, errno);
		return -1;
	}

	s = zlookup->ibuf;

	while (command != ZEBRA_IPMR_ROUTE_STATS) {
		int err;
		uint16_t length = 0;
		vrf_id_t vrf_id;
		u_char marker;
		u_char version;

		stream_reset(s);
		err = zclient_read_header(s, zlookup->sock, &length, &marker,
					  &version, &vrf_id, &command);
		if (err < 0) {
			zlog_err("%s %s: zclient_read_header() failed",
				 __FILE__, __PRETTY_FUNCTION__);
			zclient_lookup_failed(zlookup);
			return -1;
		}
	}

	sg.src.s_addr = stream_get_ipv4(s);
	sg.grp.s_addr = stream_get_ipv4(s);
	if (sg.src.s_addr != c_oil->oil.mfcc_origin.s_addr
	    || sg.grp.s_addr != c_oil->oil.mfcc_mcastgrp.s_addr) {
		zlog_err("%s: Received wrong %s information",
			 __PRETTY_FUNCTION__, pim_str_sg_dump(&sg));
		zclient_lookup_failed(zlookup);
		return -3;
	}

	stream_get(&lastused, s, sizeof(lastused));
	ret = stream_getl(s);

	if (PIM_DEBUG_ZEBRA)
		zlog_debug("Received %lld for %s success: %d", lastused,
			   pim_str_sg_dump(&sg), ret);

	c_oil->cc.lastused = lastused;

	return 0;
}
