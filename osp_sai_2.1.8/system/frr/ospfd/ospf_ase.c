/*
 * OSPF AS external route calculation.
 * Copyright (C) 1999, 2000 Alex Zinin, Toshiaki Takada
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

#include "thread.h"
#include "memory.h"
#include "hash.h"
#include "linklist.h"
#include "prefix.h"
#include "if.h"
#include "table.h"
#include "vty.h"
#include "log.h"

#include "ospfd/ospfd.h"
#include "ospfd/ospf_interface.h"
#include "ospfd/ospf_ism.h"
#include "ospfd/ospf_asbr.h"
#include "ospfd/ospf_lsa.h"
#include "ospfd/ospf_lsdb.h"
#include "ospfd/ospf_neighbor.h"
#include "ospfd/ospf_nsm.h"
#include "ospfd/ospf_spf.h"
#include "ospfd/ospf_route.h"
#include "ospfd/ospf_ase.h"
#include "ospfd/ospf_zebra.h"
#include "ospfd/ospf_dump.h"
/* modified by liwh for bug 45943, 2017-12-21 */
#ifdef _CENTEC_
#include "ospfd/ospfd.h"
extern char   ignore_lsa_reason[256];
#endif
/* liwh end */

struct ospf_route *ospf_find_asbr_route(struct ospf *ospf,
					struct route_table *rtrs,
					struct prefix_ipv4 *asbr)
{
	struct route_node *rn;
	struct ospf_route * or, *best = NULL;
	struct listnode *node;
	struct list *chosen;

	/* Sanity check. */
	if (rtrs == NULL)
		return NULL;

	rn = route_node_lookup(rtrs, (struct prefix *)asbr);
	if (!rn)
		return NULL;

	route_unlock_node(rn);

	chosen = list_new();

	/* First try to find intra-area non-bb paths. */
	if (!CHECK_FLAG(ospf->config, OSPF_RFC1583_COMPATIBLE))
		for (ALL_LIST_ELEMENTS_RO((struct list *)rn->info, node, or))
			if (or->cost < OSPF_LS_INFINITY)
				if (!OSPF_IS_AREA_ID_BACKBONE(or->u.std.area_id)
					    &&
				    or->path_type == OSPF_PATH_INTRA_AREA)
					listnode_add(chosen, or);

	/* If none is found -- look through all. */
	if (listcount(chosen) == 0) {
		#if 0 /* djd */
		list_free(chosen);
		#else /* djd */
		list_delete(chosen);
		#endif /* djd */
		chosen = rn->info;
	}

	/* Now find the route with least cost. */
	for (ALL_LIST_ELEMENTS_RO(chosen, node, or))
		if (or->cost < OSPF_LS_INFINITY) {
			if (best == NULL)
				best = or ;
			else if (best->cost > or->cost)
				best = or ;
			else if (best->cost ==
				 or->cost
					   && IPV4_ADDR_CMP(
						      &best->u.std.area_id,
						      & or->u.std.area_id)
						      < 0)
				best = or ;
		}

	if (chosen != rn->info)
		list_delete(chosen);

	return best;
}

struct ospf_route *ospf_find_asbr_route_through_area(struct route_table *rtrs,
						     struct prefix_ipv4 *asbr,
						     struct ospf_area *area)
{
	struct route_node *rn;

	/* Sanity check. */
	if (rtrs == NULL)
		return NULL;

	rn = route_node_lookup(rtrs, (struct prefix *)asbr);

	if (rn) {
		struct listnode *node;
		struct ospf_route * or ;

		route_unlock_node(rn);

		for (ALL_LIST_ELEMENTS_RO((struct list *)rn->info, node, or))
			if (IPV4_ADDR_SAME(& or->u.std.area_id, &area->area_id))
				return or ;
	}

	return NULL;
}

static void ospf_ase_complete_direct_routes(struct ospf_route *ro,
					    struct in_addr nexthop)
{
	struct listnode *node;
	struct ospf_path *op;

