/*
 * Copyright (C) 2003 Yasuhiro Ohara
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
 * along with GNU Zebra; see the file COPYING.  If not, write to the
 * Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 */

#include <zebra.h>

#include "log.h"
#include "memory.h"
#include "prefix.h"
#include "command.h"
#include "vty.h"
#include "routemap.h"
#include "table.h"
#include "plist.h"
#include "thread.h"
#include "linklist.h"

#include "ospf6_proto.h"
#include "ospf6_lsa.h"
#include "ospf6_lsdb.h"
#include "ospf6_route.h"
#include "ospf6_zebra.h"
#include "ospf6_message.h"

#include "ospf6_top.h"
#include "ospf6_area.h"
#include "ospf6_interface.h"
#include "ospf6_neighbor.h"
#include "ospf6_asbr.h"
#include "ospf6_intra.h"
#include "ospf6_flood.h"
#include "ospf6d.h"

unsigned char conf_debug_ospf6_asbr = 0;

#define ZROUTE_NAME(x) zebra_route_string(x)

/* AS External LSA origination */
static void ospf6_as_external_lsa_originate(struct ospf6_route *route)
{
	char buffer[OSPF6_MAX_LSASIZE];
	struct ospf6_lsa_header *lsa_header;
	struct ospf6_lsa *lsa;
	struct ospf6_external_info *info = route->route_option;

	struct ospf6_as_external_lsa *as_external_lsa;
	char buf[PREFIX2STR_BUFFER];
	caddr_t p;

	if (IS_OSPF6_DEBUG_ASBR || IS_OSPF6_DEBUG_ORIGINATE(AS_EXTERNAL)) {
		prefix2str(&route->prefix, buf, sizeof(buf));
		zlog_debug("Originate AS-External-LSA for %s", buf);
	}

	/* prepare buffer */
	memset(buffer, 0, sizeof(buffer));
	lsa_header = (struct ospf6_lsa_header *)buffer;
	as_external_lsa = (struct ospf6_as_external_lsa
				   *)((caddr_t)lsa_header
				      + sizeof(struct ospf6_lsa_header));
	p = (caddr_t)((caddr_t)as_external_lsa
		      + sizeof(struct ospf6_as_external_lsa));

	/* Fill AS-External-LSA */
	/* Metric type */
	if (route->path.metric_type == 2)
		SET_FLAG(as_external_lsa->bits_metric, OSPF6_ASBR_BIT_E);
	else
		UNSET_FLAG(as_external_lsa->bits_metric, OSPF6_ASBR_BIT_E);

	/* forwarding address */
	if (!IN6_IS_ADDR_UNSPECIFIED(&info->forwarding))
		SET_FLAG(as_external_lsa->bits_metric, OSPF6_ASBR_BIT_F);
	else
		UNSET_FLAG(as_external_lsa->bits_metric, OSPF6_ASBR_BIT_F);

	/* external route tag */
	if (info->tag)
		SET_FLAG(as_external_lsa->bits_metric, OSPF6_ASBR_BIT_T);
	else
		UNSET_FLAG(as_external_lsa->bits_metric, OSPF6_ASBR_BIT_T);

	/* Set metric */
	OSPF6_ASBR_METRIC_SET(as_external_lsa, route->path.cost);

	/* prefixlen */
	as_external_lsa->prefix.prefix_length = route->prefix.prefixlen;

	/* PrefixOptions */
	as_external_lsa->prefix.prefix_options = route->path.prefix_options;

	/* don't use refer LS-type */
	as_external_lsa->prefix.prefix_refer_lstype = htons(0);

	/* set Prefix */
	memcpy(p, &route->prefix.u.prefix6,
	       OSPF6_PREFIX_SPACE(route->prefix.prefixlen));
	ospf6_prefix_apply_mask(&as_external_lsa->prefix);
	p += OSPF6_PREFIX_SPACE(route->prefix.prefixlen);

	/* Forwarding address */
	if (CHECK_FLAG(as_external_lsa->bits_metric, OSPF6_ASBR_BIT_F)) {
		memcpy(p, &info->forwarding, sizeof(struct in6_addr));
		p += sizeof(struct in6_addr);
	}

	/* External Route Tag */
	if (CHECK_FLAG(as_external_lsa->bits_metric, OSPF6_ASBR_BIT_T)) {
		route_tag_t network_order = htonl(info->tag);

		memcpy(p, &network_order, sizeof(network_order));
		p += sizeof(network_order);
	}

	/* Fill LSA Header */
	lsa_header->age = 0;
	lsa_header->type = htons(OSPF6_LSTYPE_AS_EXTERNAL);
	lsa_header->id = route->path.origin.id;
	lsa_header->adv_router = ospf6->router_id;
	lsa_header->seqnum =
		ospf6_new_ls_seqnum(lsa_header->type, lsa_header->id,
				    lsa_header->adv_router, ospf6->lsdb);
	lsa_header->length = htons((caddr_t)p - (caddr_t)lsa_header);

	/* LSA checksum */
	ospf6_lsa_checksum(lsa_header);

	/* create LSA */
	lsa = ospf6_lsa_create(lsa_header);

	/* Originate */
	ospf6_lsa_originate_process(lsa, ospf6);
}

static route_tag_t ospf6_as_external_lsa_get_tag(struct ospf6_lsa *lsa)
{
	struct ospf6_as_external_lsa *external;
	ptrdiff_t tag_offset;
	route_tag_t network_order;

	if (!lsa)
		return 0;

	external = (struct ospf6_as_external_lsa *)OSPF6_LSA_HEADER_END(
		lsa->header);

	if (!CHECK_FLAG(external->bits_metric, OSPF6_ASBR_BIT_T))
		return 0;

	tag_offset = sizeof(*external)
		     + OSPF6_PREFIX_SPACE(external->prefix.prefix_length);
	if (CHECK_FLAG(external->bits_metric, OSPF6_ASBR_BIT_F))
		tag_offset += sizeof(struct in6_addr);

	memcpy(&network_order, (caddr_t)external + tag_offset,
	       sizeof(network_order));
	return ntohl(network_order);
}

