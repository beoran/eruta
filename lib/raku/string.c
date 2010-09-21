#include "raku.h"
#include <stdio.h>


static char * ra_strncpy(char * dst, char * src, RaSize size) {
  RaSize i;
  for( i = 0 ; i < size ; i++) {
     dst[i] = src[i];
  } 
  return dst;
}

/** Frees data associated with the rastring and set it's size to 0 
and it's data to NULL. */
RA_FUNC(RaString) ra_string_done(RaString string) {
  if(string && string->data) {
    ra_mem_free(string->data);
    string->data = NULL;
    string->size = 0;
  }
  return string;
}

/** Frees the RaString as a whole. */
RA_FUNC(RaString) ra_string_free(RaString string) {
  ra_mem_free(ra_string_done(string));
}


/** Destructively resizes the RaString str to size sz. */
RA_FUNC(RaString) ra_string_newsize(RaString str, RaSize sz) {
  char * aid;
  if(!str) return NULL;
  aid           = ra_mem_allot(sz + 1); // One more for \0.
  if(!aid) return NULL;                 // Fail if no memory allocated.  
  ra_string_done(str);                  // Only now, free up old string contents.
  str->data     = aid;                  // Point to newly allocated string.      
  str->size     = sz;                   // Set size.   
  str->data[sz] = 0;                    // Nul terminate.
  return str;
}

/** Initializes an already allocated RaString. */
RA_FUNC(RaString) ra_string_init(RaString str, char * data, RaSize size) {
  if(!ra_string_newsize(str, size)) {
    return NULL;
  }   
  ra_strncpy(str->data, data, size);  
  return str;
}

RA_FUNC(RaString) ra_string_new(char * data, RaSize size) {
  RaString string = ra_mem_allot(sizeof(RaString));
  if(!string) return NULL;
  if(!ra_string_init(string, data, size)) {
    free(string); 
    return NULL;
  }
  return string;
}

RA_FUNC(RaString) ra_string_fromc(char * data) {
  RaSize size = strlen(data);
  return ra_string_new(data, size);
}

// The maximum number of significant (decimal!) figures in an IEEE
// double.
#ifndef RA_DOUBLE_DIGITS
#define RA_DOUBLE_DIGITS 16
#endif

RA_FUNC(RaSize) ra_string_size(RaString str) {
  if(!str) return 0; 
  return str->size;
}

/** Returns the char * representation \0 terminated, of the RaString. */
RA_FUNC(char *) ra_string_data(RaString str) {
  if(!str) return NULL;
  return (char*)str->data;
}

/** Puts the RaString to stdout. Useful for debugging. */
RA_FUNC(int) ra_string_puts(RaString str) {
  puts(ra_string_data(str));
}

RA_FUNC(RaString) ra_string_concat(RaString dest, RaString s1, RaString s2) {
  RaSize newsize;
  if(!dest || !s1 || !s2) return NULL;
  newsize = s1->size + s2->size + 1;
  ra_string_newsize(dest, newsize);
  ra_strncpy(dest->data , s1->data, s1->size);
  ra_strncpy(dest->data + s1->size, s2->data, s2->size);
  // Already \0 terminated by ra_string_newsize. 
  return dest;
}

/**Copies a substring between start and stop from src into dst if it can be 
   done so safely. */
char * ra_strbetween(char * dst, RaSize size, char * src, 
                 RaSize start, RaSize stop) {   
  int index;
  if (start > size)          return NULL; // can't copy past end of string.
  // Truncate stop that goes too far
  if (stop > size)    { stop = size ;  }    
  for(index = start; index < stop; index ++) {
    dst[index] = src[index]; 
  } 
  return dst;
}

RA_FUNC(RaString) ra_string_between(RaString dst, RaString str, 
                          RaSize start, RaSize stop) {
  RaSize newsize; 
  if(!dst || !str) return NULL;
  newsize = stop - start + 1;
  if (newsize < 0) { newsize = 0; }
  ra_string_newsize(dst, newsize);
  ra_strbetween(dst->data, dst->size, str->data, start, stop);
  return dst;  
}


RA_FUNC(int) ra_string_equal(RaString s1, RaString s2) {
  if(!s1 || !s2)            return FALSE;
  if(s1->size != s2->size)  return FALSE;
  return (memcmp(s1->data, s2->data, s1->size) == 0);
}

RA_FUNC(RaString) ra_string_fromnum(RaString dst, RaF64 num) {
  RaSize size;
  char buffer[1024]; // hope it's large enough :p
  if(!dst) return NULL; 
  sprintf(buffer, "%lf", num); // hope sprintf is accurate enough.
  size = strlen(buffer);
  ra_string_newsize(dst, size);
  ra_strncpy(dst->data, buffer, size);
  return dst;
}

  
RA_FUNC(int) ra_string_tof64(RaString src, RaF64 * f64) {
  if(!src || !src->data || !f64) return FALSE;
  (*f64) = atof(src->data);
  return TRUE;
}

RA_FUNC(int) ra_string_toi32(RaString src, RaI32 * i32) {
  if(!src || !src->data || !i32) return FALSE;
  (*i32)  =  atol(src->data);
  return TRUE;
}




