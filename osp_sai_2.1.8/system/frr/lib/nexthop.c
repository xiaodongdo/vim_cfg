/* A generic nexthop structure
 * Copyright (C) 2013 Cumulus Networks, Inc.
 *
 * This file is part of Quagga.
 *
 * Quagga is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation; either version 2, or (at your option) any
 * later version.
 *
 * Quagga is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Quagga; see the file COPYING.  If not, write to the Free
 * Software Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA
 * 02111-1307, USA.
 */
#include <zebra.h>

#include "prefix.h"
#include "table.h"
#include "memory.h"
#include "command.h"
#include "if.h"
#include "log.h"
#include "sockunion.h"
#include "linklist.h"
#include "thread.h"
#include "prefix.h"
#include "nexthop.h"
#include "mpls.h"

DEFINE_MTYPE_STATIC(LIB, NEXTHOP, "Nexthop")
DEFINE_MTYPE_STATIC(LIB, NH_LABEL, "Nexthop label")

/* check if nexthops are same, non-recursive */
int nexthop_same_no_recurse(struct nexthop *next1, struct nexthop *next2)
{
	if (next1->type != next2->type)
		return 0;

	switch (next1->type) {
	case NEXTHOP_TYPE_IPV4:
	case NEXTHOP_TYPE_IPV4_IFINDEX:
		if (!IPV4_ADDR_SAME(&next1->gate.ipv4, &next2->gate.ipv4))
			return 0;
		if (next1->ifindex && (next1->ifindex != next2->ifindex))
			return 0;
		break;
	case NEXTHOP_TYPE_IFINDEX:
		if (next1->ifindex != next2->ifindex)
			return 0;
		break;
	case NEXTHOP_TYPE_IPV6:
		if (!IPV6_ADDR_SAME(&next1->gate.ipv6, &next2->gate.ipv6))
			return 0;
		break;
	case NEXTHOP_TYPE_IPV6_IFINDEX:
		if (!IPV6_ADDR_SAME(&next1->gate.ipv6, &next2->gate.ipv6))
			return 0;
		if (next1->ifindex != next2->ifindex)
			return 0;
		break;
	default:
		/* do nothing */
		break;
	}
	return 1;
}

/*
 * nexthop_type_to_str
 */
const char *nexthop_type_to_str(enum nexthop_types_t nh_type)
{
	static const char *desc[] = {
		"none",		 "Directly connected",
		"IPv4 nexthop",  "IPv4 nexthop with ifindex",
		"IPv6 nexthop",  "IPv6 nexthop with ifindex",
		"Null0 nexthop",
	};

	return desc[nh_type];
}

struct nexthop *nexthop_new(void)
{
	return XCALLOC(MTYPE_NEXTHOP, sizeof(struct nexthop));
}

/* Add nexthop to the end of a nexthop list.  */
void nexthop_add(struct nexthop **target, struct nexthop *nexthop)
{
	struct nexthop *last;

	for (last = *target; last && last->next; last = last->next)
		;
	if (last)
		last->next = nexthop;
	else
		*target = nexthop;
	nexthop->prev = last;
}

void copy_nexthops(struct nexthop **tnh, struct nexthop *nh)
{
	struct nexthop *nexthop;
	struct nexthop *nh1;

	for (nh1 = nh; nh1; nh1 = nh1->next) {
		nexthop = nexthop_new();
		nexthop->flags = nh->flags;
		nexthop->type = nh->type;
		nexthop->ifindex = nh->ifindex;
		memcpy(&(nexthop->gate), &(nh->gate), sizeof(union g_addr));
		memcpy(&(nexthop->src), &(nh->src), sizeof(union g_addr));
		if (nh->nh_label)
			nexthop_add_labels(nexthop, nh->nh_label_type,
					   nh->nh_label->num_labels,
					   &nh->nh_label->label[0]);
		nexthop_add(tnh, nexthop);

		if (CHECK_FLAG(nh1->flags, NEXTHOP_FLAG_RECURSIVE))
			copy_nexthops(&nexthop->resolved, nh1->resolved);
	}
}

/* Free nexthop. */
void nexthop_free(struct nexthop *nexthop)
{
	nexthop_del_labels(nexthop);
	if (nexthop->resolved)
		nexthops_free(nexthop->resolved);
	XFREE(MTYPE_NEXTHOP, nexthop);
}

/* Frees a list of nexthops */
void nexthops_free(struct nexthop *nexthop)
{
	struct nexthop *nh, *next;

	for (nh = nexthop; nh; nh = next) {
		next = nh->next;
		nexthop_free(nh);
	}
}

/* Update nexthop with label information. */
void nexthop_add_labels(struct nexthop *nexthop, enum lsp_types_t type,
			u_int8_t num_labels, mpls_label_t *label)
{
	struct nexthop_label *nh_label;
	int i;

	nexthop->nh_label_type = type;
	nh_label = XCALLOC(MTYPE_NH_LABEL,
			   sizeof(struct nexthop_label)
				   + num_labels * sizeof(mpls_label_t));
	nh_label->num_labels = num_labels;
	for (i = 0; i < num_labels; i++)
		nh_label->label[i] = *(label + i);
	nexthop->nh_label = nh_label;
}

/* Free label information of nexthop, if present. */
void nexthop_del_labels(struct nexthop *nexthop)
{
	if (nexthop->nh_label) {
		XFREE(MTYPE_NH_LABEL, nexthop->nh_label);
		nexthop->nh_label_type = ZEBRA_LSP_NONE;
	}
}

const char *nexthop2str(struct nexthop *nexthop, char *str, int size)
{
	switch (nexthop->type) {
	case NEXTHOP_TYPE_IFINDEX:
		snprintf(str, size, "if %u", nexthop->ifindex);
		break;
	case NEXTHOP_TYPE_IPV4:
		snprintf(str, size, "%s", inet_ntoa(nexthop->gate.ipv4));
		break;
	case NEXTHOP_TYPE_IPV4_IFINDEX:
		snprintf(str, size, "%s if %u", inet_ntoa(nexthop->gate.ipv4),
			 nexthop->ifindex);
		break;
	case NEXTHOP_TYPE_IPV6:
		snprintf(str, size, "%s", inet6_ntoa(nexthop->gate.ipv6));
		break;
	case NEXTHOP_TYPE_IPV6_IFINDEX:
		snprintf(str, size, "%s if %u", inet6_ntoa(nexthop->gate.ipv6),
			 nexthop->ifindex);
		break;
	case NEXTHOP_TYPE_BLACKHOLE:
		snprintf(str, size, "blackhole");
		break;
	default:
		snprintf(str, size, "unknown");
		break;
	}

	return str;
}
