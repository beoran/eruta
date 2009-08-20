/**
* AU, a minimal, tiny, re-entrant, mostly ANSI 89 C compatible runtime for a 
* dynamic dispatch language like Ruby. 
* The ultimate goal is to simplify  compilation of such languages to C.
* 
* Internally, it tries to avoid dynamical allocation of memory as much as 
* is practical possible and uses static, hard-coded limits, to keep the 
* runtime as simple as possible. This may seem a problem until you realise that 
* these limits can be abritrarily set at compile time.
* 
* Bjorn De Meyer, 2008.
*
* Released under the zlib license. 
*
*/

#include "au_error.h"
#include "au_rt.h"
#include "au_memory.h"
#include "au_str.h"
#include <stddef.h>
#include <limits.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <setjmp.h>
#include <time.h>


#define AU_TEST

/** Memory limits **/

#ifndef SIZE_MAX
#define SIZE_MAX ((size_t) -1)
#endif

        
        

/** Wrappers for low level memory management. */
#ifndef AU_ALLOC
#define AU_ALLOC(size) calloc(size, 1)
#endif

#ifndef AU_REALLOC
#define AU_REALLOC(old, newsize) realloc(old, newsize)
#endif

void * au_mem_alloc(size_t size) {
    return AU_ALLOC(size);
}

void * au_mem_realloc(void * old, size_t size) {
    return AU_REALLOC(old, size);
}

#ifndef AU_FREE
#define AU_FREE(ptr) free(ptr) 
#endif
int au_mem_free(void * ptr) {
    AU_FREE(ptr);
    return 0;
}

#define au_struct_alloc(name) ((name *) au_mem_alloc(sizeof(name)))


#define AU_IARRAY_INITSPACE 32

/* Internal dynamic array, not OOP, to be wrapped by au_array*/
typedef struct au_iarray_ {
    size_t size;
    size_t space;
    void **elements;
} au_iarray ;


/* 
 * Initializes an ivector. Returns the initialized ivector, or NULL on failiure. 
 */
au_iarray * au_iarray_initialize(au_iarray * self) {    
    self->size      = 0;
    self->space     = AU_IARRAY_INITSPACE;
    self->elements  = au_mem_alloc(sizeof(void *) * self->space);
    if (!self->elements) { 
         self->space = 0;
         return NULL;
    }
    return self;
}

/* Allocates one ivector. Returns the ivector or NULL on failiure. */
au_iarray * au_iarray_alloc() {
    return au_struct_alloc(au_iarray);
}

/* Frees one ivector. Returns NULL. */

void  * au_iarray_free(au_iarray * self) {
    au_mem_free(self);
    return NULL;
}

/* Allocates and initializes an ivector. Returns the initialized ivector, 
 * or NULL on failiure.  
 */
au_iarray * au_iarray_new() {
    au_iarray * result = NULL;
    au_iarray * aid = au_iarray_alloc(); 
    if(!aid) { return NULL; }
    result = au_iarray_initialize(result);
    /* If initialization fails, free the vector and return NULL indicating error. */
    if(!result) {  return au_iarray_free(result); }
    return result;    
}

/*
 * Resizes the ivector to be big enough to fit size elements, if needed.
 * Returns the resized ivector, or NULL on failiure.
 */

au_iarray * au_iarray_resize(au_iarray * self, size_t newsize) {
    void ** newelts;
    size_t newspace  = self->space;   
    /* Don't resize if the size fits in the space, and is not less than half current size. 
     */
    if ((newsize < self->space) && (newsize > (self->space / 2))) {
        return self;         
    } 
    /* Don't shrink too small */
    if (newsize < AU_IARRAY_INITSPACE) {
        return self;         
    } 
    while ((newsize < newspace) && (newspace > AU_IARRAY_INITSPACE) ) {
        newspace >>= 1; /* Halve the space until it's OK. */
    }
    while ((newsize > newspace) && (newspace < SIZE_MAX) ) {
        newspace <<= 2; /* Double the space until it's OK. */
    }    
    /* Reallocate space */
    newelts = au_mem_realloc(self->elements, newspace);
    if(!newelts) { return NULL; }
    /* Fail if not enough memory */
    { 
        int index;
        for (index = self->size; index < newspace; index ++ ) {
            newelts[index] = NULL; 
        }    
    
    }
    /* Set new memory area to NULL pointers, and wrap it up. */
    self->space     = newspace;
    self->elements  = newelts;    
    return self;
} 

