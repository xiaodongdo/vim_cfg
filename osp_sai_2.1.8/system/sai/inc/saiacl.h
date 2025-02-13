/*
* Copyright (c) 2014 Microsoft Open Technologies, Inc.
*
*    Licensed under the Apache License, Version 2.0 (the "License"); you may
*    not use this file except in compliance with the License. You may obtain
*    a copy of the License at http://www.apache.org/licenses/LICENSE-2.0
*
*    THIS CODE IS PROVIDED ON AN  *AS IS* BASIS, WITHOUT WARRANTIES OR
*    CONDITIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING WITHOUT
*    LIMITATION ANY IMPLIED WARRANTIES OR CONDITIONS OF TITLE, FITNESS
*    FOR A PARTICULAR PURPOSE, MERCHANTABLITY OR NON-INFRINGEMENT.
*
*    See the Apache Version 2.0 License for specific language governing
*    permissions and limitations under the License.
*
*    Microsoft would like to thank the following companies for their review and
*    assistance with these files: Intel Corporation, Mellanox Technologies Ltd,
*    Dell Products, L.P., Facebook, Inc
*
* Module Name:
*
*    saiacl.h
*
* Abstract:
*
*    This module defines SAI ACL API.
*
*/

#if !defined (__SAIACL_H_)
#define __SAIACL_H_

#include <saitypes.h>

/** \defgroup SAIACL SAI - ACL specific API definitions.
 *
 *  \{
 */

/**
 *  @brief Attribute data for SAI_ACL_TABLE_ATTR_STAGE
 */
typedef enum _sai_acl_stage_t
{
    /** Ingress Stage */
    SAI_ACL_STAGE_INGRESS,

    /** Egress Stage */
    SAI_ACL_STAGE_EGRESS,

    /** Ingress Sub Stages */

    /** Ingress Sub Stage: Before L2 processing in the ingress pipeline */
    SAI_ACL_SUBSTAGE_INGRESS_PRE_L2,

    /** Ingress Sub Stage: Post L3 processing in the ingress pipeline */
    SAI_ACL_SUBSTAGE_INGRESS_POST_L3,

} sai_acl_stage_t;

typedef enum _sai_acl_ip_type_t
{
    /** Don't care */
    SAI_ACL_IP_TYPE_ANY,

    /** IPv4 and IPv6 packets */
    SAI_ACL_IP_TYPE_IP,

    /** Non-Ip packet */
    SAI_ACL_IP_TYPE_NON_IP,

    /** Any IPv4 packet */
    SAI_ACL_IP_TYPE_IPv4ANY,

    /** Anything but IPv4 packets */
    SAI_ACL_IP_TYPE_NON_IPv4,

    /** IPv6 packet */
    SAI_ACL_IP_TYPE_IPv6ANY,

    /** Anything but IPv6 packets */
    SAI_ACL_IP_TYPE_NON_IPv6,

    /** ARP/RARP */
    SAI_ACL_IP_TYPE_ARP,

    /** ARP Request */
    SAI_ACL_IP_TYPE_ARP_REQUEST,

    /** ARP Reply */
    SAI_ACL_IP_TYPE_ARP_REPLY

} sai_acl_ip_type_t;

typedef enum _sai_acl_ip_frag_t
{
    /** Any Fragment of Fragmented Packet */
    SAI_ACL_IP_FRAG_ANY,

    /** Non-Fragmented Packet */
    SAI_ACL_IP_FRAG_NON_FRAG,

    /** Non-Fragmented or First Fragment */
    SAI_ACL_IP_FRAG_NON_FRAG_OR_HEAD,

    /** First Fragment of Fragmented Packet */
    SAI_ACL_IP_FRAG_HEAD,

    /** Not the First Fragment */
    SAI_ACL_IP_FRAG_NON_HEAD

} sai_acl_ip_frag_t;

typedef enum _sai_acl_gre_type_t
{
    SAI_ACL_GRE_GRE,

    SAI_ACL_GRE_NVGRE,

} sai_acl_gre_type_t;

typedef enum _sai_acl_strip_header_pos_type_t
{
    SAI_ACL_STRIP_HEADER_POS_L2,
    SAI_ACL_STRIP_HEADER_POS_L3,
    SAI_ACL_STRIP_HEADER_POS_L4,
    SAI_ACL_STRIP_HEADER_POS_MAX,

} sai_acl_strip_header_pos_type_t;
/**
 *  @brief Attribute Id for sai_acl_table
 */
