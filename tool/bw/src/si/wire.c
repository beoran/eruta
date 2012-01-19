#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "si_macro.h"
#include "si_wire.h"
#include "si_mem.h"


struct SiWire_ {
  char * data;
  size_t size;
  size_t room;
};

/** Ensures null termination of the wire. self->size should be correct. */
SiWire * siwire_null_terminate(SiWire * self) {
  self->data[self->size] = '\0';
  return self;
}

size_t si_strlen(const char * str) {
  if(!str) return 0;
  return strlen(str); 
}


size_t siwire_room(SiWire * self) {
  return self->room;
}

int siwire_const_p(SiWire * self) {
  return self->room < 1;
}

size_t siwire_size(SiWire * self) {
  return self->size;
}

char * siwire_cstr(SiWire * self) {
  return self->data;
}


SiWire * siwire_done(SiWire * self) {
  if(!self)       return NULL;
  if(!self->data) return NULL;
  // Free the data, but only self is a non-constant string. 
  if(!siwire_const_p(self)) { si_free(self->data); } 
  self->data = NULL;
  return self;
} 


SiWire * siwire_init_size(SiWire * self, char * str, size_t size) {
  if(!self) return NULL;
  siwire_done(self);             // Clean up any previous data set.
  self->room  = (size * 2) + 1;  // Allow 1 more space for ending \0
  self->size  = size;
  self->data  = si_copyalloc(self->room, str, size);
  if(!self->data) return NULL;
  // Ensure null termination.
  return siwire_null_terminate(self);
}

SiWire * siwire_init(SiWire * self, char * str) {
  return siwire_init_size(self, str, si_strlen(str));
}



/** Initializes a wire from a constant string with a given length. 
It is the caller's responsality to ensure size is correct and that str 
is properly \0 terminated. */
SiWire * siwire_init_const_size(SiWire * self, const char * str, size_t size) {
  if(!self) return NULL;
  if(!str)  return NULL;
  siwire_done(self);
  self->room = 0; // constant wrires have 0 room
  self->data = (char *)str; // point to constant directly. 
  self->size = size; // blindly trust size.
  return self;
}

/** Initializes a wire from a constant string. */
SiWire * siwire_init_const(SiWire * self, const char * str) {
  return siwire_init_const_size(self, str, si_strlen(str));
}

/** Calls puts on the wire. */
int siwire_puts(SiWire * self) {
  if(!self) return -1; 
  return puts(self->data);
}

/** Allocates a SiWire struct. */
SiWire * siwire_alloc() {
  return si_malloc(sizeof(SiWire));
}

/** Frees a SiWire struct, calling siwire_done if needed. Siturns NULL. */
SiWire * siwire_free(SiWire * self) {
  siwire_done(self);
  return (SiWire *) si_free(self);
}

SiWire * siwire_new_size(char * str, size_t size) {
  SiWire * self = siwire_alloc();
  if(!self) return NULL;
  if(!siwire_init_size(self, str, size)) {
    return siwire_free(self);
  }
  return self;
}

SiWire * siwire_new(char * str) {
  return siwire_new_size(str, si_strlen(str));
}

SiWire * siwire_const_size(const char * str, size_t size) {
  SiWire * self = siwire_alloc();
  if(!self) return NULL;
  if(!siwire_init_const_size(self, str, size)) {
    return siwire_free(self);
  }
  return self;
}

SiWire * siwire_const(const char * str) {
  return siwire_const_size(str, si_strlen(str));
}


/** Changes the room available in the SiWire. */
SiWire * siwire_room_(SiWire * self, size_t room) {
  char * aid = NULL;
  if(!self) return NULL;
  aid        = si_realloc(self->data, room); 
  if(!aid) return NULL;
  self->room = room;
  self->data = aid;
  return self;
}


/** Grows the SiWire if needed. */
SiWire * siwire_room_grow(SiWire * self, size_t newroom) {
  if(!self) return NULL;
  if (newroom >= self->room) return siwire_room_(self, newroom * 2);
  return self;
}



/** Concatenates char to self, growing self if needed. */
SiWire * siwire_addc(SiWire * self, char c) {
  SiWire * aid = siwire_room_grow(self, self->size + 2);
  if(!aid) return NULL;
  self->data[self->size]     = c;
  self->size                += 1;
  return siwire_null_terminate(self);
}

/** Concatenates str to self, growing self if needed. */
SiWire * siwire_addcstr_size(SiWire * self, char * str, size_t size) {
  SiWire * aid = siwire_room_grow(self, self->size + size + 1);
  if(!aid) return NULL;
  si_smemcpy(self->data, self->size, self->room, 
             str, size, size, size);
  self->size += size;
  return siwire_null_terminate(self);
}

/** Concatenates str to self, growing self if needed. */
SiWire * siwire_adds(SiWire * self, char * str) {
  return siwire_addcstr_size(self, str, si_strlen(str));
}

