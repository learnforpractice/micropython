#include <stdlib.h>
#include "../../../chain/micropython/mpeoslib.h"
#include "xxhash.h"

#if MICROPY_PY_EOSLIB
static struct eosapi s_eosapi;

void register_eosapi(struct eosapi * api) {
	s_eosapi = *api;
}

mp_uint_t mp_obj_get_uint(mp_const_obj_t self_in);

STATIC mp_obj_t mod_eoslib_now(void) {
   return mp_obj_new_int(now());
}
STATIC MP_DEFINE_CONST_FUN_OBJ_0(mod_eoslib_now_obj, mod_eoslib_now);

STATIC mp_obj_t mod_eoslib_abort(void) {
   abort_();
   return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_0(mod_eoslib_abort_obj, mod_eoslib_abort);

STATIC mp_obj_t mod_eoslib_eosio_assert(mp_obj_t obj1, mp_obj_t obj2) {
	size_t len;
	int condition = mp_obj_get_int(obj1);
   const char* str = mp_obj_str_get_data(obj2, &len);
   eosio_assert(condition, str);
	return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(mod_eoslib_eosio_assert_obj, mod_eoslib_eosio_assert);

//class crypto_api
STATIC mp_obj_t mod_eoslib_assert_recover_key(mp_obj_t obj1, mp_obj_t obj2, mp_obj_t obj3) {
	const char* data;
	size_t data_len;
	const char* sig;
	size_t siglen;
	const char* pub;
	size_t publen;

   data = mp_obj_str_get_data(obj1, &data_len);
   sig = mp_obj_str_get_data(obj2, &siglen);
   pub = mp_obj_str_get_data(obj3, &publen);
   assert_recover_key( data, data_len, sig, siglen, pub, publen );
	return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_3(mod_eoslib_assert_recover_key_obj, mod_eoslib_assert_recover_key);


STATIC mp_obj_t mod_eoslib_recover_key(mp_obj_t obj1, mp_obj_t obj2) {
	const char* data;
	size_t data_len;
	const char* sig;
	size_t siglen;

   data = mp_obj_str_get_data(obj1, &data_len);
   sig = mp_obj_str_get_data(obj2, &siglen);
   return recover_key( data, data_len, sig, siglen );
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(mod_eoslib_recover_key_obj, mod_eoslib_recover_key);


#define ASSERT_HASH(METHOD) \
STATIC mp_obj_t mod_eoslib_assert_##METHOD(mp_obj_t obj1, mp_obj_t obj2) { \
	const char* data; \
	size_t datalen; \
	const char* hash; \
	size_t hash_len; \
   data = mp_obj_str_get_data(obj2, &datalen); \
	hash = mp_obj_str_get_data(obj2, &hash_len); \
	assert_##METHOD(data, datalen, hash, hash_len); \
	return mp_const_none; \
} \
STATIC MP_DEFINE_CONST_FUN_OBJ_2(mod_eoslib_assert_##METHOD##_obj, mod_eoslib_assert_##METHOD);

ASSERT_HASH(sha1)
ASSERT_HASH(sha256)
ASSERT_HASH(sha512)
ASSERT_HASH(ripemd160)

#define HASH_METHOD(METHOD) \
STATIC mp_obj_t mod_eoslib_##METHOD(mp_obj_t obj1) { \
	const char* data; \
	size_t data_len; \
   data = mp_obj_str_get_data(obj1, &data_len); \
   return METHOD( data, data_len); \
} \
STATIC MP_DEFINE_CONST_FUN_OBJ_1(mod_eoslib_##METHOD##_obj, mod_eoslib_##METHOD);

HASH_METHOD(sha1)
HASH_METHOD(sha256)
HASH_METHOD(sha512)
HASH_METHOD(ripemd160)


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


STATIC mp_obj_t mod_eoslib_n2s(mp_obj_t obj) {
   uint64_t n = mp_obj_get_uint(obj);
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

//db i64
STATIC mp_obj_t mod_eoslib_db_store_i64(size_t n_args, const mp_obj_t *args) {
   size_t value_len = 0;
   uint64_t scope = mp_obj_get_uint(args[0]);
   uint64_t table = mp_obj_get_uint(args[1]);
   uint64_t payer = mp_obj_get_uint(args[2]);
   uint64_t id = mp_obj_get_uint(args[3]);
   void* value = (void *)mp_obj_str_get_data(args[4], &value_len);
   int itr = db_store_i64(scope, table, payer, id, value, value_len);
   return mp_obj_new_int(itr);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR(mod_eoslib_db_store_i64_obj, 5, mod_eoslib_db_store_i64);

STATIC mp_obj_t mod_eoslib_db_update_i64(size_t n_args, const mp_obj_t *args) {
   size_t value_len = 0;
   uint64_t itr = mp_obj_get_uint(args[0]);
   uint64_t payer = mp_obj_get_uint(args[1]);
   void* value = (void *)mp_obj_str_get_data(args[2], &value_len);
   db_update_i64(itr, payer, value, value_len);
   return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR(mod_eoslib_db_update_i64_obj, 3, mod_eoslib_db_update_i64);

STATIC mp_obj_t mod_eoslib_db_remove_i64(size_t n_args, const mp_obj_t *args) {
   uint64_t itr = mp_obj_get_uint(args[0]);
   db_remove_i64(itr);
   return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR(mod_eoslib_db_remove_i64_obj, 1, mod_eoslib_db_remove_i64);

STATIC mp_obj_t mod_eoslib_db_get_i64(size_t n_args, const mp_obj_t *args) {
   char value[256];
   uint64_t itr = mp_obj_get_uint(args[0]);
   int size = db_get_i64(itr, value, sizeof(value));
   if (size <= 0) {
		return mp_const_none;
   }
   return mp_obj_new_str(value, size);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR(mod_eoslib_db_get_i64_obj, 1, mod_eoslib_db_get_i64);

STATIC mp_obj_t mod_eoslib_db_next_i64(size_t n_args, const mp_obj_t *args) {
   uint64_t primary = 0;
   uint64_t itr = mp_obj_get_uint(args[0]);
   int itr_next = db_next_i64(itr, &primary);

   mp_obj_tuple_t *tuple = mp_obj_new_tuple(2, NULL);
   tuple->items[0] = mp_obj_new_int(itr_next);
   tuple->items[1] = mp_obj_new_int(primary);
   return tuple;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR(mod_eoslib_db_next_i64_obj, 1, mod_eoslib_db_next_i64);

STATIC mp_obj_t mod_eoslib_db_previous_i64(size_t n_args, const mp_obj_t *args) {
   uint64_t primary = 0;
   uint64_t itr = mp_obj_get_uint(args[0]);
   int itr_pre = db_previous_i64(itr, &primary);

   mp_obj_tuple_t *tuple = mp_obj_new_tuple(2, NULL);
   tuple->items[0] = mp_obj_new_int(itr_pre);
   tuple->items[1] = mp_obj_new_int(primary);
   return tuple;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR(mod_eoslib_db_previous_i64_obj, 1, mod_eoslib_db_previous_i64);


STATIC mp_obj_t mod_eoslib_db_find_i64(size_t n_args, const mp_obj_t *args) {
   uint64_t code = mp_obj_get_uint(args[0]);
   uint64_t scope = mp_obj_get_uint(args[1]);
   uint64_t table = mp_obj_get_uint(args[2]);
   uint64_t id = mp_obj_get_uint(args[3]);

   int itr = db_find_i64(code, scope, table, id);
   return mp_obj_new_int(itr);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR(mod_eoslib_db_find_i64_obj, 4, mod_eoslib_db_find_i64);

STATIC mp_obj_t mod_eoslib_db_lowerbound_i64(size_t n_args, const mp_obj_t *args) {
   uint64_t code = mp_obj_get_uint(args[0]);
   uint64_t scope = mp_obj_get_uint(args[1]);
   uint64_t table = mp_obj_get_uint(args[2]);
   uint64_t id = mp_obj_get_uint(args[3]);
   int itr = db_lowerbound_i64(code, scope, table, id);

   return mp_obj_new_int(itr);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR(mod_eoslib_db_lowerbound_i64_obj, 4, mod_eoslib_db_lowerbound_i64);

STATIC mp_obj_t mod_eoslib_db_upperbound_i64(size_t n_args, const mp_obj_t *args) {
   uint64_t code = mp_obj_get_uint(args[0]);
   uint64_t scope = mp_obj_get_uint(args[1]);
   uint64_t table = mp_obj_get_uint(args[2]);
   uint64_t id = mp_obj_get_uint(args[3]);

   int itr = db_upperbound_i64(code, scope, table, id);
   return mp_obj_new_int(itr);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR(mod_eoslib_db_upperbound_i64_obj, 4, mod_eoslib_db_upperbound_i64);

STATIC mp_obj_t mod_eoslib_db_end_i64(size_t n_args, const mp_obj_t *args) {
   uint64_t code = mp_obj_get_uint(args[0]);
   uint64_t scope = mp_obj_get_uint(args[1]);
   uint64_t table = mp_obj_get_uint(args[2]);

   int itr = db_end_i64(code, scope, table);
   return mp_obj_new_int(itr);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR(mod_eoslib_db_end_i64_obj, 3, mod_eoslib_db_end_i64);

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
   uint64_t arg1 = mp_obj_get_uint(obj1); \
   NAME(arg1); \
   return mp_const_none; \
} \
STATIC MP_DEFINE_CONST_FUN_OBJ_1(mod_eoslib_##NAME##_obj, mod_eoslib_##NAME);


#define METHOD2(NAME) \
STATIC mp_obj_t mod_eoslib_##NAME(mp_obj_t obj1, mp_obj_t obj2) { \
   uint64_t arg1 = mp_obj_get_uint(obj1); \
   uint64_t arg2 = mp_obj_get_uint(obj2); \
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
   uint64_t arg1 = mp_obj_get_uint(obj1);
   is_account(arg1);
   return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(mod_eoslib_is_account_obj, mod_eoslib_is_account);

STATIC mp_obj_t mod_eoslib_hash64(mp_obj_t obj1) {
	uint64_t key = 0;
	if (MP_OBJ_IS_STR(obj1)) {
		size_t len;
	   const char* str = mp_obj_str_get_data(obj1, &len);
	   key = XXH64(str, len, 0);
	   return mp_obj_new_int_from_ull(key);
	} else if (MP_OBJ_IS_INT(obj1)) {
		return obj1;
	} else {
		mp_raise_TypeError("can't hash unsupported type");
	}
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(mod_eoslib_hash64_obj, mod_eoslib_hash64);



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

	 { MP_ROM_QSTR(MP_QSTR_abort), MP_ROM_PTR(&mod_eoslib_abort_obj) },
	 { MP_ROM_QSTR(MP_QSTR_eosio_assert), MP_ROM_PTR(&mod_eoslib_eosio_assert_obj) },
	 { MP_ROM_QSTR(MP_QSTR_assert_recover_key), MP_ROM_PTR(&mod_eoslib_assert_recover_key_obj) },
	 { MP_ROM_QSTR(MP_QSTR_recover_key), MP_ROM_PTR(&mod_eoslib_recover_key_obj) },
	 { MP_ROM_QSTR(MP_QSTR_assert_sha1), MP_ROM_PTR(&mod_eoslib_assert_sha1_obj) },
	 { MP_ROM_QSTR(MP_QSTR_assert_sha256), MP_ROM_PTR(&mod_eoslib_assert_sha256_obj) },
	 { MP_ROM_QSTR(MP_QSTR_assert_sha512), MP_ROM_PTR(&mod_eoslib_assert_sha512_obj) },
	 { MP_ROM_QSTR(MP_QSTR_assert_ripemd160), MP_ROM_PTR(&mod_eoslib_assert_ripemd160_obj) },

	 { MP_ROM_QSTR(MP_QSTR_sha1), MP_ROM_PTR(&mod_eoslib_sha1_obj) },
	 { MP_ROM_QSTR(MP_QSTR_sha256), MP_ROM_PTR(&mod_eoslib_sha256_obj) },
	 { MP_ROM_QSTR(MP_QSTR_sha512), MP_ROM_PTR(&mod_eoslib_sha512_obj) },
	 { MP_ROM_QSTR(MP_QSTR_ripemd160), MP_ROM_PTR(&mod_eoslib_ripemd160_obj) },


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

	 { MP_ROM_QSTR(MP_QSTR_db_store_i64), MP_ROM_PTR(&mod_eoslib_db_store_i64_obj) },
	 { MP_ROM_QSTR(MP_QSTR_db_update_i64), MP_ROM_PTR(&mod_eoslib_db_update_i64_obj) },
	 { MP_ROM_QSTR(MP_QSTR_db_remove_i64), MP_ROM_PTR(&mod_eoslib_db_remove_i64_obj) },
	 { MP_ROM_QSTR(MP_QSTR_db_get_i64), MP_ROM_PTR(&mod_eoslib_db_get_i64_obj) },
	 { MP_ROM_QSTR(MP_QSTR_db_previous_i64), MP_ROM_PTR(&mod_eoslib_db_previous_i64_obj) },
	 { MP_ROM_QSTR(MP_QSTR_db_next_i64), MP_ROM_PTR(&mod_eoslib_db_next_i64_obj) },
	 { MP_ROM_QSTR(MP_QSTR_db_find_i64), MP_ROM_PTR(&mod_eoslib_db_find_i64_obj) },
	 { MP_ROM_QSTR(MP_QSTR_db_lowerbound_i64), MP_ROM_PTR(&mod_eoslib_db_lowerbound_i64_obj) },
	 { MP_ROM_QSTR(MP_QSTR_db_upperbound_i64), MP_ROM_PTR(&mod_eoslib_db_upperbound_i64_obj) },
	 { MP_ROM_QSTR(MP_QSTR_db_end_i64), MP_ROM_PTR(&mod_eoslib_db_end_i64_obj) },
	 { MP_ROM_QSTR(MP_QSTR_hash64), MP_ROM_PTR(&mod_eoslib_hash64_obj) },
};

STATIC MP_DEFINE_CONST_DICT(mp_module_eoslib_globals, mp_module_eoslib_globals_table);

const mp_obj_module_t mp_module_eoslib = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t*)&mp_module_eoslib_globals,
};

#endif //MICROPY_PY_EOSLIB
