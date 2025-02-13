/*
 * libfrr overall management functions
 *
 * Copyright (C) 2016  David Lamparter for NetDEF, Inc.
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the Free
 * Software Foundation; either version 2 of the License, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program; if not, write to the Free Software Foundation, Inc., 59 Temple
 * Place, Suite 330, Boston, MA  02111-1307  USA
 */

#ifndef _ZEBRA_FRR_H
#define _ZEBRA_FRR_H

#include "sigevent.h"
#include "privs.h"
#include "thread.h"
#include "log.h"
#include "getopt.h"
#include "module.h"
#include "hook.h"

#define FRR_NO_PRIVSEP		(1 << 0)
#define FRR_NO_TCPVTY		(1 << 1)
#define FRR_LIMITED_CLI		(1 << 2)
#define FRR_NO_CFG_PID_DRY		(1 << 3)
#define FRR_NO_ZCLIENT		(1 << 4)

struct frr_daemon_info {
	unsigned flags;

	const char *progname;
	const char *name;
	const char *logname;
	unsigned short instance;
	struct frrmod_runtime *module;

	char *vty_addr;
	int vty_port;
	char *vty_sock_path;
	bool dryrun;
	bool daemon_mode;
	const char *config_file;
	const char *pid_file;
	const char *vty_path;
	const char *module_path;

	const char *proghelp;
	void (*printhelp)(FILE *target);
	const char *copyright;
	char startinfo[128];

	struct quagga_signal_t *signals;
	size_t n_signals;

	struct zebra_privs_t *privs;
};

/* execname is the daemon's executable (and pidfile and configfile) name,
 * i.e. "zebra" or "bgpd"
 * constname is the daemons source-level name, primarily for the logging ID,
 * i.e. "ZEBRA" or "BGP"
 *
 * note that this macro is also a latch-on point for other changes (e.g.
 * upcoming module support) that need to place some per-daemon things.  Each
 * daemon should have one of these.
 */
#define FRR_DAEMON_INFO(execname, constname, ...)                              \
	static struct frr_daemon_info execname##_di = {.name = #execname,      \
						       .logname = #constname,  \
						       .module = THIS_MODULE,  \
						       __VA_ARGS__};           \
	FRR_COREMOD_SETUP(.name = #execname,                                   \
			  .description = #execname " daemon",                  \
			  .version = FRR_VERSION, )                            \
/* end */

extern void frr_preinit(struct frr_daemon_info *daemon, int argc, char **argv);
extern void frr_opt_add(const char *optstr, const struct option *longopts,
			const char *helpstr);
extern int frr_getopt(int argc, char *const argv[], int *longindex);
extern void frr_help_exit(int status);

extern struct thread_master *frr_init(void);

DECLARE_HOOK(frr_late_init, (struct thread_master * tm), (tm))
extern void frr_config_fork(void);

extern void frr_vty_serv(void);

/* note: contains call to frr_vty_serv() */
extern void frr_run(struct thread_master *master);

extern char config_default[256];
extern const char frr_sysconfdir[];
extern const char frr_vtydir[];
extern const char frr_moduledir[];

#endif /* _ZEBRA_FRR_H */
