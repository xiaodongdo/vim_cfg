/* BGP Nexthop tracking
 * Copyright (C) 2013 Cumulus Networks, Inc.
 *
 * This file is part of GNU Zebra.
 *
 * GNU Zebra is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation; either version 2, or (at your option) any
 * later version.
 *
 * GNU Zebra is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with GNU Zebra; see the file COPYING.  If not, write to the Free
 * Software Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA
 * 02111-1307, USA.
 */

#include <zebra.h>

#include "command.h"
#include "thread.h"
#include "prefix.h"
#include "zclient.h"
#include "stream.h"
#include "network.h"
#include "log.h"
#include "memory.h"
#include "nexthop.h"
#include "vrf.h"
#include "filter.h"

#include "bgpd/bgpd.h"
#include "bgpd/bgp_table.h"
#include "bgpd/bgp_route.h"
#include "bgpd/bgp_attr.h"
#include "bgpd/bgp_nexthop.h"
#include "bgpd/bgp_debug.h"
#include "bgpd/bgp_nht.h"
#include "bgpd/bgp_fsm.h"
#include "bgpd/bgp_zebra.h"

extern struct zclient *zclient;

static void register_zebra_rnh(struct bgp_nexthop_cache *bnc,
			       int is_bgp_static_route);
static void unregister_zebra_rnh(struct bgp_nexthop_cache *bnc,
				 int is_bgp_static_route);
static void evaluate_paths(struct bgp_nexthop_cache *bnc);
static int make_prefix(int afi, struct bgp_info *ri, struct prefix *p);
static void path_nh_map(struct bgp_info *path, struct bgp_nexthop_cache *bnc,
			int keep);

static int bgp_isvalid_nexthop(struct bgp_nexthop_cache *bnc)
{
	return (bgp_zebra_num_connects() == 0
		|| (bnc && CHECK_FLAG(bnc->flags, BGP_NEXTHOP_VALID)));
}

int bgp_find_nexthop(struct bgp_info *path, int connected)
{
	struct bgp_nexthop_cache *bnc = path->nexthop;

	if (!bnc)
		return 0;

	/*
	 * We are cheating here.  Views have no associated underlying
	 * ability to detect nexthops.  So when we have a view
	 * just tell everyone the nexthop is valid
	 */
	if (path->peer && path->peer->bgp->inst_type == BGP_INSTANCE_TYPE_VIEW)
		return 1;

	if (connected && !(CHECK_FLAG(bnc->flags, BGP_NEXTHOP_CONNECTED)))
		return 0;

	return (bgp_isvalid_nexthop(bnc));
}

static void bgp_unlink_nexthop_check(struct bgp_nexthop_cache *bnc)
{
	if (LIST_EMPTY(&(bnc->paths)) && !bnc->nht_info) {
		if (BGP_DEBUG(nht, NHT)) {
			char buf[PREFIX2STR_BUFFER];
			zlog_debug("bgp_unlink_nexthop: freeing bnc %s",
				   bnc_str(bnc, buf, PREFIX2STR_BUFFER));
		}
		unregister_zebra_rnh(bnc,
				     CHECK_FLAG(bnc->flags, BGP_STATIC_ROUTE));
		bnc->node->info = NULL;
		bgp_unlock_node(bnc->node);
		bnc->node = NULL;
		bnc_free(bnc);
	}
}

void bgp_unlink_nexthop(struct bgp_info *path)
{
	struct bgp_nexthop_cache *bnc = path->nexthop;

	if (!bnc)
		return;

	path_nh_map(path, NULL, 0);

	bgp_unlink_nexthop_check(bnc);
}