typedef enum _sai_acl_table_attr_t
{
    /** READ-ONLY */

    /** READ-WRITE */

    /** ACL stage [sai_acl_stage_t]
     * (MANDATORY_ON_CREATE|CREATE_ONLY) */
    SAI_ACL_TABLE_ATTR_STAGE,

    /** Priority [sai_uint32_t]
     * (MANDATORY_ON_CREATE|CREATE_ONLY)
     * Value must be in the range defined in
     * [SAI_SWITCH_ATTR_ACL_TABLE_MINIMUM_PRIORITY,
     *  SAI_SWITCH_ATTR_ACL_TABLE_MAXIMUM_PRIORITY] */
    SAI_ACL_TABLE_ATTR_PRIORITY,

    /** Table size [sai_uint32_t], CREATE_ONLY
     * (default = 0) - Grow dynamically till MAX ACL TCAM Size
     * By default table can grow upto maximum ACL TCAM space.
     * Supported only during Table Create for now until NPU
     * supports Dynamic adjustment of Table size post Table creation
     *
     * The table size refers to the number of ACL entries. The number
     * of entries that get's allocated when we create a table with a
     * specific size would depend on the ACL CAM Arch of the NPU. Some
     * NPU supports different blocks, each may have same or different
     * size and what gets allocated can depend on the block size or other
     * factors. So internally what gets allocated when we do a table
     * create would be based on the NPU CAM Arch and size may be more
     * than what is requested. As an example the NPU may support blocks of
     * 128 entries. When a user creates a table of size 100, the actual
     * size that gets allocated is 128. Hence its recommended that the user
     * does a get_attribute(SAI_ACL_TABLE_ATTR_SIZE) to query the actual
     * table size on table create so the user knows the ACL CAM space
     * allocated and able to do ACL CAM Carving accurately */
    SAI_ACL_TABLE_ATTR_SIZE,

    /** Table Group Id [sai_object_id_t], CREATE_ONLY
     *
     * By default, if the attribute is not present during table creation, SAI
     * will internally allocate a unique group id which can be used to group
     * tables which are created later. User would perform get operation for the
     * already created ACL Table to fetch the group id created by SAI and use
     * it for grouping the next table at the time of its creation.
     * Group Id will be deleted from SAI once all the ACL Tables referencing
     * the group are removed.
     *
     * In case none of the tables require grouping, SAI will assign unique group
     * ids equal to the number of ACL Tables created and all will be independently
     * looked up.
     *
     * User should always use the group id returned by SAI in the get operation
     * to group the tables else Invalid attribute value error code will be returned.
     *
     * The ACL Table lookup could be done serially or in parallel. In both the
     * cases there could be a need to group multiple tables so that only single
     * ACL rule entry actions are performed.
     *
     * Within one ACL group, only one ACL entry will be hit which is based on the
     * table priority as well as ACL entry priority within the table. Every ACL
     * Table is associated to a priority which is a mandatory attribute needed at
     * the time of table creation. The order of lookup within the ACL table group
     * will be based on individual table priority. Action(s) from the ACL entry
     * residing in the higher priority table within the group will be performed.
     *
     * Across the ACL groups, one hit ACL entry from each group will be selected
     * and all non-conflicting actions from them will be performed.
     *
     * Without grouping, all non-conflicting actions from different ACL entry
     * hits across the tables will be performed.
     */
    SAI_ACL_TABLE_ATTR_GROUP_ID,

    /** Match fields [bool]
     * (MANDATORY_ON_CREATE, mandatory to pass at least one field during ACL Table Creation)
     * (CREATE_ONLY, match fields cannot be changed after the table is created) */

    /** Start of Table Match Field */
    SAI_ACL_TABLE_ATTR_FIELD_START = 0x00001000,

    /** Src IPv6 Address */
    SAI_ACL_TABLE_ATTR_FIELD_SRC_IPv6 = SAI_ACL_TABLE_ATTR_FIELD_START,

    /** Dst IPv6 Address */
    SAI_ACL_TABLE_ATTR_FIELD_DST_IPv6,

    /** Src MAC Address */
    SAI_ACL_TABLE_ATTR_FIELD_SRC_MAC,

    /** Dst MAC Address */
    SAI_ACL_TABLE_ATTR_FIELD_DST_MAC,

    /** Src IPv4 Address */
    SAI_ACL_TABLE_ATTR_FIELD_SRC_IP,

    /** Dst IPv4 Address */
    SAI_ACL_TABLE_ATTR_FIELD_DST_IP,

    /** In-Ports */
    SAI_ACL_TABLE_ATTR_FIELD_IN_PORTS,

    /** Out-Ports */
    SAI_ACL_TABLE_ATTR_FIELD_OUT_PORTS,

    /** In-Port */
    SAI_ACL_TABLE_ATTR_FIELD_IN_PORT,

    /** Out-Port */
    SAI_ACL_TABLE_ATTR_FIELD_OUT_PORT,

    /** Outer Vlan-Id */
    SAI_ACL_TABLE_ATTR_FIELD_OUTER_VLAN_ID,

    /** Outer Vlan-Priority */
    SAI_ACL_TABLE_ATTR_FIELD_OUTER_VLAN_PRI,

    /** Outer Vlan-CFI */
    SAI_ACL_TABLE_ATTR_FIELD_OUTER_VLAN_CFI,

    /** Inner Vlan-Id */
    SAI_ACL_TABLE_ATTR_FIELD_INNER_VLAN_ID,

    /** Inner Vlan-Priority */
    SAI_ACL_TABLE_ATTR_FIELD_INNER_VLAN_PRI,

    /** Inner Vlan-CFI */
    SAI_ACL_TABLE_ATTR_FIELD_INNER_VLAN_CFI,

    /** L4 Src Port */
    SAI_ACL_TABLE_ATTR_FIELD_L4_SRC_PORT,

    /** L4 Dst Port */
    SAI_ACL_TABLE_ATTR_FIELD_L4_DST_PORT,

    /** EtherType */
    SAI_ACL_TABLE_ATTR_FIELD_ETHER_TYPE,

    /** IP Protocol */
    SAI_ACL_TABLE_ATTR_FIELD_IP_PROTOCOL,

    /** Ip Dscp */
    SAI_ACL_TABLE_ATTR_FIELD_DSCP,

    /** Ip Ecn */
    SAI_ACL_TABLE_ATTR_FIELD_ECN,

    /** Ip Ttl */
    SAI_ACL_TABLE_ATTR_FIELD_TTL,

    /** Ip Tos */
    SAI_ACL_TABLE_ATTR_FIELD_TOS,

    /** Ip Flags */
    SAI_ACL_TABLE_ATTR_FIELD_IP_FLAGS,

    /** Tcp Flags */
    SAI_ACL_TABLE_ATTR_FIELD_TCP_FLAGS,

    /** Ip Type */
    SAI_ACL_TABLE_ATTR_FIELD_IP_TYPE,

    /** Ip Frag */
    SAI_ACL_TABLE_ATTR_FIELD_IP_FRAG,

    /** IPv6 Flow Label */
    SAI_ACL_TABLE_ATTR_FIELD_IPv6_FLOW_LABEL,

    /** Class-of-Service (Traffic Class) */
    SAI_ACL_TABLE_ATTR_FIELD_TC,

    /** ICMP Type */
    SAI_ACL_TABLE_ATTR_FIELD_ICMP_TYPE,

    /** ICMP Code */
    SAI_ACL_TABLE_ATTR_FIELD_ICMP_CODE,

    /** User Based Meta Data [bool] */

    /** FDB DST user meta data */
    SAI_ACL_TABLE_ATTR_FIELD_FDB_DST_USER_META,

    /** ROUTE DST User Meta data */
    SAI_ACL_TABLE_ATTR_FIELD_ROUTE_DST_USER_META,

    /** Neighbor DST User Meta Data */
    SAI_ACL_TABLE_ATTR_FIELD_NEIGHBOR_DST_USER_META,

    /** Port User Meta Data */
    SAI_ACL_TABLE_ATTR_FIELD_PORT_USER_META,

    /** Vlan User Meta Data */
    SAI_ACL_TABLE_ATTR_FIELD_VLAN_USER_META,

    /** Meta Data carried from previous ACL Stage */
    SAI_ACL_TABLE_ATTR_FIELD_ACL_USER_META,

    /** NPU Based Meta Data [bool] */

    /** DST MAC address match in FDB */
    SAI_ACL_TABLE_ATTR_FIELD_FDB_DST_NPU_META_HIT,

    /** DST IP address match in neighbor table */
    SAI_ACL_TABLE_ATTR_FIELD_NEIGHBOR_DST_NPU_META_HIT,

    /** End of Table Match Field */
    SAI_ACL_TABLE_ATTR_FIELD_END = SAI_ACL_TABLE_ATTR_FIELD_NEIGHBOR_DST_NPU_META_HIT,

    /* -- */

    /* Custom range base value */
    SAI_ACL_TABLE_ATTR_CUSTOM_RANGE_BASE  = 0x10000000

} sai_acl_table_attr_t;

