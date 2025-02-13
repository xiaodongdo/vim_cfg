
#include "proto.h"
#include "gen/tbl_route_if_define.h"
#include "gen/tbl_route_if.h"
#include "gen/ds_connected_define.h"
#include "gen/ds_connected.h"
#include "gen/ds_connected_v6_define.h"
#include "gen/ds_connected_v6.h"
#include "gen/ds_ospf_auth_define.h"
#include "gen/ds_ospf_auth.h"
#include "gen/ds_nd_prefix_define.h"
#include "gen/ds_nd_prefix.h"
#include "cdb_data_cmp.h"

tbl_route_if_master_t *_g_p_tbl_route_if_master = NULL;

static uint32
_tbl_route_if_hash_make(tbl_route_if_t *p_rtif)
{
    uint32 index = 0;
    uint32 key = 0;
    uint8 *pval = NULL;

    pval = (uint8*)&p_rtif->key;
    for (index = 0; index < sizeof(p_rtif->key); index++)
    {
        key += pval[index];
    }

    return key;
}

static bool
_tbl_route_if_hash_cmp(void *p_arg1, void *p_arg2)
{
    tbl_route_if_t *p_rtif1 = (tbl_route_if_t*)p_arg1;
    tbl_route_if_t *p_rtif2 = (tbl_route_if_t*)p_arg2;

    if (0 == sal_memcmp(&p_rtif1->key, &p_rtif2->key, sizeof(tbl_route_if_key_t)))
    {
        return TRUE;
    }

    return FALSE;
}

int32
tbl_route_if_add_route_if_sync(tbl_route_if_t *p_rtif, uint32 sync)
{
    tbl_route_if_master_t *p_master = _g_p_tbl_route_if_master;
    tbl_route_if_t *p_db_rtif = NULL;
    void *p_ret = NULL;
    int32 rc = PM_E_NONE;

    /* 1. lookup entry exist */
    if (tbl_route_if_get_route_if(&p_rtif->key))
    {
        return PM_E_EXIST;
    }

    /* 2. alloc a new db entry */
    p_db_rtif = XCALLOC(MEM_TBL_ROUTE_IF, sizeof(tbl_route_if_t));
    if (NULL == p_db_rtif)
    {
        return PM_E_NO_MEMORY;
    }

    /* 3. evaluate db entry */
    sal_memcpy(p_db_rtif, p_rtif, sizeof(tbl_route_if_t));
    PM_E_RETURN(cdb_reference_list_init(&p_db_rtif->ifc_ipv4, CDB_NODE_DS, DS_CONNECTED, ds_connected_cmp));
    PM_E_RETURN(cdb_reference_list_init(&p_db_rtif->ifc_ipv6, CDB_NODE_DS, DS_CONNECTED_V6, ds_connected_v6_cmp));
    PM_E_RETURN(cdb_reference_list_init(&p_db_rtif->ospf_md5_key, CDB_NODE_DS, DS_OSPF_AUTH, ds_ospf_auth_cmp));
    PM_E_RETURN(cdb_reference_list_init(&p_db_rtif->nd_prefix, CDB_NODE_DS, DS_ND_PREFIX, ds_nd_prefix_cmp));

    /* 4. add to db */
    p_ret = ctclib_hash_get(p_master->if_hash, (void*)p_db_rtif, ctclib_hash_alloc_intern);
    if (NULL == p_ret)
    {
        return PM_E_NO_MEMORY;
    }
    ctclib_slistnode_insert_sort(p_master->if_list, p_db_rtif);

    /* 5. sync to CDB */
    if (sync)
    {
        cdb_sync_tbl_add(TBL_ROUTE_IF, p_db_rtif);
        #ifdef LDB_SYNC_TO_RDB 
        cdb_sync_publish_tbl_add(TBL_ROUTE_IF, p_db_rtif);
        #endif 
    }

    return PM_E_NONE;
}

int32
tbl_route_if_del_route_if_sync(tbl_route_if_key_t *p_rtif_key, uint32 sync)
{
    tbl_route_if_master_t *p_master = _g_p_tbl_route_if_master;
    tbl_route_if_t *p_db_rtif = NULL;
    int32 rc = PM_E_NONE;

    /* 1. lookup entry exist */
    p_db_rtif = tbl_route_if_get_route_if(p_rtif_key);
    if (NULL == p_db_rtif)
    {
        return PM_E_NONE;
    }

    /* 2. sync to CDB */
    if (sync)
    {
        cdb_sync_tbl_del(TBL_ROUTE_IF, p_db_rtif);
        #ifdef LDB_SYNC_TO_RDB 
        cdb_sync_publish_tbl_del(TBL_ROUTE_IF, p_db_rtif);
        #endif 
    }

    /* 3. delete from db */
    ctclib_hash_release(p_master->if_hash, (void*)p_db_rtif);
    ctclib_slistnode_delete(p_master->if_list, p_db_rtif);
    PM_E_RETURN(cdb_reference_list_deinit(&p_db_rtif->ifc_ipv4));
    PM_E_RETURN(cdb_reference_list_deinit(&p_db_rtif->ifc_ipv6));
    PM_E_RETURN(cdb_reference_list_deinit(&p_db_rtif->ospf_md5_key));
    PM_E_RETURN(cdb_reference_list_deinit(&p_db_rtif->nd_prefix));

    /* 4. free db entry */
    XFREE(MEM_TBL_ROUTE_IF, p_db_rtif);

    return PM_E_NONE;
}

