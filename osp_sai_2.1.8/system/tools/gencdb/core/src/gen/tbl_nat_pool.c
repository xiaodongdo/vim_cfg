
#include "proto.h"
#include "gen/tbl_nat_pool_define.h"
#include "gen/tbl_nat_pool.h"
#include "cdb_data_cmp.h"

tbl_nat_pool_master_t *_g_p_tbl_nat_pool_master = NULL;

static uint32
_tbl_nat_pool_hash_make(tbl_nat_pool_t *p_nat_pool)
{
    uint32 index = 0;
    uint32 key = 0;
    uint8 *pval = NULL;

    pval = (uint8*)&p_nat_pool->key;
    for (index = 0; index < sizeof(p_nat_pool->key); index++)
    {
        key += pval[index];
    }

    return key;
}

static bool
_tbl_nat_pool_hash_cmp(void *p_arg1, void *p_arg2)
{
    tbl_nat_pool_t *p_nat_pool1 = (tbl_nat_pool_t*)p_arg1;
    tbl_nat_pool_t *p_nat_pool2 = (tbl_nat_pool_t*)p_arg2;

    if (0 == sal_memcmp(&p_nat_pool1->key, &p_nat_pool2->key, sizeof(tbl_nat_pool_key_t)))
    {
        return TRUE;
    }

    return FALSE;
}

int32
tbl_nat_pool_add_nat_pool_sync(tbl_nat_pool_t *p_nat_pool, uint32 sync)
{
    tbl_nat_pool_master_t *p_master = _g_p_tbl_nat_pool_master;
    tbl_nat_pool_t *p_db_nat_pool = NULL;
    void *p_ret = NULL;

    /* 1. lookup entry exist */
    if (tbl_nat_pool_get_nat_pool(&p_nat_pool->key))
    {
        return PM_E_EXIST;
    }

    /* 2. alloc a new db entry */
    p_db_nat_pool = XCALLOC(MEM_TBL_NAT_POOL, sizeof(tbl_nat_pool_t));
    if (NULL == p_db_nat_pool)
    {
        return PM_E_NO_MEMORY;
    }

    /* 3. evaluate db entry */
    sal_memcpy(p_db_nat_pool, p_nat_pool, sizeof(tbl_nat_pool_t));

    /* 4. add to db */
    p_ret = ctclib_hash_get(p_master->nat_pool_hash, (void*)p_db_nat_pool, ctclib_hash_alloc_intern);
    if (NULL == p_ret)
    {
        return PM_E_NO_MEMORY;
    }
    ctclib_slistnode_insert_sort(p_master->nat_pool_list, p_db_nat_pool);

    /* 5. sync to CDB */
    if (sync)
    {
        cdb_sync_tbl_add(TBL_NAT_POOL, p_db_nat_pool);
        #ifdef LDB_SYNC_TO_RDB 
        cdb_sync_publish_tbl_add(TBL_NAT_POOL, p_db_nat_pool);
        #endif 
    }

    return PM_E_NONE;
}

int32
tbl_nat_pool_del_nat_pool_sync(tbl_nat_pool_key_t *p_nat_pool_key, uint32 sync)
{
    tbl_nat_pool_master_t *p_master = _g_p_tbl_nat_pool_master;
    tbl_nat_pool_t *p_db_nat_pool = NULL;

    /* 1. lookup entry exist */
    p_db_nat_pool = tbl_nat_pool_get_nat_pool(p_nat_pool_key);
    if (NULL == p_db_nat_pool)
    {
        return PM_E_NONE;
    }

    /* 2. sync to CDB */
    if (sync)
    {
        cdb_sync_tbl_del(TBL_NAT_POOL, p_db_nat_pool);
        #ifdef LDB_SYNC_TO_RDB 
        cdb_sync_publish_tbl_del(TBL_NAT_POOL, p_db_nat_pool);
        #endif 
    }

    /* 3. delete from db */
    ctclib_hash_release(p_master->nat_pool_hash, (void*)p_db_nat_pool);
    ctclib_slistnode_delete(p_master->nat_pool_list, p_db_nat_pool);

    /* 4. free db entry */
    XFREE(MEM_TBL_NAT_POOL, p_db_nat_pool);

    return PM_E_NONE;
}

