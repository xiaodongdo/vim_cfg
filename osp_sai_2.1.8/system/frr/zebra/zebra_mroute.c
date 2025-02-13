/* zebra_mroute code
 * Copyright (C) 2016 Cumulus Networks, Inc.
 * Donald Sharp
 *
 * This file is part of Quagga
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
 * along with GNU Zebra; see the file COPYING.  If not, write to the Free
 * Software Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA
 * 02111-1307, USA.
 */

#include <zebra.h>

#include "stream.h"
#include "prefix.h"
#include "vrf.h"
#include "rib.h"

#include "zebra/zserv.h"
#include "zebra/zebra_vrf.h"
#include "zebra/zebra_mroute.h"
#include "zebra/rt.h"

int zebra_ipmr_route_stats(struct zserv *client, int fd, u_short length,
			   struct zebra_vrf *zvrf)
{
	struct mcast_route_data mroute;
	struct stream *s;
	int suc;

	char sbuf[40];
	char gbuf[40];

	memset(&mroute, 0, sizeof(mroute));
	stream_get(&mroute.sg.src, client->ibuf, 4);
	stream_get(&mroute.sg.grp, client->ibuf, 4);
	mroute.ifindex = stream_getl(client->ibuf);

	strcpy(sbuf, inet_ntoa(mroute.sg.src));
	strcpy(gbuf, inet_ntoa(mroute.sg.grp));

	suc = kernel_get_ipmr_sg_stats(&mroute);

	s = client->obuf;

	stream_reset(s);

	zserv_create_header(s, ZEBRA_IPMR_ROUTE_STATS, zvrf_id(zvrf));
	stream_put_in_addr(s, &mroute.sg.src);
	stream_put_in_addr(s, &mroute.sg.grp);
	stream_put(s, &mroute.lastused, sizeof(mroute.lastused));
	stream_putl(s, suc);

	stream_putw_at(s, 0, stream_get_endp(s));
	zebra_server_send_message(client);
	return 0;
}
