#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "re/wire.h"
#include "re/mem.h"


struct ReWire_ {
  char * data;
  size_t size;
  size_t room;
};

/** Ensures null termination of the wire. self->size should be correct. */
ReWire * rewire_null_terminate(ReWire * self) {
  self->data[self->size] = '\0';
  return self;
}

size_t re_strlen(const char * str) {
  if(!str) return 0;
  return strlen(str); 
}


size_t rewire_room(ReWire * self) {
  return self->room;
}

int rewire_const_p(ReWire * self) {
  return self->room < 1;
}

size_t rewire_size(ReWire * self) {
  return self->size;
}

char * rewire_cstr(ReWire * self) {
  return self->data;
}


ReWire * rewire_done(ReWire * self) {
  if(!self)       return NULL;
  if(!self->data) return NULL;
  // Free the data, but only self is a non-constant string. 
  if(!rewire_const_p(self)) { re_free(self->data); } 
  self->data = NULL;
  return self;
} 


ReWire * rewire_init_size(ReWire * self, char * str, size_t size) {
  if(!self) return NULL;
  rewire_done(self);             // Clean up any previous data set.
  self->room  = (size * 2) + 1;  // Allow 1 more space for ending \0
  self->size  = size;
  self->data  = re_copyalloc(self->room, str, size);
  if(!self->data) return NULL;
  // Ensure null termination.
  return rewire_null_terminate(self);
}

ReWire * rewire_init(ReWire * self, char * str) {
  return rewire_init_size(self, str, re_strlen(str));
}

/** Initializes a wire from a constant string with a given length. 
It is the caller's responsality to ensure size is correct and that str 
is properly \0 terminated. */
ReWire * rewire_init_const_size(ReWire * self, const char * str, size_t size) {
  if(!self) return NULL;
  if(!str)  return NULL;
  rewire_done(self);
  self->room = 0; // constant wrires have 0 room
  self->data = (char *)str; // point to constant directly. 
  self->size = size; // blindly trust size.
  return self;
}

/** Initializes a wire from a constant string. */
ReWire * rewire_init_const(ReWire * self, const char * str) {
  return rewire_init_const_size(self, str, re_strlen(str));
}

/** Calls puts on the wire. */
int rewire_puts(ReWire * self) {
  if(!self) return -1; 
  return puts(self->data);
}

/** Allocates a ReWire struct. */
ReWire * rewire_alloc() {
  return re_malloc(sizeof(ReWire));
}

/** Frees a ReWire struct, calling rewire_done if needed. Returns NULL. */
ReWire * rewire_free(ReWire * self) {
  rewire_done(self);
  return (ReWire *) re_free(self);
}

ReWire * rewire_new_size(char * str, size_t size) {
  ReWire * self = rewire_alloc();
  if(!self) return NULL;
  if(!rewire_init_size(self, str, size)) {
    return rewire_free(self);
  }
  return self;
}

ReWire * rewire_new(char * str) {
  return rewire_new_size(str, re_strlen(str));
}

ReWire * rewire_const_size(const char * str, size_t size) {
  ReWire * self = rewire_alloc();
  if(!self) return NULL;
  if(!rewire_init_const_size(self, str, size)) {
    return rewire_free(self);
  }
  return self;
}

ReWire * rewire_const(const char * str) {
  return rewire_const_size(str, re_strlen(str));
}


/** Changes the room available in the ReWire. */
ReWire * rewire_room_(ReWire * self, size_t room) {
  char * aid = NULL;
  if(!self) return NULL;
  aid        = re_realloc(self->data, room); 
  if(!aid) return NULL;
  self->room = room;
  self->data = aid;
  return self;
}


/** Grows the ReWire if needed. */
ReWire * rewire_room_grow(ReWire * self, size_t newroom) {
  if(!self) return NULL;
  if (newroom >= self->room) return rewire_room_(self, newroom * 2);
  return self;
}



/** Concatenates char to self, growing self if needed. */
ReWire * rewire_addc(ReWire * self, char c) {
  ReWire * aid = rewire_room_grow(self, self->size + 2);
  if(!aid) return NULL;
  self->data[self->size]     = c;
  self->size                += 1;
  return rewire_null_terminate(self);
}

