/*
 * Copyright (c) 1998-2001
 * University of Southern California/Information Sciences Institute.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the project nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE PROJECT AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE PROJECT OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */
/*
 *  $Id: main.c,v 1.19 2003/02/12 21:56:04 pavlin Exp $
 */
/*
 * Part of this program has been derived from mrouted.
 * The mrouted program is covered by the license in the accompanying file
 * named "LICENSE.mrouted".
 *
 * The mrouted program is COPYRIGHT 1989 by The Board of Trustees of
 * Leland Stanford Junior University.
 *
 */


#include "defs.h"
#include <err.h>
#include <getopt.h>
#include <sys/stat.h>
#ifndef CTC_RAW_DEBUG
#include <sys/ipc.h>
#include <sys/msg.h>
#define IPC_KEY  0x20021203

struct message{
    long msg_type;
	char msgtext[100];
};
#endif

char versionstring[100];
int do_vifs       = 1;
int retry_forever = 0;
int haveterminal  = 1;
struct rp_hold *g_rp_hold = NULL;
int mrt_table_id = 0;

char *ident       = PACKAGE_NAME;
char *prognm      = NULL;
char *pid_file    = NULL;
char *config_file = NULL;

extern int loglevel;

static int sighandled = 0;
#define GOT_SIGINT      0x01
#define GOT_SIGHUP      0x02
#define GOT_SIGUSR1     0x04
#define GOT_SIGUSR2     0x08
#define GOT_SIGALRM     0x10

#define SW 		"switch#"
#define SW_CFG 	"switch(config)#"
#define SW_INT 	"switch(config-if)#"

#define EX		"exit"
#define CONFT	"configure terminal"
#define INTF	"interface"


#define SW_SHOW_SM_NEI 		"show ip pim sparse-mode neighbor"
#define SW_SHOW_SM_MRO		"show ip pim sparse-mode mroute"
#define SW_SHOW_SM_RP		"show ip pim sparse-mode rp-set"
#define SW_SHOW_SM_RP_HASH		"show ip pim sparse-mode rp-hash"
#define SW_SHOW_IP_IGMP_ADDRESS	"show ip igmp groups"
#define SW_SHOW_IP_IGMP_INTERFACE "show ip igmp interface"
#define SW_SHOW_IP_IGMP_GLOBAL_INFO	"show ip igmp global_info"
#define SW_SHOW_IP_PIM_INTERFACE	"show ip pim sparse-mode interface"

#define INTF_SM				"ip pim sparse-mode"
#define INTF_SM_NO			"no ip pim sparse-mode"

#define GLB_BSR				"bsr-candidate"
#define GLB_RP				"rp-candidate"
#define GLB_BSR_NO			"no bsr-candidate"
#define GLB_RP_NO			"no rp-candidate"
#define GLB_GROUP			"group-prefix"
#define GLB_GROUP_NO		"no group-prefix"
#define GLB_HELLO_INTERVAL  "hello-interval"


#define NHANDLERS       3
static struct ihandler {
    int fd;			/* File descriptor               */
    ihfunc_t func;		/* Function to call with &fd_set */
} ihandlers[NHANDLERS];
static int nhandlers = 0;

