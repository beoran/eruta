#include <stdio.h>
#include <string.h>
#include "alox_crt.h"

struct string_s {
  char * data;
}; 

struct string_wrap_s {
  alox_value  self;
  char * data;
};

typedef struct string_wrap_s string;

long alox_hash_cstr(char * s) {
  long hash = 0;
  while (1)
    {
      if (*s == '\0')
        break;
      else
        hash ^= *s++;
      if (*s == '\0')
        break;
      else
        hash ^= (*s++ << 8);
      if (*s == '\0')
        break;
      else
        hash ^= (*s++ << 16);
      if (*s == '\0')
        break;
      else
        hash ^= (*s++ << 24);
    }
  return hash;
}


alox_value m_string_puts(alox_value self) {
  string *s = ((string*) self.pointer);
  puts(s->data);
  return self;
}

alox_value alox_dispatch(alox_value self, char * message, ...) {
  int         index;
  alox_value  result;
  alox_vtable *vtable;
  alox_method *method = 0;
  result.value        = 0;

  vtable = self.vtable;
  for(index = 0; index < vtable->size; index ++) {
    method = vtable->methods + index;
    if(strcmp(message, method->name.string.data) == 0) {
      return method->function(self);
    }
  } 
  /* Should call method_missing here. */
  return result;
}


int main(void) {
  string s;
  alox_method m_puts;
  alox_method m_puts2;
  alox_value  s_self;
  alox_vtable s_vtable;
  s_self.pointer      = &s;
  s.self              = s_self;

  s.data                     = "Hello AlOx!\n";
  s_vtable.size              = 1;

  m_puts.name.string.data    = "puts";
  m_puts.object              = s.self;
  m_puts.function            = (alox_functor) &m_string_puts;
  m_puts2.name.string.data   = "puts2";
  m_puts2.object             = s.self;
  m_puts2.function           = (alox_functor) &m_string_puts;
  
  s_vtable.methods[0]        = m_puts; 
  s.self.vtable              = & s_vtable; 
  alox_dispatch(s.self, "puts");
  
  /* s.vtable.methods[0].function(s.self); 
  */
  printf("%ld\n", alox_hash_cstr(m_puts.name.string.data));


  return 0;
}

