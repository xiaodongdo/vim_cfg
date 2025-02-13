
#ifndef __TBL_ROUTE_IF_DEFINE_H__
#define __TBL_ROUTE_IF_DEFINE_H__

#include "gen/ds_connected_define.h"
#include "gen/ds_connected_v6_define.h"
#include "gen/ds_ospf_auth_define.h"
#include "gen/ds_nd_prefix_define.h"

/* TBL_ROUTE_IF field defines */
typedef enum
{
    TBL_ROUTE_IF_FLD_KEY                  = 0 ,  /* READ */
    TBL_ROUTE_IF_FLD_MTU                  = 1 ,  /* RW */
    TBL_ROUTE_IF_FLD_MAC                  = 2 ,  /* RW */
    TBL_ROUTE_IF_FLD_MODE_SET_BEFORE      = 3 ,  /* RW */
    TBL_ROUTE_IF_FLD_IPV4_ENABLE          = 4 ,  /* RW */
    TBL_ROUTE_IF_FLD_IPV6_ENABLE          = 5 ,  /* RW */
    TBL_ROUTE_IF_FLD_ARP_RETRANS_TIME     = 6 ,  /* RW */
    TBL_ROUTE_IF_FLD_ARP_TIMEOUT          = 7 ,  /* RW */
    TBL_ROUTE_IF_FLD_IFINDEX              = 8 ,  /* READ */
    TBL_ROUTE_IF_FLD_KERNEL_IFINDEX       = 9 ,  /* READ */
    TBL_ROUTE_IF_FLD_ARP_RETRY_TIMEOUT    = 10,  /* RW */
    TBL_ROUTE_IF_FLD_RIP_RECV_VERSION     = 11,  /* RW */
    TBL_ROUTE_IF_FLD_RIP_SEND_VERSION     = 12,  /* RW */
    TBL_ROUTE_IF_FLD_RIP_V2_BROADCAST     = 13,  /* RW */
    TBL_ROUTE_IF_FLD_RIP_SPLIT_HORIZON    = 14,  /* RW */
    TBL_ROUTE_IF_FLD_RIP_POISONED_REVERSE = 15,  /* RW */
    TBL_ROUTE_IF_FLD_RIP_AUTH_MODE        = 16,  /* RW */
    TBL_ROUTE_IF_FLD_RIP_AUTH_STRING      = 17,  /* RW */
    TBL_ROUTE_IF_FLD_RIPNG_SPLIT_HORIZON  = 18,  /* RW */
    TBL_ROUTE_IF_FLD_RIPNG_POISONED_REVERSE = 19,  /* RW */
    TBL_ROUTE_IF_FLD_OSPF_PRIORITY        = 20,  /* RW */
    TBL_ROUTE_IF_FLD_OSPF_COST            = 21,  /* RW */
    TBL_ROUTE_IF_FLD_OSPF_MTU_IGNORE      = 22,  /* RW */
    TBL_ROUTE_IF_FLD_OSPF_TIMER_HELLO     = 23,  /* RW */
    TBL_ROUTE_IF_FLD_OSPF_TIMER_DEAD      = 24,  /* RW */
    TBL_ROUTE_IF_FLD_OSPF_AUTH_TYPE       = 25,  /* RW */
    TBL_ROUTE_IF_FLD_OSPF_AUTH_KEY        = 26,  /* RW */
    TBL_ROUTE_IF_FLD_OSPF_NETWORK_TYPE    = 27,  /* RW */
    TBL_ROUTE_IF_FLD_OSPF6_TIMER_HELLO    = 28,  /* RW */
    TBL_ROUTE_IF_FLD_OSPF6_TIMER_DEAD     = 29,  /* RW */
    TBL_ROUTE_IF_FLD_OSPF6_MTU_IGNORE     = 30,  /* RW */
    TBL_ROUTE_IF_FLD_OSPF6_COST           = 31,  /* RW */
    TBL_ROUTE_IF_FLD_OSPF6_PRIORITY       = 32,  /* RW */
    TBL_ROUTE_IF_FLD_OSPF6_NETWORK_TYPE   = 33,  /* RW */
    TBL_ROUTE_IF_FLD_ARP_PROXY_EN         = 34,  /* RW */
    TBL_ROUTE_IF_FLD_LOCAL_ARP_PROXY_EN   = 35,  /* RW */
    TBL_ROUTE_IF_FLD_UNICAST_RPF_EN       = 36,  /* RW */
    TBL_ROUTE_IF_FLD_UNICAST_RPF_EN_V6    = 37,  /* RW */
    TBL_ROUTE_IF_FLD_UNICAST_EN_V6        = 38,  /* RW */
    TBL_ROUTE_IF_FLD_IS_UP_RUNNING        = 39,  /* RW */
    TBL_ROUTE_IF_FLD_IPV6_LINK_LOCAL_HAD_DELETE = 40,  /* RW */
    TBL_ROUTE_IF_FLD_IP_UNREACHABLE_EN    = 41,  /* RW */
    TBL_ROUTE_IF_FLD_IP_REDIRECTS_EN      = 42,  /* RW */
    TBL_ROUTE_IF_FLD_DHCP_RELAY_OPTION_TRUST = 43,  /* RW */
    TBL_ROUTE_IF_FLD_DHCP_SERVER_GROUP    = 44,  /* RW */
    TBL_ROUTE_IF_FLD_DHCP_PDU_ENABLED     = 45,  /* READ */
    TBL_ROUTE_IF_FLD_DHCP_CLIENT_FLAGS    = 46,  /* RW */
    TBL_ROUTE_IF_FLD_DHCP_CLIENT_ENABLE   = 47,  /* RW */
    TBL_ROUTE_IF_FLD_DHCP_CLIENT_IPV4     = 48,  /* RW */
    TBL_ROUTE_IF_FLD_DHCP_CLIENT_IPV4_MASK = 49,  /* RW */
    TBL_ROUTE_IF_FLD_ARP_CURR_DYNAMIC     = 50,  /* READ */
    TBL_ROUTE_IF_FLD_ARP_ATTACK_NUMBER    = 51,  /* READ */
    TBL_ROUTE_IF_FLD_ARP_RATE_LIMIT_EN    = 52,  /* RW */
    TBL_ROUTE_IF_FLD_ARP_RATE_LIMIT_PKT_MAX = 53,  /* RW */
    TBL_ROUTE_IF_FLD_ARP_RATE_LIMIT_VIOLATION = 54,  /* RW */
    TBL_ROUTE_IF_FLD_ARP_RATE_LIMIT_PKT_CURR = 55,  /* READ */
    TBL_ROUTE_IF_FLD_ARP_RATE_LIMIT_PORT_ABNORMAL_FLAG = 56,  /* RW */
    TBL_ROUTE_IF_FLD_VMAC1                = 57,  /* RW */
    TBL_ROUTE_IF_FLD_VMAC2                = 58,  /* RW */
    TBL_ROUTE_IF_FLD_VMAC3                = 59,  /* RW */
    TBL_ROUTE_IF_FLD_VRRP_SESSION_NUM     = 60,  /* RW */
    TBL_ROUTE_IF_FLD_ND_FLAGS             = 61,  /* RW */
    TBL_ROUTE_IF_FLD_ND_RA_CURRRENT_HOP_LIMIT = 62,  /* RW */
    TBL_ROUTE_IF_FLD_ND_RA_ADVERTISE      = 63,  /* RW */
    TBL_ROUTE_IF_FLD_ND_RA_ADVERTISE_MTU  = 64,  /* RW */
    TBL_ROUTE_IF_FLD_ND_MANAGED_CONFIG    = 65,  /* RW */
    TBL_ROUTE_IF_FLD_ND_OTHER_CONFIG      = 66,  /* RW */
    TBL_ROUTE_IF_FLD_ND_RA_INTERVAL_MIN   = 67,  /* RW */
    TBL_ROUTE_IF_FLD_ND_RA_INTERVAL_MAX   = 68,  /* RW */
    TBL_ROUTE_IF_FLD_ND_RA_INTERVAL       = 69,  /* RW */
    TBL_ROUTE_IF_FLD_ND_RA_MIN_DELAY      = 70,  /* RW */
    TBL_ROUTE_IF_FLD_ND_RETRANSMIT_TIMER  = 71,  /* RW */
    TBL_ROUTE_IF_FLD_ND_ROUTER_LIFETIME   = 72,  /* RW */
    TBL_ROUTE_IF_FLD_ND_BASE_REACHABLE_TIME = 73,  /* RW */
    TBL_ROUTE_IF_FLD_ND_REACHABLE_TIME    = 74,  /* RW */
    TBL_ROUTE_IF_FLD_ND_DELAY_PROBE_TIME  = 75,  /* RW */
    TBL_ROUTE_IF_FLD_ND_DAD_ATTEMPTS      = 76,  /* RW */
    TBL_ROUTE_IF_FLD_ND_DST               = 77,  /* RW */
    TBL_ROUTE_IF_FLD_ND_LAST_RA_SOLICITED = 78,  /* RW */
    TBL_ROUTE_IF_FLD_ND_LAST_RA_UNSOLICITED = 79,  /* RW */
    TBL_ROUTE_IF_FLD_ND_RA_OCOUNT         = 80,  /* RW */
    TBL_ROUTE_IF_FLD_ND_RA_SOLICITED_OCOUNT = 81,  /* RW */
    TBL_ROUTE_IF_FLD_ND_RA_INIT_UNSOLICITED_OCOUNT = 82,  /* RW */
    TBL_ROUTE_IF_FLD_ND_RA_SOLICITED_TIME = 83,  /* RW */
    TBL_ROUTE_IF_FLD_ND_RA_PREFIX_DEFAULT_VLIFETIME = 84,  /* RW */
    TBL_ROUTE_IF_FLD_ND_RA_PREFIX_DEFAULT_PLIFETIME = 85,  /* RW */
    TBL_ROUTE_IF_FLD_ND_RA_PREFIX_DEFAULT_ONLINK = 86,  /* RW */
    TBL_ROUTE_IF_FLD_ND_RA_PREFIX_DEFAULT_AUTO = 87,  /* RW */
    TBL_ROUTE_IF_FLD_ND_RS_DEST_ADDR      = 88,  /* RW */
    TBL_ROUTE_IF_FLD_T_RA_SOLICITED       = 89,  /* READ */
    TBL_ROUTE_IF_FLD_T_RA_UNSOLICITED     = 90,  /* READ */
    TBL_ROUTE_IF_FLD_T_DELETE_ALL_V6_ADDR_TIMER = 91,  /* READ */
    TBL_ROUTE_IF_FLD_DHCPV6_CLIENT_FLAGS  = 92,  /* RW */
    TBL_ROUTE_IF_FLD_DHCPV6_CLIENT_ENABLE = 93,  /* RW */
    TBL_ROUTE_IF_FLD_NAT_ENABLE           = 94,  /* RW */
    TBL_ROUTE_IF_FLD_NAT_CATEGORY_ID      = 95,  /* RW */
    TBL_ROUTE_IF_FLD_SET_MIRROR_ENABLE    = 96,  /* RW */
    TBL_ROUTE_IF_FLD_T_OSPF_COST_TIMER    = 97,  /* READ */
    TBL_ROUTE_IF_FLD_T_SEND_ARP_TIMER     = 98,  /* READ */
    TBL_ROUTE_IF_FLD_IFC_IPV4             = 99,  /* SUB */
    TBL_ROUTE_IF_FLD_IFC_IPV6             = 100,  /* SUB */
    TBL_ROUTE_IF_FLD_OSPF_MD5_KEY         = 101,  /* SUB */
    TBL_ROUTE_IF_FLD_ND_PREFIX            = 102,  /* SUB */
    TBL_ROUTE_IF_FLD_PIM_MODE             = 103,  /* RW */
    TBL_ROUTE_IF_FLD_MULTICAST            = 104,  /* RW */
    TBL_ROUTE_IF_FLD_IGMP                 = 105,  /* RW */
    TBL_ROUTE_IF_FLD_IGMP_VERSION         = 106,  /* RW */
    TBL_ROUTE_IF_FLD_IGMP_INTF_LIMIT_NUMBER = 107,  /* RW */
    TBL_ROUTE_IF_FLD_MAX                  = 108
} tbl_route_if_field_id_t;

