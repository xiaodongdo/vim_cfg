
#include "proto.h"
#include "gen/tbl_rip_passive_if_define.h"
#include "gen/tbl_rip_passive_if.h"
#include "cdb_data_cmp.h"

tbl_rip_passive_if_master_t *_g_p_tbl_rip_passive_if_master = NULL;

static uint32
_tbl_rip_passive_if_hash_make(tbl_rip_passive_if_t *p_rip_passive_if)
{
    uint32 index = 0;
    uint32 key = 0;
    uint8 *pval = NULL;

    pval = (uint8*)&p_rip_passive_if->key;
    for (index = 0; index < sizeof(p_rip_passive_if->key); index++)
    {
        key += pval[index];
    }

    return key;
}

static bool
_tbl_rip_passive_if_hash_cmp(void *p_arg1, void *p_arg2)
{
    tbl_rip_passive_if_t *p_rip_passive_if1 = (tbl_rip_passive_if_t*)p_arg1;
    tbl_rip_passive_if_t *p_rip_passive_if2 = (tbl_rip_passive_if_t*)p_arg2;

    if (0 == sal_memcmp(&p_rip_passive_if1->key, &p_rip_passive_if2->key, sizeof(tbl_rip_passive_if_key_t)))
    {
        return TRUE;
    }

    return FALSE;
}

int32
tbl_rip_passive_if_add_rip_passive_if_sync(tbl_rip_passive_if_t *p_rip_passive_if, uint32 sync)
{
    tbl_rip_passive_if_master_t *p_master = _g_p_tbl_rip_passive_if_master;
    tbl_rip_passive_if_t *p_db_rip_passive_if = NULL;
    void *p_ret = NULL;

    /* 1. lookup entry exist */
    if (tbl_rip_passive_if_get_rip_passive_if(&p_rip_passive_if->key))
    {
        return PM_E_EXIST;
    }

    /* 2. alloc a new db entry */
    p_db_rip_passive_if = XCALLOC(MEM_TBL_RIP_PASSIVE_IF, sizeof(tbl_rip_passive_if_t));
    if (NULL == p_db_rip_passive_if)
    {
        return PM_E_NO_MEMORY;
    }

    /* 3. evaluate db entry */
    sal_memcpy(p_db_rip_passive_if, p_rip_passive_if, sizeof(tbl_rip_passive_if_t));

    /* 4. add to db */
    p_ret = ctclib_hash_get(p_master->rip_passive_if_hash, (void*)p_db_rip_passive_if, ctclib_hash_alloc_intern);
    if (NULL == p_ret)
    {
        return PM_E_NO_MEMORY;
    }
    ctclib_slistnode_insert_sort(p_master->rip_passive_if_list, p_db_rip_passive_if);

    /* 5. sync to CDB */
    if (sync)
    {
        cdb_sync_tbl_add(TBL_RIP_PASSIVE_IF, p_db_rip_passive_if);
        #ifdef LDB_SYNC_TO_RDB 
        cdb_sync_publish_tbl_add(TBL_RIP_PASSIVE_IF, p_db_rip_passive_if);
        #endif 
    }

    return PM_E_NONE;
}

int32
tbl_rip_passive_if_del_rip_passive_if_sync(tbl_rip_passive_if_key_t *p_rip_passive_if_key, uint32 sync)
{
    tbl_rip_passive_if_master_t *p_master = _g_p_tbl_rip_passive_if_master;
    tbl_rip_passive_if_t *p_db_rip_passive_if = NULL;

    /* 1. lookup entry exist */
    p_db_rip_passive_if = tbl_rip_passive_if_get_rip_passive_if(p_rip_passive_if_key);
    if (NULL == p_db_rip_passive_if)
    {
        return PM_E_NONE;
    }

    /* 2. sync to CDB */
    if (sync)
    {
        cdb_sync_tbl_del(TBL_RIP_PASSIVE_IF, p_db_rip_passive_if);
        #ifdef LDB_SYNC_TO_RDB 
        cdb_sync_publish_tbl_del(TBL_RIP_PASSIVE_IF, p_db_rip_passive_if);
        #endif 
    }

    /* 3. delete from db */
    ctclib_hash_release(p_master->rip_passive_if_hash, (void*)p_db_rip_passive_if);
    ctclib_slistnode_delete(p_master->rip_passive_if_list, p_db_rip_passive_if);

    /* 4. free db entry */
    XFREE(MEM_TBL_RIP_PASSIVE_IF, p_db_rip_passive_if);

    return PM_E_NONE;
}

