
#ifndef __TBL_QOS_QUEUE_SHAPE_PROFILE_H__
#define __TBL_QOS_QUEUE_SHAPE_PROFILE_H__

int32
tbl_qos_queue_shape_profile_add_qos_queue_shape_profile_sync(tbl_qos_queue_shape_profile_t *p_qos_queue_shape_profile, uint32 sync);

int32
tbl_qos_queue_shape_profile_del_qos_queue_shape_profile_sync(tbl_qos_queue_shape_profile_key_t *p_qos_queue_shape_profile_key, uint32 sync);

int32
tbl_qos_queue_shape_profile_set_qos_queue_shape_profile_field_sync(tbl_qos_queue_shape_profile_t *p_qos_queue_shape_profile, tbl_qos_queue_shape_profile_field_id_t field_id, uint32 sync);

int32
tbl_qos_queue_shape_profile_add_qos_queue_shape_profile(tbl_qos_queue_shape_profile_t *p_qos_queue_shape_profile);

int32
tbl_qos_queue_shape_profile_del_qos_queue_shape_profile(tbl_qos_queue_shape_profile_key_t *p_qos_queue_shape_profile_key);

int32
tbl_qos_queue_shape_profile_set_qos_queue_shape_profile_field(tbl_qos_queue_shape_profile_t *p_qos_queue_shape_profile, tbl_qos_queue_shape_profile_field_id_t field_id);

tbl_qos_queue_shape_profile_t*
tbl_qos_queue_shape_profile_get_qos_queue_shape_profile(tbl_qos_queue_shape_profile_key_t *p_qos_queue_shape_profile_key);

char*
tbl_qos_queue_shape_profile_key_val2str(tbl_qos_queue_shape_profile_t *p_qos_queue_shape_profile, char *str, uint32 str_len);

int32
tbl_qos_queue_shape_profile_key_str2val(char *str, tbl_qos_queue_shape_profile_t *p_qos_queue_shape_profile);

int32
tbl_qos_queue_shape_profile_dump_one(tbl_qos_queue_shape_profile_t *p_qos_queue_shape_profile, tbl_iter_args_t *pargs);

char*
tbl_qos_queue_shape_profile_key_name_dump(tbl_qos_queue_shape_profile_t *p_qos_queue_shape_profile, char* str);

char*
tbl_qos_queue_shape_profile_key_value_dump(tbl_qos_queue_shape_profile_t *p_qos_queue_shape_profile, char* str);

char*
tbl_qos_queue_shape_profile_field_name_dump(tbl_qos_queue_shape_profile_t *p_qos_queue_shape_profile, int32 field_id, char* str);

char*
tbl_qos_queue_shape_profile_field_value_dump(tbl_qos_queue_shape_profile_t *p_qos_queue_shape_profile, int32 field_id, char* str);

char**
tbl_qos_queue_shape_profile_table_dump(tbl_qos_queue_shape_profile_t *p_qos_queue_shape_profile, char** str);

int32
tbl_qos_queue_shape_profile_table_parser(char** array, char* key_value, tbl_qos_queue_shape_profile_t *p_qos_queue_shape_profile);

int32
tbl_qos_queue_shape_profile_iterate(TBL_ITER_CB_FUNC fn, tbl_iter_args_t *pargs);

tbl_qos_queue_shape_profile_master_t*
tbl_qos_queue_shape_profile_get_master();

tbl_qos_queue_shape_profile_master_t*
tbl_qos_queue_shape_profile_init_qos_queue_shape_profile();

tbl_qos_queue_shape_profile_t*
tbl_qos_queue_shape_profile_get_profile_by_name(char *name);

int32
tbl_qos_queue_shape_profile_get_field(char *name, tbl_qos_queue_shape_profile_field_id_t field_id, void* field_value);

int32
tbl_qos_queue_shape_profile_alloc_profile_id(uint32 *p_id);

#endif /* !__TBL_QOS_QUEUE_SHAPE_PROFILE_H__ */

