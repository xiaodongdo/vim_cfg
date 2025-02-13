/* Virtual terminal interface shell.
 * Copyright (C) 2000 Kunihiro Ishiguro
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

#include <sys/un.h>
#include <setjmp.h>
#include <sys/wait.h>
#include <pwd.h>
#include <sys/file.h>
#include <unistd.h>

#include <readline/readline.h>
#include <readline/history.h>

#include <lib/version.h>
#include "getopt.h"
#include "command.h"
#include "memory.h"
#include "linklist.h"
#include "memory_vty.h"
#include "libfrr.h"

#include "vtysh/vtysh.h"
#include "vtysh/vtysh_user.h"

/* VTY shell program name. */
char *progname;

/* Configuration file name and directory. */
static char vtysh_config_always[MAXPATHLEN] = SYSCONFDIR VTYSH_DEFAULT_CONFIG;
static char quagga_config_default[MAXPATHLEN] = SYSCONFDIR FRR_DEFAULT_CONFIG;
char *quagga_config = quagga_config_default;
char history_file[MAXPATHLEN];

/* Flag for indicate executing child command. */
int execute_flag = 0;

/* VTY Socket prefix */
const char *vty_sock_path = NULL;

/* For sigsetjmp() & siglongjmp(). */
static sigjmp_buf jmpbuf;

/* Flag for avoid recursive siglongjmp() call. */
static int jmpflag = 0;

/* A static variable for holding the line. */
static char *line_read;

/* Master of threads. */
struct thread_master *master;

/* Command logging */
FILE *logfile;

/* SIGTSTP handler.  This function care user's ^Z input. */
static void sigtstp(int sig)
{
	/* Execute "end" command. */
	vtysh_execute("end");

	/* Initialize readline. */
	rl_initialize();
	printf("\n");

	/* Check jmpflag for duplicate siglongjmp(). */
	if (!jmpflag)
		return;

	jmpflag = 0;

	/* Back to main command loop. */
	siglongjmp(jmpbuf, 1);
}


/*fix bug 45658 by chenc*/
#ifndef _CENTEC_
/* SIGINT handler.  This function care user's ^Z input.  */
static void sigint(int sig)
{
	/* Check this process is not child process. */
	if (!execute_flag) {
		rl_initialize();
		printf("\n");
		rl_forced_update_display();
	}
}
#endif


void sighquit(int sig)
{
    exit(0);
}


/* Signale wrapper for vtysh. We don't use sigevent because
 * vtysh doesn't use threads. TODO */
static void vtysh_signal_set(int signo, void (*func)(int))
{
	struct sigaction sig;
	struct sigaction osig;

	sig.sa_handler = func;
	sigemptyset(&sig.sa_mask);
	sig.sa_flags = 0;
#ifdef SA_RESTART
	sig.sa_flags |= SA_RESTART;
#endif /* SA_RESTART */

	sigaction(signo, &sig, &osig);
}

/* Initialization of signal handles. */
static void vtysh_signal_init(void)
{
  /*fix bug 45658 by chenc*/
  #ifndef _CENTEC_
	vtysh_signal_set(SIGINT, sigint);
  #endif
	vtysh_signal_set(SIGTSTP, sigtstp);
  /* SYSTEM MODIFIED by kcao for bug 39947, add SIGQUIT process */
      vtysh_signal_set(SIGQUIT, sighquit);
	vtysh_signal_set(SIGPIPE, SIG_IGN);
}

/* Help information display. */
static void usage(int status)
{
	if (status != 0)
		fprintf(stderr, "Try `%s --help' for more information.\n",
			progname);
	else
		printf("Usage : %s [OPTION...]\n\n"
		       "Integrated shell for FRR. \n\n"
		       "-b, --boot               Execute boot startup configuration\n"
		       "-c, --command            Execute argument as command\n"
		       "-d, --daemon             Connect only to the specified daemon\n"
		       "-f, --inputfile          Execute commands from specific file and exit\n"
		       "-E, --echo               Echo prompt and command in -c mode\n"
		       "-C, --dryrun             Check configuration for validity and exit\n"
		       "-m, --markfile           Mark input file with context end\n"
		       "    --vty_socket         Override vty socket path\n"
		       "    --config_dir         Override config directory path\n"
		       "-w, --writeconfig        Write integrated config (frr.conf) and exit\n"
		       "-h, --help               Display this help and exit\n\n"
		       "Note that multiple commands may be executed from the command\n"
		       "line by passing multiple -c args, or by embedding linefeed\n"
		       "characters in one or more of the commands.\n\n"
		       "Report bugs to %s\n",
		       progname, FRR_BUG_ADDRESS);

	exit(status);
}