static struct debugname {
    char	*name;
    uint32_t	 level;
    size_t	 nchars;
} debugnames[] = {
    {   "dvmrp_detail",	    DEBUG_DVMRP_DETAIL,   5	    },
    {   "dvmrp_prunes",	    DEBUG_DVMRP_PRUNE,    8	    },
    {   "dvmrp_pruning",    DEBUG_DVMRP_PRUNE,    8	    },
    {   "dvmrp_routes",	    DEBUG_DVMRP_ROUTE,    7	    },
    {   "dvmrp_routing",    DEBUG_DVMRP_ROUTE,    7	    },
    {	"dvmrp_mrt",	    DEBUG_DVMRP_ROUTE,	  7	    },
    {	"dvmrp_neighbors",  DEBUG_DVMRP_PEER,	  7	    },
    {	"dvmrp_peers",	    DEBUG_DVMRP_PEER,	  8	    },
    {	"dvmrp_hello",	    DEBUG_DVMRP_PEER,	  7	    },
    {	"dvmrp_timers",	    DEBUG_DVMRP_TIMER,	  7	    },
    {	"dvmrp",	    DEBUG_DVMRP,	  1	    },
    {	"igmp_proto",	    DEBUG_IGMP_PROTO,	  6	    },
    {	"igmp_timers",	    DEBUG_IGMP_TIMER,	  6	    },
    {	"igmp_members",	    DEBUG_IGMP_MEMBER,	  6	    },
    {	"groups",	    DEBUG_MEMBER,	  1	    },
    {	"membership",	    DEBUG_MEMBER,	  2	    },
    {	"igmp",		    DEBUG_IGMP,		  1	    },
    {	"trace",	    DEBUG_TRACE,	  2	    },
    {	"mtrace",	    DEBUG_TRACE,	  2	    },
    {	"traceroute",	    DEBUG_TRACE,	  2	    },
    {	"timeout",	    DEBUG_TIMEOUT,	  2	    },
    {	"callout",	    DEBUG_TIMEOUT,	  3	    },
    {	"packets",	    DEBUG_PKT,		  2	    },
    {	"pkt",		    DEBUG_PKT,		  2	    },
    {	"interfaces",	    DEBUG_IF,		  2	    },
    {	"vif",		    DEBUG_IF,		  1	    },
    {	"kernel",	    DEBUG_KERN,		  2	    },
    {	"cache",	    DEBUG_MFC,		  1	    },
    {	"mfc",		    DEBUG_MFC,		  2	    },
    {	"k_cache",	    DEBUG_MFC,		  2	    },
    {	"k_mfc",	    DEBUG_MFC,		  2	    },
    {	"rsrr",		    DEBUG_RSRR,		  2	    },
    {	"pim_detail",	    DEBUG_PIM_DETAIL,	  5	    },
    {	"pim_hello",	    DEBUG_PIM_HELLO,	  5	    },
    {	"pim_neighbors",    DEBUG_PIM_HELLO,	  5	    },
    {	"pim_peers",	    DEBUG_PIM_HELLO,	  5	    },
    {	"pim_register",	    DEBUG_PIM_REGISTER,	  5	    },
    {	"registers",	    DEBUG_PIM_REGISTER,	  2	    },
    {	"pim_join_prune",   DEBUG_PIM_JOIN_PRUNE, 5	    },
    {	"pim_j_p",	    DEBUG_PIM_JOIN_PRUNE, 5	    },
    {	"pim_jp",	    DEBUG_PIM_JOIN_PRUNE, 5	    },
    {	"pim_bootstrap",    DEBUG_PIM_BOOTSTRAP,  5	    },
    {	"pim_bsr",	    DEBUG_PIM_BOOTSTRAP,  5	    },
    {	"bsr",		    DEBUG_PIM_BOOTSTRAP,  1	    },
    {	"bootstrap",	    DEBUG_PIM_BOOTSTRAP,  1	    },
    {	"pim_asserts",	    DEBUG_PIM_ASSERT,	  5	    },
    {	"pim_cand_rp",	    DEBUG_PIM_CAND_RP,	  5	    },
    {	"pim_c_rp",	    DEBUG_PIM_CAND_RP,	  5	    },
    {	"pim_rp",	    DEBUG_PIM_CAND_RP,	  6	    },
    {	"rp",		    DEBUG_PIM_CAND_RP,	  2	    },
    {	"pim_routes",	    DEBUG_PIM_MRT,	  6	    },
    {	"pim_routing",	    DEBUG_PIM_MRT,	  6	    },
    {	"pim_mrt",	    DEBUG_PIM_MRT,	  5	    },
    {	"pim_timers",	    DEBUG_PIM_TIMER,	  5	    },
    {	"pim_rpf",	    DEBUG_PIM_RPF,	  6	    },
    {	"rpf",		    DEBUG_RPF,		  3	    },
    {	"pim",		    DEBUG_PIM,		  1	    },
    {	"routes",	    DEBUG_MRT,		  1	    },
    {	"routing",	    DEBUG_MRT,		  1	    },
    {	"mrt",		    DEBUG_MRT,		  1	    },
    {	"neighbors",	    DEBUG_NEIGHBORS,	  1	    },
    {	"routers",	    DEBUG_NEIGHBORS,	  6	    },
    {	"mrouters",	    DEBUG_NEIGHBORS,	  7	    },
    {	"peers",	    DEBUG_NEIGHBORS,	  1	    },
    {	"timers",	    DEBUG_TIMER,	  1	    },
    {	"asserts",	    DEBUG_ASSERT,	  1	    },
    {	"all",		    DEBUG_ALL,		  2	    },
    {	"3",		    0xffffffff,		  1	    }	 /* compat. */
};


/*
 * Forward declarations.
 */
static void handler      (int);
static void timer	 (void *);
static void cleanup      (void);
static void restart      (int);
static void resetlogging (void *);
static void add_static_rp(void);

int register_input_handler(int fd, ihfunc_t func)
{
    if (nhandlers >= NHANDLERS)
	return -1;

    ihandlers[nhandlers].fd = fd;
    ihandlers[nhandlers++].func = func;

    return 0;
}

static void do_randomize(void)
{
#define rol32(data,shift) ((data) >> (shift)) | ((data) << (32 - (shift)))
   int fd;
   unsigned int seed;

   /* Setup a fallback seed based on quasi random. */
#ifdef SYSV
   seed = time(NULL);
#else
   seed = time(NULL) ^ gethostid();
#endif
   seed = rol32(seed, seed);

   fd = open("/dev/urandom", O_RDONLY);
   if (fd >= 0) {
       if (-1 == read(fd, &seed, sizeof(seed)))
	   warn("Failed reading entropy from /dev/urandom");
       close(fd);
  }

#ifdef SYSV
   srand48(seed);
#else
   srandom(seed);
#endif
}

static int compose_paths(void)
{
    /* Default .conf file path: "/etc" + '/' + "pimd" + ".conf" */
    if (!config_file) {
	size_t len = strlen(SYSCONFDIR) + strlen(ident) + 7;

	config_file = malloc(len);
	if (!config_file)
	    logit(LOG_ERR, errno, "Failed allocating memory, exiting.");

	snprintf(config_file, len, _PATH_PIMD_CONF, ident);
    }

    /* Default is to let pidfile() API construct PID file from ident */
    if (!pid_file)
	pid_file = strdup(ident);

    return 0;
}