/* vlan action */
enum _sai_acl_vlan_action{
	SAI_ACL_VLAN_ACTION_REPLACE,
	SAI_ACL_VLAN_ACTION_ADD,
	SAI_ACL_VLAN_ACTION_ADD_OR_REPLACE,
}sai_acl_vlan_action_t;

/**
 *  @brief Attribute Id for sai_acl_entry
 */
typedef enum _sai_acl_entry_attr_t
{
    /** READ-ONLY */

    /** SAI acl table object id [sai_object_id_t]
     * (MANDATORY_ON_CREATE) */
    SAI_ACL_ENTRY_ATTR_TABLE_ID,

    /** READ-WRITE */

    /** Priority [sai_uint32_t]
     * Value must be in the range defined in
     * [SAI_SWITCH_ATTR_ACL_ENTRY_MINIMUM_PRIORITY,
     *  SAI_SWITCH_ATTR_ACL_ENTRY_MAXIMUM_PRIORITY]
     * (default = SAI_SWITCH_ATTR_ACL_ENTRY_MINIMUM_PRIORITY) */
    SAI_ACL_ENTRY_ATTR_PRIORITY,

    /** Enabled / Disabled [bool]
     * (default = enabled) */
    SAI_ACL_ENTRY_ATTR_ADMIN_STATE,

    /** Match fields [sai_acl_field_data_t]
     * (MANDATORY_ON_CREATE, mandatory to pass at least one field during ACL Rule Creation)
     * - Unless noted specificially, both data and mask are required.
     * - When bitfield is used the comment, only those least significent bits
     *   are valid for matching.
     */

    /** Start of Rule Match Fields */
    SAI_ACL_ENTRY_ATTR_FIELD_START = 0x00001000,

    /** Src IPv6 Address [sai_ip6_t] */
    SAI_ACL_ENTRY_ATTR_FIELD_SRC_IPv6 = SAI_ACL_ENTRY_ATTR_FIELD_START,

    /** Dst IPv6 Address [sai_ip6_t] */
    SAI_ACL_ENTRY_ATTR_FIELD_DST_IPv6,

    /** Src MAC Address [sai_mac_t] */
    SAI_ACL_ENTRY_ATTR_FIELD_SRC_MAC,

    /** Dst MAC Address [sai_mac_t] */
    SAI_ACL_ENTRY_ATTR_FIELD_DST_MAC,

    /** Src IPv4 Address [sai_ip4_t] */
    SAI_ACL_ENTRY_ATTR_FIELD_SRC_IP,

    /** Dst IPv4 Address [sai_ip4_t] */
    SAI_ACL_ENTRY_ATTR_FIELD_DST_IP,

    /** In-Ports [sai_object_list_t] (mask is not needed) */
    SAI_ACL_ENTRY_ATTR_FIELD_IN_PORTS,

    /** Out-Ports [sai_object_list_t] (mask is not needed) */
    SAI_ACL_ENTRY_ATTR_FIELD_OUT_PORTS,

    /** In-Port [sai_object_id_t] (mask is not needed) */
    SAI_ACL_ENTRY_ATTR_FIELD_IN_PORT,

    /** Out-Port [sai_object_id_t] (mask is not needed) */
    SAI_ACL_ENTRY_ATTR_FIELD_OUT_PORT,

    /** Outer Vlan-Id [sai_uint16_t : 12] */
    SAI_ACL_ENTRY_ATTR_FIELD_OUTER_VLAN_ID,

    /** Outer Vlan-Priority [sai_uint8_t : 3] */
    SAI_ACL_ENTRY_ATTR_FIELD_OUTER_VLAN_PRI,

    /** Outer Vlan-CFI [sai_uint8_t : 1] */
    SAI_ACL_ENTRY_ATTR_FIELD_OUTER_VLAN_CFI,

    /** Inner Vlan-Id [sai_uint16_t: 12] */
    SAI_ACL_ENTRY_ATTR_FIELD_INNER_VLAN_ID,

    /** Inner Vlan-Priority [sai_uint8_t : 3] */
    SAI_ACL_ENTRY_ATTR_FIELD_INNER_VLAN_PRI,

    /** Inner Vlan-CFI [sai_uint8_t : 1] */
    SAI_ACL_ENTRY_ATTR_FIELD_INNER_VLAN_CFI,

    /** Untag Vlan, added by yejl for acl untag-vlan, 2018-11-14*/
    SAI_ACL_ENTRY_ATTR_FIELD_UNTAG_VLAN,

    /** L4 Src Port [sai_uint16_t] */
    SAI_ACL_ENTRY_ATTR_FIELD_L4_SRC_PORT,

    /** L4 Dst Port [sai_uint16_t] */
    SAI_ACL_ENTRY_ATTR_FIELD_L4_DST_PORT,

    /** EtherType [sai_uint16_t] */
    SAI_ACL_ENTRY_ATTR_FIELD_ETHER_TYPE,

    /** IP Protocol [sai_uint8_t] */
    SAI_ACL_ENTRY_ATTR_FIELD_IP_PROTOCOL,

    /** Ip Dscp [sai_uint8_t : 6] */
    SAI_ACL_ENTRY_ATTR_FIELD_DSCP,

    /** Ip Ecn [sai_uint8_t : 2] */
    SAI_ACL_ENTRY_ATTR_FIELD_ECN,

    /** Ip Ttl [sai_uint8_t] */
    SAI_ACL_ENTRY_ATTR_FIELD_TTL,

    /** Ip Tos [sai_uint8_t] */
    SAI_ACL_ENTRY_ATTR_FIELD_TOS,

    /** Ip Flags [sai_uint8_t : 3] */
    SAI_ACL_ENTRY_ATTR_FIELD_IP_FLAGS,

    /** Tcp Flags [sai_uint8_t : 6] */
    SAI_ACL_ENTRY_ATTR_FIELD_TCP_FLAGS,

    /** Ip Type [sai_acl_ip_type_t] (field mask is not needed) */
    SAI_ACL_ENTRY_ATTR_FIELD_IP_TYPE,

    /** Ipv6 acl  */
    SAI_ACL_ENTRY_ATTR_FIELD_IPV6_ACL,/*added by yejl for Ipv6 acl, 2018-10-10*/

    /** MAC acl  */
    SAI_ACL_ENTRY_ATTR_FIELD_MAC_ACL,/*added by yejl for MAC acl, 2018-10-10*/

    /** MAC acl  */
    SAI_ACL_ENTRY_ATTR_FIELD_IPV4_PACKET,/*added by yejl for IPv4 acl, 2018-10-10*/
    
    /** MAC acl  */
    SAI_ACL_ENTRY_ATTR_FIELD_ARP_PACKET,/*added by yejl for IPv4 acl, 2018-10-10*/

    /** Ip Frag [sai_acl_ip_frag_t] (field mask is not needed) */
    SAI_ACL_ENTRY_ATTR_FIELD_IP_FRAG,

    /** IPv6 Flow Label [sai_uint32_t : 20] */
    SAI_ACL_ENTRY_ATTR_FIELD_IPv6_FLOW_LABEL,

    /** Class-of-Service (Traffic Class) [sai_cos_t] */
    SAI_ACL_ENTRY_ATTR_FIELD_TC,

    /** ICMP Type [sai_uint8_t] */
    SAI_ACL_ENTRY_ATTR_FIELD_ICMP_TYPE,

    /** ICMP Code [sai_uint8_t] */
    SAI_ACL_ENTRY_ATTR_FIELD_ICMP_CODE,

    /** User Based Meta Data [sai_uint32_t] */
    
    /** CTC_PRIVATE_ADDED IGMP Type [sai_uint8_t] */
    SAI_ACL_ENTRY_ATTR_FIELD_IGMP_TYPE,
    /** CTC_PRIVATE_ADDED IP-Precedence [sai_uint8_t : 3] */
    SAI_ACL_ENTRY_ATTR_FIELD_IP_PRECEDENCE,
    /** CTC_PRIVATE_ADDED IP-OPTIONS [sai_uint8_t ]*/
    SAI_ACL_ENTRY_ATTR_FIELD_IP_OPTIONS,
    /** CTC_PRIVATE_ADDED ROUTED-PKT [sai_uint8_t ]*/
    SAI_ACL_ENTRY_ATTR_FIELD_ROUTED_PKT,

    /** CTC_PRIVATE_ADDED UDF ID [sai_uint32_t ] */
    SAI_ACL_ENTRY_ATTR_FIELD_UDF_ID,
    /** CTC_PRIVATE_ADDED UDF MATCH [sai_uint32_t ] */
    SAI_ACL_ENTRY_ATTR_FIELD_UDF_MATCH,
    /** CTC_PRIVATE_ADDED UDF MATCH TYPE [sai_uint8_t ] */
    SAI_ACL_ENTRY_ATTR_FIELD_UDF_TYPE,
    
    /** CTC_PRIVATE_ADDED ARP TYPE [sai_uint16_t ] */
    SAI_ACL_ENTRY_ATTR_FIELD_ARP_OP_TYPE,

    /** CTC_PRIVATE_ADDED ARP Sender IPv4 Address [sai_ip4_t] */
    SAI_ACL_ENTRY_ATTR_FIELD_ARP_SENDER_IP,

    /** CTC_PRIVATE_ADDED ARP Target IPv4 Address [sai_ip4_t] */
    SAI_ACL_ENTRY_ATTR_FIELD_ARP_TARGET_IP,


    /** DST MAC address match user meta data in FDB
     * Value must be in the range defined in
     * SAI_SWITCH_ATTR_FDB_DST_USER_META_DATA_RANGE */
    SAI_ACL_ENTRY_ATTR_FIELD_FDB_DST_USER_META,

    /** DST IP address match user meta data in Route Table
     * Value must be in the range defined in
     * SAI_SWITCH_ATTR_ROUTE_DST_USER_META_DATA_RANGE */
    SAI_ACL_ENTRY_ATTR_FIELD_ROUTE_DST_USER_META,

    /** DST IP address match user meta data in Neighbor Table
     * Value must be in the range defined in
     * SAI_SWITCH_ATTR_NEIGHBOR_DST_USER_META_DATA_RANGE */
    SAI_ACL_ENTRY_ATTR_FIELD_NEIGHBOR_USER_META,

    /** Port User Meta Data
     * Value must be in the range defined in
     * SAI_SWITCH_ATTR_PORT_USER_META_DATA_RANGE */
    SAI_ACL_ENTRY_ATTR_FIELD_PORT_USER_META,

    /** Vlan User Meta Data
     * Value must be in the range defined in
     * SAI_SWITCH_ATTR_VLAN_USER_META_DATA_RANGE */
    SAI_ACL_ENTRY_ATTR_FIELD_VLAN_USER_META,

    /** Meta Data carried from previous ACL Stage.
     * When an ACL entry set the meta data, the ACL meta data
     * form previous stages are overriden.
     * Value must be in the range defined in
     * SAI_SWITCH_ATTR_ACL_USER_META_DATA_RANGE */
    SAI_ACL_ENTRY_ATTR_FIELD_ACL_USER_META,

    /** NPU Based Meta Data [bool] */

    /** DST MAC address match in FDB */
    SAI_ACL_ENTRY_ATTR_FIELD_FDB_NPU_META_DST_HIT,

    /** DST IP address match in neighbor Table */
    SAI_ACL_ENTRY_ATTR_FIELD_NEIGHBOR_NPU_META_DST_HIT,

    /** CTC_PRIVATE_ADDED TRUNCATION [sai_uint32_t ]*/
    SAI_ACL_ENTRY_ATTR_FIELD_VXLAN_VNI,

    /* gre type [sai_uint32_t][sai_acl_gre_type_t] */
    SAI_ACL_ENTRY_ATTR_FIELD_GRE_TYPE, 
    SAI_ACL_ENTRY_ATTR_FIELD_GRE_KEY,

    SAI_ACL_ENTRY_ATTR_FIELD_ERSPAN_ID,

    SAI_ACL_ENTRY_ATTR_FIELD_CATEGORY_ID,

    SAI_ACL_ENTRY_ATTR_FIELD_VXLAN_VNI_DSTPORT,

    /** End of Rule Match Fields */
    SAI_ACL_ENTRY_ATTR_FIELD_END,

    SAI_ACL_ENTRY_ATTR_FIELD_INNER_START = 0x00001100,
    
    SAI_ACL_ENTRY_ATTR_FIELD_INNER_SRC_IPv6,
    SAI_ACL_ENTRY_ATTR_FIELD_INNER_DST_IPv6,
    SAI_ACL_ENTRY_ATTR_FIELD_INNER_SRC_MAC,
    SAI_ACL_ENTRY_ATTR_FIELD_INNER_DST_MAC,
    SAI_ACL_ENTRY_ATTR_FIELD_INNER_SRC_IP,
    SAI_ACL_ENTRY_ATTR_FIELD_INNER_DST_IP,
    SAI_ACL_ENTRY_ATTR_FIELD_INNER_OUTER_VLAN_ID,
    SAI_ACL_ENTRY_ATTR_FIELD_INNER_OUTER_VLAN_PRI,
    SAI_ACL_ENTRY_ATTR_FIELD_INNER_OUTER_VLAN_CFI,
    SAI_ACL_ENTRY_ATTR_FIELD_INNER_INNER_VLAN_ID,
    SAI_ACL_ENTRY_ATTR_FIELD_INNER_INNER_VLAN_PRI,
    SAI_ACL_ENTRY_ATTR_FIELD_INNER_INNER_VLAN_CFI,
    SAI_ACL_ENTRY_ATTR_FIELD_INNER_L4_SRC_PORT,
    SAI_ACL_ENTRY_ATTR_FIELD_INNER_L4_DST_PORT,
    SAI_ACL_ENTRY_ATTR_FIELD_INNER_ETHER_TYPE,
    SAI_ACL_ENTRY_ATTR_FIELD_INNER_IP_PROTOCOL,
    SAI_ACL_ENTRY_ATTR_FIELD_INNER_DSCP,
    SAI_ACL_ENTRY_ATTR_FIELD_INNER_ECN,
    SAI_ACL_ENTRY_ATTR_FIELD_INNER_TTL,
    SAI_ACL_ENTRY_ATTR_FIELD_INNER_TOS,
    SAI_ACL_ENTRY_ATTR_FIELD_INNER_IP_FLAGS,
    SAI_ACL_ENTRY_ATTR_FIELD_INNER_TCP_FLAGS,
    SAI_ACL_ENTRY_ATTR_FIELD_INNER_IP_TYPE,
    SAI_ACL_ENTRY_ATTR_FIELD_INNER_IP_FRAG,
    SAI_ACL_ENTRY_ATTR_FIELD_INNER_IPv6_FLOW_LABEL,
    SAI_ACL_ENTRY_ATTR_FIELD_INNER_TC,
    SAI_ACL_ENTRY_ATTR_FIELD_INNER_ICMP_TYPE,
    SAI_ACL_ENTRY_ATTR_FIELD_INNER_ICMP_CODE,
    SAI_ACL_ENTRY_ATTR_FIELD_INNER_IGMP_TYPE,
    SAI_ACL_ENTRY_ATTR_FIELD_INNER_IP_PRECEDENCE,
    SAI_ACL_ENTRY_ATTR_FIELD_INNER_IP_OPTIONS,

    SAI_ACL_ENTRY_ATTR_FIELD_INNER_VXLAN_VNI,
    SAI_ACL_ENTRY_ATTR_FIELD_INNER_GRE_TYPE, 
    SAI_ACL_ENTRY_ATTR_FIELD_INNER_GRE_KEY,
    
    SAI_ACL_ENTRY_ATTR_FIELD_INNER_END,

    /** Actions [sai_acl_action_data_t]
     * - To enable an action, parameter is needed unless noted specifically.
     * - To disable an action, parameter is not needed. */

    /** Start of Rule Actions */
    SAI_ACL_ENTRY_ATTR_ACTION_START = 0x00002000,

    /** Redirect Packet to a destination which can be a port,
     * lag, nexthop, nexthopgroup. [sai_object_id_t] */
    SAI_ACL_ENTRY_ATTR_ACTION_REDIRECT = SAI_ACL_ENTRY_ATTR_ACTION_START,

    /** Drop Packet [sai_packet_action_t] */
    SAI_ACL_ENTRY_ATTR_PACKET_ACTION,

    /** Flood Packet on Vlan domain (parameter is not needed) */
    SAI_ACL_ENTRY_ATTR_ACTION_FLOOD,

    /** Attach/detach counter id to the entry [sai_object_id_t] */
    SAI_ACL_ENTRY_ATTR_ACTION_COUNTER,

    /** Ingress Mirror (mirror session id list) [sai_object_list_t] */
    SAI_ACL_ENTRY_ATTR_ACTION_MIRROR_INGRESS,

    /** Egress Mirror (mirror session id list) [sai_object_list_t] */
    SAI_ACL_ENTRY_ATTR_ACTION_MIRROR_EGRESS,

    /** Assosiate with policer (policer id) [sai_object_id_t] */
    SAI_ACL_ENTRY_ATTR_ACTION_SET_POLICER,

    /** Decrement TTL (enable/disable) (parameter is not needed) */
    SAI_ACL_ENTRY_ATTR_ACTION_DECREMENT_TTL,

    /** Set Class-of-Service (Traffic Class) [sai_cos_t] */
    SAI_ACL_ENTRY_ATTR_ACTION_SET_TC,

    /** Set packet color [sai_packet_color_t] */
    SAI_ACL_ENTRY_ATTR_ACTION_SET_COLOR,

    /* SYSTEM MODIFIED BEGIN: fix gb tc+color bug add by wangqj at 2016/8/11 */
    /** Set Class-of-Service (Traffic Class) [sai_cos_t] and packet color [sai_packet_color_t] */
    SAI_ACL_ENTRY_ATTR_ACTION_SET_TC_AND_COLOR,
    /* END: fix gb tc+color bug add by wangqj at 2016/8/11 */

    /** Set Packet Inner Vlan-Id [sai_uint16_t : 12] */
    SAI_ACL_ENTRY_ATTR_ACTION_SET_INNER_VLAN_ID,

    /** Set Packet Inner Vlan-Priority [sai_uint8_t : 3] */
    SAI_ACL_ENTRY_ATTR_ACTION_SET_INNER_VLAN_PRI,

    /** Set Packet Outer Vlan-Id [sai_uint16_t : 12] */
    SAI_ACL_ENTRY_ATTR_ACTION_SET_OUTER_VLAN_ID,

	/** Outer Vlan-Id action [sai_acl_vlan_action_t] */
    SAI_ACL_ENTRY_ATTR_ACTION_SET_OUTER_VLAN_ID_ACTION,

    /** Set Packet Outer Vlan-Priority [sai_uint8_t : 3] */
    SAI_ACL_ENTRY_ATTR_ACTION_SET_OUTER_VLAN_PRI,

    /** Set Packet Src MAC Address [sai_mac_t] */
    SAI_ACL_ENTRY_ATTR_ACTION_SET_SRC_MAC,

    /** Set Packet Dst MAC Address [sai_mac_t] */
    SAI_ACL_ENTRY_ATTR_ACTION_SET_DST_MAC,

    /** Set Packet Src IPv4 Address [sai_ip4_t] */
    SAI_ACL_ENTRY_ATTR_ACTION_SET_SRC_IP,

    /** Set Packet Src IPv4 Address [sai_ip4_t] */
    SAI_ACL_ENTRY_ATTR_ACTION_SET_DST_IP,

    /** Set Packet Src IPv6 Address [sai_ip6_t] */
    SAI_ACL_ENTRY_ATTR_ACTION_SET_SRC_IPv6,

    /** Set Packet Src IPv6 Address [sai_ip6_t] */
    SAI_ACL_ENTRY_ATTR_ACTION_SET_DST_IPv6,

    /** Set Packet DSCP [sai_uint8_t : 6] */
    SAI_ACL_ENTRY_ATTR_ACTION_SET_DSCP,

    /** Set Packet ECN [sai_uint8_t : 2] */
    SAI_ACL_ENTRY_ATTR_ACTION_SET_ECN,

    /** Set Packet L4 Src Port [sai_uint16_t] */
    SAI_ACL_ENTRY_ATTR_ACTION_SET_L4_SRC_PORT,

    /** Set Packet L4 Src Port [sai_uint16_t] */
    SAI_ACL_ENTRY_ATTR_ACTION_SET_L4_DST_PORT,

    /** Set ingress packet sampling (samplepacket session id) [sai_object_id_t] */
    SAI_ACL_ENTRY_ATTR_ACTION_INGRESS_SAMPLEPACKET_ENABLE,

    /** Set egress packet sampling (samplepacket session id) [sai_object_id_t] */
    SAI_ACL_ENTRY_ATTR_ACTION_EGRESS_SAMPLEPACKET_ENABLE,

    /** Set CPU Queue for CPU bound traffic [sai_object_id_t]
     * Action can be used whenever packet is destined to CPU such as
     * when packet action specifies the packet needs to be punted
     * to CPU (Trap/Log) or the destination port points to CPU. */
    SAI_ACL_ENTRY_ATTR_ACTION_SET_CPU_QUEUE,

    /** Set Packet Redirect vlan [sai_uint16_t] */
    SAI_ACL_ENTRY_ATTR_ACTION_SET_REDIRECT_VLAN_ID,
    /** Set Packet Redirect Untag [sai_tap_group_member_untag_mode_t] */
    SAI_ACL_ENTRY_ATTR_ACTION_SET_REDIRECT_UNTAG,
    /** CTC_PRIVATE_ADDED TRUNCATION [sai_uint8_t ]*/
    SAI_ACL_ENTRY_ATTR_ACTION_SET_TRUNCATION,

    /** CTC_PRIVATE_ADDED strip header [sai_uint32_t ]*/
    SAI_ACL_ENTRY_ATTR_ACTION_SET_STRIP_HEADER,

    /** CTC_PRIVATE_ADDED strip header pos [sai_acl_strip_header_pos_type_t ]*/
    SAI_ACL_ENTRY_ATTR_ACTION_SET_STRIP_HEADER_POS,

    /** CTC_PRIVATE_ADDED strip header offset [sai_uint32_t ]*/
    SAI_ACL_ENTRY_ATTR_ACTION_SET_STRIP_HEADER_OFFSET,

    /** CTC_PRIVATE_ADDED strip header [sai_uint32_t ]*/
    SAI_ACL_ENTRY_ATTR_ACTION_SET_STRIP_INNER_VXLAN_HEADER,

    /*added by yejl for bug 53475, 2019-09-10*/
    SAI_ACL_ENTRY_ATTR_ACTION_SET_ADD_L2GRE_HEADER,

    SAI_ACL_ENTRY_ATTR_ACTION_SET_ADD_L2GRE_SRC_IP,

    SAI_ACL_ENTRY_ATTR_ACTION_SET_ADD_L2GRE_DST_IP,

    SAI_ACL_ENTRY_ATTR_ACTION_SET_ADD_L2GRE_DST_MAC,
    
    SAI_ACL_ENTRY_ATTR_ACTION_SET_ADD_L2GRE_KEY_NUM,

    SAI_ACL_ENTRY_ATTR_ACTION_SET_ADD_L2GRE_KEY_LEN,

    SAI_ACL_ENTRY_ATTR_ACTION_SET_ADD_L3GRE_HEADER,

    SAI_ACL_ENTRY_ATTR_ACTION_SET_ADD_L3GRE_SRC_IP,

    SAI_ACL_ENTRY_ATTR_ACTION_SET_ADD_L3GRE_DST_IP,

    SAI_ACL_ENTRY_ATTR_ACTION_SET_ADD_L3GRE_DST_MAC,
    /*ended by yejl*/

    /*added by guoxd for vxlan*/
    SAI_ACL_ENTRY_ATTR_ACTION_SET_ADD_VXLAN_HEADER,

    SAI_ACL_ENTRY_ATTR_ACTION_SET_ADD_VXLAN_DST_MAC,

    SAI_ACL_ENTRY_ATTR_ACTION_SET_ADD_VXLAN_SRC_IP,

    SAI_ACL_ENTRY_ATTR_ACTION_SET_ADD_VXLAN_DST_IP,
    
    SAI_ACL_ENTRY_ATTR_ACTION_SET_ADD_VXLAN_SRCPORT,

    SAI_ACL_ENTRY_ATTR_ACTION_SET_ADD_VXLAN_DESTPORT,

    SAI_ACL_ENTRY_ATTR_ACTION_SET_ADD_VXLAN_VNI,
    /*ended by guoxd*/

    /** Set Meta Data to carry forward to next ACL Stage
     * [sai_uint32_t]
     * Value Range SAI_SWITCH_ATTR_ACL_USER_META_DATA_RANGE */
    SAI_ACL_ENTRY_ATTR_ACTION_SET_ACL_META_DATA,
    
    SAI_ACL_ENTRY_ATTR_ACTION_NAT_CANCEL,


    /** End of Rule Actions */
    SAI_ACL_ENTRY_ATTR_ACTION_END = SAI_ACL_ENTRY_ATTR_ACTION_SET_ACL_META_DATA

} sai_acl_entry_attr_t;