int32
tbl_nat_pool_set_nat_pool_field_sync(tbl_nat_pool_t *p_nat_pool, tbl_nat_pool_field_id_t field_id, uint32 sync)
{
    tbl_nat_pool_t *p_db_nat_pool = NULL;

    /* 1. lookup entry exist */
    p_db_nat_pool = tbl_nat_pool_get_nat_pool(&p_nat_pool->key);
    if (NULL == p_db_nat_pool)
    {
        return PM_E_NOT_EXIST;
    }

    /* 2. update db entry field */
    switch (field_id)
    {
    case TBL_NAT_POOL_FLD_IP_LO:
        sal_memcpy(&p_db_nat_pool->ip_lo, &p_nat_pool->ip_lo, sizeof(p_nat_pool->ip_lo));
        break;

    case TBL_NAT_POOL_FLD_IP_HI:
        sal_memcpy(&p_db_nat_pool->ip_hi, &p_nat_pool->ip_hi, sizeof(p_nat_pool->ip_hi));
        break;

    case TBL_NAT_POOL_FLD_IP_MASK:
        sal_memcpy(&p_db_nat_pool->ip_mask, &p_nat_pool->ip_mask, sizeof(p_nat_pool->ip_mask));
        break;

    case TBL_NAT_POOL_FLD_REFER_OIF:
        sal_memcpy(p_db_nat_pool->refer_oif, p_nat_pool->refer_oif, sizeof(p_nat_pool->refer_oif));
        break;

    case TBL_NAT_POOL_FLD_MAX:
        sal_memcpy(p_db_nat_pool, p_nat_pool, sizeof(tbl_nat_pool_t));
        break;
    default:
        return PM_E_NOT_SUPPORT;
    }

    /* 3. sync to CDB */
    if (sync)
    {
        cdb_sync_tbl_set(TBL_NAT_POOL, field_id, p_db_nat_pool);
        #ifdef LDB_SYNC_TO_RDB 
        cdb_sync_publish_tbl_set(TBL_NAT_POOL, field_id, p_db_nat_pool);
        #endif 
    }

    return PM_E_NONE;
}

int32
tbl_nat_pool_add_nat_pool(tbl_nat_pool_t *p_nat_pool)
{
    return tbl_nat_pool_add_nat_pool_sync(p_nat_pool, TRUE);
}

int32
tbl_nat_pool_del_nat_pool(tbl_nat_pool_key_t *p_nat_pool_key)
{
    return tbl_nat_pool_del_nat_pool_sync(p_nat_pool_key, TRUE);
}

int32
tbl_nat_pool_set_nat_pool_field(tbl_nat_pool_t *p_nat_pool, tbl_nat_pool_field_id_t field_id)
{
    return tbl_nat_pool_set_nat_pool_field_sync(p_nat_pool, field_id, TRUE);
}

tbl_nat_pool_t*
tbl_nat_pool_get_nat_pool(tbl_nat_pool_key_t *p_nat_pool_key)
{
    tbl_nat_pool_master_t *p_master = _g_p_tbl_nat_pool_master;
    tbl_nat_pool_t lkp;

    sal_memcpy(&lkp.key, p_nat_pool_key, sizeof(tbl_nat_pool_key_t));
    return ctclib_hash_lookup(p_master->nat_pool_hash, &lkp);
}

char*
tbl_nat_pool_key_val2str(tbl_nat_pool_t *p_nat_pool, char *str, uint32 str_len)
{
    sal_memset(str, 0, str_len);
    sal_snprintf(str, str_len, "%s", p_nat_pool->key.name);
    return str;
}

int32
tbl_nat_pool_key_str2val(char *str, tbl_nat_pool_t *p_nat_pool)
{
    int32 ret = 0;

    sal_strcpy(p_nat_pool->key.name, str);
    return ret;
}

char*
tbl_nat_pool_key_name_dump(tbl_nat_pool_t *p_nat_pool, char *str)
{
    cdb_node_t *p_node = cdb_get_tbl(TBL_NAT_POOL);
    sal_sprintf(str, "%s"KEY_CONNECT_STR"%s", p_node->name, 
        p_nat_pool->key.name);
    return str;
}

char*
tbl_nat_pool_key_value_dump(tbl_nat_pool_t *p_nat_pool, char *str)
{
    sal_sprintf(str, "%s", p_nat_pool->key.name);
    return str;
}