	for (ALL_LIST_ELEMENTS_RO(ro->paths, node, op))
		if (op->nexthop.s_addr == 0)
			op->nexthop.s_addr = nexthop.s_addr;
}

static int ospf_ase_forward_address_check(struct ospf *ospf,
					  struct in_addr fwd_addr)
{
	struct listnode *ifn;
	struct ospf_interface *oi;

	for (ALL_LIST_ELEMENTS_RO(ospf->oiflist, ifn, oi))
		if (if_is_operative(oi->ifp))
			if (oi->type != OSPF_IFTYPE_VIRTUALLINK)
				if (IPV4_ADDR_SAME(&oi->address->u.prefix4,
						   &fwd_addr))
					return 0;

	return 1;
}

#if 0
/* Calculate ASBR route. */
static struct ospf_route *
ospf_ase_calculate_asbr_route (struct ospf *ospf,
			       struct route_table *rt_network,
			       struct route_table *rt_router,
			       struct as_external_lsa *al)
{
  struct prefix_ipv4 asbr;
  struct ospf_route *asbr_route;
  struct route_node *rn;

  /* Find ASBR route from Router routing table. */
  asbr.family = AF_INET;
  asbr.prefix = al->header.adv_router;
  asbr.prefixlen = IPV4_MAX_BITLEN;
  apply_mask_ipv4 (&asbr);

  asbr_route = ospf_find_asbr_route (ospf, rt_router, &asbr);

  if (asbr_route == NULL)
    {
      if (IS_DEBUG_OSPF (lsa, LSA))
	zlog_debug ("ospf_ase_calculate(): Route to ASBR %s not found",
		    inet_ntoa (asbr.prefix));
      return NULL;
    }

  if (!(asbr_route->u.std.flags & ROUTER_LSA_EXTERNAL))
    {
      if (IS_DEBUG_OSPF (lsa, LSA))
	zlog_debug ("ospf_ase_calculate(): Originating router is not an ASBR");
      return NULL;
    }
   
  if (al->e[0].fwd_addr.s_addr != 0)
    {
      if (IS_DEBUG_OSPF (lsa, LSA))
	zlog_debug ("ospf_ase_calculate(): "
		    "Forwarding address is not 0.0.0.0.");

      if (! ospf_ase_forward_address_check (ospf, al->e[0].fwd_addr))
	{
	  if (IS_DEBUG_OSPF (lsa, LSA))
	    zlog_debug ("ospf_ase_calculate(): "
			"Forwarding address is one of our addresses, Ignore.");
	  return NULL;
        }

      if (IS_DEBUG_OSPF (lsa, LSA))
	zlog_debug ("ospf_ase_calculate(): "
		    "Looking up in the Network Routing Table.");

      /* Looking up the path to the fwd_addr from Network route. */
      asbr.family = AF_INET;
      asbr.prefix = al->e[0].fwd_addr;
      asbr.prefixlen = IPV4_MAX_BITLEN;

      rn = route_node_match (rt_network, (struct prefix *) &asbr);
   
      if (rn == NULL)
	{
	  if (IS_DEBUG_OSPF (lsa, LSA))
	    zlog_debug ("ospf_ase_calculate(): "
			"Couldn't find a route to the forwarding address.");
	  return NULL;
	}

      route_unlock_node (rn);

      if ((asbr_route = rn->info) == NULL)
	{
	  if (IS_DEBUG_OSPF (lsa, LSA))
	    zlog_debug ("ospf_ase_calculate(): "
			"Somehow OSPF route to ASBR is lost");
	  return NULL;
	}
    }

  return asbr_route;
}
#endif

static struct ospf_route *
ospf_ase_calculate_new_route(struct ospf_lsa *lsa,
			     struct ospf_route *asbr_route, u_int32_t metric)
{
	struct as_external_lsa *al;
	struct ospf_route *new;

	al = (struct as_external_lsa *)lsa->data;

	new = ospf_route_new();