void ospf6_asbr_lsa_add(struct ospf6_lsa *lsa)
{
	struct ospf6_as_external_lsa *external;
	struct prefix asbr_id;
	struct ospf6_route *asbr_entry, *route;
	char buf[PREFIX2STR_BUFFER];

	external = (struct ospf6_as_external_lsa *)OSPF6_LSA_HEADER_END(
		lsa->header);

	if (IS_OSPF6_DEBUG_EXAMIN(AS_EXTERNAL))
		zlog_debug("Calculate AS-External route for %s", lsa->name);

	if (lsa->header->adv_router == ospf6->router_id) {
		if (IS_OSPF6_DEBUG_EXAMIN(AS_EXTERNAL))
			zlog_debug("Ignore self-originated AS-External-LSA");
		return;
	}

	if (OSPF6_ASBR_METRIC(external) == OSPF_LS_INFINITY) {
		if (IS_OSPF6_DEBUG_EXAMIN(AS_EXTERNAL))
			zlog_debug("Ignore LSA with LSInfinity Metric");
		return;
	}

	if (CHECK_FLAG(external->prefix.prefix_options,
		       OSPF6_PREFIX_OPTION_NU)) {
		if (IS_OSPF6_DEBUG_EXAMIN(AS_EXTERNAL))
			zlog_debug("Ignore LSA with NU bit set Metric");
		return;
	}

	ospf6_linkstate_prefix(lsa->header->adv_router, htonl(0), &asbr_id);
	asbr_entry = ospf6_route_lookup(&asbr_id, ospf6->brouter_table);
	if (asbr_entry == NULL
	    || !CHECK_FLAG(asbr_entry->path.router_bits, OSPF6_ROUTER_BIT_E)) {
		if (IS_OSPF6_DEBUG_EXAMIN(AS_EXTERNAL)) {
			prefix2str(&asbr_id, buf, sizeof(buf));
			zlog_debug("ASBR entry not found: %s", buf);
		}
		return;
	}

	route = ospf6_route_create();
	route->type = OSPF6_DEST_TYPE_NETWORK;
	route->prefix.family = AF_INET6;
	route->prefix.prefixlen = external->prefix.prefix_length;
	ospf6_prefix_in6_addr(&route->prefix.u.prefix6, &external->prefix);

	route->path.area_id = asbr_entry->path.area_id;
	route->path.origin.type = lsa->header->type;
	route->path.origin.id = lsa->header->id;
	route->path.origin.adv_router = lsa->header->adv_router;

	route->path.prefix_options = external->prefix.prefix_options;
	if (CHECK_FLAG(external->bits_metric, OSPF6_ASBR_BIT_E)) {
		route->path.type = OSPF6_PATH_TYPE_EXTERNAL2;
		route->path.metric_type = 2;
		route->path.cost = asbr_entry->path.cost;
		route->path.u.cost_e2 = OSPF6_ASBR_METRIC(external);
	} else {
		route->path.type = OSPF6_PATH_TYPE_EXTERNAL1;
		route->path.metric_type = 1;
		route->path.cost =
			asbr_entry->path.cost + OSPF6_ASBR_METRIC(external);
		route->path.u.cost_e2 = 0;
	}

	route->path.tag = ospf6_as_external_lsa_get_tag(lsa);

	ospf6_route_copy_nexthops(route, asbr_entry);

	if (IS_OSPF6_DEBUG_EXAMIN(AS_EXTERNAL)) {
		prefix2str(&route->prefix, buf, sizeof(buf));
		zlog_debug("AS-External route add: %s", buf);
	}

	ospf6_route_add(route, ospf6->route_table);
}

void ospf6_asbr_lsa_remove(struct ospf6_lsa *lsa)
{
	struct ospf6_as_external_lsa *external;
	struct prefix prefix;
	struct ospf6_route *route, *nroute;
	char buf[PREFIX2STR_BUFFER];

	external = (struct ospf6_as_external_lsa *)OSPF6_LSA_HEADER_END(
		lsa->header);

	if (IS_OSPF6_DEBUG_EXAMIN(AS_EXTERNAL))
		zlog_debug("Withdraw AS-External route for %s", lsa->name);

	if (lsa->header->adv_router == ospf6->router_id) {
		if (IS_OSPF6_DEBUG_EXAMIN(AS_EXTERNAL))
			zlog_debug("Ignore self-originated AS-External-LSA");
		return;
	}

	memset(&prefix, 0, sizeof(struct prefix));
	prefix.family = AF_INET6;
	prefix.prefixlen = external->prefix.prefix_length;
	ospf6_prefix_in6_addr(&prefix.u.prefix6, &external->prefix);

	route = ospf6_route_lookup(&prefix, ospf6->route_table);
	if (route == NULL) {
		if (IS_OSPF6_DEBUG_EXAMIN(AS_EXTERNAL)) {
			prefix2str(&prefix, buf, sizeof(buf));
			zlog_debug("AS-External route %s not found", buf);
		}
		return;
	}

	for (ospf6_route_lock(route);
	     route && ospf6_route_is_prefix(&prefix, route); route = nroute) {
		nroute = ospf6_route_next(route);
		if (route->type != OSPF6_DEST_TYPE_NETWORK)
			continue;
		if (route->path.origin.type != lsa->header->type)
			continue;
		if (route->path.origin.id != lsa->header->id)
			continue;
		if (route->path.origin.adv_router != lsa->header->adv_router)
			continue;

		if (IS_OSPF6_DEBUG_EXAMIN(AS_EXTERNAL)) {
			prefix2str(&route->prefix, buf, sizeof(buf));
			zlog_debug("AS-External route remove: %s", buf);
		}
		ospf6_route_remove(route, ospf6->route_table);
	}
	if (route != NULL)
		ospf6_route_unlock(route);
}

