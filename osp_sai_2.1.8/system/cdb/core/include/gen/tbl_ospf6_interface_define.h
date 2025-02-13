
#ifndef __TBL_OSPF6_INTERFACE_DEFINE_H__
#define __TBL_OSPF6_INTERFACE_DEFINE_H__

/* TBL_OSPF6_INTERFACE field defines */
typedef enum
{
    TBL_OSPF6_INTERFACE_FLD_KEY                  = 0 ,  /* READ */
    TBL_OSPF6_INTERFACE_FLD_INTERFACE            = 1 ,  /* READ */
    TBL_OSPF6_INTERFACE_FLD_AREA_ID              = 2 ,  /* READ */
    TBL_OSPF6_INTERFACE_FLD_MAX                  = 3 
} tbl_ospf6_interface_field_id_t;

/* TBL_OSPF6_INTERFACE defines */
typedef struct
{
    char                 name[IFNAME_SIZE];
} tbl_ospf6_interface_key_t;

typedef struct
{
    tbl_ospf6_interface_key_t key;
    char                 interface[IFNAME_SIZE];
    addr_ipv4_t          area_id;
} tbl_ospf6_interface_t;

typedef struct
{
    ctclib_hash_t        *ospf6_interface_hash;
    ctclib_slist_t       *ospf6_interface_list;
} tbl_ospf6_interface_master_t;

#endif /* !__TBL_OSPF6_INTERFACE_DEFINE_H__ */

