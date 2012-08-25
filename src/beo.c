
#include "beo.h"





/* Beo is Beoran's own scripting language. Experimental, inspired by TCL,
Picol, LIL, and (UN)ICON. Unlike TCL, not everything will be a string for performance reasons. There will be string, but also integers, doubles, 
user data, and lists of values. Furthermore, there will be no expr sub-language,
at least not initially, but some commands will later use a shunting yard 
algorithm to go from infix to prefix notation. Also, 
it will reuse several C libraries used in Eruta, namely allegro, dynar, hatab,
silli, ... 

             
*/

#if COMMENT___

This is the formal syntax I'd like to implement:

script     ::= command ? (eol script )
eol        ::= ( “\n” | “;” )             /* End of line, can be escaped with \ */
command    ::= comment
             | word ( space word )*       
             /* sequence of space-separated words; first is command name */
             |                            /* empty */
comment    ::= “#” characters “\n”        /* comment */             
word       ::= block               
             | "%{" characters "}" /* Rew, uninterpolated string */ 
             | “"” charSubsts “"”  /* double-quotes must be balanced */
             | charSubsts          /* braces must be balanced */
block      ::= '{' script '}' 
charSubsts ::= “[” script “]” charSubsts? /* brackets must be balanced */
             | “$” varName charSubsts?
             | “${” varName “}” charSubsts?
             | “\\” escapeSequence charSubsts?
             | ordinaryChar charSubsts?
#endif

/** A union with the possible data values for a BeoValue. */
union BeoValueUnion_ {
  long              i; // long integer
  double            d; // double precision float
  USTR          *   s; // strings
  void          *   p; // pointer, may be use for lists, etc.
  void          * (*f)(void *); 
  // function pointer, since they are NOT neccesarily compatible with void *
};


/** Possible types for the BeoValue */
enum BeoValueType_ {
  BEO_FAIL    = 0,
  // Used to indicate errors or failures, in (un)icon language style.
  // in this case, the DataUnion string s will be set with the error message.
  // there is no false or nil, all is FAIL
  BEO_OK      = 1,
  // OK, only boolen type with tru-like value 
  // (FAIL is always used in stead of false)
  BEO_LONG    = 2,
  BEO_DOUBLE  = 3,
  BEO_STRING  = 4,
  BEO_LIST    = 5, // uses p to store the address of the list. 
  // Lists are normally Dynars.
  BEO_FUNCPTR = 6,
  // function pointer
  BEO_DATAPTR = 7
  // data pointer
};


#define BEO_STRING_P(VALUE) ((VALUE)->type == BEO_STRING)
#define BEO_FAIL_P(VALUE)   ((VALUE)->type == BEO_FAIL)
#define BEO_OK_P(VALUE)     ((VALUE)->type == BEO_OK)
#define BEO_LONG_P(VALUE)   ((VALUE)->type == BEO_LONG)
#define BEO_DOUBLE_P(VALUE) ((VALUE)->type == BEO_DOUBLE)
#define BEO_STRING_P(VALUE) ((VALUE)->type == BEO_STRING)
#define BEO_LIST_P(VALUE)   ((VALUE)->type == BEO_LIST)
#define BEO_DATA_P(VALUE)   ((VALUE)->type == BEO_DATAPTR)
#define BEO_FUNC_P(VALUE)   ((VALUE)->type == BEO_FUNCPTR)


/** A BeoValue is the type for any value data inside the beo interpreter.
*   Implemented as a tagged union with a reference count.
*/
struct BeoValue_ {
  enum  BeoValueType_   type; // type of the value
  union BeoValueUnion_  data; // data 
  int                   refs; // reference counting
};


BeoValue * beovalue_initall(BeoValue * self, BeoValueType type, 
                                             BeoValueUnion data) {
  if(!self) return NULL;
  self->refs = 1;
  self->type = type;
  self->data = data;
  return self;
}

BeoValue * beovalue_alloc() {
  return STRUCT_ALLOC(BeoValue);
}

/** Constructor for beovalue. */
BeoValue * beovalue_newall(BeoValueType type, BeoValueUnion data) {
  BeoValue * self = beovalue_alloc();
  return beovalue_initall(self, type, data);
}

/** Frees the beovalue uses the given destructor to clean up the data 
too. Can handle strings on it's own.  */
BeoValue * beovalue_free_destroy(BeoValue * self, MemDestructor * destroy) {
  if(!self) return NULL;
  if(destroy) { destroy(self); } 
  if(BEO_STRING_P(self)) { ustr_free(self->data.s); }
  return mem_free(self);
}


/* Reference counting of BeoValue */

/** Increases the refence count of self and returns self.  */
BeoValue * beo_ref(BeoValue * self) {
  if(!self) return self;
  self->refs++;
  return self;
}

/** Decreses the reference count. Calls MemDestructor if it's not null. 
Finally calls beovalue_free. Retuns NULL if the value was finalized, 
not-null if not. */
BeoValue * beo_unref(BeoValue * self, MemDestructor destroy) {
  if(!self) return self;
  self->refs--;
  if(self->refs <= 0 ) {
    return beovalue_free_destroy(self, destroy);
  }
  return self;
}


/* A block is a list of commands to be executed in sequence */
struct BeoBlock_ {
  Dynar * commands;
  int     used;
};


/* a command consists of a name followed by 
zero or more arguments and zero or more blocks. */
struct BeoCommand_ {
  USTR    name;
  Dynar * arguments;
  int     argc;
  Dynar * blocks;
  int     blockc;
};


/** The beo interpreter. */

struct Beo_ {
  Dynar * heap;
  // a dynamic array that models the heap memory of the interpreter
  Dynar * stack;
  // a dynamic array that models the stack memory of the interpreter
  Hatab * dict;
  // a hash map for variable and function lookup. 
  int sp;           // stack pointer.
  int hp;           // heap pointer.
  int sd;           // stack depth counter.
  int line;         // line counter.
  int col;          // column counter.
  USTR    program;  // current program that is being parsed.

};

/* Ideas on the way the interpreter should work: 

Basic types:
1       : integer
1.2     : double
"1.2"   : string
'1.2'   : also a string
[[1.2]] : also a string 
:hi     : also a string, to be able to use them as "symbols"
hi      : variable or command

The interpreter splits the input text into a list of commands. 
A command is a named command to execute, followed by an argument list,
and possibly followed by one or more blocks. Blocks are opened with 
( {  do, then, begin, else and closed with ) } end . Blocks are parsed
recursively as a command list.

# comment
set a 10
# can also use, to separate 
set a, 10
set b (sum 10 20)

def add (:a :b) do 
  # all arguments are passed by reference
  set a (sum a b)
end

if (< a b) then
  print "A is smallest" a
end
else
  print "B is smallest" b
end


Normally a commands are separated by a newline character, but 
this newline is ignored if :
1) Then newline is escaped with \ 
2) Before the newline, a block was opened with (, { , do, then, begin
3) Before the newline, the parameter list, which ends on , 
   is not finished (so a comma before the newline, possibly with spaces in 
   between)
4) After the newline, a block is opened with (, {, do, then, else, but  
   not with begin,

A ; always separates commands.

*/




