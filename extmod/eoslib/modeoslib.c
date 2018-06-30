#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include <eosiolib_native/vm_api.h>

#include "xxhash.h"
#include "py/runtime.h"
#include "py/obj.h"
#include "py/objtuple.h"
#include "py/objstr.h"
#include "py/lexer.h"
#include "py/compile.h"

mp_uint_t mp_obj_get_uint(mp_const_obj_t self_in);

STATIC mp_obj_t mod_eoslib_now(void) {
   return mp_obj_new_int(get_vm_api()->now());
}
STATIC MP_DEFINE_CONST_FUN_OBJ_0(mod_eoslib_now_obj, mod_eoslib_now);

STATIC mp_obj_t mod_eoslib_eosio_assert(mp_obj_t obj1, mp_obj_t obj2) {
   size_t len;
   int condition = mp_obj_get_int(obj1);
   const char* str = mp_obj_str_get_data(obj2, &len);
   get_vm_api()->eosio_assert(condition, str);
   return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(mod_eoslib_eosio_assert_obj, mod_eoslib_eosio_assert);


STATIC mp_obj_t mod_eoslib_read_action(void) {
   size_t size = get_vm_api()->action_data_size();
   if (size <= 0) {
      return mp_const_none;
   }
   byte *data = calloc(size, 1);
   get_vm_api()->read_action_data((char*)data, size);
   mp_obj_t obj = mp_obj_new_bytes(data, size);
   free(data);
   return obj;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_0(mod_eoslib_read_action_obj, mod_eoslib_read_action);


STATIC mp_obj_t mod_eoslib_action_size(void) {
   return mp_obj_new_int(get_vm_api()->action_data_size());
}
STATIC MP_DEFINE_CONST_FUN_OBJ_0(mod_eoslib_action_size_obj, mod_eoslib_action_size);

#define METHOD0(NAME) \
STATIC mp_obj_t mod_eoslib_##NAME(void) { \
   return mp_obj_new_int(get_vm_api()->NAME()); \
} \
STATIC MP_DEFINE_CONST_FUN_OBJ_0(mod_eoslib_##NAME##_obj, mod_eoslib_##NAME);


METHOD0(current_receiver)
METHOD0(publication_time)

#define METHOD1(NAME) \
STATIC mp_obj_t mod_eoslib_##NAME(mp_obj_t obj1) { \
   uint64_t arg1 = mp_obj_get_uint(obj1); \
   get_vm_api()->NAME(arg1); \
   return mp_const_none; \
} \
STATIC MP_DEFINE_CONST_FUN_OBJ_1(mod_eoslib_##NAME##_obj, mod_eoslib_##NAME);


#define METHOD2(NAME) \
STATIC mp_obj_t mod_eoslib_##NAME(mp_obj_t obj1, mp_obj_t obj2) { \
   uint64_t arg1 = mp_obj_get_uint(obj1); \
   uint64_t arg2 = mp_obj_get_uint(obj2); \
   get_vm_api()->NAME(arg1,arg2); \
   return mp_const_none; \
} \
STATIC MP_DEFINE_CONST_FUN_OBJ_2(mod_eoslib_##NAME##_obj, mod_eoslib_##NAME);

METHOD1(require_auth)
METHOD2(require_auth2)
METHOD1(require_recipient)

STATIC mp_obj_t mod_eoslib_is_account(mp_obj_t obj1) {
   uint64_t arg1 = mp_obj_get_uint(obj1);
   if (get_vm_api()->is_account(arg1)) {
         return mp_const_true;
   }
   return mp_const_false;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(mod_eoslib_is_account_obj, mod_eoslib_is_account);

STATIC mp_obj_t mod_eoslib_hash64(mp_obj_t obj1) {
   uint64_t key = 0;
   if (MP_OBJ_IS_STR_OR_BYTES(obj1)) {
      size_t len;
      const char* str = mp_obj_str_get_data(obj1, &len);
      key = XXH64(str, len, 0);
      return mp_obj_new_int_from_ll(key);
   } else if (MP_OBJ_IS_INT(obj1)) {
      return obj1;
   } else {
      mp_raise_TypeError("can't hash unsupported type");
      return mp_const_none;//mute return type check
   }
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(mod_eoslib_hash64_obj, mod_eoslib_hash64);

STATIC mp_obj_t mod_eoslib_N(mp_obj_t obj) {
   size_t len;
   const char *account = mp_obj_str_get_data(obj, &len);
   uint64_t n = get_vm_api()->string_to_uint64(account);
   return mp_obj_new_int_from_ll(n);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(mod_eoslib_N_obj, mod_eoslib_N);

STATIC mp_obj_t mod_eoslib_n2s(mp_obj_t obj) {
   char buf[32];
   uint64_t n = mp_obj_get_uint(obj);
   int size = get_vm_api()->uint64_to_string(n, buf, sizeof(buf));
   return mp_obj_new_str(buf, size);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(mod_eoslib_n2s_obj, mod_eoslib_n2s);

STATIC mp_obj_t mod_eoslib_s2n(mp_obj_t obj) {
   size_t len;
   const char *account = mp_obj_str_get_data(obj, &len);
   uint64_t n = get_vm_api()->string_to_uint64(account);
   return mp_obj_new_int_from_ll(n);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(mod_eoslib_s2n_obj, mod_eoslib_s2n);

STATIC mp_obj_t mod_eoslib_S(mp_obj_t obj1, mp_obj_t obj2) {
   uint64_t ret;
   uint8_t precision;
   const char* str;
   size_t len;

   precision = (uint8_t)mp_obj_get_uint(obj1);
   str = mp_obj_str_get_data(obj2, &len);

   ret = get_vm_api()->string_to_symbol(precision, str);
   return mp_obj_new_int_from_ll(ret);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(mod_eoslib_S_obj, mod_eoslib_S);


STATIC const mp_rom_map_elem_t mp_module_eoslib_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_eoslib) },
    { MP_ROM_QSTR(MP_QSTR_now), MP_ROM_PTR(&mod_eoslib_now_obj) },

    { MP_ROM_QSTR(MP_QSTR_eosio_assert), MP_ROM_PTR(&mod_eoslib_eosio_assert_obj) },

    { MP_ROM_QSTR(MP_QSTR_read_action), MP_ROM_PTR(&mod_eoslib_read_action_obj) },
    { MP_ROM_QSTR(MP_QSTR_action_size), MP_ROM_PTR(&mod_eoslib_action_size_obj) },
    { MP_ROM_QSTR(MP_QSTR_current_receiver), MP_ROM_PTR(&mod_eoslib_current_receiver_obj) },
    { MP_ROM_QSTR(MP_QSTR_publication_time), MP_ROM_PTR(&mod_eoslib_publication_time_obj) },

    { MP_ROM_QSTR(MP_QSTR_require_auth), MP_ROM_PTR(&mod_eoslib_require_auth_obj) },
    { MP_ROM_QSTR(MP_QSTR_require_auth2), MP_ROM_PTR(&mod_eoslib_require_auth2_obj) },
    { MP_ROM_QSTR(MP_QSTR_require_recipient), MP_ROM_PTR(&mod_eoslib_require_recipient_obj) },
    { MP_ROM_QSTR(MP_QSTR_is_account), MP_ROM_PTR(&mod_eoslib_is_account_obj) },

    { MP_ROM_QSTR(MP_QSTR_hash64), MP_ROM_PTR(&mod_eoslib_hash64_obj) },

    { MP_ROM_QSTR(MP_QSTR_N), MP_ROM_PTR(&mod_eoslib_N_obj) },
    { MP_ROM_QSTR(MP_QSTR_n2s), MP_ROM_PTR(&mod_eoslib_n2s_obj) },
    { MP_ROM_QSTR(MP_QSTR_s2n), MP_ROM_PTR(&mod_eoslib_s2n_obj) },
    { MP_ROM_QSTR(MP_QSTR_S), MP_ROM_PTR(&mod_eoslib_S_obj) },

};

STATIC MP_DEFINE_CONST_DICT(mp_module_eoslib_globals, mp_module_eoslib_globals_table);

const mp_obj_module_t mp_module_eoslib = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t*)&mp_module_eoslib_globals,
};