void ospf6_asbr_lsentry_add(struct ospf6_route *asbr_entry)
{
	struct ospf6_lsa *lsa;
	u_int16_t type;
	u_int32_t router;

	if (!CHECK_FLAG(asbr_entry->flag, OSPF6_ROUTE_BEST)) {
		char buf[16];
		inet_ntop(AF_INET, &ADV_ROUTER_IN_PREFIX(&asbr_entry->prefix),
			  buf, sizeof(buf));
		zlog_info("ignore non-best path: lsentry %s add", buf);
		return;
	}

	type = htons(OSPF6_LSTYPE_AS_EXTERNAL);
	router = ospf6_linkstate_prefix_adv_router(&asbr_entry->prefix);
	for (lsa = ospf6_lsdb_type_router_head(type, router, ospf6->lsdb); lsa;
	     lsa = ospf6_lsdb_type_router_next(type, router, lsa)) {
		if (!OSPF6_LSA_IS_MAXAGE(lsa))
			ospf6_asbr_lsa_add(lsa);
	}
}

void ospf6_asbr_lsentry_remove(struct ospf6_route *asbr_entry)
{
	struct ospf6_lsa *lsa;
	u_int16_t type;
	u_int32_t router;

	type = htons(OSPF6_LSTYPE_AS_EXTERNAL);
	router = ospf6_linkstate_prefix_adv_router(&asbr_entry->prefix);
	for (lsa = ospf6_lsdb_type_router_head(type, router, ospf6->lsdb); lsa;
	     lsa = ospf6_lsdb_type_router_next(type, router, lsa))
		ospf6_asbr_lsa_remove(lsa);
}


/* redistribute function */

static void ospf6_asbr_routemap_set(int type, const char *mapname)
{
	if (ospf6->rmap[type].name)
		free(ospf6->rmap[type].name);
	ospf6->rmap[type].name = strdup(mapname);
	ospf6->rmap[type].map = route_map_lookup_by_name(mapname);
}

static void ospf6_asbr_routemap_unset(int type)
{
	if (ospf6->rmap[type].name)
		free(ospf6->rmap[type].name);
	ospf6->rmap[type].name = NULL;
	ospf6->rmap[type].map = NULL;
}

static void ospf6_asbr_routemap_update(const char *mapname)
{
	int type;

	if (ospf6 == NULL)
		return;

	for (type = 0; type < ZEBRA_ROUTE_MAX; type++) {
		if (ospf6->rmap[type].name)
			ospf6->rmap[type].map = route_map_lookup_by_name(
				ospf6->rmap[type].name);
		else
			ospf6->rmap[type].map = NULL;
	}
}

int ospf6_asbr_is_asbr(struct ospf6 *o)
{
	return o->external_table->count;
}

static void ospf6_asbr_redistribute_set(int type)
{
	ospf6_zebra_redistribute(type);
}

static void ospf6_asbr_redistribute_unset(int type)
{
	struct ospf6_route *route;
	struct ospf6_external_info *info;

	ospf6_zebra_no_redistribute(type);

	for (route = ospf6_route_head(ospf6->external_table); route;
	     route = ospf6_route_next(route)) {
		info = route->route_option;
		if (info->type != type)
			continue;

		ospf6_asbr_redistribute_remove(info->type, 0, &route->prefix);
	}

	ospf6_asbr_routemap_unset(type);
}

/* When an area is unstubified, flood all the external LSAs in the area */
void ospf6_asbr_send_externals_to_area(struct ospf6_area *oa)
{
	struct ospf6_lsa *lsa;

	for (lsa = ospf6_lsdb_head(oa->ospf6->lsdb); lsa;
	     lsa = ospf6_lsdb_next(lsa)) {
		if (ntohs(lsa->header->type) == OSPF6_LSTYPE_AS_EXTERNAL) {
			zlog_debug("%s: Flooding AS-External LSA %s\n",
				   __func__, lsa->name);
			ospf6_flood_area(NULL, lsa, oa);
		}
	}
}

void ospf6_asbr_redistribute_add(int type, ifindex_t ifindex,
				 struct prefix *prefix, u_int nexthop_num,
				 struct in6_addr *nexthop, route_tag_t tag)
{
	int ret;
	struct ospf6_route troute;
	struct ospf6_external_info tinfo;
	struct ospf6_route *route, *match;
	struct ospf6_external_info *info;
	struct prefix prefix_id;
	struct route_node *node;
	char pbuf[PREFIX2STR_BUFFER], ibuf[16];
	struct listnode *lnode, *lnnode;
	struct ospf6_area *oa;

	if (!ospf6_zebra_is_redistribute(type))
		return;

	if (IS_OSPF6_DEBUG_ASBR) {
		prefix2str(prefix, pbuf, sizeof(pbuf));
		zlog_debug("Redistribute %s (%s)", pbuf, ZROUTE_NAME(type));
	}

	/* if route-map was specified but not found, do not advertise */
	if (ospf6->rmap[type].name) {
		if (ospf6->rmap[type].map == NULL)
			ospf6_asbr_routemap_update(NULL);
		if (ospf6->rmap[type].map == NULL) {
			zlog_warn(
				"route-map \"%s\" not found, suppress redistributing",
				ospf6->rmap[type].name);
			return;
		}
	}

	/* apply route-map */
	if (ospf6->rmap[type].map) {
		memset(&troute, 0, sizeof(troute));
		memset(&tinfo, 0, sizeof(tinfo));
		troute.route_option = &tinfo;
		tinfo.ifindex = ifindex;
		tinfo.tag = tag;

		ret = route_map_apply(ospf6->rmap[type].map, prefix, RMAP_OSPF6,
				      &troute);
		if (ret == RMAP_DENYMATCH) {
			if (IS_OSPF6_DEBUG_ASBR)
				zlog_debug("Denied by route-map \"%s\"",
					   ospf6->rmap[type].name);
			return;
		}
	}

