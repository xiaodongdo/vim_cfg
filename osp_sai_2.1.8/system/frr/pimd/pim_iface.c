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

#include "if.h"
#include "log.h"
#include "vty.h"
#include "memory.h"
#include "prefix.h"
#include "vrf.h"
#include "linklist.h"
#include "plist.h"
#include "hash.h"

#include "pimd.h"
#include "pim_zebra.h"
#include "pim_iface.h"
#include "pim_igmp.h"
#include "pim_mroute.h"
#include "pim_oil.h"
#include "pim_str.h"
#include "pim_pim.h"
#include "pim_neighbor.h"
#include "pim_ifchannel.h"
#include "pim_sock.h"
#include "pim_time.h"
#include "pim_ssmpingd.h"
#include "pim_rp.h"
#include "pim_nht.h"

struct interface *pim_regiface = NULL;
struct list *pim_ifchannel_list = NULL;
static int pim_iface_vif_index[MAXVIFS];

static void pim_if_igmp_join_del_all(struct interface *ifp);
static int igmp_join_sock(const char *ifname, ifindex_t ifindex,
			  struct in_addr group_addr,
			  struct in_addr source_addr);

void pim_if_init(void)
{
	int i;

	for (i = 0; i < MAXVIFS; i++)
		pim_iface_vif_index[i] = 0;

	vrf_iflist_create(VRF_DEFAULT);
	pim_ifchannel_list = list_new();
	pim_ifchannel_list->cmp =
		(int (*)(void *, void *))pim_ifchannel_compare;
}

void pim_if_terminate(void)
{
	if (pim_ifchannel_list)
		list_free(pim_ifchannel_list);
}

static void *if_list_clean(struct pim_interface *pim_ifp)
{
	if (pim_ifp->igmp_join_list) {
		list_delete(pim_ifp->igmp_join_list);
	}

	if (pim_ifp->igmp_socket_list) {
		list_delete(pim_ifp->igmp_socket_list);
	}

	if (pim_ifp->pim_neighbor_list) {
		list_delete(pim_ifp->pim_neighbor_list);
	}

	if (pim_ifp->upstream_switch_list)
		list_delete(pim_ifp->upstream_switch_list);

	if (pim_ifp->pim_ifchannel_list) {
		list_delete(pim_ifp->pim_ifchannel_list);
	}

	if (pim_ifp->pim_ifchannel_hash)
		hash_free(pim_ifp->pim_ifchannel_hash);

	XFREE(MTYPE_PIM_INTERFACE, pim_ifp);

	return 0;
}

struct pim_interface *pim_if_new(struct interface *ifp, int igmp, int pim)
{
	struct pim_interface *pim_ifp;

	zassert(ifp);
	zassert(!ifp->info);

	pim_ifp = XCALLOC(MTYPE_PIM_INTERFACE, sizeof(*pim_ifp));
	if (!pim_ifp) {
		zlog_err("PIM XCALLOC(%zu) failure", sizeof(*pim_ifp));
		return 0;
	}

	pim_ifp->options = 0;
	pim_ifp->mroute_vif_index = -1;

	pim_ifp->igmp_version = IGMP_DEFAULT_VERSION;
	pim_ifp->igmp_default_robustness_variable =
		IGMP_DEFAULT_ROBUSTNESS_VARIABLE;
	pim_ifp->igmp_default_query_interval = IGMP_GENERAL_QUERY_INTERVAL;
	pim_ifp->igmp_query_max_response_time_dsec =
		IGMP_QUERY_MAX_RESPONSE_TIME_DSEC;
	pim_ifp->igmp_specific_query_max_response_time_dsec =
		IGMP_SPECIFIC_QUERY_MAX_RESPONSE_TIME_DSEC;

	/*
	  RFC 3376: 8.3. Query Response Interval
	  The number of seconds represented by the [Query Response Interval]
	  must be less than the [Query Interval].
	 */
	zassert(pim_ifp->igmp_query_max_response_time_dsec
		< pim_ifp->igmp_default_query_interval);

	if (pim)
		PIM_IF_DO_PIM(pim_ifp->options);
	if (igmp)
		PIM_IF_DO_IGMP(pim_ifp->options);

	PIM_IF_DO_IGMP_LISTEN_ALLROUTERS(pim_ifp->options);

	pim_ifp->igmp_join_list = NULL;
	pim_ifp->igmp_socket_list = NULL;
	pim_ifp->pim_neighbor_list = NULL;
	pim_ifp->upstream_switch_list = NULL;
	pim_ifp->pim_ifchannel_list = NULL;
	pim_ifp->pim_ifchannel_hash = NULL;
	pim_ifp->pim_generation_id = 0;

	/* list of struct igmp_sock */
	pim_ifp->igmp_socket_list = list_new();
	if (!pim_ifp->igmp_socket_list) {
		zlog_err("%s %s: failure: igmp_socket_list=list_new()",
			 __FILE__, __PRETTY_FUNCTION__);
		return if_list_clean(pim_ifp);
	}
	pim_ifp->igmp_socket_list->del = (void (*)(void *))igmp_sock_free;

	/* list of struct pim_neighbor */
	pim_ifp->pim_neighbor_list = list_new();
	if (!pim_ifp->pim_neighbor_list) {
		zlog_err("%s %s: failure: pim_neighbor_list=list_new()",
			 __FILE__, __PRETTY_FUNCTION__);
		return if_list_clean(pim_ifp);
	}
	pim_ifp->pim_neighbor_list->del = (void (*)(void *))pim_neighbor_free;

	pim_ifp->upstream_switch_list = list_new();
	if (!pim_ifp->upstream_switch_list) {
		zlog_err("%s %s: failure: upstream_switch_list=list_new()",
			 __FILE__, __PRETTY_FUNCTION__);
		return if_list_clean(pim_ifp);
	}

	/* list of struct pim_ifchannel */
	pim_ifp->pim_ifchannel_list = list_new();
	if (!pim_ifp->pim_ifchannel_list) {
		zlog_err("%s %s: failure: pim_ifchannel_list=list_new()",
			 __FILE__, __PRETTY_FUNCTION__);
		return if_list_clean(pim_ifp);
	}
	pim_ifp->pim_ifchannel_list->del = (void (*)(void *))pim_ifchannel_free;
	pim_ifp->pim_ifchannel_list->cmp =
		(int (*)(void *, void *))pim_ifchannel_compare;

