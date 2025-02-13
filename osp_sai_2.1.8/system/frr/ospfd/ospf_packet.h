/*
 * OSPF Sending and Receiving OSPF Packets.
 * Copyright (C) 1999 Toshiaki Takada
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

#ifndef _ZEBRA_OSPF_PACKET_H
#define _ZEBRA_OSPF_PACKET_H

#define OSPF_HEADER_SIZE         24U
#define OSPF_AUTH_SIMPLE_SIZE     8U
#define OSPF_AUTH_MD5_SIZE       16U

#define OSPF_MAX_PACKET_SIZE  65535U   /* includes IP Header size. */
#define OSPF_HELLO_MIN_SIZE      20U   /* not including neighbors */
#define OSPF_DB_DESC_MIN_SIZE     8U
#define OSPF_LS_REQ_MIN_SIZE      0U
#define OSPF_LS_UPD_MIN_SIZE      4U
#define OSPF_LS_ACK_MIN_SIZE      0U

#define OSPF_MSG_HELLO         1  /* OSPF Hello Message. */
#define OSPF_MSG_DB_DESC       2  /* OSPF Database Descriptoin Message. */
#define OSPF_MSG_LS_REQ        3  /* OSPF Link State Request Message. */
#define OSPF_MSG_LS_UPD        4  /* OSPF Link State Update Message. */
#define OSPF_MSG_LS_ACK        5  /* OSPF Link State Acknoledgement Message. */

#define OSPF_SEND_PACKET_DIRECT         1
#define OSPF_SEND_PACKET_INDIRECT       2
#define OSPF_SEND_PACKET_LOOP           3

#define OSPF_HELLO_REPLY_DELAY          1

/* Return values of functions involved in packet verification, see ospf6d. */
#define MSG_OK    0
#define MSG_NG    1

struct ospf_packet {
	struct ospf_packet *next;

	/* Pointer to data stream. */
	struct stream *s;

	/* IP destination address. */
	struct in_addr dst;

	/* OSPF packet length. */
	u_int16_t length;
};

/* OSPF packet queue structure. */
struct ospf_fifo {
	unsigned long count;

	struct ospf_packet *head;
	struct ospf_packet *tail;
};

/* OSPF packet header structure. */
struct ospf_header {
	u_char version;		  /* OSPF Version. */
	u_char type;		  /* Packet Type. */
	u_int16_t length;	 /* Packet Length. */
	struct in_addr router_id; /* Router ID. */
	struct in_addr area_id;   /* Area ID. */
	u_int16_t checksum;       /* Check Sum. */
	u_int16_t auth_type;      /* Authentication Type. */
	/* Authentication Data. */
	union {
		/* Simple Authentication. */
		u_char auth_data[OSPF_AUTH_SIMPLE_SIZE];
		/* Cryptographic Authentication. */
		struct {
			u_int16_t zero;		/* Should be 0. */
			u_char key_id;		/* Key ID. */
			u_char auth_data_len;   /* Auth Data Length. */
			u_int32_t crypt_seqnum; /* Cryptographic Sequence
						   Number. */
		} crypt;
	} u;
};

/* OSPF Hello body format. */
struct ospf_hello {
	struct in_addr network_mask;
	u_int16_t hello_interval;
	u_char options;
	u_char priority;
	u_int32_t dead_interval;
	struct in_addr d_router;
	struct in_addr bd_router;
	struct in_addr neighbors[1];
};

/* OSPF Database Description body format. */
struct ospf_db_desc {
	u_int16_t mtu;
	u_char options;
	u_char flags;
	u_int32_t dd_seqnum;
};

struct ospf_ls_update {
	u_int32_t num_lsas;
};

/* Macros. */
/* XXX Perhaps obsolete; function in ospf_packet.c */
#define OSPF_PACKET_MAX(oi)     ospf_packet_max (oi)

#define OSPF_OUTPUT_PNT(S)      ((S)->data + (S)->putp)
#define OSPF_OUTPUT_LENGTH(S)   ((S)->endp)

#define IS_SET_DD_MS(X)         ((X) & OSPF_DD_FLAG_MS)
#define IS_SET_DD_M(X)          ((X) & OSPF_DD_FLAG_M)
#define IS_SET_DD_I(X)          ((X) & OSPF_DD_FLAG_I)
#define IS_SET_DD_ALL(X)        ((X) & OSPF_DD_FLAG_ALL)

/* Prototypes. */
extern void ospf_output_forward(struct stream *, int);
extern struct ospf_packet *ospf_packet_new(size_t);
extern void ospf_packet_free(struct ospf_packet *);
extern struct ospf_fifo *ospf_fifo_new(void);
extern void ospf_fifo_push(struct ospf_fifo *, struct ospf_packet *);
extern struct ospf_packet *ospf_fifo_pop(struct ospf_fifo *);
extern struct ospf_packet *ospf_fifo_head(struct ospf_fifo *);
extern void ospf_fifo_flush(struct ospf_fifo *);
extern void ospf_fifo_free(struct ospf_fifo *);
/* modified by liwh for bug 44873, 2017-08-14 */
#ifdef _CENTEC_
extern int ospf_packet_add (struct ospf_interface *, struct ospf_packet *);
#else
extern void ospf_packet_add (struct ospf_interface *, struct ospf_packet *);
#endif
/* liwh end */
extern void ospf_packet_delete(struct ospf_interface *);
extern struct stream *ospf_stream_dup(struct stream *);
extern struct ospf_packet *ospf_packet_dup(struct ospf_packet *);

extern int ospf_read(struct thread *);
extern void ospf_hello_send(struct ospf_interface *);
extern void ospf_db_desc_send(struct ospf_neighbor *);
extern void ospf_db_desc_resend(struct ospf_neighbor *);
extern void ospf_ls_req_send(struct ospf_neighbor *);
extern void ospf_ls_upd_send_lsa(struct ospf_neighbor *, struct ospf_lsa *,
				 int);
extern void ospf_ls_upd_send(struct ospf_neighbor *, struct list *, int);
extern void ospf_ls_ack_send(struct ospf_neighbor *, struct ospf_lsa *);
extern void ospf_ls_ack_send_delayed(struct ospf_interface *);
extern void ospf_ls_retransmit(struct ospf_interface *, struct ospf_lsa *);
extern void ospf_ls_req_event(struct ospf_neighbor *);

extern int ospf_ls_upd_timer(struct thread *);
extern int ospf_ls_ack_timer(struct thread *);
extern int ospf_poll_timer(struct thread *);
extern int ospf_hello_reply_timer(struct thread *);

extern const struct message ospf_packet_type_str[];
extern const size_t ospf_packet_type_str_max;

extern void ospf_proactively_arp(struct ospf_neighbor *);

#endif /* _ZEBRA_OSPF_PACKET_H */
