#ifndef __MODEOSLIB_H_
#define __MODEOSLIB_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdlib.h>
#include "../../../chain/micropython/mpeoslib.h"
#include "xxhash.h"
#include "py/obj.h"
#include "py/objstr.h"
#include "py/lexer.h"
#include "py/compile.h"

int py_is_debug_mode();
void py_set_debug_mode(int mode);
#ifdef __cplusplus
}
#endif

#endif