static int usage(int code)
{
    size_t i;
    char line[76] = "  ";
    char pidfn[80];
    struct debugname *d;

    compose_paths();
    if (pid_file && pid_file[0] != '/')
	snprintf(pidfn, sizeof(pidfn), "%s/%s.pid", _PATH_PIMD_RUNDIR, pid_file);
    else
	snprintf(pidfn, sizeof(pidfn), "%s", pid_file);

    printf("Usage: %s [-DhlNnv] [-f FILE] [-d [SYS][,SYS...]] [-s LEVEL]\n\n", prognm);
    printf(" -f, --config=FILE   Configuration file, default uses ident NAME: %s\n", config_file);
    printf(" -n, --foreground    Run in foreground, do not detach from calling terminal\n");
    printf(" -d SYS[,SYS,SYS..]  Enable debug for SYS, see below for valid systems\n");
    printf(" -l, --loglevel=LVL  Set log level: none, err, info, notice (default), debug\n");
    printf(" -I, --ident=NAME    Identity for config + PID file, and syslog, default: %s\n", ident);
    printf(" -N, --disable-vifs  Disable all virtual interfaces (phyint) by default\n");
    printf(" -P, --pidfile=FILE  File to store process ID for signaling %s\n"
	   "                     Default uses ident NAME: %s\n", prognm, pidfn);
    printf(" -r                  Retry (forever) if not all phyint interfaces are available\n"
	   "                     yet when starting up, e.g. wait for DHCP lease\n");
    printf(" -s, --syslog        Use syslog, default unless running in foreground, -n\n");
    printf(" -t, --table-id=ID   Set multicast routing table ID.  Allowed table ID#:\n"
	   "                      0 .. 999999999.  Default: 0 (use default table)\n");
    printf(" -h, --help          Show this help text\n");
    printf(" -v, --version       Show %s version\n", prognm);
    printf("\n");

    /* From pimd v2.3.0 we show *all* the debug levels again */
    printf("Available subsystems for debug:\n");
    for (i = 0, d = debugnames; i < ARRAY_LEN(debugnames); i++, d++) {
	if (strlen(line) + strlen(d->name) + 3 >= sizeof(line)) {
	    /* Finish this line and send to console */
	    strlcat(line, "\n", sizeof(line));
	    printf("%s", line);

	    /* Prepare for next line */
	    strlcpy(line, "  ", sizeof(line));
	}

	strlcat(line, d->name, sizeof(line));

	if (i + 1 < ARRAY_LEN(debugnames))
	    strlcat(line, ", ", sizeof(line));
    }
    /* Flush remaining line. */
    strlcat(line, "\n", sizeof(line));
    printf("%s", line);

    printf("\nBug report address: %-40s\n", PACKAGE_BUGREPORT);
#ifdef PACKAGE_URL
    printf("Project homepage: %s\n", PACKAGE_URL);
#endif

    return code;
}

static char *progname(char *arg0)
{
       char *nm;

       nm = strrchr(arg0, '/');
       if (nm)
	       nm++;
       else
	       nm = arg0;

       return nm;
}

int 
sm_ctl_cmd(char *cli_config,char *cmd_str,char *eth)
{
	if ((0 == strcmp(cli_config,SW)) && (0 == strcmp(cmd_str,CONFT)))
	{
		strcpy(cli_config,SW_CFG);
		return 1;
	}

	if (0 == strcmp(cli_config,SW_CFG))
	{
		if (0 == strncmp(cmd_str,INTF,9))
		{
			strcpy(cli_config,SW_INT);
			strcpy(eth,cmd_str+10);
			return 1;
		}
		else if (0 == strcmp(cmd_str,EX))
		{
			strcpy(cli_config,SW);
			return 1;
		}
	}

	if ((0 == strcmp(cli_config,SW_INT)) && (0 == strcmp(cmd_str,EX)))
	{
		strcpy(cli_config,SW_CFG);
		return 1;
	}	

	return 0;
}

int 
sm_sw_cmd(char *cmd_str)
{
    int ret = -1 ;
    FILE *fp;
	fp = fopen("/tmp/pimshow.txt", "w");
	if (0 == strcmp(cmd_str,SW_SHOW_SM_NEI))
	{
		dump_vifs_new(fp);
		ret = 0;
	}
	else if (0 == strcmp(cmd_str,SW_SHOW_SM_MRO))
	{
		dump_pim_mrt_new(fp);
		ret = 0;		
	}
	else if (0 == strcmp(cmd_str,SW_SHOW_SM_RP))
	{
		dump_rp_set(fp);
		ret = 0;		
	}
	else if (0 == strncmp(cmd_str,SW_SHOW_SM_RP_HASH,strlen(SW_SHOW_SM_RP_HASH)))
	{
		dump_rp_hash(fp,cmd_str);
		ret = 0;			
	}
	else if (0 == strncmp(cmd_str, SW_SHOW_IP_IGMP_ADDRESS, strlen(SW_SHOW_IP_IGMP_ADDRESS)))
	{
		dump_igmp_groups(fp, cmd_str);
		ret = 0;
	}
	else if (0 == strncmp(cmd_str, SW_SHOW_IP_IGMP_INTERFACE, strlen(SW_SHOW_IP_IGMP_INTERFACE)))
	{
		dump_igmp_interface(fp, cmd_str);
		ret = 0;
	}
	else if (0 == strncmp(cmd_str, SW_SHOW_IP_IGMP_GLOBAL_INFO, strlen(SW_SHOW_IP_IGMP_GLOBAL_INFO)))
	{
		dump_igmp_global_config(fp, cmd_str);
		ret = 0;
	}
	else if (0 == strncmp(cmd_str, SW_SHOW_IP_PIM_INTERFACE, strlen(SW_SHOW_IP_PIM_INTERFACE)))
	{
		dump_pim_interface(fp, cmd_str);
		ret = 0;
	}
	fclose(fp);
	return ret;
}