int32
tbl_route_if_set_route_if_field_sync(tbl_route_if_t *p_rtif, tbl_route_if_field_id_t field_id, uint32 sync)
{
    tbl_route_if_t *p_db_rtif = NULL;

    /* 1. lookup entry exist */
    p_db_rtif = tbl_route_if_get_route_if(&p_rtif->key);
    if (NULL == p_db_rtif)
    {
        return PM_E_NOT_EXIST;
    }

    /* 2. update db entry field */
    switch (field_id)
    {
    case TBL_ROUTE_IF_FLD_MTU:
        p_db_rtif->mtu = p_rtif->mtu;
        break;

    case TBL_ROUTE_IF_FLD_MAC:
        sal_memcpy(p_db_rtif->mac, p_rtif->mac, sizeof(p_rtif->mac));
        break;

    case TBL_ROUTE_IF_FLD_MODE_SET_BEFORE:
        p_db_rtif->mode_set_before = p_rtif->mode_set_before;
        break;

    case TBL_ROUTE_IF_FLD_IPV4_ENABLE:
        p_db_rtif->ipv4_enable = p_rtif->ipv4_enable;
        break;

    case TBL_ROUTE_IF_FLD_IPV6_ENABLE:
        p_db_rtif->ipv6_enable = p_rtif->ipv6_enable;
        break;

    case TBL_ROUTE_IF_FLD_ARP_RETRANS_TIME:
        p_db_rtif->arp_retrans_time = p_rtif->arp_retrans_time;
        break;

    case TBL_ROUTE_IF_FLD_ARP_TIMEOUT:
        p_db_rtif->arp_timeout = p_rtif->arp_timeout;
        break;

    case TBL_ROUTE_IF_FLD_IFINDEX:
        p_db_rtif->ifindex = p_rtif->ifindex;
        break;

    case TBL_ROUTE_IF_FLD_KERNEL_IFINDEX:
        p_db_rtif->kernel_ifindex = p_rtif->kernel_ifindex;
        break;

    case TBL_ROUTE_IF_FLD_ARP_RETRY_TIMEOUT:
        p_db_rtif->arp_retry_timeout = p_rtif->arp_retry_timeout;
        break;

    case TBL_ROUTE_IF_FLD_RIP_RECV_VERSION:
        p_db_rtif->rip_recv_version = p_rtif->rip_recv_version;
        break;

    case TBL_ROUTE_IF_FLD_RIP_SEND_VERSION:
        p_db_rtif->rip_send_version = p_rtif->rip_send_version;
        break;

    case TBL_ROUTE_IF_FLD_RIP_V2_BROADCAST:
        p_db_rtif->rip_v2_broadcast = p_rtif->rip_v2_broadcast;
        break;

    case TBL_ROUTE_IF_FLD_RIP_SPLIT_HORIZON:
        p_db_rtif->rip_split_horizon = p_rtif->rip_split_horizon;
        break;

    case TBL_ROUTE_IF_FLD_RIP_POISONED_REVERSE:
        p_db_rtif->rip_poisoned_reverse = p_rtif->rip_poisoned_reverse;
        break;

    case TBL_ROUTE_IF_FLD_RIP_AUTH_MODE:
        p_db_rtif->rip_auth_mode = p_rtif->rip_auth_mode;
        break;

    case TBL_ROUTE_IF_FLD_RIP_AUTH_STRING:
        sal_memcpy(p_db_rtif->rip_auth_string, p_rtif->rip_auth_string, sizeof(p_rtif->rip_auth_string));
        break;

    case TBL_ROUTE_IF_FLD_RIPNG_SPLIT_HORIZON:
        p_db_rtif->ripng_split_horizon = p_rtif->ripng_split_horizon;
        break;

    case TBL_ROUTE_IF_FLD_RIPNG_POISONED_REVERSE:
        p_db_rtif->ripng_poisoned_reverse = p_rtif->ripng_poisoned_reverse;
        break;

    case TBL_ROUTE_IF_FLD_OSPF_PRIORITY:
        p_db_rtif->ospf_priority = p_rtif->ospf_priority;
        break;

    case TBL_ROUTE_IF_FLD_OSPF_COST:
        p_db_rtif->ospf_cost = p_rtif->ospf_cost;
        break;

    case TBL_ROUTE_IF_FLD_OSPF_MTU_IGNORE:
        p_db_rtif->ospf_mtu_ignore = p_rtif->ospf_mtu_ignore;
        break;

    case TBL_ROUTE_IF_FLD_OSPF_TIMER_HELLO:
        p_db_rtif->ospf_timer_hello = p_rtif->ospf_timer_hello;
        break;

    case TBL_ROUTE_IF_FLD_OSPF_TIMER_DEAD:
        p_db_rtif->ospf_timer_dead = p_rtif->ospf_timer_dead;
        break;

    case TBL_ROUTE_IF_FLD_OSPF_AUTH_TYPE:
        p_db_rtif->ospf_auth_type = p_rtif->ospf_auth_type;
        break;

    case TBL_ROUTE_IF_FLD_OSPF_AUTH_KEY:
        sal_memcpy(p_db_rtif->ospf_auth_key, p_rtif->ospf_auth_key, sizeof(p_rtif->ospf_auth_key));
        break;

    case TBL_ROUTE_IF_FLD_OSPF_NETWORK_TYPE:
        p_db_rtif->ospf_network_type = p_rtif->ospf_network_type;
        break;

    case TBL_ROUTE_IF_FLD_OSPF6_TIMER_HELLO:
        p_db_rtif->ospf6_timer_hello = p_rtif->ospf6_timer_hello;
        break;

    case TBL_ROUTE_IF_FLD_OSPF6_TIMER_DEAD:
        p_db_rtif->ospf6_timer_dead = p_rtif->ospf6_timer_dead;
        break;

    case TBL_ROUTE_IF_FLD_OSPF6_MTU_IGNORE:
        p_db_rtif->ospf6_mtu_ignore = p_rtif->ospf6_mtu_ignore;
        break;

    case TBL_ROUTE_IF_FLD_OSPF6_COST:
        p_db_rtif->ospf6_cost = p_rtif->ospf6_cost;
        break;

    case TBL_ROUTE_IF_FLD_OSPF6_PRIORITY:
        p_db_rtif->ospf6_priority = p_rtif->ospf6_priority;
        break;

    case TBL_ROUTE_IF_FLD_OSPF6_NETWORK_TYPE:
        p_db_rtif->ospf6_network_type = p_rtif->ospf6_network_type;
        break;

    case TBL_ROUTE_IF_FLD_ARP_PROXY_EN:
        p_db_rtif->arp_proxy_en = p_rtif->arp_proxy_en;
        break;

    case TBL_ROUTE_IF_FLD_LOCAL_ARP_PROXY_EN:
        p_db_rtif->local_arp_proxy_en = p_rtif->local_arp_proxy_en;
        break;

    case TBL_ROUTE_IF_FLD_UNICAST_RPF_EN:
        p_db_rtif->unicast_rpf_en = p_rtif->unicast_rpf_en;
        break;

    case TBL_ROUTE_IF_FLD_UNICAST_RPF_EN_V6:
        p_db_rtif->unicast_rpf_en_v6 = p_rtif->unicast_rpf_en_v6;
        break;

    case TBL_ROUTE_IF_FLD_UNICAST_EN_V6:
        p_db_rtif->unicast_en_v6 = p_rtif->unicast_en_v6;
        break;

    case TBL_ROUTE_IF_FLD_IS_UP_RUNNING:
        p_db_rtif->is_up_running = p_rtif->is_up_running;
        break;

    case TBL_ROUTE_IF_FLD_IPV6_LINK_LOCAL_HAD_DELETE:
        p_db_rtif->ipv6_link_local_had_delete = p_rtif->ipv6_link_local_had_delete;
        break;

    case TBL_ROUTE_IF_FLD_IP_UNREACHABLE_EN:
        p_db_rtif->ip_unreachable_en = p_rtif->ip_unreachable_en;
        break;

    case TBL_ROUTE_IF_FLD_IP_REDIRECTS_EN:
        p_db_rtif->ip_redirects_en = p_rtif->ip_redirects_en;
        break;

    case TBL_ROUTE_IF_FLD_DHCP_RELAY_OPTION_TRUST:
        p_db_rtif->dhcp_relay_option_trust = p_rtif->dhcp_relay_option_trust;
        break;

    case TBL_ROUTE_IF_FLD_DHCP_SERVER_GROUP:
        p_db_rtif->dhcp_server_group = p_rtif->dhcp_server_group;
        break;

    case TBL_ROUTE_IF_FLD_DHCP_PDU_ENABLED:
        p_db_rtif->dhcp_pdu_enabled = p_rtif->dhcp_pdu_enabled;
        break;

    case TBL_ROUTE_IF_FLD_DHCP_CLIENT_FLAGS:
        p_db_rtif->dhcp_client_flags = p_rtif->dhcp_client_flags;
        break;

    case TBL_ROUTE_IF_FLD_DHCP_CLIENT_ENABLE:
        p_db_rtif->dhcp_client_enable = p_rtif->dhcp_client_enable;
        break;

    case TBL_ROUTE_IF_FLD_DHCP_CLIENT_IPV4:
        sal_memcpy(p_db_rtif->dhcp_client_ipv4, p_rtif->dhcp_client_ipv4, sizeof(p_rtif->dhcp_client_ipv4));
        break;

    case TBL_ROUTE_IF_FLD_DHCP_CLIENT_IPV4_MASK:
        sal_memcpy(p_db_rtif->dhcp_client_ipv4_mask, p_rtif->dhcp_client_ipv4_mask, sizeof(p_rtif->dhcp_client_ipv4_mask));
        break;

    case TBL_ROUTE_IF_FLD_ARP_CURR_DYNAMIC:
        p_db_rtif->arp_curr_dynamic = p_rtif->arp_curr_dynamic;
        break;

    case TBL_ROUTE_IF_FLD_ARP_ATTACK_NUMBER:
        p_db_rtif->arp_attack_number = p_rtif->arp_attack_number;
        break;

    case TBL_ROUTE_IF_FLD_ARP_RATE_LIMIT_EN:
        p_db_rtif->arp_rate_limit_en = p_rtif->arp_rate_limit_en;
        break;

    case TBL_ROUTE_IF_FLD_ARP_RATE_LIMIT_PKT_MAX:
        p_db_rtif->arp_rate_limit_pkt_max = p_rtif->arp_rate_limit_pkt_max;
        break;

    case TBL_ROUTE_IF_FLD_ARP_RATE_LIMIT_VIOLATION:
        p_db_rtif->arp_rate_limit_violation = p_rtif->arp_rate_limit_violation;
        break;

    case TBL_ROUTE_IF_FLD_ARP_RATE_LIMIT_PKT_CURR:
        p_db_rtif->arp_rate_limit_pkt_curr = p_rtif->arp_rate_limit_pkt_curr;
        break;

    case TBL_ROUTE_IF_FLD_ARP_RATE_LIMIT_PORT_ABNORMAL_FLAG:
        p_db_rtif->arp_rate_limit_port_abnormal_flag = p_rtif->arp_rate_limit_port_abnormal_flag;
        break;

    case TBL_ROUTE_IF_FLD_VMAC1:
        sal_memcpy(p_db_rtif->vmac1, p_rtif->vmac1, sizeof(p_rtif->vmac1));
        break;

    case TBL_ROUTE_IF_FLD_VMAC2:
        sal_memcpy(p_db_rtif->vmac2, p_rtif->vmac2, sizeof(p_rtif->vmac2));
        break;

    case TBL_ROUTE_IF_FLD_VMAC3:
        sal_memcpy(p_db_rtif->vmac3, p_rtif->vmac3, sizeof(p_rtif->vmac3));
        break;

    case TBL_ROUTE_IF_FLD_VRRP_SESSION_NUM:
        p_db_rtif->vrrp_session_num = p_rtif->vrrp_session_num;
        break;

    case TBL_ROUTE_IF_FLD_ND_FLAGS:
        p_db_rtif->nd_flags = p_rtif->nd_flags;
        break;

    case TBL_ROUTE_IF_FLD_ND_RA_CURRRENT_HOP_LIMIT:
        p_db_rtif->nd_ra_currrent_hop_limit = p_rtif->nd_ra_currrent_hop_limit;
        break;

    case TBL_ROUTE_IF_FLD_ND_RA_ADVERTISE:
        p_db_rtif->nd_ra_advertise = p_rtif->nd_ra_advertise;
        break;

    case TBL_ROUTE_IF_FLD_ND_RA_ADVERTISE_MTU:
        p_db_rtif->nd_ra_advertise_mtu = p_rtif->nd_ra_advertise_mtu;
        break;

    case TBL_ROUTE_IF_FLD_ND_MANAGED_CONFIG:
        p_db_rtif->nd_managed_config = p_rtif->nd_managed_config;
        break;

    case TBL_ROUTE_IF_FLD_ND_OTHER_CONFIG:
        p_db_rtif->nd_other_config = p_rtif->nd_other_config;
        break;

    case TBL_ROUTE_IF_FLD_ND_RA_INTERVAL_MIN:
        p_db_rtif->nd_ra_interval_min = p_rtif->nd_ra_interval_min;
        break;

    case TBL_ROUTE_IF_FLD_ND_RA_INTERVAL_MAX:
        p_db_rtif->nd_ra_interval_max = p_rtif->nd_ra_interval_max;
        break;

    case TBL_ROUTE_IF_FLD_ND_RA_INTERVAL:
        p_db_rtif->nd_ra_interval = p_rtif->nd_ra_interval;
        break;

    case TBL_ROUTE_IF_FLD_ND_RA_MIN_DELAY:
        p_db_rtif->nd_ra_min_delay = p_rtif->nd_ra_min_delay;
        break;

    case TBL_ROUTE_IF_FLD_ND_RETRANSMIT_TIMER:
        p_db_rtif->nd_retransmit_timer = p_rtif->nd_retransmit_timer;
        break;

    case TBL_ROUTE_IF_FLD_ND_ROUTER_LIFETIME:
        p_db_rtif->nd_router_lifetime = p_rtif->nd_router_lifetime;
        break;

    case TBL_ROUTE_IF_FLD_ND_BASE_REACHABLE_TIME:
        p_db_rtif->nd_base_reachable_time = p_rtif->nd_base_reachable_time;
        break;

    case TBL_ROUTE_IF_FLD_ND_REACHABLE_TIME:
        p_db_rtif->nd_reachable_time = p_rtif->nd_reachable_time;
        break;

    case TBL_ROUTE_IF_FLD_ND_DELAY_PROBE_TIME:
        p_db_rtif->nd_delay_probe_time = p_rtif->nd_delay_probe_time;
        break;

    case TBL_ROUTE_IF_FLD_ND_DAD_ATTEMPTS:
        p_db_rtif->nd_dad_attempts = p_rtif->nd_dad_attempts;
        break;

    case TBL_ROUTE_IF_FLD_ND_DST:
        sal_memcpy(&p_db_rtif->nd_dst, &p_rtif->nd_dst, sizeof(p_rtif->nd_dst));
        break;

    case TBL_ROUTE_IF_FLD_ND_LAST_RA_SOLICITED:
        p_db_rtif->nd_last_ra_solicited = p_rtif->nd_last_ra_solicited;
        break;

    case TBL_ROUTE_IF_FLD_ND_LAST_RA_UNSOLICITED:
        p_db_rtif->nd_last_ra_unsolicited = p_rtif->nd_last_ra_unsolicited;
        break;

    case TBL_ROUTE_IF_FLD_ND_RA_OCOUNT:
        p_db_rtif->nd_ra_ocount = p_rtif->nd_ra_ocount;
        break;

    case TBL_ROUTE_IF_FLD_ND_RA_SOLICITED_OCOUNT:
        p_db_rtif->nd_ra_solicited_ocount = p_rtif->nd_ra_solicited_ocount;
        break;

    case TBL_ROUTE_IF_FLD_ND_RA_INIT_UNSOLICITED_OCOUNT:
        p_db_rtif->nd_ra_init_unsolicited_ocount = p_rtif->nd_ra_init_unsolicited_ocount;
        break;

    case TBL_ROUTE_IF_FLD_ND_RA_SOLICITED_TIME:
        p_db_rtif->nd_ra_solicited_time = p_rtif->nd_ra_solicited_time;
        break;

    case TBL_ROUTE_IF_FLD_ND_RA_PREFIX_DEFAULT_VLIFETIME:
        p_db_rtif->nd_ra_prefix_default_vlifetime = p_rtif->nd_ra_prefix_default_vlifetime;
        break;

    case TBL_ROUTE_IF_FLD_ND_RA_PREFIX_DEFAULT_PLIFETIME:
        p_db_rtif->nd_ra_prefix_default_plifetime = p_rtif->nd_ra_prefix_default_plifetime;
        break;

    case TBL_ROUTE_IF_FLD_ND_RA_PREFIX_DEFAULT_ONLINK:
        p_db_rtif->nd_ra_prefix_default_onlink = p_rtif->nd_ra_prefix_default_onlink;
        break;

    case TBL_ROUTE_IF_FLD_ND_RA_PREFIX_DEFAULT_AUTO:
        p_db_rtif->nd_ra_prefix_default_auto = p_rtif->nd_ra_prefix_default_auto;
        break;

    case TBL_ROUTE_IF_FLD_ND_RS_DEST_ADDR:
        sal_memcpy(&p_db_rtif->nd_rs_dest_addr, &p_rtif->nd_rs_dest_addr, sizeof(p_rtif->nd_rs_dest_addr));
        break;

    case TBL_ROUTE_IF_FLD_T_RA_SOLICITED:
        p_db_rtif->t_ra_solicited = p_rtif->t_ra_solicited;
        break;

    case TBL_ROUTE_IF_FLD_T_RA_UNSOLICITED:
        p_db_rtif->t_ra_unsolicited = p_rtif->t_ra_unsolicited;
        break;

    case TBL_ROUTE_IF_FLD_T_DELETE_ALL_V6_ADDR_TIMER:
        p_db_rtif->t_delete_all_v6_addr_timer = p_rtif->t_delete_all_v6_addr_timer;
        break;

    case TBL_ROUTE_IF_FLD_DHCPV6_CLIENT_FLAGS:
        p_db_rtif->dhcpv6_client_flags = p_rtif->dhcpv6_client_flags;
        break;

    case TBL_ROUTE_IF_FLD_DHCPV6_CLIENT_ENABLE:
        p_db_rtif->dhcpv6_client_enable = p_rtif->dhcpv6_client_enable;
        break;

    case TBL_ROUTE_IF_FLD_NAT_ENABLE:
        p_db_rtif->nat_enable = p_rtif->nat_enable;
        break;

    case TBL_ROUTE_IF_FLD_NAT_CATEGORY_ID:
        p_db_rtif->nat_category_id = p_rtif->nat_category_id;
        break;

    case TBL_ROUTE_IF_FLD_SET_MIRROR_ENABLE:
        p_db_rtif->set_mirror_enable = p_rtif->set_mirror_enable;
        break;

    case TBL_ROUTE_IF_FLD_T_OSPF_COST_TIMER:
        p_db_rtif->t_ospf_cost_timer = p_rtif->t_ospf_cost_timer;
        break;

    case TBL_ROUTE_IF_FLD_T_SEND_ARP_TIMER:
        p_db_rtif->t_send_arp_timer = p_rtif->t_send_arp_timer;
        break;

    case TBL_ROUTE_IF_FLD_PIM_MODE:
        p_db_rtif->pim_mode = p_rtif->pim_mode;
        break;

    case TBL_ROUTE_IF_FLD_MULTICAST:
        p_db_rtif->multicast = p_rtif->multicast;
        break;

    case TBL_ROUTE_IF_FLD_IGMP:
        p_db_rtif->igmp = p_rtif->igmp;
        break;

    case TBL_ROUTE_IF_FLD_IGMP_VERSION:
        sal_memcpy(p_db_rtif->igmp_version, p_rtif->igmp_version, sizeof(p_rtif->igmp_version));
        break;

    case TBL_ROUTE_IF_FLD_IGMP_INTF_LIMIT_NUMBER:
        p_db_rtif->igmp_intf_limit_number = p_rtif->igmp_intf_limit_number;
        break;

    case TBL_ROUTE_IF_FLD_MAX:
        sal_memcpy(p_db_rtif, p_rtif, sizeof(tbl_route_if_t));
        break;
    default:
        return PM_E_NOT_SUPPORT;
    }

    /* 3. sync to CDB */
    if (sync)
    {
        cdb_sync_tbl_set(TBL_ROUTE_IF, field_id, p_db_rtif);
        #ifdef LDB_SYNC_TO_RDB 
        cdb_sync_publish_tbl_set(TBL_ROUTE_IF, field_id, p_db_rtif);
        #endif 
    }

    return PM_E_NONE;
}

int32
tbl_route_if_add_route_if(tbl_route_if_t *p_rtif)
{
    return tbl_route_if_add_route_if_sync(p_rtif, TRUE);
}

int32
tbl_route_if_del_route_if(tbl_route_if_key_t *p_rtif_key)
{
    return tbl_route_if_del_route_if_sync(p_rtif_key, TRUE);
}

int32
tbl_route_if_set_route_if_field(tbl_route_if_t *p_rtif, tbl_route_if_field_id_t field_id)
{
    return tbl_route_if_set_route_if_field_sync(p_rtif, field_id, TRUE);
}

tbl_route_if_t*
tbl_route_if_get_route_if(tbl_route_if_key_t *p_rtif_key)
{
    tbl_route_if_master_t *p_master = _g_p_tbl_route_if_master;
    tbl_route_if_t lkp;

    sal_memcpy(&lkp.key, p_rtif_key, sizeof(tbl_route_if_key_t));
    return ctclib_hash_lookup(p_master->if_hash, &lkp);
}

char*
tbl_route_if_key_val2str(tbl_route_if_t *p_rtif, char *str, uint32 str_len)
{
    sal_memset(str, 0, str_len);
    sal_snprintf(str, str_len, "%s", p_rtif->key.name);
    return str;
}

int32
tbl_route_if_key_str2val(char *str, tbl_route_if_t *p_rtif)
{
    int32 ret = 0;

    sal_strcpy(p_rtif->key.name, str);
    return ret;
}

char*
tbl_route_if_key_name_dump(tbl_route_if_t *p_rtif, char *str)
{
    cdb_node_t *p_node = cdb_get_tbl(TBL_ROUTE_IF);
    sal_sprintf(str, "%s"KEY_CONNECT_STR"%s", p_node->name, 
        p_rtif->key.name);
    return str;
}

char*
tbl_route_if_key_value_dump(tbl_route_if_t *p_rtif, char *str)
{
    sal_sprintf(str, "%s", p_rtif->key.name);
    return str;
}

char*
tbl_route_if_field_name_dump(tbl_route_if_t *p_rtif, int32 field_id, char *str)
{

    cdb_node_t *p_node = cdb_get_tbl(TBL_ROUTE_IF);
    cdb_tbl_info_t *p_tbl_info = p_node->tbl_info;
    if (field_id >= TBL_ROUTE_IF_FLD_MAX)
    {
        return NULL;
    }

    sal_sprintf(str, "%s", p_tbl_info->field[field_id].name);

    return str;
}

