
#include "beo.h"
#include "str.h"
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


/** Converts the BeoValue to a double. Stores true in ok if ok. 
Stores false in OK and returns 0 in case there was a conversion error. 
OK is ignored if it is NULL. */
double beovalue_getdouble(BeoValue * value, int * ok) {
  if(BEO_DOUBLE_P(value)) return BEO_TODOUBLE(value);
  if(BEO_LONG_P(value))   return (double)BEO_TOLONG(value);
  if(BEO_STRING_P(value)) return ustr_tod(value->data.s, ok);
  if(ok) *ok = FALSE;
  return 0;
}

/** Converts the BeoValue to a long. Stores true in ok if ok. 
Stores false in OK and returns 0 in case there was a conversion error. 
OK is ignored if it is NULL. Decimal numbers are assumed. */
long beovalue_getlong(BeoValue * value, int * ok) {
  if(BEO_LONG_P(value))   return BEO_TOLONG(value);
  if(BEO_DOUBLE_P(value)) return (long)BEO_TODOUBLE(value);
  if(BEO_STRING_P(value)) return ustr_tol(value->data.s, ok, 10);
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

/* Fress the frame, burt not it's parent. */
BeoFrame * beoframe_free(BeoFrame * self) {
  if(!self) return NULL;
  self->parent = NULL;
  self->vars   = hatab_free(self->vars);
  self->result = beo_unref(self->result, NULL);
  return mem_free(self);
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

/* Gets the value value of the result of the current frame.
Does not increase the refcount. */
BeoValue * beoframe_result(BeoFrame * self, BeoValue * value) {
  if(!self) return NULL;
  return (self->result);  
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


/* A command consists of a name followed by 
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


Beo * beo_register(Beo * self, char * name, BeoFunction * func) {
  hatab_put(self->funcs, name, func);
  // XXX: hatab has a bit of a problem: it does not own it's keys
  // so, if the keys are not character constants, problems
  // may occur. 
  return NULL;
}


/* Struct to store rules for the lexer */
struct BeoLexerRule_ {
  int     before_state; 
  char *  expression;
  int     action;
  int     after_state;
  int     token_type;
};

typedef struct BeoLexerRule_ BeoLexerRule;

/* Struct for the lexer's tokens. */
struct BeoLexerToken_ {
  int    kind; // Kind of token
  USTR * text; // text value of the token 
};
typedef struct BeoLexerToken_ BeoLexerToken;

/* Struct to store the lexer's state */
struct BeoLexer_ { 
  BeoLexerRule  * rules; // Rules of the lexer
  int             state; // Current state of the lexer.
  USTR          * text ; // String value of the token that is under construction  
};
typedef struct BeoLexer_ BeoLexer;

enum BeoLexerActions_ {
  BEO_LEXER_SKIP    = 1,
  BEO_LEXER_STORE   = 2,
  BEO_LEXER_OK      = 3,  
};

enum BeoLexerState_ {
  BEO_LEXER_DONE           ,
  BEO_LEXER_START          ,
  BEO_LEXER_COMMENTSTART   ,
  BEO_LEXER_COMMENTBLOCK   ,  
  BEO_LEXER_COMMENTLINE    ,  
  BEO_LEXER_COMMENTEND     ,  
  BEO_LEXER_BLOCKOPEN      ,  
  BEO_LEXER_BLOCKCLOSE     ,  
  BEO_LEXER_LISTOPEN       ,
  BEO_LEXER_LISTCLOSE      , 
  BEO_LEXER_EOL            , 
  BEO_LEXER_NUMBER_OR_WORD , 
  BEO_LEXER_NUMBER         , 
  BEO_LEXER_NUMBEREND      , 
  BEO_LEXER_WORD           ,
  BEO_LEXER_WORDEND        , 
  BEO_LEXER_STRINGDQ       , 
  BEO_LEXER_STRINGDQEND    , 
  BEO_LEXER_STRINGDQESC    , 
  BEO_LEXER_STRINGSQ       , 
  BEO_LEXER_STRINGSQEND    , 
  BEO_LEXER_STRINGBT       , 
  BEO_LEXER_STRINGBTEND    , 
  BEO_LEXER_STRINGEND      ,
  BEO_LEXER_LINEESC        ,
  BEO_LEXER_ANYSTATE       ,
  
};

enum BeoTokenKind_ {
  BEO_TOKEN_NONE        = 0,
  BEO_TOKEN_COMMENT,
  BEO_TOKEN_BLOCKOPEN,
  BEO_TOKEN_BLOCKCLOSE,
  BEO_TOKEN_LISTOPEN,
  BEO_TOKEN_LISTCLOSE,
  BEO_TOKEN_WHITESPACE,
  BEO_TOKEN_EOL,
  BEO_TOKEN_NUMBER,
  BEO_TOKEN_STRING,
  BEO_TOKEN_WORD,
  BEO_TOKEN_ERROR,
}; 



// macro to help define rules
#define RULE(STATE, MATCH, ACTION, AFTER, TOKEN)  \
        { BEO_LEXER_##START, MATCH, BEO_LEXER_##ACTION, BEO_LEXER_##AFTER, BEO_TOKEN_##TOKEN }

/* Rules for the beo lexer. The beo lexer uses a state machine.
The rules work as follows: the first field is the state to which they apply.
A rule that uses ANYSTATE will apply to any lexer statem other rules
only apply to if the active lexer state is the given state. The state of the 
lexer begins out as START. 
The second field is a string that will be matched with cstr_simplematch 
to the character that the lexer is considering. If it matches, then the rule is 
applied. 
The third field is the action the lexer has to undertake. 
SKIP means the lexer should simply skip the incoming character. 
STORE means the lexer should store the incoming character. 
OK means the lexer should notify it's caller that the current token is finished. 
and OK. However, this action will NOT store the character under consideration. 
This character MUST then be re-submitted to the lexer for further consideration. 
This is done to simplify the character lexer itself, becayse it does not need 
to keep a lookahead character. This does mean that for many tokens, 
an additional state wull be needed to save the last character in the state.
The fourth field is the lexer state to switch to. This is ALWAYS done.
The fifth field is used in conjunction with an OK action to determine the 
type of the token returned.
The rules are applied in order, which means that topmost rules are applied 
before bottom most. If a topmost rule matches, then any rules below it
are ignored. This allows to simplify many of the rules.
*/
BeoLexerRule BEO_LEXER_RULES[] = {
  // if there is a line comment start character, it should be a comment
  RULE(START        , "@#"                    , SKIP , COMMENTSTART , NONE), 
  // #{ starts a block comment it should be ended with } 
  RULE(COMMENTSTART , "@{"                    , SKIP , COMMENTBLOCK , NONE),
  // otherwise it's a line comment
  RULE(COMMENTSTART , "."                     , SKIP , COMMENTLINE  , NONE),
  // end of comment line is a newline or carriage return. 
  RULE(COMMENTLINE  , "@\r\n"                 , STORE, COMMENTEND   , COMMENT),   
  // anything in coment lines is just stored.
  RULE(COMMENTLINE  , "."                     , STORE, COMMENTLINE  , COMMENT),   
  // end of comment block is }
  RULE(COMMENTBLOCK , "@}"                    , SKIP , COMMENTEND   , COMMENT),   
  // anything in coment blocks except } is just stored.
  RULE(COMMENTBLOCK , "^}"                    , STORE, COMMENTBLOCK , COMMENT),   
  // end of comment, any character is fine (and will be retried)
  RULE(COMMENTEND   , "."                     , OK   , START        , COMMENT), 
  
  // skip whitespaces ( blanks
  RULE(START        , "$b"                    , SKIP , START        , NONE),
  
  // if there is a { it starts a block
  RULE(START       , "@{"                     , STORE , BLOCKOPEN  , NONE),
  // if there is a ( it starts a paren list
  RULE(START       , "@("                     , STORE , LISTOPEN   , NONE),
  // if there is a [ it starts a block list
  RULE(START       , "@["                     , STORE , LISTOPEN   , NONE),
  // if there is a } it closes a block
  RULE(START       , "@}"                     , STORE , BLOCKCLOSE , NONE),
  // if there is a ) it closes a pren list
  RULE(START        , "@)"                    , STORE , LISTCLOSE  , NONE),
  // if there is a ] it closes a block list  
  RULE(START        , "@]"                    , STORE , LISTCLOSE  , NONE),
  // in all cases of {}[](), any character after it ends it
  RULE(BLOCKOPEN   , "."                      , OK    , START      , BLOCKOPEN),
  RULE(BLOCKCLOSE  , "."                      , OK    , START      , BLOCKCLOSE),
  RULE(LISTOPEN    , "."                      , OK    , START      , LISTOPEN),
  RULE(LISTCLOSE   , "."                      , OK    , START      , LISTCLOSE),
  // if there is a newline or cr ; it's the end of a statement 
  RULE(START       , "@\r"                    , SKIP  , EOL        , EOL    ),
  RULE(START       , "@\n"                    , SKIP  , EOL        , EOL    ),
  RULE(START       , "@;"                     , SKIP  , EOL        , EOL    ),
  // any character ends an EOL
  RULE(EOL         , "@."                     , OK    , START      , EOL    ),
  
  // if there is a digit it should be a number
  RULE(START       , "$d"                     , STORE , NUMBER     , NONE),
  
  // if there is a quote it should be a string
  RULE(START       , "@\""                    , SKIP , STRINGDQ    , NONE),
  // if there is a quote it should be a string
  RULE(START       , "@'"                     , SKIP , STRINGSQ    , NONE),
  // if there is a backtick it should be a string
  RULE(START       , "@`"                     , SKIP , STRINGBT    , NONE),
  // if there is a - it chould be a negative number or a word
  RULE(START       , "@-"                     , STORE, NUMBER_OR_WORD, NONE),
  
  // If there is a digit, the number or word is a number
  RULE(NUMBER_OR_WORD, "$d"                   , STORE, NUMBER      ,  NONE),
  // If it's a space or a newline, end the single - word
  RULE(NUMBER_OR_WORD, "@ \t\r\n"             , OK   , WORD        , WORD),
  // Otherwise, it's a word.
  RULE(NUMBER_OR_WORD, "@."                   , STORE, WORD        , WORD),
    
  // If there is, in a number, a 0 to 9, or a . or an e or E, or a + or - 
  // or a X, x, b, B, or o, O for hexadecimal, octal and binary numbers, or any of a to f 
  // for hexadecimal numbers/ assume it's all part of the number 
  // (even though it may not be so!)
  RULE(NUMBER        , "@0123456789-+abBcdeEfgoOxX." , STORE, NUMBER, NONE),
  // any other character ends the number. 
  RULE(NUMBER        , "."                           , OK   , START , NUMBER),
  
  // Finally if there is any character except - or 0 to 9, it should be a word
  RULE(START         , "^0123456789-"                , STORE, WORD  , WORD),

  //A word goes on until whitespace or newline is found
  RULE(WORD         , "@\n\r\t "                     , OK   , START  , WORD),
  // if it's not a blank or newline, store it as part of the word
  RULE(WORD         , "^\n\r\t "                     , STORE, WORD   , WORD),
  
  // recognise escapes in double quoted strings but don't process them
  RULE(STRINGDQ     , "@\\"                          , STORE, STRINGDQESC, STRING),
  // numbers keep the escape going to allow \00u, etc.   
  RULE(STRINGDQESC  , "$d"                           , STORE, STRINGDQESC, STRING),
  // anything else 
  RULE(STRINGDQESC  , "!$d"                          , STORE, STRINGDQ, STRING),
  
  // a double quoted string goes on until a double quote is found 
  RULE(STRINGDQ     , "@\""                          , SKIP , STRINGEND, STRING),
  // store any other characters in the string
  RULE(STRINGDQ     , "^\""                          , STORE, STRINGDQ , STRING),
  
  // end of string. accept any character so it can be resubmitted
  RULE( STRINGEND   , "."                            , OK   , START    , STRING),
  
  // A single quoted string goes on until a single quote is found 
  RULE(STRINGSQ     , "@'"                           , SKIP , STRINGEND, STRING),
  // A single quoted string goes on until a single quote is found 
  RULE(STRINGSQ     , "^'"                           , STORE, STRINGSQ , STRING),
  // A backtick quoted string goes on until a double quote is found 
  RULE(STRINGBT     , "@`"                           , SKIP , STRINGEND, STRING),
  // A backtick quoted string goes on until a double quote is found 
  RULE(STRINGBT     , "^`"                           , STORE, STRINGBT , STRING)

};


BeoLexerToken beolexer_lexchar(BeoLexer * self, int ch) {
  int index = 0;
  BeoLexerRule * rule;
  BeoLexerToken token = { BEO_TOKEN_NONE, NULL };
  // look though all rules for rules that match the current state
  // could do a binary search in stead of aliner one, but keep it simple for now.
  rule = self->rules + index;
  for (index = 0 , rule = self->rules + index; 
       rule; 
       index++, rule = self->rules + index) {
       
    // Skip the rule doesn't natch the current state, except if it's ANYSTATE.
    if(
        (rule->before_state != BEO_LEXER_ANYSTATE) &&
        (rule->before_state != self->state)
      ) continue;
    // Skip the rule if the expression does not match. 
    if(!cstr_simplematch(rule->expression, ch)) continue;
    self->state = rule->after_state; 
    switch (rule->action) {    
      case BEO_LEXER_SKIP: // do nothing
        return token; // return token with type BEO_TOKEN_NULL
      case BEO_LEXER_STORE: // store character
        ustr_appendch(self->text, ch);
        return token; // return token with type BEO_TOKEN_NULL
        
      case BEO_LEXER_OK:  
        // done parsing one token, return token and empty buffer, 
        // when you recieve this you mustcall lex_char again with the same token
        // so it gets processed too!
        token.text  = ustr_dup(self->text);
        ustr_truncate(self->text, 0); 
        token.kind  = rule->token_type;
        return token; 
      default: 
        fprintf(stderr, "Unknown action %d in rule!\n", rule->action);
        return token;
    } 
  }
  // If we get here it's an error.
  token.kind = BEO_TOKEN_ERROR;
  // Need better error handling??
  return token;  
}




// lex.rule( :start        , /#/             , :skip , :commentstart) 


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



Forward polish notation: the arity is usually known, if
variable, greedily take all arguments available 
list a b c d e -> [a b c d e]
list a b list c d e -> [ a b [c d e ]]
list a plus 1 1 list c d e -> [ a 2 [c d e ]]
list a plus 1 list c d e 2 -> error, because plus will receive 1 and [c d e 2] 
as it's arguments.







*/