int
sm_intf_cmd(char *cmd_str,char *eth)
{
	char str_on[50];
	char str_off[50];
	if (0 == strcmp(cmd_str,INTF_SM))
	{
		sprintf(str_on,"ip link set %s multicast on",eth);
		system(str_on);
	}
	else if (0 == strcmp(cmd_str,INTF_SM_NO))
	{
		sprintf(str_off,"ip link set %s multicast off",eth);
		system(str_off);
	}
	return 0;
}



#if 0
int 
sm_glb_cmd(char *cmd_str)
{
	char *w, *s;
	s = cmd_str;
	w = glb_next_word(&s);
	if (0 == strncmp(cmd_str,GLB_BSR,strlen(GLB_BSR)))
	{
		parse_bsr_candidate(s);
		if (cand_bsr_flag != FALSE) {
			my_bsr_timeout = 2 * my_bsr_adv_period + 10;   /* RFC5059 section 5 */
		} else {
			/* set a sensible default to check and compute RPs holdtime */
			my_bsr_adv_period = PIM_BOOTSTRAP_PERIOD;
		}

		recommended_rp_holdtime = 2.5 * my_bsr_adv_period; /* RFC5059 section 3.3 SHOULD BE value */
	}
	else if (0 == strncmp(cmd_str,GLB_RP,strlen(GLB_RP)))
	{
		parse_rp_candidate(s);	
	}
	else if (0 == strncmp(cmd_str,GLB_BSR_NO,strlen(GLB_BSR_NO)))
	{
		cand_bsr_flag = FALSE;
	    SET_TIMER(pim_bootstrap_timer, my_bsr_timeout);
	    curr_bsr_fragment_tag = 0;
	    curr_bsr_priority     = 0;		  /* Lowest priority */
	    curr_bsr_address      = INADDR_ANY_N; /* Lowest priority */
	    MASKLEN_TO_MASK(RP_DEFAULT_IPV4_HASHMASKLEN, curr_bsr_hash_mask);
	}
	else if (0 == strncmp(cmd_str,GLB_RP_NO,strlen(GLB_RP_NO)))
	{
		cand_rp_flag = FALSE;
	}
	else if (0 == strncmp(cmd_str,GLB_GROUP,strlen(GLB_GROUP)))
	{
		uint8_t *data_ptr;
		parse_group_prefix(s);
		cand_rp_adv_message.message_size = cand_rp_adv_message.insert_data_ptr - cand_rp_adv_message.buffer;
		if (cand_rp_flag != FALSE) {
			/* Prepare the RP info */
			my_cand_rp_holdtime = 2.5 * my_cand_rp_adv_period;
			/* Is holdtime in MUST BE interval? (RFC5059 section 3.3) */
			if (my_cand_rp_holdtime <= my_bsr_adv_period)
					my_cand_rp_holdtime = recommended_rp_holdtime;
			
			/* TODO: HARDCODING! */
			data_ptr = cand_rp_adv_message.buffer + 1;
			PUT_BYTE(my_cand_rp_priority, data_ptr);
			PUT_HOSTSHORT(my_cand_rp_holdtime, data_ptr);
			PUT_EUADDR(my_cand_rp_address, data_ptr);
		}
	}
	else if (0 == strncmp(cmd_str,GLB_GROUP_NO,strlen(GLB_GROUP_NO)))
	{
		parse_group_prefix("224.0.0.0 masklen 4");
	}
	else if (0 == strncmp(cmd_str,GLB_HELLO_INTERVAL,strlen(GLB_HELLO_INTERVAL)))
	{

	    parse_hello_interval(s);
	}	
	return 0;
}

