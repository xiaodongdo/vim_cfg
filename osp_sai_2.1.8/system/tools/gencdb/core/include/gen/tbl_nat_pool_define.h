
#ifndef __TBL_NAT_POOL_DEFINE_H__
#define __TBL_NAT_POOL_DEFINE_H__

/* TBL_NAT_POOL field defines */
typedef enum
{
    TBL_NAT_POOL_FLD_KEY                  = 0 ,  /* READ */
    TBL_NAT_POOL_FLD_IP_LO                = 1 ,  /* RW */
    TBL_NAT_POOL_FLD_IP_HI                = 2 ,  /* RW */
    TBL_NAT_POOL_FLD_IP_MASK              = 3 ,  /* RW */
    TBL_NAT_POOL_FLD_REFER_OIF            = 4 ,  /* RW */
    TBL_NAT_POOL_FLD_MAX                  = 5 
} tbl_nat_pool_field_id_t;

/* TBL_NAT_POOL defines */
typedef struct
{
    char                 name[GLB_NAT_POOL_NAME_LEN+1];
} tbl_nat_pool_key_t;

typedef struct
{
    tbl_nat_pool_key_t   key;
    addr_t               ip_lo;               /* IP low, configured*/
    addr_t               ip_hi;               /* IP high, configured*/
    addr_t               ip_mask;             /* IP mask, configured*/
    char                 refer_oif[IFNAME_SIZE+1]; /* nat outside interface name, find it by ip*/
} tbl_nat_pool_t;

typedef struct
{
    ctclib_hash_t        *nat_pool_hash;
    ctclib_slist_t       *nat_pool_list;
} tbl_nat_pool_master_t;

#endif /* !__TBL_NAT_POOL_DEFINE_H__ */