	pim_ifp->pim_ifchannel_hash =
		hash_create(pim_ifchannel_hash_key, pim_ifchannel_equal);

	ifp->info = pim_ifp;

	pim_sock_reset(ifp);

	pim_if_add_vif(ifp);

	return pim_ifp;
}

void pim_if_delete(struct interface *ifp)
{
	struct pim_interface *pim_ifp;

	zassert(ifp);
	pim_ifp = ifp->info;
	zassert(pim_ifp);

	if (pim_ifp->igmp_join_list) {
		pim_if_igmp_join_del_all(ifp);
	}

	pim_ifchannel_delete_all(ifp);
	igmp_sock_delete_all(ifp);

	pim_neighbor_delete_all(ifp, "Interface removed from configuration");

	pim_if_del_vif(ifp);

	list_delete(pim_ifp->igmp_socket_list);
	list_delete(pim_ifp->pim_neighbor_list);
	list_delete(pim_ifp->upstream_switch_list);
	list_delete(pim_ifp->pim_ifchannel_list);

	hash_free(pim_ifp->pim_ifchannel_hash);

	XFREE(MTYPE_PIM_INTERFACE, pim_ifp);

	ifp->info = NULL;
}

void pim_if_update_could_assert(struct interface *ifp)
{
	struct pim_interface *pim_ifp;
	struct listnode *node;
	struct listnode *next_node;
	struct pim_ifchannel *ch;

	pim_ifp = ifp->info;
	zassert(pim_ifp);

	for (ALL_LIST_ELEMENTS(pim_ifp->pim_ifchannel_list, node, next_node,
			       ch)) {
		pim_ifchannel_update_could_assert(ch);
	}
}

static void pim_if_update_my_assert_metric(struct interface *ifp)
{
	struct pim_interface *pim_ifp;
	struct listnode *node;
	struct listnode *next_node;
	struct pim_ifchannel *ch;

	pim_ifp = ifp->info;
	zassert(pim_ifp);

	for (ALL_LIST_ELEMENTS(pim_ifp->pim_ifchannel_list, node, next_node,
			       ch)) {
		pim_ifchannel_update_my_assert_metric(ch);
	}
}

static void pim_addr_change(struct interface *ifp)
{
	struct pim_interface *pim_ifp;

	pim_ifp = ifp->info;
	zassert(pim_ifp);

	pim_if_dr_election(ifp); /* router's own DR Priority (addr) changes --
				    Done TODO T30 */
	pim_if_update_join_desired(pim_ifp); /* depends on DR */
	pim_if_update_could_assert(ifp);     /* depends on DR */
	pim_if_update_my_assert_metric(ifp); /* depends on could_assert */
	pim_if_update_assert_tracking_desired(
		ifp); /* depends on DR, join_desired */

	/*
	  RFC 4601: 4.3.1.  Sending Hello Messages

	  1) Before an interface goes down or changes primary IP address, a
	  Hello message with a zero HoldTime should be sent immediately
	  (with the old IP address if the IP address changed).
	  -- FIXME See CAVEAT C13

	  2) After an interface has changed its IP address, it MUST send a
	  Hello message with its new IP address.
	  -- DONE below

	  3) If an interface changes one of its secondary IP addresses, a
	  Hello message with an updated Address_List option and a non-zero
	  HoldTime should be sent immediately.
	  -- FIXME See TODO T31
	 */
	pim_ifp->pim_ifstat_hello_sent = 0; /* reset hello counter */
	if (pim_ifp->pim_sock_fd < 0)
		return;
	pim_hello_restart_now(ifp); /* send hello and restart timer */
}

static int detect_primary_address_change(struct interface *ifp,
					 int force_prim_as_any,
					 const char *caller)
{
	struct pim_interface *pim_ifp = ifp->info;
	struct in_addr new_prim_addr;
	int changed;

	if (force_prim_as_any)
		new_prim_addr = qpim_inaddr_any;
	else
		new_prim_addr = pim_find_primary_addr(ifp);

	changed = new_prim_addr.s_addr != pim_ifp->primary_address.s_addr;

	if (PIM_DEBUG_ZEBRA) {
		char new_prim_str[INET_ADDRSTRLEN];
		char old_prim_str[INET_ADDRSTRLEN];
		pim_inet4_dump("<new?>", new_prim_addr, new_prim_str,
			       sizeof(new_prim_str));
		pim_inet4_dump("<old?>", pim_ifp->primary_address, old_prim_str,
			       sizeof(old_prim_str));
		zlog_debug("%s: old=%s new=%s on interface %s: %s",
			   __PRETTY_FUNCTION__, old_prim_str, new_prim_str,
			   ifp->name, changed ? "changed" : "unchanged");
	}

	if (changed) {
		pim_ifp->primary_address = new_prim_addr;
	}

	return changed;
}

static int pim_sec_addr_comp(const void *p1, const void *p2)
{
	const struct pim_secondary_addr *sec1 = p1;
	const struct pim_secondary_addr *sec2 = p2;

	if (ntohl(sec1->addr.s_addr) < ntohl(sec2->addr.s_addr))
		return -1;

	if (ntohl(sec1->addr.s_addr) > ntohl(sec2->addr.s_addr))
		return 1;

	return 0;
}

static void pim_sec_addr_free(struct pim_secondary_addr *sec_addr)
{
	XFREE(MTYPE_PIM_SEC_ADDR, sec_addr);
}

static struct pim_secondary_addr *
pim_sec_addr_find(struct pim_interface *pim_ifp, struct in_addr addr)
{
	struct pim_secondary_addr *sec_addr;
	struct listnode *node;

	if (!pim_ifp->sec_addr_list) {
		return NULL;
	}

	for (ALL_LIST_ELEMENTS_RO(pim_ifp->sec_addr_list, node, sec_addr)) {
		if (sec_addr->addr.s_addr == addr.s_addr) {
			return sec_addr;
		}
	}

	return NULL;
}

static void pim_sec_addr_del(struct pim_interface *pim_ifp,
			     struct pim_secondary_addr *sec_addr)
{
	listnode_delete(pim_ifp->sec_addr_list, sec_addr);
	pim_sec_addr_free(sec_addr);
}

