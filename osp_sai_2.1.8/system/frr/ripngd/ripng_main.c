/*
 * RIPngd main routine.
 * Copyright (C) 1998, 1999 Kunihiro Ishiguro
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

#include <zebra.h>

#include <lib/version.h>
#include "getopt.h"
#include "vector.h"
#include "vty.h"
#include "command.h"
#include "memory.h"
#include "memory_vty.h"
#include "thread.h"
#include "log.h"
#include "prefix.h"
#include "if.h"
#include "privs.h"
#include "sigevent.h"
#include "vrf.h"
#include "libfrr.h"

#include "ripngd/ripngd.h"

/* RIPngd options. */
struct option longopts[] = {{"retain", no_argument, NULL, 'r'}, {0}};

/* ripngd privileges */
zebra_capabilities_t _caps_p[] = {ZCAP_NET_RAW, ZCAP_BIND};

struct zebra_privs_t ripngd_privs = {
#if defined(FRR_USER)
	.user = FRR_USER,
#endif
#if defined FRR_GROUP
	.group = FRR_GROUP,
#endif
#ifdef VTY_GROUP
	.vty_group = VTY_GROUP,
#endif
	.caps_p = _caps_p,
	.cap_num_p = 2,
	.cap_num_i = 0};


/* RIPngd program name */

/* Route retain mode flag. */
int retain_mode = 0;

/* Master of threads. */
struct thread_master *master;

static struct frr_daemon_info ripngd_di;

/* SIGHUP handler. */
static void sighup(void)
{
	zlog_info("SIGHUP received");
	ripng_clean();
	ripng_reset();

	/* Reload config file. */
	vty_read_config(ripngd_di.config_file, config_default);

	/* Try to return to normal operation. */
}

/* SIGINT handler. */
static void sigint(void)
{
	zlog_notice("Terminating on signal");

	if (!retain_mode)
		ripng_clean();

	exit(0);
}

/* SIGUSR1 handler. */
static void sigusr1(void)
{
	zlog_rotate();
}

struct quagga_signal_t ripng_signals[] = {
	{
		.signal = SIGHUP,
		.handler = &sighup,
	},
	{
		.signal = SIGUSR1,
		.handler = &sigusr1,
	},
	{
		.signal = SIGINT,
		.handler = &sigint,
	},
	{
		.signal = SIGTERM,
		.handler = &sigint,
	},
};

FRR_DAEMON_INFO(ripngd, RIPNG, .vty_port = RIPNG_VTY_PORT,

		.proghelp = "Implementation of the RIPng routing protocol.",

		.signals = ripng_signals,
		.n_signals = array_size(ripng_signals),

		.privs = &ripngd_privs, )

/* RIPngd main routine. */
int main(int argc, char **argv)
{
	frr_preinit(&ripngd_di, argc, argv);
	frr_opt_add(
		"r", longopts,
		"  -r, --retain       When program terminates, retain added route by ripd.\n");

	while (1) {
		int opt;

		opt = frr_getopt(argc, argv, NULL);

		if (opt == EOF)
			break;

		switch (opt) {
		case 0:
			break;
		case 'r':
			retain_mode = 1;
			break;
		default:
			frr_help_exit(1);
			break;
		}
	}

	master = frr_init();

	/* Library inits. */
	vrf_init();

	/* RIPngd inits. */
	ripng_init();
	zebra_init(master);
	ripng_peer_init();

	frr_config_fork();
	frr_run(master);

	/* Not reached. */
	return 0;
}
