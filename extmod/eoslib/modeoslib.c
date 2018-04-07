#include "modeoslib.h"

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
   return mp_obj_new_int_from_ll(n);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(mod_eoslib_s2n_obj, mod_eoslib_s2n);

STATIC mp_obj_t mod_eoslib_N(mp_obj_t obj) {
   size_t len;
   const char *account = mp_obj_str_get_data(obj, &len);
   uint64_t n = string_to_uint64_(account);
   return mp_obj_new_int_from_ll(n);
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
   return mp_obj_new_bytes((byte*)value, size);
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
	byte *data = malloc(size);
	read_action((char*)data, size);
	mp_obj_t obj = mp_obj_new_bytes(data, size);
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
   if (is_account(arg1)) {
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
	}
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(mod_eoslib_hash64_obj, mod_eoslib_hash64);



STATIC mp_obj_t mod_eoslib_S(mp_obj_t obj1, mp_obj_t obj2) {
	uint64_t ret;
	uint8_t precision;
	const char* str;
	size_t len;

	precision = (uint8_t)mp_obj_get_uint(obj1);

	str = mp_obj_str_get_data(obj2, &len);

	ret = string_to_symbol(precision, str);
	return mp_obj_new_int_from_ll(ret);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(mod_eoslib_S_obj, mod_eoslib_S);


#define DB_API_METHOD_WRAPPERS_SIMPLE_SECONDARY_WRAP(IDX, TYPE)\
      STATIC mp_obj_t mod_eoslib_db_##IDX##_store(size_t n_args, const mp_obj_t *args) { \
         size_t value_len = 0; \
         TYPE* secondary; \
         uint64_t scope = mp_obj_get_uint(args[0]); \
         uint64_t table = mp_obj_get_uint(args[1]); \
         uint64_t payer = mp_obj_get_uint(args[2]); \
         uint64_t id = mp_obj_get_uint(args[3]); \
         secondary = (TYPE *)mp_obj_str_get_data(args[4], &value_len); \
         eosio_assert(value_len == sizeof(TYPE), "bad length"); \
         int itr = db_##IDX##_store(scope, table, payer, id, (const char*)secondary, value_len); \
         return mp_obj_new_int(itr); \
      } \
      STATIC MP_DEFINE_CONST_FUN_OBJ_VAR(mod_eoslib_db_##IDX##_store_obj, 5, mod_eoslib_db_##IDX##_store); \
      \
      STATIC mp_obj_t mod_eoslib_db_##IDX##_update(size_t n_args, const mp_obj_t *args) { \
         size_t value_len = 0; \
         TYPE* secondary; \
         uint64_t iterator = mp_obj_get_uint(args[0]); \
         uint64_t payer = mp_obj_get_uint(args[1]); \
         secondary = (TYPE *)mp_obj_str_get_data(args[2], &value_len); \
         eosio_assert(value_len == sizeof(TYPE), "bad length"); \
         db_##IDX##_update(iterator, payer, (const char*)secondary, value_len); \
         return mp_const_none; \
      } \
      STATIC MP_DEFINE_CONST_FUN_OBJ_VAR(mod_eoslib_db_##IDX##_update_obj, 3, mod_eoslib_db_##IDX##_update); \
      STATIC mp_obj_t mod_eoslib_db_##IDX##_remove(size_t n_args, const mp_obj_t *args) { \
         uint64_t iterator = mp_obj_get_uint(args[0]); \
         db_##IDX##_remove(iterator); \
         return mp_const_none; \
      } \
      STATIC MP_DEFINE_CONST_FUN_OBJ_VAR(mod_eoslib_db_##IDX##_remove_obj, 3, mod_eoslib_db_##IDX##_remove); \
      STATIC mp_obj_t mod_eoslib_db_##IDX##_find_secondary(size_t n_args, const mp_obj_t *args) { \
         size_t value_len = 0; \
         TYPE* secondary; \
         uint64_t primary = 0; \
         uint64_t code = mp_obj_get_uint(args[0]); \
         uint64_t scope = mp_obj_get_uint(args[1]); \
         uint64_t table = mp_obj_get_uint(args[2]); \
         secondary = (TYPE *)mp_obj_str_get_data(args[3], &value_len); \
         int itr = db_##IDX##_find_secondary( code, scope, table, (const char*)secondary , sizeof(TYPE), &primary ); \
         mp_obj_tuple_t *tuple = mp_obj_new_tuple(2, NULL); \
         tuple->items[0] = mp_obj_new_int(itr); \
         tuple->items[1] = mp_obj_new_int(primary); \
         return tuple; \
      } \
      STATIC MP_DEFINE_CONST_FUN_OBJ_VAR(mod_eoslib_db_##IDX##_find_secondary_obj, 4, mod_eoslib_db_##IDX##_find_secondary); \
      STATIC mp_obj_t mod_eoslib_db_##IDX##_find_primary(size_t n_args, const mp_obj_t *args) { \
         TYPE secondary = 0; \
         uint64_t primary = 0; \
         uint64_t code = mp_obj_get_uint(args[0]); \
         uint64_t scope = mp_obj_get_uint(args[1]); \
         uint64_t table = mp_obj_get_uint(args[2]); \
         primary = mp_obj_get_uint(args[3]); \
         int itr = db_##IDX##_find_primary( code, scope, table, (char*)&secondary , sizeof(TYPE), primary ); \
         mp_obj_t _secondary = mp_obj_new_bytes((byte*)&secondary, sizeof(secondary)); \
         mp_obj_tuple_t *tuple = mp_obj_new_tuple(2, NULL); \
         tuple->items[0] = mp_obj_new_int(itr); \
         tuple->items[1] = _secondary; \
         return tuple; \
      } \
      STATIC MP_DEFINE_CONST_FUN_OBJ_VAR(mod_eoslib_db_##IDX##_find_primary_obj, 4, mod_eoslib_db_##IDX##_find_primary); \
      STATIC mp_obj_t mod_eoslib_db_##IDX##_lowerbound(size_t n_args, const mp_obj_t *args) { \
         uint64_t primary = 0; \
         TYPE secondary; \
         uint64_t code = mp_obj_get_uint(args[0]); \
         uint64_t scope = mp_obj_get_uint(args[1]); \
         uint64_t table = mp_obj_get_uint(args[2]); \
         int itr = db_##IDX##_lowerbound( code, scope, table, (char*)&secondary , sizeof(TYPE), &primary ); \
         mp_obj_t _secondary = mp_obj_new_bytes((byte*)&secondary, sizeof(TYPE)); \
         mp_obj_tuple_t *tuple = mp_obj_new_tuple(3, NULL); \
         tuple->items[0] = mp_obj_new_int(itr); \
         tuple->items[1] = mp_obj_new_int(primary); \
         tuple->items[2] = _secondary; \
         return tuple; \
      } \
      STATIC MP_DEFINE_CONST_FUN_OBJ_VAR(mod_eoslib_db_##IDX##_lowerbound_obj, 3, mod_eoslib_db_##IDX##_lowerbound); \
      \
      STATIC mp_obj_t mod_eoslib_db_##IDX##_upperbound(size_t n_args, const mp_obj_t *args) { \
         uint64_t primary = 0; \
         TYPE secondary  = 0; \
         uint64_t code = mp_obj_get_uint(args[0]); \
         uint64_t scope = mp_obj_get_uint(args[1]); \
         uint64_t table = mp_obj_get_uint(args[2]); \
         int itr = db_##IDX##_upperbound( code, scope, table, (char*)&secondary , sizeof(TYPE), &primary ); \
         mp_obj_t _secondary = mp_obj_new_bytes((byte*)&secondary, sizeof(TYPE)); \
         mp_obj_tuple_t *tuple = mp_obj_new_tuple(3, NULL); \
         tuple->items[0] = mp_obj_new_int(itr); \
         tuple->items[1] = mp_obj_new_int(primary); \
         tuple->items[2] = _secondary; \
         return tuple; \
      } \
      STATIC MP_DEFINE_CONST_FUN_OBJ_VAR(mod_eoslib_db_##IDX##_upperbound_obj, 3, mod_eoslib_db_##IDX##_upperbound); \
      STATIC mp_obj_t mod_eoslib_db_##IDX##_end(size_t n_args, const mp_obj_t *args) { \
         uint64_t code = mp_obj_get_uint(args[0]); \
         uint64_t scope = mp_obj_get_uint(args[1]); \
         uint64_t table = mp_obj_get_uint(args[2]); \
         int itr = db_##IDX##_end( code, scope, table ); \
         return mp_obj_new_int(itr); \
      } \
      STATIC MP_DEFINE_CONST_FUN_OBJ_VAR(mod_eoslib_db_##IDX##_end_obj, 3, mod_eoslib_db_##IDX##_end); \
      \
      STATIC mp_obj_t mod_eoslib_db_##IDX##_next(size_t n_args, const mp_obj_t *args) { \
         uint64_t primary = 0;\
         uint64_t iterator = mp_obj_get_uint(args[0]); \
         int itr = db_##IDX##_next(iterator, &primary); \
         mp_obj_tuple_t *tuple = mp_obj_new_tuple(2, NULL); \
         tuple->items[0] = mp_obj_new_int(itr); \
         tuple->items[1] = mp_obj_new_int(primary); \
         return tuple; \
      } \
      STATIC MP_DEFINE_CONST_FUN_OBJ_VAR(mod_eoslib_db_##IDX##_next_obj, 1, mod_eoslib_db_##IDX##_next); \
      STATIC mp_obj_t mod_eoslib_db_##IDX##_previous(size_t n_args, const mp_obj_t *args) { \
         uint64_t primary = 0;\
         uint64_t iterator = mp_obj_get_uint(args[0]); \
         int itr = db_##IDX##_previous(iterator, &primary); \
         mp_obj_tuple_t *tuple = mp_obj_new_tuple(2, NULL); \
         tuple->items[0] = mp_obj_new_int(itr); \
         tuple->items[1] = mp_obj_new_int(primary); \
         return tuple; \
      } \
      STATIC MP_DEFINE_CONST_FUN_OBJ_VAR(mod_eoslib_db_##IDX##_previous_obj, 1, mod_eoslib_db_##IDX##_previous);


      #define DB_API_METHOD_WRAPPERS_ARRAY_SECONDARY_WRAP(IDX, ARR_SIZE, ARR_ELEMENT_TYPE)\
      STATIC mp_obj_t mod_eoslib_db_##IDX##_store(size_t n_args, const mp_obj_t *args) { \
         size_t value_len = 0; \
         ARR_ELEMENT_TYPE* secondary; \
         uint64_t scope = mp_obj_get_uint(args[0]); \
         uint64_t table = mp_obj_get_uint(args[1]); \
         uint64_t payer = mp_obj_get_uint(args[2]); \
         uint64_t id = mp_obj_get_uint(args[3]); \
         secondary = (ARR_ELEMENT_TYPE *)mp_obj_str_get_data(args[4], &value_len); \
         eosio_assert(value_len == sizeof(ARR_ELEMENT_TYPE)*ARR_SIZE, "bad length"); \
         int itr = db_##IDX##_store(scope, table, payer, id, (const char*)secondary, value_len); \
         return mp_obj_new_int(itr); \
      } \
      STATIC MP_DEFINE_CONST_FUN_OBJ_VAR(mod_eoslib_db_##IDX##_store_obj, 5, mod_eoslib_db_##IDX##_store); \
      \
      STATIC mp_obj_t mod_eoslib_db_##IDX##_update(size_t n_args, const mp_obj_t *args) { \
         size_t value_len = 0; \
         ARR_ELEMENT_TYPE* secondary; \
         uint64_t iterator = mp_obj_get_uint(args[0]); \
         uint64_t payer = mp_obj_get_uint(args[1]); \
         secondary = (ARR_ELEMENT_TYPE *)mp_obj_str_get_data(args[2], &value_len); \
         eosio_assert(value_len == sizeof(ARR_ELEMENT_TYPE)*ARR_SIZE, "bad length"); \
         db_##IDX##_update(iterator, payer, (const char*)secondary, value_len); \
         return mp_const_none; \
      } \
      STATIC MP_DEFINE_CONST_FUN_OBJ_VAR(mod_eoslib_db_##IDX##_update_obj, 3, mod_eoslib_db_##IDX##_update); \
      STATIC mp_obj_t mod_eoslib_db_##IDX##_remove(size_t n_args, const mp_obj_t *args) { \
         uint64_t iterator = mp_obj_get_uint(args[0]); \
         db_##IDX##_remove(iterator); \
         return mp_const_none; \
      } \
      STATIC MP_DEFINE_CONST_FUN_OBJ_VAR(mod_eoslib_db_##IDX##_remove_obj, 3, mod_eoslib_db_##IDX##_remove); \
      STATIC mp_obj_t mod_eoslib_db_##IDX##_find_secondary(size_t n_args, const mp_obj_t *args) { \
         size_t value_len = 0; \
         ARR_ELEMENT_TYPE* secondary; \
         uint64_t primary = 0; \
         uint64_t code = mp_obj_get_uint(args[0]); \
         uint64_t scope = mp_obj_get_uint(args[1]); \
         uint64_t table = mp_obj_get_uint(args[2]); \
         secondary = (ARR_ELEMENT_TYPE *)mp_obj_str_get_data(args[3], &value_len); \
         int itr = db_##IDX##_find_secondary( code, scope, table, (const char*)secondary , sizeof(ARR_ELEMENT_TYPE)*ARR_SIZE, &primary ); \
         mp_obj_tuple_t *tuple = mp_obj_new_tuple(2, NULL); \
         tuple->items[0] = mp_obj_new_int(itr); \
         tuple->items[1] = mp_obj_new_int(primary); \
         return tuple; \
      } \
      STATIC MP_DEFINE_CONST_FUN_OBJ_VAR(mod_eoslib_db_##IDX##_find_secondary_obj, 4, mod_eoslib_db_##IDX##_find_secondary); \
      STATIC mp_obj_t mod_eoslib_db_##IDX##_find_primary(size_t n_args, const mp_obj_t *args) { \
         ARR_ELEMENT_TYPE secondary[ARR_SIZE]; \
         uint64_t primary = 0; \
         uint64_t code = mp_obj_get_uint(args[0]); \
         uint64_t scope = mp_obj_get_uint(args[1]); \
         uint64_t table = mp_obj_get_uint(args[2]); \
         primary = mp_obj_get_uint(args[3]); \
         int itr = db_##IDX##_find_primary( code, scope, table, (char*)&secondary , sizeof(ARR_ELEMENT_TYPE)*ARR_SIZE, primary ); \
         mp_obj_t _secondary = mp_obj_new_bytes((byte*)&secondary, sizeof(secondary)); \
         mp_obj_tuple_t *tuple = mp_obj_new_tuple(2, NULL); \
         tuple->items[0] = mp_obj_new_int(itr); \
         tuple->items[1] = _secondary; \
         return tuple; \
      } \
      STATIC MP_DEFINE_CONST_FUN_OBJ_VAR(mod_eoslib_db_##IDX##_find_primary_obj, 4, mod_eoslib_db_##IDX##_find_primary); \
      STATIC mp_obj_t mod_eoslib_db_##IDX##_lowerbound(size_t n_args, const mp_obj_t *args) { \
         uint64_t primary = 0; \
         ARR_ELEMENT_TYPE secondary[ARR_SIZE]; \
         uint64_t code = mp_obj_get_uint(args[0]); \
         uint64_t scope = mp_obj_get_uint(args[1]); \
         uint64_t table = mp_obj_get_uint(args[2]); \
         int itr = db_##IDX##_lowerbound( code, scope, table, (char*)&secondary , sizeof(ARR_ELEMENT_TYPE)*ARR_SIZE, &primary ); \
         mp_obj_t _secondary = mp_obj_new_bytes((byte*)&secondary, sizeof(ARR_ELEMENT_TYPE)*ARR_SIZE); \
         mp_obj_tuple_t *tuple = mp_obj_new_tuple(3, NULL); \
         tuple->items[0] = mp_obj_new_int(itr); \
         tuple->items[1] = mp_obj_new_int(primary); \
         tuple->items[2] = _secondary; \
         return tuple; \
      } \
      STATIC MP_DEFINE_CONST_FUN_OBJ_VAR(mod_eoslib_db_##IDX##_lowerbound_obj, 3, mod_eoslib_db_##IDX##_lowerbound); \
      \
      STATIC mp_obj_t mod_eoslib_db_##IDX##_upperbound(size_t n_args, const mp_obj_t *args) { \
         uint64_t primary = 0; \
         ARR_ELEMENT_TYPE secondary[ARR_SIZE]; \
         uint64_t code = mp_obj_get_uint(args[0]); \
         uint64_t scope = mp_obj_get_uint(args[1]); \
         uint64_t table = mp_obj_get_uint(args[2]); \
         int itr = db_##IDX##_upperbound( code, scope, table, (char*)&secondary , sizeof(secondary), &primary ); \
         mp_obj_t _secondary = mp_obj_new_bytes((byte*)&secondary, sizeof(secondary)); \
         mp_obj_tuple_t *tuple = mp_obj_new_tuple(3, NULL); \
         tuple->items[0] = mp_obj_new_int(itr); \
         tuple->items[1] = mp_obj_new_int(primary); \
         tuple->items[2] = _secondary; \
         return tuple; \
      } \
      STATIC MP_DEFINE_CONST_FUN_OBJ_VAR(mod_eoslib_db_##IDX##_upperbound_obj, 3, mod_eoslib_db_##IDX##_upperbound); \
      STATIC mp_obj_t mod_eoslib_db_##IDX##_end(size_t n_args, const mp_obj_t *args) { \
         uint64_t code = mp_obj_get_uint(args[0]); \
         uint64_t scope = mp_obj_get_uint(args[1]); \
         uint64_t table = mp_obj_get_uint(args[2]); \
         int itr = db_##IDX##_end( code, scope, table ); \
         return mp_obj_new_int(itr); \
      } \
      STATIC MP_DEFINE_CONST_FUN_OBJ_VAR(mod_eoslib_db_##IDX##_end_obj, 3, mod_eoslib_db_##IDX##_end); \
      \
      STATIC mp_obj_t mod_eoslib_db_##IDX##_next(size_t n_args, const mp_obj_t *args) { \
         uint64_t primary = 0;\
         uint64_t iterator = mp_obj_get_uint(args[0]); \
         int itr = db_##IDX##_next(iterator, &primary); \
         mp_obj_tuple_t *tuple = mp_obj_new_tuple(2, NULL); \
         tuple->items[0] = mp_obj_new_int(itr); \
         tuple->items[1] = mp_obj_new_int(primary); \
         return tuple; \
      } \
      STATIC MP_DEFINE_CONST_FUN_OBJ_VAR(mod_eoslib_db_##IDX##_next_obj, 1, mod_eoslib_db_##IDX##_next); \
      STATIC mp_obj_t mod_eoslib_db_##IDX##_previous(size_t n_args, const mp_obj_t *args) { \
         uint64_t primary = 0;\
         uint64_t iterator = mp_obj_get_uint(args[0]); \
         int itr = db_##IDX##_previous(iterator, &primary); \
         mp_obj_tuple_t *tuple = mp_obj_new_tuple(2, NULL); \
         tuple->items[0] = mp_obj_new_int(itr); \
         tuple->items[1] = mp_obj_new_int(primary); \
         return tuple; \
      } \
      STATIC MP_DEFINE_CONST_FUN_OBJ_VAR(mod_eoslib_db_##IDX##_previous_obj, 1, mod_eoslib_db_##IDX##_previous);

DB_API_METHOD_WRAPPERS_SIMPLE_SECONDARY_WRAP(idx64, uint64_t)
DB_API_METHOD_WRAPPERS_SIMPLE_SECONDARY_WRAP(idx128, uint128_t)
DB_API_METHOD_WRAPPERS_SIMPLE_SECONDARY_WRAP(idx_double, uint64_t)
DB_API_METHOD_WRAPPERS_ARRAY_SECONDARY_WRAP(idx256, 2, uint128_t)

#if 0
DB_API_METHOD_WRAPPERS_SIMPLE_SECONDARY_WRAP(idx64,  uint64_t)

DB_API_METHOD_WRAPPERS_ARRAY_SECONDARY_WRAP(idx256, 2, uint128_t)
#endif

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

   { MP_ROM_QSTR(MP_QSTR_db_idx64_store), MP_ROM_PTR(&mod_eoslib_db_idx64_store_obj) },
   { MP_ROM_QSTR(MP_QSTR_db_idx64_update), MP_ROM_PTR(&mod_eoslib_db_idx64_update_obj) },
   { MP_ROM_QSTR(MP_QSTR_db_idx64_remove), MP_ROM_PTR(&mod_eoslib_db_idx64_remove_obj) },
   { MP_ROM_QSTR(MP_QSTR_db_idx64_find_secondary), MP_ROM_PTR(&mod_eoslib_db_idx64_find_secondary_obj) },
   { MP_ROM_QSTR(MP_QSTR_db_idx64_find_primary), MP_ROM_PTR(&mod_eoslib_db_idx64_find_primary_obj) },
   { MP_ROM_QSTR(MP_QSTR_db_idx64_lowerbound), MP_ROM_PTR(&mod_eoslib_db_idx64_lowerbound_obj) },
   { MP_ROM_QSTR(MP_QSTR_db_idx64_upperbound), MP_ROM_PTR(&mod_eoslib_db_idx64_upperbound_obj) },
   { MP_ROM_QSTR(MP_QSTR_db_idx64_end), MP_ROM_PTR(&mod_eoslib_db_idx64_end_obj) },
   { MP_ROM_QSTR(MP_QSTR_db_idx64_next), MP_ROM_PTR(&mod_eoslib_db_idx64_next_obj) },
   { MP_ROM_QSTR(MP_QSTR_db_idx64_previous), MP_ROM_PTR(&mod_eoslib_db_idx64_previous_obj) },

   { MP_ROM_QSTR(MP_QSTR_db_idx128_store), MP_ROM_PTR(&mod_eoslib_db_idx128_store_obj) },
   { MP_ROM_QSTR(MP_QSTR_db_idx128_update), MP_ROM_PTR(&mod_eoslib_db_idx128_update_obj) },
   { MP_ROM_QSTR(MP_QSTR_db_idx128_remove), MP_ROM_PTR(&mod_eoslib_db_idx128_remove_obj) },
   { MP_ROM_QSTR(MP_QSTR_db_idx128_find_secondary), MP_ROM_PTR(&mod_eoslib_db_idx128_find_secondary_obj) },
   { MP_ROM_QSTR(MP_QSTR_db_idx128_find_primary), MP_ROM_PTR(&mod_eoslib_db_idx128_find_primary_obj) },
   { MP_ROM_QSTR(MP_QSTR_db_idx128_lowerbound), MP_ROM_PTR(&mod_eoslib_db_idx128_lowerbound_obj) },
   { MP_ROM_QSTR(MP_QSTR_db_idx128_upperbound), MP_ROM_PTR(&mod_eoslib_db_idx128_upperbound_obj) },
   { MP_ROM_QSTR(MP_QSTR_db_idx128_end), MP_ROM_PTR(&mod_eoslib_db_idx128_end_obj) },
   { MP_ROM_QSTR(MP_QSTR_db_idx128_next), MP_ROM_PTR(&mod_eoslib_db_idx128_next_obj) },
   { MP_ROM_QSTR(MP_QSTR_db_idx128_previous), MP_ROM_PTR(&mod_eoslib_db_idx128_previous_obj) },

   { MP_ROM_QSTR(MP_QSTR_db_idx_double_store), MP_ROM_PTR(&mod_eoslib_db_idx_double_store_obj) },
   { MP_ROM_QSTR(MP_QSTR_db_idx_double_update), MP_ROM_PTR(&mod_eoslib_db_idx_double_update_obj) },
   { MP_ROM_QSTR(MP_QSTR_db_idx_double_remove), MP_ROM_PTR(&mod_eoslib_db_idx_double_remove_obj) },
   { MP_ROM_QSTR(MP_QSTR_db_idx_double_find_secondary), MP_ROM_PTR(&mod_eoslib_db_idx_double_find_secondary_obj) },
   { MP_ROM_QSTR(MP_QSTR_db_idx_double_find_primary), MP_ROM_PTR(&mod_eoslib_db_idx_double_find_primary_obj) },
   { MP_ROM_QSTR(MP_QSTR_db_idx_double_lowerbound), MP_ROM_PTR(&mod_eoslib_db_idx_double_lowerbound_obj) },
   { MP_ROM_QSTR(MP_QSTR_db_idx_double_upperbound), MP_ROM_PTR(&mod_eoslib_db_idx_double_upperbound_obj) },
   { MP_ROM_QSTR(MP_QSTR_db_idx_double_end), MP_ROM_PTR(&mod_eoslib_db_idx_double_end_obj) },
   { MP_ROM_QSTR(MP_QSTR_db_idx_double_next), MP_ROM_PTR(&mod_eoslib_db_idx_double_next_obj) },
   { MP_ROM_QSTR(MP_QSTR_db_idx_double_previous), MP_ROM_PTR(&mod_eoslib_db_idx_double_previous_obj) },

   { MP_ROM_QSTR(MP_QSTR_db_idx256_store), MP_ROM_PTR(&mod_eoslib_db_idx256_store_obj) },
   { MP_ROM_QSTR(MP_QSTR_db_idx256_update), MP_ROM_PTR(&mod_eoslib_db_idx256_update_obj) },
   { MP_ROM_QSTR(MP_QSTR_db_idx256_remove), MP_ROM_PTR(&mod_eoslib_db_idx256_remove_obj) },
   { MP_ROM_QSTR(MP_QSTR_db_idx256_find_secondary), MP_ROM_PTR(&mod_eoslib_db_idx256_find_secondary_obj) },
   { MP_ROM_QSTR(MP_QSTR_db_idx256_find_primary), MP_ROM_PTR(&mod_eoslib_db_idx256_find_primary_obj) },
   { MP_ROM_QSTR(MP_QSTR_db_idx256_lowerbound), MP_ROM_PTR(&mod_eoslib_db_idx256_lowerbound_obj) },
   { MP_ROM_QSTR(MP_QSTR_db_idx256_upperbound), MP_ROM_PTR(&mod_eoslib_db_idx256_upperbound_obj) },
   { MP_ROM_QSTR(MP_QSTR_db_idx256_end), MP_ROM_PTR(&mod_eoslib_db_idx256_end_obj) },
   { MP_ROM_QSTR(MP_QSTR_db_idx256_next), MP_ROM_PTR(&mod_eoslib_db_idx256_next_obj) },
   { MP_ROM_QSTR(MP_QSTR_db_idx256_previous), MP_ROM_PTR(&mod_eoslib_db_idx256_previous_obj) },

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

	 { MP_ROM_QSTR(MP_QSTR_S), MP_ROM_PTR(&mod_eoslib_S_obj) },
};

STATIC MP_DEFINE_CONST_DICT(mp_module_eoslib_globals, mp_module_eoslib_globals_table);

const mp_obj_module_t mp_module_eoslib = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t*)&mp_module_eoslib_globals,
};

#endif //MICROPY_PY_EOSLIB