void bgp_unlink_nexthop_by_peer(struct peer *peer)
{
	struct prefix p;
	struct bgp_node *rn;
	struct bgp_nexthop_cache *bnc;
	afi_t afi = family2afi(peer->su.sa.sa_family);

	if (!sockunion2hostprefix(&peer->su, &p))
		return;

	rn = bgp_node_get(peer->bgp->nexthop_cache_table[afi], &p);

	if (!rn->info)
		return;

	bnc = rn->info;

	/* cleanup the peer reference */
	bnc->nht_info = NULL;

	bgp_unlink_nexthop_check(bnc);
}

int bgp_find_or_add_nexthop(struct bgp *bgp, afi_t afi, struct bgp_info *ri,
			    struct peer *peer, int connected)
{
	struct bgp_node *rn;
	struct bgp_nexthop_cache *bnc;
	struct prefix p;
	int is_bgp_static_route = 0;

	if (ri) {
		is_bgp_static_route = ((ri->type == ZEBRA_ROUTE_BGP)
				       && (ri->sub_type == BGP_ROUTE_STATIC))
					      ? 1
					      : 0;

		/* Since Extended Next-hop Encoding (RFC5549) support, we want
		   to derive
		   address-family from the next-hop. */
		if (!is_bgp_static_route)
			afi = BGP_ATTR_NEXTHOP_AFI_IP6(ri->attr) ? AFI_IP6
								 : AFI_IP;

		/* This will return TRUE if the global IPv6 NH is a link local
		 * addr */
		if (make_prefix(afi, ri, &p) < 0)
			return 1;
	} else if (peer) {
		/* Don't register link local NH */
		if (afi == AFI_IP6
		    && IN6_IS_ADDR_LINKLOCAL(&peer->su.sin6.sin6_addr))
			return 1;

		if (!sockunion2hostprefix(&peer->su, &p)) {
			if (BGP_DEBUG(nht, NHT)) {
				zlog_debug(
					"%s: Attempting to register with unknown AFI %d (not %d or %d)",
					__FUNCTION__, afi, AFI_IP, AFI_IP6);
			}
			return 0;
		}
	} else
		return 0;

	if (is_bgp_static_route)
		rn = bgp_node_get(bgp->import_check_table[afi], &p);
	else
		rn = bgp_node_get(bgp->nexthop_cache_table[afi], &p);

	if (!rn->info) {
		bnc = bnc_new();
		rn->info = bnc;
		bnc->node = rn;
		bnc->bgp = bgp;
		bgp_lock_node(rn);
		if (BGP_DEBUG(nht, NHT)) {
			char buf[PREFIX2STR_BUFFER];

			zlog_debug("Allocated bnc %s peer %p",
				   bnc_str(bnc, buf, PREFIX2STR_BUFFER), peer);
		}
	}