	/* Set redistributed type -- does make sense? */
	/* new->type = type; */
	new->id = al->header.id;
	new->mask = al->mask;

	if (!IS_EXTERNAL_METRIC(al->e[0].tos)) {
		if (IS_DEBUG_OSPF(lsa, LSA))
			zlog_debug("Route[External]: type-1 created.");
		new->path_type = OSPF_PATH_TYPE1_EXTERNAL;
		new->cost = asbr_route->cost + metric; /* X + Y */
	} else {
		if (IS_DEBUG_OSPF(lsa, LSA))
			zlog_debug("Route[External]: type-2 created.");
		new->path_type = OSPF_PATH_TYPE2_EXTERNAL;
		new->cost = asbr_route->cost;   /* X */
		new->u.ext.type2_cost = metric; /* Y */
	}

	new->type = OSPF_DESTINATION_NETWORK;
	new->u.ext.origin = lsa;
	new->u.ext.tag = ntohl(al->e[0].route_tag);
	new->u.ext.asbr = asbr_route;

	assert(new != asbr_route);

	return new;
}

#define OSPF_ASE_CALC_INTERVAL 1

int ospf_ase_calculate_route(struct ospf *ospf, struct ospf_lsa *lsa)
{
	u_int32_t metric;
	struct as_external_lsa *al;
	struct ospf_route *asbr_route;
	struct prefix_ipv4 asbr, p;
	struct route_node *rn;
	struct ospf_route *new, * or ;
	int ret;

	assert(lsa);
	al = (struct as_external_lsa *)lsa->data;

	if (lsa->data->type == OSPF_AS_NSSA_LSA)
		if (IS_DEBUG_OSPF_NSSA)
			zlog_debug("ospf_ase_calc(): Processing Type-7");

	/* Stay away from any Local Translated Type-7 LSAs */
	if (CHECK_FLAG(lsa->flags, OSPF_LSA_LOCAL_XLT)) {
		if (IS_DEBUG_OSPF_NSSA)
			zlog_debug("ospf_ase_calc(): Rejecting Local Xlt'd");
		return 0;
	}

	if (IS_DEBUG_OSPF(lsa, LSA))
		zlog_debug(
			"Route[External]: Calculate AS-external-LSA to %s/%d",
			inet_ntoa(al->header.id), ip_masklen(al->mask));
	/* (1) If the cost specified by the LSA is LSInfinity, or if the
	       LSA's LS age is equal to MaxAge, then examine the next LSA. */
	if ((metric = GET_METRIC(al->e[0].metric)) >= OSPF_LS_INFINITY) {
/* modified by liwh for bug 45819, 2017-12-11 */
#ifdef _CENTEC_
      ospf_lsa_ignored_log_ctc(lsa->data, "Metric is OSPF_LS_INFINITY");
#else
      if (IS_DEBUG_OSPF (lsa, LSA))
        zlog_debug ("Route[External]: Metric is OSPF_LS_INFINITY");
#endif
/*liwh end */
		return 0;
	}
	if (IS_LSA_MAXAGE(lsa)) {
/* modified by liwh for bug 45819, 2017-12-11 */
#ifdef _CENTEC_
      ospf_lsa_ignored_log_ctc(lsa->data, "AS-external-LSA is MAXAGE");
#else
      if (IS_DEBUG_OSPF (lsa, LSA))
        zlog_debug ("Route[External]: AS-external-LSA is MAXAGE");
#endif
/*liwh end */
		return 0;
	}

	/* (2) If the LSA was originated by the calculating router itself,
	   examine the next LSA. */
	if (IS_LSA_SELF(lsa)) {
/* modified by liwh for bug 45819, 2017-12-11 */
#ifdef _CENTEC_
      ospf_lsa_ignored_log_ctc(lsa->data, "AS-external-LSA is self originated");
#else
      if (IS_DEBUG_OSPF (lsa, LSA))
        zlog_debug ("Route[External]: AS-external-LSA is self originated");
#endif
/*liwh end */
		return 0;
	}

/* modified by liwh for bug 45943, 2017-12-21 */
#ifdef _CENTEC_
  if (al->mask.s_addr == 0xffffffff &&
      ospf_zebra_if_lookup_by_local_addr(al->header.id))
    {
      sprintf(ignore_lsa_reason, "%s: ignoring host route %s/32 to self.",
        __func__, inet_ntoa (al->header.id));
      ospf_lsa_ignored_log_ctc(lsa->data, ignore_lsa_reason);
      return;
    }
#endif
/* liwh end */
	/* (3) Call the destination described by the LSA N.  N's address is
	       obtained by masking the LSA's Link State ID with the
	       network/subnet mask contained in the body of the LSA.  Look
	       up the routing table entries (potentially one per attached
	       area) for the AS boundary router (ASBR) that originated the
	       LSA. If no entries exist for router ASBR (i.e., ASBR is
	       unreachable), do nothing with this LSA and consider the next
	       in the list. */

	asbr.family = AF_INET;
	asbr.prefix = al->header.adv_router;
	asbr.prefixlen = IPV4_MAX_BITLEN;
	apply_mask_ipv4(&asbr);

	asbr_route = ospf_find_asbr_route(ospf, ospf->new_rtrs, &asbr);
	if (asbr_route == NULL) {
/* modified by liwh for bug 45819, 2017-12-11 */
#ifdef _CENTEC_
      ospf_lsa_ignored_log_ctc(lsa->data, "Can't find originating ASBR route");
#else
      if (IS_DEBUG_OSPF (lsa, LSA))
        zlog_debug ("Route[External]: Can't find originating ASBR route");
#endif
/*liwh end */
		return 0;
	}
	if (!(asbr_route->u.std.flags & ROUTER_LSA_EXTERNAL)) {
/* modified by liwh for bug 45819, 2017-12-11 */
#ifdef _CENTEC_
      ospf_lsa_ignored_log_ctc(lsa->data, "Originating router is not an ASBR");
#else
      if (IS_DEBUG_OSPF (lsa, LSA))
        zlog_debug ("Route[External]: Originating router is not an ASBR");
#endif
/*liwh end */
		return 0;
	}

	/*     Else, this LSA describes an AS external path to destination
	       N.  Examine the forwarding address specified in the AS-
	       external-LSA.  This indicates the IP address to which
	       packets for the destination should be forwarded. */

	if (al->e[0].fwd_addr.s_addr == 0) {
		/* If the forwarding address is set to 0.0.0.0, packets should
		   be sent to the ASBR itself. Among the multiple routing table
		   entries for the ASBR, select the preferred entry as follows.
		   If RFC1583Compatibility is set to "disabled", prune the set
		   of routing table entries for the ASBR as described in
		   Section 16.4.1. In any case, among the remaining routing
		   table entries, select the routing table entry with the least
		   cost; when there are multiple least cost routing table
		   entries the entry whose associated area has the largest OSPF
		   Area ID (when considered as an unsigned 32-bit integer) is
		   chosen. */

		/* asbr_route already contains the requested route */
	} else {
		/* If the forwarding address is non-zero, look up the
		   forwarding address in the routing table.[24] The matching
		   routing table entry must specify an intra-area or inter-area
		   path; if no such path exists, do nothing with the LSA and
		   consider the next in the list. */
		if (!ospf_ase_forward_address_check(ospf, al->e[0].fwd_addr)) {
/* modified by liwh for bug 45819, 2017-12-11 */
#ifdef _CENTEC_
      ospf_lsa_ignored_log_ctc(lsa->data, "Forwarding address is our router address");
#else
      if (IS_DEBUG_OSPF (lsa, LSA))
        zlog_debug ("Route[External]: Forwarding address is our router "
          "address");
#endif
/*liwh end */
			return 0;
		}

		asbr.family = AF_INET;
		asbr.prefix = al->e[0].fwd_addr;
		asbr.prefixlen = IPV4_MAX_BITLEN;

		rn = route_node_match(ospf->new_table, (struct prefix *)&asbr);

		if (rn == NULL || (asbr_route = rn->info) == NULL) {
/* modified by liwh for bug 45819, 2017-12-11 */
#ifdef _CENTEC_
      ospf_lsa_ignored_log_ctc(lsa->data, "Can't find route to forwarding address");
#else
      if (IS_DEBUG_OSPF (lsa, LSA))
        zlog_debug ("Route[External]: Can't find route to forwarding "
          "address");
#endif
/*liwh end */      
			if (rn)
				route_unlock_node(rn);
			return 0;
		}

		route_unlock_node(rn);
	}

	/* (4) Let X be the cost specified by the preferred routing table
	       entry for the ASBR/forwarding address, and Y the cost
	       specified in the LSA.  X is in terms of the link state
	       metric, and Y is a type 1 or 2 external metric. */


	/* (5) Look up the routing table entry for the destination N.  If
	       no entry exists for N, install the AS external path to N,
	       with next hop equal to the list of next hops to the
	       forwarding address, and advertising router equal to ASBR.
	       If the external metric type is 1, then the path-type is set
	       to type 1 external and the cost is equal to X+Y.  If the
	       external metric type is 2, the path-type is set to type 2
	       external, the link state component of the route's cost is X,
	       and the type 2 cost is Y. */
	new = ospf_ase_calculate_new_route(lsa, asbr_route, metric);

	/* (6) Compare the AS external path described by the LSA with the
	       existing paths in N's routing table entry, as follows. If
	       the new path is preferred, it replaces the present paths in
	       N's routing table entry.  If the new path is of equal
	       preference, it is added to N's routing table entry's list of
	       paths. */

	/* Set prefix. */
	p.family = AF_INET;
	p.prefix = al->header.id;
	p.prefixlen = ip_masklen(al->mask);

	/* if there is a Intra/Inter area route to the N
	   do not install external route */
	if ((rn = route_node_lookup(ospf->new_table, (struct prefix *)&p))) {
		route_unlock_node(rn);
/* modified by liwh for bug 45819, 2017-12-11 */
#ifdef _CENTEC_
      ospf_lsa_ignored_log_ctc(lsa->data, "there is a Intra/Inter area route to the N, "
        "do not install external route");
#else
      if (rn->info == NULL)
        zlog_info ("Route[External]: rn->info NULL");
#endif
/*liwh end */
		if (new)
			ospf_route_free(new);
		return 0;
	}
	/* Find a route to the same dest */
	/* If there is no route, create new one. */
	if ((rn = route_node_lookup(ospf->new_external_route,
				    (struct prefix *)&p)))
		route_unlock_node(rn);

	if (!rn || (or = rn->info) == NULL) {
		if (IS_DEBUG_OSPF(lsa, LSA))
			zlog_debug("Route[External]: Adding a new route %s/%d",
				   inet_ntoa(p.prefix), p.prefixlen);

		ospf_route_add(ospf->new_external_route, &p, new, asbr_route);

		if (al->e[0].fwd_addr.s_addr)
			ospf_ase_complete_direct_routes(new, al->e[0].fwd_addr);
		return 0;
	} else {
		/* (a) Intra-area and inter-area paths are always preferred
		       over AS external paths.

		   (b) Type 1 external paths are always preferred over type 2
		       external paths. When all paths are type 2 external
		       paths, the paths with the smallest advertised type 2
		       metric are always preferred. */
		ret = ospf_route_cmp(ospf, new, or);

		/*     (c) If the new AS external path is still
		   indistinguishable
			   from the current paths in the N's routing table
		   entry,
			   and RFC1583Compatibility is set to "disabled", select
			   the preferred paths based on the intra-AS paths to
		   the
			   ASBR/forwarding addresses, as specified in Section
			   16.4.1.

		       (d) If the new AS external path is still
		   indistinguishable
			   from the current paths in the N's routing table
		   entry,
			   select the preferred path based on a least cost
			   comparison.  Type 1 external paths are compared by
			   looking at the sum of the distance to the forwarding
			   address and the advertised type 1 metric (X+Y).  Type
		   2
			   external paths advertising equal type 2 metrics are
			   compared by looking at the distance to the forwarding
			   addresses.
		*/
		/* New route is better */
		if (ret < 0) {
			if (IS_DEBUG_OSPF(lsa, LSA))
				zlog_debug(
					"Route[External]: New route is better");
			ospf_route_subst(rn, new, asbr_route);
			if (al->e[0].fwd_addr.s_addr)
				ospf_ase_complete_direct_routes(
					new, al->e[0].fwd_addr);
			or = new;
			new = NULL;
		}
		/* Old route is better */
		else if (ret > 0) {
			if (IS_DEBUG_OSPF(lsa, LSA))
				zlog_debug(
					"Route[External]: Old route is better");
			/* do nothing */
		}
		/* Routes are equal */
		else {
			if (IS_DEBUG_OSPF(lsa, LSA))
				zlog_debug("Route[External]: Routes are equal");
			ospf_route_copy_nexthops(or, asbr_route->paths);
			if (al->e[0].fwd_addr.s_addr)
				ospf_ase_complete_direct_routes(
					or, al->e[0].fwd_addr);
		}
	}
	/* Make sure setting newly calculated ASBR route.*/
	or->u.ext.asbr = asbr_route;
	if (new)
		ospf_route_free(new);

	lsa->route = or ;
	return 0;
}