/* Sets a value. Returns the value set, or NULL on error. 
 * Index may not be negative. 
 */
void * au_iarray_set(au_iarray * self, size_t index, void * value) {
    au_iarray *aid; 
    void * result;
    aid = au_iarray_resize(self, index); 
    /* Potentially resize the vector if the index is too big. */
    if(!aid) { return NULL; }
    /* Bail out if resize failed. */
    result = self->elements[index] = value;
    if(index > self->size) { self->size = index; }
    /* If the index is bigger than the size, make it the size. */
    return result;
    /* Return the value set. */    
}


/* Gets a value. Returns the value at the index, or NULL on error or not found. 
 * Index may not be negative. 
 */
void * au_iarray_get(register au_iarray * self, register size_t index) {    
    if(index > self->size) { return NULL; }
    /* Don't allow gets greater than the current size.*/
    return  self->elements[index];
    /* Get value and return it. */    
}

/* Returns the size of the au_iarray */
size_t au_iarray_size(register au_iarray * self) {
    return (self->size);
}

/* Copy between two iarrays. Returns self, or NULL on failiure. */
au_iarray * au_iarray_copy(au_iarray * self, au_iarray * target) {
    int index, size;    
    size = self->size;    
    for (index = 0 ; index < size; index++) {
        void * value =  au_iarray_get(self, index);
        void * aid   =  au_iarray_set(target, index, value);
        if(!aid) { return NULL; }
        /* Bail out if set failed. */        
    }
    return self;
}

/* Internal linked list, and list node, not OOP, to be wrapped by au_array*/
typedef struct au_ilistnode_ {
    struct au_ilistnode_ * next;
    struct au_ilistnode_ * previous;    
    void                 * value;
} au_ilistnode ;

typedef struct au_ilist_ {
    au_ilistnode * head;
    au_ilistnode * tail;    
    size_t size;
} au_ilist ;


/* 
 * Initializes an ilistnode. Returns the initialized ilistnode, or NULL on failiure. 
 */
au_ilistnode * au_ilistnode_initialize(au_ilistnode * self, void * value) {
    self->value     = value;
    self->next      = NULL;
    self->previous  = NULL;
    return self;
}

/* Allocates one ilist. Returns the ilist or NULL on failiure. */
au_ilistnode * au_ilistnode_alloc() {
    return au_struct_alloc( au_ilistnode );
}

/* Frees one ilistnode. Returns NULL. */
void * au_ilistnode_free(au_ilistnode * self) {
    au_mem_free(self);
    return NULL;
}

/* Allocates and initializes a new list. */
au_ilistnode * au_ilistnode_new(void * value) {
    au_ilistnode * result   = NULL;
    au_ilistnode * aid      = au_ilistnode_alloc();
    if(!aid)                { return NULL; }
    result                  = au_ilistnode_initialize(aid, value); 
    if(!result)             { return au_ilistnode_free(aid); }
    return                  result;    
}


/* 
 * Initializes an ilist. Returns the initialized ilist, or NULL on failiure. 
 */
au_ilist * au_ilist_initialize(au_ilist * self) {    
    self->size      = 0;
    self->head      = NULL;
    self->tail      = NULL;
    return self;
}

/* Allocates one ilist. Returns the ilist or NULL on failiure. */
au_ilist * au_ilist_alloc() {
    return au_struct_alloc( au_ilist );
}

/* Frees one ilist. Returns NULL. */
void * au_ilist_free(au_ilist * self) {
    au_mem_free(self);
    return NULL;
}

/* Allocates and initializes a new list. */
au_ilist * au_ilist_new() {
    au_ilist * result   = NULL;
    au_ilist * aid      = au_ilist_alloc();
    if(!aid)              { return NULL; }
    result              = au_ilist_initialize(aid); 
    if(!result)           { return au_ilist_free(aid); }
    return                  result;    
}

au_ilist * au_ilist_insertnode(au_ilist * self, au_ilistnode * node, au_ilistnode * after) {    
    self->size += 1;
    if(self->head   == NULL)        {
        /* Special case for empty list. */
        self->head              = node; 
        self->tail              = node;
        node->next              = NULL;
        node->previous          = NULL;
        return self;
    }
    /* If after is NULL, it means, the head */
    if(after        == NULL)  {  
        self->head->previous    = node;
        node->next              = self->head;
        node->previous          = NULL;
        self->head              = node;                
        return self;        
    } 
    after->next                 = node;
    node->previous              = after;
    node->next                  = after->next;
    /* handle tail case */
    if (self->tail == after) { 
       self->tail               = node;     
    }
    
    return self;
}

