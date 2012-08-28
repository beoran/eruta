
#include "beo.h"
#include <ctype.h>





/* Beo is Beoran's own scripting language. Experimental, inspired by TCL,
Picol, LIL, and (UN)ICON. Unlike TCL, not everything will be a string for 
performance reasons. There will be string, but also integers, doubles, 
user data, and lists of values. Furthermore, there will be no expr 
sub-language, at least not initially, but some commands will later use a
shunting yard algorithm to go from infix to prefix notation. Also, 
it will reuse several C libraries used in Eruta, namely allegro, dynar, hatab,
silli, and especially allegro's utf-8 strings ... 
*/



/** A union with the possible data values for a BeoValue. */
union BeoValueUnion_ {
  long              l; // long integer
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


#define BEO_TYPE_P(VALUE, TYPE) beo_type_p((VALUE), (TYPE))
#define BEO_STRING_P(VALUE) BEO_TYPE_P((VALUE), BEO_STRING)
#define BEO_FAIL_P(VALUE)   BEO_TYPE_P((VALUE), BEO_FAIL)
#define BEO_OK_P(VALUE)     BEO_TYPE_P((VALUE), BEO_OK)
#define BEO_LONG_P(VALUE)   BEO_TYPE_P((VALUE), BEO_LONG)
#define BEO_DOUBLE_P(VALUE) BEO_TYPE_P((VALUE), BEO_DOUBLE)
#define BEO_STRING_P(VALUE) BEO_TYPE_P((VALUE), BEO_STRING)
#define BEO_LIST_P(VALUE)   BEO_TYPE_P((VALUE), BEO_LIST)
#define BEO_DATA_P(VALUE)   BEO_TYPE_P((VALUE), BEO_DATAPTR)
#define BEO_FUNC_P(VALUE)   BEO_TYPE_P((VALUE), BEO_FUNCPTR)


/** A BeoValue is the type for any value data inside the beo interpreter.
*   Implemented as a tagged union with a reference count.
*/
struct BeoValue_ {
  enum  BeoValueType_   type; // type of the value
  union BeoValueUnion_  data; // data 
  int                   refs; // reference counting
};

/* Low level initializer for a beovalue. */
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


/** returns nonzero if the value is of the given type, zero if not. 
Also returns 0 if value is NULL. */
int beo_type_p(BeoValue * value, BeoValueType type) { 
  if(!value) return 0;
  return (value->type == type);
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

/** Initializes a beovalue with a long value */
BeoValue * beovalue_initlong(BeoValue * value, long l) {
  BeoValueUnion data; 
  data.l = l;
  return beovalue_initall(value, BEO_LONG, data);
}

/** Creates a new beovalue with an long inside it. 
Must be freed with beovalue_free_destroy */
BeoValue * beovalue_newlong(long l){
  return beovalue_initlong(beovalue_alloc(), l); 
}

/** Initializes a beovalue with a double value */
BeoValue * beovalue_initdouble(BeoValue * value, double d) {
  BeoValueUnion data; 
  data.d = d;
  return beovalue_initall(value, BEO_LONG, data);
}

/** Creates a new beovalue with a double inside it. 
Must be freed with beovalue_free_destroy */
BeoValue * beovalue_newdouble(double d){
  return beovalue_initdouble(beovalue_alloc(), d); 
}


/** Initializes a beovalue with a c string */
BeoValue * beovalue_initstr(BeoValue * value, char * s) {
  BeoValueUnion data; 
  data.s = s ? ustr_new(s) : NULL;
  return beovalue_initall(value, BEO_STRING, data);
}

/** Creates a new beovalue with an string inside it. 
Must be freed with beovalue_free_destroy */
BeoValue * beovalue_newstr(char * s){
  return beovalue_initstr(beovalue_alloc(), s); 
}

/** Initializes a beovalue with a unicode string. 
The unicode string will be duplicated internally except if u is NULL,
so u must still be freed with ustr_free if it was allocated. */
BeoValue * beovalue_initustr(BeoValue * value, USTR * u) {
  BeoValueUnion data; 
  data.s = u ? ustr_dup(u) : NULL;
  return beovalue_initall(value, BEO_STRING, data);
}

/** Creates a new beovalue with an string inside it. 
Must be freed with beovalue_free_destroy */
BeoValue * beovalue_newustr(USTR * s){
  return beovalue_initustr(beovalue_alloc(), s); 
}

/** Creates a new fail value with an (optional) error string. */
BeoValue * beovalue_initfail(BeoValue * value, char * error) {
  BeoValueUnion data; 
  data.s = error ? ustr_new(error) : NULL;
  return beovalue_initall(value, BEO_FAIL, data);
}

/** Creates a new beovalue with an FAIL inside it. 
Must be freed with beovalue_free_destroy */
BeoValue * beovalue_newfail(char * error){
  return beovalue_initfail(beovalue_alloc(), error); 
}

/* Initializes a new OK value. */
BeoValue * beovalue_initok(BeoValue * value) {
  BeoValueUnion data; 
  data.l = 1; // ensure data value is non-zero.
  return beovalue_initall(value, BEO_OK, data);
}

/** Creates a new beovalue with an OK value inside it. 
Must be freed with beovalue_free_destroy */
BeoValue * beovalue_newok(){
  return beovalue_initok(beovalue_alloc()); 
}

/** Creates a new beovalue with a pointer inside it. */
BeoValue * beovalue_initptr(BeoValue * value, void * ptr) { 
  BeoValueUnion data; 
  data.p = ptr;
  return beovalue_initall(value, BEO_DATAPTR, data);
}

/** Creates a new beovalue with a pointer inside it. 
Must be freed with beovalue_free_destroy */
BeoValue * beovalue_newptr(void * ptr){
  return beovalue_initptr(beovalue_alloc(), ptr); 
} 


/** Converts the BeoValue to a long. Stores trtue in ok if ok. 
Stores false in OK and returns 0 in case there was a conversion error. 
OK is ignored if it is NULL. */
double beovalue_getdouble(BeoValue * value, int * ok) {
  if(BEO_DOUBLE_P(value)) return value->data.d;
  if(BEO_STRING_P(value)) return ustr_tod(value->data.s, ok);
  if(ok) *ok = FALSE;
  return 0;
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

struct BeoFrame_;
typedef struct BeoFrame_ BeoFrame;

/* A frame is the environment in which a Beo function or 
script runs. */
struct BeoFrame_ {
  BeoFrame        * parent; // parent frame, NULL if toplevel 
  Hatab           * vars;   // variables, including function arguments
  BeoValue        * result; // result of the function call  
};



BeoFrame * beoframe_alloc() {
  return STRUCT_ALLOC(BeoFrame);
}

BeoFrame * beoframe_init(BeoFrame * self, BeoFrame * parent) {
  if(!self) return NULL;
  self->parent = parent;
  self->vars   = hatab_new(NULL); // default cstring hash table 
  self->result = NULL;
  return self;
}

BeoFrame * beoframe_new(BeoFrame * parent){
  return beoframe_init(beoframe_alloc(), parent);
}

/* Sets the value value as the result of the current frame.
Increases the reference count of value. */
BeoValue * beoframe_result_(BeoFrame * self, BeoValue * value) {
  if(!self) return NULL;
  return (self->result = (BeoValue *)beo_ref(value));  
}

/* Sets a variable n the current frame with the given name. 
Does NOT increase the reference count.
*/
BeoValue * beoframe_localvar_(BeoFrame * self, char * name, BeoValue * value) {
  return (BeoValue *) hatab_put(self->vars, name, value);
}

/* Gets a variable in the current frame. Null if not found.  */ 
BeoValue * beoframe_localvar(BeoFrame * self, char * name) {
  if(!self) return NULL;
  return (BeoValue *) hatab_get(self->vars, name); 
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
  Hatab     * funcs;
  BeoFrame  * top;
  // a hash map for function lookup. 
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




