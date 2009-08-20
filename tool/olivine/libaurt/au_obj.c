#include <stdlib.h>
#include <stdio.h>

typedef struct au_runtime_  au_runtime;
typedef void *              (*au_funcptr) (void *);
typedef long                au_cell;

#define AU_PROGRAM_SIZE     80000
#define AU_RETURNSTACK_SIZE 8000
#define AU_DATASTACK_SIZE   80000

struct au_runtime_ {
  au_funcptr returnstack[AU_RETURNSTACK_SIZE];
  au_cell    datastack[AU_DATASTACK_SIZE];
  au_funcptr program[AU_PROGRAM_SIZE];
  long       retsp;
  long       sp;
  long       ip;
};

au_runtime * au_runtime_init(au_runtime * self) {
  self->retsp =  0;
  self->sp    =  0;
  self->ip    = -1;
  return self;
}

au_runtime * au_runtime_next(au_runtime * self) {
  au_funcptr run;
  self->ip++;
  if (self->ip > AU_PROGRAM_SIZE) { return NULL; }
  run = self->program[self->ip];
  if (!run) { return NULL; }
  run(self);
  return self;
}

void * try_hello(void * ptr) {
  puts("Hello");
  return ptr;
} 

/*
 Idea for winning space for identifiers: zcode
 z is the escape character 
 z01 is \001, q02 is \002 ... qff is \0ff
 zlz is a lowercase literal z
 zuz is an uppercase Z (for consistency?)
 zqu is ?
 zex is !
 zbo is |   "bitwise or"
 zba is &   "bitwise and"
 zla is &&  "logical and"
 zlo is ||  "logical or"
 zpl is +  
 zmi is -
 zmu is * 
 zdi is /
 zmo is %   "modulus"
 zpo is **  "power"
 zbx is ^   "bitwise xor 
 zlx is ^^  "logical xor"
 zsd is +=  "set decrement" 
 zse is =   "set" 
 zsi is +=  "set increment" 
 zsm is *=  "set multiply"
 zso is ||= "set or" 
 zss is <=> "space ship"
 zsv is /=  "set diVide"
 
 zlt is <
 zgt is > 
 zle is <=
 zge is >=
 zne is !=
 zeq is ==
 zcs is === "case sameness"
 znm is !~  "no match"
 zma is =~  "matches" 
 
 
 "!", "!=", "!~", "<", "<=", "<=>", "==", "===", "=~", ">", ">=
   
 foo_bar  -> foo_bar 
 quit!    -> quitzex
 is_a?    -> is_azqu
 ||       -> zorzor
 zz       -> zlzzlz
 
 
*/


int main(void) {
  au_runtime      runtime;
  au_runtime_init(&runtime);
  runtime.program[0] = try_hello;
  au_runtime_next(&runtime);
  
  
  
  
  /* 
  runtime               = malloc(sizeof(*runtime));
  free(runtime); 
  */  
  return 0;
}























