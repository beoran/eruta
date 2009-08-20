#ifndef OI_MEMORY_H
#define OI_MEMORY_H


typedef void (*oi_free_func) (void *);

struct oi_memory_ {
  void         *ptr;
  long          ref;
  oi_free_func  free;
};


typedef struct oi_memory_   oi_memory;


oi_memory oi_mem_build(void * ptr, oi_free_func free);
          
oi_memory oi_ref_s(oi_memory);
void      oi_free_s(oi_memory);




#endif
