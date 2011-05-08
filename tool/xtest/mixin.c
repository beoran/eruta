#include <stdlib.h>
#include <stdio.h>
#include <stddef.h>

struct Ref_ {
  volatile int count;
  void (*free) (void *ptr);
};



struct Mixed_ {
  int data;
};

typedef struct Mixed_ Mixed;

struct Parent_ {
  int data;
  Mixed mixed;
};

typedef struct Parent_ Parent;


#define MIX_PARENT(PARENT_TYPE, SELF, MEMBER) \
  ((PARENT_TYPE *)(((char *)SELF) - offsetof(PARENT_TYPE, MEMBER)))


typedef struct Ref_ Ref;

Ref * ref_init(Ref * ref, void (*free) (void *ptr)) {
  ref->count = 0;
  ref->free  = free;
  return ref;
}

Ref * ref_use(Ref * ref) {
  ref->count+= 1;
  return ref;
}

Ref * ref_toss(Ref * ref) {
  ref->count -= 1;
  if (ref <1) {
    return NULL;
  }
  return ref;
}



int main(void) {
  Parent parent;
  Parent * parptr = MIX_PARENT(Parent, &parent.data, data);
  parent.data = 0;
  parent.mixed.data = 0;
  printf("%p %p %p\n", &parent, &parent.mixed, parptr);
  if (parptr == &parent) puts("OK!");
  return 0;
}