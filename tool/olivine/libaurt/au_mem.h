/* Memory and garbage collection support for the run time library for OI language. */
#ifndef _OI_MEM
#define _OI_MEM

#include <stdlib.h>
#include "au.h"

struct au_mem;

typedef struct au_mem_  au_mem;
typedef size_t          au_size_t; 
typedef ptrdiff_t       au_ptrdiff_t;

/* Destructors. They return a pointer to allow destructor chaining. 
  If the desctuctor returns NULL, no further chaining is needed. 
  If it returns non-null, chained destructors may be run. 
*/
typedef void *        (*au_free_func) (void *);

struct au_mem_ {
  void         * ptr;
  long           ref;
  au_free_func   free;
};


AU_API(void *)  au_malloc         (au_size_t    size);
AU_API(void *)  au_free           (void *  ptr      );
AU_API(void *)  au_realloc        (void ** ptr, au_size_t size);


AU_API(au_mem *) au_mem_alloc     (void);

AU_API(au_mem *) au_mem_new       (au_size_t size , au_free_func   destructor);

AU_API(au_mem *) au_mem_malloc    (au_mem * self  , au_size_t     size      );
AU_API(au_mem *) au_mem_wrap      (au_mem * self  , void *        pointer   );
AU_API(au_mem *) au_mem_free_set  (au_mem * self  , au_free_func  destructor);

AU_API(au_mem *) au_mem_mallocnew (au_size_t size);
AU_API(au_mem *) au_mem_wrapnew   (void * pointer);
 



AU_API(au_mem *) au_mem_ref     (au_mem * self);

AU_API(au_mem *) au_mem_resize  (au_mem * self  , au_size_t     size);
AU_API(void *)   au_mem_ptr     (au_mem * self);
AU_API(char *)   au_mem_ptr_get (au_mem * self  , au_size_t offset);

AU_API(au_mem *) au_mem_free    (au_mem * self);





/*
OI_API oi_ptr oi_mem_new  (oi_size_t size);
OI_API oi_ptr oi_mem_free (oi_size_t size);
OI_API oi_ptr oi_mem_ref  (oi_ptr     ref);
OI_API oi_ptr oi_mem_free (oi_size_t size);
*/




#endif