/**
 *  @brief Attribute Id for sai_acl_counter
 */
typedef enum _sai_acl_counter_attr_t
{
    /** READ-ONLY */

    /** SAI acl table object id [sai_object_id_t]
     * (MANDATORY_ON_CREATE) */
    SAI_ACL_COUNTER_ATTR_TABLE_ID,

    /**
     * By default Byte Counter would be created and following
     * use of the below attributes would result in an error.
     *
     * - Both packet count and byte count set to disable
     * - Only Byte count used which is set to disable
     */

    /** enable/disable packet count [bool] */
    SAI_ACL_COUNTER_ATTR_ENABLE_PACKET_COUNT,

    /** enable/disable byte count [bool] */
    SAI_ACL_COUNTER_ATTR_ENABLE_BYTE_COUNT,

    /** get/set packet count [uint64_t] */
    SAI_ACL_COUNTER_ATTR_PACKETS,

    /** get/set byte count [uint64_t] */
    SAI_ACL_COUNTER_ATTR_BYTES

} sai_acl_counter_attr_t;

/**
 *   Routine Description:
 *    @brief Create an ACL table
 * 
 *  Arguments:
 *  @param[out] acl_table_id - the the acl table id
 *  @param[in] attr_count - number of attributes
 *  @param[in] attr_list - array of attributes
 * 
 *  Return Values:
 *    @return  SAI_STATUS_SUCCESS on success
 *             Failure status code on error
 */
 typedef sai_status_t (*sai_create_acl_table_fn)(
    _Out_ sai_object_id_t* acl_table_id,
    _In_ uint32_t attr_count,
    _In_ const sai_attribute_t *attr_list
    );