/** Concatenates str to self, growing self if needed. */
ReWire * rewire_addcstr_size(ReWire * self, char * str, size_t size) {
  ReWire * aid = rewire_room_grow(self, self->size + size + 1);
  if(!aid) return NULL;
  re_smemcpy(self->data, self->size, self->room, 
             str, size, size, size);
  self->size += size;
  return rewire_null_terminate(self);
}

/** Concatenates str to self, growing self if needed. */
ReWire * rewire_adds(ReWire * self, char * str) {
  return rewire_addcstr_size(self, str, re_strlen(str));
}

/** Concatenates wire to self, growing self if needed. */
ReWire * rewire_add(ReWire * self, ReWire * wire) {
  ReWire * aid = rewire_room_grow(self, self->size + wire->size + 1);
  if(!aid) return NULL;
  re_smemcpy(self->data, self->size, self->room, 
             wire->data, wire->size, wire->room, wire->size);
  self->size += wire->size;
  return rewire_null_terminate(self);
}

/** Duplicates the given wire. */
ReWire * rewire_dup(ReWire *self) {
  return rewire_new_size(self->data, self->size);
}

/** Creates a new empty wire */
ReWire * rewire_empty() {
  return rewire_new_size("", 0);
}

/** Checks if a rewire is empty */
int rewire_empty_p(ReWire *self) {
  return rewire_size(self) < 1;
}


/** Concatenates two Wires. The result is a newly allocated ReWire, 
that should be freed. */
ReWire * rewire_cat(ReWire * w1, ReWire * w2) {
  ReWire * res = rewire_dup(w1);
  if(!res) return NULL;
  if(!rewire_add(res, w2)) { // if the add failed, clean up and return NULL
    rewire_free(res);
    return NULL;
  }
  return res;
}

/** Gets a substring of the Wire. The result is a newly allocated ReWire. */
ReWire * rewire_mid(ReWire * self, size_t start, size_t amount) {
  if (start > self->size)            start  = self->size; 
  if ((amount + start) > self->size) amount = (self->size-start);
  return rewire_new_size(self->data + start, amount);
}

/** Gets a left substring of the Wire. The result is a newly allocated ReWire. */
ReWire * rewire_left(ReWire * self, size_t amount) {
  if (amount > self->size) amount = self->size;
  return rewire_new_size(self->data, amount);
}

/** Gets a right substring of the Wire. The result is a newly allocated ReWire. */
ReWire * rewire_right(ReWire * self, size_t amount) {
  size_t offset = 0;
  if (amount > self->size) amount = self->size;
  offset = self->size - amount; 
  return rewire_new_size(self->data + offset, amount);
}

/** Checks if the given index is valid for the wire. */
int rewire_index_ok(ReWire * self, size_t index) {
  if(!self) return 0;
  return index < self->size; 
}

/** Gets a character at index index from the wire. 
Returns 0 if index if not valid. */
char rewire_index(ReWire * self, size_t index) {
  if(!rewire_index_ok(self, index)) return 0;
  return self->data[index];
}

/** Sets a character at index index from the wire. 
Returns NULL if index if not valid. Otherwise returns self. */
ReWire * rewire_index_(ReWire * self, size_t index, char c) {
  if(!rewire_index_ok(self, index)) return NULL;
  self->data[index] = c;
  return self;
}


/** Joins a variable amount of wires together with join in between them.
* Result is newly allocated and must be freed. 
*/
ReWire * rewire_join_va(ReWire * join, size_t amount, ...);

/** Joins a variable amount of wires together with join in between them. 
* Result is newly allocated and must be freed.
*/
ReWire * rewire_join_ar(ReWire * join, size_t amount, ReWire ** ar);

/** Compares two wires for equality in size and contents. */
int rewire_equal_p(ReWire * self, ReWire * wire) {
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
int rewire_equalcstrsize_p(ReWire * self, char * str, size_t size) {
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

int rewire_equalcstr_p(ReWire * self, char * str) {
  return rewire_equalcstrsize_p(self, str, re_strlen(str));
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



ReWire * rewire_init(ReWire * self, char * str, int size, int room) {
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








