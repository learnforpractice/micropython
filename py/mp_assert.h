
#ifndef LIBRARIES_MICROPYTHON_PY_MP_ASSERT_H_
#define LIBRARIES_MICROPYTHON_PY_MP_ASSERT_H_

//modeoslib.c
void mp_assert_(int cond, char* str);

#ifdef assert
#undef assert
#endif

#define  assert(expression) { \
   if (!(expression)) { \
      mp_assert_(0, #expression); \
   } \
}


#endif /* LIBRARIES_MICROPYTHON_PY_MP_ASSERT_H_ */