/**
 *  Routine Description:
 *    @brief Delete an ACL table
 * 
 *  Arguments:
 *    @param[in] acl_table_id - the acl table id
 * 
 *  Return Values:
 *    @return  SAI_STATUS_SUCCESS on success
 *             Failure status code on error
 */
 typedef sai_status_t (*sai_delete_acl_table_fn)(
    _In_ sai_object_id_t acl_table_id
    );

/**
 * Routine Description:
 *   @brief Set ACL table attribute
 *
 * Arguments:
 *    @param[in] acl_table_id - the acl table id
 *    @param[in] attr - attribute
 *
 * Return Values:
 *    @return  SAI_STATUS_SUCCESS on success
 *             Failure status code on error
 */
typedef sai_status_t (*sai_set_acl_table_attribute_fn)(
    _In_ sai_object_id_t acl_table_id,
    _In_ const sai_attribute_t *attr
    );

/**
 * Routine Description:
 *   @brief Get ACL table attribute
 *
 * Arguments:
 *    @param[in] acl_table_id - acl table id
 *    @param[in] attr_count - number of attributes
 *    @param[out] attr_list - array of attributes
 *
 * Return Values:
 *    @return  SAI_STATUS_SUCCESS on success
 *             Failure status code on error
 */
typedef sai_status_t (*sai_get_acl_table_attribute_fn)(
    _In_ sai_object_id_t acl_table_id,
    _In_ uint32_t attr_count,
    _Out_ sai_attribute_t *attr_list
    );