static int ospf_ase_route_match_same(struct route_table *rt,
				     struct prefix *prefix,
				     struct ospf_route *newor)
{
	struct route_node *rn;
	struct ospf_route * or ;
	struct ospf_path *op;
	struct ospf_path *newop;
	struct listnode *n1;
	struct listnode *n2;

	if (!rt || !prefix)
		return 0;

	rn = route_node_lookup(rt, prefix);
	if (!rn)
		return 0;

	route_unlock_node(rn);

	or = rn->info;
	if (or->path_type != newor->path_type)
		return 0;

	switch (or->path_type) {
	case OSPF_PATH_TYPE1_EXTERNAL:
		if (or->cost != newor->cost)
			return 0;
		break;
	case OSPF_PATH_TYPE2_EXTERNAL:
		if ((or->cost != newor->cost)
		    || (or->u.ext.type2_cost != newor->u.ext.type2_cost))
			return 0;
		break;
	default:
		assert(0);
		return 0;
	}

	if (or->paths->count != newor->paths->count)
		return 0;

	/* Check each path. */
	for (n1 = listhead(or->paths), n2 = listhead(newor->paths); n1 && n2;
	     n1 = listnextnode(n1), n2 = listnextnode(n2)) {
		op = listgetdata(n1);
		newop = listgetdata(n2);

		if (!IPV4_ADDR_SAME(&op->nexthop, &newop->nexthop))
			return 0;
		if (op->ifindex != newop->ifindex)
			return 0;
	}

