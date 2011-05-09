#include <stdlib.h>
#include <stddef.h>
#include <stdio.h>

struct BrMix_;

typedef  void *  (BrFreeFunc)  (void *ptr);
typedef  void *  (BrAllocFunc) (struct BrMix_ * self);


struct BrMix_ {
  size_t          size  ;
  struct BrMix_ * parent;
  BrFreeFunc    * free;
  BrAllocFunc   * alloc;
};

typedef struct BrMix_ BrMix;

struct BrRef_ {
  volatile int count;
  BrMix      * mix;
};

typedef struct BrRef_ BrRef;

struct BrRefMix_ {
  BrMix parent;
};

typedef struct BrRefMix_ BrRefMix;

const BrRefMix BrRefMixInst = { sizeof(BrRef), NULL, NULL }; 

BrRef * BrRef_init(BrRef * ref) {
  ref->mix = (BrMix *) &BrRefMixInst;
}

struct BrThing_ {
  BrMix      * mix;
  volatile int ref;
};

typedef struct BrThing_ BrThing;

typedef void * BrPtr;
typedef void * (*BrFuncPtr)(void *);


 union BrValueUnion_ {
    double    number;
    int32_t   integer;
    BrFuncPtr function;
    BrPtr     data;
 };

typedef union BrValueUnion_ BrValueUnion;

struct BrValue_ {
  volatile int ref;
  BrPtr        mix;
  BrValueUnion value;
  char         kind;
};

typedef struct BrValue_ BrValue;



void * BrThingMix_free(void * obj) {
  if (!obj) return NULL;
  free(obj);
  return NULL;
}

void * BrThingMix_alloc(BrMix * mix) {
  if(!mix) return NULL;
  return calloc(mix->size, 1);
}

const BrRefMix BrThingMixInst = { sizeof(BrThing),
                                  NULL, 
                                  (BrFreeFunc*)  BrThingMix_free, 
                                  (BrAllocFunc*) BrThingMix_alloc 
                                };


BrThing * BrThing_alloc(BrMix * mix) {
  if(!mix) return NULL;
  return (BrThing *) mix->alloc(mix);
}


BrThing * BrThing_init(BrThing * obj, BrMix * mix) {
  if(!obj) return NULL;
  obj->mix = mix;
  obj->ref = 1;
  return obj;
}

BrThing * BrThing_new(BrMix * mix) {
  return BrThing_init(BrThing_alloc(mix));
}

BrMix * BrThing_mix(BrThing * obj) {
  if(!obj) return NULL;
  return obj->mix;
}
  
BrThing * BrThing_use(BrThing * obj) {
  if(!obj) return NULL;
  obj->ref ++;
  return obj;
}

BrThing * BrThing_free(BrThing * obj) {
  if (!obj) return NULL;
  if (!obj->mix) return obj;
  if (!obj->mix->free) return obj;
  return (BrThing *) obj->mix->free(obj);
}
  
BrThing * BrThing_toss(BrThing * obj) {
  if(!obj) return NULL;
  obj->ref--;
  if (obj->ref < 1) {
    return BrThing_free(obj);
  }
  return obj;
}

void * Br_use(void * ptr) {
  return (void *) BrThing_use((BrThing *)ptr);
}
 
void * Br_toss(void * ptr) {
  return (void *) BrThing_use((BrThing *)ptr);
}
 
 
int main(void) {
  printf("%d %d \n", sizeof(BrValueUnion), sizeof(BrValue));
} 