int32
tbl_rip_passive_if_set_rip_passive_if_field_sync(tbl_rip_passive_if_t *p_rip_passive_if, tbl_rip_passive_if_field_id_t field_id, uint32 sync)
{
    tbl_rip_passive_if_t *p_db_rip_passive_if = NULL;

    /* 1. lookup entry exist */
    p_db_rip_passive_if = tbl_rip_passive_if_get_rip_passive_if(&p_rip_passive_if->key);
    if (NULL == p_db_rip_passive_if)
    {
        return PM_E_NOT_EXIST;
    }

    /* 2. update db entry field */
    switch (field_id)
    {
    case TBL_RIP_PASSIVE_IF_FLD_MAX:
        sal_memcpy(p_db_rip_passive_if, p_rip_passive_if, sizeof(tbl_rip_passive_if_t));
        break;
    default:
        return PM_E_NOT_SUPPORT;
    }

    /* 3. sync to CDB */
    if (sync)
    {
        cdb_sync_tbl_set(TBL_RIP_PASSIVE_IF, field_id, p_db_rip_passive_if);
        #ifdef LDB_SYNC_TO_RDB 
        cdb_sync_publish_tbl_set(TBL_RIP_PASSIVE_IF, field_id, p_db_rip_passive_if);
        #endif 
    }

    return PM_E_NONE;
}

int32
tbl_rip_passive_if_add_rip_passive_if(tbl_rip_passive_if_t *p_rip_passive_if)
{
    return tbl_rip_passive_if_add_rip_passive_if_sync(p_rip_passive_if, TRUE);
}

int32
tbl_rip_passive_if_del_rip_passive_if(tbl_rip_passive_if_key_t *p_rip_passive_if_key)
{
    return tbl_rip_passive_if_del_rip_passive_if_sync(p_rip_passive_if_key, TRUE);
}

int32
tbl_rip_passive_if_set_rip_passive_if_field(tbl_rip_passive_if_t *p_rip_passive_if, tbl_rip_passive_if_field_id_t field_id)
{
    return tbl_rip_passive_if_set_rip_passive_if_field_sync(p_rip_passive_if, field_id, TRUE);
}

tbl_rip_passive_if_t*
tbl_rip_passive_if_get_rip_passive_if(tbl_rip_passive_if_key_t *p_rip_passive_if_key)
{
    tbl_rip_passive_if_master_t *p_master = _g_p_tbl_rip_passive_if_master;
    tbl_rip_passive_if_t lkp;

    sal_memcpy(&lkp.key, p_rip_passive_if_key, sizeof(tbl_rip_passive_if_key_t));
    return ctclib_hash_lookup(p_master->rip_passive_if_hash, &lkp);
}

char*
tbl_rip_passive_if_key_val2str(tbl_rip_passive_if_t *p_rip_passive_if, char *str, uint32 str_len)
{
    sal_memset(str, 0, str_len);
    sal_snprintf(str, str_len, "%s", p_rip_passive_if->key.name);
    return str;
}

int32
tbl_rip_passive_if_key_str2val(char *str, tbl_rip_passive_if_t *p_rip_passive_if)
{
    int32 ret = 0;

    sal_strcpy(p_rip_passive_if->key.name, str);
    return ret;
}

char*
tbl_rip_passive_if_key_name_dump(tbl_rip_passive_if_t *p_rip_passive_if, char *str)
{
    cdb_node_t *p_node = cdb_get_tbl(TBL_RIP_PASSIVE_IF);
    sal_sprintf(str, "%s"KEY_CONNECT_STR"%s", p_node->name, 
        p_rip_passive_if->key.name);
    return str;
}

char*
tbl_rip_passive_if_key_value_dump(tbl_rip_passive_if_t *p_rip_passive_if, char *str)
{
    sal_sprintf(str, "%s", p_rip_passive_if->key.name);
    return str;
}

char*
tbl_rip_passive_if_field_name_dump(tbl_rip_passive_if_t *p_rip_passive_if, int32 field_id, char *str)
{

    cdb_node_t *p_node = cdb_get_tbl(TBL_RIP_PASSIVE_IF);
    cdb_tbl_info_t *p_tbl_info = p_node->tbl_info;
    if (field_id >= TBL_RIP_PASSIVE_IF_FLD_MAX)
    {
        return NULL;
    }

    sal_sprintf(str, "%s", p_tbl_info->field[field_id].name);

    return str;
}

char*
tbl_rip_passive_if_field_value_dump(tbl_rip_passive_if_t *p_rip_passive_if, int32 field_id, char* str)
{
    if (FLD_MATCH(TBL_RIP_PASSIVE_IF_FLD_KEY, field_id))
    {
        sal_sprintf(str, "%s", p_rip_passive_if->key.name);
    }
    return str;
}

