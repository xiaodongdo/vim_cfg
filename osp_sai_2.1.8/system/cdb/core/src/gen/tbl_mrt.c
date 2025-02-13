
#include "proto.h"
#include "gen/tbl_mrt_define.h"
#include "gen/tbl_mrt.h"
#include "cdb_data_cmp.h"

tbl_mrt_master_t *_g_p_tbl_mrt_master = NULL;

static uint32
_tbl_mrt_hash_make(tbl_mrt_t *p_mrt)
{
    uint32 index = 0;
    uint32 key = 0;
    uint8 *pval = NULL;

    pval = (uint8*)&p_mrt->key;
    for (index = 0; index < sizeof(p_mrt->key); index++)
    {
        key += pval[index];
    }

    return key;
}

static bool
_tbl_mrt_hash_cmp(void *p_arg1, void *p_arg2)
{
    tbl_mrt_t *p_mrt1 = (tbl_mrt_t*)p_arg1;
    tbl_mrt_t *p_mrt2 = (tbl_mrt_t*)p_arg2;

    if (0 == sal_memcmp(&p_mrt1->key, &p_mrt2->key, sizeof(tbl_mrt_key_t)))
    {
        return TRUE;
    }

    return FALSE;
}

int32
tbl_mrt_add_mrt_sync(tbl_mrt_t *p_mrt, uint32 sync)
{
    tbl_mrt_master_t *p_master = _g_p_tbl_mrt_master;
    tbl_mrt_t *p_db_mrt = NULL;
    void *p_ret = NULL;

    /* 1. lookup entry exist */
    if (tbl_mrt_get_mrt(&p_mrt->key))
    {
        return PM_E_EXIST;
    }

    /* 2. alloc a new db entry */
    p_db_mrt = XCALLOC(MEM_TBL_MRT, sizeof(tbl_mrt_t));
    if (NULL == p_db_mrt)
    {
        return PM_E_NO_MEMORY;
    }

    /* 3. evaluate db entry */
    sal_memcpy(p_db_mrt, p_mrt, sizeof(tbl_mrt_t));

    /* 4. add to db */
    p_ret = ctclib_hash_get(p_master->mrt_hash, (void*)p_db_mrt, ctclib_hash_alloc_intern);
    if (NULL == p_ret)
    {
        return PM_E_NO_MEMORY;
    }

    /* 5. sync to CDB */
    if (sync)
    {
        cdb_sync_tbl_add(TBL_MRT, p_db_mrt);
        #ifdef LDB_SYNC_TO_RDB 
        cdb_sync_publish_tbl_add(TBL_MRT, p_db_mrt);
        #endif 
    }

    return PM_E_NONE;
}

int32
tbl_mrt_del_mrt_sync(tbl_mrt_key_t *p_mrt_key, uint32 sync)
{
    tbl_mrt_master_t *p_master = _g_p_tbl_mrt_master;
    tbl_mrt_t *p_db_mrt = NULL;

    /* 1. lookup entry exist */
    p_db_mrt = tbl_mrt_get_mrt(p_mrt_key);
    if (NULL == p_db_mrt)
    {
        return PM_E_NONE;
    }

    /* 2. sync to CDB */
    if (sync)
    {
        cdb_sync_tbl_del(TBL_MRT, p_db_mrt);
        #ifdef LDB_SYNC_TO_RDB 
        cdb_sync_publish_tbl_del(TBL_MRT, p_db_mrt);
        #endif 
    }

    /* 3. delete from db */
    ctclib_hash_release(p_master->mrt_hash, (void*)p_db_mrt);

    /* 4. free db entry */
    XFREE(MEM_TBL_MRT, p_db_mrt);

    return PM_E_NONE;
}

