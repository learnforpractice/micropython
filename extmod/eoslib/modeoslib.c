#include "modeoslib.h"

#if MICROPY_PY_EOSLIB
static struct eosapi api;

mp_obj_t micropy_load_from_py(const char *mod_name, const char *data, size_t len);
mp_obj_t micropy_load_from_mpy(const char *mod_name, const char *data, size_t len);
mp_obj_t micropy_call_0(mp_obj_t module_obj, const char *func);
mp_obj_t micropy_call_2(mp_obj_t module_obj, const char *func, uint64_t code, uint64_t type);

//in mp_compiler.c
int compile_and_save_to_buffer(const char* src_name, const char *src_buffer, size_t src_size, char* buffer, size_t size);

//defined in vm.c
void execution_start();
void execution_end();

//main_eos.c
int main_micropython(int argc, char **argv);
mp_obj_t mp_call_function_0(mp_obj_t fun);
mp_obj_t mp_compile(mp_parse_tree_t *parse_tree, qstr source_file, uint emit_opt, bool is_repl);

static int s_debug=0;

int py_is_debug_mode() {
   return s_debug;
}

void py_set_debug_mode(int mode) {
   s_debug = mode;
}

void mp_register_eosapi(struct eosapi * _api) {
	api = *_api;
}

struct eosapi* mp_get_eosapi(struct eosapi * _api) {
   return &api;
}

//main_eos.c
void* execute_from_str(const char *str);

void mp_obtain_mpapi(struct mpapi * _api) {
   if (!_api) {
      return;
   }
   _api->mp_obj_new_str = mp_obj_new_str;
   _api->micropy_load_from_py = micropy_load_from_py;
   _api->micropy_load_from_mpy = micropy_load_from_mpy;
   _api->micropy_call_0 = micropy_call_0;
   _api->micropy_call_2 = micropy_call_2;
   _api->execute_from_str = execute_from_str;

   _api->execution_start = execution_start;
   _api->execution_end = execution_end;

   _api->main_micropython = main_micropython;
   _api->mp_call_function_0 = mp_call_function_0;
   _api->mp_compile = mp_compile;

   _api->compile_and_save_to_buffer = compile_and_save_to_buffer;
   _api->set_debug_mode = py_set_debug_mode;

}

mp_uint_t mp_obj_get_uint(mp_const_obj_t self_in);

STATIC mp_obj_t mod_eoslib_now(void) {
   return mp_obj_new_int(api.now());
}
STATIC MP_DEFINE_CONST_FUN_OBJ_0(mod_eoslib_now_obj, mod_eoslib_now);