/* VTY shell options, we use GNU getopt library. */
#define OPTION_VTYSOCK 1000
#define OPTION_CONFDIR 1001
struct option longopts[] = {
	{"boot", no_argument, NULL, 'b'},
	/* For compatibility with older zebra/quagga versions */
	{"eval", required_argument, NULL, 'e'},
	{"command", required_argument, NULL, 'c'},
	{"daemon", required_argument, NULL, 'd'},
	{"vty_socket", required_argument, NULL, OPTION_VTYSOCK},
	{"config_dir", required_argument, NULL, OPTION_CONFDIR},
	{"inputfile", required_argument, NULL, 'f'},
	{"echo", no_argument, NULL, 'E'},
	{"dryrun", no_argument, NULL, 'C'},
	{"help", no_argument, NULL, 'h'},
	{"noerror", no_argument, NULL, 'n'},
	{"mark", no_argument, NULL, 'm'},
	{"writeconfig", no_argument, NULL, 'w'},
	{0}};

/* Read a string, and return a pointer to it.  Returns NULL on EOF. */
static char *vtysh_rl_gets(void)
{
	HIST_ENTRY *last;
	/* If the buffer has already been allocated, return the memory
	 * to the free pool. */
	if (line_read) {
		free(line_read);
		line_read = NULL;
	}

	/* Get a line from the user.  Change prompt according to node.  XXX. */
	line_read = readline(vtysh_prompt());

	/* If the line has any text in it, save it on the history. But only if
	 * last command in history isn't the same one. */
	if (line_read && *line_read) {
		using_history();
		last = previous_history();
		if (!last || strcmp(last->line, line_read) != 0) {
			add_history(line_read);
			append_history(1, history_file);
		}
	}

	return (line_read);
}

static void log_it(const char *line)
{
	time_t t = time(NULL);
	struct tm *tmp = localtime(&t);
	const char *user = getenv("USER");
	char tod[64];

	if (!user)
		user = "boot";

	strftime(tod, sizeof tod, "%Y%m%d-%H:%M.%S", tmp);

	fprintf(logfile, "%s:%s %s\n", tod, user, line);
}

static int flock_fd;

static void vtysh_flock_config(const char *flock_file)
{
	int count = 0;

	flock_fd = open(flock_file, O_RDONLY, 0644);
	if (flock_fd < 0) {
		fprintf(stderr, "Unable to create lock file: %s, %s\n",
			flock_file, safe_strerror(errno));
		return;
	}

	while (count < 400 && (flock(flock_fd, LOCK_EX | LOCK_NB) < 0)) {
		count++;
		usleep(500000);
	}

	if (count >= 400)
		fprintf(stderr,
			"Flock of %s failed, continuing this may cause issues\n",
			flock_file);
}

static void vtysh_unflock_config(void)
{
	flock(flock_fd, LOCK_UN);
	close(flock_fd);
}

