#include <eosiolib_native/vm_api.h>

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include <string>
#include <vector>

#include "extmod/crypto-algorithms/xxhash.h"

extern "C" {
#include "py/frozenmod.h"
#include "py/emitglue.h"
#include "py/persistentcode.h"
}


extern "C" struct eosapi* mp_get_eosapi();

extern "C" {

int mp_find_frozen_module(const char *mod_name, size_t len, void **data) {

   char path1[128];
   char path2[128];
   memset(path1, 0, sizeof(path1));
   memset(path2, 0, sizeof(path2));

   int ret = MP_FROZEN_NONE;

   size_t path_num = get_vm_api()->split_path(mod_name, path1, sizeof(path1)-1, path2, sizeof(path2)-1);
//   ilog("${n}", ("n", _path.size()));

   uint64_t code;
   uint64_t id;

   if (path_num == 1) {
      code = get_vm_api()->get_action_account();
      id = XXH64(mod_name, len, 0);
   } else if (path_num == 2) {
      code = get_vm_api()->string_to_uint64(path1);
      id = XXH64(path2, strlen(path2), 0);
      printf("+++++++++mp_find_frozen_module from account:%s %s\n", path1, path2);
      //      ilog("+++++++++load code from account: ${n1} ${n2}", ("n1", _path[0])("n2", _path[1]));
   } else {
      return MP_FROZEN_NONE;
   }

   int itr = get_vm_api()->db_find_i64(code, code, code, id);
   if (itr < 0) {
      printf("+++++++mp_find_frozen_module not found!\n");
         return MP_FROZEN_NONE;
   }

//   printf("+++++++mp_find_frozen_module, found!\n");

   size_t size = 0;
   const char *code_data = get_vm_api()->db_get_i64_exex(itr, &size);

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
         get_vm_api()->eosio_assert(false, "unknown code!");
      }
   }
   return ret;
}

const char *mp_find_frozen_str(const char *str, size_t *len) {
   get_vm_api()->eosio_assert(false, "not implemented!");
   return 0;
}

mp_import_stat_t mp_frozen_stat(const char *mod_name) {

   char path1[128];
   char path2[128];
   memset(path1, 0, sizeof(path1));
   memset(path2, 0, sizeof(path2));
    if (strcmp("genescienceinterface.mpy", mod_name) == 0) {
        mod_name = mod_name;
    }

   uint64_t code;
   uint64_t id;
   printf("+++++++++mp_frozen_stat, mod_name: %s \n", mod_name);
   size_t path_num = get_vm_api()->split_path(mod_name, path1, sizeof(path1) - 1, path2, sizeof(path2) - 1);
//   ilog("${n}", ("n", _dirs.size()));

   if (path_num == 1) {
      uint64_t _account = get_vm_api()->string_to_uint64(path1);
//      ilog("+++++++++account: ${n1}", ("n1", _account));

      if (get_vm_api()->is_account(_account)) {
//         ilog("+++++++++importing module from a account: ${n1}", ("n1", mod_name));
         return MP_IMPORT_STAT_DIR;
      }

      code = get_vm_api()->get_action_account();
      id = XXH64(path1, strlen(path1), 0);

   } else if (path_num == 2) {
      code = get_vm_api()->string_to_uint64(path1);
      id = XXH64(path2, strlen(path2), 0);
      printf("+++++++++mp_frozen_stat: account:%s %s\n", path1, path2);
   } else {
      return MP_IMPORT_STAT_NO_EXIST;
   }

   mp_import_stat_t ret;

   int itr = get_vm_api()->db_find_i64(code, code, code, id);
   if (itr < 0) {
//      printf("mp_get_eosapi()->db_find_i64(code, code, code, id); not found\n");
      ret = MP_IMPORT_STAT_NO_EXIST;
   } else {
      ret = MP_IMPORT_STAT_FILE;
   }
//   ilog("+++++++++mod_name: ${n1}, code: ${n2}, ret: ${n3}", ("n1", mod_name)("n2", eosio::chain::name(code).to_string())("n3", ret));
   return ret;
}

}

