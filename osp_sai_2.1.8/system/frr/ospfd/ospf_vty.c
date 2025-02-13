/* OSPF VTY interface.
 * Copyright (C) 2005 6WIND <alain.ritoux@6wind.com>
 * Copyright (C) 2000 Toshiaki Takada
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
#include <string.h>

#include "monotime.h"
#include "memory.h"
#include "thread.h"
#include "prefix.h"
#include "table.h"
#include "vty.h"
#include "command.h"
#include "plist.h"
#include "log.h"
#include "zclient.h"
#include <lib/json.h>
#include "defaults.h"

#include "ospfd/ospfd.h"
#include "ospfd/ospf_asbr.h"
#include "ospfd/ospf_lsa.h"
#include "ospfd/ospf_lsdb.h"
#include "ospfd/ospf_ism.h"
#include "ospfd/ospf_interface.h"
#include "ospfd/ospf_nsm.h"
#include "ospfd/ospf_neighbor.h"
#include "ospfd/ospf_flood.h"
#include "ospfd/ospf_abr.h"
#include "ospfd/ospf_spf.h"
#include "ospfd/ospf_route.h"
#include "ospfd/ospf_zebra.h"
/*#include "ospfd/ospf_routemap.h" */
#include "ospfd/ospf_vty.h"
#include "ospfd/ospf_dump.h"
#include "ospfd/ospf_bfd.h"

/* modified by liwh for bug 44726, 2017-07-19 */
#ifdef _CENTEC_
static void ospf_interface_clear (struct interface *ifp);
#endif
/*liwh end */

/* modified by liwh for bug 43976, 2017-08-07 */
#define GLB_OSPF_DEAD_INTERVAL_MULTI_HELLO     4
/* liwh end */

/*modified by chenc for bug 45485*/
#ifdef _CENTEC_
static const char *ospf_network_type_str[] =
{
  "Null",
  "POINT_TO_POINT",
  "BROADCAST",
  "NBMA",
  "POINTOMULTIPOINT",
  "VIRTUALLINK",
  "LOOPBACK"
};
#else
static const char *ospf_network_type_str[] =
{
  "Null",
  "POINTOPOINT",
  "BROADCAST",
  "NBMA",
  "POINTOMULTIPOINT",
  "VIRTUALLINK",
  "LOOPBACK"
};
#endif

/* Utility functions. */
int str2area_id(const char *str, struct in_addr *area_id, int *area_id_fmt)
{
	char *ep;

	area_id->s_addr = htonl(strtoul(str, &ep, 10));
	if (*ep && !inet_aton(str, area_id))
		return -1;

	*area_id_fmt =
		*ep ? OSPF_AREA_ID_FMT_DOTTEDQUAD : OSPF_AREA_ID_FMT_DECIMAL;

	return 0;
}

void area_id2str(char *buf, int length, struct in_addr *area_id,
		 int area_id_fmt)
{
	memset(buf, 0, length);

	if (area_id_fmt == OSPF_AREA_ID_FMT_DOTTEDQUAD)
		strncpy(buf, inet_ntoa(*area_id), length);
	else
		sprintf(buf, "%lu", (unsigned long)ntohl(area_id->s_addr));
}

static int str2metric(const char *str, int *metric)
{
	/* Sanity check. */
	if (str == NULL)
		return 0;

	*metric = strtol(str, NULL, 10);
	if (*metric < 0 && *metric > 16777214) {
		/* vty_out (vty, "OSPF metric value is invalid%s", VTY_NEWLINE);
		 */
		return 0;
	}

	return 1;
}

static int str2metric_type(const char *str, int *metric_type)
{
	/* Sanity check. */
	if (str == NULL)
		return 0;

	if (strncmp(str, "1", 1) == 0)
		*metric_type = EXTERNAL_METRIC_TYPE_1;
	else if (strncmp(str, "2", 1) == 0)
		*metric_type = EXTERNAL_METRIC_TYPE_2;
	else
		return 0;

	return 1;
}

int ospf_oi_count(struct interface *ifp)
{
	struct route_node *rn;
	int i = 0;

	for (rn = route_top(IF_OIFS(ifp)); rn; rn = route_next(rn))
		if (rn->info)
			i++;

	return i;
}

DEFUN_NOSH (router_ospf,
       router_ospf_cmd,
       "router ospf [(1-65535)]",
       "Enable a routing process\n"
       "Start OSPF configuration\n"
       "Instance ID\n")
{
	struct ospf *ospf;
	u_short instance = 0;

	ospf = ospf_lookup();
	if (!ospf) {
		vty_out(vty, "There isn't active ospf instance %s",
			VTY_NEWLINE);
		return CMD_WARNING;
	}

	if (argc > 2)
		VTY_GET_INTEGER("Instance", instance, argv[2]->arg);

	/* The following logic to set the vty qobj index is in place to be able
	   to ignore the commands which dont belong to this instance. */
	if (ospf->instance != instance)
		VTY_PUSH_CONTEXT_NULL(OSPF_NODE);
	else {
		if (IS_DEBUG_OSPF_EVENT)
			zlog_debug("Config command 'router ospf %d' received",
				   instance);
		ospf->oi_running = 1;
		VTY_PUSH_CONTEXT(OSPF_NODE, ospf);
/* modified by liwh for bug 44575, 2017-07-05 */
#ifdef _CENTEC_      
      if (0 == ospf->router_id.s_addr)
        {
#endif
/*liwh end */
/* modified by liwh for bug 44650, 2017-07-14 */
#ifdef _CENTEC_
          //ospf_router_id_update (ospf);
          ospf_router_id_update (ospf, FALSE);
#endif
/*liwh end */
/* modified by liwh for bug 44575, 2017-07-05 */
#ifdef _CENTEC_      
        }
#endif
/*liwh end */      
    }
 
	return CMD_SUCCESS;
}

DEFUN (no_router_ospf,
       no_router_ospf_cmd,
       "no router ospf [(1-65535)]",
       NO_STR
       "Enable a routing process\n"
       "Start OSPF configuration\n"
       "Instance ID\n")
{
	struct ospf *ospf;
	u_short instance = 0;

	if (argc > 3)
		VTY_GET_INTEGER("Instance", instance, argv[3]->arg);

	if ((ospf = ospf_lookup_instance(instance)) == NULL)
		return CMD_SUCCESS;

	ospf_finish(ospf);

	return CMD_SUCCESS;
}


DEFUN (ospf_router_id,
       ospf_router_id_cmd,
       "ospf router-id A.B.C.D",
       "OSPF specific commands\n"
       "router-id for the OSPF process\n"
       "OSPF router-id in IP address format\n")
{
	VTY_DECLVAR_CONTEXT(ospf, ospf);
	int idx_ipv4 = 2;
	struct listnode *node;
	struct ospf_area *area;
	struct in_addr router_id;
	int ret;
/* modified by liwh for bug 44726, 2017-07-19 */
#ifdef _CENTEC_
  struct interface *ifp = NULL;
#endif
/*liwh end */

	ret = inet_aton(argv[idx_ipv4]->arg, &router_id);
	if (!ret) {
		vty_out(vty, "Please specify Router ID by A.B.C.D%s",
			VTY_NEWLINE);
		return CMD_WARNING;
	}

	ospf->router_id_static = router_id;

	for (ALL_LIST_ELEMENTS_RO(ospf->areas, node, area))
		if (area->full_nbrs) {
/* modified by liwh for bug 44589, 2017-07-06 */
#ifdef _CENTEC_
        vty_out (vty, "Ospf router-id %s can't take effect until clear ip ospf process",
                argv[0]->arg);
#else
        vty_out (vty, "For this router-id change to take effect,"
                 " save config and restart ospfd%s", VTY_NEWLINE);
#endif
/*liwh end */
			return CMD_SUCCESS;
		}
/* modified by liwh for bug 44650, 2017-07-14 */
#ifdef _CENTEC_
  //ospf_router_id_update (ospf);

  /* modified by liwh for bug 44726, 2017-07-19 */
  for (ALL_LIST_ELEMENTS_RO (vrf_iflist (VRF_DEFAULT), node, ifp))
          ospf_interface_clear(ifp);
  /*liwh end */

  ospf_router_id_update (ospf, FALSE);
#endif
/*liwh end */

	return CMD_SUCCESS;
}

DEFUN_HIDDEN (ospf_router_id_old,
              ospf_router_id_old_cmd,
              "router-id A.B.C.D",
              "router-id for the OSPF process\n"
              "OSPF router-id in IP address format\n")
{
	VTY_DECLVAR_CONTEXT(ospf, ospf);
	int idx_ipv4 = 1;
	struct listnode *node;
	struct ospf_area *area;
	struct in_addr router_id;
	int ret;
/* modified by liwh for bug 44726, 2017-07-19 */
#ifdef _CENTEC_
  struct interface *ifp = NULL;
#endif
/*liwh end */

	ret = inet_aton(argv[idx_ipv4]->arg, &router_id);
	if (!ret) {
		vty_out(vty, "Please specify Router ID by A.B.C.D%s",
			VTY_NEWLINE);
		return CMD_WARNING;
	}

	ospf->router_id_static = router_id;

	for (ALL_LIST_ELEMENTS_RO(ospf->areas, node, area))
		if (area->full_nbrs) {
/* modified by liwh for bug 44589, 2017-07-10 */
#ifdef _CENTEC_
        /* modified by liwh for bug 47215, 2018-09-11 */
        //vty_out (vty, "No ospf router-id can't take effect until clear ip ospf process");
        /* liwh end */
#else
        vty_out (vty, "For this router-id change to take effect,"
                 " save config and restart ospfd%s", VTY_NEWLINE);
#endif
/*liwh end */

/* modified by liwh for bug 47215, 2018-09-11 */
#ifdef _CENTEC_
                      return CMD_WARNING;
#else
			return CMD_SUCCESS;
#endif
/* liwh end */
		}

    /* modified by liwh for bug 44650, 2017-07-14 */
#ifdef _CENTEC_
      //ospf_router_id_update (ospf);
    
      /* modified by liwh for bug 44726, 2017-07-19 */
      for (ALL_LIST_ELEMENTS_RO (vrf_iflist (VRF_DEFAULT), node, ifp))
              ospf_interface_clear(ifp);
      /*liwh end */
    
      ospf_router_id_update (ospf, FALSE);
#endif
    /*liwh end */


	return CMD_SUCCESS;
}

DEFUN (no_ospf_router_id,
       no_ospf_router_id_cmd,
       "no ospf router-id [A.B.C.D]",
       NO_STR
       "OSPF specific commands\n"
       "router-id for the OSPF process\n"
       "OSPF router-id in IP address format\n")
{
	VTY_DECLVAR_CONTEXT(ospf, ospf);
	struct listnode *node;
	struct ospf_area *area;
/* modified by liwh for bug 44726, 2017-07-19 */
#ifdef _CENTEC_
    struct interface *ifp = NULL;
#endif
/*liwh end */

	ospf->router_id_static.s_addr = 0;

	for (ALL_LIST_ELEMENTS_RO(ospf->areas, node, area))
		if (area->full_nbrs) {
			vty_out(vty,
				"For this router-id change to take effect,"
				" save config and restart ospfd%s",
				VTY_NEWLINE);
			return CMD_SUCCESS;
		}

/* modified by liwh for bug 44650, 2017-07-14 */
#ifdef _CENTEC_
  //ospf_router_id_update (ospf);

  /* modified by liwh for bug 44726, 2017-07-19 */
  for (ALL_LIST_ELEMENTS_RO (vrf_iflist (VRF_DEFAULT), node, ifp))
          ospf_interface_clear(ifp);
  /* liwh end */

  ospf_router_id_update (ospf, FALSE);
#endif
/*liwh end */

	return CMD_SUCCESS;
}


static void ospf_passive_interface_default(struct ospf *ospf, u_char newval)
{
	struct listnode *ln;
	struct interface *ifp;
	struct ospf_interface *oi;

	ospf->passive_interface_default = newval;

	for (ALL_LIST_ELEMENTS_RO(om->iflist, ln, ifp)) {
		if (ifp && OSPF_IF_PARAM_CONFIGURED(IF_DEF_PARAMS(ifp),
						    passive_interface))
			UNSET_IF_PARAM(IF_DEF_PARAMS(ifp), passive_interface);
	}
	for (ALL_LIST_ELEMENTS_RO(ospf->oiflist, ln, oi)) {
		if (OSPF_IF_PARAM_CONFIGURED(oi->params, passive_interface))
			UNSET_IF_PARAM(oi->params, passive_interface);
		/* update multicast memberships */
		ospf_if_set_multicast(oi);
	}
}

static void ospf_passive_interface_update_addr(struct ospf *ospf,
					       struct interface *ifp,
					       struct ospf_if_params *params,
					       u_char value,
					       struct in_addr addr)
{
	u_char dflt;

	params->passive_interface = value;
	if (params != IF_DEF_PARAMS(ifp)) {
		if (OSPF_IF_PARAM_CONFIGURED(IF_DEF_PARAMS(ifp),
					     passive_interface))
			dflt = IF_DEF_PARAMS(ifp)->passive_interface;
		else
			dflt = ospf->passive_interface_default;

		if (value != dflt)
			SET_IF_PARAM(params, passive_interface);
		else
			UNSET_IF_PARAM(params, passive_interface);

		ospf_free_if_params(ifp, addr);
		ospf_if_update_params(ifp, addr);
	}
}

static void ospf_passive_interface_update(struct ospf *ospf,
					  struct interface *ifp,
					  struct ospf_if_params *params,
					  u_char value)
{
	params->passive_interface = value;
	if (params == IF_DEF_PARAMS(ifp)) {
		if (value != ospf->passive_interface_default)
			SET_IF_PARAM(params, passive_interface);
		else
			UNSET_IF_PARAM(params, passive_interface);
	}
}

DEFUN (ospf_passive_interface,
       ospf_passive_interface_addr_cmd,
       "passive-interface <IFNAME [A.B.C.D]|default>",
       "Suppress routing updates on an interface\n"
       "Interface's name\n"
       "IPv4 address\n"
       "Suppress routing updates on interfaces by default\n")
{
	VTY_DECLVAR_CONTEXT(ospf, ospf);
	int idx_ipv4 = 2;
	struct interface *ifp;
	struct in_addr addr = {.s_addr = INADDR_ANY};
	int ret;
	struct ospf_if_params *params;
	struct route_node *rn;

	if (strcmp(argv[1]->text, "default") == 0) {
		ospf_passive_interface_default(ospf, OSPF_IF_PASSIVE);
		return CMD_SUCCESS;
	}

	ifp = if_get_by_name(argv[1]->arg, VRF_DEFAULT);

	params = IF_DEF_PARAMS(ifp);

	if (argc == 3) {
		ret = inet_aton(argv[idx_ipv4]->arg, &addr);
		if (!ret) {
			vty_out(vty,
				"Please specify interface address by A.B.C.D%s",
				VTY_NEWLINE);
			return CMD_WARNING;
		}

		params = ospf_get_if_params(ifp, addr);
		ospf_if_update_params(ifp, addr);
		ospf_passive_interface_update_addr(ospf, ifp, params,
						   OSPF_IF_PASSIVE, addr);
	}

	ospf_passive_interface_update(ospf, ifp, params, OSPF_IF_PASSIVE);

	/* XXX We should call ospf_if_set_multicast on exactly those
	 * interfaces for which the passive property changed.  It is too much
	 * work to determine this set, so we do this for every interface.
	 * This is safe and reasonable because ospf_if_set_multicast uses a
	 * record of joined groups to avoid systems calls if the desired
	 * memberships match the current memership.
	 */

	for (rn = route_top(IF_OIFS(ifp)); rn; rn = route_next(rn)) {
		struct ospf_interface *oi = rn->info;

		if (oi && (OSPF_IF_PARAM(oi, passive_interface)
			   == OSPF_IF_PASSIVE))
			ospf_if_set_multicast(oi);
	}
	/*
	 * XXX It is not clear what state transitions the interface needs to
	 * undergo when going from active to passive.  Fixing this will
	 * require precise identification of interfaces having such a
	 * transition.
	 */

	return CMD_SUCCESS;
}

DEFUN (no_ospf_passive_interface,
       no_ospf_passive_interface_addr_cmd,
       "no passive-interface <IFNAME [A.B.C.D]|default>",
       NO_STR
       "Allow routing updates on an interface\n"
       "Interface's name\n"
       "IPv4 address\n"
       "Allow routing updates on interfaces by default\n")
{
	VTY_DECLVAR_CONTEXT(ospf, ospf);
	int idx_ipv4 = 3;
	struct interface *ifp;
	struct in_addr addr = {.s_addr = INADDR_ANY};
	struct ospf_if_params *params;
	int ret;
	struct route_node *rn;

	if (strcmp(argv[2]->text, "default") == 0) {
		ospf_passive_interface_default(ospf, OSPF_IF_ACTIVE);
		return CMD_SUCCESS;
	}

	ifp = if_get_by_name(argv[2]->arg, VRF_DEFAULT);

	params = IF_DEF_PARAMS(ifp);

	if (argc == 4) {
		ret = inet_aton(argv[idx_ipv4]->arg, &addr);
		if (!ret) {
			vty_out(vty,
				"Please specify interface address by A.B.C.D%s",
				VTY_NEWLINE);
			return CMD_WARNING;
		}

		params = ospf_lookup_if_params(ifp, addr);
		if (params == NULL)
			return CMD_SUCCESS;
		ospf_passive_interface_update_addr(ospf, ifp, params,
						   OSPF_IF_ACTIVE, addr);
	}
	ospf_passive_interface_update(ospf, ifp, params, OSPF_IF_ACTIVE);

	/* XXX We should call ospf_if_set_multicast on exactly those
	 * interfaces for which the passive property changed.  It is too much
	 * work to determine this set, so we do this for every interface.
	 * This is safe and reasonable because ospf_if_set_multicast uses a
	 * record of joined groups to avoid systems calls if the desired
	 * memberships match the current memership.
	 */
	for (rn = route_top(IF_OIFS(ifp)); rn; rn = route_next(rn)) {
		struct ospf_interface *oi = rn->info;

		if (oi
		    && (OSPF_IF_PARAM(oi, passive_interface) == OSPF_IF_ACTIVE))
			ospf_if_set_multicast(oi);
	}

	return CMD_SUCCESS;
}


DEFUN (ospf_network_area,
       ospf_network_area_cmd,
       "network A.B.C.D/M area <A.B.C.D|(0-4294967295)>",
       "Enable routing on an IP network\n"
       "OSPF network prefix\n"
       "Set the OSPF area ID\n"
       "OSPF area ID in IP address format\n"
       "OSPF area ID as a decimal value\n")
{
	VTY_DECLVAR_CONTEXT(ospf, ospf);
	int idx_ipv4_prefixlen = 1;
	int idx_ipv4_number = 3;
	struct prefix_ipv4 p;
	struct in_addr area_id;
	int ret, format;

	if (ospf->instance) {
		vty_out(vty,
			"The network command is not supported in multi-instance ospf%s",
			VTY_NEWLINE);
		return CMD_WARNING;
	}

	if (ospf->if_ospf_cli_count > 0) {
		vty_out(vty,
			"Please remove all ip ospf area x.x.x.x commands first.%s",
			VTY_NEWLINE);
		return CMD_WARNING;
	}

	/* Get network prefix and Area ID. */
	VTY_GET_IPV4_PREFIX("network prefix", p, argv[idx_ipv4_prefixlen]->arg);
	VTY_GET_OSPF_AREA_ID(area_id, format, argv[idx_ipv4_number]->arg);

	ret = ospf_network_set(ospf, &p, area_id, format);
	if (ret == 0) {
		vty_out(vty, "There is already same network statement.%s",
			VTY_NEWLINE);
		return CMD_WARNING;
	}

	return CMD_SUCCESS;
}

DEFUN (no_ospf_network_area,
       no_ospf_network_area_cmd,
       "no network A.B.C.D/M area <A.B.C.D|(0-4294967295)>",
       NO_STR
       "Enable routing on an IP network\n"
       "OSPF network prefix\n"
       "Set the OSPF area ID\n"
       "OSPF area ID in IP address format\n"
       "OSPF area ID as a decimal value\n")
{
	VTY_DECLVAR_CONTEXT(ospf, ospf);
	int idx_ipv4_prefixlen = 2;
	int idx_ipv4_number = 4;
	struct prefix_ipv4 p;
	struct in_addr area_id;
	int ret, format;

	if (ospf->instance) {
		vty_out(vty,
			"The network command is not supported in multi-instance ospf%s",
			VTY_NEWLINE);
		return CMD_WARNING;
	}

	/* Get network prefix and Area ID. */
	VTY_GET_IPV4_PREFIX("network prefix", p, argv[idx_ipv4_prefixlen]->arg);
	VTY_GET_OSPF_AREA_ID(area_id, format, argv[idx_ipv4_number]->arg);

	ret = ospf_network_unset(ospf, &p, area_id);
	if (ret == 0) {
		vty_out(vty,
			"Can't find specified network area configuration.%s",
			VTY_NEWLINE);
		return CMD_WARNING;
	}

	return CMD_SUCCESS;
}

DEFUN (ospf_area_range,
       ospf_area_range_cmd,
       "area <A.B.C.D|(0-4294967295)> range A.B.C.D/M [advertise [cost (0-16777215)]]",
       "OSPF area parameters\n"
       "OSPF area ID in IP address format\n"
       "OSPF area ID as a decimal value\n"
       "Summarize routes matching address/mask (border routers only)\n"
       "Area range prefix\n"
       "Advertise this range (default)\n"
       "User specified metric for this range\n"
       "Advertised metric for this range\n")
{
	VTY_DECLVAR_CONTEXT(ospf, ospf);
	int idx_ipv4_number = 1;
	int idx_ipv4_prefixlen = 3;
	int idx_cost = 6;
	struct prefix_ipv4 p;
	struct in_addr area_id;
	int format;
	u_int32_t cost;

	VTY_GET_OSPF_AREA_ID(area_id, format, argv[idx_ipv4_number]->arg);
	VTY_GET_IPV4_PREFIX("area range", p, argv[idx_ipv4_prefixlen]->arg);

	ospf_area_range_set(ospf, area_id, &p, OSPF_AREA_RANGE_ADVERTISE);
	if (argc > 5) {
		VTY_GET_INTEGER("range cost", cost, argv[idx_cost]->arg);
		ospf_area_range_cost_set(ospf, area_id, &p, cost);
	}

	return CMD_SUCCESS;
}

DEFUN (ospf_area_range_cost,
       ospf_area_range_cost_cmd,
       "area <A.B.C.D|(0-4294967295)> range A.B.C.D/M cost (0-16777215)",
       "OSPF area parameters\n"
       "OSPF area ID in IP address format\n"
       "OSPF area ID as a decimal value\n"
       "Summarize routes matching address/mask (border routers only)\n"
       "Area range prefix\n"
       "User specified metric for this range\n"
       "Advertised metric for this range\n")
{
	VTY_DECLVAR_CONTEXT(ospf, ospf);
	int idx_ipv4_number = 1;
	int idx_ipv4_prefixlen = 3;
	int idx_cost = 5;
	struct prefix_ipv4 p;
	struct in_addr area_id;
	int format;
	u_int32_t cost;

	VTY_GET_OSPF_AREA_ID(area_id, format, argv[idx_ipv4_number]->arg);
	VTY_GET_IPV4_PREFIX("area range", p, argv[idx_ipv4_prefixlen]->arg);

	ospf_area_range_set(ospf, area_id, &p, OSPF_AREA_RANGE_ADVERTISE);
	ospf_area_display_format_set(ospf, ospf_area_get(ospf, area_id),
				     format);

	VTY_GET_INTEGER("range cost", cost, argv[idx_cost]->arg);
	ospf_area_range_cost_set(ospf, area_id, &p, cost);

	return CMD_SUCCESS;
}

DEFUN (ospf_area_range_not_advertise,
       ospf_area_range_not_advertise_cmd,
       "area <A.B.C.D|(0-4294967295)> range A.B.C.D/M not-advertise",
       "OSPF area parameters\n"
       "OSPF area ID in IP address format\n"
       "OSPF area ID as a decimal value\n"
       "Summarize routes matching address/mask (border routers only)\n"
       "Area range prefix\n"
       "DoNotAdvertise this range\n")
{
	VTY_DECLVAR_CONTEXT(ospf, ospf);
	int idx_ipv4_number = 1;
	int idx_ipv4_prefixlen = 3;
	struct prefix_ipv4 p;
	struct in_addr area_id;
	int format;

	VTY_GET_OSPF_AREA_ID(area_id, format, argv[idx_ipv4_number]->arg);
	VTY_GET_IPV4_PREFIX("area range", p, argv[idx_ipv4_prefixlen]->arg);

	ospf_area_range_set(ospf, area_id, &p, 0);
	ospf_area_display_format_set(ospf, ospf_area_get(ospf, area_id),
				     format);

	return CMD_SUCCESS;
}

DEFUN (no_ospf_area_range,
       no_ospf_area_range_cmd,
       "no area <A.B.C.D|(0-4294967295)> range A.B.C.D/M [<cost (0-16777215)|advertise [cost (0-16777215)]|not-advertise>]",
       NO_STR
       "OSPF area parameters\n"
       "OSPF area ID in IP address format\n"
       "OSPF area ID as a decimal value\n"
       "Summarize routes matching address/mask (border routers only)\n"
       "Area range prefix\n"
       "User specified metric for this range\n"
       "Advertised metric for this range\n"
       "Advertise this range (default)\n"
       "User specified metric for this range\n"
       "Advertised metric for this range\n"
       "DoNotAdvertise this range\n")
{
	VTY_DECLVAR_CONTEXT(ospf, ospf);
	int idx_ipv4_number = 2;
	int idx_ipv4_prefixlen = 4;
	struct prefix_ipv4 p;
	struct in_addr area_id;
	int format;

	VTY_GET_OSPF_AREA_ID(area_id, format, argv[idx_ipv4_number]->arg);
	VTY_GET_IPV4_PREFIX("area range", p, argv[idx_ipv4_prefixlen]->arg);

	ospf_area_range_unset(ospf, area_id, &p);

	return CMD_SUCCESS;
}

DEFUN (ospf_area_range_substitute,
       ospf_area_range_substitute_cmd,
       "area <A.B.C.D|(0-4294967295)> range A.B.C.D/M substitute A.B.C.D/M",
       "OSPF area parameters\n"
       "OSPF area ID in IP address format\n"
       "OSPF area ID as a decimal value\n"
       "Summarize routes matching address/mask (border routers only)\n"
       "Area range prefix\n"
       "Announce area range as another prefix\n"
       "Network prefix to be announced instead of range\n")
{
	VTY_DECLVAR_CONTEXT(ospf, ospf);
	int idx_ipv4_number = 1;
	int idx_ipv4_prefixlen = 3;
	int idx_ipv4_prefixlen_2 = 5;
	struct prefix_ipv4 p, s;
	struct in_addr area_id;
	int format;

	VTY_GET_OSPF_AREA_ID(area_id, format, argv[idx_ipv4_number]->arg);
	VTY_GET_IPV4_PREFIX("area range", p, argv[idx_ipv4_prefixlen]->arg);
	VTY_GET_IPV4_PREFIX("substituted network prefix", s,
			    argv[idx_ipv4_prefixlen_2]->arg);

	ospf_area_range_substitute_set(ospf, area_id, &p, &s);
	ospf_area_display_format_set(ospf, ospf_area_get(ospf, area_id),
				     format);

	return CMD_SUCCESS;
}

DEFUN (no_ospf_area_range_substitute,
       no_ospf_area_range_substitute_cmd,
       "no area <A.B.C.D|(0-4294967295)> range A.B.C.D/M substitute A.B.C.D/M",
       NO_STR
       "OSPF area parameters\n"
       "OSPF area ID in IP address format\n"
       "OSPF area ID as a decimal value\n"
       "Summarize routes matching address/mask (border routers only)\n"
       "Area range prefix\n"
       "Announce area range as another prefix\n"
       "Network prefix to be announced instead of range\n")
{
	VTY_DECLVAR_CONTEXT(ospf, ospf);
	int idx_ipv4_number = 2;
	int idx_ipv4_prefixlen = 4;
	int idx_ipv4_prefixlen_2 = 6;
	struct prefix_ipv4 p, s;
	struct in_addr area_id;
	int format;

	VTY_GET_OSPF_AREA_ID(area_id, format, argv[idx_ipv4_number]->arg);
	VTY_GET_IPV4_PREFIX("area range", p, argv[idx_ipv4_prefixlen]->arg);
	VTY_GET_IPV4_PREFIX("substituted network prefix", s,
			    argv[idx_ipv4_prefixlen_2]->arg);

	ospf_area_range_substitute_unset(ospf, area_id, &p);

	return CMD_SUCCESS;
}


/* Command Handler Logic in VLink stuff is delicate!!

	ALTER AT YOUR OWN RISK!!!!

	Various dummy values are used to represent 'NoChange' state for
	VLink configuration NOT being changed by a VLink command, and
	special syntax is used within the command strings so that the
	typed in command verbs can be seen in the configuration command
	bacckend handler.  This is to drastically reduce the verbeage
	required to coe up with a reasonably compatible Cisco VLink command

	- Matthew Grant <grantma@anathoth.gen.nz>
	Wed, 21 Feb 2001 15:13:52 +1300
 */

/* Configuration data for virtual links
 */
struct ospf_vl_config_data {
	struct vty *vty;	/* vty stuff */
	struct in_addr area_id; /* area ID from command line */
	int area_id_fmt;	/* command line area ID format */
	struct in_addr vl_peer; /* command line vl_peer */
	int auth_type;		/* Authehntication type, if given */
	char *auth_key;		/* simple password if present */
	int crypto_key_id;      /* Cryptographic key ID */
	char *md5_key;		/* MD5 authentication key */
	int hello_interval;     /* Obvious what these are... */
	int retransmit_interval;
	int transmit_delay;
	int dead_interval;
};

static void ospf_vl_config_data_init(struct ospf_vl_config_data *vl_config,
				     struct vty *vty)
{
	memset(vl_config, 0, sizeof(struct ospf_vl_config_data));
	vl_config->auth_type = OSPF_AUTH_CMD_NOTSEEN;
	vl_config->vty = vty;
}

static struct ospf_vl_data *
ospf_find_vl_data(struct ospf *ospf, struct ospf_vl_config_data *vl_config)
{
	struct ospf_area *area;
	struct ospf_vl_data *vl_data;
	struct vty *vty;
	struct in_addr area_id;

	vty = vl_config->vty;
	area_id = vl_config->area_id;

	if (area_id.s_addr == OSPF_AREA_BACKBONE) {
		vty_out(vty,
			"Configuring VLs over the backbone is not allowed%s",
			VTY_NEWLINE);
		return NULL;
	}
	area = ospf_area_get(ospf, area_id);
	ospf_area_display_format_set(ospf, area, vl_config->area_id_fmt);

	if (area->external_routing != OSPF_AREA_DEFAULT) {
		if (vl_config->area_id_fmt == OSPF_AREA_ID_FMT_DOTTEDQUAD)
			vty_out(vty, "Area %s is %s%s", inet_ntoa(area_id),
				area->external_routing == OSPF_AREA_NSSA
					? "nssa"
					: "stub",
				VTY_NEWLINE);
		else
			vty_out(vty, "Area %ld is %s%s",
				(u_long)ntohl(area_id.s_addr),
				area->external_routing == OSPF_AREA_NSSA
					? "nssa"
					: "stub",
				VTY_NEWLINE);
		return NULL;
	}

	if ((vl_data = ospf_vl_lookup(ospf, area, vl_config->vl_peer))
	    == NULL) {
		vl_data = ospf_vl_data_new(area, vl_config->vl_peer);
		if (vl_data->vl_oi == NULL) {
			vl_data->vl_oi = ospf_vl_new(ospf, vl_data);
			ospf_vl_add(ospf, vl_data);
			ospf_spf_calculate_schedule(ospf,
						    SPF_FLAG_CONFIG_CHANGE);
		}
	}
	return vl_data;
}


static int ospf_vl_set_security(struct ospf_vl_data *vl_data,
				struct ospf_vl_config_data *vl_config)
{
	struct crypt_key *ck;
	struct vty *vty;
	struct interface *ifp = vl_data->vl_oi->ifp;

	vty = vl_config->vty;

	if (vl_config->auth_type != OSPF_AUTH_CMD_NOTSEEN) {
		SET_IF_PARAM(IF_DEF_PARAMS(ifp), auth_type);
		IF_DEF_PARAMS(ifp)->auth_type = vl_config->auth_type;
	}

	if (vl_config->auth_key) {
		memset(IF_DEF_PARAMS(ifp)->auth_simple, 0,
		       OSPF_AUTH_SIMPLE_SIZE + 1);
		strncpy((char *)IF_DEF_PARAMS(ifp)->auth_simple,
			vl_config->auth_key, OSPF_AUTH_SIMPLE_SIZE);
		SET_IF_PARAM(IF_DEF_PARAMS(ifp), auth_simple); /* djd */
	} else if (vl_config->md5_key) {
		if (ospf_crypt_key_lookup(IF_DEF_PARAMS(ifp)->auth_crypt,
					  vl_config->crypto_key_id)
		    != NULL) {
			vty_out(vty, "OSPF: Key %d already exists%s",
				vl_config->crypto_key_id, VTY_NEWLINE);
			return CMD_WARNING_CONFIG_FAILED;
		}
		ck = ospf_crypt_key_new();
		ck->key_id = vl_config->crypto_key_id;
		memset(ck->auth_key, 0, OSPF_AUTH_MD5_SIZE + 1);
		strncpy((char *)ck->auth_key, vl_config->md5_key,
			OSPF_AUTH_MD5_SIZE);

		ospf_crypt_key_add(IF_DEF_PARAMS(ifp)->auth_crypt, ck);
		SET_IF_PARAM(IF_DEF_PARAMS(ifp), auth_crypt); /* djd */
	} else if (vl_config->crypto_key_id != 0) {
		/* Delete a key */

		if (ospf_crypt_key_lookup(IF_DEF_PARAMS(ifp)->auth_crypt,
					  vl_config->crypto_key_id)
		    == NULL) {
			vty_out(vty, "OSPF: Key %d does not exist%s",
				vl_config->crypto_key_id, VTY_NEWLINE);
			return CMD_WARNING_CONFIG_FAILED;
		}

		ospf_crypt_key_delete(IF_DEF_PARAMS(ifp)->auth_crypt,
				      vl_config->crypto_key_id);
	}

	return CMD_SUCCESS;
}

static int ospf_vl_set_timers(struct ospf_vl_data *vl_data,
			      struct ospf_vl_config_data *vl_config)
{
	struct interface *ifp = vl_data->vl_oi->ifp;
	/* Virtual Link data initialised to defaults, so only set
	   if a value given */
	if (vl_config->hello_interval) {
		SET_IF_PARAM(IF_DEF_PARAMS(ifp), v_hello);
		IF_DEF_PARAMS(ifp)->v_hello = vl_config->hello_interval;
	}

	if (vl_config->dead_interval) {
		SET_IF_PARAM(IF_DEF_PARAMS(ifp), v_wait);
		IF_DEF_PARAMS(ifp)->v_wait = vl_config->dead_interval;
	}

	if (vl_config->retransmit_interval) {
		SET_IF_PARAM(IF_DEF_PARAMS(ifp), retransmit_interval);
		IF_DEF_PARAMS(ifp)->retransmit_interval =
			vl_config->retransmit_interval;
	}

	if (vl_config->transmit_delay) {
		SET_IF_PARAM(IF_DEF_PARAMS(ifp), transmit_delay);
		IF_DEF_PARAMS(ifp)->transmit_delay = vl_config->transmit_delay;
	}

	return CMD_SUCCESS;
}


/* The business end of all of the above */
static int ospf_vl_set(struct ospf *ospf, struct ospf_vl_config_data *vl_config)
{
	struct ospf_vl_data *vl_data;
	int ret;

	vl_data = ospf_find_vl_data(ospf, vl_config);
	if (!vl_data)
		return CMD_WARNING_CONFIG_FAILED;

	/* Process this one first as it can have a fatal result, which can
	   only logically occur if the virtual link exists already
	   Thus a command error does not result in a change to the
	   running configuration such as unexpectedly altered timer
	   values etc.*/
	ret = ospf_vl_set_security(vl_data, vl_config);
	if (ret != CMD_SUCCESS)
		return ret;

	/* Set any time based parameters, these area already range checked */

	ret = ospf_vl_set_timers(vl_data, vl_config);
	if (ret != CMD_SUCCESS)
		return ret;

	return CMD_SUCCESS;
}

/* djd add to display virtual links information */
static int ospf_cli_show_vlink(struct vty *vty, struct ospf *ospf)
{
    struct ospf_vl_data *vl_data;
    struct listnode *node;
    struct ospf_interface *oi;
    struct interface *ifp;
    struct ospf_neighbor *nbr;
    char timer_str[OSPF_TIME_DUMP_SIZE];
    char buf[INET_ADDRSTRLEN];

    for(ALL_LIST_ELEMENTS_RO(ospf->vlinks, node, vl_data)) 
    {
        if(vl_data == NULL)
            continue;

        memset(buf, 0, INET_ADDRSTRLEN);
        area_id2str(buf, sizeof(buf), &vl_data->vl_area_id, vl_data->vl_area_id_fmt);
        
        oi = vl_data->vl_oi;
        ifp = oi->ifp;

        vty_out(vty, "Virtual link transit in area %s to router %s is %s%s", buf, inet_ntoa(vl_data->vl_peer), 
                 CHECK_FLAG(vl_data->flags, OSPF_VL_FLAG_APPROVED) ? "APPROVED/UP" : "UNAPPROVED/DOWN", VTY_NEWLINE);

        vty_out(vty, "  Interface %s, local address %s, remote address %s, state %s%s", IF_NAME(oi), inet_ntoa(oi->address->u.prefix4), 
                 inet_ntoa(vl_data->peer_addr), lookup_msg(ospf_ism_state_msg, oi->state, NULL), VTY_NEWLINE);
        
        vty_out(vty, "  Timer interval configured, Hello %d, Dead %d, Retransmit %d, Delay %d%s", 
                 IF_DEF_PARAMS(ifp)->v_hello, IF_DEF_PARAMS(ifp)->v_wait, 
                 IF_DEF_PARAMS(ifp)->retransmit_interval, IF_DEF_PARAMS(ifp)->transmit_delay, VTY_NEWLINE);

        vty_out(vty, "    Hello due in %s%s", ospf_timer_dump(oi->t_hello, timer_str, sizeof(timer_str)), VTY_NEWLINE);

        nbr = ospf_nbr_lookup_ptop(oi);
        if(nbr)
            vty_out(vty, "    Adjacency state %s%s", lookup_msg(ospf_nsm_state_msg, nbr->state, NULL), VTY_NEWLINE);
        else
            vty_out(vty, "    Adjacency state Down%s", VTY_NEWLINE);
        
        vty_out(vty, "%s", VTY_NEWLINE);
    }

    return CMD_SUCCESS;
}

/* This stuff exists to make specifying all the alias commands A LOT simpler
 */
#define VLINK_HELPSTR_IPADDR                                                   \
	"OSPF area parameters\n"                                               \
	"OSPF area ID in IP address format\n"                                  \
	"OSPF area ID as a decimal value\n"                                    \
	"Configure a virtual link\n"                                           \
	"Router ID of the remote ABR\n"

#define VLINK_HELPSTR_AUTHTYPE_SIMPLE                                          \
	"Enable authentication on this virtual link\n"                         \
	"dummy string \n"

#define VLINK_HELPSTR_AUTHTYPE_ALL                                             \
	VLINK_HELPSTR_AUTHTYPE_SIMPLE                                          \
	"Use null authentication\n"                                            \
	"Use message-digest authentication\n"

#define VLINK_HELPSTR_TIME_PARAM                                               \
	"Time between HELLO packets\n"                                         \
	"Seconds\n"                                                            \
	"Time between retransmitting lost link state advertisements\n"         \
	"Seconds\n"                                                            \
	"Link state transmit delay\n"                                          \
	"Seconds\n"                                                            \
	"Interval time after which a neighbor is declared down\n"              \
	"Seconds\n"

#define NO_VLINK_HELPSTR_TIME_PARAM                                            \
    "Time between HELLO packets\n"                                         \
    "Time between retransmitting lost link state advertisements\n"         \
    "Link state transmit delay\n"                                          \
    "Interval time after which a neighbor is declared down\n"

#define VLINK_HELPSTR_AUTH_SIMPLE                                              \
	"Authentication password (key)\n"                                      \
	"The OSPF password (key)"

#define VLINK_HELPSTR_AUTH_MD5                                                 \
	"Message digest authentication password (key)\n"                       \
	"dummy string \n"                                                      \
	"Key ID\n"                                                             \
	"Use MD5 algorithm\n"                                                  \
	"The OSPF password (key)"

DEFUN (ospf_area_vlink,
       ospf_area_vlink_cmd,
       "area <A.B.C.D|(0-4294967295)> virtual-link A.B.C.D [authentication] [<message-digest|null>] [<message-digest-key (1-255) md5 KEY|authentication-key AUTH_KEY>]",
       VLINK_HELPSTR_IPADDR
       "Enable authentication on this virtual link\n" \
       "Use message-digest authentication\n" \
       "Use null authentication\n" \
       "Use message-digest authentication\n"
       "Message digest authentication password (key)\n" \
       "Key ID\n" \
       "Use MD5 algorithm\n" \
       "The OSPF password (md5 key)\n" \
       "Simple authentication password (key)\n" \
       "The OSPF password (auth key)")
{
	VTY_DECLVAR_CONTEXT(ospf, ospf);
	int idx_ipv4_number = 1;
	int idx_ipv4 = 3;
	struct ospf_vl_config_data vl_config;
	char auth_key[OSPF_AUTH_SIMPLE_SIZE + 1];
	char md5_key[OSPF_AUTH_MD5_SIZE + 1];
	int i;
	int ret;
	int idx = 0;

	ospf_vl_config_data_init(&vl_config, vty);

	/* Read off first 2 parameters and check them */
	ret = str2area_id(argv[idx_ipv4_number]->arg, &vl_config.area_id,
			  &vl_config.area_id_fmt);
	if (ret < 0) {
		vty_out(vty, "OSPF area ID is invalid%s", VTY_NEWLINE);
		return CMD_WARNING_CONFIG_FAILED;
	}

	ret = inet_aton(argv[idx_ipv4]->arg, &vl_config.vl_peer);
	if (!ret) {
		vty_out(vty, "Please specify valid Router ID as a.b.c.d%s",
			VTY_NEWLINE);
		return CMD_WARNING_CONFIG_FAILED;
	}

	if (argc <= 4) {
		/* Thats all folks! - BUGS B. strikes again!!!*/

		return ospf_vl_set(ospf, &vl_config);
	}

	if (argv_find(argv, argc, "authentication", &idx)) {
		/* authentication  - this option can only occur
		at start of command line */
		vl_config.auth_type = OSPF_AUTH_SIMPLE;
	}

	if (argv_find(argv, argc, "message-digest", &idx)) {
		/* authentication message-digest */
		vl_config.auth_type = OSPF_AUTH_CRYPTOGRAPHIC;
	} else if (argv_find(argv, argc, "null", &idx)) {
		/* "authentication null" */
		vl_config.auth_type = OSPF_AUTH_NULL;
	}

	if (argv_find(argv, argc, "message-digest-key", &idx)) {
		vl_config.md5_key = NULL;
		vl_config.crypto_key_id = strtol(argv[idx + 1]->arg, NULL, 10);
		if (vl_config.crypto_key_id < 0)
			return CMD_WARNING_CONFIG_FAILED;

		memset(md5_key, 0, OSPF_AUTH_MD5_SIZE + 1);
		strncpy(md5_key, argv[idx + 3]->arg, OSPF_AUTH_MD5_SIZE);
		vl_config.md5_key = md5_key;
	}

	if (argv_find(argv, argc, "authentication-key", &idx)) {
		memset(auth_key, 0, OSPF_AUTH_SIMPLE_SIZE + 1);
		strncpy(auth_key, argv[idx + 1]->arg, OSPF_AUTH_SIMPLE_SIZE);
		vl_config.auth_key = auth_key;
	}	

	/* Action configuration */

	return ospf_vl_set(ospf, &vl_config);
}

DEFUN (no_ospf_area_vlink,
       no_ospf_area_vlink_cmd,
       "no area <A.B.C.D|(0-4294967295)> virtual-link A.B.C.D [authentication] [<message-digest|null>] [<message-digest-key (1-255)|authentication-key>]",
       NO_STR
       VLINK_HELPSTR_IPADDR
       "Enable authentication on this virtual link\n" \
       "Use message-digest authentication\n" \
       "Use null authentication\n" \
       "Message digest authentication password (key)\n" \
       "Key ID\n" \
       "Simple authentication password (key)")
{
	VTY_DECLVAR_CONTEXT(ospf, ospf);
	int idx_ipv4_number = 2;
	int idx_ipv4 = 4;
	struct ospf_area *area;
	struct ospf_vl_config_data vl_config;
	struct ospf_vl_data *vl_data = NULL;
	char auth_key[OSPF_AUTH_SIMPLE_SIZE + 1];
	int i;
	int ret, format;
	int idx = 0;

	ospf_vl_config_data_init(&vl_config, vty);

	ret = str2area_id(argv[idx_ipv4_number]->arg, &vl_config.area_id,
			  &format);
	if (ret < 0) {
		vty_out(vty, "OSPF area ID is invalid%s", VTY_NEWLINE);
		return CMD_WARNING_CONFIG_FAILED;
	}

	area = ospf_area_lookup_by_area_id(ospf, vl_config.area_id);
	if (!area) {
		vty_out(vty, "Area does not exist%s", VTY_NEWLINE);
		return CMD_WARNING_CONFIG_FAILED;
	}

	ret = inet_aton(argv[idx_ipv4]->arg, &vl_config.vl_peer);
	if (!ret) {
		vty_out(vty, "Please specify valid Router ID as a.b.c.d%s",
			VTY_NEWLINE);
		return CMD_WARNING_CONFIG_FAILED;
	}

	if (argc <= 5) {
		/* Basic VLink no command */
		/* Thats all folks! - BUGS B. strikes again!!!*/
		if ((vl_data = ospf_vl_lookup(ospf, area, vl_config.vl_peer)))
			ospf_vl_delete(ospf, vl_data);

		ospf_area_check_free(ospf, vl_config.area_id);

		return CMD_SUCCESS;
	}

	if (argv_find(argv, argc, "authentication", &idx)) {
		/* authentication  - this option can only occur
		at start of command line */
		vl_config.auth_type = OSPF_AUTH_NOTSET;
	}

	if (argv_find(argv, argc, "message-digest-key", &idx)) {
		vl_config.md5_key = NULL;
		vl_config.crypto_key_id = strtol(argv[idx + 1]->arg, NULL, 10);
		if (vl_config.crypto_key_id < 0)
			return CMD_WARNING_CONFIG_FAILED;
	}

	if (argv_find(argv, argc, "authentication-key", &idx)) {
		/* Reset authentication-key to 0 */
		memset(auth_key, 0, OSPF_AUTH_SIMPLE_SIZE + 1);
		vl_config.auth_key = auth_key;
	}

	/* Action configuration */

	return ospf_vl_set(ospf, &vl_config);
}

/* djd add for show ip ospf virtual-link */
DEFUN(show_ip_ospf_vlink,
        show_ip_ospf_vlink_cmd,
        "show ip ospf virtual-link",
        SHOW_STR
        IP_STR
        OSPF_STR
        "Show virtual links information")
{
    struct ospf *ospf;
            
    if((ospf = ospf_lookup()) == NULL || !ospf->oi_running)
        return CMD_SUCCESS;
                
    return ospf_cli_show_vlink(vty, ospf);
}
DEFUN (ospf_area_vlink_intervals,
       ospf_area_vlink_intervals_cmd,
       "area <A.B.C.D|(0-4294967295)> virtual-link A.B.C.D {hello-interval (1-65535)|retransmit-interval (1-65535)|transmit-delay (1-65535)|dead-interval (1-65535)}",
       VLINK_HELPSTR_IPADDR
       VLINK_HELPSTR_TIME_PARAM)
{
	VTY_DECLVAR_CONTEXT(ospf, ospf);
	struct ospf_vl_config_data vl_config;
	int ret = 0;

	ospf_vl_config_data_init(&vl_config, vty);

	char *area_id = argv[1]->arg;
	char *router_id = argv[3]->arg;

	ret = str2area_id(area_id, &vl_config.area_id, &vl_config.area_id_fmt);
	if (ret < 0) {
		vty_out(vty, "OSPF area ID is invalid%s", VTY_NEWLINE);
		return CMD_WARNING_CONFIG_FAILED;
	}

	ret = inet_aton(router_id, &vl_config.vl_peer);
	if (!ret) {
		vty_out(vty, "Please specify valid Router ID as a.b.c.d%s",
			VTY_NEWLINE);
		return CMD_WARNING_CONFIG_FAILED;
	}

	for (int idx = 4; idx < argc; idx++) {
		if (strmatch(argv[idx]->text, "hello-interval"))
			vl_config.hello_interval =
				strtol(argv[++idx]->arg, NULL, 10);
		else if (strmatch(argv[idx]->text, "retransmit-interval"))
			vl_config.retransmit_interval =
				strtol(argv[++idx]->arg, NULL, 10);
		else if (strmatch(argv[idx]->text, "transmit-delay"))
			vl_config.transmit_delay =
				strtol(argv[++idx]->arg, NULL, 10);
		else if (strmatch(argv[idx]->text, "dead-interval"))
			vl_config.dead_interval =
				strtol(argv[++idx]->arg, NULL, 10);
	}

	/* Action configuration */
	return ospf_vl_set(ospf, &vl_config);
}

DEFUN (no_ospf_area_vlink_intervals,
       no_ospf_area_vlink_intervals_cmd,
       "no area <A.B.C.D|(0-4294967295)> virtual-link A.B.C.D {hello-interval|retransmit-interval|transmit-delay|dead-interval}",
       NO_STR
       VLINK_HELPSTR_IPADDR
       NO_VLINK_HELPSTR_TIME_PARAM)
{
	VTY_DECLVAR_CONTEXT(ospf, ospf);
	struct ospf_vl_config_data vl_config;
	int ret = 0;

	ospf_vl_config_data_init(&vl_config, vty);

	char *area_id = argv[2]->arg;
	char *router_id = argv[4]->arg;

	ret = str2area_id(area_id, &vl_config.area_id, &vl_config.area_id_fmt);
	if (ret < 0) {
		vty_out(vty, "OSPF area ID is invalid%s", VTY_NEWLINE);
		return CMD_WARNING_CONFIG_FAILED;
	}

	ret = inet_aton(router_id, &vl_config.vl_peer);
	if (!ret) {
		vty_out(vty, "Please specify valid Router ID as a.b.c.d%s",
			VTY_NEWLINE);
		return CMD_WARNING_CONFIG_FAILED;
	}

	for (int idx = 5; idx < argc; idx++) {
		if (strmatch(argv[idx]->text, "hello-interval"))
			vl_config.hello_interval = OSPF_HELLO_INTERVAL_DEFAULT;
		else if (strmatch(argv[idx]->text, "retransmit-interval"))
			vl_config.retransmit_interval =
				OSPF_RETRANSMIT_INTERVAL_DEFAULT;
		else if (strmatch(argv[idx]->text, "transmit-delay"))
			vl_config.transmit_delay = OSPF_TRANSMIT_DELAY_DEFAULT;
		else if (strmatch(argv[idx]->text, "dead-interval"))
			vl_config.dead_interval =
				OSPF_ROUTER_DEAD_INTERVAL_DEFAULT;
	}

	/* Action configuration */
	return ospf_vl_set(ospf, &vl_config);
}

DEFUN (ospf_area_shortcut,
       ospf_area_shortcut_cmd,
       "area <A.B.C.D|(0-4294967295)> shortcut <default|enable|disable>",
       "OSPF area parameters\n"
       "OSPF area ID in IP address format\n"
       "OSPF area ID as a decimal value\n"
       "Configure the area's shortcutting mode\n"
       "Set default shortcutting behavior\n"
       "Enable shortcutting through the area\n"
       "Disable shortcutting through the area\n")
{
	VTY_DECLVAR_CONTEXT(ospf, ospf);
	int idx_ipv4_number = 1;
	int idx_enable_disable = 3;
	struct ospf_area *area;
	struct in_addr area_id;
	int mode;
	int format;

	VTY_GET_OSPF_AREA_ID_NO_BB("shortcut", area_id, format,
				   argv[idx_ipv4_number]->arg);

	area = ospf_area_get(ospf, area_id);
	ospf_area_display_format_set(ospf, area, format);

	if (strncmp(argv[idx_enable_disable]->arg, "de", 2) == 0)
		mode = OSPF_SHORTCUT_DEFAULT;
	else if (strncmp(argv[idx_enable_disable]->arg, "di", 2) == 0)
		mode = OSPF_SHORTCUT_DISABLE;
	else if (strncmp(argv[idx_enable_disable]->arg, "e", 1) == 0)
		mode = OSPF_SHORTCUT_ENABLE;
	else
		return CMD_WARNING;

	ospf_area_shortcut_set(ospf, area, mode);

	if (ospf->abr_type != OSPF_ABR_SHORTCUT)
		vty_out(vty,
			"Shortcut area setting will take effect "
			"only when the router is configured as Shortcut ABR%s",
			VTY_NEWLINE);

	return CMD_SUCCESS;
}

DEFUN (no_ospf_area_shortcut,
       no_ospf_area_shortcut_cmd,
       "no area <A.B.C.D|(0-4294967295)> shortcut <enable|disable>",
       NO_STR
       "OSPF area parameters\n"
       "OSPF area ID in IP address format\n"
       "OSPF area ID as a decimal value\n"
       "Deconfigure the area's shortcutting mode\n"
       "Deconfigure enabled shortcutting through the area\n"
       "Deconfigure disabled shortcutting through the area\n")
{
	VTY_DECLVAR_CONTEXT(ospf, ospf);
	int idx_ipv4_number = 2;
	struct ospf_area *area;
	struct in_addr area_id;
	int format;

	VTY_GET_OSPF_AREA_ID_NO_BB("shortcut", area_id, format,
				   argv[idx_ipv4_number]->arg);

	area = ospf_area_lookup_by_area_id(ospf, area_id);
	if (!area)
		return CMD_SUCCESS;

	ospf_area_shortcut_unset(ospf, area);

	return CMD_SUCCESS;
}


DEFUN (ospf_area_stub,
       ospf_area_stub_cmd,
       "area <A.B.C.D|(0-4294967295)> stub",
       "OSPF area parameters\n"
       "OSPF area ID in IP address format\n"
       "OSPF area ID as a decimal value\n"
       "Configure OSPF area as stub\n")
{
	VTY_DECLVAR_CONTEXT(ospf, ospf);
	int idx_ipv4_number = 1;
	struct in_addr area_id;
	int ret, format;

	VTY_GET_OSPF_AREA_ID_NO_BB("stub", area_id, format,
				   argv[idx_ipv4_number]->arg);

	ret = ospf_area_stub_set(ospf, area_id);
	ospf_area_display_format_set(ospf, ospf_area_get(ospf, area_id),
				     format);
	if (ret == 0) {
		vty_out(vty,
			"First deconfigure all virtual link through this area%s",
			VTY_NEWLINE);
		return CMD_WARNING;
	}

	ospf_area_no_summary_unset(ospf, area_id);

	return CMD_SUCCESS;
}

DEFUN (ospf_area_stub_no_summary,
       ospf_area_stub_no_summary_cmd,
       "area <A.B.C.D|(0-4294967295)> stub no-summary",
       "OSPF stub parameters\n"
       "OSPF area ID in IP address format\n"
       "OSPF area ID as a decimal value\n"
       "Configure OSPF area as stub\n"
       "Do not inject inter-area routes into stub\n")
{
	VTY_DECLVAR_CONTEXT(ospf, ospf);
	int idx_ipv4_number = 1;
	struct in_addr area_id;
	int ret, format;
	int areaAlreadyStub = 0; /* djd */

	VTY_GET_OSPF_AREA_ID_NO_BB("stub", area_id, format,
				   argv[idx_ipv4_number]->arg);

	areaAlreadyStub = ((ospf_area_get(ospf, area_id))->external_routing == OSPF_AREA_STUB);

	ret = ospf_area_stub_set(ospf, area_id);
	ospf_area_display_format_set(ospf, ospf_area_get(ospf, area_id),
				     format);
	if (ret == 0) {
		vty_out(vty,
			"%% Area cannot be stub as it contains a virtual link%s",
			VTY_NEWLINE);
		return CMD_WARNING_CONFIG_FAILED;
	}

	ospf_area_no_summary_set(ospf, area_id);
	
	/* djd: If area already stub, only modify no-summary flag, should do abr action again */
	if(areaAlreadyStub)
	    ospf_schedule_abr_task(ospf);

	return CMD_SUCCESS;
}

DEFUN (no_ospf_area_stub,
       no_ospf_area_stub_cmd,
       "no area <A.B.C.D|(0-4294967295)> stub",
       NO_STR
       "OSPF area parameters\n"
       "OSPF area ID in IP address format\n"
       "OSPF area ID as a decimal value\n"
       "Configure OSPF area as stub\n")
{
	VTY_DECLVAR_CONTEXT(ospf, ospf);
	int idx_ipv4_number = 2;
	struct in_addr area_id;
	int format;

	VTY_GET_OSPF_AREA_ID_NO_BB("stub", area_id, format,
				   argv[idx_ipv4_number]->arg);

	ospf_area_stub_unset(ospf, area_id);
	ospf_area_no_summary_unset(ospf, area_id);

	return CMD_SUCCESS;
}

DEFUN (no_ospf_area_stub_no_summary,
       no_ospf_area_stub_no_summary_cmd,
       "no area <A.B.C.D|(0-4294967295)> stub no-summary",
       NO_STR
       "OSPF area parameters\n"
       "OSPF area ID in IP address format\n"
       "OSPF area ID as a decimal value\n"
       "Configure OSPF area as stub\n"
       "Do not inject inter-area routes into area\n")
{
	VTY_DECLVAR_CONTEXT(ospf, ospf);
	int idx_ipv4_number = 2;
	struct in_addr area_id;
	int format;

	VTY_GET_OSPF_AREA_ID_NO_BB("stub", area_id, format,
				   argv[idx_ipv4_number]->arg);
	ospf_area_no_summary_unset(ospf, area_id);

	/* djd: Only modify no-summary flag, should do abr action again */
    ospf_schedule_abr_task(ospf);

	return CMD_SUCCESS;
}

static int ospf_area_nssa_cmd_handler(struct vty *vty, int argc,
				      struct cmd_token **argv, int nosum)
{
	VTY_DECLVAR_CONTEXT(ospf, ospf);
	struct in_addr area_id;
	int ret, format;

	VTY_GET_OSPF_AREA_ID_NO_BB("NSSA", area_id, format, argv[1]->arg);

	ret = ospf_area_nssa_set(ospf, area_id);
	ospf_area_display_format_set(ospf, ospf_area_get(ospf, area_id),
				     format);
	if (ret == 0) {
		vty_out(vty,
			"%% Area cannot be nssa as it contains a virtual link%s",
			VTY_NEWLINE);
		return CMD_WARNING;
	}

	if (argc > 3) {
		if (strncmp(argv[3]->text, "translate-c", 11) == 0)
			ospf_area_nssa_translator_role_set(
				ospf, area_id, OSPF_NSSA_ROLE_CANDIDATE);
		else if (strncmp(argv[3]->text, "translate-n", 11) == 0)
			ospf_area_nssa_translator_role_set(
				ospf, area_id, OSPF_NSSA_ROLE_NEVER);
		else if (strncmp(argv[3]->text, "translate-a", 11) == 0)
			ospf_area_nssa_translator_role_set(
				ospf, area_id, OSPF_NSSA_ROLE_ALWAYS);
	} else {
		ospf_area_nssa_translator_role_set(ospf, area_id,
						   OSPF_NSSA_ROLE_CANDIDATE);
	}

	if (nosum)
		ospf_area_no_summary_set(ospf, area_id);
	else
		ospf_area_no_summary_unset(ospf, area_id);

	ospf_schedule_abr_task(ospf);

	return CMD_SUCCESS;
}

DEFUN (ospf_area_nssa_translate_no_summary,
       ospf_area_nssa_translate_no_summary_cmd,
       "area <A.B.C.D|(0-4294967295)> nssa <translate-candidate|translate-never|translate-always> no-summary",
       "OSPF area parameters\n"
       "OSPF area ID in IP address format\n"
       "OSPF area ID as a decimal value\n"
       "Configure OSPF area as nssa\n"
       "Configure NSSA-ABR for translate election (default)\n"
       "Configure NSSA-ABR to never translate\n"
       "Configure NSSA-ABR to always translate\n"
       "Do not inject inter-area routes into nssa\n")
{
	return ospf_area_nssa_cmd_handler(vty, argc, argv, 1);
}

DEFUN (ospf_area_nssa_translate,
       ospf_area_nssa_translate_cmd,
       "area <A.B.C.D|(0-4294967295)> nssa <translate-candidate|translate-never|translate-always>",
       "OSPF area parameters\n"
       "OSPF area ID in IP address format\n"
       "OSPF area ID as a decimal value\n"
       "Configure OSPF area as nssa\n"
       "Configure NSSA-ABR for translate election (default)\n"
       "Configure NSSA-ABR to never translate\n"
       "Configure NSSA-ABR to always translate\n")
{
	return ospf_area_nssa_cmd_handler(vty, argc, argv, 0);
}

DEFUN (ospf_area_nssa,
       ospf_area_nssa_cmd,
       "area <A.B.C.D|(0-4294967295)> nssa",
       "OSPF area parameters\n"
       "OSPF area ID in IP address format\n"
       "OSPF area ID as a decimal value\n"
       "Configure OSPF area as nssa\n")
{
	return ospf_area_nssa_cmd_handler(vty, argc, argv, 0);
}

DEFUN (ospf_area_nssa_no_summary,
       ospf_area_nssa_no_summary_cmd,
       "area <A.B.C.D|(0-4294967295)> nssa no-summary",
       "OSPF area parameters\n"
       "OSPF area ID in IP address format\n"
       "OSPF area ID as a decimal value\n"
       "Configure OSPF area as nssa\n"
       "Do not inject inter-area routes into nssa\n")
{
	return ospf_area_nssa_cmd_handler(vty, argc, argv, 1);
}

DEFUN (no_ospf_area_nssa,
       no_ospf_area_nssa_cmd,
       "no area <A.B.C.D|(0-4294967295)> nssa [<translate-candidate|translate-never|translate-always> [no-summary]]",
       NO_STR
       "OSPF area parameters\n"
       "OSPF area ID in IP address format\n"
       "OSPF area ID as a decimal value\n"
       "Configure OSPF area as nssa\n"
       "Configure NSSA-ABR for translate election (default)\n"
       "Configure NSSA-ABR to never translate\n"
       "Configure NSSA-ABR to always translate\n"
       "Do not inject inter-area routes into nssa\n")
{
	VTY_DECLVAR_CONTEXT(ospf, ospf);
	int idx_ipv4_number = 2;
	struct in_addr area_id;
	int format;

	VTY_GET_OSPF_AREA_ID_NO_BB("NSSA", area_id, format,
				   argv[idx_ipv4_number]->arg);

	ospf_area_nssa_unset(ospf, area_id);
	ospf_area_no_summary_unset(ospf, area_id);

	ospf_schedule_abr_task(ospf);

	return CMD_SUCCESS;
}


DEFUN (ospf_area_default_cost,
       ospf_area_default_cost_cmd,
       "area <A.B.C.D|(0-4294967295)> default-cost (0-16777215)",
       "OSPF area parameters\n"
       "OSPF area ID in IP address format\n"
       "OSPF area ID as a decimal value\n"
       "Set the summary-default cost of a NSSA or stub area\n"
       "Stub's advertised default summary cost\n")
{
	VTY_DECLVAR_CONTEXT(ospf, ospf);
	int idx_ipv4_number = 1;
	int idx_number = 3;
	struct ospf_area *area;
	struct in_addr area_id;
	u_int32_t cost;
	int format;
	struct prefix_ipv4 p;

	VTY_GET_OSPF_AREA_ID_NO_BB("default-cost", area_id, format,
				   argv[idx_ipv4_number]->arg);
	VTY_GET_INTEGER_RANGE("stub default cost", cost, argv[idx_number]->arg,
			      0, 16777215);

	area = ospf_area_get(ospf, area_id);
	ospf_area_display_format_set(ospf, area, format);

	if (area->external_routing == OSPF_AREA_DEFAULT) {
		vty_out(vty, "The area is neither stub, nor NSSA%s",
			VTY_NEWLINE);
		return CMD_WARNING;
	}

	area->default_cost = cost;

	p.family = AF_INET;
	p.prefix.s_addr = OSPF_DEFAULT_DESTINATION;
	p.prefixlen = 0;
	if (IS_DEBUG_OSPF_EVENT)
		zlog_debug(
			"ospf_abr_announce_stub_defaults(): "
			"announcing 0.0.0.0/0 to area %s",
			inet_ntoa(area->area_id));
	ospf_abr_announce_network_to_area(&p, area->default_cost, area);

	return CMD_SUCCESS;
}

DEFUN (no_ospf_area_default_cost,
       no_ospf_area_default_cost_cmd,
       "no area <A.B.C.D|(0-4294967295)> default-cost (0-16777215)",
       NO_STR
       "OSPF area parameters\n"
       "OSPF area ID in IP address format\n"
       "OSPF area ID as a decimal value\n"
       "Set the summary-default cost of a NSSA or stub area\n"
       "Stub's advertised default summary cost\n")
{
	VTY_DECLVAR_CONTEXT(ospf, ospf);
	int idx_ipv4_number = 2;
	int idx_number = 4;
	struct ospf_area *area;
	struct in_addr area_id;
	int format;
	struct prefix_ipv4 p;

	VTY_GET_OSPF_AREA_ID_NO_BB("default-cost", area_id, format,
				   argv[idx_ipv4_number]->arg);
	VTY_CHECK_INTEGER_RANGE("stub default cost", argv[idx_number]->arg, 0,
				OSPF_LS_INFINITY);

	area = ospf_area_lookup_by_area_id(ospf, area_id);
	if (area == NULL)
		return CMD_SUCCESS;

	if (area->external_routing == OSPF_AREA_DEFAULT) {
		vty_out(vty, "The area is neither stub, nor NSSA%s",
			VTY_NEWLINE);
		return CMD_WARNING;
	}

	area->default_cost = 1;

	p.family = AF_INET;
	p.prefix.s_addr = OSPF_DEFAULT_DESTINATION;
	p.prefixlen = 0;
	if (IS_DEBUG_OSPF_EVENT)
		zlog_debug(
			"ospf_abr_announce_stub_defaults(): "
			"announcing 0.0.0.0/0 to area %s",
			inet_ntoa(area->area_id));
	ospf_abr_announce_network_to_area(&p, area->default_cost, area);


	ospf_area_check_free(ospf, area_id);

	return CMD_SUCCESS;
}

DEFUN (ospf_area_export_list,
       ospf_area_export_list_cmd,
       "area <A.B.C.D|(0-4294967295)> export-list NAME",
       "OSPF area parameters\n"
       "OSPF area ID in IP address format\n"
       "OSPF area ID as a decimal value\n"
       "Set the filter for networks announced to other areas\n"
       "Name of the access-list\n")
{
	VTY_DECLVAR_CONTEXT(ospf, ospf);
	int idx_ipv4_number = 1;
	struct ospf_area *area;
	struct in_addr area_id;
	int format;

	VTY_GET_OSPF_AREA_ID(area_id, format, argv[idx_ipv4_number]->arg);

	area = ospf_area_get(ospf, area_id);
	ospf_area_display_format_set(ospf, area, format);
	ospf_area_export_list_set(ospf, area, argv[3]->arg);

	return CMD_SUCCESS;
}

DEFUN (no_ospf_area_export_list,
       no_ospf_area_export_list_cmd,
       "no area <A.B.C.D|(0-4294967295)> export-list NAME",
       NO_STR
       "OSPF area parameters\n"
       "OSPF area ID in IP address format\n"
       "OSPF area ID as a decimal value\n"
       "Unset the filter for networks announced to other areas\n"
       "Name of the access-list\n")
{
	VTY_DECLVAR_CONTEXT(ospf, ospf);
	int idx_ipv4_number = 2;
	struct ospf_area *area;
	struct in_addr area_id;
	int format;

	VTY_GET_OSPF_AREA_ID(area_id, format, argv[idx_ipv4_number]->arg);

	area = ospf_area_lookup_by_area_id(ospf, area_id);
	if (area == NULL)
		return CMD_SUCCESS;

	ospf_area_export_list_unset(ospf, area);

	return CMD_SUCCESS;
}


DEFUN (ospf_area_import_list,
       ospf_area_import_list_cmd,
       "area <A.B.C.D|(0-4294967295)> import-list NAME",
       "OSPF area parameters\n"
       "OSPF area ID in IP address format\n"
       "OSPF area ID as a decimal value\n"
       "Set the filter for networks from other areas announced to the specified one\n"
       "Name of the access-list\n")
{
	VTY_DECLVAR_CONTEXT(ospf, ospf);
	int idx_ipv4_number = 1;
	struct ospf_area *area;
	struct in_addr area_id;
	int format;

	VTY_GET_OSPF_AREA_ID(area_id, format, argv[idx_ipv4_number]->arg);

	area = ospf_area_get(ospf, area_id);
	ospf_area_display_format_set(ospf, area, format);
	ospf_area_import_list_set(ospf, area, argv[3]->arg);

	return CMD_SUCCESS;
}

DEFUN (no_ospf_area_import_list,
       no_ospf_area_import_list_cmd,
       "no area <A.B.C.D|(0-4294967295)> import-list NAME",
       NO_STR
       "OSPF area parameters\n"
       "OSPF area ID in IP address format\n"
       "OSPF area ID as a decimal value\n"
       "Unset the filter for networks announced to other areas\n"
       "Name of the access-list\n")
{
	VTY_DECLVAR_CONTEXT(ospf, ospf);
	int idx_ipv4_number = 2;
	struct ospf_area *area;
	struct in_addr area_id;
	int format;

	VTY_GET_OSPF_AREA_ID(area_id, format, argv[idx_ipv4_number]->arg);

	area = ospf_area_lookup_by_area_id(ospf, area_id);
	if (area == NULL)
		return CMD_SUCCESS;

	ospf_area_import_list_unset(ospf, area);

	return CMD_SUCCESS;
}

DEFUN (ospf_area_filter_list,
       ospf_area_filter_list_cmd,
       "area <A.B.C.D|(0-4294967295)> filter-list prefix WORD <in|out>",
       "OSPF area parameters\n"
       "OSPF area ID in IP address format\n"
       "OSPF area ID as a decimal value\n"
       "Filter networks between OSPF areas\n"
       "Filter prefixes between OSPF areas\n"
       "Name of an IP prefix-list\n"
       "Filter networks sent to this area\n"
       "Filter networks sent from this area\n")
{
	VTY_DECLVAR_CONTEXT(ospf, ospf);
	int idx_ipv4_number = 1;
	int idx_word = 4;
	int idx_in_out = 5;
	struct ospf_area *area;
	struct in_addr area_id;
	struct prefix_list *plist;
	int format;

	VTY_GET_OSPF_AREA_ID(area_id, format, argv[idx_ipv4_number]->arg);

	area = ospf_area_get(ospf, area_id);
	ospf_area_display_format_set(ospf, area, format);
	plist = prefix_list_lookup(AFI_IP, argv[idx_word]->arg);
	if (strncmp(argv[idx_in_out]->arg, "in", 2) == 0) {
		PREFIX_LIST_IN(area) = plist;
		if (PREFIX_NAME_IN(area))
			free(PREFIX_NAME_IN(area));

		PREFIX_NAME_IN(area) = strdup(argv[idx_word]->arg);
		ospf_schedule_abr_task(ospf);
	} else {
		PREFIX_LIST_OUT(area) = plist;
		if (PREFIX_NAME_OUT(area))
			free(PREFIX_NAME_OUT(area));

		PREFIX_NAME_OUT(area) = strdup(argv[idx_word]->arg);
		ospf_schedule_abr_task(ospf);
	}

	return CMD_SUCCESS;
}

DEFUN (no_ospf_area_filter_list,
       no_ospf_area_filter_list_cmd,
       "no area <A.B.C.D|(0-4294967295)> filter-list prefix WORD <in|out>",
       NO_STR
       "OSPF area parameters\n"
       "OSPF area ID in IP address format\n"
       "OSPF area ID as a decimal value\n"
       "Filter networks between OSPF areas\n"
       "Filter prefixes between OSPF areas\n"
       "Name of an IP prefix-list\n"
       "Filter networks sent to this area\n"
       "Filter networks sent from this area\n")
{
	VTY_DECLVAR_CONTEXT(ospf, ospf);
	int idx_ipv4_number = 2;
	int idx_word = 5;
	int idx_in_out = 6;
	struct ospf_area *area;
	struct in_addr area_id;
	int format;

	VTY_GET_OSPF_AREA_ID(area_id, format, argv[idx_ipv4_number]->arg);

	if ((area = ospf_area_lookup_by_area_id(ospf, area_id)) == NULL)
		return CMD_SUCCESS;

	if (strncmp(argv[idx_in_out]->arg, "in", 2) == 0) {
		if (PREFIX_NAME_IN(area))
			if (strcmp(PREFIX_NAME_IN(area), argv[idx_word]->arg)
			    != 0)
				return CMD_SUCCESS;

		PREFIX_LIST_IN(area) = NULL;
		if (PREFIX_NAME_IN(area))
			free(PREFIX_NAME_IN(area));

		PREFIX_NAME_IN(area) = NULL;

		ospf_schedule_abr_task(ospf);
	} else {
		if (PREFIX_NAME_OUT(area))
			if (strcmp(PREFIX_NAME_OUT(area), argv[idx_word]->arg)
			    != 0)
				return CMD_SUCCESS;

		PREFIX_LIST_OUT(area) = NULL;
		if (PREFIX_NAME_OUT(area))
			free(PREFIX_NAME_OUT(area));

		PREFIX_NAME_OUT(area) = NULL;

		ospf_schedule_abr_task(ospf);
	}

	return CMD_SUCCESS;
}


DEFUN (ospf_area_authentication_message_digest,
       ospf_area_authentication_message_digest_cmd,
       "area <A.B.C.D|(0-4294967295)> authentication message-digest",
       "OSPF area parameters\n"
       "OSPF area ID in IP address format\n"
       "OSPF area ID as a decimal value\n"
       "Enable authentication\n"
       "Use message-digest authentication\n")
{
	VTY_DECLVAR_CONTEXT(ospf, ospf);
	int idx_ipv4_number = 1;
	struct ospf_area *area;
	struct in_addr area_id;
	int format;

	VTY_GET_OSPF_AREA_ID(area_id, format, argv[idx_ipv4_number]->arg);

	area = ospf_area_get(ospf, area_id);
	ospf_area_display_format_set(ospf, area, format);
	area->auth_type = OSPF_AUTH_CRYPTOGRAPHIC;

	return CMD_SUCCESS;
}

DEFUN (ospf_area_authentication,
       ospf_area_authentication_cmd,
       "area <A.B.C.D|(0-4294967295)> authentication",
       "OSPF area parameters\n"
       "OSPF area ID in IP address format\n"
       "OSPF area ID as a decimal value\n"
       "Enable authentication\n")
{
	VTY_DECLVAR_CONTEXT(ospf, ospf);
	int idx_ipv4_number = 1;
	struct ospf_area *area;
	struct in_addr area_id;
	int format;

	VTY_GET_OSPF_AREA_ID(area_id, format, argv[idx_ipv4_number]->arg);

	area = ospf_area_get(ospf, area_id);
	ospf_area_display_format_set(ospf, area, format);
	area->auth_type = OSPF_AUTH_SIMPLE;

	return CMD_SUCCESS;
}

DEFUN (no_ospf_area_authentication,
       no_ospf_area_authentication_cmd,
       "no area <A.B.C.D|(0-4294967295)> authentication",
       NO_STR
       "OSPF area parameters\n"
       "OSPF area ID in IP address format\n"
       "OSPF area ID as a decimal value\n"
       "Enable authentication\n")
{
	VTY_DECLVAR_CONTEXT(ospf, ospf);
	int idx_ipv4_number = 2;
	struct ospf_area *area;
	struct in_addr area_id;
	int format;

	VTY_GET_OSPF_AREA_ID(area_id, format, argv[idx_ipv4_number]->arg);

	area = ospf_area_lookup_by_area_id(ospf, area_id);
	if (area == NULL)
		return CMD_SUCCESS;

	area->auth_type = OSPF_AUTH_NULL;

	ospf_area_check_free(ospf, area_id);

	return CMD_SUCCESS;
}


DEFUN (ospf_abr_type,
       ospf_abr_type_cmd,
       "ospf abr-type <cisco|ibm|shortcut|standard>",
       "OSPF specific commands\n"
       "Set OSPF ABR type\n"
       "Alternative ABR, cisco implementation\n"
       "Alternative ABR, IBM implementation\n"
       "Shortcut ABR\n"
       "Standard behavior (RFC2328)\n")
{
	VTY_DECLVAR_CONTEXT(ospf, ospf);
	int idx_vendor = 2;
	u_char abr_type = OSPF_ABR_UNKNOWN;

	if (strncmp(argv[idx_vendor]->arg, "c", 1) == 0)
		abr_type = OSPF_ABR_CISCO;
	else if (strncmp(argv[idx_vendor]->arg, "i", 1) == 0)
		abr_type = OSPF_ABR_IBM;
	else if (strncmp(argv[idx_vendor]->arg, "sh", 2) == 0)
		abr_type = OSPF_ABR_SHORTCUT;
	else if (strncmp(argv[idx_vendor]->arg, "st", 2) == 0)
		abr_type = OSPF_ABR_STAND;
	else
		return CMD_WARNING;

	/* If ABR type value is changed, schedule ABR task. */
	if (ospf->abr_type != abr_type) {
		ospf->abr_type = abr_type;
		ospf_schedule_abr_task(ospf);
	}

	return CMD_SUCCESS;
}

DEFUN (no_ospf_abr_type,
       no_ospf_abr_type_cmd,
       "no ospf abr-type <cisco|ibm|shortcut|standard>",
       NO_STR
       "OSPF specific commands\n"
       "Set OSPF ABR type\n"
       "Alternative ABR, cisco implementation\n"
       "Alternative ABR, IBM implementation\n"
       "Shortcut ABR\n"
       "Standard ABR\n")
{
	VTY_DECLVAR_CONTEXT(ospf, ospf);
	int idx_vendor = 3;
	u_char abr_type = OSPF_ABR_UNKNOWN;

	if (strncmp(argv[idx_vendor]->arg, "c", 1) == 0)
		abr_type = OSPF_ABR_CISCO;
	else if (strncmp(argv[idx_vendor]->arg, "i", 1) == 0)
		abr_type = OSPF_ABR_IBM;
	else if (strncmp(argv[idx_vendor]->arg, "sh", 2) == 0)
		abr_type = OSPF_ABR_SHORTCUT;
	else if (strncmp(argv[idx_vendor]->arg, "st", 2) == 0)
		abr_type = OSPF_ABR_STAND;
	else
		return CMD_WARNING;

	/* If ABR type value is changed, schedule ABR task. */
	if (ospf->abr_type == abr_type) {
		ospf->abr_type = OSPF_ABR_DEFAULT;
		ospf_schedule_abr_task(ospf);
	}

	return CMD_SUCCESS;
}

DEFUN (ospf_log_adjacency_changes,
       ospf_log_adjacency_changes_cmd,
       "log-adjacency-changes",
       "Log changes in adjacency state\n")
{
	VTY_DECLVAR_CONTEXT(ospf, ospf);

	SET_FLAG(ospf->config, OSPF_LOG_ADJACENCY_CHANGES);
	UNSET_FLAG(ospf->config, OSPF_LOG_ADJACENCY_DETAIL);
	return CMD_SUCCESS;
}

DEFUN (ospf_log_adjacency_changes_detail,
       ospf_log_adjacency_changes_detail_cmd,
       "log-adjacency-changes detail",
       "Log changes in adjacency state\n"
       "Log all state changes\n")
{
	VTY_DECLVAR_CONTEXT(ospf, ospf);

	SET_FLAG(ospf->config, OSPF_LOG_ADJACENCY_CHANGES);
	SET_FLAG(ospf->config, OSPF_LOG_ADJACENCY_DETAIL);
	return CMD_SUCCESS;
}

DEFUN (no_ospf_log_adjacency_changes,
       no_ospf_log_adjacency_changes_cmd,
       "no log-adjacency-changes",
       NO_STR
       "Log changes in adjacency state\n")
{
	VTY_DECLVAR_CONTEXT(ospf, ospf);

	UNSET_FLAG(ospf->config, OSPF_LOG_ADJACENCY_DETAIL);
	UNSET_FLAG(ospf->config, OSPF_LOG_ADJACENCY_CHANGES);
	return CMD_SUCCESS;
}

DEFUN (no_ospf_log_adjacency_changes_detail,
       no_ospf_log_adjacency_changes_detail_cmd,
       "no log-adjacency-changes detail",
       NO_STR
       "Log changes in adjacency state\n"
       "Log all state changes\n")
{
	VTY_DECLVAR_CONTEXT(ospf, ospf);

	UNSET_FLAG(ospf->config, OSPF_LOG_ADJACENCY_DETAIL);
	return CMD_SUCCESS;
}

DEFUN (ospf_compatible_rfc1583,
       ospf_compatible_rfc1583_cmd,
       "compatible rfc1583",
       "OSPF compatibility list\n"
       "compatible with RFC 1583\n")
{
	VTY_DECLVAR_CONTEXT(ospf, ospf);

	if (!CHECK_FLAG(ospf->config, OSPF_RFC1583_COMPATIBLE)) {
		SET_FLAG(ospf->config, OSPF_RFC1583_COMPATIBLE);
		ospf_spf_calculate_schedule(ospf, SPF_FLAG_CONFIG_CHANGE);
	}
	return CMD_SUCCESS;
}

DEFUN (no_ospf_compatible_rfc1583,
       no_ospf_compatible_rfc1583_cmd,
       "no compatible rfc1583",
       NO_STR
       "OSPF compatibility list\n"
       "compatible with RFC 1583\n")
{
	VTY_DECLVAR_CONTEXT(ospf, ospf);

	if (CHECK_FLAG(ospf->config, OSPF_RFC1583_COMPATIBLE)) {
		UNSET_FLAG(ospf->config, OSPF_RFC1583_COMPATIBLE);
		ospf_spf_calculate_schedule(ospf, SPF_FLAG_CONFIG_CHANGE);
	}
	return CMD_SUCCESS;
}

ALIAS(ospf_compatible_rfc1583, ospf_rfc1583_flag_cmd,
      "ospf rfc1583compatibility",
      "OSPF specific commands\n"
      "Enable the RFC1583Compatibility flag\n")

ALIAS(no_ospf_compatible_rfc1583, no_ospf_rfc1583_flag_cmd,
      "no ospf rfc1583compatibility", NO_STR
      "OSPF specific commands\n"
      "Disable the RFC1583Compatibility flag\n")

static int ospf_timers_spf_set(struct vty *vty, unsigned int delay,
			       unsigned int hold, unsigned int max)
{
	VTY_DECLVAR_CONTEXT(ospf, ospf);

	ospf->spf_delay = delay;
	ospf->spf_holdtime = hold;
	ospf->spf_max_holdtime = max;

	return CMD_SUCCESS;
}

DEFUN (ospf_timers_min_ls_interval,
       ospf_timers_min_ls_interval_cmd,
       "timers throttle lsa all (0-5000)",
       "Adjust routing timers\n"
       "Throttling adaptive timer\n"
       "LSA delay between transmissions\n"
       "All LSA types\n"
       "Delay (msec) between sending LSAs\n")
{
	VTY_DECLVAR_CONTEXT(ospf, ospf);
	int idx_number = 4;
	unsigned int interval;

	if (argc != 1) {
		vty_out(vty, "Insufficient arguments%s", VTY_NEWLINE);
		return CMD_WARNING;
	}

	VTY_GET_INTEGER("LSA interval", interval, argv[idx_number]->arg);

	ospf->min_ls_interval = interval;

	return CMD_SUCCESS;
}

DEFUN (no_ospf_timers_min_ls_interval,
       no_ospf_timers_min_ls_interval_cmd,
       "no timers throttle lsa all [(0-5000)]",
       NO_STR
       "Adjust routing timers\n"
       "Throttling adaptive timer\n"
       "LSA delay between transmissions\n"
       "All LSA types\n"
       "Delay (msec) between sending LSAs\n")
{
	VTY_DECLVAR_CONTEXT(ospf, ospf);
	ospf->min_ls_interval = OSPF_MIN_LS_INTERVAL;

	return CMD_SUCCESS;
}


DEFUN (ospf_timers_min_ls_arrival,
       ospf_timers_min_ls_arrival_cmd,
       "timers lsa arrival (0-1000)",
       "Adjust routing timers\n"
       "Throttling link state advertisement delays\n"
       "OSPF minimum arrival interval delay\n"
       "Delay (msec) between accepted LSAs\n")
{
	VTY_DECLVAR_CONTEXT(ospf, ospf);
	int idx_number = 3;
	unsigned int arrival;

	if (argc != 1) {
		vty_out(vty, "Insufficient arguments%s", VTY_NEWLINE);
		return CMD_WARNING;
	}

	VTY_GET_INTEGER_RANGE("minimum LSA inter-arrival time", arrival,
			      argv[idx_number]->arg, 0, 1000);

	ospf->min_ls_arrival = arrival;

	return CMD_SUCCESS;
}

DEFUN (no_ospf_timers_min_ls_arrival,
       no_ospf_timers_min_ls_arrival_cmd,
       "no timers lsa arrival [(0-1000)]",
       NO_STR
       "Adjust routing timers\n"
       "Throttling link state advertisement delays\n"
       "OSPF minimum arrival interval delay\n"
       "Delay (msec) between accepted LSAs\n")
{
	VTY_DECLVAR_CONTEXT(ospf, ospf);

	ospf->min_ls_arrival = OSPF_MIN_LS_ARRIVAL;

	return CMD_SUCCESS;
}


DEFUN (ospf_timers_throttle_spf,
       ospf_timers_throttle_spf_cmd,
       "timers throttle spf (0-600000) (0-600000) (0-600000)",
       "Adjust routing timers\n"
       "Throttling adaptive timer\n"
       "OSPF SPF timers\n"
       "Delay (msec) from first change received till SPF calculation\n"
       "Initial hold time (msec) between consecutive SPF calculations\n"
       "Maximum hold time (msec)\n")
{
	int idx_number = 3;
	int idx_number_2 = 4;
	int idx_number_3 = 5;
	unsigned int delay, hold, max;

	if (argc != 3) {
		vty_out(vty, "Insufficient arguments%s", VTY_NEWLINE);
		return CMD_WARNING;
	}

	VTY_GET_INTEGER_RANGE("SPF delay timer", delay, argv[idx_number]->arg,
			      0, 600000);
	VTY_GET_INTEGER_RANGE("SPF hold timer", hold, argv[idx_number_2]->arg,
			      0, 600000);
	VTY_GET_INTEGER_RANGE("SPF max-hold timer", max,
			      argv[idx_number_3]->arg, 0, 600000);

	return ospf_timers_spf_set(vty, delay, hold, max);
}

DEFUN (no_ospf_timers_throttle_spf,
       no_ospf_timers_throttle_spf_cmd,
       "no timers throttle spf [(0-600000)(0-600000)(0-600000)]",
       NO_STR
       "Adjust routing timers\n"
       "Throttling adaptive timer\n"
       "OSPF SPF timers\n"
       "Delay (msec) from first change received till SPF calculation\n"
       "Initial hold time (msec) between consecutive SPF calculations\n"
       "Maximum hold time (msec)\n")
{
	return ospf_timers_spf_set(vty, OSPF_SPF_DELAY_DEFAULT,
				   OSPF_SPF_HOLDTIME_DEFAULT,
				   OSPF_SPF_MAX_HOLDTIME_DEFAULT);
}


DEFUN (ospf_timers_lsa,
       ospf_timers_lsa_cmd,
       "timers lsa min-arrival (0-600000)",
       "Adjust routing timers\n"
       "OSPF LSA timers\n"
       "Minimum delay in receiving new version of a LSA\n"
       "Delay in milliseconds\n")
{
	VTY_DECLVAR_CONTEXT(ospf, ospf);
	int idx_number = 3;
	unsigned int minarrival;

	if (argc != 1) {
		vty_out(vty, "Insufficient number of arguments%s", VTY_NEWLINE);
		return CMD_WARNING;
	}

	VTY_GET_INTEGER("LSA min-arrival", minarrival, argv[idx_number]->arg);

	ospf->min_ls_arrival = minarrival;

	return CMD_SUCCESS;
}

DEFUN (no_ospf_timers_lsa,
       no_ospf_timers_lsa_cmd,
       "no timers lsa min-arrival [(0-600000)]",
       NO_STR
       "Adjust routing timers\n"
       "OSPF LSA timers\n"
       "Minimum delay in receiving new version of a LSA\n"
       "Delay in milliseconds\n")
{
	VTY_DECLVAR_CONTEXT(ospf, ospf);
	unsigned int minarrival;

	if (argc > 4) {
		VTY_GET_INTEGER("LSA min-arrival", minarrival, argv[4]->arg);

		if (ospf->min_ls_arrival != minarrival
		    || minarrival == OSPF_MIN_LS_ARRIVAL)
			return CMD_SUCCESS;
	}

	ospf->min_ls_arrival = OSPF_MIN_LS_ARRIVAL;

	return CMD_SUCCESS;
}

DEFUN (ospf_neighbor,
       ospf_neighbor_cmd,
       "neighbor A.B.C.D [priority (0-255) [poll-interval (1-65535)]]",
       NEIGHBOR_STR
       "Neighbor IP address\n"
       "Neighbor Priority\n"
       "Priority\n"
       "Dead Neighbor Polling interval\n"
       "Seconds\n")
{
	VTY_DECLVAR_CONTEXT(ospf, ospf);
	int idx_ipv4 = 1;
	int idx_pri = 3;
	int idx_poll = 5;
	struct in_addr nbr_addr;
	unsigned int priority = OSPF_NEIGHBOR_PRIORITY_DEFAULT;
	unsigned int interval = OSPF_POLL_INTERVAL_DEFAULT;

	VTY_GET_IPV4_ADDRESS("neighbor address", nbr_addr, argv[idx_ipv4]->arg);

	if (argc > 2)
		VTY_GET_INTEGER_RANGE("neighbor priority", priority,
				      argv[idx_pri]->arg, 0, 255);

	if (argc > 4)
		VTY_GET_INTEGER_RANGE("poll interval", interval,
				      argv[idx_poll]->arg, 1, 65535);

	ospf_nbr_nbma_set(ospf, nbr_addr);

	if (argc > 2)
		ospf_nbr_nbma_priority_set(ospf, nbr_addr, priority);

	if (argc > 4)
		ospf_nbr_nbma_poll_interval_set(ospf, nbr_addr, interval);

	return CMD_SUCCESS;
}

DEFUN (ospf_neighbor_poll_interval,
       ospf_neighbor_poll_interval_cmd,
       "neighbor A.B.C.D poll-interval (1-65535) [priority (0-255)]",
       NEIGHBOR_STR
       "Neighbor IP address\n"
       "Dead Neighbor Polling interval\n"
       "Seconds\n"
       "OSPF priority of non-broadcast neighbor\n"
       "Priority\n")
{
	VTY_DECLVAR_CONTEXT(ospf, ospf);
	int idx_ipv4 = 1;
	int idx_poll = 3;
	int idx_pri = 5;
	struct in_addr nbr_addr;
	unsigned int priority = OSPF_NEIGHBOR_PRIORITY_DEFAULT;
	unsigned int interval = OSPF_POLL_INTERVAL_DEFAULT;

	VTY_GET_IPV4_ADDRESS("neighbor address", nbr_addr, argv[idx_ipv4]->arg);

	VTY_GET_INTEGER_RANGE("poll interval", interval, argv[idx_poll]->arg, 1,
			      65535);

	if (argc > 4)
		VTY_GET_INTEGER_RANGE("neighbor priority", priority,
				      argv[idx_pri]->arg, 0, 255);

	ospf_nbr_nbma_set(ospf, nbr_addr);
	ospf_nbr_nbma_poll_interval_set(ospf, nbr_addr, interval);

	if (argc > 4)
		ospf_nbr_nbma_priority_set(ospf, nbr_addr, priority);

	return CMD_SUCCESS;
}

DEFUN (no_ospf_neighbor,
       no_ospf_neighbor_cmd,
       "no neighbor A.B.C.D [priority (0-255) [poll-interval (1-65525)]]",
       NO_STR
       NEIGHBOR_STR
       "Neighbor IP address\n"
       "Neighbor Priority\n"
       "Priority\n"
       "Dead Neighbor Polling interval\n"
       "Seconds\n")
{
	VTY_DECLVAR_CONTEXT(ospf, ospf);
	int idx_ipv4 = 2;
	struct in_addr nbr_addr;

	VTY_GET_IPV4_ADDRESS("neighbor address", nbr_addr, argv[idx_ipv4]->arg);

	(void)ospf_nbr_nbma_unset(ospf, nbr_addr);

	return CMD_SUCCESS;
}

DEFUN (no_ospf_neighbor_poll,
       no_ospf_neighbor_poll_cmd,
       "no neighbor A.B.C.D poll-interval (1-65535) [priority (0-255)]",
       NO_STR
       NEIGHBOR_STR
       "Neighbor IP address\n"
       "Dead Neighbor Polling interval\n"
       "Seconds\n"
       "Neighbor Priority\n"
       "Priority\n")
{
	VTY_DECLVAR_CONTEXT(ospf, ospf);
	int idx_ipv4 = 2;
	struct in_addr nbr_addr;

	VTY_GET_IPV4_ADDRESS("neighbor address", nbr_addr, argv[idx_ipv4]->arg);

	(void)ospf_nbr_nbma_unset(ospf, nbr_addr);

	return CMD_SUCCESS;
}

DEFUN (ospf_refresh_timer,
       ospf_refresh_timer_cmd,
       "refresh timer (10-1800)",
       "Adjust refresh parameters\n"
       "Set refresh timer\n"
       "Timer value in seconds\n")
{
	VTY_DECLVAR_CONTEXT(ospf, ospf);
	int idx_number = 2;
	unsigned int interval;

	VTY_GET_INTEGER_RANGE("refresh timer", interval, argv[idx_number]->arg,
			      10, 1800);
	interval = (interval / OSPF_LSA_REFRESHER_GRANULARITY)
		   * OSPF_LSA_REFRESHER_GRANULARITY;

	ospf_timers_refresh_set(ospf, interval);

	return CMD_SUCCESS;
}

DEFUN (no_ospf_refresh_timer,
       no_ospf_refresh_timer_val_cmd,
       "no refresh timer [(10-1800)]",
       NO_STR
       "Adjust refresh parameters\n"
       "Unset refresh timer\n"
       "Timer value in seconds\n")
{
	VTY_DECLVAR_CONTEXT(ospf, ospf);
	int idx_number = 3;
	unsigned int interval;

	if (argc == 1) {
		VTY_GET_INTEGER_RANGE("refresh timer", interval,
				      argv[idx_number]->arg, 10, 1800);

		if (ospf->lsa_refresh_interval != interval
		    || interval == OSPF_LSA_REFRESH_INTERVAL_DEFAULT)
			return CMD_SUCCESS;
	}

	ospf_timers_refresh_unset(ospf);

	return CMD_SUCCESS;
}


DEFUN (ospf_auto_cost_reference_bandwidth,
       ospf_auto_cost_reference_bandwidth_cmd,
       "auto-cost reference-bandwidth (1-4294967)",
       "Calculate OSPF interface cost according to bandwidth\n"
       "Use reference bandwidth method to assign OSPF cost\n"
       "The reference bandwidth in terms of Mbits per second\n")
{
	VTY_DECLVAR_CONTEXT(ospf, ospf);
	int idx_number = 2;
	u_int32_t refbw;
	struct listnode *node;
	struct interface *ifp;

	refbw = strtol(argv[idx_number]->arg, NULL, 10);
	if (refbw < 1 || refbw > 4294967) {
		vty_out(vty, "reference-bandwidth value is invalid%s",
			VTY_NEWLINE);
		return CMD_WARNING;
	}

	/* If reference bandwidth is changed. */
	if ((refbw) == ospf->ref_bandwidth)
		return CMD_SUCCESS;

	ospf->ref_bandwidth = refbw;
	for (ALL_LIST_ELEMENTS_RO(om->iflist, node, ifp))
		ospf_if_recalculate_output_cost(ifp);

	return CMD_SUCCESS;
}

DEFUN (no_ospf_auto_cost_reference_bandwidth,
       no_ospf_auto_cost_reference_bandwidth_cmd,
       "no auto-cost reference-bandwidth [(1-4294967)]",
       NO_STR
       "Calculate OSPF interface cost according to bandwidth\n"
       "Use reference bandwidth method to assign OSPF cost\n"
       "The reference bandwidth in terms of Mbits per second\n")
{
	VTY_DECLVAR_CONTEXT(ospf, ospf);
	struct listnode *node, *nnode;
	struct interface *ifp;

	if (ospf->ref_bandwidth == OSPF_DEFAULT_REF_BANDWIDTH)
		return CMD_SUCCESS;

	ospf->ref_bandwidth = OSPF_DEFAULT_REF_BANDWIDTH;
	vty_out(vty, "%% OSPF: Reference bandwidth is changed.%s", VTY_NEWLINE);
	vty_out(vty,
		"        Please ensure reference bandwidth is consistent across all routers%s",
		VTY_NEWLINE);

	for (ALL_LIST_ELEMENTS(om->iflist, node, nnode, ifp))
		ospf_if_recalculate_output_cost(ifp);

	return CMD_SUCCESS;
}

DEFUN (ospf_write_multiplier,
       ospf_write_multiplier_cmd,
       "ospf write-multiplier (1-100)",
       "OSPF specific commands\n"
       "Write multiplier\n"
       "Maximum number of interface serviced per write\n")
{
	VTY_DECLVAR_CONTEXT(ospf, ospf);
	int idx_number;
	u_int32_t write_oi_count;

	if (argc == 3)
		idx_number = 2;
	else
		idx_number = 1;

	write_oi_count = strtol(argv[idx_number]->arg, NULL, 10);
	if (write_oi_count < 1 || write_oi_count > 100) {
		vty_out(vty, "write-multiplier value is invalid%s",
			VTY_NEWLINE);
		return CMD_WARNING;
	}

	ospf->write_oi_count = write_oi_count;
	return CMD_SUCCESS;
}

ALIAS(ospf_write_multiplier, write_multiplier_cmd, "write-multiplier (1-100)",
      "Write multiplier\n"
      "Maximum number of interface serviced per write\n")

DEFUN (no_ospf_write_multiplier,
       no_ospf_write_multiplier_cmd,
       "no ospf write-multiplier (1-100)",
       NO_STR
       "OSPF specific commands\n"
       "Write multiplier\n"
       "Maximum number of interface serviced per write\n")
{
	VTY_DECLVAR_CONTEXT(ospf, ospf);

	ospf->write_oi_count = OSPF_WRITE_INTERFACE_COUNT_DEFAULT;
	return CMD_SUCCESS;
}

ALIAS(no_ospf_write_multiplier, no_write_multiplier_cmd,
      "no write-multiplier (1-100)", NO_STR
      "Write multiplier\n"
      "Maximum number of interface serviced per write\n")

const char *ospf_abr_type_descr_str[] = {"Unknown", "Standard (RFC2328)",
					 "Alternative IBM", "Alternative Cisco",
					 "Alternative Shortcut"};

const char *ospf_shortcut_mode_descr_str[] = {"Default", "Enabled", "Disabled"};

static void show_ip_ospf_area(struct vty *vty, struct ospf_area *area,
			      json_object *json_areas, u_char use_json)
{
	json_object *json_area = NULL;

	if (use_json)
		json_area = json_object_new_object();

	/* Show Area ID. */
	if (!use_json)
		vty_out(vty, " Area ID: %s", inet_ntoa(area->area_id));

	/* Show Area type/mode. */
	if (OSPF_IS_AREA_BACKBONE(area)) {
		if (use_json)
			json_object_boolean_true_add(json_area, "backbone");
		else
			vty_out(vty, " (Backbone)%s", VTY_NEWLINE);
	} else {
		if (use_json) {
			if (area->external_routing == OSPF_AREA_STUB) {
				if (area->no_summary)
					json_object_boolean_true_add(
						json_area, "stubNoSummary");
				if (area->shortcut_configured)
					json_object_boolean_true_add(
						json_area, "stubShortcut");
			} else if (area->external_routing == OSPF_AREA_NSSA) {
				if (area->no_summary)
					json_object_boolean_true_add(
						json_area, "nssaNoSummary");
				if (area->shortcut_configured)
					json_object_boolean_true_add(
						json_area, "nssaShortcut");
			}

			json_object_string_add(
				json_area, "shortcuttingMode",
				ospf_shortcut_mode_descr_str
					[area->shortcut_configured]);
			if (area->shortcut_capability)
				json_object_boolean_true_add(json_area,
							     "sBitConcensus");
		} else {
			if (area->external_routing == OSPF_AREA_STUB)
				vty_out(vty, " (Stub%s%s)",
					area->no_summary ? ", no summary" : "",
					area->shortcut_configured ? "; " : "");
			else if (area->external_routing == OSPF_AREA_NSSA)
				vty_out(vty, " (NSSA%s%s)",
					area->no_summary ? ", no summary" : "",
					area->shortcut_configured ? "; " : "");

			vty_out(vty, "%s", VTY_NEWLINE);
			vty_out(vty, "   Shortcutting mode: %s",
				ospf_shortcut_mode_descr_str
					[area->shortcut_configured]);
			vty_out(vty, ", S-bit consensus: %s%s",
				area->shortcut_capability ? "ok" : "no",
				VTY_NEWLINE);
		}
	}

	/* Show number of interfaces */
	if (use_json) {
		json_object_int_add(json_area, "areaIfTotalCounter",
				    listcount(area->oiflist));
		json_object_int_add(json_area, "areaIfActiveCounter",
				    area->act_ints);
	} else
		vty_out(vty,
			"   Number of interfaces in this area: Total: %d, "
			"Active: %d%s",
			listcount(area->oiflist), area->act_ints, VTY_NEWLINE);

	if (area->external_routing == OSPF_AREA_NSSA) {
		if (use_json) {
			json_object_boolean_true_add(json_area, "nssa");
			if (!IS_OSPF_ABR(area->ospf))
				json_object_boolean_false_add(json_area, "abr");
			else if (area->NSSATranslatorState) {
				json_object_boolean_true_add(json_area, "abr");
				if (area->NSSATranslatorRole
				    == OSPF_NSSA_ROLE_CANDIDATE)
					json_object_boolean_true_add(
						json_area,
						"nssaTranslatorElected");
				else if (area->NSSATranslatorRole
					 == OSPF_NSSA_ROLE_ALWAYS)
					json_object_boolean_true_add(
						json_area,
						"nssaTranslatorAlways");
			} else {
				json_object_boolean_true_add(json_area, "abr");
				if (area->NSSATranslatorRole
				    == OSPF_NSSA_ROLE_CANDIDATE)
					json_object_boolean_false_add(
						json_area,
						"nssaTranslatorElected");
				else
					json_object_boolean_true_add(
						json_area,
						"nssaTranslatorNever");
			}
		} else {
			vty_out(vty,
				"   It is an NSSA configuration. %s   Elected NSSA/ABR performs type-7/type-5 LSA translation. %s",
				VTY_NEWLINE, VTY_NEWLINE);
			if (!IS_OSPF_ABR(area->ospf))
				vty_out(vty,
					"   It is not ABR, therefore not Translator. %s",
					VTY_NEWLINE);
			else if (area->NSSATranslatorState) {
				vty_out(vty, "   We are an ABR and ");
				if (area->NSSATranslatorRole
				    == OSPF_NSSA_ROLE_CANDIDATE)
					vty_out(vty,
						"the NSSA Elected Translator. %s",
						VTY_NEWLINE);
				else if (area->NSSATranslatorRole
					 == OSPF_NSSA_ROLE_ALWAYS)
					vty_out(vty,
						"always an NSSA Translator. %s",
						VTY_NEWLINE);
			} else {
				vty_out(vty, "   We are an ABR, but ");
				if (area->NSSATranslatorRole
				    == OSPF_NSSA_ROLE_CANDIDATE)
					vty_out(vty,
						"not the NSSA Elected Translator. %s",
						VTY_NEWLINE);
				else
					vty_out(vty,
						"never an NSSA Translator. %s",
						VTY_NEWLINE);
			}
		}
	}

	/* Stub-router state for this area */
	if (CHECK_FLAG(area->stub_router_state, OSPF_AREA_IS_STUB_ROUTED)) {
		char timebuf[OSPF_TIME_DUMP_SIZE];

		if (use_json) {
			json_object_boolean_true_add(
				json_area, "originStubMaxDistRouterLsa");
			if (CHECK_FLAG(area->stub_router_state,
				       OSPF_AREA_ADMIN_STUB_ROUTED))
				json_object_boolean_true_add(
					json_area, "indefiniteActiveAdmin");
			if (area->t_stub_router) {
				long time_store;
				time_store =
					monotime_until(
						&area->t_stub_router->u.sands,
						NULL)
					/ 1000LL;
				json_object_int_add(
					json_area,
					"activeStartupRemainderMsecs",
					time_store);
			}
		} else {
			vty_out(vty,
				"   Originating stub / maximum-distance Router-LSA%s",
				VTY_NEWLINE);
			if (CHECK_FLAG(area->stub_router_state,
				       OSPF_AREA_ADMIN_STUB_ROUTED))
				vty_out(vty,
					"     Administratively activated (indefinitely)%s",
					VTY_NEWLINE);
			if (area->t_stub_router)
				vty_out(vty,
					"     Active from startup, %s remaining%s",
					ospf_timer_dump(area->t_stub_router,
							timebuf,
							sizeof(timebuf)),
					VTY_NEWLINE);
		}
	}

	if (use_json) {
		/* Show number of fully adjacent neighbors. */
		json_object_int_add(json_area, "nbrFullAdjacentCounter",
				    area->full_nbrs);

		/* Show authentication type. */
		if (area->auth_type == OSPF_AUTH_NULL)
			json_object_string_add(json_area, "authentication",
					       "authenticationNone");
		else if (area->auth_type == OSPF_AUTH_SIMPLE)
			json_object_string_add(json_area, "authentication",
					       "authenticationSimplePassword");
		else if (area->auth_type == OSPF_AUTH_CRYPTOGRAPHIC)
			json_object_string_add(json_area, "authentication",
					       "authenticationMessageDigest");

		if (!OSPF_IS_AREA_BACKBONE(area))
			json_object_int_add(json_area,
					    "virtualAdjacenciesPassingCounter",
					    area->full_vls);

		/* Show SPF calculation times. */
		json_object_int_add(json_area, "spfExecutedCounter",
				    area->spf_calculation);
		json_object_int_add(json_area, "lsaNumber", area->lsdb->total);
		json_object_int_add(
			json_area, "lsaRouterNumber",
			ospf_lsdb_count(area->lsdb, OSPF_ROUTER_LSA));
		json_object_int_add(
			json_area, "lsaRouterChecksum",
			ospf_lsdb_checksum(area->lsdb, OSPF_ROUTER_LSA));
		json_object_int_add(
			json_area, "lsaNetworkNumber",
			ospf_lsdb_count(area->lsdb, OSPF_NETWORK_LSA));
		json_object_int_add(
			json_area, "lsaNetworkChecksum",
			ospf_lsdb_checksum(area->lsdb, OSPF_NETWORK_LSA));
		json_object_int_add(
			json_area, "lsaSummaryNumber",
			ospf_lsdb_count(area->lsdb, OSPF_SUMMARY_LSA));
		json_object_int_add(
			json_area, "lsaSummaryChecksum",
			ospf_lsdb_checksum(area->lsdb, OSPF_SUMMARY_LSA));
		json_object_int_add(
			json_area, "lsaAsbrNumber",
			ospf_lsdb_count(area->lsdb, OSPF_ASBR_SUMMARY_LSA));
		json_object_int_add(
			json_area, "lsaAsbrChecksum",
			ospf_lsdb_checksum(area->lsdb, OSPF_ASBR_SUMMARY_LSA));
		json_object_int_add(
			json_area, "lsaNssaNumber",
			ospf_lsdb_count(area->lsdb, OSPF_AS_NSSA_LSA));
		json_object_int_add(
			json_area, "lsaNssaChecksum",
			ospf_lsdb_checksum(area->lsdb, OSPF_AS_NSSA_LSA));
	} else {
		/* Show number of fully adjacent neighbors. */
		vty_out(vty,
			"   Number of fully adjacent neighbors in this area:"
			" %d%s",
			area->full_nbrs, VTY_NEWLINE);

		/* Show authentication type. */
		vty_out(vty, "   Area has ");
		if (area->auth_type == OSPF_AUTH_NULL)
			vty_out(vty, "no authentication%s", VTY_NEWLINE);
		else if (area->auth_type == OSPF_AUTH_SIMPLE)
			vty_out(vty, "simple password authentication%s",
				VTY_NEWLINE);
		else if (area->auth_type == OSPF_AUTH_CRYPTOGRAPHIC)
			vty_out(vty, "message digest authentication%s",
				VTY_NEWLINE);

		if (!OSPF_IS_AREA_BACKBONE(area))
			vty_out(vty,
				"   Number of full virtual adjacencies going through"
				" this area: %d%s",
				area->full_vls, VTY_NEWLINE);

		/* Show SPF calculation times. */
		vty_out(vty, "   SPF algorithm executed %d times%s",
			area->spf_calculation, VTY_NEWLINE);

		/* Show number of LSA. */
		vty_out(vty, "   Number of LSA %ld%s", area->lsdb->total,
			VTY_NEWLINE);
		vty_out(vty,
			"   Number of router LSA %ld. Checksum Sum 0x%08x%s",
			ospf_lsdb_count(area->lsdb, OSPF_ROUTER_LSA),
			ospf_lsdb_checksum(area->lsdb, OSPF_ROUTER_LSA),
			VTY_NEWLINE);
		vty_out(vty,
			"   Number of network LSA %ld. Checksum Sum 0x%08x%s",
			ospf_lsdb_count(area->lsdb, OSPF_NETWORK_LSA),
			ospf_lsdb_checksum(area->lsdb, OSPF_NETWORK_LSA),
			VTY_NEWLINE);
		vty_out(vty,
			"   Number of summary LSA %ld. Checksum Sum 0x%08x%s",
			ospf_lsdb_count(area->lsdb, OSPF_SUMMARY_LSA),
			ospf_lsdb_checksum(area->lsdb, OSPF_SUMMARY_LSA),
			VTY_NEWLINE);
		vty_out(vty,
			"   Number of ASBR summary LSA %ld. Checksum Sum 0x%08x%s",
			ospf_lsdb_count(area->lsdb, OSPF_ASBR_SUMMARY_LSA),
			ospf_lsdb_checksum(area->lsdb, OSPF_ASBR_SUMMARY_LSA),
			VTY_NEWLINE);
		vty_out(vty, "   Number of NSSA LSA %ld. Checksum Sum 0x%08x%s",
			ospf_lsdb_count(area->lsdb, OSPF_AS_NSSA_LSA),
			ospf_lsdb_checksum(area->lsdb, OSPF_AS_NSSA_LSA),
			VTY_NEWLINE);
	}

	if (use_json) {
		json_object_int_add(
			json_area, "lsaOpaqueLinkNumber",
			ospf_lsdb_count(area->lsdb, OSPF_OPAQUE_LINK_LSA));
		json_object_int_add(
			json_area, "lsaOpaqueLinkChecksum",
			ospf_lsdb_checksum(area->lsdb, OSPF_OPAQUE_LINK_LSA));
		json_object_int_add(
			json_area, "lsaOpaqueAreaNumber",
			ospf_lsdb_count(area->lsdb, OSPF_OPAQUE_AREA_LSA));
		json_object_int_add(
			json_area, "lsaOpaqueAreaChecksum",
			ospf_lsdb_checksum(area->lsdb, OSPF_OPAQUE_AREA_LSA));
	} else {
		vty_out(vty,
			"   Number of opaque link LSA %ld. Checksum Sum 0x%08x%s",
			ospf_lsdb_count(area->lsdb, OSPF_OPAQUE_LINK_LSA),
			ospf_lsdb_checksum(area->lsdb, OSPF_OPAQUE_LINK_LSA),
			VTY_NEWLINE);
		vty_out(vty,
			"   Number of opaque area LSA %ld. Checksum Sum 0x%08x%s",
			ospf_lsdb_count(area->lsdb, OSPF_OPAQUE_AREA_LSA),
			ospf_lsdb_checksum(area->lsdb, OSPF_OPAQUE_AREA_LSA),
			VTY_NEWLINE);
	}

	if (use_json)
		json_object_object_add(json_areas, inet_ntoa(area->area_id),
				       json_area);
	else
		vty_out(vty, "%s", VTY_NEWLINE);
}

static int show_ip_ospf_common(struct vty *vty, struct ospf *ospf,
			       u_char use_json)
{
	struct listnode *node, *nnode;
	struct ospf_area *area;
	struct timeval result;
	char timebuf[OSPF_TIME_DUMP_SIZE];
	json_object *json = NULL;
	json_object *json_areas = NULL;

	if (use_json) {
		json = json_object_new_object();
		json_areas = json_object_new_object();
	}

	if (ospf->instance) {
		if (use_json) {
			json_object_int_add(json, "ospfInstance",
					    ospf->instance);
		} else {
			vty_out(vty, "%sOSPF Instance: %d%s%s", VTY_NEWLINE,
				ospf->instance, VTY_NEWLINE, VTY_NEWLINE);
		}
	}

	/* Show Router ID. */
	if (use_json) {
		json_object_string_add(json, "routerId",
				       inet_ntoa(ospf->router_id));
	} else {
		vty_out(vty, " OSPF Routing Process, Router ID: %s%s",
			inet_ntoa(ospf->router_id), VTY_NEWLINE);
	}

	/* Graceful shutdown */
	if (ospf->t_deferred_shutdown) {
		if (use_json) {
			long time_store;
			time_store =
				monotime_until(
					&ospf->t_deferred_shutdown->u.sands,
					NULL)
				/ 1000LL;
			json_object_int_add(json, "deferredShutdownMsecs",
					    time_store);
		} else {
			vty_out(vty,
				" Deferred shutdown in progress, %s remaining%s",
				ospf_timer_dump(ospf->t_deferred_shutdown,
						timebuf, sizeof(timebuf)),
				VTY_NEWLINE);
		}
	}

	/* Show capability. */
	if (use_json) {
		json_object_boolean_true_add(json, "tosRoutesOnly");
		json_object_boolean_true_add(json, "rfc2328Conform");
		if (CHECK_FLAG(ospf->config, OSPF_RFC1583_COMPATIBLE)) {
			json_object_boolean_true_add(json,
						     "rfc1583Compatibility");
		}
	} else {
		vty_out(vty, " Supports only single TOS (TOS0) routes%s",
			VTY_NEWLINE);
		vty_out(vty, " This implementation conforms to RFC2328%s",
			VTY_NEWLINE);
		vty_out(vty, " RFC1583Compatibility flag is %s%s",
			CHECK_FLAG(ospf->config, OSPF_RFC1583_COMPATIBLE)
				? "enabled"
				: "disabled",
			VTY_NEWLINE);
	}

	if (use_json) {
		if (CHECK_FLAG(ospf->config, OSPF_OPAQUE_CAPABLE)) {
			json_object_boolean_true_add(json, "opaqueCapable");
		}
	} else {
		vty_out(vty, " OpaqueCapability flag is %s%s",
			CHECK_FLAG(ospf->config, OSPF_OPAQUE_CAPABLE)
				? "enabled"
				: "disabled",
			VTY_NEWLINE);
	}

	/* Show stub-router configuration */
	if (ospf->stub_router_startup_time != OSPF_STUB_ROUTER_UNCONFIGURED
	    || ospf->stub_router_shutdown_time
		       != OSPF_STUB_ROUTER_UNCONFIGURED) {
		if (use_json) {
			json_object_boolean_true_add(json, "stubAdvertisement");
			if (ospf->stub_router_startup_time
			    != OSPF_STUB_ROUTER_UNCONFIGURED)
				json_object_int_add(
					json, "postStartEnabledMsecs",
					ospf->stub_router_startup_time / 1000);
			if (ospf->stub_router_shutdown_time
			    != OSPF_STUB_ROUTER_UNCONFIGURED)
				json_object_int_add(
					json, "preShutdownEnabledMsecs",
					ospf->stub_router_shutdown_time / 1000);
		} else {
			vty_out(vty,
				" Stub router advertisement is configured%s",
				VTY_NEWLINE);
			if (ospf->stub_router_startup_time
			    != OSPF_STUB_ROUTER_UNCONFIGURED)
				vty_out(vty,
					"   Enabled for %us after start-up%s",
					ospf->stub_router_startup_time,
					VTY_NEWLINE);
			if (ospf->stub_router_shutdown_time
			    != OSPF_STUB_ROUTER_UNCONFIGURED)
				vty_out(vty,
					"   Enabled for %us prior to full shutdown%s",
					ospf->stub_router_shutdown_time,
					VTY_NEWLINE);
		}
	}

	/* Show SPF timers. */
	if (use_json) {
		json_object_int_add(json, "spfScheduleDelayMsecs",
				    ospf->spf_delay);
		json_object_int_add(json, "holdtimeMinMsecs",
				    ospf->spf_holdtime);
		json_object_int_add(json, "holdtimeMaxMsecs",
				    ospf->spf_max_holdtime);
		json_object_int_add(json, "holdtimeMultplier",
				    ospf->spf_hold_multiplier);
	} else {
		vty_out(vty,
			" Initial SPF scheduling delay %d millisec(s)%s"
			" Minimum hold time between consecutive SPFs %d millisec(s)%s"
			" Maximum hold time between consecutive SPFs %d millisec(s)%s"
			" Hold time multiplier is currently %d%s",
			ospf->spf_delay, VTY_NEWLINE, ospf->spf_holdtime,
			VTY_NEWLINE, ospf->spf_max_holdtime, VTY_NEWLINE,
			ospf->spf_hold_multiplier, VTY_NEWLINE);
	}

	if (use_json) {
		if (ospf->ts_spf.tv_sec || ospf->ts_spf.tv_usec) {
			long time_store = 0;

			time_store =
				monotime_since(&ospf->ts_spf, NULL) / 1000LL;
			json_object_int_add(json, "spfLastExecutedMsecs",
					    time_store);

			time_store = (1000 * ospf->ts_spf_duration.tv_sec)
				     + (ospf->ts_spf_duration.tv_usec / 1000);
			json_object_int_add(json, "spfLastDurationMsecs",
					    time_store);
		} else
			json_object_boolean_true_add(json, "spfHasNotRun");
	} else {
		vty_out(vty, " SPF algorithm ");
		if (ospf->ts_spf.tv_sec || ospf->ts_spf.tv_usec) {
			monotime_since(&ospf->ts_spf, &result);
			vty_out(vty, "last executed %s ago%s",
				ospf_timeval_dump(&result, timebuf,
						  sizeof(timebuf)),
				VTY_NEWLINE);
			vty_out(vty, " Last SPF duration %s%s",
				ospf_timeval_dump(&ospf->ts_spf_duration,
						  timebuf, sizeof(timebuf)),
				VTY_NEWLINE);
		} else
			vty_out(vty, "has not been run%s", VTY_NEWLINE);
	}

	if (use_json) {
		if (ospf->t_spf_calc) {
			long time_store;
			time_store =
				monotime_until(&ospf->t_spf_calc->u.sands, NULL)
				/ 1000LL;
			json_object_int_add(json, "spfTimerDueInMsecs",
					    time_store);
		}

		json_object_int_add(json, "lsaMinIntervalMsecs",
				    ospf->min_ls_interval);
		json_object_int_add(json, "lsaMinArrivalMsecs",
				    ospf->min_ls_arrival);
		/* Show write multiplier values */
		json_object_int_add(json, "writeMultiplier",
				    ospf->write_oi_count);
		/* Show refresh parameters. */
		json_object_int_add(json, "refreshTimerMsecs",
				    ospf->lsa_refresh_interval * 1000);
	} else {
		vty_out(vty, " SPF timer %s%s%s",
			(ospf->t_spf_calc ? "due in " : "is "),
			ospf_timer_dump(ospf->t_spf_calc, timebuf,
					sizeof(timebuf)),
			VTY_NEWLINE);

		vty_out(vty, " LSA minimum interval %d msecs%s",
			ospf->min_ls_interval, VTY_NEWLINE);
		vty_out(vty, " LSA minimum arrival %d msecs%s",
			ospf->min_ls_arrival, VTY_NEWLINE);

		/* Show write multiplier values */
		vty_out(vty, " Write Multiplier set to %d %s",
			ospf->write_oi_count, VTY_NEWLINE);

		/* Show refresh parameters. */
		vty_out(vty, " Refresh timer %d secs%s",
			ospf->lsa_refresh_interval, VTY_NEWLINE);
	}

	/* Show ABR/ASBR flags. */
	if (CHECK_FLAG(ospf->flags, OSPF_FLAG_ABR)) {
		if (use_json)
			json_object_string_add(
				json, "abrType",
				ospf_abr_type_descr_str[ospf->abr_type]);
		else
			vty_out(vty,
				" This router is an ABR, ABR type is: %s%s",
				ospf_abr_type_descr_str[ospf->abr_type],
				VTY_NEWLINE);
	}
	if (CHECK_FLAG(ospf->flags, OSPF_FLAG_ASBR)) {
		if (use_json)
			json_object_string_add(
				json, "asbrRouter",
				"injectingExternalRoutingInformation");
		else
			vty_out(vty,
				" This router is an ASBR "
				"(injecting external routing information)%s",
				VTY_NEWLINE);
	}

	/* Show Number of AS-external-LSAs. */
	if (use_json) {
		json_object_int_add(
			json, "lsaExternalCounter",
			ospf_lsdb_count(ospf->lsdb, OSPF_AS_EXTERNAL_LSA));
		json_object_int_add(
			json, "lsaExternalChecksum",
			ospf_lsdb_checksum(ospf->lsdb, OSPF_AS_EXTERNAL_LSA));
	} else {
		vty_out(vty,
			" Number of external LSA %ld. Checksum Sum 0x%08x%s",
			ospf_lsdb_count(ospf->lsdb, OSPF_AS_EXTERNAL_LSA),
			ospf_lsdb_checksum(ospf->lsdb, OSPF_AS_EXTERNAL_LSA),
			VTY_NEWLINE);
	}

	if (use_json) {
		json_object_int_add(
			json, "lsaAsopaqueCounter",
			ospf_lsdb_count(ospf->lsdb, OSPF_OPAQUE_AS_LSA));
		json_object_int_add(
			json, "lsaAsOpaqueChecksum",
			ospf_lsdb_checksum(ospf->lsdb, OSPF_OPAQUE_AS_LSA));
	} else {
		vty_out(vty,
			" Number of opaque AS LSA %ld. Checksum Sum 0x%08x%s",
			ospf_lsdb_count(ospf->lsdb, OSPF_OPAQUE_AS_LSA),
			ospf_lsdb_checksum(ospf->lsdb, OSPF_OPAQUE_AS_LSA),
			VTY_NEWLINE);
	}

	/* Show number of areas attached. */
	if (use_json)
		json_object_int_add(json, "attachedAreaCounter",
				    listcount(ospf->areas));
	else
		vty_out(vty, " Number of areas attached to this router: %d%s",
			listcount(ospf->areas), VTY_NEWLINE);

	if (CHECK_FLAG(ospf->config, OSPF_LOG_ADJACENCY_CHANGES)) {
		if (CHECK_FLAG(ospf->config, OSPF_LOG_ADJACENCY_DETAIL)) {
			if (use_json)
				json_object_boolean_true_add(
					json, "adjacencyChangesLoggedAll");
			else
				vty_out(vty,
					" All adjacency changes are logged%s",
					VTY_NEWLINE);
		} else {
			if (use_json)
				json_object_boolean_true_add(
					json, "adjacencyChangesLogged");
			else
				vty_out(vty, " Adjacency changes are logged%s",
					VTY_NEWLINE);
		}
	}
	/* Show each area status. */
	for (ALL_LIST_ELEMENTS(ospf->areas, node, nnode, area))
		show_ip_ospf_area(vty, area, json_areas, use_json);

	if (use_json) {
		json_object_object_add(json, "areas", json_areas);
		vty_out(vty, "%s%s", json_object_to_json_string_ext(
					     json, JSON_C_TO_STRING_PRETTY),
			VTY_NEWLINE);
		json_object_free(json);
	} else
		vty_out(vty, "%s", VTY_NEWLINE);

	return CMD_SUCCESS;
}

DEFUN (show_ip_ospf,
       show_ip_ospf_cmd,
       "show ip ospf [json]",
       SHOW_STR
       IP_STR
       "OSPF information\n"
       JSON_STR)
{
	struct ospf *ospf;
	u_char uj = use_json(argc, argv);

	if ((ospf = ospf_lookup()) == NULL || !ospf->oi_running)
	{
            vty_out(vty, "%% OSPF routing process not enabled.%s", VTY_NEWLINE);
            return CMD_SUCCESS;
	}

	return (show_ip_ospf_common(vty, ospf, uj));
}

DEFUN (show_ip_ospf_instance,
       show_ip_ospf_instance_cmd,
       "show ip ospf (1-65535) [json]",
       SHOW_STR
       IP_STR
       "OSPF information\n"
       "Instance ID\n"
       JSON_STR)
{
	int idx_number = 3;
	struct ospf *ospf;
	u_short instance = 0;
	u_char uj = use_json(argc, argv);

	VTY_GET_INTEGER("Instance", instance, argv[idx_number]->arg);
	if ((ospf = ospf_lookup_instance(instance)) == NULL
	    || !ospf->oi_running)
		return CMD_SUCCESS;

	return (show_ip_ospf_common(vty, ospf, uj));
}

  /* SYSTEM MODIFIED by wangjj for fix bug 43861, 2017-05-24 */
#ifdef _CENTEC_
/* OSPF authentication checking function */
static int
show_ospf_auth_type (struct ospf_interface *oi)
{
  int auth_type;

  if (OSPF_IF_PARAM (oi, auth_type) == OSPF_AUTH_NOTSET)
    auth_type = oi->area->auth_type;
  else
    auth_type = OSPF_IF_PARAM (oi, auth_type);

/* modified by liwh for bug 44144, 2017-08-11 */
#ifndef _CENTEC_
  /* Handle case where MD5 key list is not configured aka Cisco */
  if (auth_type == OSPF_AUTH_CRYPTOGRAPHIC &&
      list_isempty (OSPF_IF_PARAM (oi, auth_crypt)))
    return OSPF_AUTH_NULL;
#endif
/*liwh end */

  return auth_type;

}
#endif

static void show_ip_ospf_interface_sub(struct vty *vty, struct ospf *ospf,
				       struct interface *ifp,
				       json_object *json_interface_sub,
				       u_char use_json)
{
	int is_up;
	struct ospf_neighbor *nbr;
	struct route_node *rn;
	uint32_t bandwidth = ifp->bandwidth ? ifp->bandwidth : ifp->speed;

	/* Is interface up? */
	if (use_json) {
		is_up = if_is_operative(ifp);
		if (is_up)
			json_object_boolean_true_add(json_interface_sub,
						     "ifUp");
		else
			json_object_boolean_false_add(json_interface_sub,
						      "ifDown");

		json_object_int_add(json_interface_sub, "ifIndex",
				    ifp->ifindex);
		json_object_int_add(json_interface_sub, "mtuBytes", ifp->mtu);
		json_object_int_add(json_interface_sub, "bandwidthMbit",
				    bandwidth);
		json_object_string_add(json_interface_sub, "ifFlags",
				       if_flag_dump(ifp->flags));
	} else {
		vty_out(vty, "%s is %s%s", ifp->name,
			((is_up = if_is_operative(ifp)) ? "up" : "down"),
			VTY_NEWLINE);
		vty_out(vty, "  ifindex %u, MTU %u bytes, BW %u Mbit %s%s",
			ifp->ifindex, ifp->mtu, bandwidth,
			if_flag_dump(ifp->flags), VTY_NEWLINE);
	}

	/* Is interface OSPF enabled? */
	if (use_json) {
		if (ospf_oi_count(ifp) == 0) {
			json_object_boolean_false_add(json_interface_sub,
						      "ospfEnabled");
			return;
		} else if (!is_up) {
			json_object_boolean_false_add(json_interface_sub,
						      "ospfRunning");
			return;
		} else
			json_object_boolean_true_add(json_interface_sub,
						     "ospfEnabled");
	} else {
		if (ospf_oi_count(ifp) == 0) {
			vty_out(vty, "  OSPF not enabled on this interface%s",
				VTY_NEWLINE);
			return;
		} else if (!is_up) {
			vty_out(vty,
				"  OSPF is enabled, but not running on this interface%s",
				VTY_NEWLINE);
			return;
		}
	}

	for (rn = route_top(IF_OIFS(ifp)); rn; rn = route_next(rn)) {
		struct ospf_interface *oi = rn->info;

		if (oi == NULL)
			continue;
		/* djd: for vrrp virtual IP, it is not real unnumbered */
		if (CHECK_FLAG(oi->connected->flags, ZEBRA_IFA_UNNUMBERED) && (oi->type == OSPF_IFTYPE_POINTOPOINT)) {
			if (use_json)
				json_object_boolean_true_add(json_interface_sub,
							     "ifUnnumbered");
			else
				vty_out(vty, "  This interface is UNNUMBERED,");
		} else {
			/* Show OSPF interface information. */
			if (use_json) {
				json_object_string_add(
					json_interface_sub, "ipAddress",
					inet_ntoa(oi->address->u.prefix4));
				json_object_int_add(json_interface_sub,
						    "ipAddressPrefixlen",
						    oi->address->prefixlen);
			} else
				vty_out(vty, "  Internet Address %s/%d,",
					inet_ntoa(oi->address->u.prefix4),
					oi->address->prefixlen);

			if (oi->connected->destination
			    || oi->type == OSPF_IFTYPE_VIRTUALLINK) {
				struct in_addr *dest;
				const char *dstr;

				if (CONNECTED_PEER(oi->connected)
				    || oi->type == OSPF_IFTYPE_VIRTUALLINK)
					dstr = "Peer";
				else
					dstr = "Broadcast";

				/* For Vlinks, showing the peer address is
		   * probably more
		   *  *  *  *                * informative than the local
		   * interface that is being used
		   *   *   *   *                               */
				if (oi->type == OSPF_IFTYPE_VIRTUALLINK)
					dest = &oi->vl_data->peer_addr;
				else
					dest = &oi->connected->destination->u
							.prefix4;

				if (use_json) {
					json_object_string_add(
						json_interface_sub,
						"ospfIfType", dstr);
					if (oi->type == OSPF_IFTYPE_VIRTUALLINK)
						json_object_string_add(
							json_interface_sub,
							"vlinkPeer",
							inet_ntoa(*dest));
					else
						json_object_string_add(
							json_interface_sub,
							"localIfUsed",
							inet_ntoa(*dest));
				} else
					vty_out(vty, " %s %s,", dstr,
						inet_ntoa(*dest));
			}
		}
		if (use_json) {
			json_object_string_add(json_interface_sub, "area",
					       ospf_area_desc_string(oi->area));
			if (OSPF_IF_PARAM(oi, mtu_ignore))
				json_object_boolean_true_add(
					json_interface_sub,
					"mtuMismatchDetect");
			json_object_string_add(json_interface_sub, "routerId",
					       inet_ntoa(ospf->router_id));
			json_object_string_add(json_interface_sub,
					       "networkType",
					       ospf_network_type_str[oi->type]);
			json_object_int_add(json_interface_sub, "cost",
					    oi->output_cost);
			json_object_int_add(
				json_interface_sub, "transmitDelayMsecs",
				1000 / OSPF_IF_PARAM(oi, transmit_delay));
			json_object_string_add(json_interface_sub, "state",
					       lookup_msg(ospf_ism_state_msg,
							  oi->state, NULL));
			json_object_int_add(json_interface_sub, "priority",
					    PRIORITY(oi));
		} else {
			vty_out(vty, " Area %s%s",
				ospf_area_desc_string(oi->area), VTY_NEWLINE);

			vty_out(vty, "  MTU mismatch detection:%s%s",
				OSPF_IF_PARAM(oi, mtu_ignore) ? "disabled"
							      : "enabled",
				VTY_NEWLINE);

			vty_out(vty,
				"  Router ID %s, Network Type %s, Cost: %d%s",
				inet_ntoa(ospf->router_id),
				ospf_network_type_str[oi->type],
				oi->output_cost, VTY_NEWLINE);

			vty_out(vty,
				"  Transmit Delay is %d sec, State %s, Priority %d%s",
				OSPF_IF_PARAM(oi, transmit_delay),
				lookup_msg(ospf_ism_state_msg, oi->state, NULL),
				PRIORITY(oi), VTY_NEWLINE);
		}

		/* Show DR information. */
		if (DR(oi).s_addr == 0) {
			if (!use_json)
				vty_out(vty,
					"  No backup designated router on this network%s",
					VTY_NEWLINE);
		} else {

                      /* modified by liwh for bug 47265, 2018-09-10 */
                      nbr = ospf_nbr_lookup_by_addr(oi->nbrs, &DR(oi));
                      if (nbr == NULL) {
                          if (!use_json)
                              vty_out(vty,
                                  "  No designated router on this network%s",
                                  VTY_NEWLINE);
                      } else {
                          vty_out(vty,
                              "  Designated Router (ID) %s,",
                              inet_ntoa(nbr->router_id));
                              vty_out(vty, " Interface Address %s%s",
                              inet_ntoa(nbr->address.u.prefix4), VTY_NEWLINE);
                      }
                      /* liwh end */
        
			nbr = ospf_nbr_lookup_by_addr(oi->nbrs, &BDR(oi));
			if (nbr == NULL) {
				if (!use_json)
					vty_out(vty,
						"  No backup designated router on this network%s",
						VTY_NEWLINE);
			} else {
				if (use_json) {
					json_object_string_add(
						json_interface_sub, "bdrId",
						inet_ntoa(nbr->router_id));
					json_object_string_add(
						json_interface_sub,
						"bdrAddress",
						inet_ntoa(nbr->address.u
								  .prefix4));
				} else {
					vty_out(vty,
						"  Backup Designated Router (ID) %s,",
						inet_ntoa(nbr->router_id));
					vty_out(vty, " Interface Address %s%s",
						inet_ntoa(
							nbr->address.u.prefix4),
						VTY_NEWLINE);
				}
			}
		}

		/* Next network-LSA sequence number we'll use, if we're elected
		 * DR */
		if (oi->params
		    && ntohl(oi->params->network_lsa_seqnum)
			       != OSPF_INITIAL_SEQUENCE_NUMBER) {
			if (use_json)
				json_object_int_add(
					json_interface_sub,
					"networkLsaSequence",
					ntohl(oi->params->network_lsa_seqnum));
			else
				vty_out(vty,
					"  Saved Network-LSA sequence number 0x%x%s",
					ntohl(oi->params->network_lsa_seqnum),
					VTY_NEWLINE);
		}

		if (use_json) {
			if (OI_MEMBER_CHECK(oi, MEMBER_ALLROUTERS)
			    || OI_MEMBER_CHECK(oi, MEMBER_DROUTERS)) {
				if (OI_MEMBER_CHECK(oi, MEMBER_ALLROUTERS))
					json_object_boolean_true_add(
						json_interface_sub,
						"mcastMemberOspfAllRouters");
				if (OI_MEMBER_CHECK(oi, MEMBER_DROUTERS))
					json_object_boolean_true_add(
						json_interface_sub,
						"mcastMemberOspfDesignatedRouters");
			}
		} else {
			vty_out(vty, "  Multicast group memberships:");
			if (OI_MEMBER_CHECK(oi, MEMBER_ALLROUTERS)
			    || OI_MEMBER_CHECK(oi, MEMBER_DROUTERS)) {
				if (OI_MEMBER_CHECK(oi, MEMBER_ALLROUTERS))
					vty_out(vty, " OSPFAllRouters");
				if (OI_MEMBER_CHECK(oi, MEMBER_DROUTERS))
					vty_out(vty, " OSPFDesignatedRouters");
			} else
				vty_out(vty, " <None>");
			vty_out(vty, "%s", VTY_NEWLINE);
		}

		if (use_json) {
			if (OSPF_IF_PARAM(oi, fast_hello) == 0)
				json_object_int_add(
					json_interface_sub, "timerMsecs",
					1000 / OSPF_IF_PARAM(oi, v_hello));
			else
				json_object_int_add(
					json_interface_sub, "timerMsecs",
					1000 / OSPF_IF_PARAM(oi, fast_hello));
			json_object_int_add(json_interface_sub,
					    "timerDeadMsecs",
					    1000 / OSPF_IF_PARAM(oi, v_wait));
			json_object_int_add(json_interface_sub,
					    "timerWaitMsecs",
					    1000 / OSPF_IF_PARAM(oi, v_wait));
			json_object_int_add(
				json_interface_sub, "timerRetransmit",
				1000 / OSPF_IF_PARAM(oi, retransmit_interval));
		} else {
			vty_out(vty, "  Timer intervals configured,");
			vty_out(vty, " Hello ");
			if (OSPF_IF_PARAM(oi, fast_hello) == 0)
				vty_out(vty, "%ds,",
					OSPF_IF_PARAM(oi, v_hello));
			else
				vty_out(vty, "%dms,",
					1000 / OSPF_IF_PARAM(oi, fast_hello));
/* modified by liwh for bug 47369, 2018-05-23 */
#ifdef _CENTEC_
                    vty_out(vty, " Dead %ds, Wait %ds, Retransmit %ds\n",
#else
			vty_out(vty, " Dead %ds, Wait %ds, Retransmit %d%s",
#endif
/*liwh end */
				OSPF_IF_PARAM(oi, v_wait),
				OSPF_IF_PARAM(oi, v_wait),
				OSPF_IF_PARAM(oi, retransmit_interval),
				VTY_NEWLINE);
		}

		if (OSPF_IF_PASSIVE_STATUS(oi) == OSPF_IF_ACTIVE) {
			char timebuf[OSPF_TIME_DUMP_SIZE];
			if (use_json) {
				long time_store = 0;
				if (oi->t_hello)
					time_store =
						monotime_until(
							&oi->t_hello->u.sands,
							NULL)
						/ 1000LL;
				json_object_int_add(json_interface_sub,
						    "timerHelloInMsecs",
						    time_store);
			} else
				vty_out(vty, "    Hello due in %s%s",
					ospf_timer_dump(oi->t_hello, timebuf,
							sizeof(timebuf)),
					VTY_NEWLINE);
		} else /* passive-interface is set */
		{
			if (use_json)
				json_object_boolean_true_add(
					json_interface_sub,
					"timerPassiveIface");
			else
				vty_out(vty,
					"    No Hellos (Passive interface)%s",
					VTY_NEWLINE);
		}

		if (use_json) {
			json_object_int_add(json_interface_sub, "nbrCount",
					    ospf_nbr_count(oi, 0));
			json_object_int_add(json_interface_sub,
					    "nbrAdjacentCount",
					    ospf_nbr_count(oi, NSM_Full));
		} else
			vty_out(vty,
				"  Neighbor Count is %d, Adjacent neighbor count is %d%s",
				ospf_nbr_count(oi, 0),
				ospf_nbr_count(oi, NSM_Full), VTY_NEWLINE);
		ospf_bfd_interface_show(vty, ifp, json_interface_sub, use_json);

  /* SYSTEM MODIFIED by wangjj for fix bug 43861, 2017-05-24 */
#ifdef _CENTEC_
        u_int16_t iface_auth_type = 0;
        struct crypt_key *ck = NULL;

        iface_auth_type = show_ospf_auth_type (oi);
        if (OSPF_AUTH_SIMPLE == iface_auth_type)
        {
            if (use_json)
            {
                /* TODO for json */
            }
            else
            {
                vty_out (vty, "  Simple password authentication enabled%s", VTY_NEWLINE);
            }
        }
        else if (OSPF_AUTH_CRYPTOGRAPHIC == iface_auth_type)
        {
            if (use_json)
            {
                /* TODO for json */
            }
            else
            {
                vty_out (vty, "  Message digest authentication enabled%s", VTY_NEWLINE);
            }

            /* modified by liwh for bug 45361, 2017-10-19 */
            if (!list_isempty (OSPF_IF_PARAM (oi, auth_crypt)))
            /* liwh end */
            {
                ck = listgetdata (listtail(OSPF_IF_PARAM (oi,auth_crypt)));
            }
            
            if (NULL != ck)
            {
                if (use_json)
                {
                    /* TODO for json */
                }
                else
                {
                    vty_out (vty, "      Youngest key id is %d%s", ck->key_id, VTY_NEWLINE);
                }
            }
            else
            {
                if (use_json)
                {
                    /* TODO for json */
                }
                else
                {
/* modified by liwh for bug 47167, 2018-09-11 */
#ifdef _CENTEC_
                    vty_out (vty, "      No key configured, using default key id 0%s", VTY_NEWLINE);
#else
                    vty_out (vty, "      No key configured%s", VTY_NEWLINE);
#endif
/*liwh end */
                }
            }
        }
#endif /*_CENTEC_*/

	}
}

static int show_ip_ospf_interface_common(struct vty *vty, struct ospf *ospf,
					 int argc, struct cmd_token **argv,
					 int iface_argv, u_char use_json)
{
	struct interface *ifp;
	struct listnode *node;
	json_object *json = NULL;
	json_object *json_interface_sub = NULL;

	if (use_json) {
		json = json_object_new_object();
	}

	if (ospf->instance) {
		if (use_json)
			json_object_int_add(json, "ospfInstance",
					    ospf->instance);
		else
			vty_out(vty, "%sOSPF Instance: %d%s%s", VTY_NEWLINE,
				ospf->instance, VTY_NEWLINE, VTY_NEWLINE);
	}

	if (argc == iface_argv) {
		/* Show All Interfaces.*/
		for (ALL_LIST_ELEMENTS_RO(vrf_iflist(VRF_DEFAULT), node, ifp)) {
			if (ospf_oi_count(ifp)) {
				if (use_json)
					json_interface_sub =
						json_object_new_object();

				show_ip_ospf_interface_sub(vty, ospf, ifp,
							   json_interface_sub,
							   use_json);

				if (use_json)
					json_object_object_add(
						json, ifp->name,
						json_interface_sub);
			}
		}
	} else {
		/* Interface name is specified. */
		if ((ifp = if_lookup_by_name(argv[iface_argv]->arg,
					     VRF_DEFAULT))
		    == NULL) {
			if (use_json)
				json_object_boolean_true_add(json,
							     "noSuchIface");
			else
				vty_out(vty, "No such interface name%s",
					VTY_NEWLINE);
		} else {
			if (use_json)
				json_interface_sub = json_object_new_object();

			show_ip_ospf_interface_sub(
				vty, ospf, ifp, json_interface_sub, use_json);

			if (use_json)
				json_object_object_add(json, ifp->name,
						       json_interface_sub);
		}
	}

	if (use_json) {
		vty_out(vty, "%s%s", json_object_to_json_string_ext(
					     json, JSON_C_TO_STRING_PRETTY),
			VTY_NEWLINE);
		json_object_free(json);
	} else
		vty_out(vty, "%s", VTY_NEWLINE);

	return CMD_SUCCESS;
}

DEFUN (show_ip_ospf_interface,
       show_ip_ospf_interface_cmd,
       "show ip ospf interface [INTERFACE] [json]",
       SHOW_STR
       IP_STR
       "OSPF information\n"
       "Interface information\n"
       "Interface name\n"
       JSON_STR)
{
	struct ospf *ospf;
	u_char uj = use_json(argc, argv);

	if ((ospf = ospf_lookup()) == NULL || !ospf->oi_running)
		return CMD_SUCCESS;

	if (uj)
		argc--;

	return show_ip_ospf_interface_common(vty, ospf, argc, argv, 4, uj);
}

DEFUN (show_ip_ospf_instance_interface,
       show_ip_ospf_instance_interface_cmd,
       "show ip ospf (1-65535) interface [INTERFACE] [json]",
       SHOW_STR
       IP_STR
       "OSPF information\n"
       "Instance ID\n"
       "Interface information\n"
       "Interface name\n"
       JSON_STR)
{
	int idx_number = 3;
	struct ospf *ospf;
	u_short instance = 0;
	u_char uj = use_json(argc, argv);

	VTY_GET_INTEGER("Instance", instance, argv[idx_number]->arg);
	if ((ospf = ospf_lookup_instance(instance)) == NULL
	    || !ospf->oi_running)
		return CMD_SUCCESS;

	if (uj)
		argc--;

	return show_ip_ospf_interface_common(vty, ospf, argc, argv, 5, uj);
}

static void show_ip_ospf_neighbour_header(struct vty *vty)
{
/* modified by liwh for bug 43680, 2017-07-24 */
#ifdef _CENTEC_
  //vty_out (vty, "%s%-15s %3s %-15s %9s %-15s %-20s %5s %5s %5s%s",
  vty_out (vty, "%s%-15s %3s %-15s %9s %-15s %-26s %5s %5s %5s%s",
#endif
/*liwh end */
		VTY_NEWLINE, "Neighbor ID", "Pri", "State", "Dead Time",
		"Address", "Interface", "RXmtL", "RqstL", "DBsmL", VTY_NEWLINE);
}

static void show_ip_ospf_neighbor_sub(struct vty *vty,
				      struct ospf_interface *oi,
				      json_object *json, u_char use_json)
{
	struct route_node *rn;
	struct ospf_neighbor *nbr;
	char msgbuf[16];
	char timebuf[OSPF_TIME_DUMP_SIZE];
	json_object *json_neighbor = NULL;

	for (rn = route_top(oi->nbrs); rn; rn = route_next(rn)) {
		if ((nbr = rn->info)) {
			/* Do not show myself. */
			if (nbr != oi->nbr_self) {
				/* Down state is not shown. */
				if (nbr->state != NSM_Down) {
					if (use_json) {
						json_neighbor =
							json_object_new_object();
						ospf_nbr_state_message(
							nbr, msgbuf, 16);

						long time_store;

						time_store =
							monotime_until(
								&nbr->t_inactivity
									 ->u
									 .sands,
								NULL)
							/ 1000LL;

						json_object_int_add(
							json_neighbor,
							"priority",
							nbr->priority);
						json_object_string_add(
							json_neighbor, "state",
							msgbuf);
						json_object_int_add(
							json_neighbor,
							"deadTimeMsecs",
							time_store);
						json_object_string_add(
							json_neighbor,
							"address",
							inet_ntoa(nbr->src));
						json_object_string_add(
							json_neighbor,
							"ifaceName",
							IF_NAME(oi));
						json_object_int_add(
							json_neighbor,
							"retransmitCounter",
							ospf_ls_retransmit_count(
								nbr));
						json_object_int_add(
							json_neighbor,
							"requestCounter",
							ospf_ls_request_count(
								nbr));
						json_object_int_add(
							json_neighbor,
							"dbSummaryCounter",
							ospf_db_summary_count(
								nbr));
						if (nbr->state == NSM_Attempt
						    && nbr->router_id.s_addr
							       == 0)
							json_object_object_add(
								json,
								"neighbor",
								json_neighbor);
						else
							json_object_object_add(
								json,
								inet_ntoa(
									nbr->router_id),
								json_neighbor);
					} else {
						ospf_nbr_state_message(
							nbr, msgbuf, 16);

/* modified by liwh for bug 44115, 2017-06-20 */
#ifdef _CENTEC_
                      if (nbr->state == NSM_Attempt && nbr->router_id.s_addr == 0)
                        vty_out (vty, "%-15s %-3d %-15s ",
                                 "-", nbr->priority,
                                 msgbuf);
                      else
                        vty_out (vty, "%-15s %-3d %-15s ",
                                 inet_ntoa (nbr->router_id), nbr->priority,
                                 msgbuf);
                      
                      vty_out (vty, "%-9s ",
                               ospf_timer_dump (nbr->t_inactivity, timebuf,
                                                sizeof(timebuf)));
                      vty_out (vty, "%-15s ", inet_ntoa (nbr->src));
                      /* modified by liwh for bug 43680, 2017-07-24 */
                      //vty_out (vty, "%-20s %-5ld %-5ld %-5d%s",
                      vty_out (vty, "%-26s %-5ld %-5ld %-5d%s",
                      /*liwh end */
                               IF_NAME (oi), ospf_ls_retransmit_count (nbr),
                               ospf_ls_request_count (nbr), ospf_db_summary_count (nbr),
                               VTY_NEWLINE);

#else
						if (nbr->state == NSM_Attempt
						    && nbr->router_id.s_addr
							       == 0)
							vty_out(vty,
								"%-15s %3d %-15s ",
								"-",
								nbr->priority,
								msgbuf);
						else
							vty_out(vty,
								"%-15s %3d %-15s ",
								inet_ntoa(
									nbr->router_id),
								nbr->priority,
								msgbuf);

						vty_out(vty, "%9s ",
							ospf_timer_dump(
								nbr->t_inactivity,
								timebuf,
								sizeof(timebuf)));
						vty_out(vty, "%-15s ",
							inet_ntoa(nbr->src));
						vty_out(vty,
							"%-20s %5ld %5ld %5d%s",
							IF_NAME(oi),
							ospf_ls_retransmit_count(
								nbr),
							ospf_ls_request_count(
								nbr),
							ospf_db_summary_count(
								nbr),
							VTY_NEWLINE);
#endif
/* liwh end */
					}
				}
			}
		}
	}
}

static int show_ip_ospf_neighbor_common(struct vty *vty, struct ospf *ospf,
					u_char use_json)
{
	struct ospf_interface *oi;
	struct listnode *node;
	json_object *json = NULL;

	if (use_json)
		json = json_object_new_object();
	else
		show_ip_ospf_neighbour_header(vty);

	if (ospf->instance) {
		if (use_json)
			json_object_int_add(json, "ospfInstance",
					    ospf->instance);
		else
			vty_out(vty, "%sOSPF Instance: %d%s%s", VTY_NEWLINE,
				ospf->instance, VTY_NEWLINE, VTY_NEWLINE);
	}

	for (ALL_LIST_ELEMENTS_RO(ospf->oiflist, node, oi))
		show_ip_ospf_neighbor_sub(vty, oi, json, use_json);

	if (use_json) {
		vty_out(vty, "%s%s", json_object_to_json_string_ext(
					     json, JSON_C_TO_STRING_PRETTY),
			VTY_NEWLINE);
		json_object_free(json);
	} else
		vty_out(vty, "%s", VTY_NEWLINE);

	return CMD_SUCCESS;
}

DEFUN (show_ip_ospf_neighbor,
       show_ip_ospf_neighbor_cmd,
       "show ip ospf neighbor [json]",
       SHOW_STR
       IP_STR
       "OSPF information\n"
       "Neighbor list\n"
       JSON_STR)
{
	struct ospf *ospf;
	u_char uj = use_json(argc, argv);

	if ((ospf = ospf_lookup()) == NULL || !ospf->oi_running)
		return CMD_SUCCESS;

	return show_ip_ospf_neighbor_common(vty, ospf, uj);
}


DEFUN (show_ip_ospf_instance_neighbor,
       show_ip_ospf_instance_neighbor_cmd,
       "show ip ospf (1-65535) neighbor [json]",
       SHOW_STR
       IP_STR
       "OSPF information\n"
       "Instance ID\n"
       "Neighbor list\n"
       JSON_STR)
{
	int idx_number = 3;
	struct ospf *ospf;
	u_short instance = 0;
	u_char uj = use_json(argc, argv);

	VTY_GET_INTEGER("Instance", instance, argv[idx_number]->arg);
	if ((ospf = ospf_lookup_instance(instance)) == NULL
	    || !ospf->oi_running)
		return CMD_SUCCESS;

	return show_ip_ospf_neighbor_common(vty, ospf, uj);
}

static int show_ip_ospf_neighbor_all_common(struct vty *vty, struct ospf *ospf,
					    u_char use_json)
{
	struct listnode *node;
	struct ospf_interface *oi;
	json_object *json = NULL;
	json_object *json_neighbor_sub = NULL;

	if (use_json) {
		json = json_object_new_object();
		json_neighbor_sub = json_object_new_object();
	} else
		show_ip_ospf_neighbour_header(vty);

	if (ospf->instance) {
		if (use_json)
			json_object_int_add(json, "ospfInstance",
					    ospf->instance);
		else
			vty_out(vty, "%sOSPF Instance: %d%s%s", VTY_NEWLINE,
				ospf->instance, VTY_NEWLINE, VTY_NEWLINE);
	}

	for (ALL_LIST_ELEMENTS_RO(ospf->oiflist, node, oi)) {
		struct listnode *nbr_node;
		struct ospf_nbr_nbma *nbr_nbma;

		show_ip_ospf_neighbor_sub(vty, oi, json, use_json);

		/* print Down neighbor status */
		for (ALL_LIST_ELEMENTS_RO(oi->nbr_nbma, nbr_node, nbr_nbma)) {
			if (nbr_nbma->nbr == NULL
			    || nbr_nbma->nbr->state == NSM_Down) {
				if (use_json) {
					json_object_int_add(json_neighbor_sub,
							    "nbrNbmaPriority",
							    nbr_nbma->priority);
					json_object_boolean_true_add(
						json_neighbor_sub,
						"nbrNbmaDown");
					json_object_string_add(
						json_neighbor_sub,
						"nbrNbmaIfaceName",
						IF_NAME(oi));
					json_object_int_add(
						json_neighbor_sub,
						"nbrNbmaRetransmitCounter", 0);
					json_object_int_add(
						json_neighbor_sub,
						"nbrNbmaRequestCounter", 0);
					json_object_int_add(
						json_neighbor_sub,
						"nbrNbmaDbSummaryCounter", 0);
					json_object_object_add(
						json, inet_ntoa(nbr_nbma->addr),
						json_neighbor_sub);
				} else {
					vty_out(vty, "%-15s %3d %-15s %9s ",
						"-", nbr_nbma->priority, "Down",
						"-");
					vty_out(vty,
						"%-15s %-20s %5d %5d %5d%s",
						inet_ntoa(nbr_nbma->addr),
						IF_NAME(oi), 0, 0, 0,
						VTY_NEWLINE);
				}
			}
		}
	}

	if (use_json) {
		vty_out(vty, "%s%s", json_object_to_json_string_ext(
					     json, JSON_C_TO_STRING_PRETTY),
			VTY_NEWLINE);
		json_object_free(json);
	} else
		vty_out(vty, "%s", VTY_NEWLINE);

	return CMD_SUCCESS;
}

DEFUN (show_ip_ospf_neighbor_all,
       show_ip_ospf_neighbor_all_cmd,
       "show ip ospf neighbor all [json]",
       SHOW_STR
       IP_STR
       "OSPF information\n"
       "Neighbor list\n"
       "include down status neighbor\n"
       JSON_STR)
{
	struct ospf *ospf;
	u_char uj = use_json(argc, argv);

	if ((ospf = ospf_lookup()) == NULL || !ospf->oi_running)
		return CMD_SUCCESS;

	return show_ip_ospf_neighbor_all_common(vty, ospf, uj);
}

DEFUN (show_ip_ospf_instance_neighbor_all,
       show_ip_ospf_instance_neighbor_all_cmd,
       "show ip ospf (1-65535) neighbor all [json]",
       SHOW_STR
       IP_STR
       "OSPF information\n"
       "Instance ID\n"
       "Neighbor list\n"
       "include down status neighbor\n"
       JSON_STR)
{
	int idx_number = 3;
	struct ospf *ospf;
	u_short instance = 0;
	u_char uj = use_json(argc, argv);

	VTY_GET_INTEGER("Instance", instance, argv[idx_number]->arg);
	if ((ospf = ospf_lookup_instance(instance)) == NULL
	    || !ospf->oi_running)
		return CMD_SUCCESS;

	return show_ip_ospf_neighbor_all_common(vty, ospf, uj);
}

static int show_ip_ospf_neighbor_int_common(struct vty *vty, struct ospf *ospf,
					    int arg_base,
					    struct cmd_token **argv,
					    u_char use_json)
{
	struct interface *ifp;
	struct route_node *rn;
	json_object *json = NULL;

	if (use_json)
		json = json_object_new_object();
	else
		show_ip_ospf_neighbour_header(vty);

	if (ospf->instance) {
		if (use_json)
			json_object_int_add(json, "ospfInstance",
					    ospf->instance);
		else
			vty_out(vty, "%sOSPF Instance: %d%s%s", VTY_NEWLINE,
				ospf->instance, VTY_NEWLINE, VTY_NEWLINE);
	}

	ifp = if_lookup_by_name(argv[arg_base]->arg, VRF_DEFAULT);
	if (!ifp) {
		if (use_json)
			json_object_boolean_true_add(json, "noSuchIface");
		else
			vty_out(vty, "No such interface.%s", VTY_NEWLINE);
		return CMD_WARNING;
	}

	for (rn = route_top(IF_OIFS(ifp)); rn; rn = route_next(rn)) {
		struct ospf_interface *oi = rn->info;

		if (oi == NULL)
			continue;

		show_ip_ospf_neighbor_sub(vty, oi, json, use_json);
	}

	if (use_json) {
		vty_out(vty, "%s%s", json_object_to_json_string_ext(
					     json, JSON_C_TO_STRING_PRETTY),
			VTY_NEWLINE);
		json_object_free(json);
	} else
		vty_out(vty, "%s", VTY_NEWLINE);

	return CMD_SUCCESS;
}

DEFUN (show_ip_ospf_neighbor_int,
       show_ip_ospf_neighbor_int_cmd,
       "show ip ospf neighbor IFNAME [json]",
       SHOW_STR
       IP_STR
       "OSPF information\n"
       "Neighbor list\n"
       "Interface name\n"
       JSON_STR)
{
	struct ospf *ospf;
	u_char uj = use_json(argc, argv);

	if ((ospf = ospf_lookup()) == NULL || !ospf->oi_running)
		return CMD_SUCCESS;

	return show_ip_ospf_neighbor_int_common(vty, ospf, 0, argv, uj);
}

DEFUN (show_ip_ospf_instance_neighbor_int,
       show_ip_ospf_instance_neighbor_int_cmd,
       "show ip ospf (1-65535) neighbor IFNAME [json]",
       SHOW_STR
       IP_STR
       "OSPF information\n"
       "Instance ID\n"
       "Neighbor list\n"
       "Interface name\n"
       JSON_STR)
{
	int idx_number = 3;
	struct ospf *ospf;
	u_short instance = 0;
	u_char uj = use_json(argc, argv);

	VTY_GET_INTEGER("Instance", instance, argv[idx_number]->arg);
	if ((ospf = ospf_lookup_instance(instance)) == NULL
	    || !ospf->oi_running)
		return CMD_SUCCESS;

	return show_ip_ospf_neighbor_int_common(vty, ospf, 1, argv, uj);
}

static void show_ip_ospf_nbr_nbma_detail_sub(struct vty *vty,
					     struct ospf_interface *oi,
					     struct ospf_nbr_nbma *nbr_nbma,
					     u_char use_json, json_object *json)
{
	char timebuf[OSPF_TIME_DUMP_SIZE];
	json_object *json_sub = NULL;

	if (use_json)
		json_sub = json_object_new_object();
	else /* Show neighbor ID. */
		vty_out(vty, " Neighbor %s,", "-");

	/* Show interface address. */
	if (use_json)
		json_object_string_add(json_sub, "ifaceAddress",
				       inet_ntoa(nbr_nbma->addr));
	else
		vty_out(vty, " interface address %s%s",
			inet_ntoa(nbr_nbma->addr), VTY_NEWLINE);

	/* Show Area ID. */
	if (use_json) {
		json_object_string_add(json_sub, "areaId",
				       ospf_area_desc_string(oi->area));
		json_object_string_add(json_sub, "iface", IF_NAME(oi));
	} else
		vty_out(vty, "    In the area %s via interface %s%s",
			ospf_area_desc_string(oi->area), IF_NAME(oi),
			VTY_NEWLINE);

	/* Show neighbor priority and state. */
	if (use_json) {
		json_object_int_add(json_sub, "nbrPriority",
				    nbr_nbma->priority);
		json_object_string_add(json_sub, "nbrState", "down");
	} else
		vty_out(vty, "    Neighbor priority is %d, State is %s,",
			nbr_nbma->priority, "Down");

	/* Show state changes. */
	if (use_json)
		json_object_int_add(json_sub, "stateChangeCounter",
				    nbr_nbma->state_change);
	else
		vty_out(vty, " %d state changes%s", nbr_nbma->state_change,
			VTY_NEWLINE);

	/* Show PollInterval */
	if (use_json)
		json_object_int_add(json_sub, "pollInterval", nbr_nbma->v_poll);
	else
		vty_out(vty, "    Poll interval %d%s", nbr_nbma->v_poll,
			VTY_NEWLINE);

	/* Show poll-interval timer. */
	if (use_json) {
		long time_store;
		time_store = monotime_until(&nbr_nbma->t_poll->u.sands, NULL)
			     / 1000LL;
		json_object_int_add(json_sub, "pollIntervalTimerDueMsec",
				    time_store);
	} else
		vty_out(vty, "    Poll timer due in %s%s",
			ospf_timer_dump(nbr_nbma->t_poll, timebuf,
					sizeof(timebuf)),
			VTY_NEWLINE);

	/* Show poll-interval timer thread. */
	if (use_json) {
		if (nbr_nbma->t_poll != NULL)
			json_object_string_add(json_sub,
					       "pollIntervalTimerThread", "on");
	} else
		vty_out(vty, "    Thread Poll Timer %s%s",
			nbr_nbma->t_poll != NULL ? "on" : "off", VTY_NEWLINE);

	if (use_json)
		json_object_object_add(json, "noNbrId", json_sub);
}

static void show_ip_ospf_neighbor_detail_sub(struct vty *vty,
					     struct ospf_interface *oi,
					     struct ospf_neighbor *nbr,
					     u_char use_json, json_object *json)
{
	char timebuf[OSPF_TIME_DUMP_SIZE];
	json_object *json_sub = NULL;

	if (use_json)
		json_sub = json_object_new_object();
	else {
		/* Show neighbor ID. */
		if (nbr->state == NSM_Attempt && nbr->router_id.s_addr == 0)
			vty_out(vty, " Neighbor %s,", "-");
		else
			vty_out(vty, " Neighbor %s,",
				inet_ntoa(nbr->router_id));
	}

	/* Show interface address. */
	if (use_json)
		json_object_string_add(json_sub, "ifaceAddress",
				       inet_ntoa(nbr->address.u.prefix4));
	else
		vty_out(vty, " interface address %s%s",
			inet_ntoa(nbr->address.u.prefix4), VTY_NEWLINE);

	/* Show Area ID. */
	if (use_json) {
		json_object_string_add(json_sub, "areaId",
				       ospf_area_desc_string(oi->area));
		json_object_string_add(json_sub, "ifaceName", oi->ifp->name);
	} else
		vty_out(vty, "    In the area %s via interface %s%s",
			ospf_area_desc_string(oi->area), oi->ifp->name,
			VTY_NEWLINE);

	/* Show neighbor priority and state. */
	if (use_json) {
		json_object_int_add(json_sub, "nbrPriority", nbr->priority);
		json_object_string_add(
			json_sub, "nbrState",
			lookup_msg(ospf_nsm_state_msg, nbr->state, NULL));
	} else
		vty_out(vty, "    Neighbor priority is %d, State is %s,",
			nbr->priority,
			lookup_msg(ospf_nsm_state_msg, nbr->state, NULL));

	/* Show state changes. */
	if (use_json)
		json_object_int_add(json_sub, "stateChangeCounter",
				    nbr->state_change);
	else
		vty_out(vty, " %d state changes%s", nbr->state_change,
			VTY_NEWLINE);

	if (nbr->ts_last_progress.tv_sec || nbr->ts_last_progress.tv_usec) {
		struct timeval res;
		long time_store;

		time_store =
			monotime_since(&nbr->ts_last_progress, &res) / 1000LL;
		if (use_json) {
			json_object_int_add(json_sub, "lastPrgrsvChangeMsec",
					    time_store);
		} else {
			vty_out(vty,
				"    Most recent state change statistics:%s",
				VTY_NEWLINE);
			vty_out(vty, "      Progressive change %s ago%s",
				ospf_timeval_dump(&res, timebuf,
						  sizeof(timebuf)),
				VTY_NEWLINE);
		}
	}

	if (nbr->ts_last_regress.tv_sec || nbr->ts_last_regress.tv_usec) {
		struct timeval res;
		long time_store;

		time_store =
			monotime_since(&nbr->ts_last_regress, &res) / 1000LL;
		if (use_json) {
			json_object_int_add(json_sub,
					    "lastRegressiveChangeMsec",
					    time_store);
			if (nbr->last_regress_str)
				json_object_string_add(
					json_sub, "lastRegressiveChangeReason",
					nbr->last_regress_str);
		} else {
			vty_out(vty,
				"      Regressive change %s ago, due to %s%s",
				ospf_timeval_dump(&res, timebuf,
						  sizeof(timebuf)),
				(nbr->last_regress_str ? nbr->last_regress_str
						       : "??"),
				VTY_NEWLINE);
		}
	}

	/* Show Designated Rotuer ID. */
	if (use_json)
		json_object_string_add(json_sub, "routerDesignatedId",
				       inet_ntoa(nbr->d_router));
	else
		vty_out(vty, "    DR is %s,", inet_ntoa(nbr->d_router));

	/* Show Backup Designated Rotuer ID. */
	if (use_json)
		json_object_string_add(json_sub, "routerDesignatedBackupId",
				       inet_ntoa(nbr->bd_router));
	else
		vty_out(vty, " BDR is %s%s", inet_ntoa(nbr->bd_router),
			VTY_NEWLINE);

	/* Show options. */
	if (use_json) {
		json_object_int_add(json_sub, "optionsCounter", nbr->options);
		json_object_string_add(json_sub, "optionsList",
				       ospf_options_dump(nbr->options));
	} else
		vty_out(vty, "    Options %d %s%s", nbr->options,
			ospf_options_dump(nbr->options), VTY_NEWLINE);

	/* Show Router Dead interval timer. */
	if (use_json) {
		if (nbr->t_inactivity) {
			long time_store;
			time_store = monotime_until(&nbr->t_inactivity->u.sands,
						    NULL)
				     / 1000LL;
			json_object_int_add(json_sub,
					    "routerDeadIntervalTimerDueMsec",
					    time_store);
		} else
			json_object_int_add(
				json_sub, "routerDeadIntervalTimerDueMsec", -1);
	} else
		vty_out(vty, "    Dead timer due in %s%s",
			ospf_timer_dump(nbr->t_inactivity, timebuf,
					sizeof(timebuf)),
			VTY_NEWLINE);

	/* Show Database Summary list. */
	if (use_json)
		json_object_int_add(json_sub, "databaseSummaryListCounter",
				    ospf_db_summary_count(nbr));
	else
		vty_out(vty, "    Database Summary List %d%s",
			ospf_db_summary_count(nbr), VTY_NEWLINE);

	/* Show Link State Request list. */
	if (use_json)
		json_object_int_add(json_sub, "linkStateRequestListCounter",
				    ospf_ls_request_count(nbr));
	else
		vty_out(vty, "    Link State Request List %ld%s",
			ospf_ls_request_count(nbr), VTY_NEWLINE);

	/* Show Link State Retransmission list. */
	if (use_json)
		json_object_int_add(json_sub,
				    "linkStateRetransmissionListCounter",
				    ospf_ls_retransmit_count(nbr));
	else
		vty_out(vty, "    Link State Retransmission List %ld%s",
			ospf_ls_retransmit_count(nbr), VTY_NEWLINE);

	/* Show inactivity timer thread. */
	if (use_json) {
		if (nbr->t_inactivity != NULL)
			json_object_string_add(json_sub,
					       "threadInactivityTimer", "on");
	} else
		vty_out(vty, "    Thread Inactivity Timer %s%s",
			nbr->t_inactivity != NULL ? "on" : "off", VTY_NEWLINE);

	/* Show Database Description retransmission thread. */
	if (use_json) {
		if (nbr->t_db_desc != NULL)
			json_object_string_add(
				json_sub,
				"threadDatabaseDescriptionRetransmission",
				"on");
	} else
		vty_out(vty,
			"    Thread Database Description Retransmision %s%s",
			nbr->t_db_desc != NULL ? "on" : "off", VTY_NEWLINE);

	/* Show Link State Request Retransmission thread. */
	if (use_json) {
		if (nbr->t_ls_req != NULL)
			json_object_string_add(
				json_sub,
				"threadLinkStateRequestRetransmission", "on");
	} else
		vty_out(vty,
			"    Thread Link State Request Retransmission %s%s",
			nbr->t_ls_req != NULL ? "on" : "off", VTY_NEWLINE);

	/* Show Link State Update Retransmission thread. */
	if (use_json) {
		if (nbr->t_ls_upd != NULL)
			json_object_string_add(
				json_sub, "threadLinkStateUpdateRetransmission",
				"on");
	} else
		vty_out(vty,
			"    Thread Link State Update Retransmission %s%s%s",
			nbr->t_ls_upd != NULL ? "on" : "off", VTY_NEWLINE,
			VTY_NEWLINE);

	if (use_json) {
		if (nbr->state == NSM_Attempt && nbr->router_id.s_addr == 0)
			json_object_object_add(json, "noNbrId", json_sub);
		else
			json_object_object_add(json, inet_ntoa(nbr->router_id),
					       json_sub);
	}

	ospf_bfd_show_info(vty, nbr->bfd_info, json, use_json, 0);
}

static int show_ip_ospf_neighbor_id_common(struct vty *vty, struct ospf *ospf,
					   int arg_base,
					   struct cmd_token **argv,
					   u_char use_json)
{
	struct listnode *node;
	struct ospf_neighbor *nbr;
	struct ospf_interface *oi;
	struct in_addr router_id;
	int ret;
	json_object *json = NULL;

	if (use_json)
		json = json_object_new_object();

	if (ospf->instance) {
		if (use_json)
			json_object_int_add(json, "ospfInstance",
					    ospf->instance);
		else
			vty_out(vty, "%sOSPF Instance: %d%s%s", VTY_NEWLINE,
				ospf->instance, VTY_NEWLINE, VTY_NEWLINE);
	}

	ret = inet_aton(argv[arg_base]->arg, &router_id);
	if (!ret) {
		if (!use_json)
			vty_out(vty, "Please specify Neighbor ID by A.B.C.D%s",
				VTY_NEWLINE);
		return CMD_WARNING;
	}

	for (ALL_LIST_ELEMENTS_RO(ospf->oiflist, node, oi)) {
		if ((nbr = ospf_nbr_lookup_by_routerid(oi->nbrs, &router_id))) {
			show_ip_ospf_neighbor_detail_sub(vty, oi, nbr, use_json,
							 json);
		}
	}

	if (use_json) {
		vty_out(vty, "%s%s", json_object_to_json_string_ext(
					     json, JSON_C_TO_STRING_PRETTY),
			VTY_NEWLINE);
		json_object_free(json);
	} else
		vty_out(vty, "%s", VTY_NEWLINE);

	return CMD_SUCCESS;
}

DEFUN (show_ip_ospf_neighbor_id,
       show_ip_ospf_neighbor_id_cmd,
       "show ip ospf neighbor A.B.C.D [json]",
       SHOW_STR
       IP_STR
       "OSPF information\n"
       "Neighbor list\n"
       "Neighbor ID\n"
       JSON_STR)
{
	struct ospf *ospf;
	u_char uj = use_json(argc, argv);

	if ((ospf = ospf_lookup()) == NULL || !ospf->oi_running)
		return CMD_SUCCESS;

	return show_ip_ospf_neighbor_id_common(vty, ospf, 0, argv, uj);
}

DEFUN (show_ip_ospf_instance_neighbor_id,
       show_ip_ospf_instance_neighbor_id_cmd,
       "show ip ospf (1-65535) neighbor A.B.C.D [json]",
       SHOW_STR
       IP_STR
       "OSPF information\n"
       "Instance ID\n"
       "Neighbor list\n"
       "Neighbor ID\n"
       JSON_STR)
{
	int idx_number = 3;
	struct ospf *ospf;
	u_short instance = 0;
	u_char uj = use_json(argc, argv);

	VTY_GET_INTEGER("Instance", instance, argv[idx_number]->arg);
	if ((ospf = ospf_lookup_instance(instance)) == NULL
	    || !ospf->oi_running)
		return CMD_SUCCESS;

	return show_ip_ospf_neighbor_id_common(vty, ospf, 1, argv, uj);
}

static int show_ip_ospf_neighbor_detail_common(struct vty *vty,
					       struct ospf *ospf,
					       u_char use_json)
{
	struct ospf_interface *oi;
	struct listnode *node;
	json_object *json = NULL;

	if (use_json)
		json = json_object_new_object();

	if (ospf->instance) {
		if (use_json)
			json_object_int_add(json, "ospfInstance",
					    ospf->instance);
		else
			vty_out(vty, "%sOSPF Instance: %d%s%s", VTY_NEWLINE,
				ospf->instance, VTY_NEWLINE, VTY_NEWLINE);
	}

	for (ALL_LIST_ELEMENTS_RO(ospf->oiflist, node, oi)) {
		struct route_node *rn;
		struct ospf_neighbor *nbr;

		for (rn = route_top(oi->nbrs); rn; rn = route_next(rn)) {
			if ((nbr = rn->info)) {
				if (nbr != oi->nbr_self) {
					if (nbr->state != NSM_Down) {
						show_ip_ospf_neighbor_detail_sub(
							vty, oi, nbr, use_json,
							json);
					}
				}
			}
		}
	}

	if (use_json) {
		vty_out(vty, "%s%s", json_object_to_json_string_ext(
					     json, JSON_C_TO_STRING_PRETTY),
			VTY_NEWLINE);
		json_object_free(json);
	} else
		vty_out(vty, "%s", VTY_NEWLINE);

	return CMD_SUCCESS;
}

DEFUN (show_ip_ospf_neighbor_detail,
       show_ip_ospf_neighbor_detail_cmd,
       "show ip ospf neighbor detail [json]",
       SHOW_STR
       IP_STR
       "OSPF information\n"
       "Neighbor list\n"
       "detail of all neighbors\n"
       JSON_STR)
{
	struct ospf *ospf;
	u_char uj = use_json(argc, argv);

	if ((ospf = ospf_lookup()) == NULL || !ospf->oi_running)
		return CMD_SUCCESS;

	return show_ip_ospf_neighbor_detail_common(vty, ospf, uj);
}

DEFUN (show_ip_ospf_instance_neighbor_detail,
       show_ip_ospf_instance_neighbor_detail_cmd,
       "show ip ospf (1-65535) neighbor detail [json]",
       SHOW_STR
       IP_STR
       "OSPF information\n"
       "Instance ID\n"
       "Neighbor list\n"
       "detail of all neighbors\n"
       JSON_STR)
{
	int idx_number = 3;
	struct ospf *ospf;
	u_short instance = 0;
	u_char uj = use_json(argc, argv);

	VTY_GET_INTEGER("Instance", instance, argv[idx_number]->arg);
	if ((ospf = ospf_lookup_instance(instance)) == NULL
	    || !ospf->oi_running)
		return CMD_SUCCESS;

	return show_ip_ospf_neighbor_detail_common(vty, ospf, uj);
}

static int show_ip_ospf_neighbor_detail_all_common(struct vty *vty,
						   struct ospf *ospf,
						   u_char use_json)
{
	struct listnode *node;
	struct ospf_interface *oi;
	json_object *json = NULL;

	if (use_json)
		json = json_object_new_object();

	if (ospf->instance) {
		if (use_json)
			json_object_int_add(json, "ospfInstance",
					    ospf->instance);
		else
			vty_out(vty, "%sOSPF Instance: %d%s%s", VTY_NEWLINE,
				ospf->instance, VTY_NEWLINE, VTY_NEWLINE);
	}

	for (ALL_LIST_ELEMENTS_RO(ospf->oiflist, node, oi)) {
		struct route_node *rn;
		struct ospf_neighbor *nbr;
		struct ospf_nbr_nbma *nbr_nbma;

		for (rn = route_top(oi->nbrs); rn; rn = route_next(rn))
			if ((nbr = rn->info))
				if (nbr != oi->nbr_self)
					if (nbr->state != NSM_Down)
						show_ip_ospf_neighbor_detail_sub(
							vty, oi, rn->info,
							use_json, json);

		if (oi->type == OSPF_IFTYPE_NBMA) {
			struct listnode *nd;

			for (ALL_LIST_ELEMENTS_RO(oi->nbr_nbma, nd, nbr_nbma)) {
				if (nbr_nbma->nbr == NULL
				    || nbr_nbma->nbr->state == NSM_Down)
					show_ip_ospf_nbr_nbma_detail_sub(
						vty, oi, nbr_nbma, use_json,
						json);
			}
		}
	}

	if (use_json) {
		vty_out(vty, "%s%s", json_object_to_json_string_ext(
					     json, JSON_C_TO_STRING_PRETTY),
			VTY_NEWLINE);
		json_object_free(json);
	} else {
		vty_out(vty, "%s", VTY_NEWLINE);
	}

	return CMD_SUCCESS;
}

DEFUN (show_ip_ospf_neighbor_detail_all,
       show_ip_ospf_neighbor_detail_all_cmd,
       "show ip ospf neighbor detail all [json]",
       SHOW_STR
       IP_STR
       "OSPF information\n"
       "Neighbor list\n"
       "detail of all neighbors\n"
       "include down status neighbor\n"
       JSON_STR)
{
	struct ospf *ospf;
	u_char uj = use_json(argc, argv);

	if ((ospf = ospf_lookup()) == NULL || !ospf->oi_running)
		return CMD_SUCCESS;

	return show_ip_ospf_neighbor_detail_all_common(vty, ospf, uj);
}

DEFUN (show_ip_ospf_instance_neighbor_detail_all,
       show_ip_ospf_instance_neighbor_detail_all_cmd,
       "show ip ospf (1-65535) neighbor detail all [json]",
       SHOW_STR
       IP_STR
       "OSPF information\n"
       "Instance ID\n"
       "Neighbor list\n"
       "detail of all neighbors\n"
       "include down status neighbor\n"
       JSON_STR)
{
	int idx_number = 3;
	struct ospf *ospf;
	u_short instance = 0;
	u_char uj = use_json(argc, argv);

	VTY_GET_INTEGER("Instance", instance, argv[idx_number]->arg);
	if ((ospf = ospf_lookup_instance(instance)) == NULL
	    || !ospf->oi_running)
		return CMD_SUCCESS;

	return show_ip_ospf_neighbor_detail_all_common(vty, ospf, uj);
}

static int show_ip_ospf_neighbor_int_detail_common(struct vty *vty,
						   struct ospf *ospf,
						   int arg_base,
						   struct cmd_token **argv,
						   u_char use_json)
{
	struct ospf_interface *oi;
	struct interface *ifp;
	struct route_node *rn, *nrn;
	struct ospf_neighbor *nbr;
	json_object *json = NULL;

	if (use_json)
		json = json_object_new_object();

	if (ospf->instance) {
		if (use_json)
			json_object_int_add(json, "ospfInstance",
					    ospf->instance);
		else
			vty_out(vty, "%sOSPF Instance: %d%s%s", VTY_NEWLINE,
				ospf->instance, VTY_NEWLINE, VTY_NEWLINE);
	}

	ifp = if_lookup_by_name(argv[arg_base]->arg, VRF_DEFAULT);
	if (!ifp) {
		if (!use_json)
			vty_out(vty, "No such interface.%s", VTY_NEWLINE);
		return CMD_WARNING;
	}

	for (rn = route_top(IF_OIFS(ifp)); rn; rn = route_next(rn)) {
		if ((oi = rn->info)) {
			for (nrn = route_top(oi->nbrs); nrn;
			     nrn = route_next(nrn)) {
				if ((nbr = nrn->info)) {
					if (nbr != oi->nbr_self) {
						if (nbr->state != NSM_Down)
							show_ip_ospf_neighbor_detail_sub(
								vty, oi, nbr,
								use_json, json);
					}
				}
			}
		}
	}

	if (use_json) {
		vty_out(vty, "%s%s", json_object_to_json_string_ext(
					     json, JSON_C_TO_STRING_PRETTY),
			VTY_NEWLINE);
		json_object_free(json);
	} else
		vty_out(vty, "%s", VTY_NEWLINE);

	return CMD_SUCCESS;
}

DEFUN (show_ip_ospf_neighbor_int_detail,
       show_ip_ospf_neighbor_int_detail_cmd,
       "show ip ospf neighbor IFNAME detail [json]",
       SHOW_STR
       IP_STR
       "OSPF information\n"
       "Neighbor list\n"
       "Interface name\n"
       "detail of all neighbors\n"
       JSON_STR)
{
	struct ospf *ospf;
	u_char uj = use_json(argc, argv);

	if ((ospf = ospf_lookup()) == NULL || !ospf->oi_running)
		return CMD_SUCCESS;

	return show_ip_ospf_neighbor_int_detail_common(vty, ospf, 0, argv, uj);
}

DEFUN (show_ip_ospf_instance_neighbor_int_detail,
       show_ip_ospf_instance_neighbor_int_detail_cmd,
       "show ip ospf (1-65535) neighbor IFNAME detail [json]",
       SHOW_STR
       IP_STR
       "OSPF information\n"
       "Instance ID\n"
       "Neighbor list\n"
       "Interface name\n"
       "detail of all neighbors\n"
       JSON_STR)
{
	int idx_number = 3;
	struct ospf *ospf;
	u_short instance = 0;
	u_char uj = use_json(argc, argv);

	VTY_GET_INTEGER("Instance", instance, argv[idx_number]->arg);
	if ((ospf = ospf_lookup_instance(instance)) == NULL
	    || !ospf->oi_running)
		return CMD_SUCCESS;

	return show_ip_ospf_neighbor_int_detail_common(vty, ospf, 1, argv, uj);
}

/* Show functions */
static int show_lsa_summary(struct vty *vty, struct ospf_lsa *lsa, int self)
{
	struct router_lsa *rl;
	struct summary_lsa *sl;
	struct as_external_lsa *asel;
	struct prefix_ipv4 p;

	if (lsa != NULL)
		/* If self option is set, check LSA self flag. */
		if (self == 0 || IS_LSA_SELF(lsa)) {
			/* LSA common part show. */
			vty_out(vty, "%-15s ", inet_ntoa(lsa->data->id));
			vty_out(vty, "%-15s %4d 0x%08lx 0x%04x",
				inet_ntoa(lsa->data->adv_router), LS_AGE(lsa),
				(u_long)ntohl(lsa->data->ls_seqnum),
				ntohs(lsa->data->checksum));
			/* LSA specific part show. */
			switch (lsa->data->type) {
			case OSPF_ROUTER_LSA:
				rl = (struct router_lsa *)lsa->data;
				vty_out(vty, " %-d", ntohs(rl->links));
				break;
			case OSPF_SUMMARY_LSA:
				sl = (struct summary_lsa *)lsa->data;

				p.family = AF_INET;
				p.prefix = sl->header.id;
				p.prefixlen = ip_masklen(sl->mask);
				apply_mask_ipv4(&p);

				vty_out(vty, " %s/%d", inet_ntoa(p.prefix),
					p.prefixlen);
				break;
			case OSPF_AS_EXTERNAL_LSA:
			case OSPF_AS_NSSA_LSA:
				asel = (struct as_external_lsa *)lsa->data;

				p.family = AF_INET;
				p.prefix = asel->header.id;
				p.prefixlen = ip_masklen(asel->mask);
				apply_mask_ipv4(&p);

				vty_out(vty, " %s %s/%d [0x%lx]",
					IS_EXTERNAL_METRIC(asel->e[0].tos)
						? "E2"
						: "E1",
					inet_ntoa(p.prefix), p.prefixlen,
					(u_long)ntohl(asel->e[0].route_tag));
				break;
			case OSPF_NETWORK_LSA:
			case OSPF_ASBR_SUMMARY_LSA:
			case OSPF_OPAQUE_LINK_LSA:
			case OSPF_OPAQUE_AREA_LSA:
			case OSPF_OPAQUE_AS_LSA:
			default:
				break;
			}
			vty_out(vty, VTY_NEWLINE);
		}

	return 0;
}

static const char *show_database_desc[] = {
	"unknown",
	"Router Link States",
	"Net Link States",
	"Summary Link States",
	"ASBR-Summary Link States",
	"AS External Link States",
	"Group Membership LSA",
	"NSSA-external Link States",
	"Type-8 LSA",
	"Link-Local Opaque-LSA",
	"Area-Local Opaque-LSA",
	"AS-external Opaque-LSA",
};

static const char *show_database_header[] = {
	"",
	"Link ID         ADV Router      Age  Seq#       CkSum  Link count",
	"Link ID         ADV Router      Age  Seq#       CkSum",
	"Link ID         ADV Router      Age  Seq#       CkSum  Route",
	"Link ID         ADV Router      Age  Seq#       CkSum",
	"Link ID         ADV Router      Age  Seq#       CkSum  Route",
	" --- header for Group Member ----",
	"Link ID         ADV Router      Age  Seq#       CkSum  Route",
	" --- type-8 ---",
	"Opaque-Type/Id  ADV Router      Age  Seq#       CkSum",
	"Opaque-Type/Id  ADV Router      Age  Seq#       CkSum",
	"Opaque-Type/Id  ADV Router      Age  Seq#       CkSum",
};

static void show_ip_ospf_database_header(struct vty *vty, struct ospf_lsa *lsa)
{
	struct router_lsa *rlsa = (struct router_lsa *)lsa->data;

	vty_out(vty, "  LS age: %d%s", LS_AGE(lsa), VTY_NEWLINE);
	vty_out(vty, "  Options: 0x%-2x : %s%s", lsa->data->options,
		ospf_options_dump(lsa->data->options), VTY_NEWLINE);
	vty_out(vty, "  LS Flags: 0x%-2x %s%s", lsa->flags,
		((lsa->flags & OSPF_LSA_LOCAL_XLT) ? "(Translated from Type-7)"
						   : ""),
		VTY_NEWLINE);

	if (lsa->data->type == OSPF_ROUTER_LSA) {
		vty_out(vty, "  Flags: 0x%x", rlsa->flags);

		if (rlsa->flags)
			vty_out(vty, " :%s%s%s%s",
				IS_ROUTER_LSA_BORDER(rlsa) ? " ABR" : "",
				IS_ROUTER_LSA_EXTERNAL(rlsa) ? " ASBR" : "",
				IS_ROUTER_LSA_VIRTUAL(rlsa) ? " VL-endpoint"
							    : "",
				IS_ROUTER_LSA_SHORTCUT(rlsa) ? " Shortcut"
							     : "");

		vty_out(vty, "%s", VTY_NEWLINE);
	}
	vty_out(vty, "  LS Type: %s%s",
		lookup_msg(ospf_lsa_type_msg, lsa->data->type, NULL),
		VTY_NEWLINE);
	vty_out(vty, "  Link State ID: %s %s%s", inet_ntoa(lsa->data->id),
		lookup_msg(ospf_link_state_id_type_msg, lsa->data->type, NULL),
		VTY_NEWLINE);
	vty_out(vty, "  Advertising Router: %s%s",
		inet_ntoa(lsa->data->adv_router), VTY_NEWLINE);
	vty_out(vty, "  LS Seq Number: %08lx%s",
		(u_long)ntohl(lsa->data->ls_seqnum), VTY_NEWLINE);
	vty_out(vty, "  Checksum: 0x%04x%s", ntohs(lsa->data->checksum),
		VTY_NEWLINE);
	vty_out(vty, "  Length: %d%s", ntohs(lsa->data->length), VTY_NEWLINE);
}

const char *link_type_desc[] = {
	"(null)",
	"another Router (point-to-point)",
	"a Transit Network",
	"Stub Network",
	"a Virtual Link",
};

const char *link_id_desc[] = {
	"(null)", "Neighboring Router ID", "Designated Router address",
	"Net",    "Neighboring Router ID",
};

const char *link_data_desc[] = {
	"(null)",       "Router Interface address", "Router Interface address",
	"Network Mask", "Router Interface address",
};

/* Show router-LSA each Link information. */
static void show_ip_ospf_database_router_links(struct vty *vty,
					       struct router_lsa *rl)
{
	int len, type;
	unsigned int i;

	len = ntohs(rl->header.length) - 4;
	for (i = 0; i < ntohs(rl->links) && len > 0; len -= 12, i++) {
		type = rl->link[i].type;

		vty_out(vty, "    Link connected to: %s%s",
			link_type_desc[type], VTY_NEWLINE);
		vty_out(vty, "     (Link ID) %s: %s%s", link_id_desc[type],
			inet_ntoa(rl->link[i].link_id), VTY_NEWLINE);
		vty_out(vty, "     (Link Data) %s: %s%s", link_data_desc[type],
			inet_ntoa(rl->link[i].link_data), VTY_NEWLINE);
		vty_out(vty, "      Number of TOS metrics: 0%s", VTY_NEWLINE);
		vty_out(vty, "       TOS 0 Metric: %d%s",
			ntohs(rl->link[i].metric), VTY_NEWLINE);
		vty_out(vty, "%s", VTY_NEWLINE);
	}
}

/* Show router-LSA detail information. */
static int show_router_lsa_detail(struct vty *vty, struct ospf_lsa *lsa)
{
	if (lsa != NULL) {
		struct router_lsa *rl = (struct router_lsa *)lsa->data;

		show_ip_ospf_database_header(vty, lsa);

		vty_out(vty, "   Number of Links: %d%s%s", ntohs(rl->links),
			VTY_NEWLINE, VTY_NEWLINE);

		show_ip_ospf_database_router_links(vty, rl);
		vty_out(vty, "%s", VTY_NEWLINE);
	}

	return 0;
}

/* Show network-LSA detail information. */
static int show_network_lsa_detail(struct vty *vty, struct ospf_lsa *lsa)
{
	int length, i;

	if (lsa != NULL) {
		struct network_lsa *nl = (struct network_lsa *)lsa->data;

		show_ip_ospf_database_header(vty, lsa);

		vty_out(vty, "  Network Mask: /%d%s", ip_masklen(nl->mask),
			VTY_NEWLINE);

		length = ntohs(lsa->data->length) - OSPF_LSA_HEADER_SIZE - 4;

		for (i = 0; length > 0; i++, length -= 4)
			vty_out(vty, "        Attached Router: %s%s",
				inet_ntoa(nl->routers[i]), VTY_NEWLINE);

		vty_out(vty, "%s", VTY_NEWLINE);
	}

	return 0;
}

/* Show summary-LSA detail information. */
static int show_summary_lsa_detail(struct vty *vty, struct ospf_lsa *lsa)
{
	if (lsa != NULL) {
		struct summary_lsa *sl = (struct summary_lsa *)lsa->data;

		show_ip_ospf_database_header(vty, lsa);

		vty_out(vty, "  Network Mask: /%d%s", ip_masklen(sl->mask),
			VTY_NEWLINE);
		vty_out(vty, "        TOS: 0  Metric: %d%s",
			GET_METRIC(sl->metric), VTY_NEWLINE);
		vty_out(vty, "%s", VTY_NEWLINE);
	}

	return 0;
}

/* Show summary-ASBR-LSA detail information. */
static int show_summary_asbr_lsa_detail(struct vty *vty, struct ospf_lsa *lsa)
{
	if (lsa != NULL) {
		struct summary_lsa *sl = (struct summary_lsa *)lsa->data;

		show_ip_ospf_database_header(vty, lsa);

		vty_out(vty, "  Network Mask: /%d%s", ip_masklen(sl->mask),
			VTY_NEWLINE);
		vty_out(vty, "        TOS: 0  Metric: %d%s",
			GET_METRIC(sl->metric), VTY_NEWLINE);
		vty_out(vty, "%s", VTY_NEWLINE);
	}

	return 0;
}

/* Show AS-external-LSA detail information. */
static int show_as_external_lsa_detail(struct vty *vty, struct ospf_lsa *lsa)
{
	if (lsa != NULL) {
		struct as_external_lsa *al =
			(struct as_external_lsa *)lsa->data;

		show_ip_ospf_database_header(vty, lsa);

		vty_out(vty, "  Network Mask: /%d%s", ip_masklen(al->mask),
			VTY_NEWLINE);
		vty_out(vty, "        Metric Type: %s%s",
			IS_EXTERNAL_METRIC(al->e[0].tos)
				? "2 (Larger than any link state path)"
				: "1",
			VTY_NEWLINE);
		vty_out(vty, "        TOS: 0%s", VTY_NEWLINE);
		vty_out(vty, "        Metric: %d%s",
			GET_METRIC(al->e[0].metric), VTY_NEWLINE);
		vty_out(vty, "        Forward Address: %s%s",
			inet_ntoa(al->e[0].fwd_addr), VTY_NEWLINE);

		vty_out(vty,
			"        External Route Tag: %" ROUTE_TAG_PRI "%s%s",
			(route_tag_t)ntohl(al->e[0].route_tag), VTY_NEWLINE,
			VTY_NEWLINE);
	}

	return 0;
}
#if 0
static int
show_as_external_lsa_stdvty (struct ospf_lsa *lsa)
{
  struct as_external_lsa *al = (struct as_external_lsa *) lsa->data;

  /* show_ip_ospf_database_header (vty, lsa); */

  zlog_debug( "  Network Mask: /%d%s",
	     ip_masklen (al->mask), "\n");
  zlog_debug( "        Metric Type: %s%s",
	     IS_EXTERNAL_METRIC (al->e[0].tos) ?
	     "2 (Larger than any link state path)" : "1", "\n");
  zlog_debug( "        TOS: 0%s", "\n");
  zlog_debug( "        Metric: %d%s",
	     GET_METRIC (al->e[0].metric), "\n");
  zlog_debug( "        Forward Address: %s%s",
	     inet_ntoa (al->e[0].fwd_addr), "\n");

  zlog_debug( "        External Route Tag: %"ROUTE_TAG_PRI"%s%s",
	     (route_tag_t)ntohl (al->e[0].route_tag), "\n", "\n");

  return 0;
}
#endif
/* Show AS-NSSA-LSA detail information. */
static int show_as_nssa_lsa_detail(struct vty *vty, struct ospf_lsa *lsa)
{
	if (lsa != NULL) {
		struct as_external_lsa *al =
			(struct as_external_lsa *)lsa->data;

		show_ip_ospf_database_header(vty, lsa);

		vty_out(vty, "  Network Mask: /%d%s", ip_masklen(al->mask),
			VTY_NEWLINE);
		vty_out(vty, "        Metric Type: %s%s",
			IS_EXTERNAL_METRIC(al->e[0].tos)
				? "2 (Larger than any link state path)"
				: "1",
			VTY_NEWLINE);
		vty_out(vty, "        TOS: 0%s", VTY_NEWLINE);
		vty_out(vty, "        Metric: %d%s",
			GET_METRIC(al->e[0].metric), VTY_NEWLINE);
		vty_out(vty, "        NSSA: Forward Address: %s%s",
			inet_ntoa(al->e[0].fwd_addr), VTY_NEWLINE);

		vty_out(vty,
			"        External Route Tag: %" ROUTE_TAG_PRI "%s%s",
			(route_tag_t)ntohl(al->e[0].route_tag), VTY_NEWLINE,
			VTY_NEWLINE);
	}

	return 0;
}

static int show_func_dummy(struct vty *vty, struct ospf_lsa *lsa)
{
	return 0;
}

static int show_opaque_lsa_detail(struct vty *vty, struct ospf_lsa *lsa)
{
	if (lsa != NULL) {
		show_ip_ospf_database_header(vty, lsa);
		show_opaque_info_detail(vty, lsa);

		vty_out(vty, "%s", VTY_NEWLINE);
	}
	return 0;
}

int (*show_function[])(struct vty *, struct ospf_lsa *) = {
	NULL,
	show_router_lsa_detail,
	show_network_lsa_detail,
	show_summary_lsa_detail,
	show_summary_asbr_lsa_detail,
	show_as_external_lsa_detail,
	show_func_dummy,
	show_as_nssa_lsa_detail, /* almost same as external */
	NULL,			 /* type-8 */
	show_opaque_lsa_detail,
	show_opaque_lsa_detail,
	show_opaque_lsa_detail,
};

static void show_lsa_prefix_set(struct vty *vty, struct prefix_ls *lp,
				struct in_addr *id, struct in_addr *adv_router)
{
	memset(lp, 0, sizeof(struct prefix_ls));
	lp->family = 0;
	if (id == NULL)
		lp->prefixlen = 0;
	else if (adv_router == NULL) {
		lp->prefixlen = 32;
		lp->id = *id;
	} else {
		lp->prefixlen = 64;
		lp->id = *id;
		lp->adv_router = *adv_router;
	}
}

static void show_lsa_detail_proc(struct vty *vty, struct route_table *rt,
				 struct in_addr *id, struct in_addr *adv_router)
{
	struct prefix_ls lp;
	struct route_node *rn, *start;
	struct ospf_lsa *lsa;

	show_lsa_prefix_set(vty, &lp, id, adv_router);
	start = route_node_get(rt, (struct prefix *)&lp);
	if (start) {
		route_lock_node(start);
		for (rn = start; rn; rn = route_next_until(rn, start))
			if ((lsa = rn->info)) {
				if (show_function[lsa->data->type] != NULL)
					show_function[lsa->data->type](vty,
								       lsa);
			}
		route_unlock_node(start);
	}
}

/* Show detail LSA information
   -- if id is NULL then show all LSAs. */
static void show_lsa_detail(struct vty *vty, struct ospf *ospf, int type,
			    struct in_addr *id, struct in_addr *adv_router)
{
	struct listnode *node;
	struct ospf_area *area;

	switch (type) {
	case OSPF_AS_EXTERNAL_LSA:
	case OSPF_OPAQUE_AS_LSA:
		vty_out(vty, "                %s %s%s",
			show_database_desc[type], VTY_NEWLINE, VTY_NEWLINE);
		show_lsa_detail_proc(vty, AS_LSDB(ospf, type), id, adv_router);
		break;
	default:
		for (ALL_LIST_ELEMENTS_RO(ospf->areas, node, area)) {
			vty_out(vty, "%s                %s (Area %s)%s%s",
				VTY_NEWLINE, show_database_desc[type],
				ospf_area_desc_string(area), VTY_NEWLINE,
				VTY_NEWLINE);
			show_lsa_detail_proc(vty, AREA_LSDB(area, type), id,
					     adv_router);
		}
		break;
	}
}

static void show_lsa_detail_adv_router_proc(struct vty *vty,
					    struct route_table *rt,
					    struct in_addr *adv_router)
{
	struct route_node *rn;
	struct ospf_lsa *lsa;

	for (rn = route_top(rt); rn; rn = route_next(rn))
		if ((lsa = rn->info))
			if (IPV4_ADDR_SAME(adv_router,
					   &lsa->data->adv_router)) {
				if (CHECK_FLAG(lsa->flags, OSPF_LSA_LOCAL_XLT))
					continue;
				if (show_function[lsa->data->type] != NULL)
					show_function[lsa->data->type](vty,
								       lsa);
			}
}

/* Show detail LSA information. */
static void show_lsa_detail_adv_router(struct vty *vty, struct ospf *ospf,
				       int type, struct in_addr *adv_router)
{
	struct listnode *node;
	struct ospf_area *area;

	switch (type) {
	case OSPF_AS_EXTERNAL_LSA:
	case OSPF_OPAQUE_AS_LSA:
		vty_out(vty, "                %s %s%s",
			show_database_desc[type], VTY_NEWLINE, VTY_NEWLINE);
		show_lsa_detail_adv_router_proc(vty, AS_LSDB(ospf, type),
						adv_router);
		break;
	default:
		for (ALL_LIST_ELEMENTS_RO(ospf->areas, node, area)) {
			vty_out(vty, "%s                %s (Area %s)%s%s",
				VTY_NEWLINE, show_database_desc[type],
				ospf_area_desc_string(area), VTY_NEWLINE,
				VTY_NEWLINE);
			show_lsa_detail_adv_router_proc(
				vty, AREA_LSDB(area, type), adv_router);
		}
		break;
	}
}

static void show_ip_ospf_database_summary(struct vty *vty, struct ospf *ospf,
					  int self)
{
	struct ospf_lsa *lsa;
	struct route_node *rn;
	struct ospf_area *area;
	struct listnode *node;
	int type;

	for (ALL_LIST_ELEMENTS_RO(ospf->areas, node, area)) {
		for (type = OSPF_MIN_LSA; type < OSPF_MAX_LSA; type++) {
			switch (type) {
			case OSPF_AS_EXTERNAL_LSA:
			case OSPF_OPAQUE_AS_LSA:
				continue;
			default:
				break;
			}
			if (ospf_lsdb_count_self(area->lsdb, type) > 0
			    || (!self
				&& ospf_lsdb_count(area->lsdb, type) > 0)) {
				vty_out(vty, "                %s (Area %s)%s%s",
					show_database_desc[type],
					ospf_area_desc_string(area),
					VTY_NEWLINE, VTY_NEWLINE);
				vty_out(vty, "%s%s", show_database_header[type],
					VTY_NEWLINE);

				LSDB_LOOP(AREA_LSDB(area, type), rn, lsa)
				show_lsa_summary(vty, lsa, self);

				vty_out(vty, "%s", VTY_NEWLINE);
			}
		}
	}

	for (type = OSPF_MIN_LSA; type < OSPF_MAX_LSA; type++) {
		switch (type) {
		case OSPF_AS_EXTERNAL_LSA:
		case OSPF_OPAQUE_AS_LSA:
			break;
		default:
			continue;
		}
		if (ospf_lsdb_count_self(ospf->lsdb, type)
		    || (!self && ospf_lsdb_count(ospf->lsdb, type))) {
			vty_out(vty, "                %s%s%s",
				show_database_desc[type], VTY_NEWLINE,
				VTY_NEWLINE);
			vty_out(vty, "%s%s", show_database_header[type],
				VTY_NEWLINE);

			LSDB_LOOP(AS_LSDB(ospf, type), rn, lsa)
			show_lsa_summary(vty, lsa, self);

			vty_out(vty, "%s", VTY_NEWLINE);
		}
	}

	vty_out(vty, "%s", VTY_NEWLINE);
}

static void show_ip_ospf_database_maxage(struct vty *vty, struct ospf *ospf)
{
	struct route_node *rn;

	vty_out(vty, "%s                MaxAge Link States:%s%s", VTY_NEWLINE,
		VTY_NEWLINE, VTY_NEWLINE);

	for (rn = route_top(ospf->maxage_lsa); rn; rn = route_next(rn)) {
		struct ospf_lsa *lsa;

		if ((lsa = rn->info) != NULL) {
			vty_out(vty, "Link type: %d%s", lsa->data->type,
				VTY_NEWLINE);
			vty_out(vty, "Link State ID: %s%s",
				inet_ntoa(lsa->data->id), VTY_NEWLINE);
			vty_out(vty, "Advertising Router: %s%s",
				inet_ntoa(lsa->data->adv_router), VTY_NEWLINE);
			vty_out(vty, "LSA lock count: %d%s", lsa->lock,
				VTY_NEWLINE);
			vty_out(vty, "%s", VTY_NEWLINE);
		}
	}
}

#define OSPF_LSA_TYPE_NSSA_DESC      "NSSA external link state\n"
#define OSPF_LSA_TYPE_NSSA_CMD_STR   "|nssa-external"

#define OSPF_LSA_TYPE_OPAQUE_LINK_DESC "Link local Opaque-LSA\n"
#define OSPF_LSA_TYPE_OPAQUE_AREA_DESC "Link area Opaque-LSA\n"
#define OSPF_LSA_TYPE_OPAQUE_AS_DESC   "Link AS Opaque-LSA\n"
#define OSPF_LSA_TYPE_OPAQUE_CMD_STR   "|opaque-link|opaque-area|opaque-as"

#define OSPF_LSA_TYPES_DESC                                                    \
	"ASBR summary link states\n"                                           \
	"External link states\n"                                               \
	"Network link states\n"                                                \
	"Router link states\n"                                                 \
	"Network summary link states\n" OSPF_LSA_TYPE_NSSA_DESC                \
		OSPF_LSA_TYPE_OPAQUE_LINK_DESC OSPF_LSA_TYPE_OPAQUE_AREA_DESC  \
			OSPF_LSA_TYPE_OPAQUE_AS_DESC

static int show_ip_ospf_database_common(struct vty *vty, struct ospf *ospf,
					int arg_base, int argc,
					struct cmd_token **argv)
{
	int idx_type = 4;
	int type, ret;
	struct in_addr id, adv_router;

	if (ospf->instance)
		vty_out(vty, "%sOSPF Instance: %d%s", VTY_NEWLINE,
			ospf->instance, VTY_NEWLINE);

	vty_out(vty, "%s       OSPF Router with ID (%s)%s%s", VTY_NEWLINE,
		inet_ntoa(ospf->router_id), VTY_NEWLINE, VTY_NEWLINE);

	/* Show all LSA. */
	if (argc == arg_base + 4) {
		show_ip_ospf_database_summary(vty, ospf, 0);
		return CMD_SUCCESS;
	}

	/* Set database type to show. */
	if (strncmp(argv[arg_base + idx_type]->text, "r", 1) == 0)
		type = OSPF_ROUTER_LSA;
	else if (strncmp(argv[arg_base + idx_type]->text, "ne", 2) == 0)
		type = OSPF_NETWORK_LSA;
	else if (strncmp(argv[arg_base + idx_type]->text, "ns", 2) == 0)
		type = OSPF_AS_NSSA_LSA;
	else if (strncmp(argv[arg_base + idx_type]->text, "su", 2) == 0)
		type = OSPF_SUMMARY_LSA;
	else if (strncmp(argv[arg_base + idx_type]->text, "a", 1) == 0)
		type = OSPF_ASBR_SUMMARY_LSA;
	else if (strncmp(argv[arg_base + idx_type]->text, "e", 1) == 0)
		type = OSPF_AS_EXTERNAL_LSA;
	else if (strncmp(argv[arg_base + idx_type]->text, "se", 2) == 0) {
		show_ip_ospf_database_summary(vty, ospf, 1);
		return CMD_SUCCESS;
	} else if (strncmp(argv[arg_base + idx_type]->text, "m", 1) == 0) {
		show_ip_ospf_database_maxage(vty, ospf);
		return CMD_SUCCESS;
	} else if (strncmp(argv[arg_base + idx_type]->text, "opaque-l", 8) == 0)
		type = OSPF_OPAQUE_LINK_LSA;
	else if (strncmp(argv[arg_base + idx_type]->text, "opaque-ar", 9) == 0)
		type = OSPF_OPAQUE_AREA_LSA;
	else if (strncmp(argv[arg_base + idx_type]->text, "opaque-as", 9) == 0)
		type = OSPF_OPAQUE_AS_LSA;
	else
		return CMD_WARNING;

	/* `show ip ospf database LSA'. */
	if (argc == arg_base + 5)
		show_lsa_detail(vty, ospf, type, NULL, NULL);
	else if (argc >= arg_base + 6) {
		ret = inet_aton(argv[arg_base + 5]->arg, &id);
		if (!ret)
			return CMD_WARNING;

		/* `show ip ospf database LSA ID'. */
		if (argc == arg_base + 6)
			show_lsa_detail(vty, ospf, type, &id, NULL);
		/* `show ip ospf database LSA ID adv-router ADV_ROUTER'. */
		else if (argc == arg_base + 7) {
			if (strncmp(argv[arg_base + 6]->text, "s", 1) == 0)
				adv_router = ospf->router_id;
			else {
				ret = inet_aton(argv[arg_base + 7]->arg,
						&adv_router);
				if (!ret)
					return CMD_WARNING;
			}
			show_lsa_detail(vty, ospf, type, &id, &adv_router);
		}
	}

	return CMD_SUCCESS;
}

DEFUN (show_ip_ospf_database_max,
       show_ip_ospf_database_max_cmd,
       "show ip ospf database <max-age|self-originate>",
       SHOW_STR
       IP_STR
       "OSPF information\n"
       "Database summary\n"
       "LSAs in MaxAge list\n"
       "Self-originated link states\n")
{
	struct ospf *ospf;

	if ((ospf = ospf_lookup()) == NULL || !ospf->oi_running)
		return CMD_SUCCESS;

	return (show_ip_ospf_database_common(vty, ospf, 0, argc, argv));
}

DEFUN (show_ip_ospf_instance_database,
       show_ip_ospf_instance_database_cmd,
       "show ip ospf [(1-65535)] database [<asbr-summary|external|network|router|summary|nssa-external|opaque-link|opaque-area|opaque-as> [A.B.C.D [<self-originate|adv-router A.B.C.D>]]]",
       SHOW_STR
       IP_STR
       "OSPF information\n"
       "Instance ID\n"
       "Database summary\n"
        OSPF_LSA_TYPES_DESC
       "Link State ID (as an IP address)\n"
       "Self-originated link states\n"
       "Advertising Router link states\n"
       "Advertising Router (as an IP address)\n")
{
	struct ospf *ospf;
	u_short instance = 0;

	int idx = 0;
	if (argv_find(argv, argc, "(1-65535)", &idx)) {
		VTY_GET_INTEGER("Instance", instance, argv[idx]->arg);
		ospf = ospf_lookup_instance(instance);
	} else {
		ospf = ospf_lookup();
	}

	if (!ospf || !ospf->oi_running)
		return CMD_SUCCESS;

	return (show_ip_ospf_database_common(vty, ospf, idx ? 1 : 0, argc,
					     argv));
}

DEFUN (show_ip_ospf_instance_database_max,
       show_ip_ospf_instance_database_max_cmd,
       "show ip ospf (1-65535) database <max-age|self-originate>",
       SHOW_STR
       IP_STR
       "OSPF information\n"
       "Instance ID\n"
       "Database summary\n"
       "LSAs in MaxAge list\n"
       "Self-originated link states\n")
{
	int idx_number = 3;
	struct ospf *ospf;
	u_short instance = 0;

	VTY_GET_INTEGER("Instance", instance, argv[idx_number]->arg);

	if ((ospf = ospf_lookup_instance(instance)) == NULL
	    || !ospf->oi_running)
		return CMD_SUCCESS;

	return (show_ip_ospf_database_common(vty, ospf, 1, argc, argv));
}


static int show_ip_ospf_database_type_adv_router_common(struct vty *vty,
							struct ospf *ospf,
							int arg_base, int argc,
							struct cmd_token **argv)
{
	int idx_type = 4;
	int type, ret;
	struct in_addr adv_router;

	if (ospf->instance)
		vty_out(vty, "%sOSPF Instance: %d%s", VTY_NEWLINE,
			ospf->instance, VTY_NEWLINE);

	vty_out(vty, "%s       OSPF Router with ID (%s)%s%s", VTY_NEWLINE,
		inet_ntoa(ospf->router_id), VTY_NEWLINE, VTY_NEWLINE);

	if (argc != arg_base + 7)
		return CMD_WARNING;

	/* Set database type to show. */
	if (strncmp(argv[arg_base + idx_type]->text, "r", 1) == 0)
		type = OSPF_ROUTER_LSA;
	else if (strncmp(argv[arg_base + idx_type]->text, "ne", 2) == 0)
		type = OSPF_NETWORK_LSA;
	else if (strncmp(argv[arg_base + idx_type]->text, "ns", 2) == 0)
		type = OSPF_AS_NSSA_LSA;
	else if (strncmp(argv[arg_base + idx_type]->text, "s", 1) == 0)
		type = OSPF_SUMMARY_LSA;
	else if (strncmp(argv[arg_base + idx_type]->text, "a", 1) == 0)
		type = OSPF_ASBR_SUMMARY_LSA;
	else if (strncmp(argv[arg_base + idx_type]->text, "e", 1) == 0)
		type = OSPF_AS_EXTERNAL_LSA;
	else if (strncmp(argv[arg_base + idx_type]->text, "opaque-l", 8) == 0)
		type = OSPF_OPAQUE_LINK_LSA;
	else if (strncmp(argv[arg_base + idx_type]->text, "opaque-ar", 9) == 0)
		type = OSPF_OPAQUE_AREA_LSA;
	else if (strncmp(argv[arg_base + idx_type]->text, "opaque-as", 9) == 0)
		type = OSPF_OPAQUE_AS_LSA;
	else
		return CMD_WARNING;

	/* `show ip ospf database LSA adv-router ADV_ROUTER'. */
	if (strncmp(argv[arg_base + 5]->text, "s", 1) == 0)
		adv_router = ospf->router_id;
	else {
		ret = inet_aton(argv[arg_base + 6]->arg, &adv_router);
		if (!ret)
			return CMD_WARNING;
	}

	show_lsa_detail_adv_router(vty, ospf, type, &adv_router);

	return CMD_SUCCESS;
}

DEFUN (show_ip_ospf_database_type_adv_router,
       show_ip_ospf_database_type_adv_router_cmd,
       "show ip ospf database <asbr-summary|external|network|router|summary|nssa-external|opaque-link|opaque-area|opaque-as> <adv-router A.B.C.D|self-originate>",
       SHOW_STR
       IP_STR
       "OSPF information\n"
       "Database summary\n"
       OSPF_LSA_TYPES_DESC
       "Advertising Router link states\n"
       "Advertising Router (as an IP address)\n"
       "Self-originated link states\n")
{
	struct ospf *ospf;

	if ((ospf = ospf_lookup()) == NULL || !ospf->oi_running)
		return CMD_SUCCESS;

	return (show_ip_ospf_database_type_adv_router_common(vty, ospf, 0, argc,
							     argv));
}

DEFUN (show_ip_ospf_instance_database_type_adv_router,
       show_ip_ospf_instance_database_type_adv_router_cmd,
       "show ip ospf (1-65535) database <asbr-summary|external|network|router|summary|nssa-external|opaque-link|opaque-area|opaque-as> <adv-router A.B.C.D|self-originate>",
       SHOW_STR
       IP_STR
       "OSPF information\n"
       "Instance ID\n"
       "Database summary\n"
       OSPF_LSA_TYPES_DESC
       "Advertising Router link states\n"
       "Advertising Router (as an IP address)\n"
       "Self-originated link states\n")
{
	int idx_number = 3;
	struct ospf *ospf;
	u_short instance = 0;

	VTY_GET_INTEGER("Instance", instance, argv[idx_number]->arg);

	if ((ospf = ospf_lookup_instance(instance)) == NULL
	    || !ospf->oi_running)
		return CMD_SUCCESS;

	return (show_ip_ospf_database_type_adv_router_common(vty, ospf, 1, argc,
							     argv));
}

DEFUN (ip_ospf_authentication_args,
       ip_ospf_authentication_args_addr_cmd,
       "ip ospf authentication <null|message-digest> [A.B.C.D]",
       "IP Information\n"
       "OSPF interface commands\n"
       "Enable authentication on this interface\n"
       "Use null authentication\n"
       "Use message-digest authentication\n"
       "Address of interface\n")
{
	VTY_DECLVAR_CONTEXT(interface, ifp);
	int idx_encryption = 3;
	int idx_ipv4 = 4;
	struct in_addr addr;
	int ret;
	struct ospf_if_params *params;

	params = IF_DEF_PARAMS(ifp);

	if (argc == 5) {
		ret = inet_aton(argv[idx_ipv4]->arg, &addr);
		if (!ret) {
			vty_out(vty,
				"Please specify interface address by A.B.C.D%s",
				VTY_NEWLINE);
			return CMD_WARNING;
		}

		params = ospf_get_if_params(ifp, addr);
		ospf_if_update_params(ifp, addr);
	}

	/* Handle null authentication */
	if (argv[idx_encryption]->arg[0] == 'n') {
		SET_IF_PARAM(params, auth_type);
		params->auth_type = OSPF_AUTH_NULL;
		return CMD_SUCCESS;
	}

	/* Handle message-digest authentication */
	if (argv[idx_encryption]->arg[0] == 'm') {
		SET_IF_PARAM(params, auth_type);
		params->auth_type = OSPF_AUTH_CRYPTOGRAPHIC;
		return CMD_SUCCESS;
	}

	vty_out(vty, "You shouldn't get here!%s", VTY_NEWLINE);
	return CMD_WARNING;
}

DEFUN (ip_ospf_authentication,
       ip_ospf_authentication_addr_cmd,
       "ip ospf authentication [A.B.C.D]",
       "IP Information\n"
       "OSPF interface commands\n"
       "Enable authentication on this interface\n"
       "Address of interface")
{
	VTY_DECLVAR_CONTEXT(interface, ifp);
	int idx_ipv4 = 3;
	struct in_addr addr;
	int ret;
	struct ospf_if_params *params;

	params = IF_DEF_PARAMS(ifp);

	if (argc == 4) {
		ret = inet_aton(argv[idx_ipv4]->arg, &addr);
		if (!ret) {
			vty_out(vty,
				"Please specify interface address by A.B.C.D%s",
				VTY_NEWLINE);
			return CMD_WARNING;
		}

		params = ospf_get_if_params(ifp, addr);
		ospf_if_update_params(ifp, addr);
	}

	SET_IF_PARAM(params, auth_type);
	params->auth_type = OSPF_AUTH_SIMPLE;

	return CMD_SUCCESS;
}

DEFUN (no_ip_ospf_authentication_args,
       no_ip_ospf_authentication_args_addr_cmd,
       "no ip ospf authentication <null|message-digest> [A.B.C.D]",
       NO_STR
       "IP Information\n"
       "OSPF interface commands\n"
       "Enable authentication on this interface\n"
       "Use null authentication\n"
       "Use message-digest authentication\n"
       "Address of interface")
{
	VTY_DECLVAR_CONTEXT(interface, ifp);
	int idx_encryption = 4;
	int idx_ipv4 = 5;
	struct in_addr addr;
	int ret;
	struct ospf_if_params *params;
	struct route_node *rn;
	int auth_type;
/* modified by liwh for bug 44298, 2017-06-22 */
#ifdef _CENTEC_
  struct route_node *rn_next = NULL;
#endif
/*liwh end */

	params = IF_DEF_PARAMS(ifp);

	if (argc == 6) {
		ret = inet_aton(argv[idx_ipv4]->arg, &addr);
		if (!ret) {
			vty_out(vty,
				"Please specify interface address by A.B.C.D%s",
				VTY_NEWLINE);
			return CMD_WARNING;
		}

		params = ospf_lookup_if_params(ifp, addr);
		if (params == NULL) {
			vty_out(vty, "Ip Address specified is unknown%s",
				VTY_NEWLINE);
			return CMD_WARNING;
		}
		params->auth_type = OSPF_AUTH_NOTSET;
		UNSET_IF_PARAM(params, auth_type);
		if (params != IF_DEF_PARAMS(ifp)) {
			ospf_free_if_params(ifp, addr);
			ospf_if_update_params(ifp, addr);
		}
	} else {
		if (argv[idx_encryption]->arg[0] == 'n') {
			auth_type = OSPF_AUTH_NULL;
		} else if (argv[idx_encryption]->arg[0] == 'm') {
			auth_type = OSPF_AUTH_CRYPTOGRAPHIC;
		} else {
			vty_out(vty, "Unexpected input encountered%s",
				VTY_NEWLINE);
			return CMD_WARNING;
		}
		/*
		 * Here we have a case where the user has entered
		 * 'no ip ospf authentication (null | message_digest )'
		 * we need to find if we have any ip addresses underneath it
		 * that
		 * correspond to the associated type.
		 */
		if (params->auth_type == auth_type) {
			params->auth_type = OSPF_AUTH_NOTSET;
			UNSET_IF_PARAM(params, auth_type);
		}

/* modified by liwh for bug 44298, 2017-06-22 */
#ifdef _CENTEC_
      //for (rn = route_top (IF_OIFS_PARAMS (ifp)); rn; rn = route_next (rn))
      //  {
      for (rn = route_top (IF_OIFS_PARAMS (ifp)); rn; rn = rn_next)
        {
          rn_next = route_next (rn);
#endif
/*liwh end */          
			if ((params = rn->info)) {
				if (params->auth_type == auth_type) {
					params->auth_type = OSPF_AUTH_NOTSET;
					UNSET_IF_PARAM(params, auth_type);
					if (params != IF_DEF_PARAMS(ifp)) {
						ospf_free_if_params(
							ifp, rn->p.u.prefix4);
						ospf_if_update_params(
							ifp, rn->p.u.prefix4);
					}
				}
			}
		}
	}

	return CMD_SUCCESS;
}

DEFUN (no_ip_ospf_authentication,
       no_ip_ospf_authentication_addr_cmd,
       "no ip ospf authentication [A.B.C.D]",
       NO_STR
       "IP Information\n"
       "OSPF interface commands\n"
       "Enable authentication on this interface\n"
       "Address of interface")
{
	VTY_DECLVAR_CONTEXT(interface, ifp);
	int idx_ipv4 = 4;
	struct in_addr addr;
	int ret;
	struct ospf_if_params *params;
	struct route_node *rn;
/* modified by liwh for bug 44298, 2017-06-22 */
#ifdef _CENTEC_
  struct route_node *rn_next = NULL;
#endif
/*liwh end */

	params = IF_DEF_PARAMS(ifp);

	if (argc == 5) {
		ret = inet_aton(argv[idx_ipv4]->arg, &addr);
		if (!ret) {
			vty_out(vty,
				"Please specify interface address by A.B.C.D%s",
				VTY_NEWLINE);
			return CMD_WARNING;
		}

		params = ospf_lookup_if_params(ifp, addr);
		if (params == NULL) {
			vty_out(vty, "Ip Address specified is unknown%s",
				VTY_NEWLINE);
			return CMD_WARNING;
		}

		params->auth_type = OSPF_AUTH_NOTSET;
		UNSET_IF_PARAM(params, auth_type);
		if (params != IF_DEF_PARAMS(ifp)) {
			ospf_free_if_params(ifp, addr);
			ospf_if_update_params(ifp, addr);
		}
	} else {
		/*
		 * When a user enters 'no ip ospf authentication'
		 * We should remove all authentication types from
		 * the interface.
		 */
		if ((params->auth_type == OSPF_AUTH_NULL)
		    || (params->auth_type == OSPF_AUTH_CRYPTOGRAPHIC)
		    || (params->auth_type == OSPF_AUTH_SIMPLE)) {
			params->auth_type = OSPF_AUTH_NOTSET;
			UNSET_IF_PARAM(params, auth_type);
		}
/* modified by liwh for bug 44298, 2017-06-22 */
#ifdef _CENTEC_
//      for (rn = route_top (IF_OIFS_PARAMS (ifp)); rn; rn = route_next (rn))
//        {
      for (rn = route_top (IF_OIFS_PARAMS (ifp)); rn; rn = rn_next)
        {
          rn_next = route_next (rn);
#endif
/*liwh end */

			if ((params = rn->info)) {

				if ((params->auth_type == OSPF_AUTH_NULL)
				    || (params->auth_type
					== OSPF_AUTH_CRYPTOGRAPHIC)
				    || (params->auth_type
					== OSPF_AUTH_SIMPLE)) {
					params->auth_type = OSPF_AUTH_NOTSET;
					UNSET_IF_PARAM(params, auth_type);
					if (params != IF_DEF_PARAMS(ifp)) {
						ospf_free_if_params(
							ifp, rn->p.u.prefix4);
						ospf_if_update_params(
							ifp, rn->p.u.prefix4);
					}
				}
			}
		}
	}

	return CMD_SUCCESS;
}


DEFUN (ip_ospf_authentication_key,
       ip_ospf_authentication_key_addr_cmd,
       "ip ospf authentication-key AUTH_KEY [A.B.C.D]",
       "IP Information\n"
       "OSPF interface commands\n"
       "Authentication password (key)\n"
       "The OSPF password (key)\n"
       "Address of interface")
{
	VTY_DECLVAR_CONTEXT(interface, ifp);
	int idx = 0;
	struct in_addr addr;
	struct ospf_if_params *params;

	params = IF_DEF_PARAMS(ifp);

	if (argv_find(argv, argc, "A.B.C.D", &idx)) {
		if (!inet_aton(argv[idx]->arg, &addr)) {
			vty_out(vty,
				"Please specify interface address by A.B.C.D%s",
				VTY_NEWLINE);
			return CMD_WARNING;
		}

		params = ospf_get_if_params(ifp, addr);
		ospf_if_update_params(ifp, addr);
	}

	memset(params->auth_simple, 0, OSPF_AUTH_SIMPLE_SIZE + 1);
	strncpy((char *)params->auth_simple, argv[3]->arg,
		OSPF_AUTH_SIMPLE_SIZE);
	SET_IF_PARAM(params, auth_simple);

	return CMD_SUCCESS;
}

DEFUN_HIDDEN (ospf_authentication_key,
              ospf_authentication_key_cmd,
              "ospf authentication-key AUTH_KEY [A.B.C.D]",
              "OSPF interface commands\n"
              "Authentication password (key)\n"
              "The OSPF password (key)\n"
              "Address of interface\n")
{
	return ip_ospf_authentication_key(self, vty, argc, argv);
}

DEFUN (no_ip_ospf_authentication_key,
       no_ip_ospf_authentication_key_authkey_addr_cmd,
       "no ip ospf authentication-key [AUTH_KEY [A.B.C.D]]",
       NO_STR
       "IP Information\n"
       "OSPF interface commands\n"
       "Authentication password (key)\n"
       "The OSPF password (key)")
{
	VTY_DECLVAR_CONTEXT(interface, ifp);
	int idx = 0;
	struct in_addr addr;
	struct ospf_if_params *params;
	params = IF_DEF_PARAMS(ifp);

	if (argv_find(argv, argc, "A.B.C.D", &idx)) {
		if (!inet_aton(argv[idx]->arg, &addr)) {
			vty_out(vty,
				"Please specify interface address by A.B.C.D%s",
				VTY_NEWLINE);
			return CMD_WARNING;
		}

		params = ospf_lookup_if_params(ifp, addr);
		if (params == NULL)
			return CMD_SUCCESS;
	}

	memset(params->auth_simple, 0, OSPF_AUTH_SIMPLE_SIZE);
	UNSET_IF_PARAM(params, auth_simple);

	if (params != IF_DEF_PARAMS(ifp)) {
		ospf_free_if_params(ifp, addr);
		ospf_if_update_params(ifp, addr);
	}

	return CMD_SUCCESS;
}

DEFUN_HIDDEN (no_ospf_authentication_key,
              no_ospf_authentication_key_authkey_addr_cmd,
              "no ospf authentication-key [AUTH_KEY [A.B.C.D]]",
              NO_STR
              "OSPF interface commands\n"
              "Authentication password (key)\n"
              "The OSPF password (key)")
{
	return no_ip_ospf_authentication_key(self, vty, argc, argv);
}

DEFUN (ip_ospf_message_digest_key,
       ip_ospf_message_digest_key_cmd,
       "ip ospf message-digest-key (1-255) md5 KEY [A.B.C.D]",
       "IP Information\n"
       "OSPF interface commands\n"
       "Message digest authentication password (key)\n"
       "Key ID\n"
       "Use MD5 algorithm\n"
       "The OSPF password (key)\n"
       "Address of interface\n")
{
	VTY_DECLVAR_CONTEXT(interface, ifp);
	struct crypt_key *ck;
	u_char key_id;
	struct in_addr addr;
	struct ospf_if_params *params;

	params = IF_DEF_PARAMS(ifp);
	int idx = 0;

	argv_find(argv, argc, "(1-255)", &idx);
	char *keyid = argv[idx]->arg;
	argv_find(argv, argc, "KEY", &idx);
	char *cryptkey = argv[idx]->arg;

	if (argv_find(argv, argc, "A.B.C.D", &idx)) {
		if (!inet_aton(argv[idx]->arg, &addr)) {
			vty_out(vty,
				"Please specify interface address by A.B.C.D%s",
				VTY_NEWLINE);
			return CMD_WARNING;
		}

		params = ospf_get_if_params(ifp, addr);
		ospf_if_update_params(ifp, addr);
	}

	key_id = strtol(keyid, NULL, 10);
	if (ospf_crypt_key_lookup(params->auth_crypt, key_id) != NULL) {
		vty_out(vty, "OSPF: Key %d already exists%s", key_id,
			VTY_NEWLINE);
		return CMD_WARNING;
	}

	ck = ospf_crypt_key_new();
	ck->key_id = (u_char)key_id;
	memset(ck->auth_key, 0, OSPF_AUTH_MD5_SIZE + 1);
	strncpy((char *)ck->auth_key, cryptkey, OSPF_AUTH_MD5_SIZE);

	ospf_crypt_key_add(params->auth_crypt, ck);
	SET_IF_PARAM(params, auth_crypt);

	return CMD_SUCCESS;
}

DEFUN_HIDDEN (ospf_message_digest_key,
              ospf_message_digest_key_cmd,
              "ospf message-digest-key (1-255) md5 KEY [A.B.C.D]",
              "OSPF interface commands\n"
              "Message digest authentication password (key)\n"
              "Key ID\n"
              "Use MD5 algorithm\n"
              "The OSPF password (key)\n"
              "Address of interface\n")
{
	return ip_ospf_message_digest_key(self, vty, argc, argv);
}

DEFUN (no_ip_ospf_message_digest_key,
       no_ip_ospf_message_digest_key_cmd,
       "no ip ospf message-digest-key (1-255) [md5 KEY] [A.B.C.D]",
        NO_STR
       "IP Information\n"
       "OSPF interface commands\n"
       "Message digest authentication password (key)\n"
       "Key ID\n"
       "Use MD5 algorithm\n"
       "The OSPF password (key)\n"
       "Address of interface\n")
{
	VTY_DECLVAR_CONTEXT(interface, ifp);
	int idx = 0;
	struct crypt_key *ck;
	int key_id;
	struct in_addr addr;
	struct ospf_if_params *params;
	params = IF_DEF_PARAMS(ifp);

	argv_find(argv, argc, "(1-255)", &idx);
	char *keyid = argv[idx]->arg;

	if (argv_find(argv, argc, "A.B.C.D", &idx)) {
		if (!inet_aton(argv[idx]->arg, &addr)) {
			vty_out(vty,
				"Please specify interface address by A.B.C.D%s",
				VTY_NEWLINE);
			return CMD_WARNING;
		}

		params = ospf_lookup_if_params(ifp, addr);
		if (params == NULL)
			return CMD_SUCCESS;
	}

	key_id = strtol(keyid, NULL, 10);
	ck = ospf_crypt_key_lookup(params->auth_crypt, key_id);
	if (ck == NULL) {
		vty_out(vty, "OSPF: Key %d does not exist%s", key_id,
			VTY_NEWLINE);
		return CMD_WARNING;
	}

	ospf_crypt_key_delete(params->auth_crypt, key_id);

	if (params != IF_DEF_PARAMS(ifp)) {
		ospf_free_if_params(ifp, addr);
		ospf_if_update_params(ifp, addr);
	}

	return CMD_SUCCESS;
}

DEFUN_HIDDEN (no_ospf_message_digest_key,
              no_ospf_message_digest_key_cmd,
              "no ospf message-digest-key (1-255) [md5 KEY] [A.B.C.D]",
              NO_STR
              "OSPF interface commands\n"
              "Message digest authentication password (key)\n"
              "Key ID\n"
              "Address of interface")
{
	return no_ip_ospf_message_digest_key(self, vty, argc, argv);
}

DEFUN (ip_ospf_cost,
       ip_ospf_cost_cmd,
       "ip ospf cost (1-65535) [A.B.C.D]",
       "IP Information\n"
       "OSPF interface commands\n"
       "Interface cost\n"
       "Cost\n"
       "Address of interface\n")
{
	VTY_DECLVAR_CONTEXT(interface, ifp);
	int idx = 0;
	u_int32_t cost;
	struct in_addr addr;
	struct ospf_if_params *params;
	params = IF_DEF_PARAMS(ifp);

	// get arguments
	char *coststr = NULL, *ifaddr = NULL;
	coststr = argv_find(argv, argc, "(1-65535)", &idx) ? argv[idx]->arg
							   : NULL;
	ifaddr = argv_find(argv, argc, "A.B.C.D", &idx) ? argv[idx]->arg : NULL;

	cost = strtol(coststr, NULL, 10);

	if (ifaddr) {
		if (!inet_aton(ifaddr, &addr)) {
			vty_out(vty,
				"Please specify interface address by A.B.C.D%s",
				VTY_NEWLINE);
			return CMD_WARNING;
		}

		params = ospf_get_if_params(ifp, addr);
		ospf_if_update_params(ifp, addr);
	}

	SET_IF_PARAM(params, output_cost_cmd);
	params->output_cost_cmd = cost;

	ospf_if_recalculate_output_cost(ifp);

	return CMD_SUCCESS;
}

DEFUN_HIDDEN (ospf_cost,
              ospf_cost_cmd,
              "ospf cost (1-65535) [A.B.C.D]",
              "OSPF interface commands\n"
              "Interface cost\n"
              "Cost\n"
              "Address of interface\n")
{
	return ip_ospf_cost(self, vty, argc, argv);
}

DEFUN (no_ip_ospf_cost,
       no_ip_ospf_cost_cmd,
       "no ip ospf cost [(1-65535)] [A.B.C.D]",
       NO_STR
       "OSPF interface commands\n"
       "Interface cost\n"
       "Address of interface")
{
	VTY_DECLVAR_CONTEXT(interface, ifp);
	int idx = 0;
	struct in_addr addr;
	struct ospf_if_params *params;

	params = IF_DEF_PARAMS(ifp);

	// get arguments
	char *ifaddr = NULL;
	ifaddr = argv_find(argv, argc, "A.B.C.D", &idx) ? argv[idx]->arg : NULL;

	/* According to the semantics we are mimicking "no ip ospf cost N" is
	 * always treated as "no ip ospf cost" regardless of the actual value
	 * of N already configured for the interface. Thus ignore cost. */

	if (ifaddr) {
		if (!inet_aton(ifaddr, &addr)) {
			vty_out(vty,
				"Please specify interface address by A.B.C.D%s",
				VTY_NEWLINE);
			return CMD_WARNING;
		}

		params = ospf_lookup_if_params(ifp, addr);
		if (params == NULL)
			return CMD_SUCCESS;
	}

	UNSET_IF_PARAM(params, output_cost_cmd);

	if (params != IF_DEF_PARAMS(ifp)) {
		ospf_free_if_params(ifp, addr);
		ospf_if_update_params(ifp, addr);
	}

	ospf_if_recalculate_output_cost(ifp);

	return CMD_SUCCESS;
}

DEFUN_HIDDEN (no_ospf_cost,
              no_ospf_cost_cmd,
              "no ospf cost [(1-65535)] [A.B.C.D]",
              NO_STR
              "OSPF interface commands\n"
              "Interface cost\n"
              "Cost\n"
              "Address of interface\n")
{
	return no_ip_ospf_cost(self, vty, argc, argv);
}

static void ospf_nbr_timer_update(struct ospf_interface *oi)
{
	struct route_node *rn;
	struct ospf_neighbor *nbr;

	for (rn = route_top(oi->nbrs); rn; rn = route_next(rn))
		if ((nbr = rn->info)) {
			nbr->v_inactivity = OSPF_IF_PARAM(oi, v_wait);
			nbr->v_db_desc = OSPF_IF_PARAM(oi, retransmit_interval);
			nbr->v_ls_req = OSPF_IF_PARAM(oi, retransmit_interval);
			nbr->v_ls_upd = OSPF_IF_PARAM(oi, retransmit_interval);
		}
}

static int ospf_vty_dead_interval_set(struct vty *vty, const char *interval_str,
				      const char *nbr_str,
				      const char *fast_hello_str)
{
	VTY_DECLVAR_CONTEXT(interface, ifp);
	u_int32_t seconds;
	u_char hellomult;
	struct in_addr addr;
	int ret;
	struct ospf_if_params *params;
	struct ospf_interface *oi;
	struct route_node *rn;

	params = IF_DEF_PARAMS(ifp);

	if (nbr_str) {
		ret = inet_aton(nbr_str, &addr);
		if (!ret) {
			vty_out(vty,
				"Please specify interface address by A.B.C.D%s",
				VTY_NEWLINE);
			return CMD_WARNING;
		}

		params = ospf_get_if_params(ifp, addr);
		ospf_if_update_params(ifp, addr);
	}

	if (interval_str) {
		VTY_GET_INTEGER_RANGE("Router Dead Interval", seconds,
				      interval_str, 1, 65535);

		/* reset fast_hello too, just to be sure */
		UNSET_IF_PARAM(params, fast_hello);
		params->fast_hello = OSPF_FAST_HELLO_DEFAULT;
	} else if (fast_hello_str) {
		VTY_GET_INTEGER_RANGE("Hello Multiplier", hellomult,
				      fast_hello_str, 1, 10);
		/* 1s dead-interval with sub-second hellos desired */
		seconds = OSPF_ROUTER_DEAD_INTERVAL_MINIMAL;
		SET_IF_PARAM(params, fast_hello);
		params->fast_hello = hellomult;
	} else {
		vty_out(vty,
			"Please specify dead-interval or hello-multiplier%s",
			VTY_NEWLINE);
		return CMD_WARNING;
	}

	SET_IF_PARAM(params, v_wait);
	params->v_wait = seconds;

	/* Update timer values in neighbor structure. */
	if (nbr_str) {
		struct ospf *ospf;
		if ((ospf = ospf_lookup())) {
			oi = ospf_if_lookup_by_local_addr(ospf, ifp, addr);
			if (oi)
				ospf_nbr_timer_update(oi);
		}
	} else {
		for (rn = route_top(IF_OIFS(ifp)); rn; rn = route_next(rn))
			if ((oi = rn->info))
				ospf_nbr_timer_update(oi);
	}

	return CMD_SUCCESS;
}

DEFUN (ip_ospf_dead_interval,
       ip_ospf_dead_interval_cmd,
       "ip ospf dead-interval (1-65535) [A.B.C.D]",
       "IP Information\n"
       "OSPF interface commands\n"
       "Interval time after which a neighbor is declared down\n"
       "Seconds\n"
       "Address of interface\n")
{
	int idx = 0;
	char *interval = argv_find(argv, argc, "(1-65535)", &idx)
				 ? argv[idx]->arg
				 : NULL;
	char *ifaddr =
		argv_find(argv, argc, "A.B.C.D", &idx) ? argv[idx]->arg : NULL;
	return ospf_vty_dead_interval_set(vty, interval, ifaddr, NULL);
}


DEFUN_HIDDEN (ospf_dead_interval,
              ospf_dead_interval_cmd,
              "ospf dead-interval (1-65535) [A.B.C.D]",
              "OSPF interface commands\n"
              "Interval time after which a neighbor is declared down\n"
              "Seconds\n"
              "Address of interface\n")
{
	return ip_ospf_dead_interval(self, vty, argc, argv);
}

DEFUN (ip_ospf_dead_interval_minimal,
       ip_ospf_dead_interval_minimal_addr_cmd,
       "ip ospf dead-interval minimal hello-multiplier (1-10) [A.B.C.D]",
       "IP Information\n"
       "OSPF interface commands\n"
       "Interval time after which a neighbor is declared down\n"
       "Minimal 1s dead-interval with fast sub-second hellos\n"
       "Hello multiplier factor\n"
       "Number of Hellos to send each second\n"
       "Address of interface\n")
{
	int idx_number = 5;
	int idx_ipv4 = 6;
	if (argc == 7)
		return ospf_vty_dead_interval_set(
			vty, NULL, argv[idx_ipv4]->arg, argv[idx_number]->arg);
	else
		return ospf_vty_dead_interval_set(vty, NULL, NULL,
						  argv[idx_number]->arg);
}

DEFUN (no_ip_ospf_dead_interval,
       no_ip_ospf_dead_interval_cmd,
       "no ip ospf dead-interval [<(1-65535)|minimal hello-multiplier (1-10)> [A.B.C.D]]",
       NO_STR
       "IP Information\n"
       "OSPF interface commands\n"
       "Interval time after which a neighbor is declared down\n"
       "Seconds\n"
       "Address of interface")
{
	VTY_DECLVAR_CONTEXT(interface, ifp);
	int idx_ipv4 = argc - 1;
	struct in_addr addr = {.s_addr = 0L};
	int ret;
	struct ospf_if_params *params;
	struct ospf_interface *oi;
	struct route_node *rn;

	params = IF_DEF_PARAMS(ifp);

	if (argv[idx_ipv4]->type == IPV4_TKN) {
		ret = inet_aton(argv[idx_ipv4]->arg, &addr);
		if (!ret) {
			vty_out(vty,
				"Please specify interface address by A.B.C.D%s",
				VTY_NEWLINE);
			return CMD_WARNING;
		}

		params = ospf_lookup_if_params(ifp, addr);
		if (params == NULL)
			return CMD_SUCCESS;
	}

	UNSET_IF_PARAM(params, v_wait);
/* modified by liwh for bug 43976, 2017-08-07 */
#ifdef _CENTEC_
  //params->v_wait = OSPF_ROUTER_DEAD_INTERVAL_DEFAULT;
  params->v_wait = params->v_hello * GLB_OSPF_DEAD_INTERVAL_MULTI_HELLO;
#endif

	UNSET_IF_PARAM(params, fast_hello);
	params->fast_hello = OSPF_FAST_HELLO_DEFAULT;

	if (params != IF_DEF_PARAMS(ifp)) {
		ospf_free_if_params(ifp, addr);
		ospf_if_update_params(ifp, addr);
	}

	/* Update timer values in neighbor structure. */
	if (argc == 1) {
		struct ospf *ospf;

		if ((ospf = ospf_lookup())) {
			oi = ospf_if_lookup_by_local_addr(ospf, ifp, addr);
			if (oi)
				ospf_nbr_timer_update(oi);
		}
	} else {
		for (rn = route_top(IF_OIFS(ifp)); rn; rn = route_next(rn))
			if ((oi = rn->info))
				ospf_nbr_timer_update(oi);
	}

	return CMD_SUCCESS;
}

DEFUN_HIDDEN (no_ospf_dead_interval,
              no_ospf_dead_interval_cmd,
              "no ospf dead-interval [<(1-65535)|minimal hello-multiplier (1-10)> [A.B.C.D]]",
              NO_STR
              "OSPF interface commands\n"
              "Interval time after which a neighbor is declared down\n"
              "Seconds\n"
              "Address of interface")
{
	return no_ip_ospf_dead_interval(self, vty, argc, argv);
}
/* modified by liwh for bug 43976, 2017-08-07 */
#ifdef _CENTEC_
static int
ospf_if_update_dead_interval (struct interface *ifp, struct ospf_if_params *params, u_int32_t seconds)
{
  struct ospf_interface *oi;
  struct route_node *rn;

  if (params->v_wait == (params->v_hello * GLB_OSPF_DEAD_INTERVAL_MULTI_HELLO))
    {                        
      /* reset fast_hello too, just to be sure */
      UNSET_IF_PARAM (params, fast_hello);
      params->fast_hello = OSPF_FAST_HELLO_DEFAULT;
    
      SET_IF_PARAM (params, v_wait);
      params->v_wait = seconds * GLB_OSPF_DEAD_INTERVAL_MULTI_HELLO;

      for (rn = route_top (IF_OIFS (ifp)); rn; rn = route_next (rn))
        if ((oi = rn->info))
          ospf_nbr_timer_update (oi);
    }

  return CMD_SUCCESS;
}
#endif
/*liwh end */

DEFUN (ip_ospf_hello_interval,
       ip_ospf_hello_interval_cmd,
       "ip ospf hello-interval (1-65535) [A.B.C.D]",
       "IP Information\n"
       "OSPF interface commands\n"
       "Time between HELLO packets\n"
       "Seconds\n"
       "Address of interface\n")
{
	VTY_DECLVAR_CONTEXT(interface, ifp);
	int idx = 0;
	struct in_addr addr;
	struct ospf_if_params *params;
	params = IF_DEF_PARAMS(ifp);
	u_int32_t seconds = 0;

	argv_find(argv, argc, "(1-65535)", &idx);
	seconds = strtol(argv[idx]->arg, NULL, 10);

	if (argv_find(argv, argc, "A.B.C.D", &idx)) {
		if (!inet_aton(argv[idx]->arg, &addr)) {
			vty_out(vty,
				"Please specify interface address by A.B.C.D%s",
				VTY_NEWLINE);
			return CMD_WARNING;
		}

		params = ospf_get_if_params(ifp, addr);
		ospf_if_update_params(ifp, addr);
	}

/* modified by liwh for bug 43976, 2017-08-07 */
#ifdef _CENTEC_
  ospf_if_update_dead_interval (ifp, params, seconds);
#endif
/*liwh end */

	SET_IF_PARAM(params, v_hello);
	params->v_hello = seconds;

	return CMD_SUCCESS;
}

DEFUN_HIDDEN (ospf_hello_interval,
              ospf_hello_interval_cmd,
              "ospf hello-interval (1-65535) [A.B.C.D]",
              "OSPF interface commands\n"
              "Time between HELLO packets\n"
              "Seconds\n"
              "Address of interface\n")
{
	return ip_ospf_hello_interval(self, vty, argc, argv);
}

DEFUN (no_ip_ospf_hello_interval,
       no_ip_ospf_hello_interval_cmd,
       "no ip ospf hello-interval [(1-65535) [A.B.C.D]]",
       NO_STR
       "IP Information\n"
       "OSPF interface commands\n"
       "Time between HELLO packets\n" // ignored
       "Seconds\n"
       "Address of interface\n")
{
	VTY_DECLVAR_CONTEXT(interface, ifp);
	int idx = 0;
	struct in_addr addr;
	struct ospf_if_params *params;
	params = IF_DEF_PARAMS(ifp);

	if (argv_find(argv, argc, "A.B.C.D", &idx)) {
		if (!inet_aton(argv[idx]->arg, &addr)) {
			vty_out(vty,
				"Please specify interface address by A.B.C.D%s",
				VTY_NEWLINE);
			return CMD_WARNING;
		}

		params = ospf_lookup_if_params(ifp, addr);
		if (params == NULL)
			return CMD_SUCCESS;
	}

  /* modified by liwh for bug 43976, 2017-08-07 */
#ifdef _CENTEC_
  ospf_if_update_dead_interval (ifp, params, OSPF_HELLO_INTERVAL_DEFAULT);
#endif
  /*liwh end */

	UNSET_IF_PARAM(params, v_hello);
	params->v_hello = OSPF_HELLO_INTERVAL_DEFAULT;

	if (params != IF_DEF_PARAMS(ifp)) {
		ospf_free_if_params(ifp, addr);
		ospf_if_update_params(ifp, addr);
	}

	return CMD_SUCCESS;
}

DEFUN_HIDDEN (no_ospf_hello_interval,
              no_ospf_hello_interval_cmd,
              "no ospf hello-interval [(1-65535) [A.B.C.D]]",
              NO_STR
              "OSPF interface commands\n"
              "Time between HELLO packets\n" // ignored
              "Seconds\n"
              "Address of interface\n")
{
	return no_ip_ospf_hello_interval(self, vty, argc, argv);
}

DEFUN (ip_ospf_network,
       ip_ospf_network_cmd,
       "ip ospf network <broadcast|non-broadcast|point-to-multipoint|point-to-point>",
       "IP Information\n"
       "OSPF interface commands\n"
       "Network type\n"
       "Specify OSPF broadcast multi-access network\n"
       "Specify OSPF NBMA network\n"
       "Specify OSPF point-to-multipoint network\n"
       "Specify OSPF point-to-point network\n")
{
	VTY_DECLVAR_CONTEXT(interface, ifp);
	int idx = 0;
	int old_type = IF_DEF_PARAMS(ifp)->type;
	struct route_node *rn;

	if (old_type == OSPF_IFTYPE_LOOPBACK) {
		vty_out(vty,
			"This is a loopback interface. Can't set network type.%s",
			VTY_NEWLINE);
		return CMD_WARNING;
	}

	if (argv_find(argv, argc, "broadcast", &idx))
		IF_DEF_PARAMS(ifp)->type = OSPF_IFTYPE_BROADCAST;
	else if (argv_find(argv, argc, "non-broadcast", &idx))
		IF_DEF_PARAMS(ifp)->type = OSPF_IFTYPE_NBMA;
	else if (argv_find(argv, argc, "point-to-multipoint", &idx))
		IF_DEF_PARAMS(ifp)->type = OSPF_IFTYPE_POINTOMULTIPOINT;
	else if (argv_find(argv, argc, "point-to-point", &idx))
		IF_DEF_PARAMS(ifp)->type = OSPF_IFTYPE_POINTOPOINT;

	if (IF_DEF_PARAMS(ifp)->type == old_type)
		return CMD_SUCCESS;

	SET_IF_PARAM(IF_DEF_PARAMS(ifp), type);

	for (rn = route_top(IF_OIFS(ifp)); rn; rn = route_next(rn)) {
		struct ospf_interface *oi = rn->info;

		if (!oi)
			continue;

		oi->type = IF_DEF_PARAMS(ifp)->type;

		if (oi->state > ISM_Down) {
			OSPF_ISM_EVENT_EXECUTE(oi, ISM_InterfaceDown);
			OSPF_ISM_EVENT_EXECUTE(oi, ISM_InterfaceUp);
		}
	}

	return CMD_SUCCESS;
}

DEFUN_HIDDEN (ospf_network,
              ospf_network_cmd,
              "ospf network <broadcast|non-broadcast|point-to-multipoint|point-to-point>",
              "OSPF interface commands\n"
              "Network type\n"
              "Specify OSPF broadcast multi-access network\n"
              "Specify OSPF NBMA network\n"
              "Specify OSPF point-to-multipoint network\n"
              "Specify OSPF point-to-point network\n")
{
	return ip_ospf_network(self, vty, argc, argv);
}

DEFUN (no_ip_ospf_network,
       no_ip_ospf_network_cmd,
       "no ip ospf network [<broadcast|non-broadcast|point-to-multipoint|point-to-point>]",
       NO_STR
       "IP Information\n"
       "OSPF interface commands\n"
       "Network type\n"
       "Specify OSPF broadcast multi-access network\n"
       "Specify OSPF NBMA network\n"
       "Specify OSPF point-to-multipoint network\n"
       "Specify OSPF point-to-point network\n")
{
	VTY_DECLVAR_CONTEXT(interface, ifp);
	int old_type = IF_DEF_PARAMS(ifp)->type;
	struct route_node *rn;

	IF_DEF_PARAMS(ifp)->type = ospf_default_iftype(ifp);

	if (IF_DEF_PARAMS(ifp)->type == old_type)
		return CMD_SUCCESS;

	for (rn = route_top(IF_OIFS(ifp)); rn; rn = route_next(rn)) {
		struct ospf_interface *oi = rn->info;

		if (!oi)
			continue;

		oi->type = IF_DEF_PARAMS(ifp)->type;

		if (oi->state > ISM_Down) {
			OSPF_ISM_EVENT_EXECUTE(oi, ISM_InterfaceDown);
			OSPF_ISM_EVENT_EXECUTE(oi, ISM_InterfaceUp);
		}
	}

	return CMD_SUCCESS;
}

DEFUN_HIDDEN (no_ospf_network,
              no_ospf_network_cmd,
              "no ospf network [<broadcast|non-broadcast|point-to-multipoint|point-to-point>]",
              NO_STR
              "OSPF interface commands\n"
              "Network type\n"
              "Specify OSPF broadcast multi-access network\n"
              "Specify OSPF NBMA network\n"
              "Specify OSPF point-to-multipoint network\n"
              "Specify OSPF point-to-point network\n")
{
	return no_ip_ospf_network(self, vty, argc, argv);
}

DEFUN (ip_ospf_priority,
       ip_ospf_priority_cmd,
       "ip ospf priority (0-255) [A.B.C.D]",
       "IP Information\n"
       "OSPF interface commands\n"
       "Router priority\n"
       "Priority\n"
       "Address of interface")
{
	VTY_DECLVAR_CONTEXT(interface, ifp);
	int idx = 0;
	long priority;
	struct route_node *rn;
	struct in_addr addr;
	struct ospf_if_params *params;
	params = IF_DEF_PARAMS(ifp);

	argv_find(argv, argc, "(0-255)", &idx);
	priority = strtol(argv[idx]->arg, NULL, 10);

	if (argv_find(argv, argc, "A.B.C.D", &idx)) {
		if (!inet_aton(argv[idx]->arg, &addr)) {
			vty_out(vty,
				"Please specify interface address by A.B.C.D%s",
				VTY_NEWLINE);
			return CMD_WARNING;
		}

		params = ospf_get_if_params(ifp, addr);
		ospf_if_update_params(ifp, addr);
	}

	SET_IF_PARAM(params, priority);
	params->priority = priority;

	for (rn = route_top(IF_OIFS(ifp)); rn; rn = route_next(rn)) {
		struct ospf_interface *oi = rn->info;

		if (!oi)
			continue;

		if (PRIORITY(oi) != OSPF_IF_PARAM(oi, priority)) {
			PRIORITY(oi) = OSPF_IF_PARAM(oi, priority);
			OSPF_ISM_EVENT_SCHEDULE(oi, ISM_NeighborChange);
		}
	}

	return CMD_SUCCESS;
}

DEFUN_HIDDEN (ospf_priority,
              ospf_priority_cmd,
              "ospf priority (0-255) [A.B.C.D]",
              "OSPF interface commands\n"
              "Router priority\n"
              "Priority\n"
              "Address of interface")
{
	return ip_ospf_priority(self, vty, argc, argv);
}

DEFUN (no_ip_ospf_priority,
       no_ip_ospf_priority_cmd,
       "no ip ospf priority [(0-255) [A.B.C.D]]",
       NO_STR
       "IP Information\n"
       "OSPF interface commands\n"
       "Router priority\n" // ignored
       "Priority\n"
       "Address of interface")
{
	VTY_DECLVAR_CONTEXT(interface, ifp);
	int idx = 0;
	struct route_node *rn;
	struct in_addr addr;
	struct ospf_if_params *params;

	params = IF_DEF_PARAMS(ifp);

	if (argv_find(argv, argc, "A.B.C.D", &idx)) {
		if (!inet_aton(argv[idx]->arg, &addr)) {
			vty_out(vty,
				"Please specify interface address by A.B.C.D%s",
				VTY_NEWLINE);
			return CMD_WARNING;
		}

		params = ospf_lookup_if_params(ifp, addr);
		if (params == NULL)
			return CMD_SUCCESS;
	}

	UNSET_IF_PARAM(params, priority);
	params->priority = OSPF_ROUTER_PRIORITY_DEFAULT;

	if (params != IF_DEF_PARAMS(ifp)) {
		ospf_free_if_params(ifp, addr);
		ospf_if_update_params(ifp, addr);
	}

	for (rn = route_top(IF_OIFS(ifp)); rn; rn = route_next(rn)) {
		struct ospf_interface *oi = rn->info;

		if (!oi)
			continue;

		if (PRIORITY(oi) != OSPF_IF_PARAM(oi, priority)) {
			PRIORITY(oi) = OSPF_IF_PARAM(oi, priority);
			OSPF_ISM_EVENT_SCHEDULE(oi, ISM_NeighborChange);
		}
	}

	return CMD_SUCCESS;
}

DEFUN_HIDDEN (no_ospf_priority,
              no_ospf_priority_cmd,
              "no ospf priority [(0-255) [A.B.C.D]]",
              NO_STR
              "OSPF interface commands\n"
              "Router priority\n"
              "Priority\n"
              "Address of interface")
{
	return no_ip_ospf_priority(self, vty, argc, argv);
}

DEFUN (ip_ospf_retransmit_interval,
       ip_ospf_retransmit_interval_addr_cmd,
       "ip ospf retransmit-interval (3-65535) [A.B.C.D]",
       "IP Information\n"
       "OSPF interface commands\n"
       "Time between retransmitting lost link state advertisements\n"
       "Seconds\n"
       "Address of interface")
{
	VTY_DECLVAR_CONTEXT(interface, ifp);
	int idx = 0;
	u_int32_t seconds;
	struct in_addr addr;
	struct ospf_if_params *params;
	params = IF_DEF_PARAMS(ifp);

	argv_find(argv, argc, "(3-65535)", &idx);
	seconds = strtol(argv[idx]->arg, NULL, 10);

	if (argv_find(argv, argc, "A.B.C.D", &idx)) {
		if (!inet_aton(argv[idx]->arg, &addr)) {
			vty_out(vty,
				"Please specify interface address by A.B.C.D%s",
				VTY_NEWLINE);
			return CMD_WARNING;
		}

		params = ospf_get_if_params(ifp, addr);
		ospf_if_update_params(ifp, addr);
	}

	SET_IF_PARAM(params, retransmit_interval);
	params->retransmit_interval = seconds;

	return CMD_SUCCESS;
}

DEFUN_HIDDEN (ospf_retransmit_interval,
              ospf_retransmit_interval_cmd,
              "ospf retransmit-interval (3-65535) [A.B.C.D]",
              "OSPF interface commands\n"
              "Time between retransmitting lost link state advertisements\n"
              "Seconds\n"
              "Address of interface")
{
	return ip_ospf_retransmit_interval(self, vty, argc, argv);
}

DEFUN (no_ip_ospf_retransmit_interval,
       no_ip_ospf_retransmit_interval_addr_cmd,
       "no ip ospf retransmit-interval [(3-65535)] [A.B.C.D]",
       NO_STR
       "IP Information\n"
       "OSPF interface commands\n"
       "Time between retransmitting lost link state advertisements\n"
       "Seconds\n"
       "Address of interface\n")
{
	VTY_DECLVAR_CONTEXT(interface, ifp);
	int idx = 0;
	struct in_addr addr;
	struct ospf_if_params *params;

	params = IF_DEF_PARAMS(ifp);

	if (argv_find(argv, argc, "A.B.C.D", &idx)) {
		if (!inet_aton(argv[idx]->arg, &addr)) {
			vty_out(vty,
				"Please specify interface address by A.B.C.D%s",
				VTY_NEWLINE);
			return CMD_WARNING;
		}

		params = ospf_lookup_if_params(ifp, addr);
		if (params == NULL)
			return CMD_SUCCESS;
	}

	UNSET_IF_PARAM(params, retransmit_interval);
	params->retransmit_interval = OSPF_RETRANSMIT_INTERVAL_DEFAULT;

	if (params != IF_DEF_PARAMS(ifp)) {
		ospf_free_if_params(ifp, addr);
		ospf_if_update_params(ifp, addr);
	}

	return CMD_SUCCESS;
}

DEFUN_HIDDEN (no_ospf_retransmit_interval,
       no_ospf_retransmit_interval_cmd,
       "no ospf retransmit-interval [(3-65535)] [A.B.C.D]",
       NO_STR
       "OSPF interface commands\n"
       "Time between retransmitting lost link state advertisements\n"
       "Seconds\n"
       "Address of interface\n")
{
	return no_ip_ospf_retransmit_interval(self, vty, argc, argv);
}

DEFUN (ip_ospf_transmit_delay,
       ip_ospf_transmit_delay_addr_cmd,
       "ip ospf transmit-delay (1-65535) [A.B.C.D]",
       "IP Information\n"
       "OSPF interface commands\n"
       "Link state transmit delay\n"
       "Seconds\n"
       "Address of interface")
{
	VTY_DECLVAR_CONTEXT(interface, ifp);
	int idx = 0;
	u_int32_t seconds;
	struct in_addr addr;
	struct ospf_if_params *params;

	params = IF_DEF_PARAMS(ifp);
	argv_find(argv, argc, "(1-65535)", &idx);
	seconds = strtol(argv[idx]->arg, NULL, 10);

	if (argv_find(argv, argc, "A.B.C.D", &idx)) {
		if (!inet_aton(argv[idx]->arg, &addr)) {
			vty_out(vty,
				"Please specify interface address by A.B.C.D%s",
				VTY_NEWLINE);
			return CMD_WARNING;
		}

		params = ospf_get_if_params(ifp, addr);
		ospf_if_update_params(ifp, addr);
	}

	SET_IF_PARAM(params, transmit_delay);
	params->transmit_delay = seconds;

	return CMD_SUCCESS;
}

DEFUN_HIDDEN (ospf_transmit_delay,
              ospf_transmit_delay_cmd,
              "ospf transmit-delay (1-65535) [A.B.C.D]",
              "OSPF interface commands\n"
              "Link state transmit delay\n"
              "Seconds\n"
              "Address of interface")
{
	return ip_ospf_transmit_delay(self, vty, argc, argv);
}

DEFUN (no_ip_ospf_transmit_delay,
       no_ip_ospf_transmit_delay_addr_cmd,
       "no ip ospf transmit-delay [(1-65535)] [A.B.C.D]",
       NO_STR
       "IP Information\n"
       "OSPF interface commands\n"
       "Link state transmit delay\n"
       "Address of interface")
{
	VTY_DECLVAR_CONTEXT(interface, ifp);
	int idx = 0;
	struct in_addr addr;
	struct ospf_if_params *params;

	params = IF_DEF_PARAMS(ifp);

	if (argv_find(argv, argc, "A.B.C.D", &idx)) {
		if (!inet_aton(argv[idx]->arg, &addr)) {
			vty_out(vty,
				"Please specify interface address by A.B.C.D%s",
				VTY_NEWLINE);
			return CMD_WARNING;
		}

		params = ospf_lookup_if_params(ifp, addr);
		if (params == NULL)
			return CMD_SUCCESS;
	}

	UNSET_IF_PARAM(params, transmit_delay);
	params->transmit_delay = OSPF_TRANSMIT_DELAY_DEFAULT;

	if (params != IF_DEF_PARAMS(ifp)) {
		ospf_free_if_params(ifp, addr);
		ospf_if_update_params(ifp, addr);
	}

	return CMD_SUCCESS;
}


DEFUN_HIDDEN (no_ospf_transmit_delay,
              no_ospf_transmit_delay_cmd,
              "no ospf transmit-delay",
              NO_STR
              "OSPF interface commands\n"
              "Link state transmit delay\n")
{
	return no_ip_ospf_transmit_delay(self, vty, argc, argv);
}

DEFUN (ip_ospf_area,
       ip_ospf_area_cmd,
       "ip ospf [(1-65535)] area <A.B.C.D|(0-4294967295)>",
       "IP Information\n"
       "OSPF interface commands\n"
       "Instance ID\n"
       "Enable OSPF on this interface\n"
       "OSPF area ID in IP address format\n"
       "OSPF area ID as a decimal value\n")
{
	VTY_DECLVAR_CONTEXT(interface, ifp);
	int idx = 0;
	int format, ret;
	struct in_addr area_id;
	struct ospf *ospf;
	struct ospf_if_params *params;
	struct route_node *rn;
	u_short instance = 0;

	if (argv_find(argv, argc, "(1-65535)", &idx))
		instance = strtol(argv[idx]->arg, NULL, 10);
	char *areaid = argv[argc - 1]->arg;

	ospf = ospf_lookup_instance(instance);
	if (ospf == NULL) {
		params = IF_DEF_PARAMS(ifp);
		if (OSPF_IF_PARAM_CONFIGURED(params, if_area)) {
			ospf_interface_unset(ifp);
			ospf = ospf_lookup();
			ospf->if_ospf_cli_count--;
		}
		return CMD_SUCCESS;
	}

	ret = str2area_id(areaid, &area_id, &format);
	if (ret < 0) {
		vty_out(vty, "Please specify area by A.B.C.D|<0-4294967295>%s",
			VTY_NEWLINE);
		return CMD_WARNING;
	}
	if (memcmp(ifp->name, "VLINK", 5) == 0) {
		vty_out(vty, "Cannot enable OSPF on a virtual link.%s",
			VTY_NEWLINE);
		return CMD_WARNING;
	}

	params = IF_DEF_PARAMS(ifp);
	if (OSPF_IF_PARAM_CONFIGURED(params, if_area)) {
		vty_out(vty,
			"Must remove previous area config before changing ospf area %s",
			VTY_NEWLINE);
		return CMD_WARNING;
	}

	for (rn = route_top(ospf->networks); rn; rn = route_next(rn)) {
		if (rn->info != NULL) {
			vty_out(vty,
				"Please remove all network commands first.%s",
				VTY_NEWLINE);
			return CMD_WARNING;
		}
	}

	/* enable ospf on this interface with area_id */
	ospf_interface_set(ifp, area_id);
	ospf->if_ospf_cli_count++;

	return CMD_SUCCESS;
}

DEFUN (no_ip_ospf_area,
       no_ip_ospf_area_cmd,
       "no ip ospf [(1-65535)] area [<A.B.C.D|(0-4294967295)>]",
       NO_STR
       "IP Information\n"
       "OSPF interface commands\n"
       "Instance ID\n"
       "Disable OSPF on this interface\n"
       "OSPF area ID in IP address format\n"
       "OSPF area ID as a decimal value\n")
{
	VTY_DECLVAR_CONTEXT(interface, ifp);
	int idx = 0;
	struct ospf *ospf;
	struct ospf_if_params *params;
	u_short instance = 0;

	if (argv_find(argv, argc, "(1-65535)", &idx))
		instance = strtol(argv[idx]->arg, NULL, 10);

	if ((ospf = ospf_lookup_instance(instance)) == NULL)
		return CMD_SUCCESS;

	params = IF_DEF_PARAMS(ifp);
	if (!OSPF_IF_PARAM_CONFIGURED(params, if_area)) {
		vty_out(vty,
			"Can't find specified interface area configuration.%s",
			VTY_NEWLINE);
		return CMD_WARNING;
	}

	ospf_interface_unset(ifp);
	ospf->if_ospf_cli_count--;
	return CMD_SUCCESS;
}

DEFUN (ospf_redistribute_source,
       ospf_redistribute_source_cmd,
       "redistribute " FRR_REDIST_STR_OSPFD " [<metric (0-16777214)|metric-type (1-2)|route-map WORD>]",
       REDIST_STR
       FRR_REDIST_HELP_STR_OSPFD
       "Metric for redistributed routes\n"
       "OSPF default metric\n"
       "OSPF exterior metric type for redistributed routes\n"
       "Set OSPF External Type 1 metrics\n"
       "Set OSPF External Type 2 metrics\n"
       "Route map reference\n"
       "Pointer to route-map entries\n")
{
	VTY_DECLVAR_CONTEXT(ospf, ospf);
	int idx_protocol = 1;
	int source;
	int type = -1;
	int metric = -1;
	struct ospf_redist *red;
	int idx = 0;

	if (!ospf)
		return CMD_SUCCESS;

	/* Get distribute source. */
	source = proto_redistnum(AFI_IP, argv[idx_protocol]->text);
	if (source < 0)
		return CMD_WARNING;

	red = ospf_redist_add(ospf, source, 0);

	/* Get metric value. */
	if (argv_find(argv, argc, "(0-16777214)", &idx)) {
		if (!str2metric(argv[idx]->arg, &metric))
			return CMD_WARNING;
	}
	/* Get metric type. */
	else if (argv_find(argv, argc, "(1-2)", &idx)) {
		if (!str2metric_type(argv[idx]->arg, &type))
			return CMD_WARNING;
	}
	/* Get route-map */
	else if (argv_find(argv, argc, "WORD", &idx)) {
		ospf_routemap_set(red, argv[idx]->arg);
	} else
		ospf_routemap_unset(red);

	return ospf_redistribute_set(ospf, source, 0, type, metric);
}

DEFUN (no_ospf_redistribute_source,
       no_ospf_redistribute_source_cmd,
       "no redistribute " FRR_REDIST_STR_OSPFD " [<metric (0-16777214)|metric-type (1-2)|route-map WORD>]",
       NO_STR
       REDIST_STR
       FRR_REDIST_HELP_STR_OSPFD
       "Metric for redistributed routes\n"
       "OSPF default metric\n"
       "OSPF exterior metric type for redistributed routes\n"
       "Set OSPF External Type 1 metrics\n"
       "Set OSPF External Type 2 metrics\n"
       "Route map reference\n"
       "Pointer to route-map entries\n")
{
	VTY_DECLVAR_CONTEXT(ospf, ospf);
	int idx_protocol = 2;
	int source;
	struct ospf_redist *red;

	source = proto_redistnum(AFI_IP, argv[idx_protocol]->text);
	if (source < 0)
		return CMD_WARNING;

	red = ospf_redist_lookup(ospf, source, 0);
	if (!red)
		return CMD_SUCCESS;

	ospf_routemap_unset(red);
	return ospf_redistribute_unset(ospf, source, 0);
}

DEFUN (ospf_redistribute_instance_source,
       ospf_redistribute_instance_source_cmd,
       "redistribute <ospf|table> (1-65535) [{metric (0-16777214)|metric-type (1-2)|route-map WORD}]",
       REDIST_STR
       "Open Shortest Path First\n"
       "Non-main Kernel Routing Table\n"
       "Instance ID/Table ID\n"
       "Metric for redistributed routes\n"
       "OSPF default metric\n"
       "OSPF exterior metric type for redistributed routes\n"
       "Set OSPF External Type 1 metrics\n"
       "Set OSPF External Type 2 metrics\n"
       "Route map reference\n"
       "Pointer to route-map entries\n")
{
	VTY_DECLVAR_CONTEXT(ospf, ospf);
	int idx_ospf_table = 1;
	int idx_number = 2;
	int idx = 3;
	int source;
	int type = -1;
	int metric = -1;
	u_short instance;
	struct ospf_redist *red;

	if (!ospf)
		return CMD_SUCCESS;

	source = proto_redistnum(AFI_IP, argv[idx_ospf_table]->text);

	VTY_GET_INTEGER("Instance ID", instance, argv[idx_number]->arg);

	if (!ospf)
		return CMD_SUCCESS;

	if ((source == ZEBRA_ROUTE_OSPF) && !ospf->instance) {
		vty_out(vty,
			"Instance redistribution in non-instanced OSPF not allowed%s",
			VTY_NEWLINE);
		return CMD_WARNING;
	}

	if ((source == ZEBRA_ROUTE_OSPF) && (ospf->instance == instance)) {
		vty_out(vty, "Same instance OSPF redistribution not allowed%s",
			VTY_NEWLINE);
		return CMD_WARNING;
	}

	/* Get metric value. */
	if (argv_find(argv, argc, "metric", &idx))
		if (!str2metric(argv[idx + 1]->arg, &metric))
			return CMD_WARNING;

	idx = 3;
	/* Get metric type. */
	if (argv_find(argv, argc, "metric-type", &idx))
		if (!str2metric_type(argv[idx + 1]->arg, &type))
			return CMD_WARNING;

	red = ospf_redist_add(ospf, source, instance);

	idx = 3;
	if (argv_find(argv, argc, "route-map", &idx))
		ospf_routemap_set(red, argv[idx + 1]->arg);
	else
		ospf_routemap_unset(red);

	return ospf_redistribute_set(ospf, source, instance, type, metric);
}

DEFUN (no_ospf_redistribute_instance_source,
       no_ospf_redistribute_instance_source_cmd,
       "no redistribute <ospf|table> (1-65535) [{metric (0-16777214)|metric-type (1-2)|route-map WORD}]",
       NO_STR
       REDIST_STR
       "Open Shortest Path First\n"
       "Non-main Kernel Routing Table\n"
       "Instance ID/Table Id\n"
       "Metric for redistributed routes\n"
       "OSPF default metric\n"
       "OSPF exterior metric type for redistributed routes\n"
       "Set OSPF External Type 1 metrics\n"
       "Set OSPF External Type 2 metrics\n"
       "Route map reference\n"
       "Pointer to route-map entries\n")
{
	VTY_DECLVAR_CONTEXT(ospf, ospf);
	int idx_ospf_table = 2;
	int idx_number = 3;
	u_int instance;
	struct ospf_redist *red;
	int source;

	if (strncmp(argv[idx_ospf_table]->arg, "o", 1) == 0)
		source = ZEBRA_ROUTE_OSPF;
	else
		source = ZEBRA_ROUTE_TABLE;

	VTY_GET_INTEGER("Instance ID", instance, argv[idx_number]->arg);

	if ((source == ZEBRA_ROUTE_OSPF) && !ospf->instance) {
		vty_out(vty,
			"Instance redistribution in non-instanced OSPF not allowed%s",
			VTY_NEWLINE);
		return CMD_WARNING;
	}

	if ((source == ZEBRA_ROUTE_OSPF) && (ospf->instance == instance)) {
		vty_out(vty, "Same instance OSPF redistribution not allowed%s",
			VTY_NEWLINE);
		return CMD_WARNING;
	}

	red = ospf_redist_lookup(ospf, source, instance);
	if (!red)
		return CMD_SUCCESS;

	ospf_routemap_unset(red);
	return ospf_redistribute_unset(ospf, source, instance);
}

DEFUN (ospf_distribute_list_out,
       ospf_distribute_list_out_cmd,
       "distribute-list WORD out " FRR_REDIST_STR_OSPFD,
       "Filter networks in routing updates\n"
       "Access-list name\n"
       OUT_STR
       FRR_REDIST_HELP_STR_OSPFD)
{
	VTY_DECLVAR_CONTEXT(ospf, ospf);
	int idx_word = 1;
	int source;

	char *proto = argv[argc - 1]->text;

	/* Get distribute source. */
	source = proto_redistnum(AFI_IP, proto);
	if (source < 0)
		return CMD_WARNING;

	return ospf_distribute_list_out_set(ospf, source, argv[idx_word]->arg);
}

DEFUN (no_ospf_distribute_list_out,
       no_ospf_distribute_list_out_cmd,
       "no distribute-list WORD out " FRR_REDIST_STR_OSPFD,
       NO_STR
       "Filter networks in routing updates\n"
       "Access-list name\n"
       OUT_STR
       FRR_REDIST_HELP_STR_OSPFD)
{
	VTY_DECLVAR_CONTEXT(ospf, ospf);
	int idx_word = 2;
	int source;

	char *proto = argv[argc - 1]->text;
	source = proto_redistnum(AFI_IP, proto);
	if (source < 0)
		return CMD_WARNING;

	return ospf_distribute_list_out_unset(ospf, source,
					      argv[idx_word]->arg);
}

/* Default information originate. */
DEFUN (ospf_default_information_originate,
       ospf_default_information_originate_cmd,
       "default-information originate [<always|metric (0-16777214)|metric-type (1-2)|route-map WORD>]",
       "Control distribution of default information\n"
       "Distribute a default route\n"
       "Always advertise default route\n"
       "OSPF default metric\n"
       "OSPF metric\n"
       "OSPF metric type for default routes\n"
       "Set OSPF External Type 1 metrics\n"
       "Set OSPF External Type 2 metrics\n"
       "Route map reference\n"
       "Pointer to route-map entries\n")
{
	VTY_DECLVAR_CONTEXT(ospf, ospf);
	int default_originate = DEFAULT_ORIGINATE_ZEBRA;
	int type = -1;
	int metric = -1;
	struct ospf_redist *red;
	int idx = 0;

	red = ospf_redist_add(ospf, DEFAULT_ROUTE, 0);

	/* Check whether "always" was specified */
	if (argv_find(argv, argc, "always", &idx))
		default_originate = DEFAULT_ORIGINATE_ALWAYS;
	/* Get metric value */
	else if (argv_find(argv, argc, "(0-16777214)", &idx)) {
		if (!str2metric(argv[idx]->arg, &metric))
			return CMD_WARNING;
	}
	/* Get metric type. */
	else if (argv_find(argv, argc, "(1-2)", &idx)) {
		if (!str2metric_type(argv[idx]->arg, &type))
			return CMD_WARNING;
	}
	/* Get route-map */
	else if (argv_find(argv, argc, "WORD", &idx))
		ospf_routemap_set(red, argv[idx]->arg);
	else
		ospf_routemap_unset(red);

	return ospf_redistribute_default_set(ospf, default_originate, type,
					     metric);
}

DEFUN (no_ospf_default_information_originate,
       no_ospf_default_information_originate_cmd,
       "no default-information originate [<always|metric (0-16777214)|metric-type (1-2)|route-map WORD>]",
       NO_STR
       "Control distribution of default information\n"
       "Distribute a default route\n"
       "Always advertise default route\n"
       "OSPF default metric\n"
       "OSPF metric\n"
       "OSPF metric type for default routes\n"
       "Set OSPF External Type 1 metrics\n"
       "Set OSPF External Type 2 metrics\n"
       "Route map reference\n"
       "Pointer to route-map entries\n")
{
	VTY_DECLVAR_CONTEXT(ospf, ospf);
	struct prefix_ipv4 p;
	struct ospf_external *ext;
	struct ospf_redist *red;

	p.family = AF_INET;
	p.prefix.s_addr = 0;
	p.prefixlen = 0;

	ospf_external_lsa_flush(ospf, DEFAULT_ROUTE, &p, 0);

	if ((ext = ospf_external_lookup(DEFAULT_ROUTE, 0))
	    && EXTERNAL_INFO(ext)) {
		ospf_external_info_delete(DEFAULT_ROUTE, 0, p);
		ospf_external_del(DEFAULT_ROUTE, 0);
	}

	red = ospf_redist_lookup(ospf, DEFAULT_ROUTE, 0);
	if (!red)
		return CMD_SUCCESS;

	ospf_routemap_unset(red);
	return ospf_redistribute_default_unset(ospf);
}

DEFUN (ospf_default_metric,
       ospf_default_metric_cmd,
       "default-metric (0-16777214)",
       "Set metric of redistributed routes\n"
       "Default metric\n")
{
	VTY_DECLVAR_CONTEXT(ospf, ospf);
	int idx_number = 1;
	int metric = -1;

	if (!str2metric(argv[idx_number]->arg, &metric))
		return CMD_WARNING;

	ospf->default_metric = metric;

	return CMD_SUCCESS;
}

DEFUN (no_ospf_default_metric,
       no_ospf_default_metric_cmd,
       "no default-metric [(0-16777214)]",
       NO_STR
       "Set metric of redistributed routes\n"
       "Default metric\n")
{
	VTY_DECLVAR_CONTEXT(ospf, ospf);

	ospf->default_metric = -1;

	return CMD_SUCCESS;
}


DEFUN (ospf_distance,
       ospf_distance_cmd,
       "distance (1-255)",
       "Administrative distance\n"
       "OSPF Administrative distance\n")
{
	VTY_DECLVAR_CONTEXT(ospf, ospf);
	int idx_number = 1;

	ospf->distance_all = atoi(argv[idx_number]->arg);

	return CMD_SUCCESS;
}

DEFUN (no_ospf_distance,
       no_ospf_distance_cmd,
       "no distance (1-255)",
       NO_STR
       "Administrative distance\n"
       "OSPF Administrative distance\n")
{
	VTY_DECLVAR_CONTEXT(ospf, ospf);

	ospf->distance_all = 0;

	return CMD_SUCCESS;
}

DEFUN (no_ospf_distance_ospf,
       no_ospf_distance_ospf_cmd,
       "no distance ospf [{intra-area [(1-255)]|inter-area [(1-255)]|external [(1-255)]}]",
       NO_STR
       "Administrative distance\n"
       "OSPF administrative distance\n"
       "Intra-area routes\n"
       "Distance for intra-area routes\n"
       "Inter-area routes\n"
       "Distance for inter-area routes\n"
       "External routes\n"
       "Distance for external routes\n")
{
	VTY_DECLVAR_CONTEXT(ospf, ospf);
	int idx = 0;

	if (!ospf)
		return CMD_SUCCESS;

	if (argv_find(argv, argc, "intra-area", &idx) || argc == 3)
		idx = ospf->distance_intra = 0;
	if (argv_find(argv, argc, "inter-area", &idx) || argc == 3)
		idx = ospf->distance_inter = 0;
	if (argv_find(argv, argc, "external", &idx) || argc == 3)
		ospf->distance_external = 0;

	return CMD_SUCCESS;
}

DEFUN (ospf_distance_ospf,
       ospf_distance_ospf_cmd,
       "distance ospf {intra-area (1-255)|inter-area (1-255)|external (1-255)}",
       "Administrative distance\n"
       "OSPF administrative distance\n"
       "Intra-area routes\n"
       "Distance for intra-area routes\n"
       "Inter-area routes\n"
       "Distance for inter-area routes\n"
       "External routes\n"
       "Distance for external routes\n")
{
	VTY_DECLVAR_CONTEXT(ospf, ospf);
	int idx = 0;

	if (argv_find(argv, argc, "intra-area", &idx))
		ospf->distance_intra = atoi(argv[idx + 1]->arg);
	idx = 0;
	if (argv_find(argv, argc, "inter-area", &idx))
		ospf->distance_inter = atoi(argv[idx + 1]->arg);
	idx = 0;
	if (argv_find(argv, argc, "external", &idx))
		ospf->distance_external = atoi(argv[idx + 1]->arg);

	return CMD_SUCCESS;
}

#if 0
DEFUN (ospf_distance_source,
       ospf_distance_source_cmd,
       "distance (1-255) A.B.C.D/M",
       "Administrative distance\n"
       "Distance value\n"
       "IP source prefix\n")
{
  VTY_DECLVAR_CONTEXT(ospf, ospf);
  int idx_number = 1;
  int idx_ipv4_prefixlen = 2;

  if (!ospf)
    return CMD_SUCCESS;

  ospf_distance_set (vty, ospf, argv[idx_number]->arg, argv[idx_ipv4_prefixlen]->arg, NULL);

  return CMD_SUCCESS;
}

DEFUN (no_ospf_distance_source,
       no_ospf_distance_source_cmd,
       "no distance (1-255) A.B.C.D/M",
       NO_STR
       "Administrative distance\n"
       "Distance value\n"
       "IP source prefix\n")
{
  VTY_DECLVAR_CONTEXT(ospf, ospf);
  int idx_number = 2;
  int idx_ipv4_prefixlen = 3;

  if (!ospf)
    return CMD_SUCCESS;

  ospf_distance_unset (vty, ospf, argv[idx_number]->arg, argv[idx_ipv4_prefixlen]->arg, NULL);

  return CMD_SUCCESS;
}

DEFUN (ospf_distance_source_access_list,
       ospf_distance_source_access_list_cmd,
       "distance (1-255) A.B.C.D/M WORD",
       "Administrative distance\n"
       "Distance value\n"
       "IP source prefix\n"
       "Access list name\n")
{
  VTY_DECLVAR_CONTEXT(ospf, ospf);
  int idx_number = 1;
  int idx_ipv4_prefixlen = 2;
  int idx_word = 3;

  if (!ospf)
    return CMD_SUCCESS;

  ospf_distance_set (vty, ospf, argv[idx_number]->arg, argv[idx_ipv4_prefixlen]->arg, argv[idx_word]->arg);

  return CMD_SUCCESS;
}

DEFUN (no_ospf_distance_source_access_list,
       no_ospf_distance_source_access_list_cmd,
       "no distance (1-255) A.B.C.D/M WORD",
       NO_STR
       "Administrative distance\n"
       "Distance value\n"
       "IP source prefix\n"
       "Access list name\n")
{
  VTY_DECLVAR_CONTEXT(ospf, ospf);
  int idx_number = 2;
  int idx_ipv4_prefixlen = 3;
  int idx_word = 4;

  if (!ospf)
    return CMD_SUCCESS;

  ospf_distance_unset (vty, ospf, argv[idx_number]->arg, argv[idx_ipv4_prefixlen]->arg, argv[idx_word]->arg);

  return CMD_SUCCESS;
}
#endif

DEFUN (ip_ospf_mtu_ignore,
       ip_ospf_mtu_ignore_addr_cmd,
       "ip ospf mtu-ignore [A.B.C.D]",
       "IP Information\n"
       "OSPF interface commands\n"
       "Disable MTU mismatch detection on this interface\n"
       "Address of interface")
{
	VTY_DECLVAR_CONTEXT(interface, ifp);
	int idx_ipv4 = 3;
	struct in_addr addr;
	int ret;

	struct ospf_if_params *params;
	params = IF_DEF_PARAMS(ifp);

	if (argc == 4) {
		ret = inet_aton(argv[idx_ipv4]->arg, &addr);
		if (!ret) {
			vty_out(vty,
				"Please specify interface address by A.B.C.D%s",
				VTY_NEWLINE);
			return CMD_WARNING;
		}
		params = ospf_get_if_params(ifp, addr);
		ospf_if_update_params(ifp, addr);
	}
	params->mtu_ignore = 1;
	if (params->mtu_ignore != OSPF_MTU_IGNORE_DEFAULT)
		SET_IF_PARAM(params, mtu_ignore);
	else {
		UNSET_IF_PARAM(params, mtu_ignore);
		if (params != IF_DEF_PARAMS(ifp)) {
			ospf_free_if_params(ifp, addr);
			ospf_if_update_params(ifp, addr);
		}
	}
	return CMD_SUCCESS;
}

DEFUN (no_ip_ospf_mtu_ignore,
       no_ip_ospf_mtu_ignore_addr_cmd,
       "no ip ospf mtu-ignore [A.B.C.D]",
       "IP Information\n"
       "OSPF interface commands\n"
       "Disable MTU mismatch detection on this interface\n"
       "Address of interface")
{
	VTY_DECLVAR_CONTEXT(interface, ifp);
	int idx_ipv4 = 4;
	struct in_addr addr;
	int ret;

	struct ospf_if_params *params;
	params = IF_DEF_PARAMS(ifp);

	if (argc == 5) {
		ret = inet_aton(argv[idx_ipv4]->arg, &addr);
		if (!ret) {
			vty_out(vty,
				"Please specify interface address by A.B.C.D%s",
				VTY_NEWLINE);
			return CMD_WARNING;
		}
		params = ospf_get_if_params(ifp, addr);
		ospf_if_update_params(ifp, addr);
	}
	params->mtu_ignore = 0;
	if (params->mtu_ignore != OSPF_MTU_IGNORE_DEFAULT)
		SET_IF_PARAM(params, mtu_ignore);
	else {
		UNSET_IF_PARAM(params, mtu_ignore);
		if (params != IF_DEF_PARAMS(ifp)) {
			ospf_free_if_params(ifp, addr);
			ospf_if_update_params(ifp, addr);
		}
	}
	return CMD_SUCCESS;
}


DEFUN (ospf_max_metric_router_lsa_admin,
       ospf_max_metric_router_lsa_admin_cmd,
       "max-metric router-lsa administrative",
       "OSPF maximum / infinite-distance metric\n"
       "Advertise own Router-LSA with infinite distance (stub router)\n"
       "Administratively applied, for an indefinite period\n")
{
	VTY_DECLVAR_CONTEXT(ospf, ospf);
	struct listnode *ln;
	struct ospf_area *area;

	for (ALL_LIST_ELEMENTS_RO(ospf->areas, ln, area)) {
		SET_FLAG(area->stub_router_state, OSPF_AREA_ADMIN_STUB_ROUTED);

		if (!CHECK_FLAG(area->stub_router_state,
				OSPF_AREA_IS_STUB_ROUTED))
			ospf_router_lsa_update_area(area);
	}

	/* Allows for areas configured later to get the property */
	ospf->stub_router_admin_set = OSPF_STUB_ROUTER_ADMINISTRATIVE_SET;

	return CMD_SUCCESS;
}

DEFUN (no_ospf_max_metric_router_lsa_admin,
       no_ospf_max_metric_router_lsa_admin_cmd,
       "no max-metric router-lsa administrative",
       NO_STR
       "OSPF maximum / infinite-distance metric\n"
       "Advertise own Router-LSA with infinite distance (stub router)\n"
       "Administratively applied, for an indefinite period\n")
{
	VTY_DECLVAR_CONTEXT(ospf, ospf);
	struct listnode *ln;
	struct ospf_area *area;

	for (ALL_LIST_ELEMENTS_RO(ospf->areas, ln, area)) {
		UNSET_FLAG(area->stub_router_state,
			   OSPF_AREA_ADMIN_STUB_ROUTED);

		/* Don't trample on the start-up stub timer */
		if (CHECK_FLAG(area->stub_router_state,
			       OSPF_AREA_IS_STUB_ROUTED)
		    && !area->t_stub_router) {
			UNSET_FLAG(area->stub_router_state,
				   OSPF_AREA_IS_STUB_ROUTED);
			ospf_router_lsa_update_area(area);
		}
	}
	ospf->stub_router_admin_set = OSPF_STUB_ROUTER_ADMINISTRATIVE_UNSET;
	return CMD_SUCCESS;
}

DEFUN (ospf_max_metric_router_lsa_startup,
       ospf_max_metric_router_lsa_startup_cmd,
       "max-metric router-lsa on-startup (5-86400)",
       "OSPF maximum / infinite-distance metric\n"
       "Advertise own Router-LSA with infinite distance (stub router)\n"
       "Automatically advertise stub Router-LSA on startup of OSPF\n"
       "Time (seconds) to advertise self as stub-router\n")
{
	VTY_DECLVAR_CONTEXT(ospf, ospf);
	int idx_number = 3;
	unsigned int seconds;

	if (argc != 1) {
		vty_out(vty, "%% Must supply stub-router period");
		return CMD_WARNING;
	}

	VTY_GET_INTEGER("stub-router startup period", seconds,
			argv[idx_number]->arg);

	ospf->stub_router_startup_time = seconds;

	return CMD_SUCCESS;
}

DEFUN (no_ospf_max_metric_router_lsa_startup,
       no_ospf_max_metric_router_lsa_startup_cmd,
       "no max-metric router-lsa on-startup [(5-86400)]",
       NO_STR
       "OSPF maximum / infinite-distance metric\n"
       "Advertise own Router-LSA with infinite distance (stub router)\n"
       "Automatically advertise stub Router-LSA on startup of OSPF\n"
       "Time (seconds) to advertise self as stub-router\n")
{
	VTY_DECLVAR_CONTEXT(ospf, ospf);
	struct listnode *ln;
	struct ospf_area *area;

	ospf->stub_router_startup_time = OSPF_STUB_ROUTER_UNCONFIGURED;

	for (ALL_LIST_ELEMENTS_RO(ospf->areas, ln, area)) {
		SET_FLAG(area->stub_router_state,
			 OSPF_AREA_WAS_START_STUB_ROUTED);
		OSPF_TIMER_OFF(area->t_stub_router);

		/* Don't trample on admin stub routed */
		if (!CHECK_FLAG(area->stub_router_state,
				OSPF_AREA_ADMIN_STUB_ROUTED)) {
			UNSET_FLAG(area->stub_router_state,
				   OSPF_AREA_IS_STUB_ROUTED);
			ospf_router_lsa_update_area(area);
		}
	}
	return CMD_SUCCESS;
}


DEFUN (ospf_max_metric_router_lsa_shutdown,
       ospf_max_metric_router_lsa_shutdown_cmd,
       "max-metric router-lsa on-shutdown (5-100)",
       "OSPF maximum / infinite-distance metric\n"
       "Advertise own Router-LSA with infinite distance (stub router)\n"
       "Advertise stub-router prior to full shutdown of OSPF\n"
       "Time (seconds) to wait till full shutdown\n")
{
	VTY_DECLVAR_CONTEXT(ospf, ospf);
	int idx_number = 3;
	unsigned int seconds;

	if (argc != 1) {
		vty_out(vty, "%% Must supply stub-router shutdown period");
		return CMD_WARNING;
	}

	VTY_GET_INTEGER("stub-router shutdown wait period", seconds,
			argv[idx_number]->arg);

	ospf->stub_router_shutdown_time = seconds;

	return CMD_SUCCESS;
}

DEFUN (no_ospf_max_metric_router_lsa_shutdown,
       no_ospf_max_metric_router_lsa_shutdown_cmd,
       "no max-metric router-lsa on-shutdown [(5-100)]",
       NO_STR
       "OSPF maximum / infinite-distance metric\n"
       "Advertise own Router-LSA with infinite distance (stub router)\n"
       "Advertise stub-router prior to full shutdown of OSPF\n"
       "Time (seconds) to wait till full shutdown\n")
{
	VTY_DECLVAR_CONTEXT(ospf, ospf);

	ospf->stub_router_shutdown_time = OSPF_STUB_ROUTER_UNCONFIGURED;

	return CMD_SUCCESS;
}

static void config_write_stub_router(struct vty *vty, struct ospf *ospf)
{
	struct listnode *ln;
	struct ospf_area *area;

	if (ospf->stub_router_startup_time != OSPF_STUB_ROUTER_UNCONFIGURED)
		vty_out(vty, " max-metric router-lsa on-startup %u%s",
			ospf->stub_router_startup_time, VTY_NEWLINE);
	if (ospf->stub_router_shutdown_time != OSPF_STUB_ROUTER_UNCONFIGURED)
		vty_out(vty, " max-metric router-lsa on-shutdown %u%s",
			ospf->stub_router_shutdown_time, VTY_NEWLINE);
	for (ALL_LIST_ELEMENTS_RO(ospf->areas, ln, area)) {
		if (CHECK_FLAG(area->stub_router_state,
			       OSPF_AREA_ADMIN_STUB_ROUTED)) {
			vty_out(vty, " max-metric router-lsa administrative%s",
				VTY_NEWLINE);
			break;
		}
	}
	return;
}

static void show_ip_ospf_route_network(struct vty *vty, struct route_table *rt)
{
	struct route_node *rn;
	struct ospf_route * or ;
	struct listnode *pnode, *pnnode;
	struct ospf_path *path;

	vty_out(vty, "============ OSPF network routing table ============%s",
		VTY_NEWLINE);

	for (rn = route_top(rt); rn; rn = route_next(rn))
		if ((or = rn->info) != NULL) {
			char buf1[19];
			snprintf(buf1, 19, "%s/%d", inet_ntoa(rn->p.u.prefix4),
				 rn->p.prefixlen);

			switch (or->path_type) {
			case OSPF_PATH_INTER_AREA:
				if (or->type == OSPF_DESTINATION_NETWORK)
					vty_out(vty,
						"N IA %-18s    [%d] area: %s%s",
						buf1, or->cost,
						inet_ntoa(or->u.std.area_id),
						VTY_NEWLINE);
				else if (or->type == OSPF_DESTINATION_DISCARD)
					vty_out(vty,
						"D IA %-18s    Discard entry%s",
						buf1, VTY_NEWLINE);
				break;
			case OSPF_PATH_INTRA_AREA:
				vty_out(vty, "N    %-18s    [%d] area: %s%s",
					buf1, or->cost,
					inet_ntoa(or->u.std.area_id),
					VTY_NEWLINE);
				break;
			default:
				break;
			}

			if (or->type == OSPF_DESTINATION_NETWORK)
				for (ALL_LIST_ELEMENTS(or->paths, pnode, pnnode,
						       path)) {
					if (if_lookup_by_index(path->ifindex,
							       VRF_DEFAULT)) {
						if (path->nexthop.s_addr == 0)
							vty_out(vty,
								"%24s   directly attached to %s%s",
								"",
								ifindex2ifname(
									path->ifindex,
									VRF_DEFAULT),
								VTY_NEWLINE);
						else
							vty_out(vty,
								"%24s   via %s, %s%s",
								"",
								inet_ntoa(
									path->nexthop),
								ifindex2ifname(
									path->ifindex,
									VRF_DEFAULT),
								VTY_NEWLINE);
					}
				}
		}
	vty_out(vty, "%s", VTY_NEWLINE);
}

static void show_ip_ospf_route_router(struct vty *vty, struct route_table *rtrs)
{
	struct route_node *rn;
	struct ospf_route * or ;
	struct listnode *pnode;
	struct listnode *node;
	struct ospf_path *path;

	vty_out(vty, "============ OSPF router routing table =============%s",
		VTY_NEWLINE);
	for (rn = route_top(rtrs); rn; rn = route_next(rn))
		if (rn->info) {
			int flag = 0;

			vty_out(vty, "R    %-15s    ",
				inet_ntoa(rn->p.u.prefix4));

			for (ALL_LIST_ELEMENTS_RO((struct list *)rn->info, node,
						  or)) {
				if (flag++)
					vty_out(vty, "%24s", "");

				/* Show path. */
				vty_out(vty, "%s [%d] area: %s",
					(or->path_type == OSPF_PATH_INTER_AREA
						 ? "IA"
						 : "  "),
					or->cost, inet_ntoa(or->u.std.area_id));
				/* Show flags. */
				vty_out(vty, "%s%s%s",
					(or->u.std.flags & ROUTER_LSA_BORDER
						 ? ", ABR"
						 : ""),
					(or->u.std.flags & ROUTER_LSA_EXTERNAL
						 ? ", ASBR"
						 : ""),
					VTY_NEWLINE);

				for (ALL_LIST_ELEMENTS_RO(or->paths, pnode,
							  path)) {
					if (if_lookup_by_index(path->ifindex,
							       VRF_DEFAULT)) {
						if (path->nexthop.s_addr == 0)
							vty_out(vty,
								"%24s   directly attached to %s%s",
								"",
								ifindex2ifname(
									path->ifindex,
									VRF_DEFAULT),
								VTY_NEWLINE);
						else
							vty_out(vty,
								"%24s   via %s, %s%s",
								"",
								inet_ntoa(
									path->nexthop),
								ifindex2ifname(
									path->ifindex,
									VRF_DEFAULT),
								VTY_NEWLINE);
					}
				}
			}
		}
	vty_out(vty, "%s", VTY_NEWLINE);
}

static void show_ip_ospf_route_external(struct vty *vty, struct route_table *rt)
{
	struct route_node *rn;
	struct ospf_route *er;
	struct listnode *pnode, *pnnode;
	struct ospf_path *path;

	vty_out(vty, "============ OSPF external routing table ===========%s",
		VTY_NEWLINE);
	for (rn = route_top(rt); rn; rn = route_next(rn))
		if ((er = rn->info) != NULL) {
			char buf1[19];
			snprintf(buf1, 19, "%s/%d", inet_ntoa(rn->p.u.prefix4),
				 rn->p.prefixlen);

			switch (er->path_type) {
			case OSPF_PATH_TYPE1_EXTERNAL:
				vty_out(vty,
					"N E1 %-18s    [%d] tag: %" ROUTE_TAG_PRI
					"%s",
					buf1, er->cost, er->u.ext.tag,
					VTY_NEWLINE);
				break;
			case OSPF_PATH_TYPE2_EXTERNAL:
				vty_out(vty,
					"N E2 %-18s    [%d/%d] tag: %" ROUTE_TAG_PRI
					"%s",
					buf1, er->cost, er->u.ext.type2_cost,
					er->u.ext.tag, VTY_NEWLINE);
				break;
			}

			for (ALL_LIST_ELEMENTS(er->paths, pnode, pnnode,
					       path)) {
				if (if_lookup_by_index(path->ifindex,
						       VRF_DEFAULT)) {
					if (path->nexthop.s_addr == 0)
						vty_out(vty,
							"%24s   directly attached to %s%s",
							"",
							ifindex2ifname(
								path->ifindex,
								VRF_DEFAULT),
							VTY_NEWLINE);
					else
						vty_out(vty,
							"%24s   via %s, %s%s",
							"",
							inet_ntoa(
								path->nexthop),
							ifindex2ifname(
								path->ifindex,
								VRF_DEFAULT),
							VTY_NEWLINE);
				}
			}
		}
	vty_out(vty, "%s", VTY_NEWLINE);
}

static int show_ip_ospf_border_routers_common(struct vty *vty,
					      struct ospf *ospf)
{
	if (ospf->instance)
		vty_out(vty, "%sOSPF Instance: %d%s%s", VTY_NEWLINE,
			ospf->instance, VTY_NEWLINE, VTY_NEWLINE);

	if (ospf->new_table == NULL) {
		vty_out(vty, "No OSPF routing information exist%s",
			VTY_NEWLINE);
		return CMD_SUCCESS;
	}

/* modified by liwh for bug 43719, 2017-08-02 */
#ifdef _CENTEC_
  vty_out (vty, "R -Router, IA - OSPF inter area\n\n");
#endif
/*liwh end */

	/* Show Network routes.
	show_ip_ospf_route_network (vty, ospf->new_table);   */

	/* Show Router routes. */
	show_ip_ospf_route_router(vty, ospf->new_rtrs);

	vty_out(vty, "%s", VTY_NEWLINE);

	return CMD_SUCCESS;
}

DEFUN (show_ip_ospf_border_routers,
       show_ip_ospf_border_routers_cmd,
       "show ip ospf border-routers",
       SHOW_STR
       IP_STR
       "OSPF information\n"
       "Show all the ABR's and ASBR's\n")
{
	struct ospf *ospf;

	if ((ospf = ospf_lookup()) == NULL || !ospf->oi_running)
		return CMD_SUCCESS;

	return show_ip_ospf_border_routers_common(vty, ospf);
}

DEFUN (show_ip_ospf_instance_border_routers,
       show_ip_ospf_instance_border_routers_cmd,
       "show ip ospf (1-65535) border-routers",
       SHOW_STR
       IP_STR
       "OSPF information\n"
       "Instance ID\n"
       "Show all the ABR's and ASBR's\n")
{
	int idx_number = 3;
	struct ospf *ospf;
	u_short instance = 0;

	VTY_GET_INTEGER("Instance", instance, argv[idx_number]->arg);
	if ((ospf = ospf_lookup_instance(instance)) == NULL
	    || !ospf->oi_running)
		return CMD_SUCCESS;

	return show_ip_ospf_border_routers_common(vty, ospf);
}

static int show_ip_ospf_route_common(struct vty *vty, struct ospf *ospf)
{
	if (ospf->instance)
		vty_out(vty, "%sOSPF Instance: %d%s%s", VTY_NEWLINE,
			ospf->instance, VTY_NEWLINE, VTY_NEWLINE);

	if (ospf->new_table == NULL) {
		vty_out(vty, "No OSPF routing information exist%s",
			VTY_NEWLINE);
		return CMD_SUCCESS;
	}

/* modified by liwh for bug 43719, 2017-08-02 */
#ifdef _CENTEC_
  vty_out (vty, "N - Network, D - Discard, IA - OSPF inter area\n");
  vty_out (vty, "R -Router, E1 - OSPF external type 1\n");
  vty_out (vty, "E2 - OSPF external type 2\n\n");
#endif
/*liwh end */

	/* Show Network routes. */
	show_ip_ospf_route_network(vty, ospf->new_table);

	/* Show Router routes. */
	show_ip_ospf_route_router(vty, ospf->new_rtrs);

	/* Show AS External routes. */
	show_ip_ospf_route_external(vty, ospf->old_external_route);

	vty_out(vty, "%s", VTY_NEWLINE);

	return CMD_SUCCESS;
}

DEFUN (show_ip_ospf_route,
       show_ip_ospf_route_cmd,
       "show ip ospf route",
       SHOW_STR
       IP_STR
       "OSPF information\n"
       "OSPF routing table\n")
{
	struct ospf *ospf;

	if ((ospf = ospf_lookup()) == NULL || !ospf->oi_running)
		return CMD_SUCCESS;

	return show_ip_ospf_route_common(vty, ospf);
}

DEFUN (show_ip_ospf_instance_route,
       show_ip_ospf_instance_route_cmd,
       "show ip ospf (1-65535) route",
       SHOW_STR
       IP_STR
       "OSPF information\n"
       "Instance ID\n"
       "OSPF routing table\n")
{
	int idx_number = 3;
	struct ospf *ospf;
	u_short instance = 0;

	VTY_GET_INTEGER("Instance", instance, argv[idx_number]->arg);
	if ((ospf = ospf_lookup_instance(instance)) == NULL
	    || !ospf->oi_running)
		return CMD_SUCCESS;

	return show_ip_ospf_route_common(vty, ospf);
}

const char *ospf_abr_type_str[] = {"unknown", "standard", "ibm", "cisco",
				   "shortcut"};

const char *ospf_shortcut_mode_str[] = {"default", "enable", "disable"};

const char *ospf_int_type_str[] = {"unknown", /* should never be used. */
				   "point-to-point", "broadcast",
				   "non-broadcast",  "point-to-multipoint",
				   "virtual-link", /* should never be used. */
				   "loopback"};

/* Configuration write function for ospfd. */
static int config_write_interface(struct vty *vty)
{
	struct listnode *n1, *n2;
	struct interface *ifp;
	struct crypt_key *ck;
	int write = 0;
	struct route_node *rn = NULL;
	struct ospf_if_params *params;
	struct ospf *ospf = ospf_lookup();

	for (ALL_LIST_ELEMENTS_RO(vrf_iflist(VRF_DEFAULT), n1, ifp)) {
		if (memcmp(ifp->name, "VLINK", 5) == 0)
			continue;

		if (ifp->ifindex == IFINDEX_DELETED)
			continue;

		vty_out(vty, "!%s", VTY_NEWLINE);
		vty_out(vty, "interface %s%s", ifp->name, VTY_NEWLINE);
		if (ifp->desc)
			vty_out(vty, " description %s%s", ifp->desc,
				VTY_NEWLINE);

		write++;

		params = IF_DEF_PARAMS(ifp);

		do {
			/* Interface Network print. */
			if (OSPF_IF_PARAM_CONFIGURED(params, type)
			    && params->type != OSPF_IFTYPE_LOOPBACK) {
				if (params->type != ospf_default_iftype(ifp)) {
					vty_out(vty, " ip ospf network %s",
						ospf_int_type_str
							[params->type]);
					if (params != IF_DEF_PARAMS(ifp))
						vty_out(vty, " %s",
							inet_ntoa(
								rn->p.u.prefix4));
					vty_out(vty, "%s", VTY_NEWLINE);
				}
			}

			/* OSPF interface authentication print */
			if (OSPF_IF_PARAM_CONFIGURED(params, auth_type)
			    && params->auth_type != OSPF_AUTH_NOTSET) {
				const char *auth_str;

				/* Translation tables are not that much help
				   here due to syntax
				   of the simple option */
				switch (params->auth_type) {

				case OSPF_AUTH_NULL:
					auth_str = " null";
					break;

				case OSPF_AUTH_SIMPLE:
					auth_str = "";
					break;

				case OSPF_AUTH_CRYPTOGRAPHIC:
					auth_str = " message-digest";
					break;

				default:
					auth_str = "";
					break;
				}

				vty_out(vty, " ip ospf authentication%s",
					auth_str);
				if (params != IF_DEF_PARAMS(ifp))
					vty_out(vty, " %s",
						inet_ntoa(rn->p.u.prefix4));
				vty_out(vty, "%s", VTY_NEWLINE);
			}

			/* Simple Authentication Password print. */
			if (OSPF_IF_PARAM_CONFIGURED(params, auth_simple)
			    && params->auth_simple[0] != '\0') {
				vty_out(vty, " ip ospf authentication-key %s",
					params->auth_simple);
				if (params != IF_DEF_PARAMS(ifp))
					vty_out(vty, " %s",
						inet_ntoa(rn->p.u.prefix4));
				vty_out(vty, "%s", VTY_NEWLINE);
			}

			/* Cryptographic Authentication Key print. */
			for (ALL_LIST_ELEMENTS_RO(params->auth_crypt, n2, ck)) {
				vty_out(vty,
					" ip ospf message-digest-key %d md5 %s",
					ck->key_id, ck->auth_key);
				if (params != IF_DEF_PARAMS(ifp))
					vty_out(vty, " %s",
						inet_ntoa(rn->p.u.prefix4));
				vty_out(vty, "%s", VTY_NEWLINE);
			}

			/* Interface Output Cost print. */
			if (OSPF_IF_PARAM_CONFIGURED(params, output_cost_cmd)) {
				vty_out(vty, " ip ospf cost %u",
					params->output_cost_cmd);
				if (params != IF_DEF_PARAMS(ifp))
					vty_out(vty, " %s",
						inet_ntoa(rn->p.u.prefix4));
				vty_out(vty, "%s", VTY_NEWLINE);
			}

			/* Hello Interval print. */
			if (OSPF_IF_PARAM_CONFIGURED(params, v_hello)
			    && params->v_hello != OSPF_HELLO_INTERVAL_DEFAULT) {
				vty_out(vty, " ip ospf hello-interval %u",
					params->v_hello);
				if (params != IF_DEF_PARAMS(ifp))
					vty_out(vty, " %s",
						inet_ntoa(rn->p.u.prefix4));
				vty_out(vty, "%s", VTY_NEWLINE);
			}


			/* Router Dead Interval print. */
	if (OSPF_IF_PARAM_CONFIGURED (params, v_wait) &&
/* modified by liwh for bug 43976, 2017-08-07 */
#ifdef _CENTEC_
        (params->v_wait != params->v_hello * GLB_OSPF_DEAD_INTERVAL_MULTI_HELLO))
        //params->v_wait != OSPF_ROUTER_DEAD_INTERVAL_DEFAULT)
#endif
/* liwh end */
	  {
				vty_out(vty, " ip ospf dead-interval ");

				/* fast hello ? */
				if (OSPF_IF_PARAM_CONFIGURED(params,
							     fast_hello))
					vty_out(vty,
						"minimal hello-multiplier %d",
						params->fast_hello);
				else
					vty_out(vty, "%u", params->v_wait);

				if (params != IF_DEF_PARAMS(ifp))
					vty_out(vty, " %s",
						inet_ntoa(rn->p.u.prefix4));
				vty_out(vty, "%s", VTY_NEWLINE);
			}

			/* Router Priority print. */
			if (OSPF_IF_PARAM_CONFIGURED(params, priority)
			    && params->priority
				       != OSPF_ROUTER_PRIORITY_DEFAULT) {
				vty_out(vty, " ip ospf priority %u",
					params->priority);
				if (params != IF_DEF_PARAMS(ifp))
					vty_out(vty, " %s",
						inet_ntoa(rn->p.u.prefix4));
				vty_out(vty, "%s", VTY_NEWLINE);
			}

			/* Retransmit Interval print. */
			if (OSPF_IF_PARAM_CONFIGURED(params,
						     retransmit_interval)
			    && params->retransmit_interval
				       != OSPF_RETRANSMIT_INTERVAL_DEFAULT) {
				vty_out(vty, " ip ospf retransmit-interval %u",
					params->retransmit_interval);
				if (params != IF_DEF_PARAMS(ifp))
					vty_out(vty, " %s",
						inet_ntoa(rn->p.u.prefix4));
				vty_out(vty, "%s", VTY_NEWLINE);
			}

			/* Transmit Delay print. */
			if (OSPF_IF_PARAM_CONFIGURED(params, transmit_delay)
			    && params->transmit_delay
				       != OSPF_TRANSMIT_DELAY_DEFAULT) {
				vty_out(vty, " ip ospf transmit-delay %u",
					params->transmit_delay);
				if (params != IF_DEF_PARAMS(ifp))
					vty_out(vty, " %s",
						inet_ntoa(rn->p.u.prefix4));
				vty_out(vty, "%s", VTY_NEWLINE);
			}

			/* Area  print. */
			if (OSPF_IF_PARAM_CONFIGURED(params, if_area)) {
				if (ospf->instance)
					vty_out(vty, " ip ospf %d area %s%s",
						ospf->instance,
						inet_ntoa(params->if_area),
						VTY_NEWLINE);
				else
					vty_out(vty, " ip ospf area %s%s",
						inet_ntoa(params->if_area),
						VTY_NEWLINE);
			}

			/* bfd  print. */
			ospf_bfd_write_config(vty, params);

			/* MTU ignore print. */
			if (OSPF_IF_PARAM_CONFIGURED(params, mtu_ignore)
			    && params->mtu_ignore != OSPF_MTU_IGNORE_DEFAULT) {
				if (params->mtu_ignore == 0)
					vty_out(vty, " no ip ospf mtu-ignore");
				else
					vty_out(vty, " ip ospf mtu-ignore");
				if (params != IF_DEF_PARAMS(ifp))
					vty_out(vty, " %s",
						inet_ntoa(rn->p.u.prefix4));
				vty_out(vty, "%s", VTY_NEWLINE);
			}


			while (1) {
				if (rn == NULL)
					rn = route_top(IF_OIFS_PARAMS(ifp));
				else
					rn = route_next(rn);

				if (rn == NULL)
					break;
				params = rn->info;
				if (params != NULL)
					break;
			}
		} while (rn);

		ospf_opaque_config_write_if(vty, ifp);
	}

	return write;
}

static int config_write_network_area(struct vty *vty, struct ospf *ospf)
{
	struct route_node *rn;
	u_char buf[INET_ADDRSTRLEN];

	/* `network area' print. */
	for (rn = route_top(ospf->networks); rn; rn = route_next(rn))
		if (rn->info) {
			struct ospf_network *n = rn->info;

			memset(buf, 0, INET_ADDRSTRLEN);

			/* Create Area ID string by specified Area ID format. */
			if (n->area_id_fmt == OSPF_AREA_ID_FMT_DOTTEDQUAD)
				strncpy((char *)buf, inet_ntoa(n->area_id),
					INET_ADDRSTRLEN);
			else
				sprintf((char *)buf, "%lu",
					(unsigned long int)ntohl(
						n->area_id.s_addr));

			/* Network print. */
			vty_out(vty, " network %s/%d area %s%s",
				inet_ntoa(rn->p.u.prefix4), rn->p.prefixlen,
				buf, VTY_NEWLINE);
		}

	return 0;
}

static int config_write_ospf_area(struct vty *vty, struct ospf *ospf)
{
	struct listnode *node;
	struct ospf_area *area;
	u_char buf[INET_ADDRSTRLEN];

	/* Area configuration print. */
	for (ALL_LIST_ELEMENTS_RO(ospf->areas, node, area)) {
		struct route_node *rn1;

		area_id2str((char *)buf, INET_ADDRSTRLEN, &area->area_id,
			    area->area_id_fmt);

		if (area->auth_type != OSPF_AUTH_NULL) {
			if (area->auth_type == OSPF_AUTH_SIMPLE)
				vty_out(vty, " area %s authentication%s", buf,
					VTY_NEWLINE);
			else
				vty_out(vty,
					" area %s authentication message-digest%s",
					buf, VTY_NEWLINE);
		}

		if (area->shortcut_configured != OSPF_SHORTCUT_DEFAULT)
			vty_out(vty, " area %s shortcut %s%s", buf,
				ospf_shortcut_mode_str
					[area->shortcut_configured],
				VTY_NEWLINE);

		if ((area->external_routing == OSPF_AREA_STUB)
		    || (area->external_routing == OSPF_AREA_NSSA)) {
			if (area->external_routing == OSPF_AREA_STUB)
				vty_out(vty, " area %s stub", buf);
			else if (area->external_routing == OSPF_AREA_NSSA) {
				vty_out(vty, " area %s nssa", buf);
				switch (area->NSSATranslatorRole) {
				case OSPF_NSSA_ROLE_NEVER:
					vty_out(vty, " translate-never");
					break;
				case OSPF_NSSA_ROLE_ALWAYS:
					vty_out(vty, " translate-always");
					break;
				case OSPF_NSSA_ROLE_CANDIDATE:
				default:
					vty_out(vty, " translate-candidate");
				}
			}

			if (area->no_summary)
				vty_out(vty, " no-summary");

			vty_out(vty, "%s", VTY_NEWLINE);

			if (area->default_cost != 1)
				vty_out(vty, " area %s default-cost %d%s", buf,
					area->default_cost, VTY_NEWLINE);
		}

		for (rn1 = route_top(area->ranges); rn1; rn1 = route_next(rn1))
			if (rn1->info) {
				struct ospf_area_range *range = rn1->info;

				vty_out(vty, " area %s range %s/%d", buf,
					inet_ntoa(rn1->p.u.prefix4),
					rn1->p.prefixlen);

				if (range->cost_config
				    != OSPF_AREA_RANGE_COST_UNSPEC)
					vty_out(vty, " cost %d",
						range->cost_config);

				if (!CHECK_FLAG(range->flags,
						OSPF_AREA_RANGE_ADVERTISE))
					vty_out(vty, " not-advertise");

				if (CHECK_FLAG(range->flags,
					       OSPF_AREA_RANGE_SUBSTITUTE))
					vty_out(vty, " substitute %s/%d",
						inet_ntoa(range->subst_addr),
						range->subst_masklen);

				vty_out(vty, "%s", VTY_NEWLINE);
			}

		if (EXPORT_NAME(area))
			vty_out(vty, " area %s export-list %s%s", buf,
				EXPORT_NAME(area), VTY_NEWLINE);

		if (IMPORT_NAME(area))
			vty_out(vty, " area %s import-list %s%s", buf,
				IMPORT_NAME(area), VTY_NEWLINE);

		if (PREFIX_NAME_IN(area))
			vty_out(vty, " area %s filter-list prefix %s in%s", buf,
				PREFIX_NAME_IN(area), VTY_NEWLINE);

		if (PREFIX_NAME_OUT(area))
			vty_out(vty, " area %s filter-list prefix %s out%s",
				buf, PREFIX_NAME_OUT(area), VTY_NEWLINE);
	}

	return 0;
}

static int config_write_ospf_nbr_nbma(struct vty *vty, struct ospf *ospf)
{
	struct ospf_nbr_nbma *nbr_nbma;
	struct route_node *rn;

	/* Static Neighbor configuration print. */
	for (rn = route_top(ospf->nbr_nbma); rn; rn = route_next(rn))
		if ((nbr_nbma = rn->info)) {
			vty_out(vty, " neighbor %s", inet_ntoa(nbr_nbma->addr));

			if (nbr_nbma->priority
			    != OSPF_NEIGHBOR_PRIORITY_DEFAULT)
				vty_out(vty, " priority %d",
					nbr_nbma->priority);

			if (nbr_nbma->v_poll != OSPF_POLL_INTERVAL_DEFAULT)
				vty_out(vty, " poll-interval %d",
					nbr_nbma->v_poll);

			vty_out(vty, "%s", VTY_NEWLINE);
		}

	return 0;
}

static int config_write_virtual_link(struct vty *vty, struct ospf *ospf)
{
	struct listnode *node;
	struct ospf_vl_data *vl_data;
	char buf[INET_ADDRSTRLEN];

	/* Virtual-Link print */
	for (ALL_LIST_ELEMENTS_RO(ospf->vlinks, node, vl_data)) {
		struct listnode *n2;
		struct crypt_key *ck;
		struct ospf_interface *oi;

		if (vl_data != NULL) {
			memset(buf, 0, INET_ADDRSTRLEN);

			area_id2str(buf, sizeof(buf), &vl_data->vl_area_id,
				    vl_data->vl_area_id_fmt);
			oi = vl_data->vl_oi;

			/* timers */
			if (OSPF_IF_PARAM(oi, v_hello)
				    != OSPF_HELLO_INTERVAL_DEFAULT
			    || OSPF_IF_PARAM(oi, v_wait)
				       != OSPF_ROUTER_DEAD_INTERVAL_DEFAULT
			    || OSPF_IF_PARAM(oi, retransmit_interval)
				       != OSPF_RETRANSMIT_INTERVAL_DEFAULT
			    || OSPF_IF_PARAM(oi, transmit_delay)
				       != OSPF_TRANSMIT_DELAY_DEFAULT)
				vty_out(vty,
					" area %s virtual-link %s hello-interval %d retransmit-interval %d transmit-delay %d dead-interval %d%s",
					buf, inet_ntoa(vl_data->vl_peer),
					OSPF_IF_PARAM(oi, v_hello),
					OSPF_IF_PARAM(oi, retransmit_interval),
					OSPF_IF_PARAM(oi, transmit_delay),
					OSPF_IF_PARAM(oi, v_wait), VTY_NEWLINE);
			else
				vty_out(vty, " area %s virtual-link %s%s", buf,
					inet_ntoa(vl_data->vl_peer),
					VTY_NEWLINE);
			/* Auth key */
			if (IF_DEF_PARAMS(vl_data->vl_oi->ifp)->auth_simple[0]
			    != '\0')
				vty_out(vty,
					" area %s virtual-link %s authentication-key %s%s",
					buf, inet_ntoa(vl_data->vl_peer),
					IF_DEF_PARAMS(vl_data->vl_oi->ifp)
						->auth_simple,
					VTY_NEWLINE);
			/* md5 keys */
			for (ALL_LIST_ELEMENTS_RO(
				     IF_DEF_PARAMS(vl_data->vl_oi->ifp)
					     ->auth_crypt,
				     n2, ck))
				vty_out(vty,
					" area %s virtual-link %s"
					" message-digest-key %d md5 %s%s",
					buf, inet_ntoa(vl_data->vl_peer),
					ck->key_id, ck->auth_key, VTY_NEWLINE);
		}
	}

	return 0;
}


static int config_write_ospf_redistribute(struct vty *vty, struct ospf *ospf)
{
	int type;

	/* redistribute print. */
	for (type = 0; type < ZEBRA_ROUTE_MAX; type++) {
		struct list *red_list;
		struct listnode *node;
		struct ospf_redist *red;

		red_list = ospf->redist[type];
		if (!red_list)
			continue;

		for (ALL_LIST_ELEMENTS_RO(red_list, node, red)) {
			vty_out(vty, " redistribute %s",
				zebra_route_string(type));
			if (red->instance)
				vty_out(vty, " %d", red->instance);

			if (red->dmetric.value >= 0)
				vty_out(vty, " metric %d", red->dmetric.value);

			if (red->dmetric.type == EXTERNAL_METRIC_TYPE_1)
				vty_out(vty, " metric-type 1");

			if (ROUTEMAP_NAME(red))
				vty_out(vty, " route-map %s",
					ROUTEMAP_NAME(red));

			vty_out(vty, "%s", VTY_NEWLINE);
		}
	}

	return 0;
}

static int config_write_ospf_default_metric(struct vty *vty, struct ospf *ospf)
{
	if (ospf->default_metric != -1)
		vty_out(vty, " default-metric %d%s", ospf->default_metric,
			VTY_NEWLINE);
	return 0;
}

static int config_write_ospf_distribute(struct vty *vty, struct ospf *ospf)
{
	int type;
	struct ospf_redist *red;

	if (ospf) {
		/* distribute-list print. */
		for (type = 0; type < ZEBRA_ROUTE_MAX; type++)
			if (DISTRIBUTE_NAME(ospf, type))
				vty_out(vty, " distribute-list %s out %s%s",
					DISTRIBUTE_NAME(ospf, type),
					zebra_route_string(type), VTY_NEWLINE);

		/* default-information print. */
		if (ospf->default_originate != DEFAULT_ORIGINATE_NONE) {
			vty_out(vty, " default-information originate");
			if (ospf->default_originate == DEFAULT_ORIGINATE_ALWAYS)
				vty_out(vty, " always");

			red = ospf_redist_lookup(ospf, DEFAULT_ROUTE, 0);
			if (red) {
				if (red->dmetric.value >= 0)
					vty_out(vty, " metric %d",
						red->dmetric.value);
				if (red->dmetric.type == EXTERNAL_METRIC_TYPE_1)
					vty_out(vty, " metric-type 1");

				if (ROUTEMAP_NAME(red))
					vty_out(vty, " route-map %s",
						ROUTEMAP_NAME(red));
			}

			vty_out(vty, "%s", VTY_NEWLINE);
		}
	}

	return 0;
}

static int config_write_ospf_distance(struct vty *vty, struct ospf *ospf)
{
	struct route_node *rn;
	struct ospf_distance *odistance;

	if (ospf->distance_all)
		vty_out(vty, " distance %d%s", ospf->distance_all, VTY_NEWLINE);

	if (ospf->distance_intra || ospf->distance_inter
	    || ospf->distance_external) {
		vty_out(vty, " distance ospf");

		if (ospf->distance_intra)
			vty_out(vty, " intra-area %d", ospf->distance_intra);
		if (ospf->distance_inter)
			vty_out(vty, " inter-area %d", ospf->distance_inter);
		if (ospf->distance_external)
			vty_out(vty, " external %d", ospf->distance_external);

		vty_out(vty, "%s", VTY_NEWLINE);
	}

	for (rn = route_top(ospf->distance_table); rn; rn = route_next(rn))
		if ((odistance = rn->info) != NULL) {
			vty_out(vty, " distance %d %s/%d %s%s",
				odistance->distance, inet_ntoa(rn->p.u.prefix4),
				rn->p.prefixlen,
				odistance->access_list ? odistance->access_list
						       : "",
				VTY_NEWLINE);
		}
	return 0;
}

/* OSPF configuration write function. */
static int ospf_config_write(struct vty *vty)
{
	struct ospf *ospf;
	struct interface *ifp;
	struct ospf_interface *oi;
	struct listnode *node;
	int write = 0;

	ospf = ospf_lookup();
	if (ospf != NULL && ospf->oi_running) {
		/* `router ospf' print. */
		if (ospf->instance)
			vty_out(vty, "router ospf %d%s", ospf->instance,
				VTY_NEWLINE);
		else
			vty_out(vty, "router ospf%s", VTY_NEWLINE);

		write++;

		if (!ospf->networks)
			return write;

		/* Router ID print. */
		if (ospf->router_id_static.s_addr != 0)
			vty_out(vty, " ospf router-id %s%s",
				inet_ntoa(ospf->router_id_static), VTY_NEWLINE);

		/* ABR type print. */
		if (ospf->abr_type != OSPF_ABR_DEFAULT)
			vty_out(vty, " ospf abr-type %s%s",
				ospf_abr_type_str[ospf->abr_type], VTY_NEWLINE);

		/* log-adjacency-changes flag print. */
		if (CHECK_FLAG(ospf->config, OSPF_LOG_ADJACENCY_CHANGES)) {
			if (CHECK_FLAG(ospf->config, OSPF_LOG_ADJACENCY_DETAIL))
				vty_out(vty, " log-adjacency-changes detail%s",
					VTY_NEWLINE);
			else if (!DFLT_OSPF_LOG_ADJACENCY_CHANGES)
				vty_out(vty, " log-adjacency-changes%s",
					VTY_NEWLINE);
		} else if (DFLT_OSPF_LOG_ADJACENCY_CHANGES) {
			vty_out(vty, " no log-adjacency-changes%s",
				VTY_NEWLINE);
		}

		/* RFC1583 compatibility flag print -- Compatible with CISCO
		 * 12.1. */
		if (CHECK_FLAG(ospf->config, OSPF_RFC1583_COMPATIBLE))
			vty_out(vty, " compatible rfc1583%s", VTY_NEWLINE);

		/* auto-cost reference-bandwidth configuration.  */
		if (ospf->ref_bandwidth != OSPF_DEFAULT_REF_BANDWIDTH) {
			vty_out(vty,
				"! Important: ensure reference bandwidth "
				"is consistent across all routers%s",
				VTY_NEWLINE);
			vty_out(vty, " auto-cost reference-bandwidth %d%s",
				ospf->ref_bandwidth, VTY_NEWLINE);
		}

		/* SPF timers print. */
		if (ospf->spf_delay != OSPF_SPF_DELAY_DEFAULT
		    || ospf->spf_holdtime != OSPF_SPF_HOLDTIME_DEFAULT
		    || ospf->spf_max_holdtime != OSPF_SPF_MAX_HOLDTIME_DEFAULT)
			vty_out(vty, " timers throttle spf %d %d %d%s",
				ospf->spf_delay, ospf->spf_holdtime,
				ospf->spf_max_holdtime, VTY_NEWLINE);

		/* LSA timers print. */
		if (ospf->min_ls_interval != OSPF_MIN_LS_INTERVAL)
			vty_out(vty, " timers throttle lsa all %d%s",
				ospf->min_ls_interval, VTY_NEWLINE);
		if (ospf->min_ls_arrival != OSPF_MIN_LS_ARRIVAL)
			vty_out(vty, " timers lsa min-arrival %d%s",
				ospf->min_ls_arrival, VTY_NEWLINE);

		/* Write multiplier print. */
		if (ospf->write_oi_count != OSPF_WRITE_INTERFACE_COUNT_DEFAULT)
			vty_out(vty, " ospf write-multiplier %d%s",
				ospf->write_oi_count, VTY_NEWLINE);

		/* Max-metric router-lsa print */
		config_write_stub_router(vty, ospf);

		/* SPF refresh parameters print. */
		if (ospf->lsa_refresh_interval
		    != OSPF_LSA_REFRESH_INTERVAL_DEFAULT)
			vty_out(vty, " refresh timer %d%s",
				ospf->lsa_refresh_interval, VTY_NEWLINE);

		/* Redistribute information print. */
		config_write_ospf_redistribute(vty, ospf);

		/* passive-interface print. */
		if (ospf->passive_interface_default == OSPF_IF_PASSIVE)
			vty_out(vty, " passive-interface default%s",
				VTY_NEWLINE);

		for (ALL_LIST_ELEMENTS_RO(om->iflist, node, ifp))
			if (OSPF_IF_PARAM_CONFIGURED(IF_DEF_PARAMS(ifp),
						     passive_interface)
			    && IF_DEF_PARAMS(ifp)->passive_interface
				       != ospf->passive_interface_default) {
				vty_out(vty, " %spassive-interface %s%s",
					IF_DEF_PARAMS(ifp)->passive_interface
						? ""
						: "no ",
					ifp->name, VTY_NEWLINE);
			}
		for (ALL_LIST_ELEMENTS_RO(ospf->oiflist, node, oi)) {
			if (!OSPF_IF_PARAM_CONFIGURED(oi->params,
						      passive_interface))
				continue;
			if (OSPF_IF_PARAM_CONFIGURED(IF_DEF_PARAMS(oi->ifp),
						     passive_interface)) {
				if (oi->params->passive_interface
				    == IF_DEF_PARAMS(oi->ifp)
					       ->passive_interface)
					continue;
			} else if (oi->params->passive_interface
				   == ospf->passive_interface_default)
				continue;

			vty_out(vty, " %spassive-interface %s %s%s",
				oi->params->passive_interface ? "" : "no ",
				oi->ifp->name,
				inet_ntoa(oi->address->u.prefix4), VTY_NEWLINE);
		}

		/* Network area print. */
		config_write_network_area(vty, ospf);

		/* Area config print. */
		config_write_ospf_area(vty, ospf);

		/* static neighbor print. */
		config_write_ospf_nbr_nbma(vty, ospf);

		/* Virtual-Link print. */
		config_write_virtual_link(vty, ospf);

		/* Default metric configuration.  */
		config_write_ospf_default_metric(vty, ospf);

		/* Distribute-list and default-information print. */
		config_write_ospf_distribute(vty, ospf);

		/* Distance configuration. */
		config_write_ospf_distance(vty, ospf);

		ospf_opaque_config_write_router(vty, ospf);
	}

	return write;
}

void ospf_vty_show_init(void)
{
	/* "show ip ospf" commands. */
	install_element(VIEW_NODE, &show_ip_ospf_cmd);

	install_element(VIEW_NODE, &show_ip_ospf_instance_cmd);

	/* "show ip ospf database" commands. */
	install_element(VIEW_NODE, &show_ip_ospf_database_type_adv_router_cmd);
	install_element(VIEW_NODE, &show_ip_ospf_database_max_cmd);

	install_element(VIEW_NODE,
			&show_ip_ospf_instance_database_type_adv_router_cmd);
	install_element(VIEW_NODE, &show_ip_ospf_instance_database_cmd);
	install_element(VIEW_NODE, &show_ip_ospf_instance_database_max_cmd);

	/* "show ip ospf interface" commands. */
	install_element(VIEW_NODE, &show_ip_ospf_interface_cmd);

	install_element(VIEW_NODE, &show_ip_ospf_instance_interface_cmd);

	/* "show ip ospf neighbor" commands. */
	install_element(VIEW_NODE, &show_ip_ospf_neighbor_int_detail_cmd);
	install_element(VIEW_NODE, &show_ip_ospf_neighbor_int_cmd);
	install_element(VIEW_NODE, &show_ip_ospf_neighbor_id_cmd);
	install_element(VIEW_NODE, &show_ip_ospf_neighbor_detail_all_cmd);
	install_element(VIEW_NODE, &show_ip_ospf_neighbor_detail_cmd);
	install_element(VIEW_NODE, &show_ip_ospf_neighbor_cmd);
	install_element(VIEW_NODE, &show_ip_ospf_neighbor_all_cmd);

	install_element(VIEW_NODE,
			&show_ip_ospf_instance_neighbor_int_detail_cmd);
	install_element(VIEW_NODE, &show_ip_ospf_instance_neighbor_int_cmd);
	install_element(VIEW_NODE, &show_ip_ospf_instance_neighbor_id_cmd);
	install_element(VIEW_NODE,
			&show_ip_ospf_instance_neighbor_detail_all_cmd);
	install_element(VIEW_NODE, &show_ip_ospf_instance_neighbor_detail_cmd);
	install_element(VIEW_NODE, &show_ip_ospf_instance_neighbor_cmd);
	install_element(VIEW_NODE, &show_ip_ospf_instance_neighbor_all_cmd);

	/* "show ip ospf route" commands. */
	install_element(VIEW_NODE, &show_ip_ospf_route_cmd);
	install_element(VIEW_NODE, &show_ip_ospf_border_routers_cmd);

	install_element(VIEW_NODE, &show_ip_ospf_instance_route_cmd);
	install_element(VIEW_NODE, &show_ip_ospf_instance_border_routers_cmd);
   
    /* djd add: show ip ospf virtual-link command */
    install_element(VIEW_NODE, &show_ip_ospf_vlink_cmd);
}


/* ospfd's interface node. */
static struct cmd_node interface_node = {INTERFACE_NODE, "%s(config-if)# ", 1};

/* Initialization of OSPF interface. */
static void ospf_vty_if_init(void)
{
	/* Install interface node. */
	install_node(&interface_node, config_write_interface);
	if_cmd_init();

	/* "ip ospf authentication" commands. */
	install_element(INTERFACE_NODE, &ip_ospf_authentication_args_addr_cmd);
	install_element(INTERFACE_NODE, &ip_ospf_authentication_addr_cmd);
	install_element(INTERFACE_NODE,
			&no_ip_ospf_authentication_args_addr_cmd);
	install_element(INTERFACE_NODE, &no_ip_ospf_authentication_addr_cmd);
	install_element(INTERFACE_NODE, &ip_ospf_authentication_key_addr_cmd);
	install_element(INTERFACE_NODE,
			&no_ip_ospf_authentication_key_authkey_addr_cmd);
	install_element(INTERFACE_NODE,
			&no_ospf_authentication_key_authkey_addr_cmd);

	/* "ip ospf message-digest-key" commands. */
	install_element(INTERFACE_NODE, &ip_ospf_message_digest_key_cmd);
	install_element(INTERFACE_NODE, &no_ip_ospf_message_digest_key_cmd);

	/* "ip ospf cost" commands. */
	install_element(INTERFACE_NODE, &ip_ospf_cost_cmd);
	install_element(INTERFACE_NODE, &no_ip_ospf_cost_cmd);

	/* "ip ospf mtu-ignore" commands. */
	install_element(INTERFACE_NODE, &ip_ospf_mtu_ignore_addr_cmd);
	install_element(INTERFACE_NODE, &no_ip_ospf_mtu_ignore_addr_cmd);

	/* "ip ospf dead-interval" commands. */
	install_element(INTERFACE_NODE, &ip_ospf_dead_interval_cmd);
	install_element(INTERFACE_NODE,
			&ip_ospf_dead_interval_minimal_addr_cmd);
	install_element(INTERFACE_NODE, &no_ip_ospf_dead_interval_cmd);

	/* "ip ospf hello-interval" commands. */
	install_element(INTERFACE_NODE, &ip_ospf_hello_interval_cmd);
	install_element(INTERFACE_NODE, &no_ip_ospf_hello_interval_cmd);

	/* "ip ospf network" commands. */
	install_element(INTERFACE_NODE, &ip_ospf_network_cmd);
	install_element(INTERFACE_NODE, &no_ip_ospf_network_cmd);

	/* "ip ospf priority" commands. */
	install_element(INTERFACE_NODE, &ip_ospf_priority_cmd);
	install_element(INTERFACE_NODE, &no_ip_ospf_priority_cmd);

	/* "ip ospf retransmit-interval" commands. */
	install_element(INTERFACE_NODE, &ip_ospf_retransmit_interval_addr_cmd);
	install_element(INTERFACE_NODE,
			&no_ip_ospf_retransmit_interval_addr_cmd);

	/* "ip ospf transmit-delay" commands. */
	install_element(INTERFACE_NODE, &ip_ospf_transmit_delay_addr_cmd);
	install_element(INTERFACE_NODE, &no_ip_ospf_transmit_delay_addr_cmd);

	/* "ip ospf area" commands. */
	install_element(INTERFACE_NODE, &ip_ospf_area_cmd);
	install_element(INTERFACE_NODE, &no_ip_ospf_area_cmd);

	/* These commands are compatibitliy for previous version. */
	install_element(INTERFACE_NODE, &ospf_authentication_key_cmd);
	install_element(INTERFACE_NODE, &ospf_message_digest_key_cmd);
	install_element(INTERFACE_NODE, &no_ospf_message_digest_key_cmd);
	install_element(INTERFACE_NODE, &ospf_dead_interval_cmd);
	install_element(INTERFACE_NODE, &no_ospf_dead_interval_cmd);
	install_element(INTERFACE_NODE, &ospf_hello_interval_cmd);
	install_element(INTERFACE_NODE, &no_ospf_hello_interval_cmd);
	install_element(INTERFACE_NODE, &ospf_cost_cmd);
	install_element(INTERFACE_NODE, &no_ospf_cost_cmd);
	install_element(INTERFACE_NODE, &ospf_network_cmd);
	install_element(INTERFACE_NODE, &no_ospf_network_cmd);
	install_element(INTERFACE_NODE, &ospf_priority_cmd);
	install_element(INTERFACE_NODE, &no_ospf_priority_cmd);
	install_element(INTERFACE_NODE, &ospf_retransmit_interval_cmd);
	install_element(INTERFACE_NODE, &no_ospf_retransmit_interval_cmd);
	install_element(INTERFACE_NODE, &ospf_transmit_delay_cmd);
	install_element(INTERFACE_NODE, &no_ospf_transmit_delay_cmd);
}

static void ospf_vty_zebra_init(void)
{
	install_element(OSPF_NODE, &ospf_redistribute_source_cmd);
	install_element(OSPF_NODE, &no_ospf_redistribute_source_cmd);
	install_element(OSPF_NODE, &ospf_redistribute_instance_source_cmd);
	install_element(OSPF_NODE, &no_ospf_redistribute_instance_source_cmd);

	install_element(OSPF_NODE, &ospf_distribute_list_out_cmd);
	install_element(OSPF_NODE, &no_ospf_distribute_list_out_cmd);

	install_element(OSPF_NODE, &ospf_default_information_originate_cmd);
	install_element(OSPF_NODE, &no_ospf_default_information_originate_cmd);

	install_element(OSPF_NODE, &ospf_default_metric_cmd);
	install_element(OSPF_NODE, &no_ospf_default_metric_cmd);

	install_element(OSPF_NODE, &ospf_distance_cmd);
	install_element(OSPF_NODE, &no_ospf_distance_cmd);
	install_element(OSPF_NODE, &no_ospf_distance_ospf_cmd);
	install_element(OSPF_NODE, &ospf_distance_ospf_cmd);
#if 0
  install_element (OSPF_NODE, &ospf_distance_source_cmd);
  install_element (OSPF_NODE, &no_ospf_distance_source_cmd);
  install_element (OSPF_NODE, &ospf_distance_source_access_list_cmd);
  install_element (OSPF_NODE, &no_ospf_distance_source_access_list_cmd);
#endif /* 0 */
}

static struct cmd_node ospf_node = {OSPF_NODE, "%s(config-router)# ", 1};

static void ospf_interface_clear(struct interface *ifp)
{
	if (!if_is_operative(ifp))
		return;

	if (IS_DEBUG_OSPF(ism, ISM_EVENTS))
		zlog_debug("ISM[%s]: clear by reset", ifp->name);

	ospf_if_reset(ifp);
}

DEFUN (clear_ip_ospf_interface,
       clear_ip_ospf_interface_cmd,
       "clear ip ospf interface [IFNAME]",
       CLEAR_STR
       IP_STR
       "OSPF information\n"
       "Interface information\n"
       "Interface name\n")
{
	int idx_ifname = 4;
	struct interface *ifp;
	struct listnode *node;

	if (argc == 4) /* Clear all the ospfv2 interfaces. */
	{
		for (ALL_LIST_ELEMENTS_RO(vrf_iflist(VRF_DEFAULT), node, ifp))
			ospf_interface_clear(ifp);
	} else /* Interface name is specified. */
	{
		if ((ifp = if_lookup_by_name(argv[idx_ifname]->arg,
					     VRF_DEFAULT))
		    == NULL)
			vty_out(vty, "No such interface name%s", VTY_NEWLINE);
		else
			ospf_interface_clear(ifp);
	}

	return CMD_SUCCESS;
}

/* modified by liwh for bug 44589, 2017-07-06 */
#ifdef _CENTEC_
DEFUN (clear_ip_ospf_process,
       clear_ip_ospf_process_cmd,
       "clear ip ospf process",
       CLEAR_STR
       IP_STR
       "OSPF information\n"
       "Reset OSPF process\n")
{
  struct interface *ifp;
  struct listnode *node;  
  struct ospf *ospf;

  ospf = ospf_lookup();
  if (!ospf || !ospf->oi_running)
    return CMD_SUCCESS;

  for (ALL_LIST_ELEMENTS_RO (vrf_iflist (VRF_DEFAULT), node, ifp))
        ospf_interface_clear(ifp);
  
  /* modified by liwh for bug 44650, 2017-07-14 */
  ospf_router_id_update (ospf, TRUE);
  /* liwh end */

  return CMD_SUCCESS;
}
#endif
/*liwh end */

/* modified by liwh for bug 44589, 2017-07-06 */
#ifdef _CENTEC_
static int
ospf_start_task_timer (struct thread *thread)
{
  struct ospf *ospf = THREAD_ARG (thread);
  struct interface *ifp;
  struct listnode *node;
  struct route_node *rn;
  struct route_node *rn_nbr;
  struct ospf_interface *oi;
  struct ospf_neighbor *nbr;
  bool   reset_if;
  struct ospf_if_params *params;

  ospf->t_start_reset_task = NULL;

  for (ALL_LIST_ELEMENTS_RO (vrf_iflist (VRF_DEFAULT), node, ifp))
    {
      params = IF_DEF_PARAMS (ifp);

      if (params->v_wait > OSPF_START_TASK_DELAY)
        {
          continue;
        }
      
      reset_if = FALSE;
      for (rn = route_top (IF_OIFS (ifp)); rn; rn = route_next (rn))
        {
          oi = rn->info;
    
          if (oi == NULL)
            continue;

          for (rn_nbr = route_top (oi->nbrs); rn_nbr; rn_nbr = route_next (rn_nbr))
            {
              if ((nbr = rn_nbr->info))
                {
                  /* Do not show myself. */
                  if (nbr != oi->nbr_self)
                    {
                      /* Down state is not shown. */
                      if ((nbr->state != NSM_Down) && (nbr->state != NSM_Full))
                        {
                          reset_if = TRUE;
                          break;
                        }
                    }
                }
            }

          if (reset_if) 
            {
              break;
            }
        }

      if (reset_if) 
        {
          ospf_interface_clear(ifp);
        }
    }

  return 0;
}

DEFUN (reset_ip_ospf_process,
       reset_ip_ospf_process_cmd,
       "reset ip ospf process",
       CLEAR_STR
       IP_STR
       "OSPF information\n"
       "Reset OSPF process\n")
{
  
  struct ospf *ospf;

  ospf = ospf_lookup();
  if (!ospf)
    return CMD_SUCCESS;

  if (ospf->t_start_reset_task == NULL)
    ospf->t_start_reset_task = thread_add_timer (master, ospf_start_task_timer,
        ospf, OSPF_START_TASK_DELAY);
 
  return CMD_SUCCESS;
}
#endif
/*liwh end */

void ospf_vty_clear_init(void)
{
	install_element(ENABLE_NODE, &clear_ip_ospf_interface_cmd);
/* modified by liwh for bug 44589, 2017-07-06 */
#ifdef _CENTEC_
  install_element (ENABLE_NODE, &clear_ip_ospf_process_cmd);
#endif
/*liwh end */
/* modified by liwh for bug 44929, 2017-08-08 */
#ifdef _CENTEC_
  install_element (ENABLE_NODE, &reset_ip_ospf_process_cmd);
#endif
/*liwh end */
}


/* Install OSPF related vty commands. */
void ospf_vty_init(void)
{
	/* Install ospf top node. */
	install_node(&ospf_node, ospf_config_write);

	/* "router ospf" commands. */
	install_element(CONFIG_NODE, &router_ospf_cmd);
	install_element(CONFIG_NODE, &no_router_ospf_cmd);


	install_default(OSPF_NODE);

	/* "ospf router-id" commands. */
	install_element(OSPF_NODE, &ospf_router_id_cmd);
	install_element(OSPF_NODE, &ospf_router_id_old_cmd);
	install_element(OSPF_NODE, &no_ospf_router_id_cmd);

	/* "passive-interface" commands. */
	install_element(OSPF_NODE, &ospf_passive_interface_addr_cmd);
	install_element(OSPF_NODE, &no_ospf_passive_interface_addr_cmd);

	/* "ospf abr-type" commands. */
	install_element(OSPF_NODE, &ospf_abr_type_cmd);
	install_element(OSPF_NODE, &no_ospf_abr_type_cmd);

	/* "ospf log-adjacency-changes" commands. */
	install_element(OSPF_NODE, &ospf_log_adjacency_changes_cmd);
	install_element(OSPF_NODE, &ospf_log_adjacency_changes_detail_cmd);
	install_element(OSPF_NODE, &no_ospf_log_adjacency_changes_cmd);
	install_element(OSPF_NODE, &no_ospf_log_adjacency_changes_detail_cmd);

	/* "ospf rfc1583-compatible" commands. */
	install_element(OSPF_NODE, &ospf_compatible_rfc1583_cmd);
	install_element(OSPF_NODE, &no_ospf_compatible_rfc1583_cmd);
	install_element(OSPF_NODE, &ospf_rfc1583_flag_cmd);
	install_element(OSPF_NODE, &no_ospf_rfc1583_flag_cmd);

	/* "network area" commands. */
	install_element(OSPF_NODE, &ospf_network_area_cmd);
	install_element(OSPF_NODE, &no_ospf_network_area_cmd);

	/* "area authentication" commands. */
	install_element(OSPF_NODE,
			&ospf_area_authentication_message_digest_cmd);
	install_element(OSPF_NODE, &ospf_area_authentication_cmd);
	install_element(OSPF_NODE, &no_ospf_area_authentication_cmd);

	/* "area range" commands.  */
	install_element(OSPF_NODE, &ospf_area_range_cmd);
	install_element(OSPF_NODE, &ospf_area_range_cost_cmd);
	install_element(OSPF_NODE, &ospf_area_range_not_advertise_cmd);
	install_element(OSPF_NODE, &no_ospf_area_range_cmd);
	install_element(OSPF_NODE, &ospf_area_range_substitute_cmd);
	install_element(OSPF_NODE, &no_ospf_area_range_substitute_cmd);

	/* "area virtual-link" commands. */
	install_element(OSPF_NODE, &ospf_area_vlink_cmd);
	install_element(OSPF_NODE, &ospf_area_vlink_intervals_cmd);
	install_element(OSPF_NODE, &no_ospf_area_vlink_cmd);
	install_element(OSPF_NODE, &no_ospf_area_vlink_intervals_cmd);


	/* "area stub" commands. */
	install_element(OSPF_NODE, &ospf_area_stub_no_summary_cmd);
	install_element(OSPF_NODE, &ospf_area_stub_cmd);
	install_element(OSPF_NODE, &no_ospf_area_stub_no_summary_cmd);
	install_element(OSPF_NODE, &no_ospf_area_stub_cmd);

	/* "area nssa" commands. */
	install_element(OSPF_NODE, &ospf_area_nssa_cmd);
	install_element(OSPF_NODE, &ospf_area_nssa_translate_no_summary_cmd);
	install_element(OSPF_NODE, &ospf_area_nssa_translate_cmd);
	install_element(OSPF_NODE, &ospf_area_nssa_no_summary_cmd);
	install_element(OSPF_NODE, &no_ospf_area_nssa_cmd);

	install_element(OSPF_NODE, &ospf_area_default_cost_cmd);
	install_element(OSPF_NODE, &no_ospf_area_default_cost_cmd);

	install_element(OSPF_NODE, &ospf_area_shortcut_cmd);
	install_element(OSPF_NODE, &no_ospf_area_shortcut_cmd);

	install_element(OSPF_NODE, &ospf_area_export_list_cmd);
	install_element(OSPF_NODE, &no_ospf_area_export_list_cmd);

	install_element(OSPF_NODE, &ospf_area_filter_list_cmd);
	install_element(OSPF_NODE, &no_ospf_area_filter_list_cmd);

	install_element(OSPF_NODE, &ospf_area_import_list_cmd);
	install_element(OSPF_NODE, &no_ospf_area_import_list_cmd);

	/* SPF timer commands */
	install_element(OSPF_NODE, &ospf_timers_throttle_spf_cmd);
	install_element(OSPF_NODE, &no_ospf_timers_throttle_spf_cmd);

	/* LSA timers commands */
	install_element(OSPF_NODE, &ospf_timers_min_ls_interval_cmd);
	install_element(OSPF_NODE, &no_ospf_timers_min_ls_interval_cmd);
	install_element(OSPF_NODE, &ospf_timers_min_ls_arrival_cmd);
	install_element(OSPF_NODE, &no_ospf_timers_min_ls_arrival_cmd);
	install_element(OSPF_NODE, &ospf_timers_lsa_cmd);
	install_element(OSPF_NODE, &no_ospf_timers_lsa_cmd);

	/* refresh timer commands */
	install_element(OSPF_NODE, &ospf_refresh_timer_cmd);
	install_element(OSPF_NODE, &no_ospf_refresh_timer_val_cmd);

	/* max-metric commands */
	install_element(OSPF_NODE, &ospf_max_metric_router_lsa_admin_cmd);
	install_element(OSPF_NODE, &no_ospf_max_metric_router_lsa_admin_cmd);
	install_element(OSPF_NODE, &ospf_max_metric_router_lsa_startup_cmd);
	install_element(OSPF_NODE, &no_ospf_max_metric_router_lsa_startup_cmd);
	install_element(OSPF_NODE, &ospf_max_metric_router_lsa_shutdown_cmd);
	install_element(OSPF_NODE, &no_ospf_max_metric_router_lsa_shutdown_cmd);

	/* reference bandwidth commands */
	install_element(OSPF_NODE, &ospf_auto_cost_reference_bandwidth_cmd);
	install_element(OSPF_NODE, &no_ospf_auto_cost_reference_bandwidth_cmd);

	/* "neighbor" commands. */
	install_element(OSPF_NODE, &ospf_neighbor_cmd);
	install_element(OSPF_NODE, &ospf_neighbor_poll_interval_cmd);
	install_element(OSPF_NODE, &no_ospf_neighbor_cmd);
	install_element(OSPF_NODE, &no_ospf_neighbor_poll_cmd);

	/* write multiplier commands */
	install_element(OSPF_NODE, &ospf_write_multiplier_cmd);
	install_element(OSPF_NODE, &write_multiplier_cmd);
	install_element(OSPF_NODE, &no_ospf_write_multiplier_cmd);
	install_element(OSPF_NODE, &no_write_multiplier_cmd);

	/* Init interface related vty commands. */
	ospf_vty_if_init();

	/* Init zebra related vty commands. */
	ospf_vty_zebra_init();
}