static int pim_sec_addr_add(struct pim_interface *pim_ifp, struct in_addr addr)
{
	int changed = 0;
	struct pim_secondary_addr *sec_addr;

	sec_addr = pim_sec_addr_find(pim_ifp, addr);
	if (sec_addr) {
		sec_addr->flags &= ~PIM_SEC_ADDRF_STALE;
		return changed;
	}

	if (!pim_ifp->sec_addr_list) {
		pim_ifp->sec_addr_list = list_new();
		pim_ifp->sec_addr_list->del =
			(void (*)(void *))pim_sec_addr_free;
		pim_ifp->sec_addr_list->cmp =
			(int (*)(void *, void *))pim_sec_addr_comp;
	}

	sec_addr = XCALLOC(MTYPE_PIM_SEC_ADDR, sizeof(*sec_addr));
	if (!sec_addr) {
		if (list_isempty(pim_ifp->sec_addr_list)) {
			list_free(pim_ifp->sec_addr_list);
			pim_ifp->sec_addr_list = NULL;
		}
		return changed;
	}

	changed = 1;
	sec_addr->addr = addr;
	listnode_add_sort(pim_ifp->sec_addr_list, sec_addr);

	return changed;
}

static int pim_sec_addr_del_all(struct pim_interface *pim_ifp)
{
	int changed = 0;

	if (!pim_ifp->sec_addr_list) {
		return changed;
	}
	if (!list_isempty(pim_ifp->sec_addr_list)) {
		changed = 1;
		/* remove all nodes and free up the list itself */
		list_delete_all_node(pim_ifp->sec_addr_list);
		list_free(pim_ifp->sec_addr_list);
		pim_ifp->sec_addr_list = NULL;
	}

	return changed;
}

static int pim_sec_addr_update(struct interface *ifp)
{
	struct pim_interface *pim_ifp = ifp->info;
	struct connected *ifc;
	struct listnode *node;
	struct listnode *nextnode;
	struct pim_secondary_addr *sec_addr;
	int changed = 0;

	if (pim_ifp->sec_addr_list) {
		for (ALL_LIST_ELEMENTS_RO(pim_ifp->sec_addr_list, node,
					  sec_addr)) {
			sec_addr->flags |= PIM_SEC_ADDRF_STALE;
		}
	}

	for (ALL_LIST_ELEMENTS_RO(ifp->connected, node, ifc)) {
		struct prefix *p = ifc->address;

		if (p->family != AF_INET) {
			continue;
		}

		if (PIM_INADDR_IS_ANY(p->u.prefix4)) {
			continue;
		}

		if (pim_ifp->primary_address.s_addr == p->u.prefix4.s_addr) {
			/* don't add the primary address into the secondary
			 * address list */
			continue;
		}

		if (pim_sec_addr_add(pim_ifp, p->u.prefix4)) {
			changed = 1;
		}
	}

	if (pim_ifp->sec_addr_list) {
		/* Drop stale entries */
		for (ALL_LIST_ELEMENTS(pim_ifp->sec_addr_list, node, nextnode,
				       sec_addr)) {
			if (sec_addr->flags & PIM_SEC_ADDRF_STALE) {
				pim_sec_addr_del(pim_ifp, sec_addr);
				changed = 1;
			}
		}

		/* If the list went empty free it up */
		if (list_isempty(pim_ifp->sec_addr_list)) {
			list_free(pim_ifp->sec_addr_list);
			pim_ifp->sec_addr_list = NULL;
		}
	}

	return changed;
}

static int detect_secondary_address_change(struct interface *ifp,
					   int force_prim_as_any,
					   const char *caller)
{
	struct pim_interface *pim_ifp = ifp->info;
	int changed = 0;

	if (force_prim_as_any) {
		/* if primary address is being forced to zero just flush the
		 * secondary address list */
		changed = pim_sec_addr_del_all(pim_ifp);
	} else {
		/* re-evaluate the secondary address list */
		changed = pim_sec_addr_update(ifp);
	}

	return changed;
}

static void detect_address_change(struct interface *ifp, int force_prim_as_any,
				  const char *caller)
{
	int changed = 0;
	struct pim_interface *pim_ifp;

	pim_ifp = ifp->info;
	if (!pim_ifp)
		return;

	if (detect_primary_address_change(ifp, force_prim_as_any, caller)) {
		changed = 1;
	}

	if (detect_secondary_address_change(ifp, force_prim_as_any, caller)) {
		changed = 1;
	}


	if (changed) {
		if (!PIM_IF_TEST_PIM(pim_ifp->options)) {
			return;
		}

		pim_addr_change(ifp);
	}

	/* XXX: if we have unnumbered interfaces we need to run detect address
	 * address change on all of them when the lo address changes */
}

int pim_update_source_set(struct interface *ifp, struct in_addr source)
{
	struct pim_interface *pim_ifp = ifp->info;

	if (!pim_ifp) {
		return PIM_IFACE_NOT_FOUND;
	}

	if (pim_ifp->update_source.s_addr == source.s_addr) {
		return PIM_UPDATE_SOURCE_DUP;
	}

	pim_ifp->update_source = source;
	detect_address_change(ifp, 0 /* force_prim_as_any */,
			      __PRETTY_FUNCTION__);

	return PIM_SUCCESS;
}

