
#include "ses.h"
#include <stdio.h>
#include <string.h>

/* Returns true if the string is in an unusable state, false if not. */
int ses_bad_p(char **me, size_t * size, size_t * space) {
  if (!me) return !0;
  if (!*me) return !0;
  if (!size) return !0;
  if (!space) return !0;
  return 0;
}


/* Makes the string an empty string by writing a nul character at positon 0
 * and setting size to 0. */
char * ses_make_empty(char ** me, size_t * size, size_t * space) {
  if (ses_bad_p(me, size, space)) return NULL;
  if ((*space) < 1) return NULL;
  (*me)[0] = '\0';
  (*size)  = 0;
  return (*me);
}

/* Ensures the string is NUL terminated. */
char * ses_nul_terminate(char ** me, size_t * size, size_t * space) {
  if (ses_bad_p(me, size, space)) return NULL;
  if (((*size) + 1) > (*space)) {
    (*me)[(*space)] = '\0';
    (*size) = (*space) - 1;
  }  else { 
    (*me)[(*size)+1] = '\0';
  }
  return (*me);
}


/* allocates a new string buffer with init_space. If init_space == 0 
 *  uses 1024 in stead.*/
char * ses_alloc(char ** me, size_t * size, size_t * space, size_t init_space) {
  if (!me) return NULL;
  if (!space) return NULL;
  if (!size) return NULL;
  if (init_space < 1) init_space = 1024; 
  (*me) = calloc(init_space, 1);
  (*space) = init_space;
  if (!*me) return NULL;
  return ses_make_empty(me, size, space);
}

/* Grows the given string's space. */
char * ses_grow(char ** me, size_t  * size, size_t * space, size_t new_space) {
  char * aid;
  (void) size;
  if (!me) return NULL;
  if (!*me) return NULL;
  
  aid = realloc(*me, new_space);
  if (!aid) return NULL;
  (*space) = new_space;
  (*me)    = aid;   
  return (*me);
} 

/* Makes sure there is enough space to add amount characters. */
char * ses_ensure_space(char ** me, size_t * size, size_t * space, size_t grow_by) {
  if (!me) return NULL;
  if (!*me) return NULL;
  if (!size) return NULL;
  if (!space) return NULL;
  
  if ((*space) < ((*size) + grow_by)) { 
    if (!ses_grow(me, size, space, (*space) + grow_by + 255)) return NULL;
  }
  
  return (*me);
} 

char * ses_append_char(char ** me, size_t * size, size_t * space, char ch) {
  if (!ses_ensure_space(me, size, space, 1)) return NULL;
    
  (*me)[(*size)] = ch;
  (*size) = (*size) + 1;
  return (*me);
}


char * ses_append_buf(char  ** me, size_t * size, size_t * space, char * buf, size_t bufsize) {
  if (!ses_ensure_space(me, size, space, bufsize + 1)) return NULL;
  strncpy((*me) + (*size), buf, bufsize);
  (*size) = (*size) + bufsize;
  return ses_nul_terminate(me, size, space);
}

char * ses_append_cstr(char ** me, size_t * size, size_t * space, char * str) {
  return ses_append_buf(me, size, space, str, strlen(str));
}

char * ses_init_buf(char ** me, size_t * size, size_t * space, char * buf, size_t bufsize) {
  if (!size) return NULL;
  if (!ses_make_empty(me, size, space)) return NULL; 
  return ses_append_buf(me, size, space, buf, bufsize);
} 

char * ses_init_cstr(char ** me, size_t * size, size_t * space, char * buf) {
  return ses_init_buf(me, size, space, buf, strlen(buf));
} 

char * ses_new_buf(char **me, size_t * size, size_t * space, char * buf, size_t bufsize) {
  if (!ses_alloc(me, size, space, bufsize)) return NULL;
  return ses_init_buf(me, size, space, buf, bufsize);
}

/* Creates a new string with the given space and initialies it from init. */
char * ses_new(char **me, size_t * size, size_t * space, char * init) {
  return ses_new_buf(me, size, space, init, strlen(init));
}