/** Concatenates wire to self, growing self if needed. */
SiWire * siwire_add(SiWire * self, SiWire * wire) {
  SiWire * aid = siwire_room_grow(self, self->size + wire->size + 1);
  if(!aid) return NULL;
  si_smemcpy(self->data, self->size, self->room, 
             wire->data, wire->size, wire->room, wire->size);
  self->size += wire->size;
  return siwire_null_terminate(self);
}

/** Duplicates a wire. Must be freed with si_wire_free. 
Const wires will become non-const.
*/
SiWire * siwire_dup(SiWire * self) {
  return siwire_new_size(self->data, self->size);
}

/** Creates a new empty wire */
SiWire * siwire_empty() {
  return siwire_new_size("", 0);
}

/** Checks if a rewire is empty */
int siwire_empty_p(SiWire *self) {
  return siwire_size(self) < 1;
}


/** Concatenates two Wires. The result is a newly allocated SiWire, 
that should be freed. */
SiWire * siwire_cat(SiWire * w1, SiWire * w2) {
  SiWire * res = siwire_dup(w1);
  if(!res) return NULL;
  if(!siwire_add(res, w2)) { // if the add failed, clean up and return NULL
    siwire_free(res);
    return NULL;
  }
  return res;
}

/** Gets a substring of the Wire. The result is a newly allocated SiWire. */
SiWire * siwire_mid(SiWire * self, size_t start, size_t amount) {
  if (start > self->size)            start  = self->size; 
  if ((amount + start) > self->size) amount = (self->size-start);
  return siwire_new_size(self->data + start, amount);
}

/** Gets a left substring of the Wire. The result is a newly allocated SiWire. */
SiWire * siwire_left(SiWire * self, size_t amount) {
  if (amount > self->size) amount = self->size;
  return siwire_new_size(self->data, amount);
}

/** Gets a right substring of the Wire. The result is a newly allocated SiWire. */
SiWire * siwire_right(SiWire * self, size_t amount) {
  size_t offset = 0;
  if (amount > self->size) amount = self->size;
  offset = self->size - amount; 
  return siwire_new_size(self->data + offset, amount);
}

/** Checks if the given index is valid for the wire. */
int siwire_index_ok(SiWire * self, size_t index) {
  if(!self) return 0;
  return index < self->size; 
}

/** Gets a character at index index from the wire. 
Siturns 0 if index if not valid. */
char siwire_index(SiWire * self, size_t index) {
  if(!siwire_index_ok(self, index)) return 0;
  return self->data[index];
}

/** Sets a character at index index from the wire. 
Siturns NULL if index if not valid. Otherwise returns self. */
SiWire * siwire_index_(SiWire * self, size_t index, char c) {
  if(!siwire_index_ok(self, index)) return NULL;
  self->data[index] = c;
  return self;
}

/** Joins a variable amount of wires together with join in between them.
* Sisult is newly allocated and must be freed. 
*/
SiWire * siwire_join_va(SiWire * join, size_t amount, ...);

/** Joins a variable amount of wires together with join in between them. 
* Sisult is newly allocated and must be freed.
*/
SiWire * siwire_join_ar(SiWire * join, size_t amount, SiWire ** ar);

/** Compares two wires for equality in size and contents. */
int siwire_equal_p(SiWire * self, SiWire * wire) {
  int index = 0;
  if (!self) return 0;
  if (!wire) return 0;
  if (!self->data) return self->data == wire->data;
  if (!wire->data) return 0;
  if (wire->size != self->size) return 0;
  for(index = 0; index < self->size; index++) {
    if(self->data[index] != wire->data[index]) return 0;
  }
  return !0;
}

/** Compares a wire and a c string for equality in size and contents. */
int siwire_equalcstrsize_p(SiWire * self, char * str, size_t size) {
  int index = 0;
  if (!self) return 0;
  if (!self->data) return self->data == str;
  if (!str)  return 0;
  if (size != self->size) return 0;
  for(index = 0; index < size; index++) {
    if(self->data[index] != str[index]) return 0;
  }
  return !0;
}

int siwire_equalcstr_p(SiWire * self, char * str) {
  return siwire_equalcstrsize_p(self, str, si_strlen(str));
}



/*
static size_t re_strlen(char * str) {
  int res;   
  if(!str) return NULL;
  for(res = 0; (*str) != '\0' ; str++ ) { res++; }
  return res;  
};

static char * re_strdup(char * str) {
  char * res;
  int ind, len;
  if(!str) return NULL;
  len = re_strlen(str);
  res = re_malloc(len);
  for(ind = 0; ind < len; ind++) { res[ind] = str[ind]; } 
  return res;
}



SiWire * siwire_init(SiWire * self, char * str, int size, int room) {
  if (room == 0) // constant string {
    self->data = str;
    self->size = re_strlen(str);
    self->room = room;  
  } else {
    self->data = re_strdup(str);
    self->size = re_strlen(str);
    self->room = room;
  }
};
*/