char*
tbl_route_if_field_value_dump(tbl_route_if_t *p_rtif, int32 field_id, char* str)
{
    if (FLD_MATCH(TBL_ROUTE_IF_FLD_KEY, field_id))
    {
        sal_sprintf(str, "%s", p_rtif->key.name);
    }
    if (FLD_MATCH(TBL_ROUTE_IF_FLD_MTU, field_id))
    {
        sal_sprintf(str, "%u", p_rtif->mtu);
    }
    if (FLD_MATCH(TBL_ROUTE_IF_FLD_MAC, field_id))
    {
        cdb_mac_addr_val2str(str, MAX_CMD_STR_LEN, p_rtif->mac);
    }
    if (FLD_MATCH(TBL_ROUTE_IF_FLD_MODE_SET_BEFORE, field_id))
    {
        sal_sprintf(str, "%s", cdb_enum_val2str(glb_if_mode_strs, GLB_IF_MODE_MAX, p_rtif->mode_set_before));
    }
    if (FLD_MATCH(TBL_ROUTE_IF_FLD_IPV4_ENABLE, field_id))
    {
        sal_sprintf(str, "%u", p_rtif->ipv4_enable);
    }
    if (FLD_MATCH(TBL_ROUTE_IF_FLD_IPV6_ENABLE, field_id))
    {
        sal_sprintf(str, "%u", p_rtif->ipv6_enable);
    }
    if (FLD_MATCH(TBL_ROUTE_IF_FLD_ARP_RETRANS_TIME, field_id))
    {
        sal_sprintf(str, "%u", p_rtif->arp_retrans_time);
    }
    if (FLD_MATCH(TBL_ROUTE_IF_FLD_ARP_TIMEOUT, field_id))
    {
        sal_sprintf(str, "%u", p_rtif->arp_timeout);
    }
    if (FLD_MATCH(TBL_ROUTE_IF_FLD_IFINDEX, field_id))
    {
        sal_sprintf(str, "%u", p_rtif->ifindex);
    }
    if (FLD_MATCH(TBL_ROUTE_IF_FLD_KERNEL_IFINDEX, field_id))
    {
        sal_sprintf(str, "%u", p_rtif->kernel_ifindex);
    }
    if (FLD_MATCH(TBL_ROUTE_IF_FLD_ARP_RETRY_TIMEOUT, field_id))
    {
        sal_sprintf(str, "%u", p_rtif->arp_retry_timeout);
    }
    if (FLD_MATCH(TBL_ROUTE_IF_FLD_RIP_RECV_VERSION, field_id))
    {
        sal_sprintf(str, "%u", p_rtif->rip_recv_version);
    }
    if (FLD_MATCH(TBL_ROUTE_IF_FLD_RIP_SEND_VERSION, field_id))
    {
        sal_sprintf(str, "%u", p_rtif->rip_send_version);
    }
    if (FLD_MATCH(TBL_ROUTE_IF_FLD_RIP_V2_BROADCAST, field_id))
    {
        sal_sprintf(str, "%u", p_rtif->rip_v2_broadcast);
    }
    if (FLD_MATCH(TBL_ROUTE_IF_FLD_RIP_SPLIT_HORIZON, field_id))
    {
        sal_sprintf(str, "%u", p_rtif->rip_split_horizon);
    }
    if (FLD_MATCH(TBL_ROUTE_IF_FLD_RIP_POISONED_REVERSE, field_id))
    {
        sal_sprintf(str, "%u", p_rtif->rip_poisoned_reverse);
    }
    if (FLD_MATCH(TBL_ROUTE_IF_FLD_RIP_AUTH_MODE, field_id))
    {
        sal_sprintf(str, "%u", p_rtif->rip_auth_mode);
    }
    if (FLD_MATCH(TBL_ROUTE_IF_FLD_RIP_AUTH_STRING, field_id))
    {
        sal_sprintf(str, "%s", p_rtif->rip_auth_string);
    }
    if (FLD_MATCH(TBL_ROUTE_IF_FLD_RIPNG_SPLIT_HORIZON, field_id))
    {
        sal_sprintf(str, "%u", p_rtif->ripng_split_horizon);
    }
    if (FLD_MATCH(TBL_ROUTE_IF_FLD_RIPNG_POISONED_REVERSE, field_id))
    {
        sal_sprintf(str, "%u", p_rtif->ripng_poisoned_reverse);
    }
    if (FLD_MATCH(TBL_ROUTE_IF_FLD_OSPF_PRIORITY, field_id))
    {
        sal_sprintf(str, "%u", p_rtif->ospf_priority);
    }
    if (FLD_MATCH(TBL_ROUTE_IF_FLD_OSPF_COST, field_id))
    {
        sal_sprintf(str, "%u", p_rtif->ospf_cost);
    }
    if (FLD_MATCH(TBL_ROUTE_IF_FLD_OSPF_MTU_IGNORE, field_id))
    {
        sal_sprintf(str, "%u", p_rtif->ospf_mtu_ignore);
    }
    if (FLD_MATCH(TBL_ROUTE_IF_FLD_OSPF_TIMER_HELLO, field_id))
    {
        sal_sprintf(str, "%u", p_rtif->ospf_timer_hello);
    }
    if (FLD_MATCH(TBL_ROUTE_IF_FLD_OSPF_TIMER_DEAD, field_id))
    {
        sal_sprintf(str, "%u", p_rtif->ospf_timer_dead);
    }
    if (FLD_MATCH(TBL_ROUTE_IF_FLD_OSPF_AUTH_TYPE, field_id))
    {
        sal_sprintf(str, "%u", p_rtif->ospf_auth_type);
    }
    if (FLD_MATCH(TBL_ROUTE_IF_FLD_OSPF_AUTH_KEY, field_id))
    {
        sal_sprintf(str, "%s", p_rtif->ospf_auth_key);
    }
    if (FLD_MATCH(TBL_ROUTE_IF_FLD_OSPF_NETWORK_TYPE, field_id))
    {
        sal_sprintf(str, "%u", p_rtif->ospf_network_type);
    }
    if (FLD_MATCH(TBL_ROUTE_IF_FLD_OSPF6_TIMER_HELLO, field_id))
    {
        sal_sprintf(str, "%u", p_rtif->ospf6_timer_hello);
    }
    if (FLD_MATCH(TBL_ROUTE_IF_FLD_OSPF6_TIMER_DEAD, field_id))
    {
        sal_sprintf(str, "%u", p_rtif->ospf6_timer_dead);
    }
    if (FLD_MATCH(TBL_ROUTE_IF_FLD_OSPF6_MTU_IGNORE, field_id))
    {
        sal_sprintf(str, "%u", p_rtif->ospf6_mtu_ignore);
    }
    if (FLD_MATCH(TBL_ROUTE_IF_FLD_OSPF6_COST, field_id))
    {
        sal_sprintf(str, "%u", p_rtif->ospf6_cost);
    }
    if (FLD_MATCH(TBL_ROUTE_IF_FLD_OSPF6_PRIORITY, field_id))
    {
        sal_sprintf(str, "%u", p_rtif->ospf6_priority);
    }
    if (FLD_MATCH(TBL_ROUTE_IF_FLD_OSPF6_NETWORK_TYPE, field_id))
    {
        sal_sprintf(str, "%u", p_rtif->ospf6_network_type);
    }
    if (FLD_MATCH(TBL_ROUTE_IF_FLD_ARP_PROXY_EN, field_id))
    {
        sal_sprintf(str, "%u", p_rtif->arp_proxy_en);
    }
    if (FLD_MATCH(TBL_ROUTE_IF_FLD_LOCAL_ARP_PROXY_EN, field_id))
    {
        sal_sprintf(str, "%u", p_rtif->local_arp_proxy_en);
    }
    if (FLD_MATCH(TBL_ROUTE_IF_FLD_UNICAST_RPF_EN, field_id))
    {
        sal_sprintf(str, "%u", p_rtif->unicast_rpf_en);
    }
    if (FLD_MATCH(TBL_ROUTE_IF_FLD_UNICAST_RPF_EN_V6, field_id))
    {
        sal_sprintf(str, "%u", p_rtif->unicast_rpf_en_v6);
    }
    if (FLD_MATCH(TBL_ROUTE_IF_FLD_UNICAST_EN_V6, field_id))
    {
        sal_sprintf(str, "%u", p_rtif->unicast_en_v6);
    }
    if (FLD_MATCH(TBL_ROUTE_IF_FLD_IS_UP_RUNNING, field_id))
    {
        sal_sprintf(str, "%u", p_rtif->is_up_running);
    }
    if (FLD_MATCH(TBL_ROUTE_IF_FLD_IPV6_LINK_LOCAL_HAD_DELETE, field_id))
    {
        sal_sprintf(str, "%u", p_rtif->ipv6_link_local_had_delete);
    }
    if (FLD_MATCH(TBL_ROUTE_IF_FLD_IP_UNREACHABLE_EN, field_id))
    {
        sal_sprintf(str, "%u", p_rtif->ip_unreachable_en);
    }
    if (FLD_MATCH(TBL_ROUTE_IF_FLD_IP_REDIRECTS_EN, field_id))
    {
        sal_sprintf(str, "%u", p_rtif->ip_redirects_en);
    }
    if (FLD_MATCH(TBL_ROUTE_IF_FLD_DHCP_RELAY_OPTION_TRUST, field_id))
    {
        sal_sprintf(str, "%u", p_rtif->dhcp_relay_option_trust);
    }
    if (FLD_MATCH(TBL_ROUTE_IF_FLD_DHCP_SERVER_GROUP, field_id))
    {
        sal_sprintf(str, "%u", p_rtif->dhcp_server_group);
    }
    if (FLD_MATCH(TBL_ROUTE_IF_FLD_DHCP_PDU_ENABLED, field_id))
    {
        sal_sprintf(str, "%u", p_rtif->dhcp_pdu_enabled);
    }
    if (FLD_MATCH(TBL_ROUTE_IF_FLD_DHCP_CLIENT_FLAGS, field_id))
    {
        sal_sprintf(str, "%u", p_rtif->dhcp_client_flags);
    }
    if (FLD_MATCH(TBL_ROUTE_IF_FLD_DHCP_CLIENT_ENABLE, field_id))
    {
        sal_sprintf(str, "%u", p_rtif->dhcp_client_enable);
    }
    if (FLD_MATCH(TBL_ROUTE_IF_FLD_DHCP_CLIENT_IPV4, field_id))
    {
        sal_sprintf(str, "%s", p_rtif->dhcp_client_ipv4);
    }
    if (FLD_MATCH(TBL_ROUTE_IF_FLD_DHCP_CLIENT_IPV4_MASK, field_id))
    {
        sal_sprintf(str, "%s", p_rtif->dhcp_client_ipv4_mask);
    }
    if (FLD_MATCH(TBL_ROUTE_IF_FLD_ARP_CURR_DYNAMIC, field_id))
    {
        sal_sprintf(str, "%u", p_rtif->arp_curr_dynamic);
    }
    if (FLD_MATCH(TBL_ROUTE_IF_FLD_ARP_ATTACK_NUMBER, field_id))
    {
        sal_sprintf(str, "%"PRIu64, p_rtif->arp_attack_number);
    }
    if (FLD_MATCH(TBL_ROUTE_IF_FLD_ARP_RATE_LIMIT_EN, field_id))
    {
        sal_sprintf(str, "%u", p_rtif->arp_rate_limit_en);
    }
    if (FLD_MATCH(TBL_ROUTE_IF_FLD_ARP_RATE_LIMIT_PKT_MAX, field_id))
    {
        sal_sprintf(str, "%u", p_rtif->arp_rate_limit_pkt_max);
    }
    if (FLD_MATCH(TBL_ROUTE_IF_FLD_ARP_RATE_LIMIT_VIOLATION, field_id))
    {
        sal_sprintf(str, "%u", p_rtif->arp_rate_limit_violation);
    }
    if (FLD_MATCH(TBL_ROUTE_IF_FLD_ARP_RATE_LIMIT_PKT_CURR, field_id))
    {
        sal_sprintf(str, "%u", p_rtif->arp_rate_limit_pkt_curr);
    }
    if (FLD_MATCH(TBL_ROUTE_IF_FLD_ARP_RATE_LIMIT_PORT_ABNORMAL_FLAG, field_id))
    {
        sal_sprintf(str, "%u", p_rtif->arp_rate_limit_port_abnormal_flag);
    }
    if (FLD_MATCH(TBL_ROUTE_IF_FLD_VMAC1, field_id))
    {
        cdb_mac_addr_val2str(str, MAX_CMD_STR_LEN, p_rtif->vmac1);
    }
    if (FLD_MATCH(TBL_ROUTE_IF_FLD_VMAC2, field_id))
    {
        cdb_mac_addr_val2str(str, MAX_CMD_STR_LEN, p_rtif->vmac2);
    }
    if (FLD_MATCH(TBL_ROUTE_IF_FLD_VMAC3, field_id))
    {
        cdb_mac_addr_val2str(str, MAX_CMD_STR_LEN, p_rtif->vmac3);
    }
    if (FLD_MATCH(TBL_ROUTE_IF_FLD_VRRP_SESSION_NUM, field_id))
    {
        sal_sprintf(str, "%u", p_rtif->vrrp_session_num);
    }
    if (FLD_MATCH(TBL_ROUTE_IF_FLD_ND_FLAGS, field_id))
    {
        sal_sprintf(str, "%u", p_rtif->nd_flags);
    }
    if (FLD_MATCH(TBL_ROUTE_IF_FLD_ND_RA_CURRRENT_HOP_LIMIT, field_id))
    {
        sal_sprintf(str, "%u", p_rtif->nd_ra_currrent_hop_limit);
    }
    if (FLD_MATCH(TBL_ROUTE_IF_FLD_ND_RA_ADVERTISE, field_id))
    {
        sal_sprintf(str, "%u", p_rtif->nd_ra_advertise);
    }
    if (FLD_MATCH(TBL_ROUTE_IF_FLD_ND_RA_ADVERTISE_MTU, field_id))
    {
        sal_sprintf(str, "%u", p_rtif->nd_ra_advertise_mtu);
    }
    if (FLD_MATCH(TBL_ROUTE_IF_FLD_ND_MANAGED_CONFIG, field_id))
    {
        sal_sprintf(str, "%u", p_rtif->nd_managed_config);
    }
    if (FLD_MATCH(TBL_ROUTE_IF_FLD_ND_OTHER_CONFIG, field_id))
    {
        sal_sprintf(str, "%u", p_rtif->nd_other_config);
    }
    if (FLD_MATCH(TBL_ROUTE_IF_FLD_ND_RA_INTERVAL_MIN, field_id))
    {
        sal_sprintf(str, "%u", p_rtif->nd_ra_interval_min);
    }
    if (FLD_MATCH(TBL_ROUTE_IF_FLD_ND_RA_INTERVAL_MAX, field_id))
    {
        sal_sprintf(str, "%u", p_rtif->nd_ra_interval_max);
    }
    if (FLD_MATCH(TBL_ROUTE_IF_FLD_ND_RA_INTERVAL, field_id))
    {
        sal_sprintf(str, "%u", p_rtif->nd_ra_interval);
    }
    if (FLD_MATCH(TBL_ROUTE_IF_FLD_ND_RA_MIN_DELAY, field_id))
    {
        sal_sprintf(str, "%u", p_rtif->nd_ra_min_delay);
    }
    if (FLD_MATCH(TBL_ROUTE_IF_FLD_ND_RETRANSMIT_TIMER, field_id))
    {
        sal_sprintf(str, "%u", p_rtif->nd_retransmit_timer);
    }
    if (FLD_MATCH(TBL_ROUTE_IF_FLD_ND_ROUTER_LIFETIME, field_id))
    {
        sal_sprintf(str, "%u", p_rtif->nd_router_lifetime);
    }
    if (FLD_MATCH(TBL_ROUTE_IF_FLD_ND_BASE_REACHABLE_TIME, field_id))
    {
        sal_sprintf(str, "%u", p_rtif->nd_base_reachable_time);
    }
    if (FLD_MATCH(TBL_ROUTE_IF_FLD_ND_REACHABLE_TIME, field_id))
    {
        sal_sprintf(str, "%u", p_rtif->nd_reachable_time);
    }
    if (FLD_MATCH(TBL_ROUTE_IF_FLD_ND_DELAY_PROBE_TIME, field_id))
    {
        sal_sprintf(str, "%u", p_rtif->nd_delay_probe_time);
    }
    if (FLD_MATCH(TBL_ROUTE_IF_FLD_ND_DAD_ATTEMPTS, field_id))
    {
        sal_sprintf(str, "%u", p_rtif->nd_dad_attempts);
    }
    if (FLD_MATCH(TBL_ROUTE_IF_FLD_ND_DST, field_id))
    {
        cdb_addr_val2str(str, MAX_CMD_STR_LEN, &p_rtif->nd_dst);
    }
    if (FLD_MATCH(TBL_ROUTE_IF_FLD_ND_LAST_RA_SOLICITED, field_id))
    {
        sal_sprintf(str, "%"PRIu64, p_rtif->nd_last_ra_solicited);
    }
    if (FLD_MATCH(TBL_ROUTE_IF_FLD_ND_LAST_RA_UNSOLICITED, field_id))
    {
        sal_sprintf(str, "%"PRIu64, p_rtif->nd_last_ra_unsolicited);
    }
    if (FLD_MATCH(TBL_ROUTE_IF_FLD_ND_RA_OCOUNT, field_id))
    {
        sal_sprintf(str, "%u", p_rtif->nd_ra_ocount);
    }
    if (FLD_MATCH(TBL_ROUTE_IF_FLD_ND_RA_SOLICITED_OCOUNT, field_id))
    {
        sal_sprintf(str, "%u", p_rtif->nd_ra_solicited_ocount);
    }
    if (FLD_MATCH(TBL_ROUTE_IF_FLD_ND_RA_INIT_UNSOLICITED_OCOUNT, field_id))
    {
        sal_sprintf(str, "%u", p_rtif->nd_ra_init_unsolicited_ocount);
    }
    if (FLD_MATCH(TBL_ROUTE_IF_FLD_ND_RA_SOLICITED_TIME, field_id))
    {
        sal_sprintf(str, "%u", p_rtif->nd_ra_solicited_time);
    }
    if (FLD_MATCH(TBL_ROUTE_IF_FLD_ND_RA_PREFIX_DEFAULT_VLIFETIME, field_id))
    {
        sal_sprintf(str, "%u", p_rtif->nd_ra_prefix_default_vlifetime);
    }
    if (FLD_MATCH(TBL_ROUTE_IF_FLD_ND_RA_PREFIX_DEFAULT_PLIFETIME, field_id))
    {
        sal_sprintf(str, "%u", p_rtif->nd_ra_prefix_default_plifetime);
    }
    if (FLD_MATCH(TBL_ROUTE_IF_FLD_ND_RA_PREFIX_DEFAULT_ONLINK, field_id))
    {
        sal_sprintf(str, "%u", p_rtif->nd_ra_prefix_default_onlink);
    }
    if (FLD_MATCH(TBL_ROUTE_IF_FLD_ND_RA_PREFIX_DEFAULT_AUTO, field_id))
    {
        sal_sprintf(str, "%u", p_rtif->nd_ra_prefix_default_auto);
    }
    if (FLD_MATCH(TBL_ROUTE_IF_FLD_ND_RS_DEST_ADDR, field_id))
    {
        cdb_addr_val2str(str, MAX_CMD_STR_LEN, &p_rtif->nd_rs_dest_addr);
    }
    if (FLD_MATCH(TBL_ROUTE_IF_FLD_T_RA_SOLICITED, field_id))
    {
        ctc_task_val2str(str, MAX_CMD_STR_LEN, p_rtif->t_ra_solicited);
    }
    if (FLD_MATCH(TBL_ROUTE_IF_FLD_T_RA_UNSOLICITED, field_id))
    {
        ctc_task_val2str(str, MAX_CMD_STR_LEN, p_rtif->t_ra_unsolicited);
    }
    if (FLD_MATCH(TBL_ROUTE_IF_FLD_T_DELETE_ALL_V6_ADDR_TIMER, field_id))
    {
        ctc_task_val2str(str, MAX_CMD_STR_LEN, p_rtif->t_delete_all_v6_addr_timer);
    }
    if (FLD_MATCH(TBL_ROUTE_IF_FLD_DHCPV6_CLIENT_FLAGS, field_id))
    {
        sal_sprintf(str, "%u", p_rtif->dhcpv6_client_flags);
    }
    if (FLD_MATCH(TBL_ROUTE_IF_FLD_DHCPV6_CLIENT_ENABLE, field_id))
    {
        sal_sprintf(str, "%u", p_rtif->dhcpv6_client_enable);
    }
    if (FLD_MATCH(TBL_ROUTE_IF_FLD_NAT_ENABLE, field_id))
    {
        sal_sprintf(str, "%u", p_rtif->nat_enable);
    }
    if (FLD_MATCH(TBL_ROUTE_IF_FLD_NAT_CATEGORY_ID, field_id))
    {
        sal_sprintf(str, "%u", p_rtif->nat_category_id);
    }
    if (FLD_MATCH(TBL_ROUTE_IF_FLD_SET_MIRROR_ENABLE, field_id))
    {
        sal_sprintf(str, "%u", p_rtif->set_mirror_enable);
    }
    if (FLD_MATCH(TBL_ROUTE_IF_FLD_T_OSPF_COST_TIMER, field_id))
    {
        ctc_task_val2str(str, MAX_CMD_STR_LEN, p_rtif->t_ospf_cost_timer);
    }
    if (FLD_MATCH(TBL_ROUTE_IF_FLD_T_SEND_ARP_TIMER, field_id))
    {
        ctc_task_val2str(str, MAX_CMD_STR_LEN, p_rtif->t_send_arp_timer);
    }
    if (FLD_MATCH(TBL_ROUTE_IF_FLD_IFC_IPV4, field_id))
    {
    }
    if (FLD_MATCH(TBL_ROUTE_IF_FLD_IFC_IPV6, field_id))
    {
    }
    if (FLD_MATCH(TBL_ROUTE_IF_FLD_OSPF_MD5_KEY, field_id))
    {
    }
    if (FLD_MATCH(TBL_ROUTE_IF_FLD_ND_PREFIX, field_id))
    {
    }
    if (FLD_MATCH(TBL_ROUTE_IF_FLD_PIM_MODE, field_id))
    {
        sal_sprintf(str, "%u", p_rtif->pim_mode);
    }
    if (FLD_MATCH(TBL_ROUTE_IF_FLD_MULTICAST, field_id))
    {
        sal_sprintf(str, "%u", p_rtif->multicast);
    }
    if (FLD_MATCH(TBL_ROUTE_IF_FLD_IGMP, field_id))
    {
        sal_sprintf(str, "%u", p_rtif->igmp);
    }
    if (FLD_MATCH(TBL_ROUTE_IF_FLD_IGMP_VERSION, field_id))
    {
        sal_sprintf(str, "%s", p_rtif->igmp_version);
    }
    if (FLD_MATCH(TBL_ROUTE_IF_FLD_IGMP_INTF_LIMIT_NUMBER, field_id))
    {
        sal_sprintf(str, "%u", p_rtif->igmp_intf_limit_number);
    }
    return str;
}