au_ilistnode * au_ilist_appendnode(au_ilist * self, au_ilistnode * node) {    
    au_ilist_insertnode(self, node, self->tail);
    return node;
}

au_ilistnode * au_ilist_walk(au_ilist * self, au_ilistnode * index) {        
    if(index   == NULL)        {
        /* Special case for start. */
        return self->head;
    }
    return index->next;
}

/* Internal simple dynamic chained hash table. */
typedef struct au_ihash_ {
    size_t    size;
    au_iarray buckets;
} au_ihash;

/* Forward declaration of block struct. */
struct au_iblock_;

/* Internal low level block function and block. */
typedef  void * (*au_iblockfunc)(struct au_iblock_ * block);

typedef struct au_iblock_ {
    au_iarray       * arguments;
    au_iarray       environment;
    au_iarray       results;
    au_iblockfunc   block;
} au_iblock;


au_iblock * au_iblock_initialize(au_iblock * self, au_iblockfunc block, au_iarray * environment) 
{
    au_iarray * aid;
    self->block     = block;
    self->arguments = NULL; 
    aid             = au_iarray_copy(environment, &self->environment);
    if(!aid)        { return NULL; }
    aid             = au_iarray_initialize(&self->results);
    if(!aid)        { return NULL; }
    return self;
}

void * au_iblock_call(au_iblock * self, au_iarray * arguments) {
    self->arguments = arguments;
    return self->block(self);
}


/* Internal low level iterator function. */
typedef void * (*au_ilowiter)(int argc, void** argv);




/* Internal copy on write character buffer, somewhat compatible with C string,
 * not  OOP, to be wrapped.
 */
struct au_str {    
    int size;        
    int space;
    char * buffer;    
}; 


struct au_str *  au_str_alloc() {
    struct au_str * result = au_struct_alloc(struct au_str);
    return result;
} 





/* Real definition of the structs.*/
/* The runtime. */
struct au_rt_ {
    /* Empty for now. */
} ;

/* Messages to be passed to objects. */
struct au_message_ {
   int              size;     
   char             name[AU_MESSAGE_SIZE];
   unsigned short   hash;
};


/* Structure for the hash buckets of the method lookup table. */
typedef struct au_msghash_bucket_ {
    au_method       method;
    au_message  *   message;
    char            status;
} au_msghash_bucket; 

/* Method lookup hash bucket values. */
#define AU_BUCKET_EMPTY     1
#define AU_BUCKET_DELETED   2
#define AU_BUCKET_FULL      4






#ifdef NOWAY

#define AU_MSGLASH_INITSIZE 32

/* Linear message hash table */
typedef struct au_msglash_ {
    size_t used;
    size_t space;
    int    level;
    au_msghash_bucket *table;    
} au_msglash;

au_msglash * au_msglash_initialize(au_msglash * hash) {
    int index;
    lash->used  = 0;
    lash->level = 0;
    lash->size  = AU_MSGLASH_INITSIZE;
    lash->table = au_allloc(lash->size * sizeof(au_msghash_bucket)); 
    if (! lash->table) { 
        return 0;
    } 
    for (index  = 0; index < lash->size ; index ++ ) {
        hash->table[index].status = AU_BUCKET_EMPTY;
    }
    return hash;
}

int au_lash_levelhash(position, level) {
    
    
}

/** Add to hash. */
au_msglash * au_msglash_add(au_msglash * hash, au_message * msg, au_method met) {
    au_msghash_bucket * bucket;
    int position;
    position          = msg->hash;
    bucket            = hash->table + position;
    while (bucket->status == AU_BUCKET_FULL) {  
        position     += AU_BUCKET_SKIP;
        position      = position % AU_METHODS_HASHSIZE;
        bucket        = hash->table + position;
    }
    
    bucket->status    = AU_BUCKET_FULL;
    bucket->method    = met;
    bucket->message   = msg;

    return hash;
}


