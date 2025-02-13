/* BGP message debug header.
   Copyright (C) 1996, 97, 98 Kunihiro Ishiguro

This file is part of GNU Zebra.

GNU Zebra is free software; you can redistribute it and/or modify it
under the terms of the GNU General Public License as published by the
Free Software Foundation; either version 2, or (at your option) any
later version.

GNU Zebra is distributed in the hope that it will be useful, but
WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
General Public License for more details.

You should have received a copy of the GNU General Public License
along with GNU Zebra; see the file COPYING.  If not, write to the Free
Software Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA
02111-1307, USA.  */

#ifndef _QUAGGA_BGP_DEBUG_H
#define _QUAGGA_BGP_DEBUG_H

#include "bgp_attr.h"
#include "bgp_updgrp.h"

/* sort of packet direction */
#define DUMP_ON        1
#define DUMP_SEND      2
#define DUMP_RECV      4

/* for dump_update */
#define DUMP_WITHDRAW  8
#define DUMP_NLRI     16

/* dump detail */
#define DUMP_DETAIL   32

/* RD + Prefix + Path-Id */
#define BGP_PRD_PATH_STRLEN (PREFIX_STRLEN + RD_ADDRSTRLEN + 20)

extern int dump_open;
extern int dump_update;
extern int dump_keepalive;
extern int dump_notify;

extern int Debug_Event;
extern int Debug_Keepalive;
extern int Debug_Update;
extern int Debug_Radix;

#define	NLRI	 1
#define	WITHDRAW 2
#define	NO_OPT	 3
#define	SEND	 4
#define	RECV	 5
#define	DETAIL	 6

/* Prototypes. */
extern void bgp_debug_init(void);
extern void bgp_packet_dump(struct stream *);

extern int debug(unsigned int option);

extern unsigned long conf_bgp_debug_as4;
extern unsigned long conf_bgp_debug_neighbor_events;
extern unsigned long conf_bgp_debug_packet;
extern unsigned long conf_bgp_debug_keepalive;
extern unsigned long conf_bgp_debug_update;
extern unsigned long conf_bgp_debug_bestpath;
extern unsigned long conf_bgp_debug_zebra;
extern unsigned long conf_bgp_debug_allow_martians;
extern unsigned long conf_bgp_debug_nht;
extern unsigned long conf_bgp_debug_update_groups;

extern unsigned long term_bgp_debug_as4;
extern unsigned long term_bgp_debug_neighbor_events;
extern unsigned long term_bgp_debug_packet;
extern unsigned long term_bgp_debug_keepalive;
extern unsigned long term_bgp_debug_update;
extern unsigned long term_bgp_debug_bestpath;
extern unsigned long term_bgp_debug_zebra;
extern unsigned long term_bgp_debug_allow_martians;
extern unsigned long term_bgp_debug_nht;
extern unsigned long term_bgp_debug_update_groups;

extern struct list *bgp_debug_neighbor_events_peers;
extern struct list *bgp_debug_keepalive_peers;
extern struct list *bgp_debug_update_in_peers;
extern struct list *bgp_debug_update_out_peers;
extern struct list *bgp_debug_update_prefixes;
extern struct list *bgp_debug_bestpath_prefixes;
extern struct list *bgp_debug_zebra_prefixes;

struct bgp_debug_filter {
	char *host;
	struct prefix *p;
};

#define BGP_DEBUG_AS4                 0x01
#define BGP_DEBUG_AS4_SEGMENT         0x02

#define BGP_DEBUG_BESTPATH            0x01
#define BGP_DEBUG_NEIGHBOR_EVENTS     0x01
#define BGP_DEBUG_PACKET              0x01
#define BGP_DEBUG_KEEPALIVE           0x01
#define BGP_DEBUG_UPDATE_IN           0x01
#define BGP_DEBUG_UPDATE_OUT          0x02
#define BGP_DEBUG_UPDATE_PREFIX       0x04
#define BGP_DEBUG_ZEBRA               0x01
#define BGP_DEBUG_ALLOW_MARTIANS      0x01
#define BGP_DEBUG_NHT                 0x01
#define BGP_DEBUG_UPDATE_GROUPS       0x01

#define BGP_DEBUG_PACKET_SEND         0x01
#define BGP_DEBUG_PACKET_SEND_DETAIL  0x02

#define CONF_DEBUG_ON(a, b)	(conf_bgp_debug_ ## a |= (BGP_DEBUG_ ## b))
#define CONF_DEBUG_OFF(a, b)	(conf_bgp_debug_ ## a &= ~(BGP_DEBUG_ ## b))

#define TERM_DEBUG_ON(a, b)	(term_bgp_debug_ ## a |= (BGP_DEBUG_ ## b))
#define TERM_DEBUG_OFF(a, b)	(term_bgp_debug_ ## a &= ~(BGP_DEBUG_ ## b))

#define DEBUG_ON(a, b)                                                         \
	do {                                                                   \
		CONF_DEBUG_ON(a, b);                                           \
		TERM_DEBUG_ON(a, b);                                           \
	} while (0)
#define DEBUG_OFF(a, b)                                                        \
	do {                                                                   \
		CONF_DEBUG_OFF(a, b);                                          \
		TERM_DEBUG_OFF(a, b);                                          \
	} while (0)

#define BGP_DEBUG(a, b)		(term_bgp_debug_ ## a & BGP_DEBUG_ ## b)
#define CONF_BGP_DEBUG(a, b)    (conf_bgp_debug_ ## a & BGP_DEBUG_ ## b)

extern const char *bgp_type_str[];

extern int bgp_dump_attr(struct peer *, struct attr *, char *, size_t);
extern int bgp_debug_peer_updout_enabled(char *host);
extern const char *bgp_notify_code_str(char);
extern const char *bgp_notify_subcode_str(char, char);
extern void bgp_notify_print(struct peer *, struct bgp_notify *, const char *);

extern const struct message bgp_status_msg[];
extern int bgp_debug_neighbor_events(struct peer *peer);
extern int bgp_debug_keepalive(struct peer *peer);
extern int bgp_debug_update(struct peer *peer, struct prefix *p,
			    struct update_group *updgrp, unsigned int inbound);
extern int bgp_debug_bestpath(struct prefix *p);
extern int bgp_debug_zebra(struct prefix *p);

extern int bgp_debug_count(void);
extern const char *bgp_debug_rdpfxpath2str(struct prefix_rd *,
					   union prefixconstptr, int, u_int32_t,
					   char *, int);
const char *bgp_notify_admin_message(char *buf, size_t bufsz, u_char *data,
				     size_t datalen);

#endif /* _QUAGGA_BGP_DEBUG_H */