	bnc = rn->info;
	bgp_unlock_node(rn);
	if (is_bgp_static_route) {
		SET_FLAG(bnc->flags, BGP_STATIC_ROUTE);

		/* If we're toggling the type, re-register */
		if ((bgp_flag_check(bgp, BGP_FLAG_IMPORT_CHECK))
		    && !CHECK_FLAG(bnc->flags, BGP_STATIC_ROUTE_EXACT_MATCH)) {
			SET_FLAG(bnc->flags, BGP_STATIC_ROUTE_EXACT_MATCH);
			UNSET_FLAG(bnc->flags, BGP_NEXTHOP_REGISTERED);
			UNSET_FLAG(bnc->flags, BGP_NEXTHOP_VALID);
		} else if ((!bgp_flag_check(bgp, BGP_FLAG_IMPORT_CHECK))
			   && CHECK_FLAG(bnc->flags,
					 BGP_STATIC_ROUTE_EXACT_MATCH)) {
			UNSET_FLAG(bnc->flags, BGP_STATIC_ROUTE_EXACT_MATCH);
			UNSET_FLAG(bnc->flags, BGP_NEXTHOP_REGISTERED);
			UNSET_FLAG(bnc->flags, BGP_NEXTHOP_VALID);
		}
	}
	/* When nexthop is already known, but now requires 'connected'
	 * resolution,
	 * re-register it. The reverse scenario where the nexthop currently
	 * requires
	 * 'connected' resolution does not need a re-register (i.e., we treat
	 * 'connected-required' as an override) except in the scenario where
	 * this
	 * is actually a case of tracking a peer for connectivity (e.g., after
	 * disable connected-check).
	 * NOTE: We don't track the number of paths separately for 'connected-
	 * required' vs 'connected-not-required' as this change is not a common
	 * scenario.
	 */
	else if (connected && !CHECK_FLAG(bnc->flags, BGP_NEXTHOP_CONNECTED)) {
		SET_FLAG(bnc->flags, BGP_NEXTHOP_CONNECTED);
		UNSET_FLAG(bnc->flags, BGP_NEXTHOP_REGISTERED);
		UNSET_FLAG(bnc->flags, BGP_NEXTHOP_VALID);
	} else if (peer && !connected
		   && CHECK_FLAG(bnc->flags, BGP_NEXTHOP_CONNECTED)) {
		UNSET_FLAG(bnc->flags, BGP_NEXTHOP_CONNECTED);
		UNSET_FLAG(bnc->flags, BGP_NEXTHOP_REGISTERED);
		UNSET_FLAG(bnc->flags, BGP_NEXTHOP_VALID);
	}
	if (bgp->inst_type == BGP_INSTANCE_TYPE_VIEW) {
		bnc->flags |= BGP_NEXTHOP_REGISTERED;
		bnc->flags |= BGP_NEXTHOP_VALID;
	} else if (!CHECK_FLAG(bnc->flags, BGP_NEXTHOP_REGISTERED))
		register_zebra_rnh(bnc, is_bgp_static_route);
	if (ri && ri->nexthop != bnc) {
		/* Unlink from existing nexthop cache, if any. This will also
		 * free
		 * the nexthop cache entry, if appropriate.
		 */
		bgp_unlink_nexthop(ri);

		path_nh_map(ri, bnc, 1); /* updates NHT ri list reference */

		if (CHECK_FLAG(bnc->flags, BGP_NEXTHOP_VALID) && bnc->metric)
			(bgp_info_extra_get(ri))->igpmetric = bnc->metric;
		else if (ri->extra)
			ri->extra->igpmetric = 0;
	} else if (peer)
		bnc->nht_info = (void *)peer; /* NHT peer reference */

	/*
	 * We are cheating here.  Views have no associated underlying
	 * ability to detect nexthops.  So when we have a view
	 * just tell everyone the nexthop is valid
	 */
	if (bgp->inst_type == BGP_INSTANCE_TYPE_VIEW)
		return 1;
	else
		return (bgp_isvalid_nexthop(bnc));
}

void bgp_delete_connected_nexthop(afi_t afi, struct peer *peer)
{
	struct bgp_node *rn;
	struct bgp_nexthop_cache *bnc;
	struct prefix p;

	if (!peer)
		return;

	/* We don't register link local address for NHT */
	if (afi == AFI_IP6 && IN6_IS_ADDR_LINKLOCAL(&peer->su.sin6.sin6_addr))
		return;

	if (!sockunion2hostprefix(&peer->su, &p))
		return;

	rn = bgp_node_lookup(
		peer->bgp->nexthop_cache_table[family2afi(p.family)], &p);
	if (!rn || !rn->info) {
		if (BGP_DEBUG(nht, NHT))
			zlog_debug("Cannot find connected NHT node for peer %s",
				   peer->host);
		if (rn)
			bgp_unlock_node(rn);
		return;
	}

	bnc = rn->info;
	bgp_unlock_node(rn);

	if (bnc->nht_info != peer) {
		if (BGP_DEBUG(nht, NHT))
			zlog_debug(
				"Connected NHT %p node for peer %s points to %p",
				bnc, peer->host, bnc->nht_info);
		return;
	}

	bnc->nht_info = NULL;

	if (LIST_EMPTY(&(bnc->paths))) {
		if (BGP_DEBUG(nht, NHT))
			zlog_debug("Freeing connected NHT node %p for peer %s",
				   bnc, peer->host);
		unregister_zebra_rnh(bnc, 0);
		bnc->node->info = NULL;
		bgp_unlock_node(bnc->node);
		bnc_free(bnc);
	}
}

