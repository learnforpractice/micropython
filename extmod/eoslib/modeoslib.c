/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2014-2016 Damien P. George
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */


#include "mpeoslib.h"
#include <stdlib.h>

#if MICROPY_PY_EOSLIB

// The function below implements a simple non-recursive JSON parser.
//
// The JSON specification is at http://www.ietf.org/rfc/rfc4627.txt
// The parser here will parse any valid JSON and return the correct
// corresponding Python object.  It allows through a superset of JSON, since
// it treats commas and colons as "whitespace", and doesn't care if
// brackets/braces are correctly paired.  It will raise a ValueError if the
// input is outside it's specs.
//
// Most of the work is parsing the primitives (null, false, true, numbers,
// strings).  It does 1 pass over the input stream.  It tries to be fast and
// small in code size, while not using more RAM than necessary.

typedef struct _eoslib_stream_t {
    mp_obj_t stream_obj;
    mp_uint_t (*read)(mp_obj_t obj, void *buf, mp_uint_t size, int *errcode);
    int errcode;
    byte cur;
} eoslib_stream_t;

#define S_EOF (0) // null is not allowed in json stream so is ok as EOF marker
#define S_END(s) ((s).cur == S_EOF)
#define S_CUR(s) ((s).cur)
#define S_NEXT(s) (eoslib_stream_next(&(s)))

const int KEY_TYPE_KEY = 0;
const int KEY_TYPE_KEY128x128 = 1;
const int KEY_TYPE_KEY64x64x64 = 2;
const int KEY_TYPE_STR = 3;




#define DB_METHOD_SEQ(SUFFIX) \
   int store_##SUFFIX(uint64_t scope, uint64_t table, char* keys, size_t keys_len, char* data, size_t data_len); \
   int update_##SUFFIX(uint64_t scope, uint64_t table, char* keys, size_t keys_len, char* data, size_t data_len); \
   int remove_##SUFFIX(uint64_t scope, uint64_t table, char* keys);

#define DB_INDEX_METHOD_SEQ(SUFFIX)\
   int load_##SUFFIX(int64_t scope, int64_t code, int64_t table, char* keys, size_t keys_len, char* data, size_t data_len); \
	int front_##SUFFIX(int64_t scope, int64_t code, int64_t table, char* keys, size_t keys_len, char* data, size_t data_len); \
	int previous_##SUFFIX(int64_t scope, int64_t code, int64_t table, char* keys, size_t keys_len, char* data, size_t data_len); \
   int back_##SUFFIX(int64_t scope, int64_t code, int64_t table, char* keys, size_t keys_len, char* data, size_t data_len); \
	int next_##SUFFIX(int64_t scope, int64_t code, int64_t table, char* keys, size_t keys_len, char* data, size_t data_len); \
	int lower_bound_##SUFFIX(int64_t scope, int64_t code, int64_t table, char* keys, size_t keys_len, char* data, size_t data_len); \
   int upper_bound_##SUFFIX(int64_t scope, int64_t code, int64_t table,char* keys, size_t keys_len,  char* data, size_t data_len);

DB_METHOD_SEQ(i64)
DB_METHOD_SEQ(i128i128)
DB_METHOD_SEQ(i64i64)
DB_METHOD_SEQ(i64i64i64)



DB_INDEX_METHOD_SEQ(i64)

DB_INDEX_METHOD_SEQ(primary_i128i128)
DB_INDEX_METHOD_SEQ(secondary_i128i128)

DB_INDEX_METHOD_SEQ(primary_i64i64)
DB_INDEX_METHOD_SEQ(secondary_i64i64)

DB_INDEX_METHOD_SEQ(primary_i64i64i64)
DB_INDEX_METHOD_SEQ(secondary_i64i64i64)
DB_INDEX_METHOD_SEQ(tertiary_i64i64i64)




STATIC mp_obj_t mod_eoslib_now(void) {
   return mp_obj_new_int(now());
}
STATIC MP_DEFINE_CONST_FUN_OBJ_0(mod_eoslib_now_obj, mod_eoslib_now);