#else 
int 
sm_glb_cmd(char *cmd_str)
{
    char *w, *s;
	FILE *fp;
	fp = fopen("/tmp/pimshow.txt", "w");
	
	s = cmd_str;
	w = glb_next_word(&s);

    printf("sm_glb_cmd:%s  ----- w=%s   num = %d\n",cmd_str,w,parse_option(w));
	switch (parse_option(w)) {
		case CONF_EMPTY:
		break;

		case CONF_PHYINT:
		parse_phyint(s);
		break;

		case CONF_DISABLE_VIFS:
		/* Ignore, handled in first stage */
		break;

		case CONF_CANDIDATE_RP:
		parse_rp_candidate(s);
		break;

		case CONF_RP_ADDRESS:
		parse_rp_address(s);
		add_static_rp();
		break;

		case CONF_GROUP_PREFIX:
		{
			uint8_t *data_ptr;
			parse_group_prefix(s, fp);
			cand_rp_adv_message.message_size = cand_rp_adv_message.insert_data_ptr - cand_rp_adv_message.buffer;
			if (cand_rp_flag != FALSE) {
				/* Prepare the RP info */
				my_cand_rp_holdtime = 2.5 * my_cand_rp_adv_period;
				/* Is holdtime in MUST BE interval? (RFC5059 section 3.3) */
				if (my_cand_rp_holdtime <= my_bsr_adv_period)
						my_cand_rp_holdtime = recommended_rp_holdtime;
				
				/* TODO: HARDCODING! */
				data_ptr = cand_rp_adv_message.buffer + 1;
				PUT_BYTE(my_cand_rp_priority, data_ptr);
				PUT_HOSTSHORT(my_cand_rp_holdtime, data_ptr);
				PUT_EUADDR(my_cand_rp_address, data_ptr);
				/*send rp-cand-adv*/
				send_pim_cand_rp_adv();
			}
		}

		break;

		case CONF_BOOTSTRAP_RP:
		{
			parse_bsr_candidate(s);
			if (cand_bsr_flag != FALSE) {
				my_bsr_timeout = 2 * my_bsr_adv_period + 10;   /* RFC5059 section 5 */
			} else {
				/* set a sensible default to check and compute RPs holdtime */
				my_bsr_adv_period = PIM_BOOTSTRAP_PERIOD;
			}

			recommended_rp_holdtime = 2.5 * my_bsr_adv_period; /* RFC5059 section 3.3 SHOULD BE value */
		}
		break;

		case CONF_SPT_THRESHOLD:
		parse_spt_threshold(s);
		break;

		case CONF_DEFAULT_ROUTE_METRIC:
		parse_default_route_metric(s);
		break;

		case CONF_DEFAULT_ROUTE_DISTANCE:
		parse_default_route_distance(s);
		break;

		case CONF_IGMP_QUERY_INTERVAL:
		parse_igmp_query_interval(s);
		break;

		case CONF_IGMP_QUERIER_TIMEOUT:
		parse_igmp_querier_timeout(s);
		break;

		case CONF_HELLO_INTERVAL:
		parse_hello_interval(s);
		break;
		
		case CONF_JOIN_PRUNE_PERIOD:
		parse_join_prune_period(s);	
		break;
		
		case CONF_DEBUG_LOG:
		parse_debug_log(s);	
		break;
		
		case CONF_LAST_MEMBER_QUERY_COUNT:
		parse_last_member_query_count(s);
		break;
		
		case CONF_LAST_MEMBER_QUERY_INTERVAL:
		parse_last_member_query_interval(s);
		break;
		
		case CONF_ROBUSTNESS_VARIABLE:
		parse_robustness_variable(s);
		break;
		
		case CONF_MULTICAST:
		parse_multicast(s);
		break;
		
		case GLOBAL_LIMIT_NUMBER:
		parse_global_limit_number(s);
		break;
		
		case INTERFACE_LIMIT_NUMBER:
		parse_interface_limit_number(s);
		break;

		case NO_CONFIG:
		parse_no_config(s);
		break;
		
		default:
		break;
	}
	fclose(fp);
    return 0;
}


#endif



void
str_ch(char *cmd_str)
{
	char *str = cmd_str;
	if (strlen(str) != 49)
		str[strlen(str)-1] = '\0';
}	

void *cmd_process_fun()
{	
	int ret = 0;
	struct message msgq;
	msgq.msg_type = 9527;
	char cmd_str[100];
	int msgid = msgget(IPC_KEY, IPC_CREAT|0666);
	if(msgid == -1)
		return NULL;
	
	while(1)
	{
		//log_sys(M_MOD_PIMSM,	E_NOTICE,"zhw test for cmd_process_fun, msgid=%d\n", msgid);
		ret = msgrcv(msgid, &msgq, 100, 9527, 0);
		if(ret < 0)
		{
	  		 //fprintf(stdout,"rev error \n");          
		}
		else{
			IF_DEBUG(DEBUG_IGMP)
				log_sys(M_MOD_PIMSM,	E_NOTICE,"msgq.msgtext is : %s len=%d\n", msgq.msgtext, strlen(msgq.msgtext));				
		    strcpy(cmd_str,msgq.msgtext);
			if(strncmp(cmd_str, "show", 4) == 0){
				ret = sm_sw_cmd(cmd_str);
				if(0 == ret){	
		    		msgq.msg_type = 1234;						
					strcpy(msgq.msgtext, "OK");
					msgsnd(msgid, &msgq, 100,0);
				}	
			} else
				sm_glb_cmd(cmd_str);				
            
				//fprintf(stdout,"file write ok");				
		}
   }
}