void bgp_parse_nexthop_update(int command, vrf_id_t vrf_id)
{
	struct stream *s;
	struct bgp_node *rn = NULL;
	struct bgp_nexthop_cache *bnc;
	struct nexthop *nexthop;
	struct nexthop *oldnh;
	struct nexthop *nhlist_head = NULL;
	struct nexthop *nhlist_tail = NULL;
	uint32_t metric;
	u_char nexthop_num;
	struct prefix p;
	int i;
	struct bgp *bgp;

	bgp = bgp_lookup_by_vrf_id(vrf_id);
	if (!bgp) {
		zlog_err(
			"parse nexthop update: instance not found for vrf_id %d",
			vrf_id);
		return;
	}

	s = zclient->ibuf;

	memset(&p, 0, sizeof(struct prefix));
	p.family = stream_getw(s);
	p.prefixlen = stream_getc(s);
	switch (p.family) {
	case AF_INET:
		p.u.prefix4.s_addr = stream_get_ipv4(s);
		break;
	case AF_INET6:
		stream_get(&p.u.prefix6, s, 16);
		break;
	default:
		break;
	}

	if (command == ZEBRA_NEXTHOP_UPDATE)
		rn = bgp_node_lookup(
			bgp->nexthop_cache_table[family2afi(p.family)], &p);
	else if (command == ZEBRA_IMPORT_CHECK_UPDATE)
		rn = bgp_node_lookup(
			bgp->import_check_table[family2afi(p.family)], &p);

	if (!rn || !rn->info) {
		if (BGP_DEBUG(nht, NHT)) {
			char buf[PREFIX2STR_BUFFER];
			prefix2str(&p, buf, sizeof(buf));
			zlog_debug("parse nexthop update(%s): rn not found",
				   buf);
		}
		if (rn)
			bgp_unlock_node(rn);
		return;
	}

	bnc = rn->info;
	bgp_unlock_node(rn);
	bnc->last_update = bgp_clock();
	bnc->change_flags = 0;
	stream_getc(s); // Distance but not currently used
	metric = stream_getl(s);
	nexthop_num = stream_getc(s);

	/* debug print the input */
	if (BGP_DEBUG(nht, NHT)) {
		char buf[PREFIX2STR_BUFFER];
		prefix2str(&p, buf, sizeof(buf));
		zlog_debug(
			"%d: NH update for %s - metric %d (cur %d) #nhops %d (cur %d)",
			vrf_id, buf, metric, bnc->metric, nexthop_num,
			bnc->nexthop_num);
	}

	if (metric != bnc->metric)
		bnc->change_flags |= BGP_NEXTHOP_METRIC_CHANGED;

	if (nexthop_num != bnc->nexthop_num)
		bnc->change_flags |= BGP_NEXTHOP_CHANGED;

	if (nexthop_num) {
		/* notify bgp fsm if nbr ip goes from invalid->valid */
		if (!bnc->nexthop_num)
			UNSET_FLAG(bnc->flags, BGP_NEXTHOP_PEER_NOTIFIED);

		bnc->flags |= BGP_NEXTHOP_VALID;
		bnc->metric = metric;
		bnc->nexthop_num = nexthop_num;

		for (i = 0; i < nexthop_num; i++) {
			nexthop = nexthop_new();
			nexthop->type = stream_getc(s);
			switch (nexthop->type) {
			case NEXTHOP_TYPE_IPV4:
				nexthop->gate.ipv4.s_addr = stream_get_ipv4(s);
				nexthop->ifindex = stream_getl(s);
				break;
			case NEXTHOP_TYPE_IFINDEX:
				nexthop->ifindex = stream_getl(s);
				break;
			case NEXTHOP_TYPE_IPV4_IFINDEX:
				nexthop->gate.ipv4.s_addr = stream_get_ipv4(s);
				nexthop->ifindex = stream_getl(s);
				break;
			case NEXTHOP_TYPE_IPV6:
				stream_get(&nexthop->gate.ipv6, s, 16);
				nexthop->ifindex = stream_getl(s);
				break;
			case NEXTHOP_TYPE_IPV6_IFINDEX:
				stream_get(&nexthop->gate.ipv6, s, 16);
				nexthop->ifindex = stream_getl(s);
				break;
			default:
				/* do nothing */
				break;
			}

			if (BGP_DEBUG(nht, NHT)) {
				char buf[NEXTHOP_STRLEN];
				zlog_debug(
					"    nhop via %s",
					nexthop2str(nexthop, buf, sizeof(buf)));
			}

			if (nhlist_tail) {
				nhlist_tail->next = nexthop;
				nhlist_tail = nexthop;
			} else {
				nhlist_tail = nexthop;
				nhlist_head = nexthop;
			}

			/* No need to evaluate the nexthop if we have already
			 * determined
			 * that there has been a change.
			 */
			if (bnc->change_flags & BGP_NEXTHOP_CHANGED)
				continue;

			for (oldnh = bnc->nexthop; oldnh; oldnh = oldnh->next)
				if (nexthop_same_no_recurse(oldnh, nexthop))
					break;

			if (!oldnh)
				bnc->change_flags |= BGP_NEXTHOP_CHANGED;
		}
		bnc_nexthop_free(bnc);
		bnc->nexthop = nhlist_head;
	} else {
		bnc->flags &= ~BGP_NEXTHOP_VALID;
		bnc->nexthop_num = nexthop_num;

		/* notify bgp fsm if nbr ip goes from valid->invalid */
		UNSET_FLAG(bnc->flags, BGP_NEXTHOP_PEER_NOTIFIED);

		bnc_nexthop_free(bnc);
		bnc->nexthop = NULL;
	}

	evaluate_paths(bnc);
}