/**
 * Routine Description:
 *   @brief Create an ACL entry
 *
 * Arguments:
 *   @param[out] acl_entry_id - the acl entry id
 *   @param[in] attr_count - number of attributes
 *   @param[in] attr_list - array of attributes
 *
 * Return Values:
 *    @return  SAI_STATUS_SUCCESS on success
 *             Failure status code on error
 */
typedef sai_status_t (*sai_create_acl_entry_fn)(
    _Out_ sai_object_id_t *acl_entry_id,
    _In_ uint32_t attr_count,
    _In_ const sai_attribute_t *attr_list
    );

/**
 * Routine Description:
 *   @brief Delete an ACL entry
 *
 * Arguments:
 *  @param[in] acl_entry_id - the acl entry id
 *
 * Return Values:
 *    @return  SAI_STATUS_SUCCESS on success
 *             Failure status code on error
 */
typedef sai_status_t (*sai_delete_acl_entry_fn)(
    _In_ sai_object_id_t acl_entry_id
    );

/**
 * Routine Description:
 *   @brief Set ACL entry attribute
 *
 * Arguments:
 *    @param[in] acl_entry_id - the acl entry id
 *    @param[in] attr - attribute
 *
 * Return Values:
 *    @return  SAI_STATUS_SUCCESS on success
 *             Failure status code on error
 */