STATIC mp_obj_t mod_eoslib_s2n(mp_obj_t obj) {
   size_t len;
   const char *account = mp_obj_str_get_data(obj, &len);
   uint64_t n = string_to_uint64_(account);
   return mp_obj_new_int_from_ull(n);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(mod_eoslib_s2n_obj, mod_eoslib_s2n);

STATIC mp_obj_t mod_eoslib_N(mp_obj_t obj) {
   size_t len;
   const char *account = mp_obj_str_get_data(obj, &len);
   uint64_t n = string_to_uint64_(account);
   return mp_obj_new_int_from_ull(n);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(mod_eoslib_N_obj, mod_eoslib_N);

mp_uint_t mp_obj_uint_get_checked(mp_const_obj_t self_in);

STATIC mp_obj_t mod_eoslib_n2s(mp_obj_t obj) {
   uint64_t n = mp_obj_uint_get_checked(obj);
   return uint64_to_string_(n);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(mod_eoslib_n2s_obj, mod_eoslib_n2s);

STATIC mp_obj_t mod_eoslib_pack(mp_obj_t obj) {
   size_t len = 0;
   const char *s = mp_obj_str_get_data(obj, &len);
   return pack_(s, len);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(mod_eoslib_pack_obj, mod_eoslib_pack);

STATIC mp_obj_t mod_eoslib_unpack(mp_obj_t obj) {
   size_t len = 0;
   const char *s = mp_obj_str_get_data(obj, &len);
   return unpack_(s, len);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(mod_eoslib_unpack_obj, mod_eoslib_unpack);


#define i64_Size (8)
#define i128i128_Size (32)
#define i64i64_Size 16
#define i64i64i64_Size 24

#define DEFINE_WRITE_FUNCTION(NAME, SUFFIX) \
STATIC mp_obj_t mod_eoslib_##NAME##_##SUFFIX(size_t n_args, const mp_obj_t *args) { \
   size_t keys_len = 0; \
   size_t value_len = 0; \
   uint64_t scope = mp_obj_uint_get_checked(args[0]); \
   uint64_t table = mp_obj_uint_get_checked(args[1]); \
   void* keys = (void *)mp_obj_str_get_data(args[2], &keys_len); \
   if (SUFFIX##_Size != keys_len) { \
		return mp_obj_new_int(0); \
	} \
   void* value = (void *)mp_obj_str_get_data(args[3], &value_len); \
   int ret = NAME##_##SUFFIX(scope, table, keys, keys_len, value, value_len); \
   return mp_obj_new_int(ret); \
} \
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR(mod_eoslib_##NAME##_##SUFFIX##_obj, 4, mod_eoslib_##NAME##_##SUFFIX);

#define DEFINE_REMOVE_FUNCTION(SUFFIX) \
STATIC mp_obj_t mod_eoslib_remove_##SUFFIX(size_t n_args, const mp_obj_t *args) { \
   size_t keys_len = 0; \
   uint64_t scope = mp_obj_uint_get_checked(args[0]); \
   uint64_t table = mp_obj_uint_get_checked(args[1]); \
   void* keys = (void *)mp_obj_str_get_data(args[2], &keys_len); \
   if (SUFFIX##_Size != keys_len) { \
		return mp_obj_new_int(0); \
	} \
   int ret = remove_##SUFFIX(scope, table, keys); \
   return mp_obj_new_int(ret); \
} \
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR(mod_eoslib_remove_##SUFFIX##_obj, 3, mod_eoslib_remove_##SUFFIX);


#define DEFINE_READ_FUNCTION(name, SUFFIX) \
   STATIC mp_obj_t mod_eoslib_##name##_##SUFFIX(size_t n_args, const mp_obj_t *args) { \
      size_t keys_len = 0; \
      size_t value_len = 0; \
      uint64_t scope = mp_obj_uint_get_checked(args[0]); \
      uint64_t code = mp_obj_uint_get_checked(args[1]); \
      uint64_t table = mp_obj_uint_get_checked(args[2]); \
      void* keys = (void *)mp_obj_str_get_data(args[3], &keys_len); \
      if (SUFFIX##_Size != keys_len) { \
   		return mp_obj_new_int(0); \
      } \
      void* value = (void *)mp_obj_str_get_data(args[4], &value_len); \
      int ret = name##_##SUFFIX(scope, code, table, keys, keys_len, value, value_len); \
      return mp_obj_new_int(ret); \
   } \
   STATIC MP_DEFINE_CONST_FUN_OBJ_VAR(mod_eoslib_##name##_##SUFFIX##_obj, 5, mod_eoslib_##name##_##SUFFIX);

#define MPY_DB_METHOD_SEQ(SUFFIX) \
	DEFINE_WRITE_FUNCTION(store, SUFFIX) \
	DEFINE_WRITE_FUNCTION(update, SUFFIX) \
	DEFINE_REMOVE_FUNCTION(SUFFIX)

#define MPY_DB_INDEX_METHOD_SEQ(SUFFIX)\
	DEFINE_READ_FUNCTION(load, SUFFIX) \
	DEFINE_READ_FUNCTION(front, SUFFIX) \
	DEFINE_READ_FUNCTION(previous, SUFFIX) \
	DEFINE_READ_FUNCTION(back, SUFFIX) \
	DEFINE_READ_FUNCTION(next, SUFFIX) \
	DEFINE_READ_FUNCTION(lower_bound, SUFFIX) \
	DEFINE_READ_FUNCTION(upper_bound, SUFFIX)

MPY_DB_METHOD_SEQ(i64)
MPY_DB_INDEX_METHOD_SEQ(i64)


#define DEFINE_WRITE_STR_FUNCTION(name) \
STATIC mp_obj_t mod_eoslib_##name##_str(size_t n_args, const mp_obj_t *args) { \
   size_t keys_len = 0; \
   size_t value_len = 0; \
   uint64_t scope = mp_obj_uint_get_checked(args[0]); \
   uint64_t table = mp_obj_uint_get_checked(args[1]); \
   void* keys = (void *)mp_obj_str_get_data(args[2], &keys_len); \
   void* value = (void *)mp_obj_str_get_data(args[3], &value_len); \
   int ret = name##_str_(scope, table, keys, keys_len, value, value_len); \
   return mp_obj_new_int(ret); \
} \
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR(mod_eoslib_##name##_str##_obj, 4, mod_eoslib_##name##_str);

#define DEFINE_READ_STR_FUNCTION(name) \
   STATIC mp_obj_t mod_eoslib_##name##_str(size_t n_args, const mp_obj_t *args) { \
      char value[256]; \
      size_t keys_len = 0; \
      uint64_t scope = mp_obj_uint_get_checked(args[0]); \
      uint64_t code = mp_obj_uint_get_checked(args[1]); \
      uint64_t table = mp_obj_uint_get_checked(args[2]); \
      void* keys = (void *)mp_obj_str_get_data(args[3], &keys_len); \
      int ret = name##_str_(scope, code, table, keys, keys_len,value, sizeof(value)); \
      if (ret > 0) { \
         return mp_obj_new_str(value, ret); \
      } else { \
         return mp_const_none; \
      } \
   } \
   STATIC MP_DEFINE_CONST_FUN_OBJ_VAR(mod_eoslib_##name##_str##_obj, 4, mod_eoslib_##name##_str);

DEFINE_WRITE_STR_FUNCTION(store)
DEFINE_WRITE_STR_FUNCTION(update)

STATIC mp_obj_t mod_eoslib_remove_str(size_t n_args, const mp_obj_t *args) {
   size_t keys_len = 0;
   uint64_t scope = mp_obj_uint_get_checked(args[0]);
   uint64_t table = mp_obj_uint_get_checked(args[1]);
   void* keys = (void *)mp_obj_str_get_data(args[2], &keys_len);
   int ret = remove_str_(scope, table, keys, keys_len);
   return mp_obj_new_int(ret);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR(mod_eoslib_remove_str_obj, 3, mod_eoslib_remove_str);

DEFINE_READ_STR_FUNCTION(load)
DEFINE_READ_STR_FUNCTION(front)
DEFINE_READ_STR_FUNCTION(back)
DEFINE_READ_STR_FUNCTION(next)
DEFINE_READ_STR_FUNCTION(previous)
DEFINE_READ_STR_FUNCTION(lower_bound)
DEFINE_READ_STR_FUNCTION(upper_bound)

STATIC mp_obj_t mod_eoslib_read_action(void) {
	size_t size = action_size();
	if (size == 0) {
		return mp_const_none;
	}
	char *data = malloc(size);
	read_action(data, size);
	mp_obj_t obj = mp_obj_new_str(data, size);
	free(data);
   return obj;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_0(mod_eoslib_read_action_obj, mod_eoslib_read_action);


int read_action(char* memory, size_t size);

#define METHOD0(NAME) \
STATIC mp_obj_t mod_eoslib_##NAME(void) { \
   return mp_obj_new_int(NAME()); \
} \
STATIC MP_DEFINE_CONST_FUN_OBJ_0(mod_eoslib_##NAME##_obj, mod_eoslib_##NAME);

METHOD0(action_size)
METHOD0(current_receiver)
METHOD0(publication_time)
METHOD0(current_sender)


#define METHOD1(NAME) \
STATIC mp_obj_t mod_eoslib_##NAME(mp_obj_t obj1) { \
   uint64_t arg1 = mp_obj_uint_get_checked(obj1); \
   NAME(arg1); \
   return mp_const_none; \
} \
STATIC MP_DEFINE_CONST_FUN_OBJ_1(mod_eoslib_##NAME##_obj, mod_eoslib_##NAME);


#define METHOD2(NAME) \
STATIC mp_obj_t mod_eoslib_##NAME(mp_obj_t obj1, mp_obj_t obj2) { \
   uint64_t arg1 = mp_obj_uint_get_checked(obj1); \
   uint64_t arg2 = mp_obj_uint_get_checked(obj2); \
   NAME(arg1,arg2); \
   return mp_const_none; \
} \
STATIC MP_DEFINE_CONST_FUN_OBJ_2(mod_eoslib_##NAME##_obj, mod_eoslib_##NAME);

METHOD1(require_auth)
METHOD2(require_auth_ex)
METHOD1(require_write_lock)
METHOD2(require_read_lock)
METHOD1(require_recipient)

STATIC mp_obj_t mod_eoslib_is_account(mp_obj_t obj1) {
   uint64_t arg1 = mp_obj_uint_get_checked(obj1);
   is_account(arg1);
   return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(mod_eoslib_is_account_obj, mod_eoslib_is_account);



#if 0
STATIC mp_obj_t mod_eoslib_send_inline(size_t n_args, const mp_obj_t *args) {
	return 0;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR(mod_eoslib_send_inline_obj, 3, mod_eoslib_send_inline);
#endif

/*
      account_name               account;
      action_name                name;
      vector<permission_level>   authorization;
      bytes                      data;
*/

STATIC const mp_rom_map_elem_t mp_module_eoslib_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_eoslib) },
	 { MP_ROM_QSTR(MP_QSTR_now), MP_ROM_PTR(&mod_eoslib_now_obj) },
	 { MP_ROM_QSTR(MP_QSTR_s2n), MP_ROM_PTR(&mod_eoslib_s2n_obj) },
    { MP_ROM_QSTR(MP_QSTR_N), MP_ROM_PTR(&mod_eoslib_N_obj) },
    { MP_ROM_QSTR(MP_QSTR_n2s), MP_ROM_PTR(&mod_eoslib_n2s_obj) },
    { MP_ROM_QSTR(MP_QSTR_pack), MP_ROM_PTR(&mod_eoslib_pack_obj) },
    { MP_ROM_QSTR(MP_QSTR_unpack), MP_ROM_PTR(&mod_eoslib_unpack_obj) },

	 { MP_ROM_QSTR(MP_QSTR_read_action), MP_ROM_PTR(&mod_eoslib_read_action_obj) },
	 { MP_ROM_QSTR(MP_QSTR_action_size), MP_ROM_PTR(&mod_eoslib_action_size_obj) },
	 { MP_ROM_QSTR(MP_QSTR_current_receiver), MP_ROM_PTR(&mod_eoslib_current_receiver_obj) },
	 { MP_ROM_QSTR(MP_QSTR_publication_time), MP_ROM_PTR(&mod_eoslib_publication_time_obj) },
	 { MP_ROM_QSTR(MP_QSTR_current_sender), MP_ROM_PTR(&mod_eoslib_current_sender_obj) },

	 { MP_ROM_QSTR(MP_QSTR_require_auth), MP_ROM_PTR(&mod_eoslib_require_auth_obj) },
	 { MP_ROM_QSTR(MP_QSTR_require_auth_ex), MP_ROM_PTR(&mod_eoslib_require_auth_ex_obj) },
	 { MP_ROM_QSTR(MP_QSTR_require_write_lock), MP_ROM_PTR(&mod_eoslib_require_write_lock_obj) },
	 { MP_ROM_QSTR(MP_QSTR_require_read_lock), MP_ROM_PTR(&mod_eoslib_require_read_lock_obj) },
	 { MP_ROM_QSTR(MP_QSTR_require_recipient), MP_ROM_PTR(&mod_eoslib_require_recipient_obj) },
	 { MP_ROM_QSTR(MP_QSTR_is_account), MP_ROM_PTR(&mod_eoslib_is_account_obj) },

	 { MP_ROM_QSTR(MP_QSTR_store_i64), MP_ROM_PTR(&mod_eoslib_store_i64_obj) },
	 { MP_ROM_QSTR(MP_QSTR_update_i64), MP_ROM_PTR(&mod_eoslib_update_i64_obj) },
	 { MP_ROM_QSTR(MP_QSTR_remove_i64), MP_ROM_PTR(&mod_eoslib_remove_i64_obj) },

	 { MP_ROM_QSTR(MP_QSTR_load_i64), MP_ROM_PTR(&mod_eoslib_load_i64_obj) },
	 { MP_ROM_QSTR(MP_QSTR_front_i64), MP_ROM_PTR(&mod_eoslib_front_i64_obj) },
	 { MP_ROM_QSTR(MP_QSTR_previous_i64), MP_ROM_PTR(&mod_eoslib_previous_i64_obj) },
	 { MP_ROM_QSTR(MP_QSTR_back_i64), MP_ROM_PTR(&mod_eoslib_back_i64_obj) },
	 { MP_ROM_QSTR(MP_QSTR_next_i64), MP_ROM_PTR(&mod_eoslib_next_i64_obj) },
	 { MP_ROM_QSTR(MP_QSTR_lower_bound_i64), MP_ROM_PTR(&mod_eoslib_lower_bound_i64_obj) },
	 { MP_ROM_QSTR(MP_QSTR_upper_bound_i64), MP_ROM_PTR(&mod_eoslib_upper_bound_i64_obj) },

    { MP_ROM_QSTR(MP_QSTR_store_str), MP_ROM_PTR(&mod_eoslib_store_str_obj) },
    { MP_ROM_QSTR(MP_QSTR_update_str), MP_ROM_PTR(&mod_eoslib_update_str_obj) },
    { MP_ROM_QSTR(MP_QSTR_remove_str), MP_ROM_PTR(&mod_eoslib_remove_str_obj) },
    { MP_ROM_QSTR(MP_QSTR_load_str), MP_ROM_PTR(&mod_eoslib_load_str_obj) },
    { MP_ROM_QSTR(MP_QSTR_front_str), MP_ROM_PTR(&mod_eoslib_front_str_obj) },
    { MP_ROM_QSTR(MP_QSTR_back_str), MP_ROM_PTR(&mod_eoslib_back_str_obj) },
    { MP_ROM_QSTR(MP_QSTR_next_str), MP_ROM_PTR(&mod_eoslib_next_str_obj) },
    { MP_ROM_QSTR(MP_QSTR_previous_str), MP_ROM_PTR(&mod_eoslib_previous_str_obj) },
    { MP_ROM_QSTR(MP_QSTR_lower_bound_str), MP_ROM_PTR(&mod_eoslib_lower_bound_str_obj) },
    { MP_ROM_QSTR(MP_QSTR_upper_bound_str), MP_ROM_PTR(&mod_eoslib_upper_bound_str_obj) },
};

STATIC MP_DEFINE_CONST_DICT(mp_module_eoslib_globals, mp_module_eoslib_globals_table);

const mp_obj_module_t mp_module_eoslib = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t*)&mp_module_eoslib_globals,
};

#endif //MICROPY_PY_EOSLIB
