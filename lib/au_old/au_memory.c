/*
 * Memory related functions.
 */

#include <stdio.h>
#include <stdlib.h>

/* #define AU_NO_GC */
#ifndef AU_NO_GC
    #include <gc.h>
    #define AU_ALLOCATE(size)               GC_MALLOC(size)
    #define AU_REALLOCATE(pointer, size)    GC_REALLOC(pointer, size)
    #define AU_FREE(pointer)                GC_FREE(pointer)
    #define AU_INIT_MEMORY()                GC_INIT()
#else
    #include <stdlib.h>
    #define AU_ALLOCATE(size)               calloc(1, size)
    #define AU_REALLOCATE(pointer, size)    realloc(pointer, size)
    #define AU_FREE(pointer)                free(pointer)
    #define AU_INIT_MEMORY()                {;} 
    /* Use normal memory allocators. */
#endif



void *  au_malloc(size_t size) {
    return AU_ALLOCATE(size);    
}

void *  au_realloc(void * old, size_t size) {
    return AU_REALLOCATE(old, size);
}

/* Freest memory. Returns NULL, guaranteed. */
void  * au_free(void * ptr) {
   AU_FREE(ptr);     
   return NULL;
}

void au_mem_init(void)  {
    AU_INIT_MEMORY()
}





#define AU_RTSTACK_SIZE 512

struct au_rtstack_struct {    
    size_t  size;
    size_t  space;    
    char   *stack;
};

struct          au_rtstack_struct;
typedef struct  au_rtstack_struct * au_rtstack;


au_rtstack au_rtstack_newspace(size_t space) {
    au_rtstack  result =  
        (au_rtstack) au_malloc( sizeof(struct au_rtstack_struct) + space );    
    /* Allowed because we have the char * stack at the end of the struct. */
    if(!result) {
        perror("Out of stack space!");
        exit(EXIT_FAILURE); /* Need better error handling here. */
    }    
    result->size  = 0;
    result->space = space;
    return result;
}    


au_rtstack au_rtstack_new() {
    return au_rtstack_newspace(AU_RTSTACK_SIZE);    
}

au_rtstack au_rtstack_free(au_rtstack self) {
    free(self); 
    return NULL;
}
     

au_rtstack au_rtstack_copy(au_rtstack self, const au_rtstack from ) {
    size_t index;
    for(index = 0; (index < from->size) && (index < self->space) ; index++  ) {
        self->stack[index] = from->stack[index];        
    }
    self->size = index;
    return self;
}

au_rtstack au_rtstack_resize(au_rtstack self, size_t newspace ) {
    au_rtstack result = au_rtstack_newspace(newspace);
    if(!result) {
        perror("Out of stack space!");
        exit(EXIT_FAILURE); /* Need better error handling here. */
    }
    return au_rtstack_copy(result, self);    
}

void * au_rtstack_alloca(au_rtstack * selfp, size_t size) {
    au_rtstack self = (*selfp);
    void * result   = NULL; 
    if ((self->size + size) < self->space ) {
        /* Stack is full, should try to reallocate it somehow. */
        self     = au_rtstack_resize( self, self->space * 2 );
        (*selfp) = self; /* send back grown stack to the caller */
        perror("Out of stack space!");
        exit(EXIT_FAILURE); /* Need better error handling here. */
    }     
    result         = self->stack + self->size;
    self->size    += size;
    return result;
}

int * au_rtstack_int(au_rtstack self) {
    return (int *) au_rtstack_alloca(self, sizeof(int));
}



