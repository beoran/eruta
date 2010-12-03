/*
 * Implementation of a C character buffer. 
 *
 */

#include <string.h>
#include <stdlib.h>
#include "au_str.h"
#include "au_memory.h"


/* Dealloctes the buffer only, emptying the string. */
static au_str au_str_empty(au_str self) {
    if(!self)        { return self; }
    
    if(self->cstr) {
        au_free(self->cstr);
        self->cstr   = NULL;
        self->size   = 0;
    } 
    return self;
}

/* Deallocates the buffer and the C structure itself. */
au_str au_str_free(au_str self) {
    return au_free(au_str_empty(self)); 
    /* Method chaining in plain C. :) */    
}


static au_str au_str_set_cstr(au_str self, const char * cstr, size_t size) {
    if(!self)        { return self; }        
    au_str_empty(self);     
    if(!cstr)        { return self; }    
    self->cstr          = (char *) au_malloc(size);            
    if(!self->cstr)  { 
        self->size = 0; 
        return NULL;        
    }
        
    strncpy(self->cstr, cstr, size);
    /* And copy the data. */

    return self;
}

au_str au_str_new(char * cstr) {
    au_str self  = au_struct_new(au_str);
    if (!self) return NULL;
    if(!au_str_set_cstr(self, cstr, strlen(cstr))) {
        return au_str_free(self);
       /* Free ourself if we cou'lnt copy the c string, and return null. */
    }
    return self;    
}

/* Converts to C string, without copying. 
 * Ownership of memory remains with the au_str. 
 */
const char * au_str_c(au_str self) {
    return (const char *) self->cstr;
}


au_str au_str_cpy   (au_str target, const au_str source) {
    return target;
}




