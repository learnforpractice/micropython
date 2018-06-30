#include <vector>
#include <string>
#include <map>
#include <memory>

using namespace std;

#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <eosiolib_native/vm_api.h>
#include <eosiolib/types.hpp>

extern "C" {
#include "py/objlist.h"
#include "py/objstringio.h"
#include "py/runtime.h"
#include "py/stream.h"
#include "py/parse.h"
#include "py/emitglue.h"
#include "py/stackctrl.h"
#include "py/gc.h"
}

#ifndef VM_PY_VM_PY_HPP_
#define VM_PY_VM_PY_HPP_

class vm_py {
public:

   static vm_py& get();

   void setcode(uint64_t _account);
   void apply(uint64_t receiver, uint64_t account, uint64_t act, const char* code, size_t size);
   void apply(uint64_t receiver, uint64_t account, uint64_t act);

private:
   struct py_module {
      void* obj;
      mp_raw_code_t* raw_code;
      checksum512 hash;
      mp_state_mem_t mem;
      char heap[1024*64];
   };

   std::map<uint64_t, std::unique_ptr<py_module>> pymodules;

   vm_py();
};

#endif /* VM_PY_VM_PY_HPP_ */
