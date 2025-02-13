/*
 * OSPF Interface functions.
 * Copyright (C) 1999 Toshiaki Takada
 *
 * This file is part of GNU Zebra.
 *
 * GNU Zebra is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published
 * by the Free Software Foundation; either version 2, or (at your
 * option) any later version.
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

#ifndef _ZEBRA_OSPF_INTERFACE_H
#define _ZEBRA_OSPF_INTERFACE_H

#include "qobj.h"
#include "hook.h"
#include "ospfd/ospf_packet.h"
#include "ospfd/ospf_spf.h"

#define IF_OSPF_IF_INFO(I) ((struct ospf_if_info *)((I)->info))
#define IF_DEF_PARAMS(I) (IF_OSPF_IF_INFO (I)->def_params)
#define IF_OIFS(I)  (IF_OSPF_IF_INFO (I)->oifs)
#define IF_OIFS_PARAMS(I) (IF_OSPF_IF_INFO (I)->params)
#define OSPF_IF_PARAM_CONFIGURED(S, P) ((S) && (S)->P##__config)
#define OSPF_IF_PARAM(O, P)                                                    \
	(OSPF_IF_PARAM_CONFIGURED((O)->params, P)                              \
		 ? (O)->params->P                                              \
		 : IF_DEF_PARAMS((O)->ifp)->P)

#define DECLARE_IF_PARAM(T, P) T P; u_char P##__config:1
#define UNSET_IF_PARAM(S, P) ((S)->P##__config) = 0
#define SET_IF_PARAM(S, P) ((S)->P##__config) = 1

struct ospf_if_params {
	DECLARE_IF_PARAM(u_int32_t,
			 transmit_delay); /* Interface Transmisson Delay */
	DECLARE_IF_PARAM(u_int32_t,
			 output_cost_cmd); /* Command Interface Output Cost */
	DECLARE_IF_PARAM(u_int32_t,
			 retransmit_interval); /* Retransmission Interval */
	DECLARE_IF_PARAM(u_char, passive_interface); /* OSPF Interface is
							passive: no sending or
							receiving (no need to
							join multicast groups)
							*/
	DECLARE_IF_PARAM(u_char, priority); /* OSPF Interface priority */
	/* Enable OSPF on this interface with area if_area */
	DECLARE_IF_PARAM(struct in_addr, if_area);
	DECLARE_IF_PARAM(u_char, type); /* type of interface */
#define OSPF_IF_ACTIVE                  0
#define OSPF_IF_PASSIVE		        1

#define OSPF_IF_PASSIVE_STATUS(O)                                              \
	(OSPF_IF_PARAM_CONFIGURED((O)->params, passive_interface)              \
		 ? (O)->params->passive_interface                              \
		 : (OSPF_IF_PARAM_CONFIGURED(IF_DEF_PARAMS((O)->ifp),          \
					     passive_interface)                \
			    ? IF_DEF_PARAMS((O)->ifp)->passive_interface       \
			    : (O)->ospf->passive_interface_default))

	DECLARE_IF_PARAM(u_int32_t, v_hello); /* Hello Interval */
	DECLARE_IF_PARAM(u_int32_t, v_wait);  /* Router Dead Interval */

	/* MTU mismatch check (see RFC2328, chap 10.6) */
	DECLARE_IF_PARAM(u_char, mtu_ignore);

	/* Fast-Hellos */
	DECLARE_IF_PARAM(u_char, fast_hello);

	/* Authentication data. */
	u_char auth_simple[OSPF_AUTH_SIMPLE_SIZE + 1]; /* Simple password. */
	u_char auth_simple__config : 1;

	DECLARE_IF_PARAM(struct list *,
			 auth_crypt);     /* List of Auth cryptographic data. */
	DECLARE_IF_PARAM(int, auth_type); /* OSPF authentication type */

	/* Other, non-configuration state */
	u_int32_t network_lsa_seqnum; /* Network LSA seqnum */

	/* BFD configuration */
	struct bfd_info *bfd_info;
};

enum { MEMBER_ALLROUTERS = 0,
       MEMBER_DROUTERS,
       MEMBER_MAX,
};

struct ospf_if_info {
	struct ospf_if_params *def_params;
	struct route_table *params;
	struct route_table *oifs;
	unsigned int
		membership_counts[MEMBER_MAX]; /* multicast group refcnts */
};

struct ospf_interface;

struct ospf_vl_data {
	struct in_addr vl_peer;	/* Router-ID of the peer */
	struct in_addr vl_area_id;     /* Transit area */
	int vl_area_id_fmt;	    /* Area ID format */
	struct ospf_interface *vl_oi;  /* Interface data structure */
	struct vertex_nexthop nexthop; /* Nexthop router and oi to use */
	struct in_addr peer_addr;      /* Address used to reach the peer */
	u_char flags;
};