/**
 * make_prefix - make a prefix structure from the path (essentially
 * path's node.
 */
static int make_prefix(int afi, struct bgp_info *ri, struct prefix *p)
{

	int is_bgp_static = ((ri->type == ZEBRA_ROUTE_BGP)
			     && (ri->sub_type == BGP_ROUTE_STATIC))
				    ? 1
				    : 0;

	memset(p, 0, sizeof(struct prefix));
	switch (afi) {
	case AFI_IP:
		p->family = AF_INET;
		if (is_bgp_static) {
			p->u.prefix4 = ri->net->p.u.prefix4;
			p->prefixlen = ri->net->p.prefixlen;
		} else {
			p->u.prefix4 = ri->attr->nexthop;
			p->prefixlen = IPV4_MAX_BITLEN;
		}
		break;
	case AFI_IP6:
		/* We don't register link local NH */
		if (ri->attr->extra->mp_nexthop_len
			    != BGP_ATTR_NHLEN_IPV6_GLOBAL
		    || IN6_IS_ADDR_LINKLOCAL(
			       &ri->attr->extra->mp_nexthop_global))
			return -1;

		p->family = AF_INET6;

		if (is_bgp_static) {
			p->u.prefix6 = ri->net->p.u.prefix6;
			p->prefixlen = ri->net->p.prefixlen;
		} else {
			p->u.prefix6 = ri->attr->extra->mp_nexthop_global;
			p->prefixlen = IPV6_MAX_BITLEN;
		}
		break;
	default:
		if (BGP_DEBUG(nht, NHT)) {
			zlog_debug(
				"%s: Attempting to make prefix with unknown AFI %d (not %d or %d)",
				__FUNCTION__, afi, AFI_IP, AFI_IP6);
		}
		break;
	}
	return 0;
}

