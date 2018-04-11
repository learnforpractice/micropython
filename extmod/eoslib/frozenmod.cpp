#include <string>
#include <vector>

#include "modeoslib.h"
#include "extmod/crypto-algorithms/xxhash.h"

extern "C" {
#include "py/frozenmod.h"
#include "py/emitglue.h"
#include "py/persistentcode.h"
}



/*
int db_store_i64( uint64_t scope, uint64_t table, uint64_t payer, uint64_t id, const char* buffer, size_t buffer_size );
void db_update_i64( int itr, uint64_t payer, const char* buffer, size_t buffer_size );
void db_remove_i64( int itr );
int db_get_i64( int itr, char* buffer, size_t buffer_size );
int db_next_i64( int itr, uint64_t* primary );
int db_previous_i64( int itr, uint64_t* primary );
int db_find_i64( uint64_t code, uint64_t scope, uint64_t table, uint64_t id );
int db_lowerbound_i64( uint64_t code, uint64_t scope, uint64_t table, uint64_t id );
int db_upperbound_i64( uint64_t code, uint64_t scope, uint64_t table, uint64_t id );
int db_end_i64( uint64_t code, uint64_t scope, uint64_t table );
*/

/*
static int s_debug_mode = 0;
void set_debug_mode(int mode) {
   s_debug_mode = mode;
}

int get_debug_mode() {
   return s_debug_mode;
}
*/

extern "C" struct eosapi* mp_get_eosapi();

extern "C" {

static char code_data[1024*64];

int mp_find_frozen_module(const char *mod_name, size_t len, void **data) {
//   ilog("+++++++++mod_name: ${n}", ("n", mod_name));
   char path1[128];
   char path2[128];
   memset(path1, 0, sizeof(path1));
   memset(path2, 0, sizeof(path2));

   int ret = MP_FROZEN_NONE;
   /*
   if (get_debug_mode()) {
      return MP_FROZEN_NONE;
   }
   */

   size_t path_num = mp_get_eosapi()->split_path(mod_name, path1, sizeof(path1)-1, path2, sizeof(path2)-1);
//   ilog("${n}", ("n", _path.size()));

   uint64_t code;
   uint64_t id;

   if (path_num == 1) {
      code = mp_get_eosapi()->get_action_account();
      id = XXH64(mod_name, len, 0);
   } else if (path_num == 2) {
      code = mp_get_eosapi()->string_to_uint64_(path1);
      id = XXH64(path2, strlen(path2), 0);
//      ilog("+++++++++load code from account: ${n1} ${n2}", ("n1", _path[0])("n2", _path[1]));
   } else {
      return MP_FROZEN_NONE;
   }

   int itr = mp_get_eosapi()->db_find_i64(code, code, code, id);
   if (itr < 0) {
         return MP_FROZEN_NONE;
   }

   int size = mp_get_eosapi()->db_get_i64(itr, code_data, sizeof(code_data));
   mp_get_eosapi()->eosio_assert(size < sizeof(code_data), "source file too large!");
//   ilog("+++++++++code_data: ${n1} ${n2}", ("n1", code_data[0])("n2", size));
   if (size > 0) {
      int code_type = code_data[0];
      if (code_type == 0) {//py
//         wlog("+++++++++load source code");
         qstr qstr_mod_name = qstr_from_str(mod_name);
         mp_lexer_t *lex = mp_lexer_new_from_str_len(qstr_mod_name, &code_data[1], size - 1, 0);
         *data = lex;
         ret = MP_FROZEN_STR;
      } else if (code_type == 1) {//mpy
//         wlog("+++++++++load compiled code");
         mp_raw_code_t *raw_code = mp_raw_code_load_mem((byte *)&code_data[1], size - 1);
         *data = raw_code;
         ret = MP_FROZEN_MPY;
      } else {
         assert(false);
      }
   }
   return ret;

}

const char *mp_find_frozen_str(const char *str, size_t *len) {
   mp_get_eosapi()->eosio_assert(false, "not implemented!");
}

mp_import_stat_t mp_frozen_stat(const char *mod_name) {
//   ilog("+++++++++mod_name: ${n}", ("n",mod_name));
/*
   if (get_debug_mode()) {
      return MP_IMPORT_STAT_NO_EXIST;
   }
*/
   char path1[128];
   char path2[128];
   memset(path1, 0, sizeof(path1));
   memset(path2, 0, sizeof(path2));

   uint64_t code;
   uint64_t id;

   size_t path_num = mp_get_eosapi()->split_path(mod_name, path1, sizeof(path1) - 1, path2, sizeof(path2) - 1);
//   ilog("${n}", ("n", _dirs.size()));

   if (path_num == 1) {
      uint64_t _account = mp_get_eosapi()->string_to_uint64_(path1);
//      ilog("+++++++++account: ${n1}", ("n1", _account));

      if (mp_get_eosapi()->is_account(_account)) {
//         ilog("+++++++++importing module from a account: ${n1}", ("n1", mod_name));
         return MP_IMPORT_STAT_DIR;
      }

      code = mp_get_eosapi()->get_action_account();
      id = XXH64(path1, strlen(path1), 0);

   } else if (path_num == 2) {
      code = mp_get_eosapi()->string_to_uint64_(path1);
      id = XXH64(path2, strlen(path2), 0);
//      ilog("+++++++++load code from account: ${n1} ${n2}", ("n1", _dirs[0])("n2", _dirs[1]));
   } else {
      return MP_IMPORT_STAT_NO_EXIST;
   }

   mp_import_stat_t ret;

   int itr = mp_get_eosapi()->db_find_i64(code, code, code, id);
   if (itr < 0) {
         ret = MP_IMPORT_STAT_NO_EXIST;
   } else {
      ret = MP_IMPORT_STAT_FILE;
   }
//   ilog("+++++++++mod_name: ${n1}, code: ${n2}, ret: ${n3}", ("n1", mod_name)("n2", eosio::chain::name(code).to_string())("n3", ret));
   return ret;
}

}

