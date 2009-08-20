#ifndef ALOX_CRT
#define ALOX_CRT


typedef void * au_id;
typedef char * au_sel;
typedef au_id (*au_imp)(au_id self, au_sel _cmd,...);




struct alox_vtable_s;

struct alox_value_s {
  void * pointer;
  long value;
  struct alox_vtable_s *vtable;
};

typedef struct alox_value_s alox_value;

void * alox_alloc(long size);
void * alox_realloc(void * memory, long size);
void alox_free(void * memory);

typedef alox_value (*alox_functor) (alox_value self, ...);

struct alox_string_s {
  long  size;
  char *data;
};

typedef struct alox_string_s alox_string;

struct alox_symbol_s {
  alox_string string;
  long hash;
};

typedef struct alox_symbol_s alox_symbol;

alox_value alox_new(alox_value klass);
alox_value alox_copy(alox_value object);
void alox_kill(alox_value object);

struct alox_method_s {
  alox_symbol   name;
  alox_functor  function;
  alox_value    object;
};

typedef struct alox_method_s alox_method;

/* Dynamic methods understood. Later change this to a hash table. */
struct alox_vtable_s {
  long size;
  alox_method methods[100]; /*XXX: just a test hack*/
} ;

typedef struct alox_vtable_s alox_vtable;

/* Dynamic member variables.  Later change this to a hash table. */
struct alox_mtable_s {
  long size;
  alox_value values[100]; /*XXX: just a test hack*/
};

typedef struct alox_mtable_s alox_mtable;

struct alox_object_s {
  alox_value    _alox_self;
  alox_value    _alox_class;
  alox_vtable   _alox_vtable;
  alox_mtable   _alox_mtable; 
  void *        _alox_struct;
  /* Low level C struct that we are wrapping. */
};

typedef struct alox_object_s alox_object; 


#endif