int32
tbl_mrt_set_mrt_field_sync(tbl_mrt_t *p_mrt, tbl_mrt_field_id_t field_id, uint32 sync)
{
    tbl_mrt_t *p_db_mrt = NULL;

    /* 1. lookup entry exist */
    p_db_mrt = tbl_mrt_get_mrt(&p_mrt->key);
    if (NULL == p_db_mrt)
    {
        return PM_E_NOT_EXIST;
    }

    /* 2. update db entry field */
    switch (field_id)
    {
    case TBL_MRT_FLD_RPF_IFINDEX:
        p_db_mrt->rpf_ifindex = p_mrt->rpf_ifindex;
        break;

    case TBL_MRT_FLD_OIF_INDEX_BMP:
        sal_memcpy(p_db_mrt->oif_index_bmp, p_mrt->oif_index_bmp, sizeof(p_mrt->oif_index_bmp));
        break;

    case TBL_MRT_FLD_NHG_OID:
        p_db_mrt->nhg_oid = p_mrt->nhg_oid;
        break;

    case TBL_MRT_FLD_PACKET_MATCH:
        p_db_mrt->packet_match = p_mrt->packet_match;
        break;

    case TBL_MRT_FLD_BYTE_MATCH:
        p_db_mrt->byte_match = p_mrt->byte_match;
        break;

    case TBL_MRT_FLD_STATIC_MROUTE:
        p_db_mrt->static_mroute = p_mrt->static_mroute;
        break;

    case TBL_MRT_FLD_MAX:
        sal_memcpy(p_db_mrt, p_mrt, sizeof(tbl_mrt_t));
        break;
    default:
        return PM_E_NOT_SUPPORT;
    }

    /* 3. sync to CDB */
    if (sync)
    {
        cdb_sync_tbl_set(TBL_MRT, field_id, p_db_mrt);
        #ifdef LDB_SYNC_TO_RDB 
        cdb_sync_publish_tbl_set(TBL_MRT, field_id, p_db_mrt);
        #endif 
    }

    return PM_E_NONE;
}

int32
tbl_mrt_add_mrt(tbl_mrt_t *p_mrt)
{
    return tbl_mrt_add_mrt_sync(p_mrt, TRUE);
}

int32
tbl_mrt_del_mrt(tbl_mrt_key_t *p_mrt_key)
{
    return tbl_mrt_del_mrt_sync(p_mrt_key, TRUE);
}

int32
tbl_mrt_set_mrt_field(tbl_mrt_t *p_mrt, tbl_mrt_field_id_t field_id)
{
    return tbl_mrt_set_mrt_field_sync(p_mrt, field_id, TRUE);
}

tbl_mrt_t*
tbl_mrt_get_mrt(tbl_mrt_key_t *p_mrt_key)
{
    tbl_mrt_master_t *p_master = _g_p_tbl_mrt_master;
    tbl_mrt_t lkp;

    sal_memcpy(&lkp.key, p_mrt_key, sizeof(tbl_mrt_key_t));
    return ctclib_hash_lookup(p_master->mrt_hash, &lkp);
}

char*
tbl_mrt_key_val2str(tbl_mrt_t *p_mrt, char *str, uint32 str_len)
{
    sal_memset(str, 0, str_len);
    char buf[MAX_CMD_STR_LEN];

    sal_snprintf(str, str_len, "%s", 
            cdb_mrt_val2str(buf, MAX_CMD_STR_LEN, &p_mrt->key));
    return str;
}

int32
tbl_mrt_key_str2val(char *str, tbl_mrt_t *p_mrt)
{
    int32 ret = 0;

    ret = cdb_mrt_str2val(str, &p_mrt->key);
    return ret;
}

char*
tbl_mrt_key_name_dump(tbl_mrt_t *p_mrt, char *str)
{
    cdb_node_t *p_node = cdb_get_tbl(TBL_MRT);
    char buf[MAX_CMD_STR_LEN];

    sal_sprintf(str, "%s"KEY_CONNECT_STR"%s", p_node->name, 
            cdb_mrt_val2str(buf, MAX_CMD_STR_LEN, &p_mrt->key));
    return str;
}

char*
tbl_mrt_key_value_dump(tbl_mrt_t *p_mrt, char *str)
{

    cdb_mrt_val2str(str, MAX_CMD_STR_LEN, &p_mrt->key);
    return str;
}

char*
tbl_mrt_field_name_dump(tbl_mrt_t *p_mrt, int32 field_id, char *str)
{

    cdb_node_t *p_node = cdb_get_tbl(TBL_MRT);
    cdb_tbl_info_t *p_tbl_info = p_node->tbl_info;
    if (field_id >= TBL_MRT_FLD_MAX)
    {
        return NULL;
    }

    sal_sprintf(str, "%s", p_tbl_info->field[field_id].name);

    return str;
}