/* TBL_ROUTE_IF defines */
typedef struct
{
    char                 name[IFNAME_SIZE];
} tbl_route_if_key_t;

typedef struct
{
    tbl_route_if_key_t   key;
    uint32               mtu;                 /* MTU same to interface */
    mac_addr_t           mac;                 /* route-mac address */
    uint8                mode_set_before;     /* glb_if_mode_t *//*add by chenc for bug 50407, 2018-12-20*/
    uint32               ipv4_enable;         /* ipv4 enable */
    uint32               ipv6_enable;         /* ipv6 enable */
    uint32               arp_retrans_time;    /* retrans time */
    uint32               arp_timeout;         /* arp timeout */
    uint32               ifindex;
    uint32               kernel_ifindex;      /* ifindex assign in kernel to indicate an interface  */
    uint32               arp_retry_timeout;
    uint8                rip_recv_version;
    uint8                rip_send_version;
    uint8                rip_v2_broadcast;
    uint8                rip_split_horizon;
    uint8                rip_poisoned_reverse;
    uint8                rip_auth_mode;
    char                 rip_auth_string[256];
    uint8                ripng_split_horizon;
    uint8                ripng_poisoned_reverse;
    uint32               ospf_priority;
    uint32               ospf_cost;
    uint32               ospf_mtu_ignore;
    uint32               ospf_timer_hello;
    uint32               ospf_timer_dead;
    uint32               ospf_auth_type;
    char                 ospf_auth_key[256];
    uint8                ospf_network_type;
    uint32               ospf6_timer_hello;
    uint32               ospf6_timer_dead;
    uint32               ospf6_mtu_ignore;
    uint32               ospf6_cost;
    uint32               ospf6_priority;
    uint8                ospf6_network_type;
    uint8                arp_proxy_en;
    uint8                local_arp_proxy_en;
    uint32               unicast_rpf_en;
    uint32               unicast_rpf_en_v6;
    uint32               unicast_en_v6;
    uint32               is_up_running;
    uint32               ipv6_link_local_had_delete;
    uint8                ip_unreachable_en;
    uint8                ip_redirects_en;
    uint8                dhcp_relay_option_trust; /* trust this interface */
    uint8                dhcp_server_group;   /* server group for dhcp relay <1-16> */
    uint8                dhcp_pdu_enabled;    /* DHCP PDU to CPU enbaled or not */
    uint32               dhcp_client_flags;   /* dhcp client flags */
    uint8                dhcp_client_enable;  /* dhcp client enable */
    char                 dhcp_client_ipv4[GLB_IPV4_MAX_PREFIXLEN]; /* dhcp client ip */
    char                 dhcp_client_ipv4_mask[GLB_MAX_IPV4_MASK_LEN]; /* dhcp client mask */
    uint32               arp_curr_dynamic;    /* arp numberlimit current dynamic arp number */
    uint64               arp_attack_number;   /* arp numberlimit attack number */
    uint8                arp_rate_limit_en;   /* arp rate limit enable flag */
    uint32               arp_rate_limit_pkt_max; /* arp rate limit max pkt rcv in 30s */
    uint8                arp_rate_limit_violation; /* arp rate limit rcv pkts exceed max num , then follow this action to do :restrict, errdisable */
    uint32               arp_rate_limit_pkt_curr; /* arp rate limit current pkt number */
    uint8                arp_rate_limit_port_abnormal_flag; /* arp rate limit rcv pkts exceed max num */
    mac_addr_t           vmac1;               /* vrrp vmac address for session1 */
    mac_addr_t           vmac2;               /* vrrp vmac address for session2 */
    mac_addr_t           vmac3;               /* vrrp vmac address for session3 */
    uint8                vrrp_session_num;    /* vrrp session number */
    uint32               nd_flags;            /* configuration flags */
    uint32               nd_ra_currrent_hop_limit; /* Current Hop Limit */
    uint8                nd_ra_advertise;     /* ra suppress */
    uint8                nd_ra_advertise_mtu; /* ra mtu suppress */
    uint8                nd_managed_config;   /* Hosts should use DHCP for address config */
    uint8                nd_other_config;     /* Hosts should use DHCP for non-address config */
    uint32               nd_ra_interval_min;  /* Min RA interval in msec. */
    uint32               nd_ra_interval_max;  /* Max RA interval in msec. */
    uint32               nd_ra_interval;      /* RA interval in msec. */
    uint32               nd_ra_min_delay;     /* RA minimum interval in msec. */
    uint32               nd_retransmit_timer; /* Retransmit interval in msec. */
    uint32               nd_router_lifetime;  /* Router lifetime  in sec. */
    uint32               nd_base_reachable_time; /* Base reachable time. */
    uint32               nd_reachable_time;   /* Reachable time. */
    uint32               nd_delay_probe_time; /* time from DELAY to PROBE,  default is DELAY_FIRST_PROBE_TIME */
    uint32               nd_dad_attempts;     /* DAD retry numbers. */
    addr_t               nd_dst;              /* Destination address for the unicast Router Advertisement. */
    uint64               nd_last_ra_solicited; /* last time for packet sending, unit is msec */
    uint64               nd_last_ra_unsolicited; /* last time for packet sending, unit is msec */
    uint32               nd_ra_ocount;        /* number of output ra packets */
    uint32               nd_ra_solicited_ocount; /* number of solicited ra packets */
    uint32               nd_ra_init_unsolicited_ocount; /* number of unsolicited ra packets */
    uint32               nd_ra_solicited_time; /* solicited RA delay time. */
    uint32               nd_ra_prefix_default_vlifetime; /* prefix default vlifetime */
    uint32               nd_ra_prefix_default_plifetime; /* prefix default plifetime */
    uint16               nd_ra_prefix_default_onlink; /* prefix default onlink */
    uint16               nd_ra_prefix_default_auto; /* prefix default auto */
    addr_t               nd_rs_dest_addr;     /* RS unicast dest address */
    ctc_task_t           *t_ra_solicited;     /* Solicited Router Advertisement */
    ctc_task_t           *t_ra_unsolicited;   /* Unsolicited Router Advertisement */
    ctc_task_t           *t_delete_all_v6_addr_timer;
    uint32               dhcpv6_client_flags; /* dhcpv6 client flags */
    uint8                dhcpv6_client_enable; /* dhcpv6 client enable */
    uint8                nat_enable;          /* nat enable, GLB_IF_NAT_DISABLE, GLB_IF_NAT_INSIDE, GLB_IF_NAT_OUTSIDE, default is DISABLE*/
    uint8                nat_category_id;     /* nat category id, valid when nat_enable equals to outside.*/
    uint32               set_mirror_enable;   /* set mirror source or destination enable */
    ctc_task_t           *t_ospf_cost_timer;
    ctc_task_t           *t_send_arp_timer;
    cdb_reference_list_t ifc_ipv4;
    cdb_reference_list_t ifc_ipv6;
    cdb_reference_list_t ospf_md5_key;
    cdb_reference_list_t nd_prefix;           /* nd prefix */
    uint8                pim_mode;
    uint8                multicast;
    uint8                igmp;                /*add by zhw for igmp on/off*/
    char                 igmp_version[3];     /*add by zhw for interface igmp version*/
    uint32               igmp_intf_limit_number; /*add by zhw for interface groups limit number, 1-8192*/
} tbl_route_if_t;

typedef struct
{
    ctclib_hash_t        *if_hash;
    ctclib_slist_t       *if_list;
} tbl_route_if_master_t;

#endif /* !__TBL_ROUTE_IF_DEFINE_H__ */