	match = ospf6_route_lookup(prefix, ospf6->external_table);
	if (match) {
		info = match->route_option;

		/* copy result of route-map */
		if (ospf6->rmap[type].map) {
			if (troute.path.metric_type)
				match->path.metric_type =
					troute.path.metric_type;
			if (troute.path.cost)
				match->path.cost = troute.path.cost;
			if (!IN6_IS_ADDR_UNSPECIFIED(&tinfo.forwarding))
				memcpy(&info->forwarding, &tinfo.forwarding,
				       sizeof(struct in6_addr));
			info->tag = tinfo.tag;
		} else {
			/* If there is no route-map, simply update the tag */
			info->tag = tag;
		}

		info->type = type;

		if (nexthop_num && nexthop)
			ospf6_route_add_nexthop(match, ifindex, nexthop);
		else
			ospf6_route_add_nexthop(match, ifindex, NULL);

		/* create/update binding in external_id_table */
		prefix_id.family = AF_INET;
		prefix_id.prefixlen = 32;
		prefix_id.u.prefix4.s_addr = htonl(info->id);
		node = route_node_get(ospf6->external_id_table, &prefix_id);
		node->info = match;

		if (IS_OSPF6_DEBUG_ASBR) {
			inet_ntop(AF_INET, &prefix_id.u.prefix4, ibuf,
				  sizeof(ibuf));
			zlog_debug("Advertise as AS-External Id:%s", ibuf);
		}

		match->path.origin.id = htonl(info->id);
		ospf6_as_external_lsa_originate(match);
		return;
	}

	/* create new entry */
	route = ospf6_route_create();
	route->type = OSPF6_DEST_TYPE_NETWORK;
	memcpy(&route->prefix, prefix, sizeof(struct prefix));

	info = (struct ospf6_external_info *)XCALLOC(
		MTYPE_OSPF6_EXTERNAL_INFO, sizeof(struct ospf6_external_info));
	route->route_option = info;
	info->id = ospf6->external_id++;

	/* copy result of route-map */
	if (ospf6->rmap[type].map) {
		if (troute.path.metric_type)
			route->path.metric_type = troute.path.metric_type;
		if (troute.path.cost)
			route->path.cost = troute.path.cost;
		if (!IN6_IS_ADDR_UNSPECIFIED(&tinfo.forwarding))
			memcpy(&info->forwarding, &tinfo.forwarding,
			       sizeof(struct in6_addr));
		info->tag = tinfo.tag;
	} else {
		/* If there is no route-map, simply set the tag */
		info->tag = tag;
	}

	info->type = type;
	if (nexthop_num && nexthop)
		ospf6_route_add_nexthop(route, ifindex, nexthop);
	else
		ospf6_route_add_nexthop(route, ifindex, NULL);

	/* create/update binding in external_id_table */
	prefix_id.family = AF_INET;
	prefix_id.prefixlen = 32;
	prefix_id.u.prefix4.s_addr = htonl(info->id);
	node = route_node_get(ospf6->external_id_table, &prefix_id);
	node->info = route;

	route = ospf6_route_add(route, ospf6->external_table);
	route->route_option = info;

	if (IS_OSPF6_DEBUG_ASBR) {
		inet_ntop(AF_INET, &prefix_id.u.prefix4, ibuf, sizeof(ibuf));
		zlog_debug("Advertise as AS-External Id:%s", ibuf);
	}

	route->path.origin.id = htonl(info->id);
	ospf6_as_external_lsa_originate(route);

	/* Router-Bit (ASBR Flag) may have to be updated */
	for (ALL_LIST_ELEMENTS(ospf6->area_list, lnode, lnnode, oa))
		OSPF6_ROUTER_LSA_SCHEDULE(oa);
}

void ospf6_asbr_redistribute_remove(int type, ifindex_t ifindex,
				    struct prefix *prefix)
{
	struct ospf6_route *match;
	struct ospf6_external_info *info = NULL;
	struct route_node *node;
	struct ospf6_lsa *lsa;
	struct prefix prefix_id;
	char pbuf[PREFIX2STR_BUFFER], ibuf[16];
	struct listnode *lnode, *lnnode;
	struct ospf6_area *oa;

	match = ospf6_route_lookup(prefix, ospf6->external_table);
	if (match == NULL) {
		if (IS_OSPF6_DEBUG_ASBR) {
			prefix2str(prefix, pbuf, sizeof(pbuf));
			zlog_debug("No such route %s to withdraw", pbuf);
		}
		return;
	}

	info = match->route_option;
	assert(info);

	if (info->type != type) {
		if (IS_OSPF6_DEBUG_ASBR) {
			prefix2str(prefix, pbuf, sizeof(pbuf));
			zlog_debug("Original protocol mismatch: %s", pbuf);
		}
		return;
	}

	if (IS_OSPF6_DEBUG_ASBR) {
		prefix2str(prefix, pbuf, sizeof(pbuf));
		inet_ntop(AF_INET, &prefix_id.u.prefix4, ibuf, sizeof(ibuf));
		zlog_debug("Withdraw %s (AS-External Id:%s)", pbuf, ibuf);
	}

	lsa = ospf6_lsdb_lookup(htons(OSPF6_LSTYPE_AS_EXTERNAL),
				htonl(info->id), ospf6->router_id, ospf6->lsdb);
	if (lsa)
		ospf6_lsa_purge(lsa);

	/* remove binding in external_id_table */
	prefix_id.family = AF_INET;
	prefix_id.prefixlen = 32;
	prefix_id.u.prefix4.s_addr = htonl(info->id);
	node = route_node_lookup(ospf6->external_id_table, &prefix_id);
	assert(node);
	node->info = NULL;
	route_unlock_node(node);

	ospf6_route_remove(match, ospf6->external_table);
	XFREE(MTYPE_OSPF6_EXTERNAL_INFO, info);

	/* Router-Bit (ASBR Flag) may have to be updated */
	for (ALL_LIST_ELEMENTS(ospf6->area_list, lnode, lnnode, oa))
		OSPF6_ROUTER_LSA_SCHEDULE(oa);
}

