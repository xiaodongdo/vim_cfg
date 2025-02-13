
#ifndef __TBL_IPSG_FIB_H__
#define __TBL_IPSG_FIB_H__

int32
tbl_ipsg_fib_add_ipsg_fib_sync(tbl_ipsg_fib_t *p_ipsg_fib, uint32 sync);

int32
tbl_ipsg_fib_del_ipsg_fib_sync(tbl_ipsg_fib_key_t *p_ipsg_fib_key, uint32 sync);

int32
tbl_ipsg_fib_set_ipsg_fib_field_sync(tbl_ipsg_fib_t *p_ipsg_fib, tbl_ipsg_fib_field_id_t field_id, uint32 sync);

int32
tbl_ipsg_fib_add_ipsg_fib(tbl_ipsg_fib_t *p_ipsg_fib);

int32
tbl_ipsg_fib_del_ipsg_fib(tbl_ipsg_fib_key_t *p_ipsg_fib_key);

int32
tbl_ipsg_fib_set_ipsg_fib_field(tbl_ipsg_fib_t *p_ipsg_fib, tbl_ipsg_fib_field_id_t field_id);

tbl_ipsg_fib_t*
tbl_ipsg_fib_get_ipsg_fib(tbl_ipsg_fib_key_t *p_ipsg_fib_key);

char*
tbl_ipsg_fib_key_val2str(tbl_ipsg_fib_t *p_ipsg_fib, char *str, uint32 str_len);

int32
tbl_ipsg_fib_key_str2val(char *str, tbl_ipsg_fib_t *p_ipsg_fib);

int32
tbl_ipsg_fib_dump_one(tbl_ipsg_fib_t *p_ipsg_fib, tbl_iter_args_t *pargs);

char*
tbl_ipsg_fib_key_name_dump(tbl_ipsg_fib_t *p_ipsg_fib, char* str);

char*
tbl_ipsg_fib_key_value_dump(tbl_ipsg_fib_t *p_ipsg_fib, char* str);

char*
tbl_ipsg_fib_field_name_dump(tbl_ipsg_fib_t *p_ipsg_fib, int32 field_id, char* str);

char*
tbl_ipsg_fib_field_value_dump(tbl_ipsg_fib_t *p_ipsg_fib, int32 field_id, char* str);

char**
tbl_ipsg_fib_table_dump(tbl_ipsg_fib_t *p_ipsg_fib, char** str);

int32
tbl_ipsg_fib_table_parser(char** array, char* key_value, tbl_ipsg_fib_t *p_ipsg_fib);

int32
tbl_ipsg_fib_iterate(TBL_ITER_CB_FUNC fn, tbl_iter_args_t *pargs);

tbl_ipsg_fib_master_t*
tbl_ipsg_fib_get_master();

tbl_ipsg_fib_master_t*
tbl_ipsg_fib_init_ipsg_fib();


tbl_ipsg_fib_t*
tbl_ipsg_fib_get_ipsg_fib_by_ifname(char *ifname);

#endif /* !__TBL_IPSG_FIB_H__ */

