#cmake_minimum_required(VERSION 3.9)

file(GLOB HEADERS "py/*.h")



add_custom_command(
    OUTPUT ${CMAKE_CURRENT_SOURCE_DIR}/ports/unix/build/libaxtls.a
    COMMAND make axtls
    COMMENT "build axtls "
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/ports/unix
#    DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/ports/unix/mpconfigport.h
)

add_custom_command(
    OUTPUT ${CMAKE_CURRENT_SOURCE_DIR}/ports/unix/build/frozen.c 
    OUTPUT ${CMAKE_CURRENT_SOURCE_DIR}/ports/unix/build/frozen_mpy.c
    OUTPUT ${CMAKE_CURRENT_SOURCE_DIR}/ports/unix/build/genhdr/qstrdefs.collected.h
    COMMAND make lib
#    COMMAND touch ${CMAKE_CURRENT_SOURCE_DIR}/ports/unix/build/frozen.c
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/ports/unix
    COMMENT "build vm_py"
    DEPENDS  ${CMAKE_CURRENT_SOURCE_DIR}/ports/unix/build/libaxtls.a ${CMAKE_CURRENT_SOURCE_DIR}/extmod/eoslib/modeoslib.c ${CMAKE_CURRENT_SOURCE_DIR}/ports/unix/mpconfigport.h
)

set (SRC_FILES ${CMAKE_CURRENT_SOURCE_DIR}/ports/unix/build/frozen.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ports/unix/build/frozen_mpy.c

#ports/unix/build/py/emitnx64.o
#ports/unix/mpthreadport.c
#ports/unix/alloc.c
#ports/unix/coverage.c
#            py/nlrx86.c
#            py/nlrthumb.c
#            py/nlrxtensa.c
#            py/nlrsetjmp.c
#    py/pystack.c
#    py/asmbase.c
#    py/asmx64.c
#    py/emitnative.c
#    py/asmthumb.c
#    py/emitinlinethumb.c
#    py/asmarm.c
#    py/asmxtensa.c
#    py/emitinlinextensa.c
#    py/formatfloat.c
#    py/nativeglue.c
#    py/objcomplex.c
#    py/objfloat.c
#    py/objint_longlong.c
#    py/builtinhelp.c
#    py/modmath.c
#    py/modcmath.c
#    py/modthread.c
#extmod/modussl_axtls.c
#extmod/modussl_mbedtls.c
#extmod/moduselect.c
##extmod/modwebrepl.c
#extmod/uos_dupterm.c
#extmod/modframebuf.c
#extmod/rai/modrailib.c

    py/mp_compiler.c
    extmod/eoslib/modeoslib.c
    extmod/eoslib/frozenmod.cpp
    extmod/crypto-algorithms/xxhash.c
    ports/unix/gccollect.c
    ports/unix/unix_mphal.c
    ports/unix/input.c
    ports/unix/file.c
    ports/unix/modmachine.c
    ports/unix/modos.c
    ports/unix/modtime.c
    ports/unix/moduselect.c
    
    py/nlrx64.c
    py/nlr.c
    py/malloc.c
    py/gc.c
    py/qstr.c
    py/vstr.c
    py/mpprint.c
    py/unicode.c
    py/mpz.c
    py/reader.c
    py/lexer.c
    py/parse.c
    py/scope.c
    py/compile.c
    py/emitcommon.c
    py/emitbc.c
    py/parsenumbase.c
    py/parsenum.c
    py/emitglue.c
    py/persistentcode.c
    py/runtime.c
    py/runtime_utils.c
    py/scheduler.c
    py/stackctrl.c
    py/argcheck.c
    py/warning.c
    py/map.c
    py/obj.c
    py/objarray.c
    py/objattrtuple.c
    py/objbool.c
    py/objboundmeth.c
    py/objcell.c
    py/objclosure.c
    py/objdict.c
    py/objenumerate.c
    py/objexcept.c
    py/objfilter.c
    py/objfun.c
    py/objgenerator.c
    py/objgetitemiter.c
    py/objint.c
#    py/objint_mpz.c
    py/objint_longlong.c
    py/objlist.c
    py/objmap.c
    py/objmodule.c
    py/objobject.c
    py/objpolyiter.c
    py/objproperty.c
    py/objnone.c
    py/objnamedtuple.c
    py/objrange.c
    py/objreversed.c
    py/objset.c
    py/objsingleton.c
    py/objslice.c
    py/objstr.c
    py/objstrunicode.c
    py/objstringio.c
    py/objtuple.c
    py/objtype.c
    py/objzip.c
    py/opmethods.c
    py/sequence.c
    py/stream.c
    py/binary.c
    py/builtinimport.c
    py/builtinevex.c
    py/modarray.c
    py/modbuiltins.c
    py/modcollections.c
    py/modgc.c
    py/modio.c
    py/modmicropython.c
    py/modstruct.c
    py/modsys.c
    py/moduerrno.c
    py/vm.c
    py/bc.c
    py/showbc.c
    py/repl.c
    py/smallint.c
#    py/frozenmod.c
    extmod/moductypes.c
    extmod/modujson.c
    extmod/modure.c
    extmod/moduzlib.c
    extmod/moduheapq.c
    extmod/modutimeq.c
    extmod/moduhashlib.c
    extmod/modubinascii.c
    extmod/virtpin.c
    extmod/machine_mem.c
    extmod/machine_pinbase.c
    extmod/machine_signal.c
    extmod/machine_pulse.c
#    extmod/machine_i2c.c
#    extmod/machine_spi.c
    extmod/modurandom.c
#    extmod/modwebsocket.c
    extmod/utime_mphal.c
    lib/embed/abort_.c
    lib/utils/printf.c
    ports/unix/build/frozen.c
    ports/unix/build/genhdr/qstrdefs.collected.h
    ${HEADERS}
    vm_api.cpp
)

set (INCLUDE_DIRS   PRIVATE ${CMAKE_SOURCE_DIR}/contracts
                    PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/py
                    PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/extmod
                    PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/extmod/crypto-algorithms
                    PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/ports/unix
                    PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/ports/unix/build
                    PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

#vm_py1
add_library( vm_py STATIC  ${SRC_FILES} )
if (APPLE)
    target_link_libraries( vm_py PUBLIC  intl dl util)
else()
    target_compile_options(vm_py PRIVATE -DSHARED_MICROPYTHON  -fPIC)
    target_link_libraries( vm_py PUBLIC dl util)
endif()
target_include_directories( vm_py ${INCLUDE_DIRS} )

foreach(LIBINDEX RANGE 1 10 1)
    add_library( vm_py-${LIBINDEX} SHARED ports/unix/main_eos.c py/mpstate.c )
    if (APPLE)
        target_link_libraries( vm_py-${LIBINDEX} PUBLIC  intl dl util PRIVATE vm_py )
    else()
        target_compile_options(vm_py-${LIBINDEX} PRIVATE -DSHARED_MICROPYTHON)
        target_link_libraries( vm_py-${LIBINDEX} PUBLIC dl util PRIVATE vm_py )
    endif()
    target_include_directories( vm_py-${LIBINDEX} ${INCLUDE_DIRS} )

    INSTALL( TARGETS
       vm_py-${LIBINDEX}
    
       RUNTIME DESTINATION bin
       LIBRARY DESTINATION lib
       ARCHIVE DESTINATION lib
    )

endforeach(LIBINDEX)