/** Get from hash*/
au_method * au_msglash_get(au_msglash * hash, au_message * msg) {
    au_msghash_bucket * bucket;
    int position;
    position          = msg->hash;
    bucket            = hash->table + position;
    while ( !(au_message_equal(bucket->message, msg))) {  
        position     += AU_BUCKET_SKIP;
        position      = position % AU_METHODS_HASHSIZE;
        bucket        = hash->table + position;
    }
    return bucket->method;
}

#endif


/* The structure that holds the methods. 
 * It is a statically allocated open adressed hash for absolute simplicity.
 */    
typedef struct au_msghash_ {
  long              used;
  au_msghash_bucket table[AU_METHODS_HASHSIZE];
} au_msghash;


struct au_id_ {    
    au_rt *     runtime;
    au_msghash  messages;    
    void *      cache;
    void *      wrapped;
};

au_msghash * au_msghash_initialize(au_msghash * hash) {
    int index;
    hash->used = 0;
    for (index = 0; index < AU_METHODS_HASHSIZE ; index ++ ) {
        hash->table[index].status = AU_BUCKET_EMPTY;
    }
    return hash;
}

#ifndef AU_BUCKET_SKIP 
#define AU_BUCKET_SKIP 7
#endif


/** Add to hash. */
au_msghash * au_msghash_add(au_msghash * hash, au_message * msg, au_method met) {
    au_msghash_bucket * bucket;
    int position;
    position          = msg->hash;
    bucket            = hash->table + position;
    while (bucket->status == AU_BUCKET_FULL) {  
        position     += AU_BUCKET_SKIP;
        position      = position % AU_METHODS_HASHSIZE;
        bucket        = hash->table + position;
    }
    
    bucket->status    = AU_BUCKET_FULL;
    bucket->method    = met;
    bucket->message   = msg;

    return hash;
}


/** Get from hash*/
au_method * au_msghash_get(au_msghash * hash, au_message * msg) {
    au_msghash_bucket * bucket;
    int position;
    position          = msg->hash;
    bucket            = hash->table + position;
    while ( !(au_message_equal(bucket->message, msg))) {  
        position     += AU_BUCKET_SKIP;
        position      = position % AU_METHODS_HASHSIZE;
        bucket        = hash->table + position;
    }
    return bucket->method;
}




/* 
 * Create the runtime. 
 */
au_rt *     au_rt_new() {
    au_rt * rt;
    rt = au_struct_alloc(au_rt);
    return rt;
}

int         au_rt_free(au_rt *self) {
    return au_mem_free(self);
}

#ifdef _NO_WAY_

/** Functions for higher level memory management. */
void *      au_alloc(au_rt *rt, size_t size) {
    void * ptr;
    ptr = au_mem_alloc(size);
    return ptr;    
}

int         au_free(au_rt * rt, void * pointer) {
    return au_mem_free(pointer);
}

#endif

/** Functions to create messages. */

/* Calculate a simple one way hash of a C string, based upon the OAT algorithm . */
unsigned short au_message_hash(au_message * self) 
{
    unsigned char *p = (unsigned char *) self->name;
    unsigned short h = 0;
    int i;
    for ( i = 0; i < self->size; i ++) {         
        h += p[i];
        h += ( h << 10 );
        h ^= ( h >> 6 );
    } 
    h += ( h << 3 );
    h ^= ( h >> 11 );
    h += ( h << 15 );
    
    self->hash = h;
    return self->hash;    
}

au_message *    au_message_initialize(au_message * self, const char * cstr) {
    self->size       = strlen(cstr);
    if (self->size   >= AU_MESSAGE_SIZE) {
        self->size   = AU_MESSAGE_SIZE - 1;
    }
    strncpy(self->name, cstr, self->size);
    self->name[self->size] = '\0'; /* Make sure it is null terminated. */
    au_message_hash(self);
    return self;
}


au_message *    au_message_new(const char * cstr) {
    au_message * msg;
    msg = au_struct_alloc(au_message);
    msg = au_message_initialize(msg, cstr);    
    return msg;
}

const char *    au_message_cstr(au_message * self) {
    return (const char *) self->name;
}


int             au_message_free(au_message * self) {
    return au_mem_free(self);
}

/* Compares mesages for equality. */
int             au_message_equal(au_message * self, au_message * other) {
    if (self->size != other->size) {
        return 0;
    }    
    /* Sizes are the same. Cmpare them*/    
    return strncmp(self->name, other->name, self->size) != 0;
} 