int main(int argc, char *argv[])
{
	if (freopen("/dev/null","w",stderr) == NULL)
	{
		fprintf(stdout,"error redirecting sterr\n");
	}
    int dummysigalrm, foreground = 0, do_syslog = 1;
    struct timeval tv, difftime, curtime, lasttime, *timeout;
    fd_set rfds, readers;
    int nfds, n, i, secs, ch;
    struct sigaction sa;
	pthread_t th;
	pthread_t tid_cmd;
	
    struct option long_options[] = {
	{ "config",        1, 0, 'f' },
	{ "disable-vifs",  0, 0, 'N' },
	{ "foreground",    0, 0, 'n' },
	{ "help",          0, 0, 'h' },
	{ "ident",         1, 0, 'I' },
	{ "loglevel",      1, 0, 'l' },
	{ "pidfile",       1, 0, 'P' },
	{ "syslog",        0, 0, 's' },
	{ "table-id",      1, 0, 't' },
	{ "version",       0, 0, 'v' },
	{ NULL, 0, 0, 0 }
    };
#if 1
		char cli_config[20];
		char cmd_str[100];
		char eth[20];
		int ret  = 0;
		strcpy(cli_config,SW);
	
		int msgid = msgget(IPC_KEY, IPC_CREAT|0666);
		struct message msgq;
		msgq.msg_type = 9527;	
#endif	

	
	//log_sys(M_MOD_PIMSM,	E_NOTICE, "hello world\n");
    log_sys(M_MOD_PIMSM, E_ERROR, "logit test starting ...\n");


    snprintf(versionstring, sizeof (versionstring), "pimd version %s", PACKAGE_VERSION);

    prognm = ident = progname(argv[0]);
    while ((ch = getopt_long(argc, argv, "d:f:hI:l:nNP:rst:v", long_options, NULL)) != EOF) {
	const char *errstr;

	switch (ch) {
	    case 'd':
		{
		    char *p,*q;
		    size_t i, len;
		    struct debugname *d;

		    debug = 0;
		    p = optarg;
		    q = NULL;
		    while (p) {
			q = strchr(p, ',');
			if (q)
			    *q++ = '\0';
			len = strlen(p);
			for (i = 0, d = debugnames; i < ARRAY_LEN(debugnames); i++, d++) {
			    if (len >= d->nchars && strncmp(d->name, p, len) == 0)
				break;
			}

			if (i == ARRAY_LEN(debugnames))
			    return usage(1);

			debug |= d->level;
			p = q;
		    }
		}
		break;

	    case 'f':
		config_file = optarg;
		break;

	    case 'h':
		return usage(0);

	    case 'I':	/* --ident=NAME */
		ident = optarg;
		break;

	    case 'l':
		loglevel = loglvl(optarg);
		if (-1 == loglevel)
		    return usage(1);
		break;

	    case 'n':
		do_syslog--;
		foreground = 1;
		break;

	    case 'N':
		do_vifs = 0;
		break;

	    case 'P':	/* --pidfile=NAME */
		pid_file = strdup(optarg);
		break;

	    case 'r':
		retry_forever++;
		break;

	    case 's':
		do_syslog++;
		break;

	    case 't':
		mrt_table_id = strtonum(optarg, 0, 999999999, &errstr);
		if (errstr) {
		    fprintf(stderr, "Table ID %s!\n", errstr);
		    return usage(1);
		}
		break;

	    case 'v':
		printf("%s\n", versionstring);
		return 0;

	    default:
		return usage(1);
	}
    }

    argc -= optind;
    if (argc > 0)
	return usage(1);

    if (geteuid() != 0)
	errx(1, "Need root privileges to start.");

    compose_paths();
    setlinebuf(stderr);

    if (debug != 0) {
	struct debugname *d;
	char c;
	int tmpd = debug;

	fprintf(stderr, "debug level 0x%lx ", debug);
	c = '(';
	for (d = debugnames; d < debugnames + ARRAY_LEN(debugnames); d++) {
	    if ((tmpd & d->level) == d->level) {
		tmpd &= ~d->level;
		fprintf(stderr, "%c%s", c, d->name);
		c = ',';
	    }
	}
	fprintf(stderr, ")\n");
    }

    if (!debug && !foreground) {
	/* Detach from the terminal */
	haveterminal = 0;
	if (fork())
	    exit(0);

	close(STDIN_FILENO);
	close(STDOUT_FILENO);
	close(STDERR_FILENO);

	n = open("/dev/null", O_RDWR, 0);
	if (n >= 0) {
	    dup2(n, STDIN_FILENO);
	    dup2(n, STDOUT_FILENO);
	    dup2(n, STDERR_FILENO);
	}
#ifdef SYSV
	setpgrp();
#else
#ifdef TIOCNOTTY
	n = open("/dev/tty", 2);
	if (n >= 0) {
	    (void)ioctl(n, TIOCNOTTY, (char *)0);
	    (void)close(n);
	}
#else
	if (setsid() < 0)
	    perror("setsid");
#endif /* TIOCNOTTY */
#endif /* SYSV */
    } /* End of child process code */

    /*
     * Create directory for runtime files
     */
    if (-1 == mkdir(_PATH_PIMD_RUNDIR, 0755) && errno != EEXIST)
	err(1, "Failed creating %s directory for runtime files", _PATH_PIMD_RUNDIR);

/*add by whs for remove old log*/
#if 1
    /*
     * Setup logging
     */
    log_init(haveterminal && do_syslog > 0);
#endif	
    logit(LOG_NOTICE, 0, "%s starting ...", versionstring);

	/*init g_rp_hold for static rp-address*/
    g_rp_hold = calloc(1, sizeof(struct rp_hold));
	if (!g_rp_hold) {
		logit(LOG_WARNING, 0, "Out of memory when colloc g_rp_hold");
		return FALSE;
    }
	
    do_randomize();

    callout_init();
    init_igmp();
    init_pim();
    init_routesock(); /* Both for Linux netlink and BSD routing socket */
    init_pim_mrt();
    init_timers();
/*add by whs for remove old log*/
#if 1
    /* Start up the log rate-limiter */
    resetlogging(NULL);
#endif
    /* TODO: check the kernel DVMRP/MROUTED/PIM support version */

    init_vifs();
	
    init_rp_and_bsr();   /* Must be after init_vifs() */
    add_static_rp();	 /* Must be after init_vifs() */
#ifdef RSRR
    rsrr_init();
#endif /* RSRR */

    sa.sa_handler = handler;
    sa.sa_flags = 0;	/* Interrupt system calls */
    sigemptyset(&sa.sa_mask);
    sigaction(SIGALRM, &sa, NULL);
    sigaction(SIGHUP, &sa, NULL);
    sigaction(SIGTERM, &sa, NULL);
    sigaction(SIGINT, &sa, NULL);
    sigaction(SIGUSR1, &sa, NULL);
    sigaction(SIGUSR2, &sa, NULL);

    FD_ZERO(&readers);
    FD_SET(igmp_socket, &readers);
    nfds = igmp_socket + 1;
    for (i = 0; i < nhandlers; i++) {
	FD_SET(ihandlers[i].fd, &readers);
	if (ihandlers[i].fd >= nfds)
	    nfds = ihandlers[i].fd + 1;
    }

    IF_DEBUG(DEBUG_IF)
	dump_vifs(stderr);
    IF_DEBUG(DEBUG_PIM_MRT)
	dump_pim_mrt(stderr);

    /* schedule first timer interrupt */
    timer_setTimer(TIMER_INTERVAL, timer, NULL);

    if (pidfile(pid_file))
	warn("Cannot create pidfile");

    /*
     * Main receive loop.
     */
    dummysigalrm = SIGALRM;
    difftime.tv_usec = 0;
    gettimeofday(&curtime, NULL);
    lasttime = curtime;
	
	pimsm_netlink_init();

	/*add by zhw: no wait for show; cmd_process_fun is a dead loop, so do not use pthread_join*/
	if(pthread_create(&tid_cmd, NULL, cmd_process_fun, NULL) != 0)
	{
		return -1;
	}
    while (1) {
	memcpy(&rfds, &readers, sizeof(rfds));
	secs = timer_nextTimer();
	if (secs == -1)
	    timeout = NULL;
	else {
	    timeout = &tv;
	    timeout->tv_sec = secs;
	    timeout->tv_usec = 0;
	}
	if (sighandled) {
	    if (sighandled & GOT_SIGINT) {
		sighandled &= ~GOT_SIGINT;

		/*break; delete by whs for sigint no action*/
	    }
	    if (sighandled & GOT_SIGHUP) {
		sighandled &= ~GOT_SIGHUP;
#if 0		
		restart(SIGHUP);

		/* reconstruct readers and nfds */
		FD_ZERO(&readers);
		FD_SET(igmp_socket, &readers);
		nfds = igmp_socket + 1;
		for (i = 0; i < nhandlers; i++) {
		    FD_SET(ihandlers[i].fd, &readers);
		    if (ihandlers[i].fd >= nfds)
			nfds = ihandlers[i].fd + 1;
		}
		memcpy(&rfds, &readers, sizeof(rfds));
#endif
		}
	    if (sighandled & GOT_SIGUSR1) {
		sighandled &= ~GOT_SIGUSR1;
		fdump(_PATH_PIMD_DUMP);
	    }
	    if (sighandled & GOT_SIGUSR2) {
		sighandled &= ~GOT_SIGUSR2;
		cdump(_PATH_PIMD_CACHE);
	    }
	    if (sighandled & GOT_SIGALRM) {
		sighandled &= ~GOT_SIGALRM;
		timer(&dummysigalrm);
	    }
	}
	if ((n = select(nfds, &rfds, NULL, NULL, timeout)) < 0) {
	    if (errno != EINTR) /* SIGALRM is expected */
		logit(LOG_WARNING, errno, "select failed");
	    continue;
	}
	
	if (n > 0) {
	    /* TODO: shall check first igmp_socket for better performance? */
	    for (i = 0; i < nhandlers; i++) {
		if (FD_ISSET(ihandlers[i].fd, &rfds)) {
		    (*ihandlers[i].func)(ihandlers[i].fd, &rfds);
		}
	    }
	}

	/*
	 * Handle timeout queue.
	 *
	 * If select + packet processing took more than 1 second,
	 * or if there is a timeout pending, age the timeout queue.
	 *
	 * If not, collect usec in difftime to make sure that the
	 * time doesn't drift too badly.
	 *
	 * If the timeout handlers took more than 1 second,
	 * age the timeout queue again.  XXX This introduces the
	 * potential for infinite loops!
	 */
	do {
	    /*
	     * If the select timed out, then there's no other
	     * activity to account for and we don't need to
	     * call gettimeofday.
	     */
	    if (n == 0) {
		curtime.tv_sec = lasttime.tv_sec + secs;
		curtime.tv_usec = lasttime.tv_usec;
		n = -1;	/* don't do this next time through the loop */
	    } else
			gettimeofday(&curtime, NULL);
	    difftime.tv_sec = curtime.tv_sec - lasttime.tv_sec;
	    difftime.tv_usec += curtime.tv_usec - lasttime.tv_usec;
	    while (difftime.tv_usec >= 1000000) {
		difftime.tv_sec++;
		difftime.tv_usec -= 1000000;
	    }
	    if (difftime.tv_usec < 0) {
		difftime.tv_sec--;
		difftime.tv_usec += 1000000;
	    }
	    lasttime = curtime;
	    if (secs == 0 || difftime.tv_sec > 0)
		age_callout_queue(difftime.tv_sec);
	    secs = -1;
	} while (difftime.tv_sec > 0);
	
	pim_netlink_handler();
    } /* Main loop */

    logit(LOG_NOTICE, 0, "%s exiting.", versionstring);
    cleanup();
    exit(0);
}