DEFUN (ospf6_redistribute,
       ospf6_redistribute_cmd,
       "redistribute " FRR_REDIST_STR_OSPF6D,
       "Redistribute\n"
       FRR_REDIST_HELP_STR_OSPF6D)
{
	int type;

	char *proto = argv[argc - 1]->text;
	type = proto_redistnum(AFI_IP6, proto);
	if (type < 0)
		return CMD_WARNING;

	ospf6_asbr_redistribute_unset(type);
	ospf6_asbr_redistribute_set(type);
	return CMD_SUCCESS;
}

DEFUN (ospf6_redistribute_routemap,
       ospf6_redistribute_routemap_cmd,
       "redistribute " FRR_REDIST_STR_OSPF6D " route-map WORD",
       "Redistribute\n"
       FRR_REDIST_HELP_STR_OSPF6D
       "Route map reference\n"
       "Route map name\n")
{
	int idx_protocol = 1;
	int idx_word = 3;
	int type;

	char *proto = argv[idx_protocol]->text;
	type = proto_redistnum(AFI_IP6, proto);
	if (type < 0)
		return CMD_WARNING;

	ospf6_asbr_redistribute_unset(type);
	ospf6_asbr_routemap_set(type, argv[idx_word]->arg);
	ospf6_asbr_redistribute_set(type);
	return CMD_SUCCESS;
}

DEFUN (no_ospf6_redistribute,
       no_ospf6_redistribute_cmd,
       "no redistribute " FRR_REDIST_STR_OSPF6D " [route-map WORD]",
       NO_STR
       "Redistribute\n"
       FRR_REDIST_HELP_STR_OSPF6D
       "Route map reference\n"
       "Route map name\n")
{
	int idx_protocol = 2;
	int type;

	char *proto = argv[idx_protocol]->text;
	type = proto_redistnum(AFI_IP6, proto);
	if (type < 0)
		return CMD_WARNING;

	ospf6_asbr_redistribute_unset(type);

	return CMD_SUCCESS;
}

int ospf6_redistribute_config_write(struct vty *vty)
{
	int type;

	for (type = 0; type < ZEBRA_ROUTE_MAX; type++) {
		if (type == ZEBRA_ROUTE_OSPF6)
			continue;
		if (!ospf6_zebra_is_redistribute(type))
			continue;

		if (ospf6->rmap[type].name)
			vty_out(vty, " redistribute %s route-map %s%s",
				ZROUTE_NAME(type), ospf6->rmap[type].name, VNL);
		else
			vty_out(vty, " redistribute %s%s", ZROUTE_NAME(type),
				VNL);
	}

	return 0;
}

static void ospf6_redistribute_show_config(struct vty *vty)
{
	int type;
	int nroute[ZEBRA_ROUTE_MAX];
	int total;
	struct ospf6_route *route;
	struct ospf6_external_info *info;

	total = 0;
	for (type = 0; type < ZEBRA_ROUTE_MAX; type++)
		nroute[type] = 0;
	for (route = ospf6_route_head(ospf6->external_table); route;
	     route = ospf6_route_next(route)) {
		info = route->route_option;
		nroute[info->type]++;
		total++;
	}

	vty_out(vty, "Redistributing External Routes from:%s", VNL);
	for (type = 0; type < ZEBRA_ROUTE_MAX; type++) {
		if (type == ZEBRA_ROUTE_OSPF6)
			continue;
		if (!ospf6_zebra_is_redistribute(type))
			continue;

		if (ospf6->rmap[type].name)
			vty_out(vty, "    %d: %s with route-map \"%s\"%s%s",
				nroute[type], ZROUTE_NAME(type),
				ospf6->rmap[type].name,
				(ospf6->rmap[type].map ? "" : " (not found !)"),
				VNL);
		else
			vty_out(vty, "    %d: %s%s", nroute[type],
				ZROUTE_NAME(type), VNL);
	}
	vty_out(vty, "Total %d routes%s", total, VNL);
}


/* Routemap Functions */
static route_map_result_t
ospf6_routemap_rule_match_address_prefixlist(void *rule, struct prefix *prefix,
					     route_map_object_t type,
					     void *object)
{
	struct prefix_list *plist;

	if (type != RMAP_OSPF6)
		return RMAP_NOMATCH;

	plist = prefix_list_lookup(AFI_IP6, (char *)rule);
	if (plist == NULL)
		return RMAP_NOMATCH;

	return (prefix_list_apply(plist, prefix) == PREFIX_DENY ? RMAP_NOMATCH
								: RMAP_MATCH);
}

static void *
ospf6_routemap_rule_match_address_prefixlist_compile(const char *arg)
{
	return XSTRDUP(MTYPE_ROUTE_MAP_COMPILED, arg);
}

static void ospf6_routemap_rule_match_address_prefixlist_free(void *rule)
{
	XFREE(MTYPE_ROUTE_MAP_COMPILED, rule);
}

struct route_map_rule_cmd ospf6_routemap_rule_match_address_prefixlist_cmd = {
	"ipv6 address prefix-list",
	ospf6_routemap_rule_match_address_prefixlist,
	ospf6_routemap_rule_match_address_prefixlist_compile,
	ospf6_routemap_rule_match_address_prefixlist_free,
};

/* `match interface IFNAME' */
/* Match function should return 1 if match is success else return
   zero. */
static route_map_result_t
ospf6_routemap_rule_match_interface(void *rule, struct prefix *prefix,
				    route_map_object_t type, void *object)
{
	struct interface *ifp;
	struct ospf6_external_info *ei;

	if (type == RMAP_OSPF6) {
		ei = ((struct ospf6_route *)object)->route_option;
		ifp = if_lookup_by_name((char *)rule, VRF_DEFAULT);

		if (ifp != NULL && ei->ifindex == ifp->ifindex)
			return RMAP_MATCH;
	}

	return RMAP_NOMATCH;
}

