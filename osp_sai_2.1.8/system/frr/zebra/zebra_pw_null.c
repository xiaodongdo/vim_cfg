/* Zebra PW code
 * Copyright (C) 2016 Volta Networks, Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; see the file COPYING; if not, write to the
 * Free Software Foundation, Inc., 51 Franklin St, Fifth Floor, Boston,
 * MA 02110-1301 USA
 */

#include <zebra.h>

#include "vrf.h"

#include "zebra/rib.h"
#include "zebra/zserv.h"
#include "zebra/zebra_vrf.h"

/* upgrade frr to 3.0, modified by liwh for bug 46090, 2018-01-04 */
#ifndef _CENTEC_
void zebra_pw_init(struct zebra_vrf *zvrf)
{
}

void zebra_pw_exit(struct zebra_vrf *zvrf)
{
}
#endif
/*liwh end */
