#include "vm_py.hpp"
#include "vm_py_api.h"

#include "xxhash.h"

using namespace eosio;
static struct vm_api s_api;

extern "C" {
mp_obj_t micropy_load_from_py(const char *mod_name, const char *data, size_t len);
mp_obj_t micropy_load_from_mpy(const char *mod_name, const char *data, size_t len, mp_raw_code_t** raw_code);

mp_obj_t micropy_call_3(mp_obj_t module_obj, mp_raw_code_t * raw_code, const char *func, uint64_t receiver, uint64_t code, uint64_t type);

//defined in vm.c
void execution_start();
void execution_end();
void set_max_execution_time(int time);
uint64_t get_execution_time();

//main_eos.c
void* execute_from_str(const char *str);
int main_micropython(int argc, char **argv);

int compile_and_save_to_buffer(const char* src_name, const char *src_buffer, size_t src_size, char* buffer, size_t size);
//mpprint.c
void set_printer(fn_printer _printer);
}


const char * init_mp = "" \
"libs = ('asset.py', 'token.py', 'cache.py', 'garden.py', 'solidity.py', 'storage.py')\n" \
"for lib in libs:\n" \
"    mod = 'backyard.'+lib[:-3]\n" \
"    print(mod)\n"
"    __import__(mod)\n"
;

static int _init = 0;

extern "C" int is_mp_init_finished() {
   return _init;
}

void init() {
   if (_init) {
      return;
   }

   printf("Initialize common library.\n");

   uint64_t hash = XXH64("storage.mpy", strlen("storage.mpy"), 0);

   int itr = get_vm_api()->rodb_find_i64(N(backyard), N(backyard), N(backyard), hash);
   if (itr < 0) {
      return;
   }
   printf("Execute script\n");
   execute_from_str(init_mp);
   _init = 1;
}

vm_py::vm_py() {

}

vm_py& vm_py::get() {
   static vm_py *vm = nullptr;
   if (vm == nullptr) {
      vm = new vm_py();
   }
   return *vm;
}

void vm_py::setcode(uint64_t _account) {
   checksum512 hash;
   size_t size = 0;
   const char* code = get_vm_api()->get_code(_account, &size);
   if (size <= 10) {//avoid access to invalid memory
      return;
   }
   init();

   get_vm_api()->sha512( (char*)code, size, &hash );

   auto _itr = pymodules.find(_account);

   if (_itr != pymodules.end()) {
      if (memcmp(&_itr->second->hash, &hash, sizeof(hash)) == 0) {
         return;
      }
   }
//   ilog("++++++++++update code ${n1}", ("n1", name(_account).to_string()));
   mp_obj_t obj = nullptr;
   mp_raw_code_t* raw_code = nullptr;
   execution_start();
   if (code[0] == 0) {//py
      obj = micropy_load_from_py(name{_account}.to_string().c_str(), (const char*)&code[1], size-1);
   } else if (code[0] == 1) {//mpy
      obj = micropy_load_from_mpy(name{_account}.to_string().c_str(), (const char*)&code[1], size-1, &raw_code);
   } else {
      execution_end();
      get_vm_api()->eosio_assert(false, "unknown micropython code!");
   }

   uint64_t execution_time = get_execution_time();
   if (execution_time > 1000) {
      printf("+++++++load module %s, cost: %llu", name{_account}.to_string().c_str(), execution_time);
   }

   execution_end();

   if (obj == NULL) {
      get_vm_api()->eosio_assert(false, "load micropython code failed!");
   } else {
      if (_itr != pymodules.end()) {
         delete _itr->second;
      }
      py_module* mod = new py_module();
      mod->obj = obj;
      mod->raw_code = raw_code;
      memcpy(&mod->hash, &hash, sizeof(hash));
      pymodules[_account] = mod;
   }
}

void vm_py::apply(uint64_t receiver, uint64_t account, uint64_t act, const char* code, size_t size) {
   if (size <= 10 || code == NULL) {
      return;
   }

   init();
   execution_start();

   try {
      mp_obj_t obj = nullptr;
      mp_raw_code_t* raw_code = nullptr;

      auto itr = pymodules.find(receiver);
      if (itr != pymodules.end()) {
         obj = itr->second->obj;
         raw_code = itr->second->raw_code;
      } else {
         if (code[0] == 0) {//py
            obj = micropy_load_from_py(name{account}.to_string().c_str(), (const char*)&code[1], size-1);
         } else if (code[0] == 1) {//mpy
            obj = micropy_load_from_mpy(name{account}.to_string().c_str(), (const char*)&code[1], size-1, &raw_code);
         } else {
            execution_end();
            get_vm_api()->eosio_assert(false, "unknown micropython code!");
         }

         if (obj != NULL) {
            checksum512 hash;
            get_vm_api()->sha512( (char*)code, size, &hash );

            py_module* mod = new py_module();
            mod->obj = obj;
            mod->raw_code = raw_code;

            memcpy(&mod->hash, &hash, sizeof(hash));
            pymodules[account] = mod;
         }
      }
      mp_obj_t ret = 0;
      if (obj) {
         ret = micropy_call_3(obj, raw_code, "apply", receiver, account, act);
      }
      uint64_t execution_time = get_execution_time();
      if (execution_time > 1000) {
         printf("+++++++call module %s, cost: %llu\n", name{account}.to_string().c_str(), execution_time);
      }
      execution_end();
      get_vm_api()->eosio_assert(ret != 0, "code execution with exception!");
   } catch(...) {
      execution_end();
      throw;
   }
}

void vm_py::apply(uint64_t receiver, uint64_t account, uint64_t act) {
   size_t size = 0;
   const char* src = get_vm_api()->get_code(receiver, &size);
   apply(receiver, account, act, src, size);
}

void register_vm_api(struct vm_api* api) {
   s_api = *api;
}

struct vm_api* get_vm_api() {
   return &s_api;
}

int setcode(uint64_t account) {
//   printf("+++++micropython: setcode\n");
   vm_py::get().setcode(account);
   return 1;
}

int apply(uint64_t receiver, uint64_t account, uint64_t act) {
//   printf("+++++micropython: apply %s %s %s\n", name{receiver}.to_string().c_str(), name{account}.to_string().c_str(), name{act}.to_string().c_str());
   vm_py::get().apply(receiver, account, act);
   return 1;
}

static struct vm_py_api s_vm_py_api;

void vm_init() {
   s_vm_py_api.set_max_execution_time = set_max_execution_time;
   s_vm_py_api.compile_and_save_to_buffer = compile_and_save_to_buffer;
   s_vm_py_api.set_printer = set_printer;

   main_micropython(0, NULL);
}

extern "C" void micropython_finalize();

void vm_deinit() {
   printf("vm_py finalize\n");
   micropython_finalize();
}

extern "C" struct vm_py_api* get_py_vm_api() {
   return &s_vm_py_api;
}