/* Route map `interface' match statement.  `arg' should be
   interface name. */
static void *ospf6_routemap_rule_match_interface_compile(const char *arg)
{
	return XSTRDUP(MTYPE_ROUTE_MAP_COMPILED, arg);
}

/* Free route map's compiled `interface' value. */
static void ospf6_routemap_rule_match_interface_free(void *rule)
{
	XFREE(MTYPE_ROUTE_MAP_COMPILED, rule);
}

/* Route map commands for interface matching. */
struct route_map_rule_cmd ospf6_routemap_rule_match_interface_cmd = {
	"interface", ospf6_routemap_rule_match_interface,
	ospf6_routemap_rule_match_interface_compile,
	ospf6_routemap_rule_match_interface_free};

/* Match function for matching route tags */
static route_map_result_t ospf6_routemap_rule_match_tag(void *rule,
							struct prefix *prefix,
							route_map_object_t type,
							void *object)
{
	route_tag_t *tag = rule;
	struct ospf6_route *route = object;
	struct ospf6_external_info *info = route->route_option;

	if (type == RMAP_OSPF6 && info->tag == *tag)
		return RMAP_MATCH;

	return RMAP_NOMATCH;
}

static struct route_map_rule_cmd ospf6_routemap_rule_match_tag_cmd = {
	"tag", ospf6_routemap_rule_match_tag, route_map_rule_tag_compile,
	route_map_rule_tag_free,
};

static route_map_result_t
ospf6_routemap_rule_set_metric_type(void *rule, struct prefix *prefix,
				    route_map_object_t type, void *object)
{
	char *metric_type = rule;
	struct ospf6_route *route = object;

	if (type != RMAP_OSPF6)
		return RMAP_OKAY;

	if (strcmp(metric_type, "type-2") == 0)
		route->path.metric_type = 2;
	else
		route->path.metric_type = 1;

	return RMAP_OKAY;
}

static void *ospf6_routemap_rule_set_metric_type_compile(const char *arg)
{
	if (strcmp(arg, "type-2") && strcmp(arg, "type-1"))
		return NULL;
	return XSTRDUP(MTYPE_ROUTE_MAP_COMPILED, arg);
}

static void ospf6_routemap_rule_set_metric_type_free(void *rule)
{
	XFREE(MTYPE_ROUTE_MAP_COMPILED, rule);
}

struct route_map_rule_cmd ospf6_routemap_rule_set_metric_type_cmd = {
	"metric-type", ospf6_routemap_rule_set_metric_type,
	ospf6_routemap_rule_set_metric_type_compile,
	ospf6_routemap_rule_set_metric_type_free,
};

static route_map_result_t
ospf6_routemap_rule_set_metric(void *rule, struct prefix *prefix,
			       route_map_object_t type, void *object)
{
	char *metric = rule;
	struct ospf6_route *route = object;

	if (type != RMAP_OSPF6)
		return RMAP_OKAY;

	route->path.cost = atoi(metric);
	return RMAP_OKAY;
}

static void *ospf6_routemap_rule_set_metric_compile(const char *arg)
{
	u_int32_t metric;
	char *endp;
	metric = strtoul(arg, &endp, 0);
	if (metric > OSPF_LS_INFINITY || *endp != '\0')
		return NULL;
	return XSTRDUP(MTYPE_ROUTE_MAP_COMPILED, arg);
}

static void ospf6_routemap_rule_set_metric_free(void *rule)
{
	XFREE(MTYPE_ROUTE_MAP_COMPILED, rule);
}

struct route_map_rule_cmd ospf6_routemap_rule_set_metric_cmd = {
	"metric", ospf6_routemap_rule_set_metric,
	ospf6_routemap_rule_set_metric_compile,
	ospf6_routemap_rule_set_metric_free,
};

static route_map_result_t
ospf6_routemap_rule_set_forwarding(void *rule, struct prefix *prefix,
				   route_map_object_t type, void *object)
{
	char *forwarding = rule;
	struct ospf6_route *route = object;
	struct ospf6_external_info *info = route->route_option;

	if (type != RMAP_OSPF6)
		return RMAP_OKAY;

	if (inet_pton(AF_INET6, forwarding, &info->forwarding) != 1) {
		memset(&info->forwarding, 0, sizeof(struct in6_addr));
		return RMAP_ERROR;
	}

	return RMAP_OKAY;
}

static void *ospf6_routemap_rule_set_forwarding_compile(const char *arg)
{
	struct in6_addr a;
	if (inet_pton(AF_INET6, arg, &a) != 1)
		return NULL;
	return XSTRDUP(MTYPE_ROUTE_MAP_COMPILED, arg);
}

static void ospf6_routemap_rule_set_forwarding_free(void *rule)
{
	XFREE(MTYPE_ROUTE_MAP_COMPILED, rule);
}

struct route_map_rule_cmd ospf6_routemap_rule_set_forwarding_cmd = {
	"forwarding-address", ospf6_routemap_rule_set_forwarding,
	ospf6_routemap_rule_set_forwarding_compile,
	ospf6_routemap_rule_set_forwarding_free,
};

static route_map_result_t ospf6_routemap_rule_set_tag(void *rule,
						      struct prefix *prefix,
						      route_map_object_t type,
						      void *object)
{
	route_tag_t *tag = rule;
	struct ospf6_route *route = object;
	struct ospf6_external_info *info = route->route_option;

	if (type != RMAP_OSPF6)
		return RMAP_OKAY;

	info->tag = *tag;
	return RMAP_OKAY;
}

static struct route_map_rule_cmd ospf6_routemap_rule_set_tag_cmd = {
	"tag", ospf6_routemap_rule_set_tag, route_map_rule_tag_compile,
	route_map_rule_tag_free,
};