char**
tbl_rip_passive_if_table_dump(tbl_rip_passive_if_t *p_rip_passive_if, char **str)
{
    char buf[MAX_CMD_STR_LEN];
    int i = 0, j= 0;

    sal_sprintf(str[j++], "%s",
         tbl_rip_passive_if_key_name_dump(p_rip_passive_if, buf));
    for(i=1; i<TBL_RIP_PASSIVE_IF_FLD_MAX; i++)
    {
        sal_sprintf(str[j++], "%s",
            tbl_rip_passive_if_field_name_dump(p_rip_passive_if, i, buf));
        sal_sprintf(str[j++], "%s",
            tbl_rip_passive_if_field_value_dump(p_rip_passive_if, i, buf));
    }
    return str;
}

int32
tbl_rip_passive_if_field_value_parser(char *str, int32 field_id, tbl_rip_passive_if_t *p_rip_passive_if)
{
    if (FLD_MATCH(TBL_RIP_PASSIVE_IF_FLD_KEY, field_id))
    {
        sal_strcpy(p_rip_passive_if->key.name, str);
    }
    return PM_E_NONE;
}

int32
tbl_rip_passive_if_table_parser(char** array, char* key_value,tbl_rip_passive_if_t *p_rip_passive_if)
{
    int32 rc;
    int i = 0, j= 0;
    cdb_node_t *p_node = cdb_get_tbl(TBL_RIP_PASSIVE_IF);
    cdb_tbl_info_t *p_tbl_info = p_node->tbl_info;

    PM_E_RETURN(tbl_rip_passive_if_key_str2val(key_value, p_rip_passive_if));

    for(i=1; i<TBL_RIP_PASSIVE_IF_FLD_MAX; i++)
    {
        if(sal_strncmp(array[j++], p_tbl_info->field[i].name, sal_strlen(p_tbl_info->field[i].name)))
        {
             return PM_E_INVALID_PARAM;
        }
        PM_E_RETURN(tbl_rip_passive_if_field_value_parser( array[j++], i, p_rip_passive_if));
    }

    return PM_E_NONE;
}

int32
tbl_rip_passive_if_dump_one(tbl_rip_passive_if_t *p_rip_passive_if, tbl_iter_args_t *pargs)
{
    cdb_node_t *p_node = cdb_get_tbl(TBL_RIP_PASSIVE_IF);
    field_parse_t *p_field = pargs->argv[0];
    FILE *fp = pargs->argv[1];
    cdb_tbl_info_t *p_tbl_info = p_node->tbl_info;

    if (FLD_MATCH(TBL_RIP_PASSIVE_IF_FLD_KEY, p_field->field_id[0]))
    {
        sal_fprintf(fp, "/%s"CMD_EQUAL_STR"%s", p_tbl_info->field[TBL_RIP_PASSIVE_IF_FLD_KEY].name,
            p_rip_passive_if->key.name);
    }

    sal_fprintf(fp, "\n");

    return PM_E_NONE;
}

int32
tbl_rip_passive_if_iterate(TBL_ITER_CB_FUNC fn, tbl_iter_args_t *pargs)
{
    tbl_rip_passive_if_master_t *p_master = _g_p_tbl_rip_passive_if_master;
    ctclib_slistnode_t *listnode = NULL;
    ctclib_slistnode_t *next = NULL;
    tbl_rip_passive_if_t *p_db_rip_passive_if = NULL;
    int32 rc = PM_E_NONE;

    if (NULL == p_master)
    {
        return PM_E_NONE;
    }

    CTCLIB_SLIST_LOOP_DEL(p_master->rip_passive_if_list, p_db_rip_passive_if, listnode, next)
    {
        rc |= fn(p_db_rip_passive_if, pargs);
    }

    return (rc) ? PM_E_FAIL : PM_E_NONE;
}

tbl_rip_passive_if_master_t*
tbl_rip_passive_if_get_master()
{
    return _g_p_tbl_rip_passive_if_master;
}

tbl_rip_passive_if_master_t*
tbl_rip_passive_if_init_rip_passive_if()
{
    _g_p_tbl_rip_passive_if_master = XCALLOC(MEM_TBL_MASTER, sizeof(tbl_rip_passive_if_master_t));
    _g_p_tbl_rip_passive_if_master->rip_passive_if_hash = ctclib_hash_create(_tbl_rip_passive_if_hash_make, _tbl_rip_passive_if_hash_cmp);
    _g_p_tbl_rip_passive_if_master->rip_passive_if_list = ctclib_slist_create(tbl_rip_passive_if_cmp, NULL);
    return _g_p_tbl_rip_passive_if_master;
}