void pim_if_addr_add(struct connected *ifc)
{
	struct pim_interface *pim_ifp;
	struct interface *ifp;
	struct in_addr ifaddr;

	zassert(ifc);

	ifp = ifc->ifp;
	zassert(ifp);
	pim_ifp = ifp->info;
	if (!pim_ifp)
		return;

	if (!if_is_operative(ifp))
		return;

	if (PIM_DEBUG_ZEBRA) {
		char buf[BUFSIZ];
		prefix2str(ifc->address, buf, BUFSIZ);
		zlog_debug("%s: %s ifindex=%d connected IP address %s %s",
			   __PRETTY_FUNCTION__, ifp->name, ifp->ifindex, buf,
			   CHECK_FLAG(ifc->flags, ZEBRA_IFA_SECONDARY)
				   ? "secondary"
				   : "primary");
	}

	ifaddr = ifc->address->u.prefix4;

	detect_address_change(ifp, 0, __PRETTY_FUNCTION__);

	if (PIM_IF_TEST_IGMP(pim_ifp->options)) {
		struct igmp_sock *igmp;

		/* lookup IGMP socket */
		igmp = pim_igmp_sock_lookup_ifaddr(pim_ifp->igmp_socket_list,
						   ifaddr);
		if (!igmp) {
			/* if addr new, add IGMP socket */
			pim_igmp_sock_add(pim_ifp->igmp_socket_list, ifaddr,
					  ifp);
		}

		/* Replay Static IGMP groups */
		if (pim_ifp->igmp_join_list) {
			struct listnode *node;
			struct listnode *nextnode;
			struct igmp_join *ij;
			int join_fd;

			for (ALL_LIST_ELEMENTS(pim_ifp->igmp_join_list, node,
					       nextnode, ij)) {
				/* Close socket and reopen with Source and Group
				 */
				close(ij->sock_fd);
				join_fd = igmp_join_sock(
					ifp->name, ifp->ifindex, ij->group_addr,
					ij->source_addr);
				if (join_fd < 0) {
					char group_str[INET_ADDRSTRLEN];
					char source_str[INET_ADDRSTRLEN];
					pim_inet4_dump("<grp?>", ij->group_addr,
						       group_str,
						       sizeof(group_str));
					pim_inet4_dump(
						"<src?>", ij->source_addr,
						source_str, sizeof(source_str));
					zlog_warn(
						"%s: igmp_join_sock() failure for IGMP group %s source %s on interface %s",
						__PRETTY_FUNCTION__, group_str,
						source_str, ifp->name);
					/* warning only */
				} else
					ij->sock_fd = join_fd;
			}
		}
	} /* igmp */

	if (PIM_IF_TEST_PIM(pim_ifp->options)) {

		if (PIM_INADDR_ISNOT_ANY(pim_ifp->primary_address)) {

			/* Interface has a valid socket ? */
			if (pim_ifp->pim_sock_fd < 0) {
				if (pim_sock_add(ifp)) {
					zlog_warn(
						"Failure creating PIM socket for interface %s",
						ifp->name);
				}
			}
			struct pim_nexthop_cache *pnc = NULL;
			struct pim_rpf rpf;
			struct zclient *zclient = NULL;

			zclient = pim_zebra_zclient_get();
			/* RP config might come prior to (local RP's interface)
			   IF UP event.
			   In this case, pnc would not have pim enabled
			   nexthops.
			   Once Interface is UP and pim info is available,
			   reregister
			   with RNH address to receive update and add the
			   interface as nexthop. */
			memset(&rpf, 0, sizeof(struct pim_rpf));
			rpf.rpf_addr.family = AF_INET;
			rpf.rpf_addr.prefixlen = IPV4_MAX_BITLEN;
			rpf.rpf_addr.u.prefix4 = ifc->address->u.prefix4;
			pnc = pim_nexthop_cache_find(&rpf);
			if (pnc)
				pim_sendmsg_zebra_rnh(zclient, pnc,
						      ZEBRA_NEXTHOP_REGISTER);
		}
	} /* pim */

	/*
	  PIM or IGMP is enabled on interface, and there is at least one
	  address assigned, then try to create a vif_index.
	*/
	if (pim_ifp->mroute_vif_index < 0) {
		pim_if_add_vif(ifp);
	}
	pim_ifchannel_scan_forward_start(ifp);
}

static void pim_if_addr_del_igmp(struct connected *ifc)
{
	struct pim_interface *pim_ifp = ifc->ifp->info;
	struct igmp_sock *igmp;
	struct in_addr ifaddr;

	if (ifc->address->family != AF_INET) {
		/* non-IPv4 address */
		return;
	}

	if (!pim_ifp) {
		/* IGMP not enabled on interface */
		return;
	}

	ifaddr = ifc->address->u.prefix4;

	/* lookup IGMP socket */
	igmp = pim_igmp_sock_lookup_ifaddr(pim_ifp->igmp_socket_list, ifaddr);
	if (igmp) {
		/* if addr found, del IGMP socket */
		igmp_sock_delete(igmp);
	}
}

static void pim_if_addr_del_pim(struct connected *ifc)
{
	struct pim_interface *pim_ifp = ifc->ifp->info;

	if (ifc->address->family != AF_INET) {
		/* non-IPv4 address */
		return;
	}

	if (!pim_ifp) {
		/* PIM not enabled on interface */
		return;
	}

	if (PIM_INADDR_ISNOT_ANY(pim_ifp->primary_address)) {
		/* Interface keeps a valid primary address */
		return;
	}

	if (pim_ifp->pim_sock_fd < 0) {
		/* Interface does not hold a valid socket any longer */
		return;
	}

	/*
	  pim_sock_delete() closes the socket, stops read and timer threads,
	  and kills all neighbors.
	 */
	pim_sock_delete(ifc->ifp,
			"last address has been removed from interface");
}

void pim_if_addr_del(struct connected *ifc, int force_prim_as_any)
{
	struct interface *ifp;

	zassert(ifc);
	ifp = ifc->ifp;
	zassert(ifp);

	if (PIM_DEBUG_ZEBRA) {
		char buf[BUFSIZ];
		prefix2str(ifc->address, buf, BUFSIZ);
		zlog_debug("%s: %s ifindex=%d disconnected IP address %s %s",
			   __PRETTY_FUNCTION__, ifp->name, ifp->ifindex, buf,
			   CHECK_FLAG(ifc->flags, ZEBRA_IFA_SECONDARY)
				   ? "secondary"
				   : "primary");
	}

	detect_address_change(ifp, force_prim_as_any, __PRETTY_FUNCTION__);

	pim_if_addr_del_igmp(ifc);
	pim_if_addr_del_pim(ifc);
}

void pim_if_addr_add_all(struct interface *ifp)
{
	struct connected *ifc;
	struct listnode *node;
	struct listnode *nextnode;
	int v4_addrs = 0;
	int v6_addrs = 0;
	struct pim_interface *pim_ifp = ifp->info;


	/* PIM/IGMP enabled ? */
	if (!pim_ifp)
		return;

	for (ALL_LIST_ELEMENTS(ifp->connected, node, nextnode, ifc)) {
		struct prefix *p = ifc->address;

		if (p->family != AF_INET) {
			v6_addrs++;
			continue;
		}

		v4_addrs++;
		pim_if_addr_add(ifc);
	}

	if (!v4_addrs && v6_addrs && !if_is_loopback(ifp)) {
		if (PIM_IF_TEST_PIM(pim_ifp->options)) {

			/* Interface has a valid primary address ? */
			if (PIM_INADDR_ISNOT_ANY(pim_ifp->primary_address)) {

				/* Interface has a valid socket ? */
				if (pim_ifp->pim_sock_fd < 0) {
					if (pim_sock_add(ifp)) {
						zlog_warn(
							"Failure creating PIM socket for interface %s",
							ifp->name);
					}
				}
			}
		} /* pim */
	}
	/*
	 * PIM or IGMP is enabled on interface, and there is at least one
	 * address assigned, then try to create a vif_index.
	 */
	if (pim_ifp->mroute_vif_index < 0) {
		pim_if_add_vif(ifp);
	}
	pim_ifchannel_scan_forward_start(ifp);

	pim_rp_setup();
	pim_rp_check_on_if_add(pim_ifp);
}