typedef sai_status_t (*sai_set_acl_entry_attribute_fn)(
    _In_ sai_object_id_t acl_entry_id,
    _In_ const sai_attribute_t *attr
    );

/**
 * Routine Description:
 *   @brief Get ACL entry attribute
 *
 * Arguments:
 *    @param[in] acl_entry_id - acl entry id
 *    @param[in] attr_count - number of attributes
 *    @param[out] attr_list - array of attributes
 *
 * Return Values:
 *    @return  SAI_STATUS_SUCCESS on success
 *             Failure status code on error
 */
typedef sai_status_t (*sai_get_acl_entry_attribute_fn)(
    _In_ sai_object_id_t acl_entry_id,
    _In_ uint32_t attr_count,
    _Out_ sai_attribute_t *attr_list
    );

/**
 * Routine Description:
 *   @brief Create an ACL counter
 *
 * Arguments:
 *   @param[out] acl_counter_id - the acl counter id
 *   @param[in] attr_count - number of attributes
 *   @param[in] attr_list - array of attributes
 *
 * Return Values:
 *    @return  SAI_STATUS_SUCCESS on success
 *             Failure status code on error
 */
typedef sai_status_t (*sai_create_acl_counter_fn)(
    _Out_ sai_object_id_t *acl_counter_id,
    _In_ uint32_t attr_count,
    _In_ const sai_attribute_t *attr_list
    );

