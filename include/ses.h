#ifndef ses_H_INCLUDED
#define ses_H_INCLUDED

/* Simple explicit string handling and growing. 
 * These functions help handling strings without 
 * using struct wrappers, you need to pass in the parameters explicitly.
 * 
 * Nevertheless a struct wrapper is also provided as a Swis, namely a
 * Simple wrapped idiotic string. The wrapper must be alocated by the user.
 */
#include <stdlib.h> 
#include <stdio.h>
 

 
int ses_bad_p(char **me, size_t * size, size_t * space);
char * ses_make_empty(char ** me, size_t * size, size_t * space);
char * ses_nul_terminate(char ** me, size_t * size, size_t * space);
char * ses_alloc(char ** me, size_t * size, size_t * space, size_t init_space);
char * ses_grow(char ** me, size_t  * size, size_t * space, size_t new_space);
char * ses_ensure_space(char ** me, size_t * size, size_t * space, size_t grow_by);
char * ses_append_char(char ** me, size_t * size, size_t * space, char ch);
char * ses_append_buf(char  ** me, size_t * size, size_t * space, char * buf, size_t bufsize);
char * ses_append_cstr(char ** me, size_t * size, size_t * space, char * str);
char * ses_init_buf(char ** me, size_t * size, size_t * space, char * buf, size_t bufsize);
char * ses_init_cstr(char ** me, size_t * size, size_t * space, char * buf);
char * ses_new_buf(char **me, size_t * size, size_t * space, char * buf, size_t bufsize);
char * ses_new(char **me, size_t * size, size_t * space, char * init);
char * ses_new_empty(char **me, size_t * size, size_t * space);
char * ses_free(char ** me, size_t * size, size_t * space);
char * ses_read_file(char ** me, size_t * size, size_t * space, FILE * file);
char * ses_read_filename(char ** me, size_t * size, size_t * space, char * fn);


typedef struct Swis_ Swis; 

struct Swis_ {
  char      * text;
  size_t      size;
  size_t      space;  
};
 
int swis_bad_p(Swis * me);
Swis * swis_make_empty(Swis * me);
Swis * swis_nul_terminate(Swis * me);
Swis * swis_alloc(Swis * me, size_t init_space);
Swis * swis_grow(Swis * me, size_t new_space);
Swis * swis_ensure_space(Swis * me, size_t grow_by);
Swis * swis_append_char(Swis * me, char ch);
Swis * swis_append_buf(Swis * me, char * buf, size_t bufsize);
Swis * swis_append_cstr(Swis * me, char * str);
Swis * swis_init_buf(Swis * me, char * buf, size_t bufsize);
Swis * swis_init_cstr(Swis * me, char * buf);
Swis * swis_new_buf(Swis * me, char * buf, size_t bufsize);
Swis * swis_new(Swis * me, char * init);
Swis * swis_new_empty(Swis * me);
Swis * swis_free(Swis * me);
Swis * swis_read_file(Swis * me, FILE * file);
Swis * swis_read_filename(Swis * me, char * fn); 


#endif




