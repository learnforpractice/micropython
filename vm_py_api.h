/*
 * vm_py_api.h
 *
 *  Created on: Jun 13, 2018
 *      Author: newworld
 */

#ifndef VM_PY_VM_PY_API_H_
#define VM_PY_VM_PY_API_H_

typedef void (*fn_printer)(const char * str, size_t len);

struct vm_py_api {
   void (*set_max_execution_time)(int time);
   int (*compile_and_save_to_buffer)(const char* src_name, const char *src_buffer, size_t src_size, char* buffer, size_t size);
   void (*set_printer)(fn_printer _printer);

};


#endif /* VM_PY_VM_PY_API_H_ */
