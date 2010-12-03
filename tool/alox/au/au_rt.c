/**
* AU, a minimal, tiny, re-entrant, mostly ANSI 89 C  compatible runtime for a 
* dynamic dispatch language like Ruby. The ultimate goal is to simplify 
* compilation of  such languages to C.
*
* Bjorn De Meyer, 2008.
*
* Released under the zlib license. 
*
*/


#include "au_rt.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <setjmp.h>

/* Macro's for local continuations, using a variation of the (in)famous Duff's Device.  */
#define AU_CONT_INIT(cont)    cont = 0;
#define AU_CONT_RESUME(cont)  switch(cont) { case 0:
#define AU_CONT_SET(cont)     cont = __LINE__; case __LINE__:
#define AU_CONT_END(cont)     }


/*
* Portable coroutines
*/
struct au_coro_s { 
  int line;
};  

#define au_coro_local_begin struct au_coro_context { int line;  
#define au_coro_local_end(context) } * context; 

/*
* Lamport's bakery algorithm for locking threads.
*/

struct au_thread_s {
  volatile int     entering;
  volatile int     number;
  au_id            lowlevel;
};

typedef struct au_thread_s au_thread; 

int au_lock(int pid, au_thread * threads, int amount) {
  volatile au_thread *  thread;
  volatile int          max = 0;
  volatile int          i;
  thread                = threads + pid;
  thread->entering      = 1; 
  
  /* Get the highest number and add one.      */
  for(i=0; i< amount; i++) {
    int tn = threads[i].number;
    if(tn > max) { max = tn; } 
  }
  thread->number        = max + 1;
  /* Current thread gets one more than the current maximum number */
  thread->entering      = 0;
  /* Thread is past entering the bakery. */
  for (i=0; i< amount; i++) {
    volatile au_thread *  other;
    other               = threads + i;
    /* Wait until the other thread recieves it's number. */  
    while(other->entering) { /* Busy wait. Do nothing. */ }
    while(other->number != 0 && (other->number < thread->number)) { 
    /* While the other is still being served or waiting to do so, and it's priority is higher. (Number is lower.) */
    /* Busy wait. Do nothing. */
    }     
  }
  return 1;   
}
 

void au_unlock(int pid, au_thread * threads) {
  threads[pid].number = 0; 
}


/* Heap-less pure ANSI C Exception handling. Data structures. */
struct au_exception_s {
  jmp_buf                 jump;
  struct au_exception_s * parent;
  int                     amount;
  int                     status;
  au_id                   exception;
};

typedef struct au_exception_s au_exception; 

/* Hash table */


struct au_hashcell_s {
  au_id                  key;
  au_id                  value;
  struct au_hashcell_s*  next;
};

typedef struct au_hashcell_s au_hashcell;

struct au_hashbucket_s {
  au_hashcell *   first;
  au_hashcell *   last;
};

typedef struct au_hashbucket_s au_hashbucket;

struct au_hash_s {
  au_id           self;
  long            size;
  au_hashbucket*  buckets;
};

typedef struct au_hash_s au_hash;

/* Memory management */

struct au_meminfo_s {
  struct au_meminfo *  parent;
  struct au_meminfo ** children;
  char          hello[9];
  unsigned char magic;
  unsigned char flags;
  void *        pointer;
};

typedef struct au_meminfo_s au_meminfo;


/* Complete run-time state for one thread of excecution. */

struct au_state_s {
  au_exception  exception; /* Current exception state. */
  au_meminfo    memory;    /* Current memory state.    */
};

typedef struct au_state_s au_state;



/* Exception handling. Implementation functions and macros. */

/* 
* Auxiliary setjmp and  longjmp functions that should avoid any with local variables when jumping. 
*/
int au_setjmp_aux(jmp_buf buffer) {
  if(setjmp(buffer)) {  return !0; } else { return 0; }
}

int au_longjmp_aux(jmp_buf buffer) {
  longjmp(buffer, 1);
}






#define AU_MEM_MARK   1
#define AU_MEM_FREEZE 2
#define AU_MEM_MAGIC  0xa0

void * au_alloc(size_t size) {
  void        * ptr;
  char        * p;
  au_meminfo  * info;
  
  ptr         = malloc(size + sizeof(au_meminfo)); /* Allocate extra size for meminfo */
  info        = (au_meminfo *) ptr;
  info->magic = AU_MEM_MAGIC; 
  info->flags = 0;
  strcpy(info->hello, "Mem!");
  p           = (char*) info;
  return p + sizeof(au_meminfo);  
} 

void au_kill(void * ptr) {
  char        *p;
  au_meminfo  * info;
  p           = (char *) ptr;
  p          -= sizeof(au_meminfo);
  info        = (au_meminfo *) p; 
  if (info->magic == AU_MEM_MAGIC) { 
    puts(info->hello);
    free(info);
  } else {
    puts("Tried to free corrupted block of memory");
  }
}  