	if (or->u.ext.tag != newor->u.ext.tag)
		return 0;

	return 1;
}

static int ospf_ase_compare_tables(struct route_table *new_external_route,
				   struct route_table *old_external_route)
{
	struct route_node *rn, *new_rn;
	struct ospf_route * or ;

	/* Remove deleted routes */
	for (rn = route_top(old_external_route); rn; rn = route_next(rn))
		if ((or = rn->info)) {
			if (!(new_rn = route_node_lookup(new_external_route,
							 &rn->p)))
				ospf_zebra_delete((struct prefix_ipv4 *)&rn->p,
						  or);
			else
				route_unlock_node(new_rn);
		}


	/* Install new routes */
	for (rn = route_top(new_external_route); rn; rn = route_next(rn))
		if ((or = rn->info) != NULL)
			if (!ospf_ase_route_match_same(old_external_route,
						       &rn->p, or))
				ospf_zebra_add((struct prefix_ipv4 *)&rn->p,
					       or);

	return 0;
}

static int ospf_ase_calculate_timer(struct thread *t)
{
	struct ospf *ospf;
	struct ospf_lsa *lsa;
	struct route_node *rn;
	struct listnode *node;
	struct ospf_area *area;
	struct timeval start_time, stop_time;

	ospf = THREAD_ARG(t);
	ospf->t_ase_calc = NULL;

	if (ospf->ase_calc) {
		ospf->ase_calc = 0;

		monotime(&start_time);

		/* Calculate external route for each AS-external-LSA */
		LSDB_LOOP(EXTERNAL_LSDB(ospf), rn, lsa)
		ospf_ase_calculate_route(ospf, lsa);

		/*  This version simple adds to the table all NSSA areas  */
		if (ospf->anyNSSA)
			for (ALL_LIST_ELEMENTS_RO(ospf->areas, node, area)) {
				if (IS_DEBUG_OSPF_NSSA)
					zlog_debug(
						"ospf_ase_calculate_timer(): looking at area %s",
						inet_ntoa(area->area_id));

				if (area->external_routing == OSPF_AREA_NSSA)
					LSDB_LOOP(NSSA_LSDB(area), rn, lsa)
				ospf_ase_calculate_route(ospf, lsa);
			}
		/* kevinm: And add the NSSA routes in ospf_top */
		LSDB_LOOP(NSSA_LSDB(ospf), rn, lsa)
		ospf_ase_calculate_route(ospf, lsa);

		/* Compare old and new external routing table and install the
		   difference info zebra/kernel */
		ospf_ase_compare_tables(ospf->new_external_route,
					ospf->old_external_route);

		/* Delete old external routing table */
		ospf_route_table_free(ospf->old_external_route);
		ospf->old_external_route = ospf->new_external_route;
		ospf->new_external_route = route_table_init();

		monotime(&stop_time);

/* modified by liwh for bug 43666, 2017-06-27 */
#ifdef _CENTEC_
      /*zlog_info ("SPF Processing Time(usecs): External Routes: %lld\n",
		 (stop_time.tv_sec - start_time.tv_sec)*1000000LL+
		 (stop_time.tv_usec - start_time.tv_usec)); */
#endif
/*liwh end */
	}
	return 0;
}

