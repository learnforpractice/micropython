#include <eosiolib_native/vm_api.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "xxhash.h"
#include "py/obj.h"
#include "py/objtuple.h"
#include "py/objstr.h"
#include "py/lexer.h"
#include "py/compile.h"


//db i64
STATIC mp_obj_t mod_db_store_i64(size_t n_args, const mp_obj_t *args) {
   size_t value_len = 0;
   uint64_t scope = mp_obj_get_uint(args[0]);
   uint64_t table = mp_obj_get_uint(args[1]);
   uint64_t payer = mp_obj_get_uint(args[2]);
   uint64_t id = mp_obj_get_uint(args[3]);
   void* value = (void *)mp_obj_str_get_data(args[4], &value_len);
   int itr = get_vm_api()->db_store_i64(scope, table, payer, id, value, value_len);
   return mp_obj_new_int(itr);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR(mod_db_store_i64_obj, 5, mod_db_store_i64);

STATIC mp_obj_t mod_db_update_i64(size_t n_args, const mp_obj_t *args) {
   size_t value_len = 0;
   uint64_t itr = mp_obj_get_uint(args[0]);
   uint64_t payer = mp_obj_get_uint(args[1]);
   void* value = (void *)mp_obj_str_get_data(args[2], &value_len);
   get_vm_api()->db_update_i64(itr, payer, value, value_len);
   return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR(mod_db_update_i64_obj, 3, mod_db_update_i64);

STATIC mp_obj_t mod_db_remove_i64(size_t n_args, const mp_obj_t *args) {
   uint64_t itr = mp_obj_get_uint(args[0]);
   get_vm_api()->db_remove_i64(itr);
   return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR(mod_db_remove_i64_obj, 1, mod_db_remove_i64);


STATIC mp_obj_t mod_db_get_i64(size_t n_args, const mp_obj_t *args) {
   char* buffer;
   mp_obj_t ret;
   uint64_t itr = mp_obj_get_uint(args[0]);
   int size = get_vm_api()->db_get_i64(itr, (char*)0, 0);
   if (size <= 0) {
      return mp_const_none;
   }

   buffer = (char*)malloc(size);
   get_vm_api()->db_get_i64(itr, buffer, size);
   ret = mp_obj_new_bytes((byte*)buffer, size);
   free(buffer);
   return ret;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR(mod_db_get_i64_obj, 1, mod_db_get_i64);

STATIC mp_obj_t mod_db_get_i64_ex(size_t n_args, const mp_obj_t *args) {
   char* buffer;
   uint64_t primary = 0;
   uint64_t itr = mp_obj_get_uint(args[0]);
   int size = get_vm_api()->db_get_i64_ex(itr, &primary, (char*)0, 0);
   if (size <= 0) {
      return mp_const_none;
   }

   buffer = (char*)malloc(size);
   get_vm_api()->db_get_i64_ex(itr, &primary, buffer, size);

   mp_obj_tuple_t *tuple = mp_obj_new_tuple(2, NULL);
   tuple->items[0] = mp_obj_new_int(primary);
   tuple->items[1] = mp_obj_new_bytes((byte*)buffer, size);
   free(buffer);

   return tuple;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR(mod_db_get_i64_ex_obj, 1, mod_db_get_i64_ex);


STATIC mp_obj_t mod_db_next_i64(size_t n_args, const mp_obj_t *args) {
   uint64_t primary = 0;
   uint64_t itr = mp_obj_get_uint(args[0]);
   int itr_next = get_vm_api()->db_next_i64(itr, &primary);

   mp_obj_tuple_t *tuple = mp_obj_new_tuple(2, NULL);
   tuple->items[0] = mp_obj_new_int(itr_next);
   tuple->items[1] = mp_obj_new_int(primary);
   return tuple;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR(mod_db_next_i64_obj, 1, mod_db_next_i64);

STATIC mp_obj_t mod_db_previous_i64(size_t n_args, const mp_obj_t *args) {
   uint64_t primary = 0;
   uint64_t itr = mp_obj_get_uint(args[0]);
   int itr_pre = get_vm_api()->db_previous_i64(itr, &primary);

   mp_obj_tuple_t *tuple = mp_obj_new_tuple(2, NULL);
   tuple->items[0] = mp_obj_new_int(itr_pre);
   tuple->items[1] = mp_obj_new_int(primary);
   return tuple;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR(mod_db_previous_i64_obj, 1, mod_db_previous_i64);


STATIC mp_obj_t mod_db_find_i64(size_t n_args, const mp_obj_t *args) {
   uint64_t code = mp_obj_get_uint(args[0]);
   uint64_t scope = mp_obj_get_uint(args[1]);
   uint64_t table = mp_obj_get_uint(args[2]);
   uint64_t id = mp_obj_get_uint(args[3]);

   int itr = get_vm_api()->db_find_i64(code, scope, table, id);
   return mp_obj_new_int(itr);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR(mod_db_find_i64_obj, 4, mod_db_find_i64);

STATIC mp_obj_t mod_db_lowerbound_i64(size_t n_args, const mp_obj_t *args) {
   uint64_t code = mp_obj_get_uint(args[0]);
   uint64_t scope = mp_obj_get_uint(args[1]);
   uint64_t table = mp_obj_get_uint(args[2]);
   uint64_t id = mp_obj_get_uint(args[3]);
   int itr = get_vm_api()->db_lowerbound_i64(code, scope, table, id);

   return mp_obj_new_int(itr);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR(mod_db_lowerbound_i64_obj, 4, mod_db_lowerbound_i64);

STATIC mp_obj_t mod_db_upperbound_i64(size_t n_args, const mp_obj_t *args) {
   uint64_t code = mp_obj_get_uint(args[0]);
   uint64_t scope = mp_obj_get_uint(args[1]);
   uint64_t table = mp_obj_get_uint(args[2]);
   uint64_t id = mp_obj_get_uint(args[3]);

   int itr = get_vm_api()->db_upperbound_i64(code, scope, table, id);
   return mp_obj_new_int(itr);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR(mod_db_upperbound_i64_obj, 4, mod_db_upperbound_i64);

STATIC mp_obj_t mod_db_end_i64(size_t n_args, const mp_obj_t *args) {
   uint64_t code = mp_obj_get_uint(args[0]);
   uint64_t scope = mp_obj_get_uint(args[1]);
   uint64_t table = mp_obj_get_uint(args[2]);

   int itr = get_vm_api()->db_end_i64(code, scope, table);
   return mp_obj_new_int(itr);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR(mod_db_end_i64_obj, 3, mod_db_end_i64);


STATIC const mp_rom_map_elem_t mp_module_db_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_db) },

	 { MP_ROM_QSTR(MP_QSTR_store_i64), MP_ROM_PTR(&mod_db_store_i64_obj) },
	 { MP_ROM_QSTR(MP_QSTR_update_i64), MP_ROM_PTR(&mod_db_update_i64_obj) },
	 { MP_ROM_QSTR(MP_QSTR_remove_i64), MP_ROM_PTR(&mod_db_remove_i64_obj) },
	 { MP_ROM_QSTR(MP_QSTR_get_i64), MP_ROM_PTR(&mod_db_get_i64_obj) },
    { MP_ROM_QSTR(MP_QSTR_get_i64_ex), MP_ROM_PTR(&mod_db_get_i64_ex_obj) },
	 { MP_ROM_QSTR(MP_QSTR_previous_i64), MP_ROM_PTR(&mod_db_previous_i64_obj) },
	 { MP_ROM_QSTR(MP_QSTR_next_i64), MP_ROM_PTR(&mod_db_next_i64_obj) },
	 { MP_ROM_QSTR(MP_QSTR_find_i64), MP_ROM_PTR(&mod_db_find_i64_obj) },
	 { MP_ROM_QSTR(MP_QSTR_lowerbound_i64), MP_ROM_PTR(&mod_db_lowerbound_i64_obj) },
	 { MP_ROM_QSTR(MP_QSTR_upperbound_i64), MP_ROM_PTR(&mod_db_upperbound_i64_obj) },
	 { MP_ROM_QSTR(MP_QSTR_end_i64), MP_ROM_PTR(&mod_db_end_i64_obj) },

};

STATIC MP_DEFINE_CONST_DICT(mp_module_db_globals, mp_module_db_globals_table);

const mp_obj_module_t mp_module_db = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t*)&mp_module_db_globals,
};