void pim_if_addr_del_all(struct interface *ifp)
{
	struct connected *ifc;
	struct listnode *node;
	struct listnode *nextnode;

	/* PIM/IGMP enabled ? */
	if (!ifp->info)
		return;

	for (ALL_LIST_ELEMENTS(ifp->connected, node, nextnode, ifc)) {
		struct prefix *p = ifc->address;

		if (p->family != AF_INET)
			continue;

		pim_if_addr_del(ifc, 1 /* force_prim_as_any=true */);
	}

	pim_rp_setup();
	pim_i_am_rp_re_evaluate();
}

void pim_if_addr_del_all_igmp(struct interface *ifp)
{
	struct connected *ifc;
	struct listnode *node;
	struct listnode *nextnode;

	/* PIM/IGMP enabled ? */
	if (!ifp->info)
		return;

	for (ALL_LIST_ELEMENTS(ifp->connected, node, nextnode, ifc)) {
		struct prefix *p = ifc->address;

		if (p->family != AF_INET)
			continue;

		pim_if_addr_del_igmp(ifc);
	}
}

void pim_if_addr_del_all_pim(struct interface *ifp)
{
	struct connected *ifc;
	struct listnode *node;
	struct listnode *nextnode;

	/* PIM/IGMP enabled ? */
	if (!ifp->info)
		return;

	for (ALL_LIST_ELEMENTS(ifp->connected, node, nextnode, ifc)) {
		struct prefix *p = ifc->address;

		if (p->family != AF_INET)
			continue;

		pim_if_addr_del_pim(ifc);
	}
}

struct in_addr pim_find_primary_addr(struct interface *ifp)
{
	struct connected *ifc;
	struct listnode *node;
	struct in_addr addr;
	int v4_addrs = 0;
	int v6_addrs = 0;
	struct pim_interface *pim_ifp = ifp->info;

	if (pim_ifp && PIM_INADDR_ISNOT_ANY(pim_ifp->update_source)) {
		return pim_ifp->update_source;
	}

	for (ALL_LIST_ELEMENTS_RO(ifp->connected, node, ifc)) {
		struct prefix *p = ifc->address;

		if (p->family != AF_INET) {
			v6_addrs++;
			continue;
		}

		if (PIM_INADDR_IS_ANY(p->u.prefix4)) {
			zlog_warn(
				"%s: null IPv4 address connected to interface %s",
				__PRETTY_FUNCTION__, ifp->name);
			continue;
		}

		v4_addrs++;

		if (CHECK_FLAG(ifc->flags, ZEBRA_IFA_SECONDARY))
			continue;

		return p->u.prefix4;
	}

	/*
	 * If we have no v4_addrs and v6 is configured
	 * We probably are using unnumbered
	 * So let's grab the loopbacks v4 address
	 * and use that as the primary address
	 */
	if (!v4_addrs && v6_addrs && !if_is_loopback(ifp)) {
		struct interface *lo_ifp;
		lo_ifp = if_lookup_by_name("lo", VRF_DEFAULT);
		if (lo_ifp)
			return pim_find_primary_addr(lo_ifp);
	}

	addr.s_addr = PIM_NET_INADDR_ANY;

	return addr;
}

static int pim_iface_next_vif_index(struct interface *ifp)
{
	int i;
	/*
	 * The pimreg vif is always going to be in index 0
	 * of the table.
	 */
	if (ifp->ifindex == PIM_OIF_PIM_REGISTER_VIF)
		return 0;

	for (i = 1; i < MAXVIFS; i++) {
		if (pim_iface_vif_index[i] == 0)
			return i;
	}
	return MAXVIFS;
}

/*
  pim_if_add_vif() uses ifindex as vif_index

  see also pim_if_find_vifindex_by_ifindex()
 */
int pim_if_add_vif(struct interface *ifp)
{
	struct pim_interface *pim_ifp = ifp->info;
	struct in_addr ifaddr;
	unsigned char flags = 0;

	zassert(pim_ifp);

	if (pim_ifp->mroute_vif_index > 0) {
		zlog_warn("%s: vif_index=%d > 0 on interface %s ifindex=%d",
			  __PRETTY_FUNCTION__, pim_ifp->mroute_vif_index,
			  ifp->name, ifp->ifindex);
		return -1;
	}

	if (ifp->ifindex < 0) {
		zlog_warn("%s: ifindex=%d < 1 on interface %s",
			  __PRETTY_FUNCTION__, ifp->ifindex, ifp->name);
		return -2;
	}

	ifaddr = pim_ifp->primary_address;
	if (ifp->ifindex != PIM_OIF_PIM_REGISTER_VIF
	    && PIM_INADDR_IS_ANY(ifaddr)) {
		zlog_warn(
			"%s: could not get address for interface %s ifindex=%d",
			__PRETTY_FUNCTION__, ifp->name, ifp->ifindex);
		return -4;
	}

	pim_ifp->mroute_vif_index = pim_iface_next_vif_index(ifp);

	if (pim_ifp->mroute_vif_index >= MAXVIFS) {
		zlog_warn(
			"%s: Attempting to configure more than MAXVIFS=%d on pim enabled interface %s",
			__PRETTY_FUNCTION__, MAXVIFS, ifp->name);
		return -3;
	}

	if (ifp->ifindex == PIM_OIF_PIM_REGISTER_VIF)
		flags = VIFF_REGISTER;
#ifdef VIFF_USE_IFINDEX
	else
		flags = VIFF_USE_IFINDEX;
#endif

	if (pim_mroute_add_vif(ifp, ifaddr, flags)) {
		/* pim_mroute_add_vif reported error */
		return -5;
	}

	pim_iface_vif_index[pim_ifp->mroute_vif_index] = 1;
	return 0;
}