static int route_map_command_status(struct vty *vty, int ret)
{
	if (!ret)
		return CMD_SUCCESS;

	switch (ret) {
	case RMAP_RULE_MISSING:
		vty_out(vty, "OSPF6 Can't find rule.%s", VNL);
		break;
	case RMAP_COMPILE_ERROR:
		vty_out(vty, "OSPF6 Argument is malformed.%s", VNL);
		break;
	default:
		vty_out(vty, "OSPF6 route-map add set failed.%s", VNL);
		break;
	}
	return CMD_WARNING;
}

/* add "set metric-type" */
DEFUN (ospf6_routemap_set_metric_type,
       ospf6_routemap_set_metric_type_cmd,
       "set metric-type <type-1|type-2>",
       "Set value\n"
       "Type of metric\n"
       "OSPF6 external type 1 metric\n"
       "OSPF6 external type 2 metric\n")
{
	VTY_DECLVAR_CONTEXT(route_map_index, route_map_index);
	int idx_external = 2;
	int ret = route_map_add_set(route_map_index, "metric-type",
				    argv[idx_external]->arg);
	return route_map_command_status(vty, ret);
}

/* delete "set metric-type" */
DEFUN (ospf6_routemap_no_set_metric_type,
       ospf6_routemap_no_set_metric_type_cmd,
       "no set metric-type [<type-1|type-2>]",
       NO_STR
       "Set value\n"
       "Type of metric\n"
       "OSPF6 external type 1 metric\n"
       "OSPF6 external type 2 metric\n")
{
	VTY_DECLVAR_CONTEXT(route_map_index, route_map_index);
	char *ext = (argc == 4) ? argv[3]->text : NULL;
	int ret = route_map_delete_set(route_map_index, "metric-type", ext);
	return route_map_command_status(vty, ret);
}

/* add "set forwarding-address" */
DEFUN (ospf6_routemap_set_forwarding,
       ospf6_routemap_set_forwarding_cmd,
       "set forwarding-address X:X::X:X",
       "Set value\n"
       "Forwarding Address\n"
       "IPv6 Address\n")
{
	VTY_DECLVAR_CONTEXT(route_map_index, route_map_index);
	int idx_ipv6 = 2;
	int ret = route_map_add_set(route_map_index, "forwarding-address",
				    argv[idx_ipv6]->arg);
	return route_map_command_status(vty, ret);
}

/* delete "set forwarding-address" */
DEFUN (ospf6_routemap_no_set_forwarding,
       ospf6_routemap_no_set_forwarding_cmd,
       "no set forwarding-address X:X::X:X",
       NO_STR
       "Set value\n"
       "Forwarding Address\n"
       "IPv6 Address\n")
{
	VTY_DECLVAR_CONTEXT(route_map_index, route_map_index);
	int idx_ipv6 = 3;
	int ret = route_map_delete_set(route_map_index, "forwarding-address",
				       argv[idx_ipv6]->arg);
	return route_map_command_status(vty, ret);
}

static void ospf6_routemap_init(void)
{
	route_map_init();

	route_map_add_hook(ospf6_asbr_routemap_update);
	route_map_delete_hook(ospf6_asbr_routemap_update);

	route_map_set_metric_hook(generic_set_add);
	route_map_no_set_metric_hook(generic_set_delete);

	route_map_match_tag_hook(generic_match_add);
	route_map_no_match_tag_hook(generic_match_delete);

	route_map_match_ipv6_address_prefix_list_hook(generic_match_add);
	route_map_no_match_ipv6_address_prefix_list_hook(generic_match_delete);

	route_map_match_interface_hook(generic_match_add);
	route_map_no_match_interface_hook(generic_match_delete);

	route_map_install_match(
		&ospf6_routemap_rule_match_address_prefixlist_cmd);
	route_map_install_match(&ospf6_routemap_rule_match_interface_cmd);
	route_map_install_match(&ospf6_routemap_rule_match_tag_cmd);

	route_map_install_set(&ospf6_routemap_rule_set_metric_type_cmd);
	route_map_install_set(&ospf6_routemap_rule_set_metric_cmd);
	route_map_install_set(&ospf6_routemap_rule_set_forwarding_cmd);
	route_map_install_set(&ospf6_routemap_rule_set_tag_cmd);

	/* ASE Metric Type (e.g. Type-1/Type-2) */
	install_element(RMAP_NODE, &ospf6_routemap_set_metric_type_cmd);
	install_element(RMAP_NODE, &ospf6_routemap_no_set_metric_type_cmd);

	/* ASE Metric */
	install_element(RMAP_NODE, &ospf6_routemap_set_forwarding_cmd);
	install_element(RMAP_NODE, &ospf6_routemap_no_set_forwarding_cmd);
}


/* Display functions */
static char *ospf6_as_external_lsa_get_prefix_str(struct ospf6_lsa *lsa,
						  char *buf, int buflen,
						  int pos)
{
	struct ospf6_as_external_lsa *external;
	struct in6_addr in6;
	int prefix_length = 0;

	if (lsa) {
		external = (struct ospf6_as_external_lsa *)OSPF6_LSA_HEADER_END(
			lsa->header);

		if (pos == 0) {
			ospf6_prefix_in6_addr(&in6, &external->prefix);
			prefix_length = external->prefix.prefix_length;
		} else {
			in6 = *((struct in6_addr
					 *)((caddr_t)external
					    + sizeof(struct
						     ospf6_as_external_lsa)
					    + OSPF6_PREFIX_SPACE(
						      external->prefix
							      .prefix_length)));
		}
		if (buf) {
			inet_ntop(AF_INET6, &in6, buf, buflen);
			if (prefix_length)
				sprintf(&buf[strlen(buf)], "/%d",
					prefix_length);
		}
	}
	return (buf);
}