#define OSPF_VL_MAX_COUNT 256
#define OSPF_VL_MTU	  1500

#define OSPF_VL_FLAG_APPROVED 0x01

struct crypt_key {
	u_char key_id;
	u_char auth_key[OSPF_AUTH_MD5_SIZE + 1];
};

/* OSPF interface structure. */
struct ospf_interface {
	/* This interface's parent ospf instance. */
	struct ospf *ospf;

	/* OSPF Area. */
	struct ospf_area *area;

	/* Position range in Router LSA */
	uint16_t lsa_pos_beg; /* inclusive, >= */
	uint16_t lsa_pos_end; /* exclusive, <  */

	/* Interface data from zebra. */
	struct interface *ifp;
	struct ospf_vl_data *vl_data; /* Data for Virtual Link */

	/* Packet send buffer. */
	struct ospf_fifo *obuf; /* Output queue */

	/* OSPF Network Type. */
	u_char type;

	/* State of Interface State Machine. */
	u_char state;

	/* To which multicast groups do we currently belong? */
	u_char multicast_memberships;
#define OI_MEMBER_FLAG(M) (1 << (M))
#define OI_MEMBER_COUNT(O,M) (IF_OSPF_IF_INFO(oi->ifp)->membership_counts[(M)])
#define OI_MEMBER_CHECK(O, M)                                                  \
	(CHECK_FLAG((O)->multicast_memberships, OI_MEMBER_FLAG(M)))
#define OI_MEMBER_JOINED(O, M)                                                 \
	do {                                                                   \
		SET_FLAG((O)->multicast_memberships, OI_MEMBER_FLAG(M));       \
		IF_OSPF_IF_INFO((O)->ifp)->membership_counts[(M)]++;           \
	} while (0)
#define OI_MEMBER_LEFT(O, M)                                                   \
	do {                                                                   \
		UNSET_FLAG((O)->multicast_memberships, OI_MEMBER_FLAG(M));     \
		IF_OSPF_IF_INFO((O)->ifp)->membership_counts[(M)]--;           \
	} while (0)

	struct prefix *address;      /* Interface prefix */
	struct connected *connected; /* Pointer to connected */

	/* Configured varables. */
	struct ospf_if_params *params;

	u_int32_t crypt_seqnum; /* Cryptographic Sequence Number */
	u_int32_t output_cost;  /* Acutual Interface Output Cost */

	/* Neighbor information. */
	struct route_table *nbrs;       /* OSPF Neighbor List */
	struct ospf_neighbor *nbr_self; /* Neighbor Self */
					/* $FRR indent$ */
					/* clang-format off */
#define DR(I)			((I)->nbr_self->d_router)
#define BDR(I)			((I)->nbr_self->bd_router)
#define OPTIONS(I)		((I)->nbr_self->options)
#define PRIORITY(I)		((I)->nbr_self->priority)

	/* List of configured NBMA neighbor. */
	struct list *nbr_nbma;

	/* self-originated LSAs. */
	struct ospf_lsa *network_lsa_self; /* network-LSA. */
	struct list *opaque_lsa_self;      /* Type-9 Opaque-LSAs */

	struct route_table *ls_upd_queue;

	struct list *ls_ack; /* Link State Acknowledgment list. */

	struct {
		struct list *ls_ack;
		struct in_addr dst;
	} ls_ack_direct;

	/* Timer values. */
	u_int32_t v_ls_ack; /* Delayed Link State Acknowledgment */

	/* Threads. */
	struct thread *t_hello;		  /* timer */
	struct thread *t_wait;		  /* timer */
	struct thread *t_ls_ack;	  /* timer */
	struct thread *t_ls_ack_direct;   /* event */
	struct thread *t_ls_upd_event;    /* event */
	struct thread *t_opaque_lsa_self; /* Type-9 Opaque-LSAs */

	int on_write_q;

	/* Statistics fields. */
	u_int32_t hello_in;     /* Hello message input count. */
	u_int32_t hello_out;    /* Hello message output count. */
	u_int32_t db_desc_in;   /* database desc. message input count. */
	u_int32_t db_desc_out;  /* database desc. message output count. */
	u_int32_t ls_req_in;    /* LS request message input count. */
	u_int32_t ls_req_out;   /* LS request message output count. */
	u_int32_t ls_upd_in;    /* LS update message input count. */
	u_int32_t ls_upd_out;   /* LS update message output count. */
	u_int32_t ls_ack_in;    /* LS Ack message input count. */
	u_int32_t ls_ack_out;   /* LS Ack message output count. */
	u_int32_t discarded;    /* discarded input count by error. */
	u_int32_t state_change; /* Number of status change. */