int pim_if_del_vif(struct interface *ifp)
{
	struct pim_interface *pim_ifp = ifp->info;

	if (pim_ifp->mroute_vif_index < 1) {
		zlog_warn("%s: vif_index=%d < 1 on interface %s ifindex=%d",
			  __PRETTY_FUNCTION__, pim_ifp->mroute_vif_index,
			  ifp->name, ifp->ifindex);
		return -1;
	}

	pim_mroute_del_vif(pim_ifp->mroute_vif_index);

	/*
	  Update vif_index
	 */
	pim_iface_vif_index[pim_ifp->mroute_vif_index] = 0;

	pim_ifp->mroute_vif_index = -1;

	return 0;
}

void pim_if_add_vif_all()
{
	struct listnode *ifnode;
	struct listnode *ifnextnode;
	struct interface *ifp;

	for (ALL_LIST_ELEMENTS(vrf_iflist(VRF_DEFAULT), ifnode, ifnextnode,
			       ifp)) {
		if (!ifp->info)
			continue;

		pim_if_add_vif(ifp);
	}
}

void pim_if_del_vif_all()
{
	struct listnode *ifnode;
	struct listnode *ifnextnode;
	struct interface *ifp;

	for (ALL_LIST_ELEMENTS(vrf_iflist(VRF_DEFAULT), ifnode, ifnextnode,
			       ifp)) {
		if (!ifp->info)
			continue;

		pim_if_del_vif(ifp);
	}
}

struct interface *pim_if_find_by_vif_index(ifindex_t vif_index)
{
	struct listnode *ifnode;
	struct interface *ifp;

	if (vif_index == 0)
		return if_lookup_by_name("pimreg", VRF_DEFAULT);

	for (ALL_LIST_ELEMENTS_RO(vrf_iflist(VRF_DEFAULT), ifnode, ifp)) {
		if (ifp->info) {
			struct pim_interface *pim_ifp;
			pim_ifp = ifp->info;

			if (vif_index == pim_ifp->mroute_vif_index)
				return ifp;
		}
	}

	return 0;
}

/*
  pim_if_add_vif() uses ifindex as vif_index
 */
int pim_if_find_vifindex_by_ifindex(ifindex_t ifindex)
{
	struct pim_interface *pim_ifp;
	struct interface *ifp;

	ifp = if_lookup_by_index(ifindex, VRF_DEFAULT);
	if (!ifp || !ifp->info)
		return -1;
	pim_ifp = ifp->info;

	return pim_ifp->mroute_vif_index;
}

int pim_if_lan_delay_enabled(struct interface *ifp)
{
	struct pim_interface *pim_ifp;

	pim_ifp = ifp->info;
	zassert(pim_ifp);
	zassert(pim_ifp->pim_number_of_nonlandelay_neighbors >= 0);

	return pim_ifp->pim_number_of_nonlandelay_neighbors == 0;
}

uint16_t pim_if_effective_propagation_delay_msec(struct interface *ifp)
{
	if (pim_if_lan_delay_enabled(ifp)) {
		struct pim_interface *pim_ifp;
		pim_ifp = ifp->info;
		return pim_ifp->pim_neighbors_highest_propagation_delay_msec;
	} else {
		return PIM_DEFAULT_PROPAGATION_DELAY_MSEC;
	}
}

uint16_t pim_if_effective_override_interval_msec(struct interface *ifp)
{
	if (pim_if_lan_delay_enabled(ifp)) {
		struct pim_interface *pim_ifp;
		pim_ifp = ifp->info;
		return pim_ifp->pim_neighbors_highest_override_interval_msec;
	} else {
		return PIM_DEFAULT_OVERRIDE_INTERVAL_MSEC;
	}
}

int pim_if_t_override_msec(struct interface *ifp)
{
	int effective_override_interval_msec;
	int t_override_msec;

	effective_override_interval_msec =
		pim_if_effective_override_interval_msec(ifp);

	t_override_msec = random() % (effective_override_interval_msec + 1);

	return t_override_msec;
}

uint16_t pim_if_jp_override_interval_msec(struct interface *ifp)
{
	return pim_if_effective_propagation_delay_msec(ifp)
	       + pim_if_effective_override_interval_msec(ifp);
}

/*
  RFC 4601: 4.1.6.  State Summarization Macros

  The function NBR( I, A ) uses information gathered through PIM Hello
  messages to map the IP address A of a directly connected PIM
  neighbor router on interface I to the primary IP address of the same
  router (Section 4.3.4).  The primary IP address of a neighbor is the
  address that it uses as the source of its PIM Hello messages.
*/
struct pim_neighbor *pim_if_find_neighbor(struct interface *ifp,
					  struct in_addr addr)
{
	struct listnode *neighnode;
	struct pim_neighbor *neigh;
	struct pim_interface *pim_ifp;

	zassert(ifp);

	pim_ifp = ifp->info;
	if (!pim_ifp) {
		zlog_warn("%s: multicast not enabled on interface %s",
			  __PRETTY_FUNCTION__, ifp->name);
		return 0;
	}

	for (ALL_LIST_ELEMENTS_RO(pim_ifp->pim_neighbor_list, neighnode,
				  neigh)) {

		/* primary address ? */
		if (neigh->source_addr.s_addr == addr.s_addr)
			return neigh;

		/* secondary address ? */
		if (pim_neighbor_find_secondary(neigh, addr))
			return neigh;
	}

	if (PIM_DEBUG_PIM_TRACE) {
		char addr_str[INET_ADDRSTRLEN];
		pim_inet4_dump("<addr?>", addr, addr_str, sizeof(addr_str));
		zlog_debug(
			"%s: neighbor not found for address %s on interface %s",
			__PRETTY_FUNCTION__, addr_str, ifp->name);
	}

	return NULL;
}

long pim_if_t_suppressed_msec(struct interface *ifp)
{
	struct pim_interface *pim_ifp;
	long t_suppressed_msec;
	uint32_t ramount = 0;

	pim_ifp = ifp->info;
	zassert(pim_ifp);

	/* join suppression disabled ? */
	if (PIM_IF_TEST_PIM_CAN_DISABLE_JOIN_SUPRESSION(pim_ifp->options))
		return 0;

	/* t_suppressed = t_periodic * rand(1.1, 1.4) */
	ramount = 1100 + (random() % (1400 - 1100 + 1));
	t_suppressed_msec = qpim_t_periodic * ramount;

	return t_suppressed_msec;
}