static int ospf6_as_external_lsa_show(struct vty *vty, struct ospf6_lsa *lsa)
{
	struct ospf6_as_external_lsa *external;
	char buf[64];

	assert(lsa->header);
	external = (struct ospf6_as_external_lsa *)OSPF6_LSA_HEADER_END(
		lsa->header);

	/* bits */
	snprintf(buf, sizeof(buf), "%c%c%c",
		 (CHECK_FLAG(external->bits_metric, OSPF6_ASBR_BIT_E) ? 'E'
								      : '-'),
		 (CHECK_FLAG(external->bits_metric, OSPF6_ASBR_BIT_F) ? 'F'
								      : '-'),
		 (CHECK_FLAG(external->bits_metric, OSPF6_ASBR_BIT_T) ? 'T'
								      : '-'));

	vty_out(vty, "     Bits: %s%s", buf, VNL);
	vty_out(vty, "     Metric: %5lu%s", (u_long)OSPF6_ASBR_METRIC(external),
		VNL);

	ospf6_prefix_options_printbuf(external->prefix.prefix_options, buf,
				      sizeof(buf));
	vty_out(vty, "     Prefix Options: %s%s", buf, VNL);

	vty_out(vty, "     Referenced LSType: %d%s",
		ntohs(external->prefix.prefix_refer_lstype), VNL);

	vty_out(vty, "     Prefix: %s%s",
		ospf6_as_external_lsa_get_prefix_str(lsa, buf, sizeof(buf), 0),
		VNL);

	/* Forwarding-Address */
	if (CHECK_FLAG(external->bits_metric, OSPF6_ASBR_BIT_F)) {
		vty_out(vty, "     Forwarding-Address: %s%s",
			ospf6_as_external_lsa_get_prefix_str(lsa, buf,
							     sizeof(buf), 1),
			VNL);
	}

	/* Tag */
	if (CHECK_FLAG(external->bits_metric, OSPF6_ASBR_BIT_T)) {
		vty_out(vty, "     Tag: %" ROUTE_TAG_PRI "%s",
			ospf6_as_external_lsa_get_tag(lsa), VNL);
	}

	return 0;
}

static void ospf6_asbr_external_route_show(struct vty *vty,
					   struct ospf6_route *route)
{
	struct ospf6_external_info *info = route->route_option;
	char prefix[PREFIX2STR_BUFFER], id[16], forwarding[64];
	u_int32_t tmp_id;

	prefix2str(&route->prefix, prefix, sizeof(prefix));
	tmp_id = ntohl(info->id);
	inet_ntop(AF_INET, &tmp_id, id, sizeof(id));
	if (!IN6_IS_ADDR_UNSPECIFIED(&info->forwarding))
		inet_ntop(AF_INET6, &info->forwarding, forwarding,
			  sizeof(forwarding));
	else
		snprintf(forwarding, sizeof(forwarding), ":: (ifindex %d)",
			 ospf6_route_get_first_nh_index(route));

	vty_out(vty, "%c %-32s %-15s type-%d %5lu %s%s",
		zebra_route_char(info->type), prefix, id,
		route->path.metric_type,
		(u_long)(route->path.metric_type == 2 ? route->path.u.cost_e2
						      : route->path.cost),
		forwarding, VNL);
}

DEFUN (show_ipv6_ospf6_redistribute,
       show_ipv6_ospf6_redistribute_cmd,
       "show ipv6 ospf6 redistribute",
       SHOW_STR
       IP6_STR
       OSPF6_STR
       "redistributing External information\n"
       )
{
	struct ospf6_route *route;

	OSPF6_CMD_CHECK_RUNNING();

	ospf6_redistribute_show_config(vty);

	for (route = ospf6_route_head(ospf6->external_table); route;
	     route = ospf6_route_next(route))
		ospf6_asbr_external_route_show(vty, route);

	return CMD_SUCCESS;
}

struct ospf6_lsa_handler as_external_handler = {
	OSPF6_LSTYPE_AS_EXTERNAL, "AS-External", "ASE",
	ospf6_as_external_lsa_show, ospf6_as_external_lsa_get_prefix_str};

void ospf6_asbr_init(void)
{
	ospf6_routemap_init();

	ospf6_install_lsa_handler(&as_external_handler);

	install_element(VIEW_NODE, &show_ipv6_ospf6_redistribute_cmd);

	install_element(OSPF6_NODE, &ospf6_redistribute_cmd);
	install_element(OSPF6_NODE, &ospf6_redistribute_routemap_cmd);
	install_element(OSPF6_NODE, &no_ospf6_redistribute_cmd);
}

void ospf6_asbr_redistribute_reset(void)
{
	int type;

	for (type = 0; type < ZEBRA_ROUTE_MAX; type++) {
		if (type == ZEBRA_ROUTE_OSPF6)
			continue;
		if (ospf6_zebra_is_redistribute(type))
			ospf6_asbr_redistribute_unset(type);
	}
}

void ospf6_asbr_terminate(void)
{
	route_map_finish();
}

DEFUN (debug_ospf6_asbr,
       debug_ospf6_asbr_cmd,
       "debug ospf6 asbr",
       DEBUG_STR
       OSPF6_STR
       "Debug OSPFv3 ASBR function\n"
      )
{
	OSPF6_DEBUG_ASBR_ON();
	return CMD_SUCCESS;
}

DEFUN (no_debug_ospf6_asbr,
       no_debug_ospf6_asbr_cmd,
       "no debug ospf6 asbr",
       NO_STR
       DEBUG_STR
       OSPF6_STR
       "Debug OSPFv3 ASBR function\n"
      )
{
	OSPF6_DEBUG_ASBR_OFF();
	return CMD_SUCCESS;
}

int config_write_ospf6_debug_asbr(struct vty *vty)
{
	if (IS_OSPF6_DEBUG_ASBR)
		vty_out(vty, "debug ospf6 asbr%s", VNL);
	return 0;
}

void install_element_ospf6_debug_asbr()
{
	install_element(ENABLE_NODE, &debug_ospf6_asbr_cmd);
	install_element(ENABLE_NODE, &no_debug_ospf6_asbr_cmd);
	install_element(CONFIG_NODE, &debug_ospf6_asbr_cmd);
	install_element(CONFIG_NODE, &no_debug_ospf6_asbr_cmd);
}