char**
tbl_route_if_table_dump(tbl_route_if_t *p_rtif, char **str)
{
    char buf[MAX_CMD_STR_LEN];
    int i = 0, j= 0;

    sal_sprintf(str[j++], "%s",
         tbl_route_if_key_name_dump(p_rtif, buf));
    for(i=1; i<TBL_ROUTE_IF_FLD_MAX; i++)
    {
        sal_sprintf(str[j++], "%s",
            tbl_route_if_field_name_dump(p_rtif, i, buf));
        sal_sprintf(str[j++], "%s",
            tbl_route_if_field_value_dump(p_rtif, i, buf));
    }
    return str;
}

int32
tbl_route_if_field_value_parser(char *str, int32 field_id, tbl_route_if_t *p_rtif)
{
    if (FLD_MATCH(TBL_ROUTE_IF_FLD_KEY, field_id))
    {
        sal_strcpy(p_rtif->key.name, str);
    }
    if (FLD_MATCH(TBL_ROUTE_IF_FLD_MTU, field_id))
    {
        int32 ret;
        p_rtif->mtu = cdb_uint_str2val(str, &ret);
    }
    if (FLD_MATCH(TBL_ROUTE_IF_FLD_MAC, field_id))
    {
        cdb_mac_addr_str2val(p_rtif->mac, str);
    }
    if (FLD_MATCH(TBL_ROUTE_IF_FLD_MODE_SET_BEFORE, field_id))
    {
        cdb_enum_str2val(glb_if_mode_strs, GLB_IF_MODE_MAX, str);
    }
    if (FLD_MATCH(TBL_ROUTE_IF_FLD_IPV4_ENABLE, field_id))
    {
        int32 ret;
        p_rtif->ipv4_enable = cdb_uint_str2val(str, &ret);
    }
    if (FLD_MATCH(TBL_ROUTE_IF_FLD_IPV6_ENABLE, field_id))
    {
        int32 ret;
        p_rtif->ipv6_enable = cdb_uint_str2val(str, &ret);
    }
    if (FLD_MATCH(TBL_ROUTE_IF_FLD_ARP_RETRANS_TIME, field_id))
    {
        int32 ret;
        p_rtif->arp_retrans_time = cdb_uint_str2val(str, &ret);
    }
    if (FLD_MATCH(TBL_ROUTE_IF_FLD_ARP_TIMEOUT, field_id))
    {
        int32 ret;
        p_rtif->arp_timeout = cdb_uint_str2val(str, &ret);
    }
    if (FLD_MATCH(TBL_ROUTE_IF_FLD_IFINDEX, field_id))
    {
        int32 ret;
        p_rtif->ifindex = cdb_uint_str2val(str, &ret);
    }
    if (FLD_MATCH(TBL_ROUTE_IF_FLD_KERNEL_IFINDEX, field_id))
    {
        int32 ret;
        p_rtif->kernel_ifindex = cdb_uint_str2val(str, &ret);
    }
    if (FLD_MATCH(TBL_ROUTE_IF_FLD_ARP_RETRY_TIMEOUT, field_id))
    {
        int32 ret;
        p_rtif->arp_retry_timeout = cdb_uint_str2val(str, &ret);
    }
    if (FLD_MATCH(TBL_ROUTE_IF_FLD_RIP_RECV_VERSION, field_id))
    {
        int32 ret;
        p_rtif->rip_recv_version = cdb_uint_str2val(str, &ret);
    }
    if (FLD_MATCH(TBL_ROUTE_IF_FLD_RIP_SEND_VERSION, field_id))
    {
        int32 ret;
        p_rtif->rip_send_version = cdb_uint_str2val(str, &ret);
    }
    if (FLD_MATCH(TBL_ROUTE_IF_FLD_RIP_V2_BROADCAST, field_id))
    {
        int32 ret;
        p_rtif->rip_v2_broadcast = cdb_uint_str2val(str, &ret);
    }
    if (FLD_MATCH(TBL_ROUTE_IF_FLD_RIP_SPLIT_HORIZON, field_id))
    {
        int32 ret;
        p_rtif->rip_split_horizon = cdb_uint_str2val(str, &ret);
    }
    if (FLD_MATCH(TBL_ROUTE_IF_FLD_RIP_POISONED_REVERSE, field_id))
    {
        int32 ret;
        p_rtif->rip_poisoned_reverse = cdb_uint_str2val(str, &ret);
    }
    if (FLD_MATCH(TBL_ROUTE_IF_FLD_RIP_AUTH_MODE, field_id))
    {
        int32 ret;
        p_rtif->rip_auth_mode = cdb_uint_str2val(str, &ret);
    }
    if (FLD_MATCH(TBL_ROUTE_IF_FLD_RIP_AUTH_STRING, field_id))
    {
        sal_strcpy(p_rtif->rip_auth_string, str);
    }
    if (FLD_MATCH(TBL_ROUTE_IF_FLD_RIPNG_SPLIT_HORIZON, field_id))
    {
        int32 ret;
        p_rtif->ripng_split_horizon = cdb_uint_str2val(str, &ret);
    }
    if (FLD_MATCH(TBL_ROUTE_IF_FLD_RIPNG_POISONED_REVERSE, field_id))
    {
        int32 ret;
        p_rtif->ripng_poisoned_reverse = cdb_uint_str2val(str, &ret);
    }
    if (FLD_MATCH(TBL_ROUTE_IF_FLD_OSPF_PRIORITY, field_id))
    {
        int32 ret;
        p_rtif->ospf_priority = cdb_uint_str2val(str, &ret);
    }
    if (FLD_MATCH(TBL_ROUTE_IF_FLD_OSPF_COST, field_id))
    {
        int32 ret;
        p_rtif->ospf_cost = cdb_uint_str2val(str, &ret);
    }
    if (FLD_MATCH(TBL_ROUTE_IF_FLD_OSPF_MTU_IGNORE, field_id))
    {
        int32 ret;
        p_rtif->ospf_mtu_ignore = cdb_uint_str2val(str, &ret);
    }
    if (FLD_MATCH(TBL_ROUTE_IF_FLD_OSPF_TIMER_HELLO, field_id))
    {
        int32 ret;
        p_rtif->ospf_timer_hello = cdb_uint_str2val(str, &ret);
    }
    if (FLD_MATCH(TBL_ROUTE_IF_FLD_OSPF_TIMER_DEAD, field_id))
    {
        int32 ret;
        p_rtif->ospf_timer_dead = cdb_uint_str2val(str, &ret);
    }
    if (FLD_MATCH(TBL_ROUTE_IF_FLD_OSPF_AUTH_TYPE, field_id))
    {
        int32 ret;
        p_rtif->ospf_auth_type = cdb_uint_str2val(str, &ret);
    }
    if (FLD_MATCH(TBL_ROUTE_IF_FLD_OSPF_AUTH_KEY, field_id))
    {
        sal_strcpy(p_rtif->ospf_auth_key, str);
    }
    if (FLD_MATCH(TBL_ROUTE_IF_FLD_OSPF_NETWORK_TYPE, field_id))
    {
        int32 ret;
        p_rtif->ospf_network_type = cdb_uint_str2val(str, &ret);
    }
    if (FLD_MATCH(TBL_ROUTE_IF_FLD_OSPF6_TIMER_HELLO, field_id))
    {
        int32 ret;
        p_rtif->ospf6_timer_hello = cdb_uint_str2val(str, &ret);
    }
    if (FLD_MATCH(TBL_ROUTE_IF_FLD_OSPF6_TIMER_DEAD, field_id))
    {
        int32 ret;
        p_rtif->ospf6_timer_dead = cdb_uint_str2val(str, &ret);
    }
    if (FLD_MATCH(TBL_ROUTE_IF_FLD_OSPF6_MTU_IGNORE, field_id))
    {
        int32 ret;
        p_rtif->ospf6_mtu_ignore = cdb_uint_str2val(str, &ret);
    }
    if (FLD_MATCH(TBL_ROUTE_IF_FLD_OSPF6_COST, field_id))
    {
        int32 ret;
        p_rtif->ospf6_cost = cdb_uint_str2val(str, &ret);
    }
    if (FLD_MATCH(TBL_ROUTE_IF_FLD_OSPF6_PRIORITY, field_id))
    {
        int32 ret;
        p_rtif->ospf6_priority = cdb_uint_str2val(str, &ret);
    }
    if (FLD_MATCH(TBL_ROUTE_IF_FLD_OSPF6_NETWORK_TYPE, field_id))
    {
        int32 ret;
        p_rtif->ospf6_network_type = cdb_uint_str2val(str, &ret);
    }
    if (FLD_MATCH(TBL_ROUTE_IF_FLD_ARP_PROXY_EN, field_id))
    {
        int32 ret;
        p_rtif->arp_proxy_en = cdb_uint_str2val(str, &ret);
    }
    if (FLD_MATCH(TBL_ROUTE_IF_FLD_LOCAL_ARP_PROXY_EN, field_id))
    {
        int32 ret;
        p_rtif->local_arp_proxy_en = cdb_uint_str2val(str, &ret);
    }
    if (FLD_MATCH(TBL_ROUTE_IF_FLD_UNICAST_RPF_EN, field_id))
    {
        int32 ret;
        p_rtif->unicast_rpf_en = cdb_uint_str2val(str, &ret);
    }
    if (FLD_MATCH(TBL_ROUTE_IF_FLD_UNICAST_RPF_EN_V6, field_id))
    {
        int32 ret;
        p_rtif->unicast_rpf_en_v6 = cdb_uint_str2val(str, &ret);
    }
    if (FLD_MATCH(TBL_ROUTE_IF_FLD_UNICAST_EN_V6, field_id))
    {
        int32 ret;
        p_rtif->unicast_en_v6 = cdb_uint_str2val(str, &ret);
    }
    if (FLD_MATCH(TBL_ROUTE_IF_FLD_IS_UP_RUNNING, field_id))
    {
        int32 ret;
        p_rtif->is_up_running = cdb_uint_str2val(str, &ret);
    }
    if (FLD_MATCH(TBL_ROUTE_IF_FLD_IPV6_LINK_LOCAL_HAD_DELETE, field_id))
    {
        int32 ret;
        p_rtif->ipv6_link_local_had_delete = cdb_uint_str2val(str, &ret);
    }
    if (FLD_MATCH(TBL_ROUTE_IF_FLD_IP_UNREACHABLE_EN, field_id))
    {
        int32 ret;
        p_rtif->ip_unreachable_en = cdb_uint_str2val(str, &ret);
    }
    if (FLD_MATCH(TBL_ROUTE_IF_FLD_IP_REDIRECTS_EN, field_id))
    {
        int32 ret;
        p_rtif->ip_redirects_en = cdb_uint_str2val(str, &ret);
    }
    if (FLD_MATCH(TBL_ROUTE_IF_FLD_DHCP_RELAY_OPTION_TRUST, field_id))
    {
        int32 ret;
        p_rtif->dhcp_relay_option_trust = cdb_uint_str2val(str, &ret);
    }
    if (FLD_MATCH(TBL_ROUTE_IF_FLD_DHCP_SERVER_GROUP, field_id))
    {
        int32 ret;
        p_rtif->dhcp_server_group = cdb_uint_str2val(str, &ret);
    }
    if (FLD_MATCH(TBL_ROUTE_IF_FLD_DHCP_PDU_ENABLED, field_id))
    {
        int32 ret;
        p_rtif->dhcp_pdu_enabled = cdb_uint_str2val(str, &ret);
    }
    if (FLD_MATCH(TBL_ROUTE_IF_FLD_DHCP_CLIENT_FLAGS, field_id))
    {
        int32 ret;
        p_rtif->dhcp_client_flags = cdb_uint_str2val(str, &ret);
    }
    if (FLD_MATCH(TBL_ROUTE_IF_FLD_DHCP_CLIENT_ENABLE, field_id))
    {
        int32 ret;
        p_rtif->dhcp_client_enable = cdb_uint_str2val(str, &ret);
    }
    if (FLD_MATCH(TBL_ROUTE_IF_FLD_DHCP_CLIENT_IPV4, field_id))
    {
        sal_strcpy(p_rtif->dhcp_client_ipv4, str);
    }
    if (FLD_MATCH(TBL_ROUTE_IF_FLD_DHCP_CLIENT_IPV4_MASK, field_id))
    {
        sal_strcpy(p_rtif->dhcp_client_ipv4_mask, str);
    }
    if (FLD_MATCH(TBL_ROUTE_IF_FLD_ARP_CURR_DYNAMIC, field_id))
    {
        int32 ret;
        p_rtif->arp_curr_dynamic = cdb_uint_str2val(str, &ret);
    }
    if (FLD_MATCH(TBL_ROUTE_IF_FLD_ARP_ATTACK_NUMBER, field_id))
    {
        int32 ret;
        p_rtif->arp_attack_number = cdb_uint64_str2val(str, &ret);
    }
    if (FLD_MATCH(TBL_ROUTE_IF_FLD_ARP_RATE_LIMIT_EN, field_id))
    {
        int32 ret;
        p_rtif->arp_rate_limit_en = cdb_uint_str2val(str, &ret);
    }
    if (FLD_MATCH(TBL_ROUTE_IF_FLD_ARP_RATE_LIMIT_PKT_MAX, field_id))
    {
        int32 ret;
        p_rtif->arp_rate_limit_pkt_max = cdb_uint_str2val(str, &ret);
    }
    if (FLD_MATCH(TBL_ROUTE_IF_FLD_ARP_RATE_LIMIT_VIOLATION, field_id))
    {
        int32 ret;
        p_rtif->arp_rate_limit_violation = cdb_uint_str2val(str, &ret);
    }
    if (FLD_MATCH(TBL_ROUTE_IF_FLD_ARP_RATE_LIMIT_PKT_CURR, field_id))
    {
        int32 ret;
        p_rtif->arp_rate_limit_pkt_curr = cdb_uint_str2val(str, &ret);
    }
    if (FLD_MATCH(TBL_ROUTE_IF_FLD_ARP_RATE_LIMIT_PORT_ABNORMAL_FLAG, field_id))
    {
        int32 ret;
        p_rtif->arp_rate_limit_port_abnormal_flag = cdb_uint_str2val(str, &ret);
    }
    if (FLD_MATCH(TBL_ROUTE_IF_FLD_VMAC1, field_id))
    {
        cdb_mac_addr_str2val(p_rtif->vmac1, str);
    }
    if (FLD_MATCH(TBL_ROUTE_IF_FLD_VMAC2, field_id))
    {
        cdb_mac_addr_str2val(p_rtif->vmac2, str);
    }
    if (FLD_MATCH(TBL_ROUTE_IF_FLD_VMAC3, field_id))
    {
        cdb_mac_addr_str2val(p_rtif->vmac3, str);
    }
    if (FLD_MATCH(TBL_ROUTE_IF_FLD_VRRP_SESSION_NUM, field_id))
    {
        int32 ret;
        p_rtif->vrrp_session_num = cdb_uint_str2val(str, &ret);
    }
    if (FLD_MATCH(TBL_ROUTE_IF_FLD_ND_FLAGS, field_id))
    {
        int32 ret;
        p_rtif->nd_flags = cdb_uint_str2val(str, &ret);
    }
    if (FLD_MATCH(TBL_ROUTE_IF_FLD_ND_RA_CURRRENT_HOP_LIMIT, field_id))
    {
        int32 ret;
        p_rtif->nd_ra_currrent_hop_limit = cdb_uint_str2val(str, &ret);
    }
    if (FLD_MATCH(TBL_ROUTE_IF_FLD_ND_RA_ADVERTISE, field_id))
    {
        int32 ret;
        p_rtif->nd_ra_advertise = cdb_uint_str2val(str, &ret);
    }
    if (FLD_MATCH(TBL_ROUTE_IF_FLD_ND_RA_ADVERTISE_MTU, field_id))
    {
        int32 ret;
        p_rtif->nd_ra_advertise_mtu = cdb_uint_str2val(str, &ret);
    }
    if (FLD_MATCH(TBL_ROUTE_IF_FLD_ND_MANAGED_CONFIG, field_id))
    {
        int32 ret;
        p_rtif->nd_managed_config = cdb_uint_str2val(str, &ret);
    }
    if (FLD_MATCH(TBL_ROUTE_IF_FLD_ND_OTHER_CONFIG, field_id))
    {
        int32 ret;
        p_rtif->nd_other_config = cdb_uint_str2val(str, &ret);
    }
    if (FLD_MATCH(TBL_ROUTE_IF_FLD_ND_RA_INTERVAL_MIN, field_id))
    {
        int32 ret;
        p_rtif->nd_ra_interval_min = cdb_uint_str2val(str, &ret);
    }
    if (FLD_MATCH(TBL_ROUTE_IF_FLD_ND_RA_INTERVAL_MAX, field_id))
    {
        int32 ret;
        p_rtif->nd_ra_interval_max = cdb_uint_str2val(str, &ret);
    }
    if (FLD_MATCH(TBL_ROUTE_IF_FLD_ND_RA_INTERVAL, field_id))
    {
        int32 ret;
        p_rtif->nd_ra_interval = cdb_uint_str2val(str, &ret);
    }
    if (FLD_MATCH(TBL_ROUTE_IF_FLD_ND_RA_MIN_DELAY, field_id))
    {
        int32 ret;
        p_rtif->nd_ra_min_delay = cdb_uint_str2val(str, &ret);
    }
    if (FLD_MATCH(TBL_ROUTE_IF_FLD_ND_RETRANSMIT_TIMER, field_id))
    {
        int32 ret;
        p_rtif->nd_retransmit_timer = cdb_uint_str2val(str, &ret);
    }
    if (FLD_MATCH(TBL_ROUTE_IF_FLD_ND_ROUTER_LIFETIME, field_id))
    {
        int32 ret;
        p_rtif->nd_router_lifetime = cdb_uint_str2val(str, &ret);
    }
    if (FLD_MATCH(TBL_ROUTE_IF_FLD_ND_BASE_REACHABLE_TIME, field_id))
    {
        int32 ret;
        p_rtif->nd_base_reachable_time = cdb_uint_str2val(str, &ret);
    }
    if (FLD_MATCH(TBL_ROUTE_IF_FLD_ND_REACHABLE_TIME, field_id))
    {
        int32 ret;
        p_rtif->nd_reachable_time = cdb_uint_str2val(str, &ret);
    }
    if (FLD_MATCH(TBL_ROUTE_IF_FLD_ND_DELAY_PROBE_TIME, field_id))
    {
        int32 ret;
        p_rtif->nd_delay_probe_time = cdb_uint_str2val(str, &ret);
    }
    if (FLD_MATCH(TBL_ROUTE_IF_FLD_ND_DAD_ATTEMPTS, field_id))
    {
        int32 ret;
        p_rtif->nd_dad_attempts = cdb_uint_str2val(str, &ret);
    }
    if (FLD_MATCH(TBL_ROUTE_IF_FLD_ND_DST, field_id))
    {
        cdb_addr_str2val(&p_rtif->nd_dst, str, 0);
    }
    if (FLD_MATCH(TBL_ROUTE_IF_FLD_ND_LAST_RA_SOLICITED, field_id))
    {
        int32 ret;
        p_rtif->nd_last_ra_solicited = cdb_uint64_str2val(str, &ret);
    }
    if (FLD_MATCH(TBL_ROUTE_IF_FLD_ND_LAST_RA_UNSOLICITED, field_id))
    {
        int32 ret;
        p_rtif->nd_last_ra_unsolicited = cdb_uint64_str2val(str, &ret);
    }
    if (FLD_MATCH(TBL_ROUTE_IF_FLD_ND_RA_OCOUNT, field_id))
    {
        int32 ret;
        p_rtif->nd_ra_ocount = cdb_uint_str2val(str, &ret);
    }
    if (FLD_MATCH(TBL_ROUTE_IF_FLD_ND_RA_SOLICITED_OCOUNT, field_id))
    {
        int32 ret;
        p_rtif->nd_ra_solicited_ocount = cdb_uint_str2val(str, &ret);
    }
    if (FLD_MATCH(TBL_ROUTE_IF_FLD_ND_RA_INIT_UNSOLICITED_OCOUNT, field_id))
    {
        int32 ret;
        p_rtif->nd_ra_init_unsolicited_ocount = cdb_uint_str2val(str, &ret);
    }
    if (FLD_MATCH(TBL_ROUTE_IF_FLD_ND_RA_SOLICITED_TIME, field_id))
    {
        int32 ret;
        p_rtif->nd_ra_solicited_time = cdb_uint_str2val(str, &ret);
    }
    if (FLD_MATCH(TBL_ROUTE_IF_FLD_ND_RA_PREFIX_DEFAULT_VLIFETIME, field_id))
    {
        int32 ret;
        p_rtif->nd_ra_prefix_default_vlifetime = cdb_uint_str2val(str, &ret);
    }
    if (FLD_MATCH(TBL_ROUTE_IF_FLD_ND_RA_PREFIX_DEFAULT_PLIFETIME, field_id))
    {
        int32 ret;
        p_rtif->nd_ra_prefix_default_plifetime = cdb_uint_str2val(str, &ret);
    }
    if (FLD_MATCH(TBL_ROUTE_IF_FLD_ND_RA_PREFIX_DEFAULT_ONLINK, field_id))
    {
        int32 ret;
        p_rtif->nd_ra_prefix_default_onlink = cdb_uint_str2val(str, &ret);
    }
    if (FLD_MATCH(TBL_ROUTE_IF_FLD_ND_RA_PREFIX_DEFAULT_AUTO, field_id))
    {
        int32 ret;
        p_rtif->nd_ra_prefix_default_auto = cdb_uint_str2val(str, &ret);
    }
    if (FLD_MATCH(TBL_ROUTE_IF_FLD_ND_RS_DEST_ADDR, field_id))
    {
        cdb_addr_str2val(&p_rtif->nd_rs_dest_addr, str, 0);
    }
    if (FLD_MATCH(TBL_ROUTE_IF_FLD_T_RA_SOLICITED, field_id))
    {
        ctc_task_str2val(str, p_rtif->t_ra_solicited);
    }
    if (FLD_MATCH(TBL_ROUTE_IF_FLD_T_RA_UNSOLICITED, field_id))
    {
        ctc_task_str2val(str, p_rtif->t_ra_unsolicited);
    }
    if (FLD_MATCH(TBL_ROUTE_IF_FLD_T_DELETE_ALL_V6_ADDR_TIMER, field_id))
    {
        ctc_task_str2val(str, p_rtif->t_delete_all_v6_addr_timer);
    }
    if (FLD_MATCH(TBL_ROUTE_IF_FLD_DHCPV6_CLIENT_FLAGS, field_id))
    {
        int32 ret;
        p_rtif->dhcpv6_client_flags = cdb_uint_str2val(str, &ret);
    }
    if (FLD_MATCH(TBL_ROUTE_IF_FLD_DHCPV6_CLIENT_ENABLE, field_id))
    {
        int32 ret;
        p_rtif->dhcpv6_client_enable = cdb_uint_str2val(str, &ret);
    }
    if (FLD_MATCH(TBL_ROUTE_IF_FLD_NAT_ENABLE, field_id))
    {
        int32 ret;
        p_rtif->nat_enable = cdb_uint_str2val(str, &ret);
    }
    if (FLD_MATCH(TBL_ROUTE_IF_FLD_NAT_CATEGORY_ID, field_id))
    {
        int32 ret;
        p_rtif->nat_category_id = cdb_uint_str2val(str, &ret);
    }
    if (FLD_MATCH(TBL_ROUTE_IF_FLD_SET_MIRROR_ENABLE, field_id))
    {
        int32 ret;
        p_rtif->set_mirror_enable = cdb_uint_str2val(str, &ret);
    }
    if (FLD_MATCH(TBL_ROUTE_IF_FLD_T_OSPF_COST_TIMER, field_id))
    {
        ctc_task_str2val(str, p_rtif->t_ospf_cost_timer);
    }
    if (FLD_MATCH(TBL_ROUTE_IF_FLD_T_SEND_ARP_TIMER, field_id))
    {
        ctc_task_str2val(str, p_rtif->t_send_arp_timer);
    }
    if (FLD_MATCH(TBL_ROUTE_IF_FLD_IFC_IPV4, field_id))
    {
    }
    if (FLD_MATCH(TBL_ROUTE_IF_FLD_IFC_IPV6, field_id))
    {
    }
    if (FLD_MATCH(TBL_ROUTE_IF_FLD_OSPF_MD5_KEY, field_id))
    {
    }
    if (FLD_MATCH(TBL_ROUTE_IF_FLD_ND_PREFIX, field_id))
    {
    }
    if (FLD_MATCH(TBL_ROUTE_IF_FLD_PIM_MODE, field_id))
    {
        int32 ret;
        p_rtif->pim_mode = cdb_uint_str2val(str, &ret);
    }
    if (FLD_MATCH(TBL_ROUTE_IF_FLD_MULTICAST, field_id))
    {
        int32 ret;
        p_rtif->multicast = cdb_uint_str2val(str, &ret);
    }
    if (FLD_MATCH(TBL_ROUTE_IF_FLD_IGMP, field_id))
    {
        int32 ret;
        p_rtif->igmp = cdb_uint_str2val(str, &ret);
    }
    if (FLD_MATCH(TBL_ROUTE_IF_FLD_IGMP_VERSION, field_id))
    {
        sal_strcpy(p_rtif->igmp_version, str);
    }
    if (FLD_MATCH(TBL_ROUTE_IF_FLD_IGMP_INTF_LIMIT_NUMBER, field_id))
    {
        int32 ret;
        p_rtif->igmp_intf_limit_number = cdb_uint_str2val(str, &ret);
    }
    return PM_E_NONE;
}