/**
 * sendmsg_zebra_rnh -- Format and send a nexthop register/Unregister
 *   command to Zebra.
 * ARGUMENTS:
 *   struct bgp_nexthop_cache *bnc -- the nexthop structure.
 *   int command -- command to send to zebra
 * RETURNS:
 *   void.
 */
static void sendmsg_zebra_rnh(struct bgp_nexthop_cache *bnc, int command)
{
	struct stream *s;
	struct prefix *p;
	int ret;

	/* Check socket. */
	if (!zclient || zclient->sock < 0)
		return;

	/* Don't try to register if Zebra doesn't know of this instance. */
	if (!IS_BGP_INST_KNOWN_TO_ZEBRA(bnc->bgp))
		return;

	p = &(bnc->node->p);
	s = zclient->obuf;
	stream_reset(s);
	zclient_create_header(s, command, bnc->bgp->vrf_id);
	if (CHECK_FLAG(bnc->flags, BGP_NEXTHOP_CONNECTED)
	    || CHECK_FLAG(bnc->flags, BGP_STATIC_ROUTE_EXACT_MATCH))
		stream_putc(s, 1);
	else
		stream_putc(s, 0);

	stream_putw(s, PREFIX_FAMILY(p));
	stream_putc(s, p->prefixlen);
	switch (PREFIX_FAMILY(p)) {
	case AF_INET:
		stream_put_in_addr(s, &p->u.prefix4);
		break;
	case AF_INET6:
		stream_put(s, &(p->u.prefix6), 16);
		break;
	default:
		break;
	}
	stream_putw_at(s, 0, stream_get_endp(s));

	ret = zclient_send_message(zclient);
	/* TBD: handle the failure */
	if (ret < 0)
		zlog_warn("sendmsg_nexthop: zclient_send_message() failed");

	if ((command == ZEBRA_NEXTHOP_REGISTER)
	    || (command == ZEBRA_IMPORT_ROUTE_REGISTER))
		SET_FLAG(bnc->flags, BGP_NEXTHOP_REGISTERED);
	else if ((command == ZEBRA_NEXTHOP_UNREGISTER)
		 || (command == ZEBRA_IMPORT_ROUTE_UNREGISTER))
		UNSET_FLAG(bnc->flags, BGP_NEXTHOP_REGISTERED);
	return;
}

/**
 * register_zebra_rnh - register a NH/route with Zebra for notification
 *    when the route or the route to the nexthop changes.
 * ARGUMENTS:
 *   struct bgp_nexthop_cache *bnc
 * RETURNS:
 *   void.
 */
static void register_zebra_rnh(struct bgp_nexthop_cache *bnc,
			       int is_bgp_import_route)
{
	/* Check if we have already registered */
	if (bnc->flags & BGP_NEXTHOP_REGISTERED)
		return;
	if (is_bgp_import_route)
		sendmsg_zebra_rnh(bnc, ZEBRA_IMPORT_ROUTE_REGISTER);
	else
		sendmsg_zebra_rnh(bnc, ZEBRA_NEXTHOP_REGISTER);
}

/**
 * unregister_zebra_rnh -- Unregister the route/nexthop from Zebra.
 * ARGUMENTS:
 *   struct bgp_nexthop_cache *bnc
 * RETURNS:
 *   void.
 */
static void unregister_zebra_rnh(struct bgp_nexthop_cache *bnc,
				 int is_bgp_import_route)
{
	/* Check if we have already registered */
	if (!CHECK_FLAG(bnc->flags, BGP_NEXTHOP_REGISTERED))
		return;

	if (is_bgp_import_route)
		sendmsg_zebra_rnh(bnc, ZEBRA_IMPORT_ROUTE_UNREGISTER);
	else
		sendmsg_zebra_rnh(bnc, ZEBRA_NEXTHOP_UNREGISTER);
}

