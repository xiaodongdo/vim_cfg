/* BGP flap dampening
   Copyright (C) 2001 IP Infusion Inc.

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

#ifndef _QUAGGA_BGP_DAMP_H
#define _QUAGGA_BGP_DAMP_H

/* Structure maintained on a per-route basis. */
struct bgp_damp_info {
	/* Doubly linked list.  This information must be linked to
	   reuse_list or no_reuse_list.  */
	struct bgp_damp_info *next;
	struct bgp_damp_info *prev;

	/* Figure-of-merit.  */
	unsigned int penalty;

	/* Number of flapping.  */
	unsigned int flap;

	/* First flap time  */
	time_t start_time;

	/* Last time penalty was updated.  */
	time_t t_updated;

	/* Time of route start to be suppressed.  */
	time_t suppress_time;

	/* Back reference to bgp_info. */
	struct bgp_info *binfo;

	/* Back reference to bgp_node. */
	struct bgp_node *rn;

	/* Current index in the reuse_list. */
	int index;

	/* Last time message type. */
	u_char lastrecord;
#define BGP_RECORD_UPDATE	1U
#define BGP_RECORD_WITHDRAW	2U

	afi_t afi;
	safi_t safi;
};

/* Specified parameter set configuration. */
struct bgp_damp_config {
	/* Value over which routes suppressed.  */
	unsigned int suppress_value;

	/* Value below which suppressed routes reused.  */
	unsigned int reuse_limit;

	/* Max time a route can be suppressed.  */
	time_t max_suppress_time;

	/* Time during which accumulated penalty reduces by half.  */
	time_t half_life;

	/* Non-configurable parameters but fixed at implementation time.
	 * To change this values, init_bgp_damp() should be modified.
	 */
	time_t tmax; /* Max time previous instability retained */
	unsigned int reuse_list_size;  /* Number of reuse lists */
	unsigned int reuse_index_size; /* Size of reuse index array */

	/* Non-configurable parameters.  Most of these are calculated from
	 * the configurable parameters above.
	 */
	unsigned int ceiling;		  /* Max value a penalty can attain */
	unsigned int decay_rate_per_tick; /* Calculated from half-life */
	unsigned int decay_array_size; /* Calculated using config parameters */
	double scale_factor;
	unsigned int reuse_scale_factor;

	/* Decay array per-set based. */
	double *decay_array;

	/* Reuse index array per-set based. */
	int *reuse_index;

	/* Reuse list array per-set based. */
	struct bgp_damp_info **reuse_list;
	int reuse_offset;

	/* All dampening information which is not on reuse list.  */
	struct bgp_damp_info *no_reuse_list;

	/* Reuse timer thread per-set base. */
	struct thread *t_reuse;
};

#define BGP_DAMP_NONE           0
#define BGP_DAMP_USED		1
#define BGP_DAMP_SUPPRESSED	2

/* Time granularity for reuse lists */
#define DELTA_REUSE	          10

/* Time granularity for decay arrays */
#define DELTA_T 	           5

#define DEFAULT_PENALTY         1000

#define DEFAULT_HALF_LIFE         15
#define DEFAULT_REUSE 	       	 750
#define DEFAULT_SUPPRESS 	2000

#define REUSE_LIST_SIZE          256
#define REUSE_ARRAY_SIZE        1024

extern int bgp_damp_enable(struct bgp *, afi_t, safi_t, time_t, unsigned int,
			   unsigned int, time_t);
extern int bgp_damp_disable(struct bgp *, afi_t, safi_t);
extern int bgp_damp_withdraw(struct bgp_info *, struct bgp_node *, afi_t,
			     safi_t, int);
extern int bgp_damp_update(struct bgp_info *, struct bgp_node *, afi_t, safi_t);
extern int bgp_damp_scan(struct bgp_info *, afi_t, safi_t);
extern void bgp_damp_info_free(struct bgp_damp_info *, int);
extern void bgp_damp_info_clean(void);
extern int bgp_damp_decay(time_t, int);
extern void bgp_config_write_damp(struct vty *);
extern void bgp_damp_info_vty(struct vty *, struct bgp_info *,
			      json_object *json_path);
extern const char *bgp_damp_reuse_time_vty(struct vty *, struct bgp_info *,
					   char *, size_t, u_char,
					   json_object *);
extern int bgp_show_dampening_parameters(struct vty *vty, afi_t, safi_t);

#endif /* _QUAGGA_BGP_DAMP_H */