int32
tbl_route_if_table_parser(char** array, char* key_value,tbl_route_if_t *p_rtif)
{
    int32 rc;
    int i = 0, j= 0;
    cdb_node_t *p_node = cdb_get_tbl(TBL_ROUTE_IF);
    cdb_tbl_info_t *p_tbl_info = p_node->tbl_info;

    PM_E_RETURN(tbl_route_if_key_str2val(key_value, p_rtif));

    for(i=1; i<TBL_ROUTE_IF_FLD_MAX; i++)
    {
        if(sal_strncmp(array[j++], p_tbl_info->field[i].name, sal_strlen(p_tbl_info->field[i].name)))
        {
             return PM_E_INVALID_PARAM;
        }
        PM_E_RETURN(tbl_route_if_field_value_parser( array[j++], i, p_rtif));
    }

    return PM_E_NONE;
}

int32
tbl_route_if_dump_one(tbl_route_if_t *p_rtif, tbl_iter_args_t *pargs)
{
    cdb_node_t *p_node = cdb_get_tbl(TBL_ROUTE_IF);
    field_parse_t *p_field = pargs->argv[0];
    FILE *fp = pargs->argv[1];
    cdb_tbl_info_t *p_tbl_info = p_node->tbl_info;

    ds_connected_t *p_db_connected = NULL;
    ds_connected_v6_t *p_db_connected_v6 = NULL;
    ds_ospf_auth_t *p_db_ospf_auth = NULL;
    ds_nd_prefix_t *p_db_nd_prefix = NULL;
    ctclib_slistnode_t *listnode = NULL;

    char buf[MAX_CMD_STR_LEN];

    if (FLD_MATCH(TBL_ROUTE_IF_FLD_KEY, p_field->field_id[0]))
    {
        sal_fprintf(fp, "/%s"CMD_EQUAL_STR"%s", p_tbl_info->field[TBL_ROUTE_IF_FLD_KEY].name,
            p_rtif->key.name);
    }
    if (FLD_MATCH(TBL_ROUTE_IF_FLD_MTU, p_field->field_id[0]))
    {
        sal_fprintf(fp, "/%s"CMD_EQUAL_STR"%u", p_tbl_info->field[TBL_ROUTE_IF_FLD_MTU].name,
            p_rtif->mtu);
    }
    if (FLD_MATCH(TBL_ROUTE_IF_FLD_MAC, p_field->field_id[0]))
    {
        sal_fprintf(fp, "/%s"CMD_EQUAL_STR"%s", p_tbl_info->field[TBL_ROUTE_IF_FLD_MAC].name, 
            cdb_mac_addr_val2str(buf, MAX_CMD_STR_LEN, p_rtif->mac));
    }
    if (FLD_MATCH(TBL_ROUTE_IF_FLD_MODE_SET_BEFORE, p_field->field_id[0]))
    {
        sal_fprintf(fp, "/%s"CMD_EQUAL_STR"%s", p_tbl_info->field[TBL_ROUTE_IF_FLD_MODE_SET_BEFORE].name, 
            cdb_enum_val2str(glb_if_mode_strs, GLB_IF_MODE_MAX, p_rtif->mode_set_before));
    }
    if (FLD_MATCH(TBL_ROUTE_IF_FLD_IPV4_ENABLE, p_field->field_id[0]))
    {
        sal_fprintf(fp, "/%s"CMD_EQUAL_STR"%u", p_tbl_info->field[TBL_ROUTE_IF_FLD_IPV4_ENABLE].name,
            p_rtif->ipv4_enable);
    }
    if (FLD_MATCH(TBL_ROUTE_IF_FLD_IPV6_ENABLE, p_field->field_id[0]))
    {
        sal_fprintf(fp, "/%s"CMD_EQUAL_STR"%u", p_tbl_info->field[TBL_ROUTE_IF_FLD_IPV6_ENABLE].name,
            p_rtif->ipv6_enable);
    }
    if (FLD_MATCH(TBL_ROUTE_IF_FLD_ARP_RETRANS_TIME, p_field->field_id[0]))
    {
        sal_fprintf(fp, "/%s"CMD_EQUAL_STR"%u", p_tbl_info->field[TBL_ROUTE_IF_FLD_ARP_RETRANS_TIME].name,
            p_rtif->arp_retrans_time);
    }
    if (FLD_MATCH(TBL_ROUTE_IF_FLD_ARP_TIMEOUT, p_field->field_id[0]))
    {
        sal_fprintf(fp, "/%s"CMD_EQUAL_STR"%u", p_tbl_info->field[TBL_ROUTE_IF_FLD_ARP_TIMEOUT].name,
            p_rtif->arp_timeout);
    }
    if (FLD_MATCH(TBL_ROUTE_IF_FLD_IFINDEX, p_field->field_id[0]))
    {
        sal_fprintf(fp, "/%s"CMD_EQUAL_STR"%u", p_tbl_info->field[TBL_ROUTE_IF_FLD_IFINDEX].name,
            p_rtif->ifindex);
    }
    if (FLD_MATCH(TBL_ROUTE_IF_FLD_KERNEL_IFINDEX, p_field->field_id[0]))
    {
        sal_fprintf(fp, "/%s"CMD_EQUAL_STR"%u", p_tbl_info->field[TBL_ROUTE_IF_FLD_KERNEL_IFINDEX].name,
            p_rtif->kernel_ifindex);
    }
    if (FLD_MATCH(TBL_ROUTE_IF_FLD_ARP_RETRY_TIMEOUT, p_field->field_id[0]))
    {
        sal_fprintf(fp, "/%s"CMD_EQUAL_STR"%u", p_tbl_info->field[TBL_ROUTE_IF_FLD_ARP_RETRY_TIMEOUT].name,
            p_rtif->arp_retry_timeout);
    }
    if (FLD_MATCH(TBL_ROUTE_IF_FLD_RIP_RECV_VERSION, p_field->field_id[0]))
    {
        sal_fprintf(fp, "/%s"CMD_EQUAL_STR"%u", p_tbl_info->field[TBL_ROUTE_IF_FLD_RIP_RECV_VERSION].name,
            p_rtif->rip_recv_version);
    }
    if (FLD_MATCH(TBL_ROUTE_IF_FLD_RIP_SEND_VERSION, p_field->field_id[0]))
    {
        sal_fprintf(fp, "/%s"CMD_EQUAL_STR"%u", p_tbl_info->field[TBL_ROUTE_IF_FLD_RIP_SEND_VERSION].name,
            p_rtif->rip_send_version);
    }
    if (FLD_MATCH(TBL_ROUTE_IF_FLD_RIP_V2_BROADCAST, p_field->field_id[0]))
    {
        sal_fprintf(fp, "/%s"CMD_EQUAL_STR"%u", p_tbl_info->field[TBL_ROUTE_IF_FLD_RIP_V2_BROADCAST].name,
            p_rtif->rip_v2_broadcast);
    }
    if (FLD_MATCH(TBL_ROUTE_IF_FLD_RIP_SPLIT_HORIZON, p_field->field_id[0]))
    {
        sal_fprintf(fp, "/%s"CMD_EQUAL_STR"%u", p_tbl_info->field[TBL_ROUTE_IF_FLD_RIP_SPLIT_HORIZON].name,
            p_rtif->rip_split_horizon);
    }
    if (FLD_MATCH(TBL_ROUTE_IF_FLD_RIP_POISONED_REVERSE, p_field->field_id[0]))
    {
        sal_fprintf(fp, "/%s"CMD_EQUAL_STR"%u", p_tbl_info->field[TBL_ROUTE_IF_FLD_RIP_POISONED_REVERSE].name,
            p_rtif->rip_poisoned_reverse);
    }
    if (FLD_MATCH(TBL_ROUTE_IF_FLD_RIP_AUTH_MODE, p_field->field_id[0]))
    {
        sal_fprintf(fp, "/%s"CMD_EQUAL_STR"%u", p_tbl_info->field[TBL_ROUTE_IF_FLD_RIP_AUTH_MODE].name,
            p_rtif->rip_auth_mode);
    }
    if (FLD_MATCH(TBL_ROUTE_IF_FLD_RIP_AUTH_STRING, p_field->field_id[0]))
    {
        sal_fprintf(fp, "/%s"CMD_EQUAL_STR"%s", p_tbl_info->field[TBL_ROUTE_IF_FLD_RIP_AUTH_STRING].name,
            p_rtif->rip_auth_string);
    }
    if (FLD_MATCH(TBL_ROUTE_IF_FLD_RIPNG_SPLIT_HORIZON, p_field->field_id[0]))
    {
        sal_fprintf(fp, "/%s"CMD_EQUAL_STR"%u", p_tbl_info->field[TBL_ROUTE_IF_FLD_RIPNG_SPLIT_HORIZON].name,
            p_rtif->ripng_split_horizon);
    }
    if (FLD_MATCH(TBL_ROUTE_IF_FLD_RIPNG_POISONED_REVERSE, p_field->field_id[0]))
    {
        sal_fprintf(fp, "/%s"CMD_EQUAL_STR"%u", p_tbl_info->field[TBL_ROUTE_IF_FLD_RIPNG_POISONED_REVERSE].name,
            p_rtif->ripng_poisoned_reverse);
    }
    if (FLD_MATCH(TBL_ROUTE_IF_FLD_OSPF_PRIORITY, p_field->field_id[0]))
    {
        sal_fprintf(fp, "/%s"CMD_EQUAL_STR"%u", p_tbl_info->field[TBL_ROUTE_IF_FLD_OSPF_PRIORITY].name,
            p_rtif->ospf_priority);
    }
    if (FLD_MATCH(TBL_ROUTE_IF_FLD_OSPF_COST, p_field->field_id[0]))
    {
        sal_fprintf(fp, "/%s"CMD_EQUAL_STR"%u", p_tbl_info->field[TBL_ROUTE_IF_FLD_OSPF_COST].name,
            p_rtif->ospf_cost);
    }
    if (FLD_MATCH(TBL_ROUTE_IF_FLD_OSPF_MTU_IGNORE, p_field->field_id[0]))
    {
        sal_fprintf(fp, "/%s"CMD_EQUAL_STR"%u", p_tbl_info->field[TBL_ROUTE_IF_FLD_OSPF_MTU_IGNORE].name,
            p_rtif->ospf_mtu_ignore);
    }
    if (FLD_MATCH(TBL_ROUTE_IF_FLD_OSPF_TIMER_HELLO, p_field->field_id[0]))
    {
        sal_fprintf(fp, "/%s"CMD_EQUAL_STR"%u", p_tbl_info->field[TBL_ROUTE_IF_FLD_OSPF_TIMER_HELLO].name,
            p_rtif->ospf_timer_hello);
    }
    if (FLD_MATCH(TBL_ROUTE_IF_FLD_OSPF_TIMER_DEAD, p_field->field_id[0]))
    {
        sal_fprintf(fp, "/%s"CMD_EQUAL_STR"%u", p_tbl_info->field[TBL_ROUTE_IF_FLD_OSPF_TIMER_DEAD].name,
            p_rtif->ospf_timer_dead);
    }
    if (FLD_MATCH(TBL_ROUTE_IF_FLD_OSPF_AUTH_TYPE, p_field->field_id[0]))
    {
        sal_fprintf(fp, "/%s"CMD_EQUAL_STR"%u", p_tbl_info->field[TBL_ROUTE_IF_FLD_OSPF_AUTH_TYPE].name,
            p_rtif->ospf_auth_type);
    }
    if (FLD_MATCH(TBL_ROUTE_IF_FLD_OSPF_AUTH_KEY, p_field->field_id[0]))
    {
        sal_fprintf(fp, "/%s"CMD_EQUAL_STR"%s", p_tbl_info->field[TBL_ROUTE_IF_FLD_OSPF_AUTH_KEY].name,
            p_rtif->ospf_auth_key);
    }
    if (FLD_MATCH(TBL_ROUTE_IF_FLD_OSPF_NETWORK_TYPE, p_field->field_id[0]))
    {
        sal_fprintf(fp, "/%s"CMD_EQUAL_STR"%u", p_tbl_info->field[TBL_ROUTE_IF_FLD_OSPF_NETWORK_TYPE].name,
            p_rtif->ospf_network_type);
    }
    if (FLD_MATCH(TBL_ROUTE_IF_FLD_OSPF6_TIMER_HELLO, p_field->field_id[0]))
    {
        sal_fprintf(fp, "/%s"CMD_EQUAL_STR"%u", p_tbl_info->field[TBL_ROUTE_IF_FLD_OSPF6_TIMER_HELLO].name,
            p_rtif->ospf6_timer_hello);
    }
    if (FLD_MATCH(TBL_ROUTE_IF_FLD_OSPF6_TIMER_DEAD, p_field->field_id[0]))
    {
        sal_fprintf(fp, "/%s"CMD_EQUAL_STR"%u", p_tbl_info->field[TBL_ROUTE_IF_FLD_OSPF6_TIMER_DEAD].name,
            p_rtif->ospf6_timer_dead);
    }
    if (FLD_MATCH(TBL_ROUTE_IF_FLD_OSPF6_MTU_IGNORE, p_field->field_id[0]))
    {
        sal_fprintf(fp, "/%s"CMD_EQUAL_STR"%u", p_tbl_info->field[TBL_ROUTE_IF_FLD_OSPF6_MTU_IGNORE].name,
            p_rtif->ospf6_mtu_ignore);
    }
    if (FLD_MATCH(TBL_ROUTE_IF_FLD_OSPF6_COST, p_field->field_id[0]))
    {
        sal_fprintf(fp, "/%s"CMD_EQUAL_STR"%u", p_tbl_info->field[TBL_ROUTE_IF_FLD_OSPF6_COST].name,
            p_rtif->ospf6_cost);
    }
    if (FLD_MATCH(TBL_ROUTE_IF_FLD_OSPF6_PRIORITY, p_field->field_id[0]))
    {
        sal_fprintf(fp, "/%s"CMD_EQUAL_STR"%u", p_tbl_info->field[TBL_ROUTE_IF_FLD_OSPF6_PRIORITY].name,
            p_rtif->ospf6_priority);
    }
    if (FLD_MATCH(TBL_ROUTE_IF_FLD_OSPF6_NETWORK_TYPE, p_field->field_id[0]))
    {
        sal_fprintf(fp, "/%s"CMD_EQUAL_STR"%u", p_tbl_info->field[TBL_ROUTE_IF_FLD_OSPF6_NETWORK_TYPE].name,
            p_rtif->ospf6_network_type);
    }
    if (FLD_MATCH(TBL_ROUTE_IF_FLD_ARP_PROXY_EN, p_field->field_id[0]))
    {
        sal_fprintf(fp, "/%s"CMD_EQUAL_STR"%u", p_tbl_info->field[TBL_ROUTE_IF_FLD_ARP_PROXY_EN].name,
            p_rtif->arp_proxy_en);
    }
    if (FLD_MATCH(TBL_ROUTE_IF_FLD_LOCAL_ARP_PROXY_EN, p_field->field_id[0]))
    {
        sal_fprintf(fp, "/%s"CMD_EQUAL_STR"%u", p_tbl_info->field[TBL_ROUTE_IF_FLD_LOCAL_ARP_PROXY_EN].name,
            p_rtif->local_arp_proxy_en);
    }
    if (FLD_MATCH(TBL_ROUTE_IF_FLD_UNICAST_RPF_EN, p_field->field_id[0]))
    {
        sal_fprintf(fp, "/%s"CMD_EQUAL_STR"%u", p_tbl_info->field[TBL_ROUTE_IF_FLD_UNICAST_RPF_EN].name,
            p_rtif->unicast_rpf_en);
    }
    if (FLD_MATCH(TBL_ROUTE_IF_FLD_UNICAST_RPF_EN_V6, p_field->field_id[0]))
    {
        sal_fprintf(fp, "/%s"CMD_EQUAL_STR"%u", p_tbl_info->field[TBL_ROUTE_IF_FLD_UNICAST_RPF_EN_V6].name,
            p_rtif->unicast_rpf_en_v6);
    }
    if (FLD_MATCH(TBL_ROUTE_IF_FLD_UNICAST_EN_V6, p_field->field_id[0]))
    {
        sal_fprintf(fp, "/%s"CMD_EQUAL_STR"%u", p_tbl_info->field[TBL_ROUTE_IF_FLD_UNICAST_EN_V6].name,
            p_rtif->unicast_en_v6);
    }
    if (FLD_MATCH(TBL_ROUTE_IF_FLD_IS_UP_RUNNING, p_field->field_id[0]))
    {
        sal_fprintf(fp, "/%s"CMD_EQUAL_STR"%u", p_tbl_info->field[TBL_ROUTE_IF_FLD_IS_UP_RUNNING].name,
            p_rtif->is_up_running);
    }
    if (FLD_MATCH(TBL_ROUTE_IF_FLD_IPV6_LINK_LOCAL_HAD_DELETE, p_field->field_id[0]))
    {
        sal_fprintf(fp, "/%s"CMD_EQUAL_STR"%u", p_tbl_info->field[TBL_ROUTE_IF_FLD_IPV6_LINK_LOCAL_HAD_DELETE].name,
            p_rtif->ipv6_link_local_had_delete);
    }
    if (FLD_MATCH(TBL_ROUTE_IF_FLD_IP_UNREACHABLE_EN, p_field->field_id[0]))
    {
        sal_fprintf(fp, "/%s"CMD_EQUAL_STR"%u", p_tbl_info->field[TBL_ROUTE_IF_FLD_IP_UNREACHABLE_EN].name,
            p_rtif->ip_unreachable_en);
    }
    if (FLD_MATCH(TBL_ROUTE_IF_FLD_IP_REDIRECTS_EN, p_field->field_id[0]))
    {
        sal_fprintf(fp, "/%s"CMD_EQUAL_STR"%u", p_tbl_info->field[TBL_ROUTE_IF_FLD_IP_REDIRECTS_EN].name,
            p_rtif->ip_redirects_en);
    }
    if (FLD_MATCH(TBL_ROUTE_IF_FLD_DHCP_RELAY_OPTION_TRUST, p_field->field_id[0]))
    {
        sal_fprintf(fp, "/%s"CMD_EQUAL_STR"%u", p_tbl_info->field[TBL_ROUTE_IF_FLD_DHCP_RELAY_OPTION_TRUST].name,
            p_rtif->dhcp_relay_option_trust);
    }
    if (FLD_MATCH(TBL_ROUTE_IF_FLD_DHCP_SERVER_GROUP, p_field->field_id[0]))
    {
        sal_fprintf(fp, "/%s"CMD_EQUAL_STR"%u", p_tbl_info->field[TBL_ROUTE_IF_FLD_DHCP_SERVER_GROUP].name,
            p_rtif->dhcp_server_group);
    }
    if (FLD_MATCH(TBL_ROUTE_IF_FLD_DHCP_PDU_ENABLED, p_field->field_id[0]))
    {
        sal_fprintf(fp, "/%s"CMD_EQUAL_STR"%u", p_tbl_info->field[TBL_ROUTE_IF_FLD_DHCP_PDU_ENABLED].name,
            p_rtif->dhcp_pdu_enabled);
    }
    if (FLD_MATCH(TBL_ROUTE_IF_FLD_DHCP_CLIENT_FLAGS, p_field->field_id[0]))
    {
        sal_fprintf(fp, "/%s"CMD_EQUAL_STR"%u", p_tbl_info->field[TBL_ROUTE_IF_FLD_DHCP_CLIENT_FLAGS].name,
            p_rtif->dhcp_client_flags);
    }
    if (FLD_MATCH(TBL_ROUTE_IF_FLD_DHCP_CLIENT_ENABLE, p_field->field_id[0]))
    {
        sal_fprintf(fp, "/%s"CMD_EQUAL_STR"%u", p_tbl_info->field[TBL_ROUTE_IF_FLD_DHCP_CLIENT_ENABLE].name,
            p_rtif->dhcp_client_enable);
    }
    if (FLD_MATCH(TBL_ROUTE_IF_FLD_DHCP_CLIENT_IPV4, p_field->field_id[0]))
    {
        sal_fprintf(fp, "/%s"CMD_EQUAL_STR"%s", p_tbl_info->field[TBL_ROUTE_IF_FLD_DHCP_CLIENT_IPV4].name,
            p_rtif->dhcp_client_ipv4);
    }
    if (FLD_MATCH(TBL_ROUTE_IF_FLD_DHCP_CLIENT_IPV4_MASK, p_field->field_id[0]))
    {
        sal_fprintf(fp, "/%s"CMD_EQUAL_STR"%s", p_tbl_info->field[TBL_ROUTE_IF_FLD_DHCP_CLIENT_IPV4_MASK].name,
            p_rtif->dhcp_client_ipv4_mask);
    }
    if (FLD_MATCH(TBL_ROUTE_IF_FLD_ARP_CURR_DYNAMIC, p_field->field_id[0]))
    {
        sal_fprintf(fp, "/%s"CMD_EQUAL_STR"%u", p_tbl_info->field[TBL_ROUTE_IF_FLD_ARP_CURR_DYNAMIC].name,
            p_rtif->arp_curr_dynamic);
    }
    if (FLD_MATCH(TBL_ROUTE_IF_FLD_ARP_ATTACK_NUMBER, p_field->field_id[0]))
    {
        sal_fprintf(fp, "/%s"CMD_EQUAL_STR"%"PRIu64, p_tbl_info->field[TBL_ROUTE_IF_FLD_ARP_ATTACK_NUMBER].name,
            p_rtif->arp_attack_number);
    }
    if (FLD_MATCH(TBL_ROUTE_IF_FLD_ARP_RATE_LIMIT_EN, p_field->field_id[0]))
    {
        sal_fprintf(fp, "/%s"CMD_EQUAL_STR"%u", p_tbl_info->field[TBL_ROUTE_IF_FLD_ARP_RATE_LIMIT_EN].name,
            p_rtif->arp_rate_limit_en);
    }
    if (FLD_MATCH(TBL_ROUTE_IF_FLD_ARP_RATE_LIMIT_PKT_MAX, p_field->field_id[0]))
    {
        sal_fprintf(fp, "/%s"CMD_EQUAL_STR"%u", p_tbl_info->field[TBL_ROUTE_IF_FLD_ARP_RATE_LIMIT_PKT_MAX].name,
            p_rtif->arp_rate_limit_pkt_max);
    }
    if (FLD_MATCH(TBL_ROUTE_IF_FLD_ARP_RATE_LIMIT_VIOLATION, p_field->field_id[0]))
    {
        sal_fprintf(fp, "/%s"CMD_EQUAL_STR"%u", p_tbl_info->field[TBL_ROUTE_IF_FLD_ARP_RATE_LIMIT_VIOLATION].name,
            p_rtif->arp_rate_limit_violation);
    }
    if (FLD_MATCH(TBL_ROUTE_IF_FLD_ARP_RATE_LIMIT_PKT_CURR, p_field->field_id[0]))
    {
        sal_fprintf(fp, "/%s"CMD_EQUAL_STR"%u", p_tbl_info->field[TBL_ROUTE_IF_FLD_ARP_RATE_LIMIT_PKT_CURR].name,
            p_rtif->arp_rate_limit_pkt_curr);
    }
    if (FLD_MATCH(TBL_ROUTE_IF_FLD_ARP_RATE_LIMIT_PORT_ABNORMAL_FLAG, p_field->field_id[0]))
    {
        sal_fprintf(fp, "/%s"CMD_EQUAL_STR"%u", p_tbl_info->field[TBL_ROUTE_IF_FLD_ARP_RATE_LIMIT_PORT_ABNORMAL_FLAG].name,
            p_rtif->arp_rate_limit_port_abnormal_flag);
    }
    if (FLD_MATCH(TBL_ROUTE_IF_FLD_VMAC1, p_field->field_id[0]))
    {
        sal_fprintf(fp, "/%s"CMD_EQUAL_STR"%s", p_tbl_info->field[TBL_ROUTE_IF_FLD_VMAC1].name, 
            cdb_mac_addr_val2str(buf, MAX_CMD_STR_LEN, p_rtif->vmac1));
    }
    if (FLD_MATCH(TBL_ROUTE_IF_FLD_VMAC2, p_field->field_id[0]))
    {
        sal_fprintf(fp, "/%s"CMD_EQUAL_STR"%s", p_tbl_info->field[TBL_ROUTE_IF_FLD_VMAC2].name, 
            cdb_mac_addr_val2str(buf, MAX_CMD_STR_LEN, p_rtif->vmac2));
    }
    if (FLD_MATCH(TBL_ROUTE_IF_FLD_VMAC3, p_field->field_id[0]))
    {
        sal_fprintf(fp, "/%s"CMD_EQUAL_STR"%s", p_tbl_info->field[TBL_ROUTE_IF_FLD_VMAC3].name, 
            cdb_mac_addr_val2str(buf, MAX_CMD_STR_LEN, p_rtif->vmac3));
    }
    if (FLD_MATCH(TBL_ROUTE_IF_FLD_VRRP_SESSION_NUM, p_field->field_id[0]))
    {
        sal_fprintf(fp, "/%s"CMD_EQUAL_STR"%u", p_tbl_info->field[TBL_ROUTE_IF_FLD_VRRP_SESSION_NUM].name,
            p_rtif->vrrp_session_num);
    }
    if (FLD_MATCH(TBL_ROUTE_IF_FLD_ND_FLAGS, p_field->field_id[0]))
    {
        sal_fprintf(fp, "/%s"CMD_EQUAL_STR"%u", p_tbl_info->field[TBL_ROUTE_IF_FLD_ND_FLAGS].name,
            p_rtif->nd_flags);
    }
    if (FLD_MATCH(TBL_ROUTE_IF_FLD_ND_RA_CURRRENT_HOP_LIMIT, p_field->field_id[0]))
    {
        sal_fprintf(fp, "/%s"CMD_EQUAL_STR"%u", p_tbl_info->field[TBL_ROUTE_IF_FLD_ND_RA_CURRRENT_HOP_LIMIT].name,
            p_rtif->nd_ra_currrent_hop_limit);
    }
    if (FLD_MATCH(TBL_ROUTE_IF_FLD_ND_RA_ADVERTISE, p_field->field_id[0]))
    {
        sal_fprintf(fp, "/%s"CMD_EQUAL_STR"%u", p_tbl_info->field[TBL_ROUTE_IF_FLD_ND_RA_ADVERTISE].name,
            p_rtif->nd_ra_advertise);
    }
    if (FLD_MATCH(TBL_ROUTE_IF_FLD_ND_RA_ADVERTISE_MTU, p_field->field_id[0]))
    {
        sal_fprintf(fp, "/%s"CMD_EQUAL_STR"%u", p_tbl_info->field[TBL_ROUTE_IF_FLD_ND_RA_ADVERTISE_MTU].name,
            p_rtif->nd_ra_advertise_mtu);
    }
    if (FLD_MATCH(TBL_ROUTE_IF_FLD_ND_MANAGED_CONFIG, p_field->field_id[0]))
    {
        sal_fprintf(fp, "/%s"CMD_EQUAL_STR"%u", p_tbl_info->field[TBL_ROUTE_IF_FLD_ND_MANAGED_CONFIG].name,
            p_rtif->nd_managed_config);
    }
    if (FLD_MATCH(TBL_ROUTE_IF_FLD_ND_OTHER_CONFIG, p_field->field_id[0]))
    {
        sal_fprintf(fp, "/%s"CMD_EQUAL_STR"%u", p_tbl_info->field[TBL_ROUTE_IF_FLD_ND_OTHER_CONFIG].name,
            p_rtif->nd_other_config);
    }
    if (FLD_MATCH(TBL_ROUTE_IF_FLD_ND_RA_INTERVAL_MIN, p_field->field_id[0]))
    {
        sal_fprintf(fp, "/%s"CMD_EQUAL_STR"%u", p_tbl_info->field[TBL_ROUTE_IF_FLD_ND_RA_INTERVAL_MIN].name,
            p_rtif->nd_ra_interval_min);
    }
    if (FLD_MATCH(TBL_ROUTE_IF_FLD_ND_RA_INTERVAL_MAX, p_field->field_id[0]))
    {
        sal_fprintf(fp, "/%s"CMD_EQUAL_STR"%u", p_tbl_info->field[TBL_ROUTE_IF_FLD_ND_RA_INTERVAL_MAX].name,
            p_rtif->nd_ra_interval_max);
    }
    if (FLD_MATCH(TBL_ROUTE_IF_FLD_ND_RA_INTERVAL, p_field->field_id[0]))
    {
        sal_fprintf(fp, "/%s"CMD_EQUAL_STR"%u", p_tbl_info->field[TBL_ROUTE_IF_FLD_ND_RA_INTERVAL].name,
            p_rtif->nd_ra_interval);
    }
    if (FLD_MATCH(TBL_ROUTE_IF_FLD_ND_RA_MIN_DELAY, p_field->field_id[0]))
    {
        sal_fprintf(fp, "/%s"CMD_EQUAL_STR"%u", p_tbl_info->field[TBL_ROUTE_IF_FLD_ND_RA_MIN_DELAY].name,
            p_rtif->nd_ra_min_delay);
    }
    if (FLD_MATCH(TBL_ROUTE_IF_FLD_ND_RETRANSMIT_TIMER, p_field->field_id[0]))
    {
        sal_fprintf(fp, "/%s"CMD_EQUAL_STR"%u", p_tbl_info->field[TBL_ROUTE_IF_FLD_ND_RETRANSMIT_TIMER].name,
            p_rtif->nd_retransmit_timer);
    }
    if (FLD_MATCH(TBL_ROUTE_IF_FLD_ND_ROUTER_LIFETIME, p_field->field_id[0]))
    {
        sal_fprintf(fp, "/%s"CMD_EQUAL_STR"%u", p_tbl_info->field[TBL_ROUTE_IF_FLD_ND_ROUTER_LIFETIME].name,
            p_rtif->nd_router_lifetime);
    }
    if (FLD_MATCH(TBL_ROUTE_IF_FLD_ND_BASE_REACHABLE_TIME, p_field->field_id[0]))
    {
        sal_fprintf(fp, "/%s"CMD_EQUAL_STR"%u", p_tbl_info->field[TBL_ROUTE_IF_FLD_ND_BASE_REACHABLE_TIME].name,
            p_rtif->nd_base_reachable_time);
    }
    if (FLD_MATCH(TBL_ROUTE_IF_FLD_ND_REACHABLE_TIME, p_field->field_id[0]))
    {
        sal_fprintf(fp, "/%s"CMD_EQUAL_STR"%u", p_tbl_info->field[TBL_ROUTE_IF_FLD_ND_REACHABLE_TIME].name,
            p_rtif->nd_reachable_time);
    }
    if (FLD_MATCH(TBL_ROUTE_IF_FLD_ND_DELAY_PROBE_TIME, p_field->field_id[0]))
    {
        sal_fprintf(fp, "/%s"CMD_EQUAL_STR"%u", p_tbl_info->field[TBL_ROUTE_IF_FLD_ND_DELAY_PROBE_TIME].name,
            p_rtif->nd_delay_probe_time);
    }
    if (FLD_MATCH(TBL_ROUTE_IF_FLD_ND_DAD_ATTEMPTS, p_field->field_id[0]))
    {
        sal_fprintf(fp, "/%s"CMD_EQUAL_STR"%u", p_tbl_info->field[TBL_ROUTE_IF_FLD_ND_DAD_ATTEMPTS].name,
            p_rtif->nd_dad_attempts);
    }
    if (FLD_MATCH(TBL_ROUTE_IF_FLD_ND_DST, p_field->field_id[0]))
    {
        sal_fprintf(fp, "/%s"CMD_EQUAL_STR"%s", p_tbl_info->field[TBL_ROUTE_IF_FLD_ND_DST].name, 
            cdb_addr_val2str(buf, MAX_CMD_STR_LEN, &p_rtif->nd_dst));
    }
    if (FLD_MATCH(TBL_ROUTE_IF_FLD_ND_LAST_RA_SOLICITED, p_field->field_id[0]))
    {
        sal_fprintf(fp, "/%s"CMD_EQUAL_STR"%"PRIu64, p_tbl_info->field[TBL_ROUTE_IF_FLD_ND_LAST_RA_SOLICITED].name,
            p_rtif->nd_last_ra_solicited);
    }
    if (FLD_MATCH(TBL_ROUTE_IF_FLD_ND_LAST_RA_UNSOLICITED, p_field->field_id[0]))
    {
        sal_fprintf(fp, "/%s"CMD_EQUAL_STR"%"PRIu64, p_tbl_info->field[TBL_ROUTE_IF_FLD_ND_LAST_RA_UNSOLICITED].name,
            p_rtif->nd_last_ra_unsolicited);
    }
    if (FLD_MATCH(TBL_ROUTE_IF_FLD_ND_RA_OCOUNT, p_field->field_id[0]))
    {
        sal_fprintf(fp, "/%s"CMD_EQUAL_STR"%u", p_tbl_info->field[TBL_ROUTE_IF_FLD_ND_RA_OCOUNT].name,
            p_rtif->nd_ra_ocount);
    }
    if (FLD_MATCH(TBL_ROUTE_IF_FLD_ND_RA_SOLICITED_OCOUNT, p_field->field_id[0]))
    {
        sal_fprintf(fp, "/%s"CMD_EQUAL_STR"%u", p_tbl_info->field[TBL_ROUTE_IF_FLD_ND_RA_SOLICITED_OCOUNT].name,
            p_rtif->nd_ra_solicited_ocount);
    }
    if (FLD_MATCH(TBL_ROUTE_IF_FLD_ND_RA_INIT_UNSOLICITED_OCOUNT, p_field->field_id[0]))
    {
        sal_fprintf(fp, "/%s"CMD_EQUAL_STR"%u", p_tbl_info->field[TBL_ROUTE_IF_FLD_ND_RA_INIT_UNSOLICITED_OCOUNT].name,
            p_rtif->nd_ra_init_unsolicited_ocount);
    }
    if (FLD_MATCH(TBL_ROUTE_IF_FLD_ND_RA_SOLICITED_TIME, p_field->field_id[0]))
    {
        sal_fprintf(fp, "/%s"CMD_EQUAL_STR"%u", p_tbl_info->field[TBL_ROUTE_IF_FLD_ND_RA_SOLICITED_TIME].name,
            p_rtif->nd_ra_solicited_time);
    }
    if (FLD_MATCH(TBL_ROUTE_IF_FLD_ND_RA_PREFIX_DEFAULT_VLIFETIME, p_field->field_id[0]))
    {
        sal_fprintf(fp, "/%s"CMD_EQUAL_STR"%u", p_tbl_info->field[TBL_ROUTE_IF_FLD_ND_RA_PREFIX_DEFAULT_VLIFETIME].name,
            p_rtif->nd_ra_prefix_default_vlifetime);
    }
    if (FLD_MATCH(TBL_ROUTE_IF_FLD_ND_RA_PREFIX_DEFAULT_PLIFETIME, p_field->field_id[0]))
    {
        sal_fprintf(fp, "/%s"CMD_EQUAL_STR"%u", p_tbl_info->field[TBL_ROUTE_IF_FLD_ND_RA_PREFIX_DEFAULT_PLIFETIME].name,
            p_rtif->nd_ra_prefix_default_plifetime);
    }
    if (FLD_MATCH(TBL_ROUTE_IF_FLD_ND_RA_PREFIX_DEFAULT_ONLINK, p_field->field_id[0]))
    {
        sal_fprintf(fp, "/%s"CMD_EQUAL_STR"%u", p_tbl_info->field[TBL_ROUTE_IF_FLD_ND_RA_PREFIX_DEFAULT_ONLINK].name,
            p_rtif->nd_ra_prefix_default_onlink);
    }
    if (FLD_MATCH(TBL_ROUTE_IF_FLD_ND_RA_PREFIX_DEFAULT_AUTO, p_field->field_id[0]))
    {
        sal_fprintf(fp, "/%s"CMD_EQUAL_STR"%u", p_tbl_info->field[TBL_ROUTE_IF_FLD_ND_RA_PREFIX_DEFAULT_AUTO].name,
            p_rtif->nd_ra_prefix_default_auto);
    }
    if (FLD_MATCH(TBL_ROUTE_IF_FLD_ND_RS_DEST_ADDR, p_field->field_id[0]))
    {
        sal_fprintf(fp, "/%s"CMD_EQUAL_STR"%s", p_tbl_info->field[TBL_ROUTE_IF_FLD_ND_RS_DEST_ADDR].name, 
            cdb_addr_val2str(buf, MAX_CMD_STR_LEN, &p_rtif->nd_rs_dest_addr));
    }
    if (FLD_MATCH(TBL_ROUTE_IF_FLD_T_RA_SOLICITED, p_field->field_id[0]))
    {
        sal_fprintf(fp, "/%s"CMD_EQUAL_STR"%s", p_tbl_info->field[TBL_ROUTE_IF_FLD_T_RA_SOLICITED].name, 
            ctc_task_val2str(buf, MAX_CMD_STR_LEN, p_rtif->t_ra_solicited));
    }
    if (FLD_MATCH(TBL_ROUTE_IF_FLD_T_RA_UNSOLICITED, p_field->field_id[0]))
    {
        sal_fprintf(fp, "/%s"CMD_EQUAL_STR"%s", p_tbl_info->field[TBL_ROUTE_IF_FLD_T_RA_UNSOLICITED].name, 
            ctc_task_val2str(buf, MAX_CMD_STR_LEN, p_rtif->t_ra_unsolicited));
    }
    if (FLD_MATCH(TBL_ROUTE_IF_FLD_T_DELETE_ALL_V6_ADDR_TIMER, p_field->field_id[0]))
    {
        sal_fprintf(fp, "/%s"CMD_EQUAL_STR"%s", p_tbl_info->field[TBL_ROUTE_IF_FLD_T_DELETE_ALL_V6_ADDR_TIMER].name, 
            ctc_task_val2str(buf, MAX_CMD_STR_LEN, p_rtif->t_delete_all_v6_addr_timer));
    }
    if (FLD_MATCH(TBL_ROUTE_IF_FLD_DHCPV6_CLIENT_FLAGS, p_field->field_id[0]))
    {
        sal_fprintf(fp, "/%s"CMD_EQUAL_STR"%u", p_tbl_info->field[TBL_ROUTE_IF_FLD_DHCPV6_CLIENT_FLAGS].name,
            p_rtif->dhcpv6_client_flags);
    }
    if (FLD_MATCH(TBL_ROUTE_IF_FLD_DHCPV6_CLIENT_ENABLE, p_field->field_id[0]))
    {
        sal_fprintf(fp, "/%s"CMD_EQUAL_STR"%u", p_tbl_info->field[TBL_ROUTE_IF_FLD_DHCPV6_CLIENT_ENABLE].name,
            p_rtif->dhcpv6_client_enable);
    }
    if (FLD_MATCH(TBL_ROUTE_IF_FLD_NAT_ENABLE, p_field->field_id[0]))
    {
        sal_fprintf(fp, "/%s"CMD_EQUAL_STR"%u", p_tbl_info->field[TBL_ROUTE_IF_FLD_NAT_ENABLE].name,
            p_rtif->nat_enable);
    }
    if (FLD_MATCH(TBL_ROUTE_IF_FLD_NAT_CATEGORY_ID, p_field->field_id[0]))
    {
        sal_fprintf(fp, "/%s"CMD_EQUAL_STR"%u", p_tbl_info->field[TBL_ROUTE_IF_FLD_NAT_CATEGORY_ID].name,
            p_rtif->nat_category_id);
    }
    if (FLD_MATCH(TBL_ROUTE_IF_FLD_SET_MIRROR_ENABLE, p_field->field_id[0]))
    {
        sal_fprintf(fp, "/%s"CMD_EQUAL_STR"%u", p_tbl_info->field[TBL_ROUTE_IF_FLD_SET_MIRROR_ENABLE].name,
            p_rtif->set_mirror_enable);
    }
    if (FLD_MATCH(TBL_ROUTE_IF_FLD_T_OSPF_COST_TIMER, p_field->field_id[0]))
    {
        sal_fprintf(fp, "/%s"CMD_EQUAL_STR"%s", p_tbl_info->field[TBL_ROUTE_IF_FLD_T_OSPF_COST_TIMER].name, 
            ctc_task_val2str(buf, MAX_CMD_STR_LEN, p_rtif->t_ospf_cost_timer));
    }
    if (FLD_MATCH(TBL_ROUTE_IF_FLD_T_SEND_ARP_TIMER, p_field->field_id[0]))
    {
        sal_fprintf(fp, "/%s"CMD_EQUAL_STR"%s", p_tbl_info->field[TBL_ROUTE_IF_FLD_T_SEND_ARP_TIMER].name, 
            ctc_task_val2str(buf, MAX_CMD_STR_LEN, p_rtif->t_send_arp_timer));
    }
    if (FLD_MATCH(TBL_ROUTE_IF_FLD_IFC_IPV4, p_field->field_id[0]))
    {
        sal_fprintf(fp, "/%s"CMD_EQUAL_STR"{", p_tbl_info->field[TBL_ROUTE_IF_FLD_IFC_IPV4].name);
        if (p_rtif->ifc_ipv4.obj_list)
        {
            p_field->field_id[1] = FLD_ID_ALL;
            CTCLIB_SLIST_LOOP(p_rtif->ifc_ipv4.obj_list, p_db_connected, listnode)
            {
                ds_connected_dump_one(p_db_connected, pargs);
                sal_fprintf(fp, "/,");
            }
        }
        sal_fprintf(fp, "}");
    }
    if (FLD_MATCH(TBL_ROUTE_IF_FLD_IFC_IPV6, p_field->field_id[0]))
    {
        sal_fprintf(fp, "/%s"CMD_EQUAL_STR"{", p_tbl_info->field[TBL_ROUTE_IF_FLD_IFC_IPV6].name);
        if (p_rtif->ifc_ipv6.obj_list)
        {
            p_field->field_id[1] = FLD_ID_ALL;
            CTCLIB_SLIST_LOOP(p_rtif->ifc_ipv6.obj_list, p_db_connected_v6, listnode)
            {
                ds_connected_v6_dump_one(p_db_connected_v6, pargs);
                sal_fprintf(fp, "/,");
            }
        }
        sal_fprintf(fp, "}");
    }
    if (FLD_MATCH(TBL_ROUTE_IF_FLD_OSPF_MD5_KEY, p_field->field_id[0]))
    {
        sal_fprintf(fp, "/%s"CMD_EQUAL_STR"{", p_tbl_info->field[TBL_ROUTE_IF_FLD_OSPF_MD5_KEY].name);
        if (p_rtif->ospf_md5_key.obj_list)
        {
            p_field->field_id[1] = FLD_ID_ALL;
            CTCLIB_SLIST_LOOP(p_rtif->ospf_md5_key.obj_list, p_db_ospf_auth, listnode)
            {
                ds_ospf_auth_dump_one(p_db_ospf_auth, pargs);
                sal_fprintf(fp, "/,");
            }
        }
        sal_fprintf(fp, "}");
    }
    if (FLD_MATCH(TBL_ROUTE_IF_FLD_ND_PREFIX, p_field->field_id[0]))
    {
        sal_fprintf(fp, "/%s"CMD_EQUAL_STR"{", p_tbl_info->field[TBL_ROUTE_IF_FLD_ND_PREFIX].name);
        if (p_rtif->nd_prefix.obj_list)
        {
            p_field->field_id[1] = FLD_ID_ALL;
            CTCLIB_SLIST_LOOP(p_rtif->nd_prefix.obj_list, p_db_nd_prefix, listnode)
            {
                ds_nd_prefix_dump_one(p_db_nd_prefix, pargs);
                sal_fprintf(fp, "/,");
            }
        }
        sal_fprintf(fp, "}");
    }
    if (FLD_MATCH(TBL_ROUTE_IF_FLD_PIM_MODE, p_field->field_id[0]))
    {
        sal_fprintf(fp, "/%s"CMD_EQUAL_STR"%u", p_tbl_info->field[TBL_ROUTE_IF_FLD_PIM_MODE].name,
            p_rtif->pim_mode);
    }
    if (FLD_MATCH(TBL_ROUTE_IF_FLD_MULTICAST, p_field->field_id[0]))
    {
        sal_fprintf(fp, "/%s"CMD_EQUAL_STR"%u", p_tbl_info->field[TBL_ROUTE_IF_FLD_MULTICAST].name,
            p_rtif->multicast);
    }
    if (FLD_MATCH(TBL_ROUTE_IF_FLD_IGMP, p_field->field_id[0]))
    {
        sal_fprintf(fp, "/%s"CMD_EQUAL_STR"%u", p_tbl_info->field[TBL_ROUTE_IF_FLD_IGMP].name,
            p_rtif->igmp);
    }
    if (FLD_MATCH(TBL_ROUTE_IF_FLD_IGMP_VERSION, p_field->field_id[0]))
    {
        sal_fprintf(fp, "/%s"CMD_EQUAL_STR"%s", p_tbl_info->field[TBL_ROUTE_IF_FLD_IGMP_VERSION].name,
            p_rtif->igmp_version);
    }
    if (FLD_MATCH(TBL_ROUTE_IF_FLD_IGMP_INTF_LIMIT_NUMBER, p_field->field_id[0]))
    {
        sal_fprintf(fp, "/%s"CMD_EQUAL_STR"%u", p_tbl_info->field[TBL_ROUTE_IF_FLD_IGMP_INTF_LIMIT_NUMBER].name,
            p_rtif->igmp_intf_limit_number);
    }

    sal_fprintf(fp, "\n");

    return PM_E_NONE;
}

