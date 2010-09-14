#include "nori.h"

#include "testing.h"
#include <unistd.h>
#include <stdio.h>
#include <math.h>
#include <time.h>
#include <stdarg.h>
#include <sys/times.h>
#include <ffi.h>
 
/** Some conclusions from tests with libffi, or wrapping a call in a 
  variant / union type: 

  * Wrapping in a variant type is 2x slower than a direct call to C functions
  for a mostly do-nothing function.  
  * Using libffi calls is 10x to 20x slower than a direct call to C functions
  for a mostly do-nothing function.
  * Wrapping in a variant type with more arguments is up to 7x slower than a direct call to C functions for a sum function.  
  * Libffi  more arguments is up to 39x slower than a direct call to C functions for a sum function.  
  * Once I/O or other overhead gets involved, wrapped calls are 1.2x to 1.3x slower, and ffi calls are 2x to 2.1x slower.
  
  Conclusions: Libffi is slower than manual wrapping by a considerable margin. Both approaches slow down when more arguments are passed, more or less linearly with the amount of arguments passed. But even then FFI stays 
  the slowest.  

*/

#define ARRAY_SIZE 123

typedef void * vptr;
typedef vptr (funcptr)();
 
int do_nothing(int i) {
  return i;
} 
 
int sum(int i1, int i2) {
  return i1 + i2;
} 

struct Varia_;
typedef struct Varia_ Varia;


#define VARIA_NIL     0
#define VARIA_FALSE   1
#define VARIA_TRUE    2
#define VARIA_INT     3
#define VARIA_DOUBLE  4
#define VARIA_POINTER 5
#define VARIA_GC_FLAGS 7

struct Varia_ {
  int type;
  int flags; //simulated flags that would beused for GC, etc in the real McCoy.
  union {
   int32_t  i;
   double   d;
   void * ptr;
  } data;   
}; 

typedef Varia (VarFunc)(Varia firtstarg, ...);

Varia wrap_do_nothing(Varia arg) {
  Varia result;
  result.type   = VARIA_INT;
  result.flags  = VARIA_GC_FLAGS;
  result.data.i = do_nothing(arg.data.i);
  return result;
}

Varia wrap_sum(Varia arg1, ...) {
  va_list ap;
  Varia result, arg2;
  result.type   = VARIA_INT;
  result.flags  = VARIA_GC_FLAGS;
  va_start(ap, arg1); 
  arg2 = va_arg(ap, Varia);  
  result.data.i = sum(arg1.data.i, arg2.data.i);
  va_end(ap);
  return result;
}


#define TRIES 10000000

TEST_FUNC(nori) {
  ffi_cif cif;
  ffi_type *args[2];
  Varia   vararg, vararg2;
  VarFunc * varfunc = wrap_do_nothing;
  VarFunc * varsum  = wrap_sum;
  void *values[2];
  int s;
  int rc;
  clock_t   start, stop, delta; 
  double d1 = 1.0, d2 = 1.0;
  int i, j;
  start = clock();  
  for(i = 0; i < TRIES; i++) {
    j = sum(i, i);
  }
  stop  = clock();
  delta = stop - start;
  d1    = ((double) delta) / ((double)CLOCKS_PER_SEC);
  fprintf(stderr, "Direct call: do_nothing %d times: %d (%lf s) %d\n", TRIES, delta, d1, j);
    
  start = clock();  
  for(i = 0; i < TRIES; i++) {
    Varia result;    
    vararg.type   = VARIA_INT;
    vararg.flags  = VARIA_GC_FLAGS;
    vararg.data.i = i;    
    result        = wrap_do_nothing(vararg);
    if (result.type == VARIA_INT) { 
      j           = result.data.i;
    }
  }
  stop  = clock();
  delta = stop - start;
  d1    = ((double) delta) / ((double)CLOCKS_PER_SEC);
  fprintf(stderr, "Wrapped call: do_nothing %d times: %d (%lf s) %d\n", TRIES, delta, d1, j);
  
  start = clock();  
  for(i = 0; i < TRIES; i++) {
    Varia result;
    vararg.type   = VARIA_INT;
    vararg.flags  = VARIA_GC_FLAGS;
    vararg.data.i = i;
    result        = varfunc(vararg);
    if (result.type == VARIA_INT) { 
      j           = result.data.i;
    }
  }
  stop  = clock();
  delta = stop - start;
  d1    = ((double) delta) / ((double)CLOCKS_PER_SEC);
  fprintf(stderr, "Wrapped ptr call: do_nothing %d times: %d (%lf s) %d\n", TRIES, delta, d1, j);

  start = clock();  
  for(i = 0; i < TRIES; i++) {
    Varia result;
    vararg.type    = VARIA_INT;
    vararg.flags   = VARIA_GC_FLAGS;
    vararg.data.i  = i;
    vararg2.type   = VARIA_INT;
    vararg2.flags  = VARIA_GC_FLAGS;
    vararg2.data.i = i;
    result         = varsum(vararg, vararg2);
    if (result.type == VARIA_INT) { 
      j            = result.data.i;
    }
  }
  stop  = clock();
  delta = stop - start;
  d1    = ((double) delta) / ((double)CLOCKS_PER_SEC);
  fprintf(stderr, "Wrapped sum call: %d times: %d (%lf s) %d\n", TRIES, delta, d1, j);

    
  /* Initialize the argument info vectors */
  args[0]   = &ffi_type_sint;
  values[0] = &i;
  args[1]   = &ffi_type_sint;
  values[1] = &i;
  /* Initialize the cif */
  ffi_prep_cif(&cif, FFI_DEFAULT_ABI, 2, &ffi_type_sint, args);
  start = clock();  
  for(i = 0; i < TRIES; i++) {
    // s = i;
    ffi_call(&cif, sum, &j, values);
  }
  stop  = clock();
  delta = stop - start;
  d1    = ((double) delta) / ((double)CLOCKS_PER_SEC);
  fprintf(stderr, "FFI call: %d times: %d (%lf s) %d\n", TRIES, delta, d1, j);

    
    
  TEST_DONE();
}


int main(int argc, char * argv[]) {
  int result;
  
  TEST_INIT(); 
  TEST_RUN(nori);
  TEST_REPORT();
}









