/**
 * bfd.h: BFD definitions and structures
 *
 * @copyright Copyright (C) 2015 Cumulus Networks, Inc.
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

#ifndef _ZEBRA_BFD_H
#define _ZEBRA_BFD_H

#include "lib/json.h"

#define BFD_DEF_MIN_RX 300
#define BFD_MIN_MIN_RX 50
#define BFD_MAX_MIN_RX 60000
#define BFD_DEF_MIN_TX 300
#define BFD_MIN_MIN_TX 50
#define BFD_MAX_MIN_TX 60000
#define BFD_DEF_DETECT_MULT 3
#define BFD_MIN_DETECT_MULT 2
#define BFD_MAX_DETECT_MULT 255

#define BFD_GBL_FLAG_IN_SHUTDOWN (1 << 0) /* The daemon in shutdown */
struct bfd_gbl {
	u_int16_t flags;
};

#define BFD_FLAG_PARAM_CFG (1 << 0) /* parameters have been configured */
#define BFD_FLAG_BFD_REG   (1 << 1) /* Peer registered with BFD */
#define BFD_FLAG_BFD_TYPE_MULTIHOP (1 << 2) /* Peer registered with BFD as multihop */

#define BFD_STATUS_UNKNOWN (1 << 0) /* BFD session status never received */
#define BFD_STATUS_DOWN    (1 << 1) /* BFD session status is down */
#define BFD_STATUS_UP      (1 << 2) /* BFD session status is up */

enum bfd_sess_type {
	BFD_TYPE_NOT_CONFIGURED,
	BFD_TYPE_SINGLEHOP,
	BFD_TYPE_MULTIHOP
};

struct bfd_info {
	u_int16_t flags;
	u_int8_t detect_mult;
	u_int32_t desired_min_tx;
	u_int32_t required_min_rx;
	time_t last_update;
	u_int8_t status;
	enum bfd_sess_type type;
};

extern struct bfd_info *bfd_info_create(void);

extern void bfd_info_free(struct bfd_info **bfd_info);

extern int bfd_validate_param(struct vty *vty, const char *dm_str,
			      const char *rx_str, const char *tx_str,
			      u_int8_t *dm_val, u_int32_t *rx_val,
			      u_int32_t *tx_val);

extern void bfd_set_param(struct bfd_info **bfd_info, u_int32_t min_rx,
			  u_int32_t min_tx, u_int8_t detect_mult, int defaults,
			  int *command);
extern void bfd_peer_sendmsg(struct zclient *zclient, struct bfd_info *bfd_info,
			     int family, void *dst_ip, void *src_ip,
			     char *if_name, int ttl, int multihop, int command,
			     int set_flag, vrf_id_t vrf_id);

extern const char *bfd_get_command_dbg_str(int command);

extern struct interface *bfd_get_peer_info(struct stream *s, struct prefix *dp,
					   struct prefix *sp, int *status,
					   vrf_id_t vrf_id);

const char *bfd_get_status_str(int status);

extern void bfd_show_param(struct vty *vty, struct bfd_info *bfd_info,
			   int bfd_tag, int extra_space, u_char use_json,
			   json_object *json_obj);

extern void bfd_show_info(struct vty *vty, struct bfd_info *bfd_info,
			  int multihop, int extra_space, u_char use_json,
			  json_object *json_obj);

extern void bfd_client_sendmsg(struct zclient *zclient, int command);

extern void bfd_gbl_init(void);

extern void bfd_gbl_exit(void);

#endif /* _ZEBRA_BFD_H */