/**
 * Routine Description:
 *   @brief Delete an ACL counter
 *
 * Arguments:
 *  @param[in] acl_counter_id - the acl counter id
 *
 * Return Values:
 *    @return  SAI_STATUS_SUCCESS on success
 *             Failure status code on error
 */
typedef sai_status_t (*sai_delete_acl_counter_fn)(
    _In_ sai_object_id_t acl_counter_id
    );

/**
 * Routine Description:
 *   @brief Set ACL counter attribute
 *
 * Arguments:
 *    @param[in] acl_counter_id - the acl counter id
 *    @param[in] attr - attribute
 *
 * Return Values:
 *    @return SAI_STATUS_SUCCESS on success
 *            Failure status code on error
 */
typedef sai_status_t (*sai_set_acl_counter_attribute_fn)(
    _In_ sai_object_id_t acl_counter_id,
    _In_ const sai_attribute_t *attr
    );

/**
 * Routine Description:
 *   @brief Get ACL counter attribute
 *
 * Arguments:
 *    @param[in] acl_counter_id - acl counter id
 *    @param[in] attr_count - number of attributes
 *    @param[out] attr_list - array of attributes
 *
 * Return Values:
 *    @return SAI_STATUS_SUCCESS on success
 *            Failure status code on error
 */
typedef sai_status_t (*sai_get_acl_counter_attribute_fn)(
    _In_ sai_object_id_t acl_counter_id,
    _In_ uint32_t attr_count,
    _Out_ sai_attribute_t *attr_list
    );

/**
 * @brief Port methods table retrieved with sai_api_query()
 */
typedef struct _sai_acl_api_t
{
    sai_create_acl_table_fn             create_acl_table;
    sai_delete_acl_table_fn             delete_acl_table;
    sai_set_acl_table_attribute_fn      set_acl_table_attribute;
    sai_get_acl_table_attribute_fn      get_acl_table_attribute;
    sai_create_acl_entry_fn             create_acl_entry;
    sai_delete_acl_entry_fn             delete_acl_entry;
    sai_set_acl_entry_attribute_fn      set_acl_entry_attribute;
    sai_get_acl_entry_attribute_fn      get_acl_entry_attribute;
    sai_create_acl_counter_fn           create_acl_counter;
    sai_delete_acl_counter_fn           delete_acl_counter;
    sai_set_acl_counter_attribute_fn    set_acl_counter_attribute;
    sai_get_acl_counter_attribute_fn    get_acl_counter_attribute;

} sai_acl_api_t;

/**
 * \}
 */
#endif // __SAIACL_H_