static void igmp_join_free(struct igmp_join *ij)
{
	XFREE(MTYPE_PIM_IGMP_JOIN, ij);
}

static struct igmp_join *igmp_join_find(struct list *join_list,
					struct in_addr group_addr,
					struct in_addr source_addr)
{
	struct listnode *node;
	struct igmp_join *ij;

	zassert(join_list);

	for (ALL_LIST_ELEMENTS_RO(join_list, node, ij)) {
		if ((group_addr.s_addr == ij->group_addr.s_addr)
		    && (source_addr.s_addr == ij->source_addr.s_addr))
			return ij;
	}

	return 0;
}

static int igmp_join_sock(const char *ifname, ifindex_t ifindex,
			  struct in_addr group_addr, struct in_addr source_addr)
{
	int join_fd;

	join_fd = pim_socket_raw(IPPROTO_IGMP);
	if (join_fd < 0) {
		return -1;
	}

	if (pim_socket_join_source(join_fd, ifindex, group_addr, source_addr,
				   ifname)) {
		close(join_fd);
		return -2;
	}

	return join_fd;
}

static struct igmp_join *igmp_join_new(struct interface *ifp,
				       struct in_addr group_addr,
				       struct in_addr source_addr)
{
	struct pim_interface *pim_ifp;
	struct igmp_join *ij;
	int join_fd;

	pim_ifp = ifp->info;
	zassert(pim_ifp);

	join_fd = igmp_join_sock(ifp->name, ifp->ifindex, group_addr,
				 source_addr);
	if (join_fd < 0) {
		char group_str[INET_ADDRSTRLEN];
		char source_str[INET_ADDRSTRLEN];
		pim_inet4_dump("<grp?>", group_addr, group_str,
			       sizeof(group_str));
		pim_inet4_dump("<src?>", source_addr, source_str,
			       sizeof(source_str));
		zlog_warn(
			"%s: igmp_join_sock() failure for IGMP group %s source %s on interface %s",
			__PRETTY_FUNCTION__, group_str, source_str, ifp->name);
		return 0;
	}

	ij = XCALLOC(MTYPE_PIM_IGMP_JOIN, sizeof(*ij));
	if (!ij) {
		char group_str[INET_ADDRSTRLEN];
		char source_str[INET_ADDRSTRLEN];
		pim_inet4_dump("<grp?>", group_addr, group_str,
			       sizeof(group_str));
		pim_inet4_dump("<src?>", source_addr, source_str,
			       sizeof(source_str));
		zlog_err(
			"%s: XCALLOC(%zu) failure for IGMP group %s source %s on interface %s",
			__PRETTY_FUNCTION__, sizeof(*ij), group_str, source_str,
			ifp->name);
		close(join_fd);
		return 0;
	}

	ij->sock_fd = join_fd;
	ij->group_addr = group_addr;
	ij->source_addr = source_addr;
	ij->sock_creation = pim_time_monotonic_sec();

	listnode_add(pim_ifp->igmp_join_list, ij);

	return ij;
}

int pim_if_igmp_join_add(struct interface *ifp, struct in_addr group_addr,
			 struct in_addr source_addr)
{
	struct pim_interface *pim_ifp;
	struct igmp_join *ij;

	pim_ifp = ifp->info;
	if (!pim_ifp) {
		zlog_warn("%s: multicast not enabled on interface %s",
			  __PRETTY_FUNCTION__, ifp->name);
		return -1;
	}

	if (!pim_ifp->igmp_join_list) {
		pim_ifp->igmp_join_list = list_new();
		if (!pim_ifp->igmp_join_list) {
			zlog_err("%s %s: failure: igmp_join_list=list_new()",
				 __FILE__, __PRETTY_FUNCTION__);
			return -2;
		}
		pim_ifp->igmp_join_list->del = (void (*)(void *))igmp_join_free;
	}

	ij = igmp_join_find(pim_ifp->igmp_join_list, group_addr, source_addr);
	if (ij) {
		char group_str[INET_ADDRSTRLEN];
		char source_str[INET_ADDRSTRLEN];
		pim_inet4_dump("<grp?>", group_addr, group_str,
			       sizeof(group_str));
		pim_inet4_dump("<src?>", source_addr, source_str,
			       sizeof(source_str));
		zlog_warn(
			"%s: can't re-join existing IGMP group %s source %s on interface %s",
			__PRETTY_FUNCTION__, group_str, source_str, ifp->name);
		return -3;
	}

	ij = igmp_join_new(ifp, group_addr, source_addr);
	if (!ij) {
		char group_str[INET_ADDRSTRLEN];
		char source_str[INET_ADDRSTRLEN];
		pim_inet4_dump("<grp?>", group_addr, group_str,
			       sizeof(group_str));
		pim_inet4_dump("<src?>", source_addr, source_str,
			       sizeof(source_str));
		zlog_warn(
			"%s: igmp_join_new() failure for IGMP group %s source %s on interface %s",
			__PRETTY_FUNCTION__, group_str, source_str, ifp->name);
		return -4;
	}

	if (PIM_DEBUG_IGMP_EVENTS) {
		char group_str[INET_ADDRSTRLEN];
		char source_str[INET_ADDRSTRLEN];
		pim_inet4_dump("<grp?>", group_addr, group_str,
			       sizeof(group_str));
		pim_inet4_dump("<src?>", source_addr, source_str,
			       sizeof(source_str));
		zlog_debug(
			"%s: issued static igmp join for channel (S,G)=(%s,%s) on interface %s",
			__PRETTY_FUNCTION__, source_str, group_str, ifp->name);
	}

	return 0;
}


int pim_if_igmp_join_del(struct interface *ifp, struct in_addr group_addr,
			 struct in_addr source_addr)
{
	struct pim_interface *pim_ifp;
	struct igmp_join *ij;

	pim_ifp = ifp->info;
	if (!pim_ifp) {
		zlog_warn("%s: multicast not enabled on interface %s",
			  __PRETTY_FUNCTION__, ifp->name);
		return -1;
	}

	if (!pim_ifp->igmp_join_list) {
		zlog_warn("%s: no IGMP join on interface %s",
			  __PRETTY_FUNCTION__, ifp->name);
		return -2;
	}