char*
tbl_nat_pool_field_name_dump(tbl_nat_pool_t *p_nat_pool, int32 field_id, char *str)
{

    cdb_node_t *p_node = cdb_get_tbl(TBL_NAT_POOL);
    cdb_tbl_info_t *p_tbl_info = p_node->tbl_info;
    if (field_id >= TBL_NAT_POOL_FLD_MAX)
    {
        return NULL;
    }

    sal_sprintf(str, "%s", p_tbl_info->field[field_id].name);

    return str;
}

char*
tbl_nat_pool_field_value_dump(tbl_nat_pool_t *p_nat_pool, int32 field_id, char* str)
{
    if (FLD_MATCH(TBL_NAT_POOL_FLD_KEY, field_id))
    {
        sal_sprintf(str, "%s", p_nat_pool->key.name);
    }
    if (FLD_MATCH(TBL_NAT_POOL_FLD_IP_LO, field_id))
    {
        cdb_addr_val2str(str, MAX_CMD_STR_LEN, &p_nat_pool->ip_lo);
    }
    if (FLD_MATCH(TBL_NAT_POOL_FLD_IP_HI, field_id))
    {
        cdb_addr_val2str(str, MAX_CMD_STR_LEN, &p_nat_pool->ip_hi);
    }
    if (FLD_MATCH(TBL_NAT_POOL_FLD_IP_MASK, field_id))
    {
        cdb_addr_val2str(str, MAX_CMD_STR_LEN, &p_nat_pool->ip_mask);
    }
    if (FLD_MATCH(TBL_NAT_POOL_FLD_REFER_OIF, field_id))
    {
        sal_sprintf(str, "%s", p_nat_pool->refer_oif);
    }
    return str;
}

char**
tbl_nat_pool_table_dump(tbl_nat_pool_t *p_nat_pool, char **str)
{
    char buf[MAX_CMD_STR_LEN];
    int i = 0, j= 0;

    sal_sprintf(str[j++], "%s",
         tbl_nat_pool_key_name_dump(p_nat_pool, buf));
    for(i=1; i<TBL_NAT_POOL_FLD_MAX; i++)
    {
        sal_sprintf(str[j++], "%s",
            tbl_nat_pool_field_name_dump(p_nat_pool, i, buf));
        sal_sprintf(str[j++], "%s",
            tbl_nat_pool_field_value_dump(p_nat_pool, i, buf));
    }
    return str;
}

int32
tbl_nat_pool_field_value_parser(char *str, int32 field_id, tbl_nat_pool_t *p_nat_pool)
{
    if (FLD_MATCH(TBL_NAT_POOL_FLD_KEY, field_id))
    {
        sal_strcpy(p_nat_pool->key.name, str);
    }
    if (FLD_MATCH(TBL_NAT_POOL_FLD_IP_LO, field_id))
    {
        cdb_addr_str2val(&p_nat_pool->ip_lo, str, 0);
    }
    if (FLD_MATCH(TBL_NAT_POOL_FLD_IP_HI, field_id))
    {
        cdb_addr_str2val(&p_nat_pool->ip_hi, str, 0);
    }
    if (FLD_MATCH(TBL_NAT_POOL_FLD_IP_MASK, field_id))
    {
        cdb_addr_str2val(&p_nat_pool->ip_mask, str, 0);
    }
    if (FLD_MATCH(TBL_NAT_POOL_FLD_REFER_OIF, field_id))
    {
        sal_strcpy(p_nat_pool->refer_oif, str);
    }
    return PM_E_NONE;
}

int32
tbl_nat_pool_table_parser(char** array, char* key_value,tbl_nat_pool_t *p_nat_pool)
{
    int32 rc;
    int i = 0, j= 0;
    cdb_node_t *p_node = cdb_get_tbl(TBL_NAT_POOL);
    cdb_tbl_info_t *p_tbl_info = p_node->tbl_info;

    PM_E_RETURN(tbl_nat_pool_key_str2val(key_value, p_nat_pool));

    for(i=1; i<TBL_NAT_POOL_FLD_MAX; i++)
    {
        if(sal_strncmp(array[j++], p_tbl_info->field[i].name, sal_strlen(p_tbl_info->field[i].name)))
        {
             return PM_E_INVALID_PARAM;
        }
        PM_E_RETURN(tbl_nat_pool_field_value_parser( array[j++], i, p_nat_pool));
    }

    return PM_E_NONE;
}

