/**
  Alox standard library. In C for speed and compileability.   
**/

#include <stdio.h>
#include <stdlib.h>
#include <setjmp.h>

typedef struct AloxSymbolStruct   * AloxSymbol; 
typedef struct AloxArrayStruct    * AloxArray; 

typedef struct AloxClassStruct    * AloxClass; 
typedef struct AloxObjectStruct   * AloxObject;
typedef struct AloxObjectsStruct  * AloxObjects;

typedef struct AloxClosureStruct  * AloxClosure;
typedef struct AloxMethodsStruct  * AloxMethods;
typedef struct AloxParamStruct    * AloxParam;


/**
* A closure knows its environment, its wrapping closures, and its jump buffer.
**/
typedef AloxObject (*AloxFunction)(AloxParam);

struct AloxSymbolStruct {
  long          size;
  char          * text;
  long          hash;
};

struct AloxArrayStruct {
  long        size;
  long        space;
  void     ** data;
}; 

struct AloxClosureStruct {
  AloxSymbol    name;
  jmp_buf     * jump;  /* The jump buffer this closure uses for exception handling.  */  
  AloxFunction  function;  
};

struct AloxMethodsStruct {
  long size;
  AloxClosure * methods; 
};

struct AloxClassStruct {
  AloxObject object;
  AloxClass  parent;
}; 

struct AloxObjectsStruct {
   long         size;
   AloxObject   *objects;
};

struct AloxObjectStruct {
  AloxObject  parent;
  AloxObject  klass;
  AloxObjects mixins;
  AloxMethods methods;
}; 


struct AloxResultStruct {
  
  AloxObject result;
};

typedef struct AloxResultStruct * AloxResult; 


/*


AloxResult alox_closure_call(AloxClosure closure, AloxParam parameters) {
  

}


AloxObject alox_try(AloxClosure closure) {
  AloxObject result = 0;
  buf = (jmp_buf*) alox_allocate(sizeof(jmp_buf))
  if(setjmp(buf)) {
    result = alox_closure_call(closure)
  } else {
    
  }
  return result;
}

*/

void * alox_allocate(size_t size) {
  return malloc(size);
}

void * alox_reallocate(void * ptr, size_t size) {
  return realloc(ptr, size);
}

void alox_free( void * ptr ) {
  free(ptr);
}

AloxArray alox_array$new(size_t size) {
  AloxArray result = (AloxArray) alox_allocate(sizeof(struct AloxArrayStruct));
  if(!result) return result;
  result->size  = size;
  result->space = size * 2;
  result->data  = alox_allocate(sizeof(void *) * result->space);
  return result;
}

void alox_array_free(AloxArray arr) {
  arr->size  = 0;
  arr->space = 0;
  alox_free(arr->data);
  alox_free(arr);
}

void * alox_array_get(AloxArray arr, long index) {
  if (index < 0         ) return 0;
  if (index >= arr->size) return 0;
  return arr->data[index];  
}

void * alox_array_set(AloxArray arr, long index, void * value) {
  if (index < 0          ) return 0;
  if (index >= arr->space) { 
    arr->space = index * 2;
    arr->data  = alox_reallocate(arr->data, sizeof(void *) * arr->space); 
    if(!arr->data) {
      return 0;
    }  
  }
  if (index >= arr->size) { 
    arr->size  = index;
  }
  return arr->data[index] = value;  
}

int main(void) {
  AloxArray arr = alox_array_new(10);
  alox_array_set(arr, 5, "Hello");
  puts(alox_array_get(arr, 5));
  return 0;
}



