STATIC mp_obj_t mod_eoslib_abort(void) {
   api.abort_();
   return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_0(mod_eoslib_abort_obj, mod_eoslib_abort);

STATIC mp_obj_t mod_eoslib_eosio_assert(mp_obj_t obj1, mp_obj_t obj2) {
	size_t len;
	int condition = mp_obj_get_int(obj1);
   const char* str = mp_obj_str_get_data(obj2, &len);
   api.eosio_assert(condition, str);
	return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(mod_eoslib_eosio_assert_obj, mod_eoslib_eosio_assert);

STATIC mp_obj_t mod_eoslib_eosio_delay(mp_obj_t obj1) {
   int ms = mp_obj_get_int(obj1);
   api.eosio_delay(ms);
   return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(mod_eoslib_eosio_delay_obj, mod_eoslib_eosio_delay);


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
   api.assert_recover_key( data, data_len, sig, siglen, pub, publen );
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
   return api.recover_key( data, data_len, sig, siglen );
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
	api.assert_##METHOD(data, datalen, hash, hash_len); \
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
   return api.METHOD( data, data_len); \
} \
STATIC MP_DEFINE_CONST_FUN_OBJ_1(mod_eoslib_##METHOD##_obj, mod_eoslib_##METHOD);

HASH_METHOD(sha1)
HASH_METHOD(sha256)
HASH_METHOD(sha512)
HASH_METHOD(ripemd160)


STATIC mp_obj_t mod_eoslib_s2n(mp_obj_t obj) {
   size_t len;
   const char *account = mp_obj_str_get_data(obj, &len);
   uint64_t n = api.string_to_uint64_(account);
   return mp_obj_new_int_from_ll(n);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(mod_eoslib_s2n_obj, mod_eoslib_s2n);

STATIC mp_obj_t mod_eoslib_N(mp_obj_t obj) {
   size_t len;
   const char *account = mp_obj_str_get_data(obj, &len);
   uint64_t n = api.string_to_uint64_(account);
   return mp_obj_new_int_from_ll(n);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(mod_eoslib_N_obj, mod_eoslib_N);


STATIC mp_obj_t mod_eoslib_n2s(mp_obj_t obj) {
   uint64_t n = mp_obj_get_uint(obj);
   return api.uint64_to_string_(n);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(mod_eoslib_n2s_obj, mod_eoslib_n2s);

STATIC mp_obj_t mod_eoslib_pack(mp_obj_t obj) {
   size_t len = 0;
   const char *s = mp_obj_str_get_data(obj, &len);
   return api.pack_(s, len);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(mod_eoslib_pack_obj, mod_eoslib_pack);

STATIC mp_obj_t mod_eoslib_unpack(mp_obj_t obj) {
   size_t len = 0;
   const char *s = mp_obj_str_get_data(obj, &len);
   return api.unpack_(s, len);
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
   int itr = api.db_store_i64(scope, table, payer, id, value, value_len);
   return mp_obj_new_int(itr);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR(mod_eoslib_db_store_i64_obj, 5, mod_eoslib_db_store_i64);

STATIC mp_obj_t mod_eoslib_db_update_i64(size_t n_args, const mp_obj_t *args) {
   size_t value_len = 0;
   uint64_t itr = mp_obj_get_uint(args[0]);
   uint64_t payer = mp_obj_get_uint(args[1]);
   void* value = (void *)mp_obj_str_get_data(args[2], &value_len);
   api.db_update_i64(itr, payer, value, value_len);
   return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR(mod_eoslib_db_update_i64_obj, 3, mod_eoslib_db_update_i64);

STATIC mp_obj_t mod_eoslib_db_remove_i64(size_t n_args, const mp_obj_t *args) {
   uint64_t itr = mp_obj_get_uint(args[0]);
   api.db_remove_i64(itr);
   return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR(mod_eoslib_db_remove_i64_obj, 1, mod_eoslib_db_remove_i64);

///TODO: hard coded buffer size
static char buffer[128*1024];
STATIC mp_obj_t mod_eoslib_db_get_i64(size_t n_args, const mp_obj_t *args) {
   uint64_t itr = mp_obj_get_uint(args[0]);
   int size = api.db_get_i64(itr, buffer, sizeof(buffer));
   if (size <= 0) {
      return mp_const_none;
   }
   if (size > sizeof(buffer)) {
      size = sizeof(buffer);
   }
   return mp_obj_new_bytes((byte*)buffer, size);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR(mod_eoslib_db_get_i64_obj, 1, mod_eoslib_db_get_i64);

STATIC mp_obj_t mod_eoslib_db_get_i64_ex(size_t n_args, const mp_obj_t *args) {
   uint64_t primary = 0;
   uint64_t itr = mp_obj_get_uint(args[0]);
   int size = api.db_get_i64_ex(itr, &primary, buffer, sizeof(buffer));
   if (size <= 0) {
      return mp_const_none;
   }

   if (size > sizeof(buffer)) {
      size = sizeof(buffer);
   }

   mp_obj_tuple_t *tuple = mp_obj_new_tuple(2, NULL);
   tuple->items[0] = mp_obj_new_int(primary);
   tuple->items[1] = mp_obj_new_bytes((byte*)buffer, size);
   return tuple;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR(mod_eoslib_db_get_i64_ex_obj, 1, mod_eoslib_db_get_i64_ex);


STATIC mp_obj_t mod_eoslib_db_next_i64(size_t n_args, const mp_obj_t *args) {
   uint64_t primary = 0;
   uint64_t itr = mp_obj_get_uint(args[0]);
   int itr_next = api.db_next_i64(itr, &primary);

   mp_obj_tuple_t *tuple = mp_obj_new_tuple(2, NULL);
   tuple->items[0] = mp_obj_new_int(itr_next);
   tuple->items[1] = mp_obj_new_int(primary);
   return tuple;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR(mod_eoslib_db_next_i64_obj, 1, mod_eoslib_db_next_i64);

STATIC mp_obj_t mod_eoslib_db_previous_i64(size_t n_args, const mp_obj_t *args) {
   uint64_t primary = 0;
   uint64_t itr = mp_obj_get_uint(args[0]);
   int itr_pre = api.db_previous_i64(itr, &primary);

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

   int itr = api.db_find_i64(code, scope, table, id);
   return mp_obj_new_int(itr);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR(mod_eoslib_db_find_i64_obj, 4, mod_eoslib_db_find_i64);

STATIC mp_obj_t mod_eoslib_db_lowerbound_i64(size_t n_args, const mp_obj_t *args) {
   uint64_t code = mp_obj_get_uint(args[0]);
   uint64_t scope = mp_obj_get_uint(args[1]);
   uint64_t table = mp_obj_get_uint(args[2]);
   uint64_t id = mp_obj_get_uint(args[3]);
   int itr = api.db_lowerbound_i64(code, scope, table, id);

   return mp_obj_new_int(itr);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR(mod_eoslib_db_lowerbound_i64_obj, 4, mod_eoslib_db_lowerbound_i64);

STATIC mp_obj_t mod_eoslib_db_upperbound_i64(size_t n_args, const mp_obj_t *args) {
   uint64_t code = mp_obj_get_uint(args[0]);
   uint64_t scope = mp_obj_get_uint(args[1]);
   uint64_t table = mp_obj_get_uint(args[2]);
   uint64_t id = mp_obj_get_uint(args[3]);

   int itr = api.db_upperbound_i64(code, scope, table, id);
   return mp_obj_new_int(itr);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR(mod_eoslib_db_upperbound_i64_obj, 4, mod_eoslib_db_upperbound_i64);

STATIC mp_obj_t mod_eoslib_db_end_i64(size_t n_args, const mp_obj_t *args) {
   uint64_t code = mp_obj_get_uint(args[0]);
   uint64_t scope = mp_obj_get_uint(args[1]);
   uint64_t table = mp_obj_get_uint(args[2]);

   int itr = api.db_end_i64(code, scope, table);
   return mp_obj_new_int(itr);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR(mod_eoslib_db_end_i64_obj, 3, mod_eoslib_db_end_i64);

STATIC mp_obj_t mod_eoslib_read_action(void) {
	size_t size = api.action_size();
	if (size == 0) {
		return mp_const_none;
	}
	byte *data = calloc(size, 1);
	api.read_action((char*)data, size);
	mp_obj_t obj = mp_obj_new_bytes(data, size);
	free(data);
   return obj;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_0(mod_eoslib_read_action_obj, mod_eoslib_read_action);


int read_action(char* memory, size_t size);

#define METHOD0(NAME) \
STATIC mp_obj_t mod_eoslib_##NAME(void) { \
   return mp_obj_new_int(api.NAME()); \
} \
STATIC MP_DEFINE_CONST_FUN_OBJ_0(mod_eoslib_##NAME##_obj, mod_eoslib_##NAME);

METHOD0(action_size)
METHOD0(current_receiver)
METHOD0(publication_time)
METHOD0(current_sender)


#define METHOD1(NAME) \
STATIC mp_obj_t mod_eoslib_##NAME(mp_obj_t obj1) { \
   uint64_t arg1 = mp_obj_get_uint(obj1); \
   api.NAME(arg1); \
   return mp_const_none; \
} \
STATIC MP_DEFINE_CONST_FUN_OBJ_1(mod_eoslib_##NAME##_obj, mod_eoslib_##NAME);


#define METHOD2(NAME) \
STATIC mp_obj_t mod_eoslib_##NAME(mp_obj_t obj1, mp_obj_t obj2) { \
   uint64_t arg1 = mp_obj_get_uint(obj1); \
   uint64_t arg2 = mp_obj_get_uint(obj2); \
   api.NAME(arg1,arg2); \
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
   if (api.is_account(arg1)) {
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

	ret = api.string_to_symbol(precision, str);
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
         api.eosio_assert(value_len == sizeof(TYPE), "bad length"); \
         int itr = api.db_##IDX##_store(scope, table, payer, id, (const char*)secondary, value_len); \
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
         api.eosio_assert(value_len == sizeof(TYPE), "bad length"); \
         api.db_##IDX##_update(iterator, payer, (const char*)secondary, value_len); \
         return mp_const_none; \
      } \
      STATIC MP_DEFINE_CONST_FUN_OBJ_VAR(mod_eoslib_db_##IDX##_update_obj, 3, mod_eoslib_db_##IDX##_update); \
      STATIC mp_obj_t mod_eoslib_db_##IDX##_remove(size_t n_args, const mp_obj_t *args) { \
         uint64_t iterator = mp_obj_get_uint(args[0]); \
         api.db_##IDX##_remove(iterator); \
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
         int itr = api.db_##IDX##_find_secondary( code, scope, table, (const char*)secondary , sizeof(TYPE), &primary ); \
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
         int itr = api.db_##IDX##_find_primary( code, scope, table, (char*)&secondary , sizeof(TYPE), primary ); \
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
         int itr = api.db_##IDX##_lowerbound( code, scope, table, (char*)&secondary , sizeof(TYPE), &primary ); \
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
         int itr = api.db_##IDX##_upperbound( code, scope, table, (char*)&secondary , sizeof(TYPE), &primary ); \
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
         int itr = api.db_##IDX##_end( code, scope, table ); \
         return mp_obj_new_int(itr); \
      } \
      STATIC MP_DEFINE_CONST_FUN_OBJ_VAR(mod_eoslib_db_##IDX##_end_obj, 3, mod_eoslib_db_##IDX##_end); \
      \
      STATIC mp_obj_t mod_eoslib_db_##IDX##_next(size_t n_args, const mp_obj_t *args) { \
         uint64_t primary = 0;\
         uint64_t iterator = mp_obj_get_uint(args[0]); \
         int itr = api.db_##IDX##_next(iterator, &primary); \
         mp_obj_tuple_t *tuple = mp_obj_new_tuple(2, NULL); \
         tuple->items[0] = mp_obj_new_int(itr); \
         tuple->items[1] = mp_obj_new_int(primary); \
         return tuple; \
      } \
      STATIC MP_DEFINE_CONST_FUN_OBJ_VAR(mod_eoslib_db_##IDX##_next_obj, 1, mod_eoslib_db_##IDX##_next); \
      STATIC mp_obj_t mod_eoslib_db_##IDX##_previous(size_t n_args, const mp_obj_t *args) { \
         uint64_t primary = 0;\
         uint64_t iterator = mp_obj_get_uint(args[0]); \
         int itr = api.db_##IDX##_previous(iterator, &primary); \
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
         api.eosio_assert(value_len == sizeof(ARR_ELEMENT_TYPE)*ARR_SIZE, "bad length"); \
         int itr = api.db_##IDX##_store(scope, table, payer, id, (const char*)secondary, value_len); \
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
         api.eosio_assert(value_len == sizeof(ARR_ELEMENT_TYPE)*ARR_SIZE, "bad length"); \
         api.db_##IDX##_update(iterator, payer, (const char*)secondary, value_len); \
         return mp_const_none; \
      } \
      STATIC MP_DEFINE_CONST_FUN_OBJ_VAR(mod_eoslib_db_##IDX##_update_obj, 3, mod_eoslib_db_##IDX##_update); \
      STATIC mp_obj_t mod_eoslib_db_##IDX##_remove(size_t n_args, const mp_obj_t *args) { \
         uint64_t iterator = mp_obj_get_uint(args[0]); \
         api.db_##IDX##_remove(iterator); \
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
         int itr = api.db_##IDX##_find_secondary( code, scope, table, (const char*)secondary , sizeof(ARR_ELEMENT_TYPE)*ARR_SIZE, &primary ); \
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
         int itr = api.db_##IDX##_find_primary( code, scope, table, (char*)&secondary , sizeof(ARR_ELEMENT_TYPE)*ARR_SIZE, primary ); \
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
         int itr = api.db_##IDX##_lowerbound( code, scope, table, (char*)&secondary , sizeof(ARR_ELEMENT_TYPE)*ARR_SIZE, &primary ); \
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
         int itr = api.db_##IDX##_upperbound( code, scope, table, (char*)&secondary , sizeof(secondary), &primary ); \
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
         int itr = api.db_##IDX##_end( code, scope, table ); \
         return mp_obj_new_int(itr); \
      } \
      STATIC MP_DEFINE_CONST_FUN_OBJ_VAR(mod_eoslib_db_##IDX##_end_obj, 3, mod_eoslib_db_##IDX##_end); \
      \
      STATIC mp_obj_t mod_eoslib_db_##IDX##_next(size_t n_args, const mp_obj_t *args) { \
         uint64_t primary = 0;\
         uint64_t iterator = mp_obj_get_uint(args[0]); \
         int itr = api.db_##IDX##_next(iterator, &primary); \
         mp_obj_tuple_t *tuple = mp_obj_new_tuple(2, NULL); \
         tuple->items[0] = mp_obj_new_int(itr); \
         tuple->items[1] = mp_obj_new_int(primary); \
         return tuple; \
      } \
      STATIC MP_DEFINE_CONST_FUN_OBJ_VAR(mod_eoslib_db_##IDX##_next_obj, 1, mod_eoslib_db_##IDX##_next); \
      STATIC mp_obj_t mod_eoslib_db_##IDX##_previous(size_t n_args, const mp_obj_t *args) { \
         uint64_t primary = 0;\
         uint64_t iterator = mp_obj_get_uint(args[0]); \
         int itr = api.db_##IDX##_previous(iterator, &primary); \
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

STATIC mp_obj_t mod_eoslib_send_inline(size_t n_args, const mp_obj_t *args) {
   struct mp_action mp_act;

   mp_act.account = mp_obj_get_uint(args[0]);
   mp_act.name    = mp_obj_get_uint(args[1]);
   mp_act.auth = (uint64_t*)mp_obj_str_get_data(args[2], &mp_act.auth_len);
   assert((mp_act.auth_len % (sizeof(uint64_t)*2)) == 0);

   mp_act.data = (unsigned char*)mp_obj_str_get_data(args[3], &mp_act.data_len);

   api.send_inline(&mp_act);
   return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR(mod_eoslib_send_inline_obj, 3, mod_eoslib_send_inline);

STATIC mp_obj_t mod_eoslib_send_context_free_inline(size_t n_args, const mp_obj_t *args) {
   struct mp_action mp_act;

   mp_act.account = mp_obj_get_uint(args[0]);
   mp_act.name    = mp_obj_get_uint(args[1]);

   if (args[2] != mp_const_none) {
      mp_act.auth = (uint64_t*)mp_obj_str_get_data(args[2], &mp_act.auth_len);
      assert((mp_act.auth_len % (sizeof(uint64_t)*2)) == 0);
      mp_act.auth_len /= sizeof(uint64_t);
   }

   mp_act.data = (unsigned char*)mp_obj_str_get_data(args[3], &mp_act.data_len);

   api.send_context_free_inline(&mp_act);
   return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR(mod_eoslib_send_context_free_inline_obj, 3, mod_eoslib_send_context_free_inline);

STATIC mp_obj_t mod_eoslib_send_deferred(size_t n_args, const mp_obj_t *args) {
   struct mp_transaction trx;
   uint128_t sender_id;
   uint64_t payer;
   size_t sender_id_len;

   memset(&trx, 0, sizeof(trx));

   const char* id = mp_obj_str_get_data(args[0], &sender_id_len);
   assert(sender_id_len == 16);
   sender_id = *((uint128_t*)id);

   payer = mp_obj_get_uint(args[1]);

   trx.expiration             = mp_obj_get_uint(args[2]);
   trx.region                 = mp_obj_get_uint(args[3]);
   trx.ref_block_num          = mp_obj_get_uint(args[4]);
   trx.ref_block_prefix       = mp_obj_get_uint(args[5]);
   trx.max_net_usage_words    = mp_obj_get_uint(args[6]);
   trx.max_kcpu_usage         = mp_obj_get_uint(args[7]);
   trx.delay_sec              = mp_obj_get_uint(args[8]);

   mp_obj_t* context_free_actions_obj;
   mp_obj_t* actions_obj;

   if (args[9] != mp_const_none) {
      mp_obj_get_array(args[9], &trx.free_actions_len, &context_free_actions_obj);
      if (trx.free_actions_len > 0) {
         trx.context_free_actions = (struct mp_action**)calloc(trx.free_actions_len, sizeof(struct mp_action**));

         for (int i=0;i<trx.free_actions_len;i++) {
            mp_obj_t* action_obj;
            size_t action_size;
            mp_obj_get_array(context_free_actions_obj[i], &action_size, &action_obj);
            assert(action_size == 4);

            struct mp_action* mp_act = (struct mp_action*)calloc(1, sizeof(struct mp_action));

            mp_act->account = mp_obj_get_uint(action_obj[0]);
            mp_act->name    = mp_obj_get_uint(action_obj[1]);

            if (action_obj[2] != mp_const_none) {
               mp_act->auth = (uint64_t*)mp_obj_str_get_data(action_obj[2], &mp_act->auth_len);
               assert((mp_act->auth_len % (sizeof(uint64_t)*2)) == 0);
               mp_act->auth_len /= sizeof(uint64_t);
            }

            mp_act->data = (unsigned char*)mp_obj_str_get_data(action_obj[3], &mp_act->data_len);

            trx.context_free_actions[i] = mp_act;
         }
      }
   }

   if (args[10] != mp_const_none) {
      mp_obj_get_array(args[10], &trx.actions_len, &actions_obj);
      if (trx.actions_len > 0) {
         trx.actions = (struct mp_action**)calloc(trx.actions_len, sizeof(struct mp_action**));

         for (int i=0;i<trx.actions_len;i++) {
            mp_obj_t* action_obj;
            size_t action_size;
            mp_obj_get_array(actions_obj[i], &action_size, &action_obj);
            assert(action_size == 4);

            struct mp_action* mp_act = (struct mp_action*)calloc(1, sizeof(struct mp_action));

            mp_act->account = mp_obj_get_uint(action_obj[0]);
            mp_act->name    = mp_obj_get_uint(action_obj[1]);

            if (action_obj[2] != mp_const_none) {
               mp_act->auth = (uint64_t*)mp_obj_str_get_data(action_obj[2], &mp_act->auth_len);
               assert((mp_act->auth_len % (sizeof(uint64_t)*2)) == 0); // 8 bytes alignment
               mp_act->auth_len /= sizeof(uint64_t);
            }

            mp_act->data = (unsigned char*)mp_obj_str_get_data(action_obj[3], &mp_act->data_len);

            trx.actions[i] = mp_act;
         }
      }
   }

   api.send_deferred( sender_id, payer, &trx );


   if (trx.context_free_actions) {
      if (trx.free_actions_len > 0) {
         for (int i=0;i<trx.free_actions_len;i++) {
            free(trx.context_free_actions[i]);
         }
      }
      free(trx.context_free_actions);
   }

   if (trx.actions) {
      if (trx.actions_len > 0) {
         for (int i=0;i<trx.actions_len;i++) {
            free(trx.actions[i]);
         }
      }
      free(trx.actions);
   }

   return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR(mod_eoslib_send_deferred_obj, 11, mod_eoslib_send_deferred);


STATIC mp_obj_t mod_eoslib_cancel_deferred(size_t n_args, const mp_obj_t *args) {

   uint128_t sender_id;
   size_t sender_id_len;
   const char* id = mp_obj_str_get_data(args[0], &sender_id_len);
   assert(sender_id_len == 16);
   memcpy(&sender_id, id, sender_id_len);

   api.cancel_deferred( sender_id );

   return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR(mod_eoslib_cancel_deferred_obj, 1, mod_eoslib_cancel_deferred);



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
    { MP_ROM_QSTR(MP_QSTR_db_get_i64_ex), MP_ROM_PTR(&mod_eoslib_db_get_i64_ex_obj) },
	 { MP_ROM_QSTR(MP_QSTR_db_previous_i64), MP_ROM_PTR(&mod_eoslib_db_previous_i64_obj) },
	 { MP_ROM_QSTR(MP_QSTR_db_next_i64), MP_ROM_PTR(&mod_eoslib_db_next_i64_obj) },
	 { MP_ROM_QSTR(MP_QSTR_db_find_i64), MP_ROM_PTR(&mod_eoslib_db_find_i64_obj) },
	 { MP_ROM_QSTR(MP_QSTR_db_lowerbound_i64), MP_ROM_PTR(&mod_eoslib_db_lowerbound_i64_obj) },
	 { MP_ROM_QSTR(MP_QSTR_db_upperbound_i64), MP_ROM_PTR(&mod_eoslib_db_upperbound_i64_obj) },
	 { MP_ROM_QSTR(MP_QSTR_db_end_i64), MP_ROM_PTR(&mod_eoslib_db_end_i64_obj) },
	 { MP_ROM_QSTR(MP_QSTR_hash64), MP_ROM_PTR(&mod_eoslib_hash64_obj) },

    { MP_ROM_QSTR(MP_QSTR_send_inline), MP_ROM_PTR(&mod_eoslib_send_inline_obj) },
    { MP_ROM_QSTR(MP_QSTR_send_context_free_inline), MP_ROM_PTR(&mod_eoslib_send_context_free_inline_obj) },
    { MP_ROM_QSTR(MP_QSTR_send_deferred), MP_ROM_PTR(&mod_eoslib_send_deferred_obj) },

    { MP_ROM_QSTR(MP_QSTR_cancel_deferred), MP_ROM_PTR(&mod_eoslib_cancel_deferred_obj) },

    { MP_ROM_QSTR(MP_QSTR_eosio_delay), MP_ROM_PTR(&mod_eoslib_eosio_delay_obj) },

	 { MP_ROM_QSTR(MP_QSTR_S), MP_ROM_PTR(&mod_eoslib_S_obj) },
};

STATIC MP_DEFINE_CONST_DICT(mp_module_eoslib_globals, mp_module_eoslib_globals_table);

const mp_obj_module_t mp_module_eoslib = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t*)&mp_module_eoslib_globals,
};

#endif //MICROPY_PY_EOSLIB