/* VTY shell main routine. */
int main(int argc, char **argv, char **env)
{
	char *p;
	int opt;
	int dryrun = 0;
	int boot_flag = 0;
	const char *daemon_name = NULL;
	const char *inputfile = NULL;
	const char *vtysh_configfile_name;
	struct cmd_rec {
		char *line;
		struct cmd_rec *next;
	} *cmd = NULL;
	struct cmd_rec *tail = NULL;
	int echo_command = 0;
	int no_error = 0;
	int markfile = 0;
	int writeconfig = 0;
	int ret = 0;
	char *homedir = NULL;

	/* check for restricted functionality if vtysh is run setuid */
	int restricted = (getuid() != geteuid()) || (getgid() != getegid());

	/* Preserve name of myself. */
	progname = ((p = strrchr(argv[0], '/')) ? ++p : argv[0]);

	/* if logging open now */
	if ((p = getenv("VTYSH_LOG")) != NULL)
		logfile = fopen(p, "a");

	/* Option handling. */
	while (1) {
		opt = getopt_long(argc, argv, "be:c:d:nf:mEhCw", longopts, 0);

		if (opt == EOF)
			break;

		switch (opt) {
		case 0:
			break;
		case 'b':
			boot_flag = 1;
			break;
		case 'e':
		case 'c': {
			struct cmd_rec *cr;
			cr = XMALLOC(MTYPE_TMP, sizeof(*cr));
			cr->line = optarg;
			cr->next = NULL;
			if (tail)
				tail->next = cr;
			else
				cmd = cr;
			tail = cr;
		} break;
		case OPTION_VTYSOCK:
			vty_sock_path = optarg;
			break;
		case OPTION_CONFDIR:
			/*
			 * Skip option for Config Directory if setuid
			 */
			if (restricted) {
				fprintf(stderr,
					"Overriding of Config Directory blocked for vtysh with setuid");
				return 1;
			}
			/*
			 * Overwrite location for vtysh.conf
			 */
			vtysh_configfile_name =
				strrchr(VTYSH_DEFAULT_CONFIG, '/');
			if (vtysh_configfile_name)
				/* skip '/' */
				vtysh_configfile_name++;
			else
				/*
				 * VTYSH_DEFAULT_CONFIG configured with relative
				 * path
				 * during config? Should really never happen for
				 * sensible config
				 */
				vtysh_configfile_name =
					(char *)VTYSH_DEFAULT_CONFIG;
			strlcpy(vtysh_config_always, optarg,
				sizeof(vtysh_config_always));
			strlcat(vtysh_config_always, "/",
				sizeof(vtysh_config_always));
			strlcat(vtysh_config_always, vtysh_configfile_name,
				sizeof(vtysh_config_always));
			/*
			 * Overwrite location for frr.conf
			 */
			vtysh_configfile_name =
				strrchr(FRR_DEFAULT_CONFIG, '/');
			if (vtysh_configfile_name)
				/* skip '/' */
				vtysh_configfile_name++;
			else
				/*
				 * FRR_DEFAULT_CONFIG configured with relative
				 * path
				 * during config? Should really never happen for
				 * sensible config
				 */
				vtysh_configfile_name =
					(char *)FRR_DEFAULT_CONFIG;
			strlcpy(quagga_config_default, optarg,
				sizeof(vtysh_config_always));
			strlcat(quagga_config_default, "/",
				sizeof(vtysh_config_always));
			strlcat(quagga_config_default, vtysh_configfile_name,
				sizeof(quagga_config_default));
			break;
		case 'd':
			daemon_name = optarg;
			break;
		case 'f':
			inputfile = optarg;
			break;
		case 'm':
			markfile = 1;
			break;
		case 'n':
			no_error = 1;
			break;
		case 'E':
			echo_command = 1;
			break;
		case 'C':
			dryrun = 1;
			break;
		case 'w':
			writeconfig = 1;
			break;
		case 'h':
			usage(0);
			break;
		default:
			usage(1);
			break;
		}
	}

	if (!vty_sock_path)
		vty_sock_path = frr_vtydir;

	if (markfile + writeconfig + dryrun + boot_flag > 1) {
		fprintf(stderr,
			"Invalid combination of arguments.  Please specify at "
			"most one of:\n\t-b, -C, -m, -w\n");
		return 1;
	}
	if (inputfile && (writeconfig || boot_flag)) {
		fprintf(stderr,
			"WARNING: Combinining the -f option with -b or -w is "
			"NOT SUPPORTED since its\nresults are inconsistent!\n");
	}

	/* Initialize user input buffer. */
	line_read = NULL;
	setlinebuf(stdout);

	/* Signal and others. */
	vtysh_signal_init();

	/* Make vty structure and register commands. */
	vtysh_init_vty();
	vtysh_init_cmd();
	vtysh_user_init();
	vtysh_config_init();

	vty_init_vtysh();

#ifndef _CENTEC_
  /* SYSTEM MODIFIED Deleted by kcao 2017-05-27 for bug 44178 */
	/* Read vtysh configuration file before connecting to daemons. */
	vtysh_read_config(vtysh_config_always);
#endif

	if (markfile) {
		if (!inputfile) {
			fprintf(stderr,
				"-f option MUST be specified with -m option\n");
			return (1);
		}
		return (vtysh_mark_file(inputfile));
	}

	/* Start execution only if not in dry-run mode */
	if (dryrun && !cmd) {
		if (inputfile) {
			ret = vtysh_read_config(inputfile);
		} else {
			ret = vtysh_read_config(quagga_config_default);
		}

		exit(ret);
	}

	if (dryrun && cmd) {
		vtysh_execute("enable");
		while (cmd) {
			struct cmd_rec *cr;
			char *cmdnow = cmd->line, *next;
			do {
				next = strchr(cmdnow, '\n');
				if (next)
					*next++ = '\0';

				if (echo_command)
					printf("%s%s\n", vtysh_prompt(),
					       cmdnow);

				ret = vtysh_execute_no_pager(cmdnow);
				if (!no_error
				    && !(ret == CMD_SUCCESS
					 || ret == CMD_SUCCESS_DAEMON
					 || ret == CMD_WARNING))
					exit(1);
			} while ((cmdnow = next) != NULL);

			cr = cmd;
			cmd = cmd->next;
			XFREE(MTYPE_TMP, cr);
		}
		exit(ret);
	}

	/* Ignore error messages */
	if (no_error) {
		if (freopen("/dev/null", "w", stdout) == NULL) {
			fprintf(stderr,
				"Exiting: Failed to duplicate stdout with -n option");
			exit(1);
		}
	}

	/* Make sure we pass authentication before proceeding. */
	vtysh_auth();

	/* Do not connect until we have passed authentication. */
	if (vtysh_connect_all(daemon_name) <= 0) {
		fprintf(stderr, "Exiting: failed to connect to any daemons.\n");
		if (no_error)
			exit(0);
		else
			exit(1);
	}

	if (writeconfig) {
		vtysh_execute("enable");
		return vtysh_write_config_integrated();
	}

	if (inputfile) {
		vtysh_flock_config(inputfile);
		ret = vtysh_read_config(inputfile);
		vtysh_unflock_config();
		exit(ret);
	}

	/*
	 * Setup history file for use by both -c and regular input
	 * If we can't find the home directory, then don't store
	 * the history information
	 */
	homedir = vtysh_get_home();
	if (homedir) {
		snprintf(history_file, sizeof(history_file),
			 "%s/.history_quagga", homedir);
		if (read_history(history_file) != 0) {
			int fp;

			fp = open(history_file, O_CREAT | O_EXCL,
				  S_IRUSR | S_IWUSR);
			if (fp)
				close(fp);

			read_history(history_file);
		}
	}

	/* If eval mode. */
	if (cmd) {
		/* Enter into enable node. */
		vtysh_execute("enable");

		while (cmd != NULL) {
			int ret;
			char *eol;

			while ((eol = strchr(cmd->line, '\n')) != NULL) {
				*eol = '\0';

				add_history(cmd->line);
				append_history(1, history_file);

				if (echo_command)
					printf("%s%s\n", vtysh_prompt(),
					       cmd->line);

				if (logfile)
					log_it(cmd->line);

				ret = vtysh_execute_no_pager(cmd->line);
				if (!no_error
				    && !(ret == CMD_SUCCESS
					 || ret == CMD_SUCCESS_DAEMON
					 || ret == CMD_WARNING))
					exit(1);

				cmd->line = eol + 1;
			}

			add_history(cmd->line);
			append_history(1, history_file);

			if (echo_command)
				printf("%s%s\n", vtysh_prompt(), cmd->line);

			if (logfile)
				log_it(cmd->line);

			ret = vtysh_execute_no_pager(cmd->line);
			if (!no_error
			    && !(ret == CMD_SUCCESS || ret == CMD_SUCCESS_DAEMON
				 || ret == CMD_WARNING))
				exit(1);

			{
				struct cmd_rec *cr;
				cr = cmd;
				cmd = cmd->next;
				XFREE(MTYPE_TMP, cr);
			}
		}

		history_truncate_file(history_file, 1000);
		exit(0);
	}

	/* Boot startup configuration file. */
	if (boot_flag) {
		vtysh_flock_config(quagga_config);
		int ret = vtysh_read_config(quagga_config);
		vtysh_unflock_config();
		if (ret) {
			fprintf(stderr,
				"Configuration file[%s] processing failure: %d\n",
				quagga_config, ret);
			if (no_error)
				exit(0);
			else
				exit(ret);
		} else
			exit(0);
	}

	vtysh_pager_init();

	vtysh_readline_init();

	vty_hello(vty);

	/* Enter into enable node. */
	vtysh_execute("enable");

	/* Preparation for longjmp() in sigtstp(). */
	sigsetjmp(jmpbuf, 1);
	jmpflag = 1;

	/* Main command loop. */
	while (vtysh_rl_gets())
		vtysh_execute(line_read);

	history_truncate_file(history_file, 1000);
	printf("\n");

	/* Rest in peace. */
	exit(0);
}
