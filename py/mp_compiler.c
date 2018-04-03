/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2013-2016 Damien P. George
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

#include "py/compile.h"
#include "py/persistentcode.h"
#include "py/runtime.h"
#include "py/gc.h"
#include "py/stackctrl.h"
#ifdef _WIN32
#include "ports/windows/fmode.h"
#endif

#include "mpconfigport.h"

// Command line options, with their defaults
STATIC uint emit_opt = MP_EMIT_OPT_NONE;


//static const mp_print_t mp_stderr_print = {NULL, stderr_print_strn};


STATIC void stderr_print_strn(void *env, const char *str, size_t len) {
    (void)env;
    ssize_t dummy = write(STDERR_FILENO, str, len);
    mp_uos_dupterm_tx_strn(str, len);
    (void)dummy;
}

struct buffer {
	int size;
	int pos;
	char *buffer;
};

STATIC void copy_to_buffer(void *env, const char *str, size_t len) {
	struct buffer* buf = (struct buffer *)env;
   assert(buf->size >= buf->pos+len);
	memcpy(buf->buffer+buf->pos, str, len);
    buf->pos += len;
}

int mp_raw_code_save_to_buffer(mp_raw_code_t *rc, char* mpy_code, size_t size) {
	struct buffer buf;
	memset(&buf, 0, sizeof(buf));
	buf.size = size;
	buf.pos = 0;
	buf.buffer = mpy_code;
	mp_print_t fd_print = {(void*)&buf, copy_to_buffer};
	mp_raw_code_save(rc, &fd_print);
	return buf.pos;
}

//mp_lexer_t *mp_lexer_new_from_str_len(qstr src_name, const char *str, size_t len, size_t free_len)
int compile_and_save_to_buffer(const char* src_name, const char *src_buffer, size_t src_size, char* buffer, size_t size) {
	nlr_buf_t nlr;
    if (nlr_push(&nlr) == 0) {
//        mp_lexer_t *lex = mp_lexer_new_from_file(file);

        mp_lexer_t *lex = mp_lexer_new_from_str_len(qstr_from_str(src_name), src_buffer, src_size, false);


        qstr source_name;
        source_name = lex->source_name;

        mp_parse_tree_t parse_tree = mp_parse(lex, MP_PARSE_FILE_INPUT);
        mp_raw_code_t *rc = mp_compile_to_raw_code(&parse_tree, source_name, emit_opt, false);

        int length = mp_raw_code_save_to_buffer(rc, buffer, size);

        nlr_pop();
        return length;
    } else {
        // uncaught exception
        mp_obj_print_exception(&mp_stderr_print, (mp_obj_t)nlr.ret_val);
        return 0;
    }
}

