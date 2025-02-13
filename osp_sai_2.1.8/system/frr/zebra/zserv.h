/* Zebra daemon server header.
 * Copyright (C) 1997, 98 Kunihiro Ishiguro
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

#ifndef _ZEBRA_ZSERV_H
#define _ZEBRA_ZSERV_H

#include "rib.h"
#include "if.h"
#include "workqueue.h"
#include "vrf.h"
#include "routemap.h"
#include "vty.h"
#include "zclient.h"

#include "zebra/zebra_ns.h"
#include "zebra/zebra_pw.h"

/* modified by liwh for bug 52992, 2019-07-20 */
/* Default port information. */
//#define ZEBRA_VTY_PORT                2601
#define ZEBRA_VTY_PORT                0
/* liwh end */

/* Default configuration filename. */
#define DEFAULT_CONFIG_FILE "zebra.conf"

#define ZEBRA_RMAP_DEFAULT_UPDATE_TIMER 5 /* disabled by default */

/* Client structure. */
struct zserv {
	/* Client file descriptor. */
	int sock;

	/* Input/output buffer to the client. */
	struct stream *ibuf;
	struct stream *obuf;

	/* Buffer of data waiting to be written to client. */
	struct buffer *wb;

	/* Threads for read/write. */
	struct thread *t_read;
	struct thread *t_write;

	/* Thread for delayed close. */
	struct thread *t_suicide;

	/* default routing table this client munges */
	int rtm_table;

	/* This client's redistribute flag. */
	struct redist_proto mi_redist[AFI_MAX][ZEBRA_ROUTE_MAX];
	vrf_bitmap_t redist[AFI_MAX][ZEBRA_ROUTE_MAX];

	/* Redistribute default route flag. */
	vrf_bitmap_t redist_default;

	/* Interface information. */
	vrf_bitmap_t ifinfo;

	/* Router-id information. */
	vrf_bitmap_t ridinfo;

	/* client's protocol */
	u_char proto;
	u_short instance;
	u_char is_synchronous;

	/* Statistics */
	u_int32_t redist_v4_add_cnt;
	u_int32_t redist_v4_del_cnt;
	u_int32_t redist_v6_add_cnt;
	u_int32_t redist_v6_del_cnt;
	u_int32_t v4_route_add_cnt;
	u_int32_t v4_route_upd8_cnt;
	u_int32_t v4_route_del_cnt;
	u_int32_t v6_route_add_cnt;
	u_int32_t v6_route_del_cnt;
	u_int32_t v6_route_upd8_cnt;
	u_int32_t connected_rt_add_cnt;
	u_int32_t connected_rt_del_cnt;
	u_int32_t ifup_cnt;
	u_int32_t ifdown_cnt;
	u_int32_t ifadd_cnt;
	u_int32_t ifdel_cnt;
	u_int32_t if_bfd_cnt;
	u_int32_t bfd_peer_add_cnt;
	u_int32_t bfd_peer_upd8_cnt;
	u_int32_t bfd_peer_del_cnt;
	u_int32_t bfd_peer_replay_cnt;
	u_int32_t vrfadd_cnt;
	u_int32_t vrfdel_cnt;
	u_int32_t if_vrfchg_cnt;
	u_int32_t bfd_client_reg_cnt;

	time_t connect_time;
	time_t last_read_time;
	time_t last_write_time;
	time_t nh_reg_time;
	time_t nh_dereg_time;
	time_t nh_last_upd_time;

	int last_read_cmd;
	int last_write_cmd;
};

/* Zebra instance */
struct zebra_t {
	/* Thread master */
	struct thread_master *master;
	struct list *client_list;

	/* default table */
	u_int32_t rtm_table_default;

	/* rib work queue */
	struct work_queue *ribq;
	struct meta_queue *mq;

	/* LSP work queue */
	struct work_queue *lsp_process_q;
};
extern struct zebra_t zebrad;
extern unsigned int multipath_num;

/* Prototypes. */
extern void zebra_init(void);
extern void zebra_if_init(void);
extern void zebra_zserv_socket_init(char *path);
extern void hostinfo_get(void);
extern void rib_init(void);
extern void interface_list(struct zebra_ns *);
extern void route_read(struct zebra_ns *);
extern void kernel_init(struct zebra_ns *);
extern void kernel_terminate(struct zebra_ns *);
extern void zebra_route_map_init(void);
extern void zebra_vty_init(void);

extern int zsend_vrf_add(struct zserv *, struct zebra_vrf *);
extern int zsend_vrf_delete(struct zserv *, struct zebra_vrf *);

extern int zsend_interface_add(struct zserv *, struct interface *);
extern int zsend_interface_delete(struct zserv *, struct interface *);
extern int zsend_interface_addresses(struct zserv *, struct interface *);
extern int zsend_interface_address(int, struct zserv *, struct interface *,
				   struct connected *);
extern void nbr_connected_add_ipv6(struct interface *, struct in6_addr *);
extern void nbr_connected_delete_ipv6(struct interface *, struct in6_addr *);
extern int zsend_interface_update(int, struct zserv *, struct interface *);
extern int zsend_redistribute_route(int, struct zserv *, struct prefix *,
				    struct prefix *, struct rib *);
extern int zsend_router_id_update(struct zserv *, struct prefix *, vrf_id_t);
extern int zsend_interface_vrf_update(struct zserv *, struct interface *,
				      vrf_id_t);

extern int zsend_interface_link_params(struct zserv *, struct interface *);
extern int zsend_pw_update(struct zserv *, struct zebra_pw *);

extern pid_t pid;

extern void zserv_create_header(struct stream *s, uint16_t cmd,
				vrf_id_t vrf_id);
extern void zserv_nexthop_num_warn(const char *, const struct prefix *,
				   const unsigned int);
extern int zebra_server_send_message(struct zserv *client);

extern struct zserv *zebra_find_client(u_char proto);

#endif /* _ZEBRA_ZEBRA_H */