/* Creates a new empty string with enough space. */
char * ses_new_empty(char **me, size_t * size, size_t * space) {
  return ses_new(me, size, space, "");
}

/* Frees the string and sets it to NULL. */
char * ses_free(char ** me, size_t * size, size_t * space) {
  if (!me) return NULL;
  if (!*me) return NULL;
  free(*me);
  *me    = NULL;
  *size  = 0;
  *space = 0;
  return NULL;
}

/** Reads in a file into the buffer. */
char * ses_read_file(char ** me, size_t * size, size_t * space, FILE * file) {
  char aid[1024];
  int read;
  if (ses_bad_p(me, size, space)) return NULL;
  if (!file) return NULL;
  
  while(!feof(file)) { 
    read = fread(aid, 1, sizeof(aid), file);
    if (read > 0) { 
      ses_append_buf(me, size, space, aid, read);
    }
  }
  return (*me);
}

/** Reads a named file into the buffer. */
char * ses_read_filename(char ** me, size_t * size, size_t * space, char * fn) {
  char * res;
  FILE * file = fopen(fn, "r");
  res = ses_read_file(me, size, space, file);
  if (file) fclose(file);
  return res;
}


/** Deletes n bytes */

#define SWIS_EXPAND(SWIS) &((SWIS)->text), &((SWIS)->size), &((SWIS)->space)

int swis_bad_p(Swis * me) {
  return (ses_bad_p(SWIS_EXPAND(me)));
}

Swis * swis_make_empty(Swis * me) {
  if(!ses_make_empty(SWIS_EXPAND(me))) return NULL;
  return me;
}


Swis * swis_nul_terminate(Swis * me) {
  if(!ses_nul_terminate(SWIS_EXPAND(me))) return NULL;
  return me;  
}

Swis * swis_alloc(Swis * me, size_t init_space) {
  if(!ses_alloc(SWIS_EXPAND(me), init_space)) return NULL;
  return me;
}

Swis * swis_grow(Swis * me, size_t new_space) {
  if(!ses_grow(SWIS_EXPAND(me), new_space)) return NULL;
  return me;
}

Swis * swis_ensure_space(Swis * me, size_t grow_by) {
  if(!ses_ensure_space(SWIS_EXPAND(me), grow_by)) return NULL;
  return me;
}


Swis * swis_append_char(Swis * me, char ch) {
  if(!ses_append_char(SWIS_EXPAND(me), ch)) return NULL;
  return me;
}


Swis * swis_append_buf(Swis * me, char * buf, size_t bufsize) {
  if(!ses_append_buf(SWIS_EXPAND(me), buf, bufsize)) return NULL;
  return me;
}

Swis * swis_append_cstr(Swis * me, char * str) {
  if(!ses_append_cstr(SWIS_EXPAND(me), str)) return NULL;
  return me;
}

Swis * swis_init_buf(Swis * me, char * buf, size_t bufsize) {
  if(!ses_init_buf(SWIS_EXPAND(me), buf, bufsize)) return NULL;
  return me;
}

Swis * swis_init_cstr(Swis * me, char * buf) {
  if(!ses_init_cstr(SWIS_EXPAND(me), buf)) return NULL;
  return me;
}


Swis * swis_new_buf(Swis * me, char * buf, size_t bufsize) {
  if(!ses_new_buf(SWIS_EXPAND(me), buf, bufsize)) return NULL;
  return me;
}

Swis * swis_new(Swis * me, char * init) {
  if(!ses_new(SWIS_EXPAND(me), init)) return NULL;
  return me;
}

Swis * swis_new_empty(Swis * me) {
  if(!ses_new_empty(SWIS_EXPAND(me))) return NULL;
  return me;
}

Swis * swis_free(Swis * me) {
  ses_free(SWIS_EXPAND(me)); 
  return NULL;
}

Swis * swis_read_file(Swis * me, FILE * file) {
  if(!ses_read_file(SWIS_EXPAND(me), file)) return NULL;
  return me;
}
 
Swis * swis_read_filename(Swis * me, char * fn) {
  if(!ses_read_filename(SWIS_EXPAND(me), fn)) return NULL;
  return me;
}


