
#ifndef __TBL_OSPF6_DEFINE_H__
#define __TBL_OSPF6_DEFINE_H__

/* TBL_OSPF6 field defines */
typedef enum
{
    TBL_OSPF6_FLD_KEY                  = 0 ,  /* READ */
    TBL_OSPF6_FLD_ROUTER_ID            = 1 ,  /* READ */
    TBL_OSPF6_FLD_DEFAULT_ORIGINATE    = 2 ,  /* READ */
    TBL_OSPF6_FLD_REDISTRIBUTE         = 3 ,  /* READ */
    TBL_OSPF6_FLD_T_OSPF6_REDISTRIBUTE_CONNECTED_TIMER = 4 ,  /* READ */
    TBL_OSPF6_FLD_T_OSPF6_REDISTRIBUTE_STATIC_TIMER = 5 ,  /* READ */
    TBL_OSPF6_FLD_MAX                  = 6 
} tbl_ospf6_field_id_t;

/* TBL_OSPF6 defines */
typedef struct
{
    uint32               id;
} tbl_ospf6_key_t;

typedef struct
{
    tbl_ospf6_key_t      key;
    addr_ipv4_t          router_id;
    uint32               default_originate;
    uint32               redistribute;
    ctc_task_t           *t_ospf6_redistribute_connected_timer;
    ctc_task_t           *t_ospf6_redistribute_static_timer;
} tbl_ospf6_t;

typedef struct
{
    ctclib_hash_t        *ospf6_hash;
    ctclib_slist_t       *ospf6_list;
} tbl_ospf6_master_t;

#endif /* !__TBL_OSPF6_DEFINE_H__ */