	ij = igmp_join_find(pim_ifp->igmp_join_list, group_addr, source_addr);
	if (!ij) {
		char group_str[INET_ADDRSTRLEN];
		char source_str[INET_ADDRSTRLEN];
		pim_inet4_dump("<grp?>", group_addr, group_str,
			       sizeof(group_str));
		pim_inet4_dump("<src?>", source_addr, source_str,
			       sizeof(source_str));
		zlog_warn(
			"%s: could not find IGMP group %s source %s on interface %s",
			__PRETTY_FUNCTION__, group_str, source_str, ifp->name);
		return -3;
	}

	if (close(ij->sock_fd)) {
		char group_str[INET_ADDRSTRLEN];
		char source_str[INET_ADDRSTRLEN];
		pim_inet4_dump("<grp?>", group_addr, group_str,
			       sizeof(group_str));
		pim_inet4_dump("<src?>", source_addr, source_str,
			       sizeof(source_str));
		zlog_warn(
			"%s: failure closing sock_fd=%d for IGMP group %s source %s on interface %s: errno=%d: %s",
			__PRETTY_FUNCTION__, ij->sock_fd, group_str, source_str,
			ifp->name, errno, safe_strerror(errno));
		/* warning only */
	}
	listnode_delete(pim_ifp->igmp_join_list, ij);
	igmp_join_free(ij);
	if (listcount(pim_ifp->igmp_join_list) < 1) {
		list_delete(pim_ifp->igmp_join_list);
		pim_ifp->igmp_join_list = 0;
	}

	return 0;
}

static void pim_if_igmp_join_del_all(struct interface *ifp)
{
	struct pim_interface *pim_ifp;
	struct listnode *node;
	struct listnode *nextnode;
	struct igmp_join *ij;

	pim_ifp = ifp->info;
	if (!pim_ifp) {
		zlog_warn("%s: multicast not enabled on interface %s",
			  __PRETTY_FUNCTION__, ifp->name);
		return;
	}

	if (!pim_ifp->igmp_join_list)
		return;

	for (ALL_LIST_ELEMENTS(pim_ifp->igmp_join_list, node, nextnode, ij))
		pim_if_igmp_join_del(ifp, ij->group_addr, ij->source_addr);
}

/*
  RFC 4601

  Transitions from "I am Assert Loser" State

  Current Winner's GenID Changes or NLT Expires

  The Neighbor Liveness Timer associated with the current winner
  expires or we receive a Hello message from the current winner
  reporting a different GenID from the one it previously reported.
  This indicates that the current winner's interface or router has
  gone down (and may have come back up), and so we must assume it no
  longer knows it was the winner.
 */
void pim_if_assert_on_neighbor_down(struct interface *ifp,
				    struct in_addr neigh_addr)
{
	struct pim_interface *pim_ifp;
	struct listnode *node;
	struct listnode *next_node;
	struct pim_ifchannel *ch;

	pim_ifp = ifp->info;
	zassert(pim_ifp);

	for (ALL_LIST_ELEMENTS(pim_ifp->pim_ifchannel_list, node, next_node,
			       ch)) {
		/* Is (S,G,I) assert loser ? */
		if (ch->ifassert_state != PIM_IFASSERT_I_AM_LOSER)
			continue;
		/* Dead neighbor was winner ? */
		if (ch->ifassert_winner.s_addr != neigh_addr.s_addr)
			continue;

		assert_action_a5(ch);
	}
}

void pim_if_update_join_desired(struct pim_interface *pim_ifp)
{
	struct listnode *ch_node;
	struct pim_ifchannel *ch;

	/* clear off flag from interface's upstreams */
	for (ALL_LIST_ELEMENTS_RO(pim_ifp->pim_ifchannel_list, ch_node, ch)) {
		PIM_UPSTREAM_FLAG_UNSET_DR_JOIN_DESIRED_UPDATED(
			ch->upstream->flags);
	}

	/* scan per-interface (S,G,I) state on this I interface */
	for (ALL_LIST_ELEMENTS_RO(pim_ifp->pim_ifchannel_list, ch_node, ch)) {
		struct pim_upstream *up = ch->upstream;

		if (PIM_UPSTREAM_FLAG_TEST_DR_JOIN_DESIRED_UPDATED(up->flags))
			continue;

		/* update join_desired for the global (S,G) state */
		pim_upstream_update_join_desired(up);
		PIM_UPSTREAM_FLAG_SET_DR_JOIN_DESIRED_UPDATED(up->flags);
	}
}

void pim_if_update_assert_tracking_desired(struct interface *ifp)
{
	struct pim_interface *pim_ifp;
	struct listnode *node;
	struct listnode *next_node;
	struct pim_ifchannel *ch;

	pim_ifp = ifp->info;
	if (!pim_ifp)
		return;

	for (ALL_LIST_ELEMENTS(pim_ifp->pim_ifchannel_list, node, next_node,
			       ch)) {
		pim_ifchannel_update_assert_tracking_desired(ch);
	}
}

/*
 * PIM wants to have an interface pointer for everything it does.
 * The pimreg is a special interface that we have that is not
 * quite an inteface but a VIF is created for it.
 */
void pim_if_create_pimreg(void)
{
	if (!pim_regiface) {
		pim_regiface =
			if_create("pimreg", strlen("pimreg"), VRF_DEFAULT);
		pim_regiface->ifindex = PIM_OIF_PIM_REGISTER_VIF;

		pim_if_new(pim_regiface, 0, 0);
	}
}

int pim_if_connected_to_source(struct interface *ifp, struct in_addr src)
{
	struct listnode *cnode;
	struct connected *c;
	struct prefix p;

	if (!ifp)
		return 0;

	p.family = AF_INET;
	p.u.prefix4 = src;
	p.prefixlen = IPV4_MAX_BITLEN;

	for (ALL_LIST_ELEMENTS_RO(ifp->connected, cnode, c)) {
		if ((c->address->family == AF_INET)
		    && prefix_match(CONNECTED_PREFIX(c), &p)) {
			return 1;
		}
	}

	return 0;
}

struct interface *pim_if_lookup_address_vrf(struct in_addr src, vrf_id_t vrf_id)
{
	struct listnode *ifnode;
	struct interface *ifp;

	for (ALL_LIST_ELEMENTS_RO(vrf_iflist(vrf_id), ifnode, ifp)) {
		if (pim_if_connected_to_source(ifp, src) && ifp->info)
			return ifp;
	}
	return NULL;
}