/**
 * evaluate_paths - Evaluate the paths/nets associated with a nexthop.
 * ARGUMENTS:
 *   struct bgp_nexthop_cache *bnc -- the nexthop structure.
 * RETURNS:
 *   void.
 */
static void evaluate_paths(struct bgp_nexthop_cache *bnc)
{
	struct bgp_node *rn;
	struct bgp_info *path;
	struct bgp *bgp = bnc->bgp;
	int afi;
	struct peer *peer = (struct peer *)bnc->nht_info;

	if (BGP_DEBUG(nht, NHT)) {
		char buf[PREFIX2STR_BUFFER];
		bnc_str(bnc, buf, PREFIX2STR_BUFFER);
		zlog_debug(
			"NH update for %s - flags 0x%x chgflags 0x%x - evaluate paths",
			buf, bnc->flags, bnc->change_flags);
	}

	LIST_FOREACH(path, &(bnc->paths), nh_thread)
	{
		if (!(path->type == ZEBRA_ROUTE_BGP
		      && ((path->sub_type == BGP_ROUTE_NORMAL)
			  || (path->sub_type == BGP_ROUTE_STATIC))))
			continue;

		rn = path->net;
		afi = family2afi(rn->p.family);

		/* Path becomes valid/invalid depending on whether the nexthop
		 * reachable/unreachable.
		 */
		if ((CHECK_FLAG(path->flags, BGP_INFO_VALID) ? 1 : 0)
		    != (bgp_isvalid_nexthop(bnc) ? 1 : 0)) {
			if (CHECK_FLAG(path->flags, BGP_INFO_VALID)) {
				bgp_aggregate_decrement(bgp, &rn->p, path, afi,
							SAFI_UNICAST);
				bgp_info_unset_flag(rn, path, BGP_INFO_VALID);
			} else {
				bgp_info_set_flag(rn, path, BGP_INFO_VALID);
				bgp_aggregate_increment(bgp, &rn->p, path, afi,
							SAFI_UNICAST);
			}
		}

		/* Copy the metric to the path. Will be used for bestpath
		 * computation */
		if (bgp_isvalid_nexthop(bnc) && bnc->metric)
			(bgp_info_extra_get(path))->igpmetric = bnc->metric;
		else if (path->extra)
			path->extra->igpmetric = 0;

		if (CHECK_FLAG(bnc->change_flags, BGP_NEXTHOP_METRIC_CHANGED)
		    || CHECK_FLAG(bnc->change_flags, BGP_NEXTHOP_CHANGED))
			SET_FLAG(path->flags, BGP_INFO_IGP_CHANGED);

		bgp_process(bgp, rn, afi, SAFI_UNICAST);
	}

	if (peer && !CHECK_FLAG(bnc->flags, BGP_NEXTHOP_PEER_NOTIFIED)) {
		if (BGP_DEBUG(nht, NHT))
			zlog_debug("%s: Updating peer (%s) status with NHT",
				   __FUNCTION__, peer->host);
		bgp_fsm_nht_update(peer, bgp_isvalid_nexthop(bnc));
		SET_FLAG(bnc->flags, BGP_NEXTHOP_PEER_NOTIFIED);
	}

	RESET_FLAG(bnc->change_flags);
}

/**
 * path_nh_map - make or break path-to-nexthop association.
 * ARGUMENTS:
 *   path - pointer to the path structure
 *   bnc - pointer to the nexthop structure
 *   make - if set, make the association. if unset, just break the existing
 *          association.
 */
static void path_nh_map(struct bgp_info *path, struct bgp_nexthop_cache *bnc,
			int make)
{
	if (path->nexthop) {
		LIST_REMOVE(path, nh_thread);
		path->nexthop->path_count--;
		path->nexthop = NULL;
	}
	if (make) {
		LIST_INSERT_HEAD(&(bnc->paths), path, nh_thread);
		path->nexthop = bnc;
		path->nexthop->path_count++;
	}
}