void * au_malloc(size_t size) {
  return malloc(size);
}

void * au_realloc(void * pointer, size_t size) {
  return realloc(pointer, size);
}

void au_free(void * pointer) {
  free(pointer);
}

#define au_make(type, amount) ((type *) au_malloc(sizeof(type) * amount))

au_id au_cstr2id(const char * cstr) {
  return (au_id)(cstr);
}

const char * au_id2cstr(au_id value) {
  return (const char *)(value);
}


/* Ultra-simple, non-rehashable hash table. Implementation functions. */

#define AU_HASH_SIZE 73

au_hash * au_hash_init(au_hash *hash) {
   hash->size     = AU_HASH_SIZE; /* Small prime. */ 
   hash->buckets  = au_make(au_hashbucket, hash->size ); /* Allocate buckets. */  
   return hash ; 
} 

long au_hash_cstr(const char * s) {
  long hash = 0;
  while (1)
    {
      if  (*s == '\0')   break;
      else hash ^= *s++;
      if  (*s == '\0')   break;
      else  hash ^= (*s++ << 8);
      if  (*s == '\0')   break;
      else hash ^= (*s++ << 16);
      if  (*s == '\0')   break;
      else hash ^= (*s++ << 24);
    }
  return hash;
}


long au_hash_id(au_id key) {
  return au_hash_cstr(au_cstr2id(key));
}

au_hash * au_hash_set(au_hash * hash, au_id key, au_id value) {
  long            index;
  const char *    keystr;
  au_hashbucket * bucket;
  au_hashcell *   cell;
  
  keystr      = au_cstr2id(key);
  index       = au_hash_cstr(keystr) % hash->size;
  bucket      = hash->buckets + index;
  printf("%ld\n", sizeof(au_hashcell));
  cell        = (au_hashcell *) au_malloc(sizeof(au_hashcell));
  cell->key   = key;
  cell->value = value;
  cell->next  = 0;
  
  if(!bucket->first) { 
    bucket->last = bucket->first = cell;
  } else {
    bucket->last->next = cell;
    bucket->last       = cell;
  }  
   
  return hash;  
}

au_id au_hash_get(au_hash * hash, au_id key) {
  long            index;
  const char *    keystr;
  au_hashbucket * bucket;
  au_hashcell *   cell;
  
  keystr      = au_cstr2id(key);
  index       = au_hash_cstr(keystr) % hash->size;
  bucket      = hash->buckets + index;
  if(!bucket->first) { 
    return (au_id)0;
  } else {  
    /* Iterate over cells in the bucket. */
    for(cell = bucket->first; cell; cell = cell->next) {
      const char * cellstr;
      cellstr = au_cstr2id(key);
      if(strcmp(cellstr, keystr) == 0) return cell->value; 
    }
  }  
  return (au_id)0; 
}




void au_start() {
  /*GC_INIT();*/
}

void test_longjmp(jmp_buf jump) {
  puts("Ready for jump!");
  longjmp(jump, 1); 
}

void test_stackmem_2(char * str) {
  printf("Stackmem: %s\n", str);
}

void test_stackmem() {
  char stackmem[1024];
  sprintf(stackmem, "Hello, stackmem!");
  printf("Stackmem: %x, %s\n", stackmem, stackmem);
  /* test_stackmem(); */
}

struct au_object_s {
  au_id     self;
  au_id     klass;
  au_hash   messages;
  au_hash   variables;
  /* Dynamic variables that this object uses to store data in. */
  void *    lowlevel;
  /* Low level C struct that we are wrapping. */
};

typedef struct au_object_s au_object;

au_object * au_object_init(au_object * object, au_id klass, void * lowlevel) {
  object->self      = (au_id) object;
  object->klass     = klass;   
  object->lowlevel  = lowlevel;   
  au_hash_init(&object->messages);
  au_hash_init(&object->variables);
  return object;
}

au_object * au_object_send(au_object * object, au_id message, int argc, au_id * argv )  {
  au_id  implementation = au_hash_get(&object->messages, message);  
  

}


int main(void) {
  au_id val;
  const char *  str    = "Hello!";
  const char *  key    = "key";
  const char *  value  = "Value!";
  char *        aid;
  au_hash       hash;
  jmp_buf       jump;
  int           aux = 0;
  aux           = 11;
  test_stackmem();
  
  aid                  = (char *) au_alloc(20);
  au_kill(aid);

  /* aid                 = GC_strdup(str); */
  au_start();  
  au_hash_init(&hash);
  au_hash_set(&hash, au_cstr2id(key), au_cstr2id(value) );
  
  val = au_cstr2id(str);
  puts(au_id2cstr(val));
  printf("%s %ld\n", str, (long) str );
  puts( au_hash_get(&hash, au_cstr2id(key)));
  return 0;
}