int32
tbl_route_if_iterate(TBL_ITER_CB_FUNC fn, tbl_iter_args_t *pargs)
{
    tbl_route_if_master_t *p_master = _g_p_tbl_route_if_master;
    ctclib_slistnode_t *listnode = NULL;
    ctclib_slistnode_t *next = NULL;
    tbl_route_if_t *p_db_rtif = NULL;
    int32 rc = PM_E_NONE;

    if (NULL == p_master)
    {
        return PM_E_NONE;
    }

    CTCLIB_SLIST_LOOP_DEL(p_master->if_list, p_db_rtif, listnode, next)
    {
        rc |= fn(p_db_rtif, pargs);
    }

    return (rc) ? PM_E_FAIL : PM_E_NONE;
}

tbl_route_if_master_t*
tbl_route_if_get_master()
{
    return _g_p_tbl_route_if_master;
}

tbl_route_if_master_t*
tbl_route_if_init_route_if()
{
    _g_p_tbl_route_if_master = XCALLOC(MEM_TBL_MASTER, sizeof(tbl_route_if_master_t));
    _g_p_tbl_route_if_master->if_hash = ctclib_hash_create(_tbl_route_if_hash_make, _tbl_route_if_hash_cmp);
    _g_p_tbl_route_if_master->if_list = ctclib_slist_create(tbl_route_if_cmp, NULL);
    return _g_p_tbl_route_if_master;
}

tbl_route_if_t*
tbl_route_if_get_route_if_by_name(char* ifname)
{
    tbl_route_if_key_t rt_if_key;

    sal_memset(&rt_if_key, 0, sizeof(rt_if_key));
    sal_memcpy(rt_if_key.name, ifname, IFNAME_SIZE);
    
    return tbl_route_if_get_route_if(&rt_if_key);
}