/*
 * The 'virtual_time' variable is initialized to a value that will cause the
 * first invocation of timer() to send a probe or route report to all vifs
 * and send group membership queries to all subnets for which this router is
 * querier.  This first invocation occurs approximately TIMER_INTERVAL seconds
 * after the router starts up.   Note that probes for neighbors and queries
 * for group memberships are also sent at start-up time, as part of initial-
 * ization.  This repetition after a short interval is desirable for quickly
 * building up topology and membership information in the presence of possible
 * packet loss.
 *
 * 'virtual_time' advances at a rate that is only a crude approximation of
 * real time, because it does not take into account any time spent processing,
 * and because the timer intervals are sometimes shrunk by a random amount to
 * avoid unwanted synchronization with other routers.
 */

uint32_t virtual_time = 0;

/*
 * Timer routine. Performs all perodic functions:
 * aging interfaces, quering neighbors and members, etc... The granularity
 * is equal to TIMER_INTERVAL.
 */
static void timer(void *i __attribute__((unused)))
{
    age_vifs();		/* Timeout neighbors and groups         */
    age_routes();	/* Timeout routing entries              */
    age_misc();		/* Timeout the rest (Cand-RP list, etc) */

    virtual_time += TIMER_INTERVAL;
    timer_setTimer(TIMER_INTERVAL, timer, NULL);
}