int32
tbl_nat_pool_dump_one(tbl_nat_pool_t *p_nat_pool, tbl_iter_args_t *pargs)
{
    cdb_node_t *p_node = cdb_get_tbl(TBL_NAT_POOL);
    field_parse_t *p_field = pargs->argv[0];
    FILE *fp = pargs->argv[1];
    cdb_tbl_info_t *p_tbl_info = p_node->tbl_info;

    char buf[MAX_CMD_STR_LEN];

    if (FLD_MATCH(TBL_NAT_POOL_FLD_KEY, p_field->field_id[0]))
    {
        sal_fprintf(fp, "/%s"CMD_EQUAL_STR"%s", p_tbl_info->field[TBL_NAT_POOL_FLD_KEY].name,
            p_nat_pool->key.name);
    }
    if (FLD_MATCH(TBL_NAT_POOL_FLD_IP_LO, p_field->field_id[0]))
    {
        sal_fprintf(fp, "/%s"CMD_EQUAL_STR"%s", p_tbl_info->field[TBL_NAT_POOL_FLD_IP_LO].name, 
            cdb_addr_val2str(buf, MAX_CMD_STR_LEN, &p_nat_pool->ip_lo));
    }
    if (FLD_MATCH(TBL_NAT_POOL_FLD_IP_HI, p_field->field_id[0]))
    {
        sal_fprintf(fp, "/%s"CMD_EQUAL_STR"%s", p_tbl_info->field[TBL_NAT_POOL_FLD_IP_HI].name, 
            cdb_addr_val2str(buf, MAX_CMD_STR_LEN, &p_nat_pool->ip_hi));
    }
    if (FLD_MATCH(TBL_NAT_POOL_FLD_IP_MASK, p_field->field_id[0]))
    {
        sal_fprintf(fp, "/%s"CMD_EQUAL_STR"%s", p_tbl_info->field[TBL_NAT_POOL_FLD_IP_MASK].name, 
            cdb_addr_val2str(buf, MAX_CMD_STR_LEN, &p_nat_pool->ip_mask));
    }
    if (FLD_MATCH(TBL_NAT_POOL_FLD_REFER_OIF, p_field->field_id[0]))
    {
        sal_fprintf(fp, "/%s"CMD_EQUAL_STR"%s", p_tbl_info->field[TBL_NAT_POOL_FLD_REFER_OIF].name,
            p_nat_pool->refer_oif);
    }

    sal_fprintf(fp, "\n");

    return PM_E_NONE;
}

int32
tbl_nat_pool_iterate(TBL_ITER_CB_FUNC fn, tbl_iter_args_t *pargs)
{
    tbl_nat_pool_master_t *p_master = _g_p_tbl_nat_pool_master;
    ctclib_slistnode_t *listnode = NULL;
    ctclib_slistnode_t *next = NULL;
    tbl_nat_pool_t *p_db_nat_pool = NULL;
    int32 rc = PM_E_NONE;

    if (NULL == p_master)
    {
        return PM_E_NONE;
    }

    CTCLIB_SLIST_LOOP_DEL(p_master->nat_pool_list, p_db_nat_pool, listnode, next)
    {
        rc |= fn(p_db_nat_pool, pargs);
    }

    return (rc) ? PM_E_FAIL : PM_E_NONE;
}

tbl_nat_pool_master_t*
tbl_nat_pool_get_master()
{
    return _g_p_tbl_nat_pool_master;
}

tbl_nat_pool_master_t*
tbl_nat_pool_init_nat_pool()
{
    _g_p_tbl_nat_pool_master = XCALLOC(MEM_TBL_MASTER, sizeof(tbl_nat_pool_master_t));
    _g_p_tbl_nat_pool_master->nat_pool_hash = ctclib_hash_create(_tbl_nat_pool_hash_make, _tbl_nat_pool_hash_cmp);
    _g_p_tbl_nat_pool_master->nat_pool_list = ctclib_slist_create(tbl_nat_pool_cmp, NULL);
    return _g_p_tbl_nat_pool_master;
}