/* Create a basic AU object.*/
au_id *         au_id_new(au_rt  * rt) {
    au_id * id;
    id  = au_struct_alloc(au_id);
    id->runtime = rt;
    au_msghash_initialize(&id->messages);
    /* id  = au_id_initialize(id, rt);  */  
    return id;
}

int             au_id_free(au_id * self) {
    return au_mem_free(self);
}

/* Add a method to the object, overriding the old one if it existed. */
au_id * au_id_set(au_id * self, au_message * message, au_method method) {
    au_msghash_add(&self->messages, message, method);
    return self;
}
/* Gets the method that will be called when given this message, or NULL if no 
 * such method. Allows method caching. 
 */
au_method *     au_id_get(au_id * self, au_message * message) {
    
}
/* Sets the method_missing special method for this object. */
au_id *         au_id_missing_set(au_id * self, au_method * method);
/* Sets the method_missing special method for this object. */
au_method *     au_id_missing_get(au_id * self);
/* The core dispatching function. */
au_id *         au_id_send(au_id * self, au_message * message, ... );



/* Calculate a simple one way hash of a C string, based upon the OAT algorithm . */
unsigned short au_hash_cstr(const char * key) 
{
    unsigned char *p = (unsigned char *) key;
    unsigned short h = 0;
    for ( ; p && (*p != '\0') ; p++ ) {
        h += *p;
        h += ( h << 10 );
        h ^= ( h >> 6 );
    } 
    h += ( h << 3 );
    h ^= ( h >> 11 );
    h += ( h << 15 );
    return h;    
}





#ifdef AU_TEST


typedef void (*au_profilable)();


double func_profile(au_profilable func, long repeat, long multiplier, const char * name) {
    clock_t start, stop;
    long delta, index;
    double seconds, frequency;
    
    start       = clock();
    for (index = 0; index < repeat; index ++) {
        func();
    }        
    stop        = clock();
    delta       = (long) stop - start;
    seconds     = ((double) delta)  / ((double) CLOCKS_PER_SEC);
    frequency   = ((double) repeat) * ((double) multiplier) / seconds;     
    printf("Profile %s: %lf s for %ld repeats : %lf Hz . \n", 
           name, seconds, repeat, frequency);
    return seconds;        
}


#define TEST_VTABLE_SIZE 255

typedef struct testobject_ {
    double counter;
    void (*increment) (struct testobject_*);
    void (*vtable [TEST_VTABLE_SIZE] ) (struct testobject_*);
    void (**dvtable) (struct testobject_*);
    au_iarray atable;
} testobject ;


void testobject_increment(testobject * self) {
    self->counter ++;
}

#define TEST_INCREMENT_VTABLE_POS 171

void testobject_initialize(testobject * self) {
    self->counter                           = 0;
    self->increment                         = & testobject_increment;
    self->vtable[TEST_INCREMENT_VTABLE_POS] = & testobject_increment;
    self->dvtable = calloc(sizeof( void (*) (struct testobject_*)) , TEST_VTABLE_SIZE );
    self->dvtable[TEST_INCREMENT_VTABLE_POS] = & testobject_increment;
    au_iarray_initialize(&self->atable);
    au_iarray_set(&self->atable, TEST_INCREMENT_VTABLE_POS, &testobject_increment);
}

testobject * testobject_new() {
    testobject * result = calloc(sizeof(testobject), 1);
    testobject_initialize(result);
    return result;
}

void testobject_free(testobject * self) {
    free(self);    
}


void test_direct() {
    long            index;
    testobject      object;
    testobject *    self;    
    self            = & object;
    testobject_initialize(self);
    for(index = 0; index < 1000000; index ++) {
        testobject_increment(self);
    }
}


void test_indirect() {
    long            index;
    testobject      object;
    testobject *    self;    
    self            = & object;
    testobject_initialize(self);
    for(index = 0; index < 1000000; index ++) {
        self->increment(self);
    }
}



void test_once_stack_vtable() {
    long            index;
    long            vtable_increment;
    testobject      object;
    testobject *    self;    
    self            = & object;
    vtable_increment= TEST_INCREMENT_VTABLE_POS;
    testobject_initialize(self);
    
    for(index = 0; index < 1000000; index ++) {
        self->vtable[vtable_increment](self);
    }
}