/*
 * Performs all necessary functions to quit gracefully
 */
/* TODO: implement all necessary stuff */
static void cleanup(void)
{
    vifi_t vifi;
    struct uvif *v;

    /* inform all neighbors that I'm going to die */
    for (vifi = 0, v = uvifs; vifi < numvifs; ++vifi, ++v) {
	if ((v->uv_flags &
	     (VIFF_DOWN | VIFF_DISABLED | VIFF_REGISTER | VIFF_TUNNEL)) == 0)
	    send_pim_hello(v, 0);
    }

#ifdef RSRR
    rsrr_clean();
#endif /* RSRR */

    /* TODO: XXX (not in the spec): if I am the BSR, somehow inform the
     * other routers I am going down and need to elect another BSR?
     * (probably by sending a the Cand-RP-set with my_priority=LOWEST?)
     */

    k_stop_pim(igmp_socket);
}


/*
 * Signal handler.  Take note of the fact that the signal arrived
 * so that the main loop can take care of it.
 */
static void handler(int sig)
{
    switch (sig) {
    case SIGALRM:
	sighandled |= GOT_SIGALRM;
	break;

    case SIGINT:
    case SIGTERM:
	sighandled |= GOT_SIGINT;
	break;

    case SIGHUP:
	sighandled |= GOT_SIGHUP;
	break;

    case SIGUSR1:
	sighandled |= GOT_SIGUSR1;
	break;

    case SIGUSR2:
	sighandled |= GOT_SIGUSR2;
	break;
    }
}

static void add_static_rp(void)
{
	vifi_t           vifi;
    struct uvif     *v;
    struct rp_hold *rph = g_rp_hold;
    if(rph){
		add_rp_grp_entry(&cand_rp_list, &grp_mask_list,
			 rph->address, 1, (uint16_t)0xffffff,
			 rph->group, rph->mask,
			 curr_bsr_hash_mask, curr_bsr_fragment_tag);
		/*when change rp-address, send query to immediately rebuild MRT*/
		for (vifi = 0, v = uvifs; vifi < numvifs; ++vifi, ++v) {
			if (v->uv_flags & (VIFF_DISABLED | VIFF_DOWN | VIFF_REGISTER))
	    		continue;
			 query_groups(v);
		}
	}
}

void del_static_rp(void)
{
    struct rp_hold *rph = g_rp_hold;
    if(rph)
		delete_rp(&cand_rp_list, &grp_mask_list, rph->address);
	g_rp_hold->address=0;
	g_rp_hold->group=0;
	g_rp_hold->mask=0;
}

/* TODO: not verified */
/*
 * Restart the daemon
 */
static void restart(int i __attribute__((unused)))
{
    logit(LOG_NOTICE, 0, "%s restarting ...", versionstring);

    /*
     * reset all the entries
     */
    /* TODO: delete?
       free_all_routes();
    */
    del_static_rp();
    free_all_callouts();
    stop_all_vifs();
    k_stop_pim(igmp_socket);
    nhandlers = 0;
    close(igmp_socket);
    close(pim_socket);

    /*
     * When IOCTL_OK_ON_RAW_SOCKET is defined, 'udp_socket' is equal
     * 'to igmp_socket'. Therefore, 'udp_socket' should be closed only
     * if they are different.
     */
#ifndef IOCTL_OK_ON_RAW_SOCKET
    close(udp_socket);
#endif

    /* Both for Linux netlink and BSD routing socket */
    close(routing_socket);

    /*
     * start processing again
     */

    init_igmp();
    init_pim();
    init_routesock(); /* Both for Linux netlink and BSD routing socket */
    init_pim_mrt();
    init_vifs();
    add_static_rp();	 /* Must be after init_vifs() */

    /* Touch PID file to acknowledge SIGHUP */
    pidfile(pid_file);

    /* schedule timer interrupts */
    timer_setTimer(TIMER_INTERVAL, timer, NULL);
}


static void resetlogging(void *arg)
{
    int nxttime = 60;
    void *narg = NULL;

    if (arg == NULL && log_nmsgs >= LOG_MAX_MSGS) {
	nxttime = LOG_SHUT_UP;
	narg = (void *)&log_nmsgs;	/* just need some valid void * */
	syslog(LOG_WARNING, "logging too fast, shutting up for %d minutes",
	       LOG_SHUT_UP / 60);
    } else {
	if (arg != NULL) {
	    syslog(LOG_NOTICE, "logging enabled again after rate limiting");
	}
	log_nmsgs = 0;
    }

    timer_setTimer(nxttime, resetlogging, narg);
}

/**
 * Local Variables:
 *  indent-tabs-mode: t
 *  c-file-style: "ellemtel"
 *  c-basic-offset: 4
 * End:
 */