	u_int32_t full_nbrs;

/* modified by liwh for bug 44068, 2017-08-08 */
#ifdef _CENTEC_
  struct prefix network_p;
#endif
/*liwh end */
	QOBJ_FIELDS
};
DECLARE_QOBJ_TYPE(ospf_interface)

/* Prototypes. */
extern char *ospf_if_name(struct ospf_interface *);
extern struct ospf_interface *ospf_if_new(struct ospf *, struct interface *,
					  struct prefix *);
extern void ospf_if_cleanup(struct ospf_interface *);
extern void ospf_if_free(struct ospf_interface *);
extern int ospf_if_up(struct ospf_interface *);
extern int ospf_if_down(struct ospf_interface *);

extern int ospf_if_is_up(struct ospf_interface *);
extern struct ospf_interface *ospf_if_exists(struct ospf_interface *);
extern struct ospf_interface *ospf_if_lookup_by_lsa_pos(struct ospf_area *,
							int);
/* modified by liwh for bug 45943, 2017-12-21 */
#ifdef _CENTEC_
struct interface *
ospf_zebra_if_lookup_by_local_addr (struct in_addr address);
#endif
/*liwh end */

extern struct ospf_interface *
ospf_if_lookup_by_local_addr(struct ospf *, struct interface *, struct in_addr);
extern struct ospf_interface *ospf_if_lookup_by_prefix(struct ospf *,
						       struct prefix_ipv4 *);
extern struct ospf_interface *ospf_if_table_lookup(struct interface *,
						   struct prefix *);
extern struct ospf_interface *ospf_if_addr_local(struct in_addr);
extern struct ospf_interface *
ospf_if_lookup_recv_if(struct ospf *, struct in_addr, struct interface *);
extern struct ospf_interface *ospf_if_is_configured(struct ospf *,
						    struct in_addr *);

extern struct ospf_if_params *ospf_lookup_if_params(struct interface *,
						    struct in_addr);
extern struct ospf_if_params *ospf_get_if_params(struct interface *,
						 struct in_addr);
extern void ospf_del_if_params(struct ospf_if_params *);
extern void ospf_free_if_params(struct interface *, struct in_addr);
extern void ospf_if_update_params(struct interface *, struct in_addr);

extern int ospf_if_new_hook(struct interface *);
extern void ospf_if_init(void);
extern void ospf_if_stream_set(struct ospf_interface *);
extern void ospf_if_stream_unset(struct ospf_interface *);
extern void ospf_if_reset_variables(struct ospf_interface *);
extern int ospf_if_is_enable(struct ospf_interface *);
extern int ospf_if_get_output_cost(struct ospf_interface *);
extern void ospf_if_recalculate_output_cost(struct interface *);

/* Simulate down/up on the interface. */
extern void ospf_if_reset(struct interface *);

extern struct ospf_interface *ospf_vl_new(struct ospf *, struct ospf_vl_data *);
extern struct ospf_vl_data *ospf_vl_data_new(struct ospf_area *,
					     struct in_addr);
extern struct ospf_vl_data *ospf_vl_lookup(struct ospf *, struct ospf_area *,
					   struct in_addr);
extern void ospf_vl_data_free(struct ospf_vl_data *);
extern void ospf_vl_add(struct ospf *, struct ospf_vl_data *);
extern void ospf_vl_delete(struct ospf *, struct ospf_vl_data *);
extern void ospf_vl_up_check(struct ospf_area *, struct in_addr,
			     struct vertex *);
extern void ospf_vl_unapprove(struct ospf *);
extern void ospf_vl_shut_unapproved(struct ospf *);
extern int ospf_full_virtual_nbrs(struct ospf_area *);
extern int ospf_vls_in_area(struct ospf_area *);

extern struct crypt_key *ospf_crypt_key_lookup(struct list *, u_char);
extern struct crypt_key *ospf_crypt_key_new(void);
extern void ospf_crypt_key_add(struct list *, struct crypt_key *);
extern int ospf_crypt_key_delete(struct list *, u_char);

extern u_char ospf_default_iftype(struct interface *ifp);

/* Set all multicast memberships appropriately based on the type and
   state of the interface. */
extern void ospf_if_set_multicast(struct ospf_interface *);

DECLARE_HOOK(ospf_vl_add, (struct ospf_vl_data * vd), (vd))
DECLARE_HOOK(ospf_vl_delete, (struct ospf_vl_data * vd), (vd))

#endif /* _ZEBRA_OSPF_INTERFACE_H */