void ospf_ase_calculate_schedule(struct ospf *ospf)
{
	if (ospf == NULL)
		return;

	ospf->ase_calc = 1;
}

void ospf_ase_calculate_timer_add(struct ospf *ospf)
{
	if (ospf == NULL)
		return;

	if (!ospf->t_ase_calc)
		ospf->t_ase_calc =
			thread_add_timer(master, ospf_ase_calculate_timer, ospf,
					 OSPF_ASE_CALC_INTERVAL);
}

void ospf_ase_register_external_lsa(struct ospf_lsa *lsa, struct ospf *top)
{
	struct route_node *rn;
	struct prefix_ipv4 p;
	struct list *lst;
	struct as_external_lsa *al;

	al = (struct as_external_lsa *)lsa->data;
	p.family = AF_INET;
	p.prefix = lsa->data->id;
	p.prefixlen = ip_masklen(al->mask);
	apply_mask_ipv4(&p);

	rn = route_node_get(top->external_lsas, (struct prefix *)&p);
	if ((lst = rn->info) == NULL)
/* modified by liwh for bug 44298, 2017-06-23   
   route_node_get will lock, therefore should unlock*/
#ifdef _CENTEC_
    {
      if (rn && (rn->lock > 1))        
        {
          route_unlock_node(rn);
        }
#endif
/*liwh end */    
		rn->info = lst = list_new();
/* modified by liwh for bug 44298, 2017-06-23   
   route_node_get will lock, therefore should unlock*/
#ifdef _CENTEC_
    }
#endif
/*liwh end */ 
	else
		route_unlock_node(rn);

	/* We assume that if LSA is deleted from DB
	   is is also deleted from this RT */
	listnode_add(lst, ospf_lsa_lock(lsa)); /* external_lsas lst */
}