char*
tbl_mrt_field_value_dump(tbl_mrt_t *p_mrt, int32 field_id, char* str)
{
    if (FLD_MATCH(TBL_MRT_FLD_KEY, field_id))
    {
        cdb_mrt_val2str(str, MAX_CMD_STR_LEN, &p_mrt->key);
    }
    if (FLD_MATCH(TBL_MRT_FLD_RPF_IFINDEX, field_id))
    {
        sal_sprintf(str, "%u", p_mrt->rpf_ifindex);
    }
    if (FLD_MATCH(TBL_MRT_FLD_OIF_INDEX_BMP, field_id))
    {
        cdb_bitmap_val2str(str, MAX_CMD_STR_LEN, p_mrt->oif_index_bmp, sizeof(p_mrt->oif_index_bmp)/4);
    }
    if (FLD_MATCH(TBL_MRT_FLD_NHG_OID, field_id))
    {
        sal_sprintf(str, "%"PRIu64, p_mrt->nhg_oid);
    }
    if (FLD_MATCH(TBL_MRT_FLD_PACKET_MATCH, field_id))
    {
        sal_sprintf(str, "%"PRIu64, p_mrt->packet_match);
    }
    if (FLD_MATCH(TBL_MRT_FLD_BYTE_MATCH, field_id))
    {
        sal_sprintf(str, "%"PRIu64, p_mrt->byte_match);
    }
    if (FLD_MATCH(TBL_MRT_FLD_STATIC_MROUTE, field_id))
    {
        sal_sprintf(str, "%u", p_mrt->static_mroute);
    }
    return str;
}

char**
tbl_mrt_table_dump(tbl_mrt_t *p_mrt, char **str)
{
    char buf[MAX_CMD_STR_LEN];
    int i = 0, j= 0;

    sal_sprintf(str[j++], "%s",
         tbl_mrt_key_name_dump(p_mrt, buf));
    for(i=1; i<TBL_MRT_FLD_MAX; i++)
    {
        sal_sprintf(str[j++], "%s",
            tbl_mrt_field_name_dump(p_mrt, i, buf));
        sal_sprintf(str[j++], "%s",
            tbl_mrt_field_value_dump(p_mrt, i, buf));
    }
    return str;
}

int32
tbl_mrt_field_value_parser(char *str, int32 field_id, tbl_mrt_t *p_mrt)
{
    if (FLD_MATCH(TBL_MRT_FLD_KEY, field_id))
    {
        cdb_mrt_str2val(str, &p_mrt->key);
    }
    if (FLD_MATCH(TBL_MRT_FLD_RPF_IFINDEX, field_id))
    {
        int32 ret;
        p_mrt->rpf_ifindex = cdb_uint_str2val(str, &ret);
    }
    if (FLD_MATCH(TBL_MRT_FLD_OIF_INDEX_BMP, field_id))
    {
        char err[MAX_CMD_STR_LEN];
        cdb_bitmap_str2val(str, 0, GLB_UINT16_BMP_WORD_MAX, p_mrt->oif_index_bmp, sizeof(p_mrt->oif_index_bmp)/4, err);
    }
    if (FLD_MATCH(TBL_MRT_FLD_NHG_OID, field_id))
    {
        int32 ret;
        p_mrt->nhg_oid = cdb_uint64_str2val(str, &ret);
    }
    if (FLD_MATCH(TBL_MRT_FLD_PACKET_MATCH, field_id))
    {
        int32 ret;
        p_mrt->packet_match = cdb_uint64_str2val(str, &ret);
    }
    if (FLD_MATCH(TBL_MRT_FLD_BYTE_MATCH, field_id))
    {
        int32 ret;
        p_mrt->byte_match = cdb_uint64_str2val(str, &ret);
    }
    if (FLD_MATCH(TBL_MRT_FLD_STATIC_MROUTE, field_id))
    {
        int32 ret;
        p_mrt->static_mroute = cdb_uint_str2val(str, &ret);
    }
    return PM_E_NONE;
}

int32
tbl_mrt_table_parser(char** array, char* key_value,tbl_mrt_t *p_mrt)
{
    int32 rc;
    int i = 0, j= 0;
    cdb_node_t *p_node = cdb_get_tbl(TBL_MRT);
    cdb_tbl_info_t *p_tbl_info = p_node->tbl_info;

    PM_E_RETURN(tbl_mrt_key_str2val(key_value, p_mrt));

    for(i=1; i<TBL_MRT_FLD_MAX; i++)
    {
        if(sal_strncmp(array[j++], p_tbl_info->field[i].name, sal_strlen(p_tbl_info->field[i].name)))
        {
             return PM_E_INVALID_PARAM;
        }
        PM_E_RETURN(tbl_mrt_field_value_parser( array[j++], i, p_mrt));
    }

    return PM_E_NONE;
}

