
#ifndef __TBL_PTP_RESIDENCE_TIME_CACHE_H__
#define __TBL_PTP_RESIDENCE_TIME_CACHE_H__

int32
tbl_ptp_residence_time_cache_add_ptp_residence_time_cache_sync(tbl_ptp_residence_time_cache_t *p_residence_time_cache, uint32 sync);

int32
tbl_ptp_residence_time_cache_del_ptp_residence_time_cache_sync(tbl_ptp_residence_time_cache_key_t *p_residence_time_cache_key, uint32 sync);

int32
tbl_ptp_residence_time_cache_set_ptp_residence_time_cache_field_sync(tbl_ptp_residence_time_cache_t *p_residence_time_cache, tbl_ptp_residence_time_cache_field_id_t field_id, uint32 sync);

int32
tbl_ptp_residence_time_cache_add_ptp_residence_time_cache(tbl_ptp_residence_time_cache_t *p_residence_time_cache);

int32
tbl_ptp_residence_time_cache_del_ptp_residence_time_cache(tbl_ptp_residence_time_cache_key_t *p_residence_time_cache_key);

int32
tbl_ptp_residence_time_cache_set_ptp_residence_time_cache_field(tbl_ptp_residence_time_cache_t *p_residence_time_cache, tbl_ptp_residence_time_cache_field_id_t field_id);

tbl_ptp_residence_time_cache_t*
tbl_ptp_residence_time_cache_get_ptp_residence_time_cache(tbl_ptp_residence_time_cache_key_t *p_residence_time_cache_key);

char*
tbl_ptp_residence_time_cache_key_val2str(tbl_ptp_residence_time_cache_t *p_residence_time_cache, char *str, uint32 str_len);

int32
tbl_ptp_residence_time_cache_key_str2val(char *str, tbl_ptp_residence_time_cache_t *p_residence_time_cache);

int32
tbl_ptp_residence_time_cache_dump_one(tbl_ptp_residence_time_cache_t *p_residence_time_cache, tbl_iter_args_t *pargs);

char*
tbl_ptp_residence_time_cache_key_name_dump(tbl_ptp_residence_time_cache_t *p_residence_time_cache, char* str);

char*
tbl_ptp_residence_time_cache_key_value_dump(tbl_ptp_residence_time_cache_t *p_residence_time_cache, char* str);

char*
tbl_ptp_residence_time_cache_field_name_dump(tbl_ptp_residence_time_cache_t *p_residence_time_cache, int32 field_id, char* str);

char*
tbl_ptp_residence_time_cache_field_value_dump(tbl_ptp_residence_time_cache_t *p_residence_time_cache, int32 field_id, char* str);

char**
tbl_ptp_residence_time_cache_table_dump(tbl_ptp_residence_time_cache_t *p_residence_time_cache, char** str);

int32
tbl_ptp_residence_time_cache_table_parser(char** array, char* key_value, tbl_ptp_residence_time_cache_t *p_residence_time_cache);

int32
tbl_ptp_residence_time_cache_iterate(TBL_ITER_CB_FUNC fn, tbl_iter_args_t *pargs);

tbl_ptp_residence_time_cache_master_t*
tbl_ptp_residence_time_cache_get_master();

tbl_ptp_residence_time_cache_master_t*
tbl_ptp_residence_time_cache_init_ptp_residence_time_cache();

#endif /* !__TBL_PTP_RESIDENCE_TIME_CACHE_H__ */