void ospf_ase_unregister_external_lsa(struct ospf_lsa *lsa, struct ospf *top)
{
	struct route_node *rn;
	struct prefix_ipv4 p;
	struct list *lst;
	struct as_external_lsa *al;

	al = (struct as_external_lsa *)lsa->data;
	p.family = AF_INET;
	p.prefix = lsa->data->id;
	p.prefixlen = ip_masklen(al->mask);
	apply_mask_ipv4(&p);

	rn = route_node_lookup(top->external_lsas, (struct prefix *)&p);

	if (rn) {
		lst = rn->info;
		listnode_delete(lst, lsa);
		ospf_lsa_unlock(&lsa); /* external_lsas list */
		route_unlock_node(rn);
	}
}

void ospf_ase_external_lsas_finish(struct route_table *rt)
{
	struct route_node *rn;
	struct ospf_lsa *lsa;
	struct list *lst;
	struct listnode *node, *nnode;

	for (rn = route_top(rt); rn; rn = route_next(rn))
		if ((lst = rn->info) != NULL) {
			for (ALL_LIST_ELEMENTS(lst, node, nnode, lsa))
				ospf_lsa_unlock(&lsa); /* external_lsas lst */
			list_delete(lst);
		}

	route_table_finish(rt);
}

void ospf_ase_incremental_update(struct ospf *ospf, struct ospf_lsa *lsa)
{
	struct list *lsas;
	struct listnode *node;
	struct route_node *rn, *rn2;
	struct prefix_ipv4 p;
	struct route_table *tmp_old;
	struct as_external_lsa *al;

	al = (struct as_external_lsa *)lsa->data;
	p.family = AF_INET;
	p.prefix = lsa->data->id;
	p.prefixlen = ip_masklen(al->mask);
	apply_mask_ipv4(&p);

	/* if new_table is NULL, there was no spf calculation, thus
	   incremental update is unneeded */
	if (!ospf->new_table)
		return;

	/* If there is already an intra-area or inter-area route
	   to the destination, no recalculation is necessary
	   (internal routes take precedence). */

	rn = route_node_lookup(ospf->new_table, (struct prefix *)&p);
	if (rn) {
		route_unlock_node(rn);
		if (rn->info)
			return;
	}

	rn = route_node_lookup(ospf->external_lsas, (struct prefix *)&p);
	assert(rn);
	assert(rn->info);
	lsas = rn->info;
	route_unlock_node(rn);

	for (ALL_LIST_ELEMENTS_RO(lsas, node, lsa))
		ospf_ase_calculate_route(ospf, lsa);

	/* prepare temporary old routing table for compare */
	tmp_old = route_table_init();
	rn = route_node_lookup(ospf->old_external_route, (struct prefix *)&p);
	if (rn && rn->info) {
		rn2 = route_node_get(tmp_old, (struct prefix *)&p);
		rn2->info = rn->info;
		route_unlock_node(rn);
	}

	/* install changes to zebra */
	ospf_ase_compare_tables(ospf->new_external_route, tmp_old);

	/* update ospf->old_external_route table */
	if (rn && rn->info)
		ospf_route_free((struct ospf_route *)rn->info);

	rn2 = route_node_lookup(ospf->new_external_route, (struct prefix *)&p);
	/* if new route exists, install it to ospf->old_external_route */
	if (rn2 && rn2->info) {
		if (!rn)
			rn = route_node_get(ospf->old_external_route,
					    (struct prefix *)&p);
		rn->info = rn2->info;
	} else {
		/* remove route node from ospf->old_external_route */
		if (rn) {
			rn->info = NULL;
			route_unlock_node(rn);
		}
	}

	if (rn2) {
		/* rn2->info is stored in route node of ospf->old_external_route
		 */
		rn2->info = NULL;
		route_unlock_node(rn2);
		route_unlock_node(rn2);
	}

	route_table_finish(tmp_old);
}