void test_cached_vtable() {
    long            index;
    volatile long   vtable_increment;
    testobject      object;
    testobject *    self;    
    void (*increment) (testobject*);    
    self            = & object;
    vtable_increment= TEST_INCREMENT_VTABLE_POS; 
    testobject_initialize(self);
    increment = self->vtable[vtable_increment];
    for(index = 0; index < 1000000; index ++) {
        increment(self);
    }
}


void test_atable() {
    long            index;
    volatile long   vtable_increment;
    testobject      object;
    testobject *    self;    
    void (*increment) (testobject*);    
    self            = & object;
    vtable_increment= TEST_INCREMENT_VTABLE_POS; 
    testobject_initialize(self);    
    for(index = 0; index < 1000000; index ++) {
        increment = au_iarray_get(&self->atable, (volatile size_t)vtable_increment);
        increment(self);
    }
}


typedef struct au_stackframe {    
    void *    bottom;
    void *    top;
    void *    data;    
    ptrdiff_t delta;
    size_t    size;
} au_stackframe ; 


void au_save_stack(au_stackframe * stack, void * bottom_ptr, void * top_ptr) {
    int stack_top   = 987654321;
    stack->top      = top_ptr; 
    stack->bottom   = bottom_ptr;
    stack->delta    = stack->top - stack->bottom;
    printf("stack save: delta : %d, top %p, bottom %p \n", stack->delta, 
            stack->top, stack->bottom);
    if (stack->delta < 0) {
        stack->size = - stack->delta;
    } else {
        stack->size = stack->delta;
    }
    stack->data = calloc(stack->size, 1);
    if(!stack->data) { return; } 
    if (stack->delta < 0) {
        memmove(stack->data, stack->bottom   , stack->size);
    } else {
        memmove(stack->data, stack->top      , stack->size);
    }            
}


void au_load_stack(au_stackframe * stack, void * bottom_ptr, void * top_ptr) {
    int stack_top   = 123456789;
    /* void * top_ptr  = & stack_top; */
    if(!stack->data) { return; } 
    if (stack->delta < 0) {
        memmove(top_ptr , stack->data, stack->size);
    } else {
        memmove(bottom_ptr, stack->data   , stack->size);
    }            
    printf("stack_top: loaded %d size : %d \n", stack_top, stack->size);
}

void test_stack_load_save() {
    au_stackframe * stack  = calloc(sizeof(au_stackframe), 1);   
    /* This wdoesn't work so well due to the fact that the botom and 
     * top cannot be determined correctly like this,  
     * as the compiler may reorder the definitions.
     */
    char bottom = 123;
    int foo = 10;
    int quux= 70;
    int bar = 20;    
    char top = 321;
    printf("Internaal diff: %d\n", (&top) -  (&bottom));    
    au_save_stack(stack, &top, &bottom);
    printf("saved: %d %d\n", foo, bar);    
    foo     = 30;
    bar     = 40;    
    quux    = 100;
    au_load_stack(stack, &top, &bottom);
    printf("loaded: %d %d %d\n", foo, bar, quux);
}




#define TEST_REPEATS 1        
#define TEST_COUNT   5        

int main(int argc, char * argv[]) {
    au_mem_init();
    { 
        au_jmpbuf jumper = au_jmpbuf_new();
        au_str     aster = au_str_new("Starting au tests...");     
        int index, jndex;
        double d;
        double results [TEST_COUNT][TEST_REPEATS];
        
        puts(au_str_c(aster)); 
        test_stack_load_save();
        au_str_free(aster);
        
        for (index = 0; index < TEST_REPEATS; index++) { 
            d = func_profile(test_direct    , 200, 1000000, "Direct");
            results[0][index] = d;
            d = func_profile(test_indirect  , 200, 1000000, "Indirect");        
            results[1][index] = d;
            d = func_profile(test_once_stack_vtable, 200, 1000000, "Vtable once");
            results[2][index] = d;
            d = func_profile(test_cached_vtable , 200, 1000000, "Vtable cached");
            results[3][index] = d;
            d = func_profile(test_atable , 200, 1000000, "Atable nocache");
            results[4][index] = d;
        }
        
        for (jndex = 0; jndex < TEST_COUNT; jndex++) {
            for (index = 0; index < TEST_REPEATS; index++) {
                printf("%lf\t", results[jndex][index]);
            }
            printf("\n");
        }

        au_jmpbuf_free(jumper);
        puts("Done!");
        return 0;
    }    
}


#endif /* AU_TEST */


