#include <stdlib.h>
#include <stdio.h>

#define OR_TAG_RUNTIME   (1<<10)
#define OR_RETSTACK_SIZE (1<<10)


/* Forward declarations. */
union OR_ALL_PTR_;
struct OR_RUNTIME_;
typedef struct OR_RUNTIME_ OR_RUNTIME;
typedef union OR_ALL_PTR_ OR_ALL_PTR;

typedef void * OR_DATA_PTR;
typedef OR_DATA_PTR (*OR_FUNC_PTR)(OR_RUNTIME * rt, int argc, OR_ALL_PTR * args);

union OR_ALL_PTR_ {
  OR_DATA_PTR data;
  OR_FUNC_PTR func;
};


struct OR_RUNTIME_ {
  int           tag;
  int           sp;
  OR_FUNC_PTR   next;
  OR_FUNC_PTR   stack[OR_RETSTACK_SIZE];
};




void * or_runtime_init(OR_RUNTIME * self) {
  int index;
  self->tag  = OR_TAG_RUNTIME;
  self->next = NULL;
  for ( index = 0; index < OR_RETSTACK_SIZE ; index++) {  
    self->stack[index] = NULL;
  }
  return self;  
} 

void * or_runtime_pushnext(OR_RUNTIME * self, OR_FUNC_PTR func) {
  int index;
  self->sp++;
  if (self->sp >= OR_RETSTACK_SIZE) { 
    self->sp = OR_RETSTACK_SIZE - 1;
    return NULL;  
  } 
  self->stack[self->sp]  = func;
  self->next               = self->stack[self->sp];
  return self;  
} 

void * or_runtime_popnext(OR_RUNTIME * self) {
  int index;
  self->sp--;
  if (self->sp < 0) { 
    self->next = 0;
    return NULL;
  } 
  self->next                    = self->stack[self->sp];
  return self;  
} 

OR_DATA_PTR or_runtime_next(OR_RUNTIME * self, int argc, OR_ALL_PTR * args) {
  OR_FUNC_PTR next = self->next;  
  if(!next) { return NULL; }  
  or_runtime_popnext(self);
  return next(self, argc, args);
}

OR_DATA_PTR or_runtime_run(OR_RUNTIME * self, int argc, OR_ALL_PTR * args) {
  OR_DATA_PTR result  = NULL;
  OR_FUNC_PTR next    = self->next;  
  while (next) { 
    result  = or_runtime_next(self, argc, args),
    next    = self->next;
  } 
  return result;  
}


void * world (OR_RUNTIME * rt, int argc, OR_ALL_PTR * args) {
  puts("Hello, world orhalrun!");
  return NULL;
}


void * hello (OR_RUNTIME * rt, int argc, OR_ALL_PTR * args) {
  puts("Hello, orhalrun!");
  or_runtime_pushnext(rt, world);
  return NULL;
}


int main(int argc, char * argv[]) {  
  OR_RUNTIME runtime;
  or_runtime_init(&runtime);
  or_runtime_pushnext(&runtime, hello);
  or_runtime_run(&runtime, 0, NULL);
  return 0;
}