int32
tbl_mrt_dump_one(tbl_mrt_t *p_mrt, tbl_iter_args_t *pargs)
{
    cdb_node_t *p_node = cdb_get_tbl(TBL_MRT);
    field_parse_t *p_field = pargs->argv[0];
    FILE *fp = pargs->argv[1];
    cdb_tbl_info_t *p_tbl_info = p_node->tbl_info;

    char buf[MAX_CMD_STR_LEN];

    if (FLD_MATCH(TBL_MRT_FLD_KEY, p_field->field_id[0]))
    {
        sal_fprintf(fp, "/%s"CMD_EQUAL_STR"%s", p_tbl_info->field[TBL_MRT_FLD_KEY].name, 
            cdb_mrt_val2str(buf, MAX_CMD_STR_LEN, &p_mrt->key));
    }
    if (FLD_MATCH(TBL_MRT_FLD_RPF_IFINDEX, p_field->field_id[0]))
    {
        sal_fprintf(fp, "/%s"CMD_EQUAL_STR"%u", p_tbl_info->field[TBL_MRT_FLD_RPF_IFINDEX].name,
            p_mrt->rpf_ifindex);
    }
    if (FLD_MATCH(TBL_MRT_FLD_OIF_INDEX_BMP, p_field->field_id[0]))
    {
        sal_fprintf(fp, "/%s"CMD_EQUAL_STR"%s", p_tbl_info->field[TBL_MRT_FLD_OIF_INDEX_BMP].name, 
            cdb_bitmap_val2str(buf, MAX_CMD_STR_LEN, p_mrt->oif_index_bmp, sizeof(p_mrt->oif_index_bmp)/4));
    }
    if (FLD_MATCH(TBL_MRT_FLD_NHG_OID, p_field->field_id[0]))
    {
        sal_fprintf(fp, "/%s"CMD_EQUAL_STR"%"PRIu64, p_tbl_info->field[TBL_MRT_FLD_NHG_OID].name,
            p_mrt->nhg_oid);
    }
    if (FLD_MATCH(TBL_MRT_FLD_PACKET_MATCH, p_field->field_id[0]))
    {
        sal_fprintf(fp, "/%s"CMD_EQUAL_STR"%"PRIu64, p_tbl_info->field[TBL_MRT_FLD_PACKET_MATCH].name,
            p_mrt->packet_match);
    }
    if (FLD_MATCH(TBL_MRT_FLD_BYTE_MATCH, p_field->field_id[0]))
    {
        sal_fprintf(fp, "/%s"CMD_EQUAL_STR"%"PRIu64, p_tbl_info->field[TBL_MRT_FLD_BYTE_MATCH].name,
            p_mrt->byte_match);
    }
    if (FLD_MATCH(TBL_MRT_FLD_STATIC_MROUTE, p_field->field_id[0]))
    {
        sal_fprintf(fp, "/%s"CMD_EQUAL_STR"%u", p_tbl_info->field[TBL_MRT_FLD_STATIC_MROUTE].name,
            p_mrt->static_mroute);
    }

    sal_fprintf(fp, "\n");

    return PM_E_NONE;
}

int32
tbl_mrt_iterate(TBL_ITER_CB_FUNC fn, tbl_iter_args_t *pargs)
{
    tbl_mrt_master_t *p_master = _g_p_tbl_mrt_master;
    if (NULL == p_master)
    {
        return PM_E_NONE;
    }

    ctclib_hash_iterate2(p_master->mrt_hash, tbl_hash_iter_adpt_fn, fn, pargs);
    return PM_E_NONE;
}

tbl_mrt_master_t*
tbl_mrt_get_master()
{
    return _g_p_tbl_mrt_master;
}

tbl_mrt_master_t*
tbl_mrt_init_mrt()
{
    _g_p_tbl_mrt_master = XCALLOC(MEM_TBL_MASTER, sizeof(tbl_mrt_master_t));
    _g_p_tbl_mrt_master->mrt_hash = ctclib_hash_create(_tbl_mrt_hash_make, _tbl_mrt_hash_cmp);
    return _g_p_tbl_mrt_master;
}

