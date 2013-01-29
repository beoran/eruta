#include <ctype.h>
#include "eruta.h"
#include "str.h"
#include "bad.h"
#include "mem.h"

/* str contains utf-8 enables strings. It's currently nothing 
but a thin layer over Allegro's string library since 
that's pretty nice and UTF-8 enabled, unlike bstring (and 
it uses bsytrig internallly, so, no sense in having 2 string libraries :) 
The idea of the thin wrapper is
a) To limit typing, strings are used everywhere so I want short names for the functions
b) To enable me to replace the string functions in case I want to reuse 
them in something that doesn't use Allegro. 
*/

/** Converts an US string to a number as per atoi. */
int ustr_atoi(USTR * str) {
  return atoi(ustr_c(str));
}

/* Appends N characters from the C string str. 
The C string is first converted to a USTR, and the ntne characters are counted. 
XXX: todo how to do this efficiently without losing any unicode data? 
*/


/** Converts a ustr to s double by using strtod. Return 0 and sets OK to
NULL if conversion failed for some reason. If OK is NULL, it is ignored.  */
double ustr_tod(USTR * ustr, int * ok) {
  char * aid = NULL;
  const char * cstr;
  double res;
  if(!ustr) goto error; 
  errno = 0;
  cstr  = ustr_c(ustr);
  res   = strtod(cstr, &aid);
  if (aid   == cstr) goto error;
  if (errno == ERANGE) goto error;
  
  // all was fine here, return ok 
  if(ok) *ok = TRUE;
  return res;
  error: // if ze get here there was a conversion error
  if(ok) *ok = FALSE; 
  return 0;
}

/** Converts a ustr to s double by using strtol. Return 0 and sets OK to
NULL if conversion failed for some reason. If OK is NULL, it is ignored.  */
long ustr_tol(USTR * ustr, int * ok, int base) {
  char * aid = NULL;
  const char * cstr;
  long res;
  if(!ustr) goto error; 
  errno = 0;
  cstr  = ustr_c(ustr);
  res   = strtol(cstr, &aid, base);
  if (aid   == cstr) goto error;
  if (errno == ERANGE) goto error;
  
  // all was fine here, return ok 
  if(ok) *ok = TRUE;
  return res;
  error: // if ze get here there was a conversion error
  if(ok) *ok = FALSE; 
  return 0;
}

/** Converts an ustr to a double without doing any error checking. */
double ustr_atod(USTR * ustr) {
  return ustr_tod(ustr, NULL);
}

/** Converts an ustr to a long without doing any error checking. 
    Uses the decimal base. 
*/
double ustr_atol(USTR * ustr) {
  return ustr_tol(ustr, NULL, 10);
}

/** Makes a new ustr from the double. Must be freed with ustr_free */
USTR * ustr_newdouble(double d) {
  return ustr_newf("%lf", d);
}

/** Makes a new ustr from a long. Must be freed with ustr_free */
USTR * ustr_newlong(long l) {
  return ustr_newf("%ld", l);
}


/** Helper for calling any of the isXXX functions 
based on the first character expression string
a call isalnum on ch and return the result.
A call isalpha on ch and return the result.
b call isblank on ch and return the result.
c call iscntrl on ch and return the result.
d call isdigit on ch and return the result.
g call isgraph on ch and return the result.
l call islower on ch and return the result.
p call isprint on ch and return the result.
P call ispunct on ch and return the result.
u call isspace on ch and return the result.
x call isxdigit on ch and return the result.
any other first character: return FALSE.
*/
int cstr_charis(const char * expression, int ch) {
  char first;
  if (!expression) return FALSE;
  first = expression[0];
  switch(first) {
    case 'a': return isalnum(ch);
    case 'A': return isalpha(ch);
    case 'b': return isblank(ch);
    case 'c': return iscntrl(ch);
    case 'd': return isdigit(ch);
    case 'g': return isgraph(ch);
    case 'l': return islower(ch);
    case 'p': return isprint(ch);
    case 'P': return ispunct(ch);
    case 's': return isspace(ch);
    case 'u': return isupper(ch);
    case 'x': return isxdigit(ch);
    default: return FALSE;
  }
  // cannot reach this place
}



/** Ultra-simple matching of a single character ch
versus a string expression of allowed characters or instructions. 
The expression is interpreted as follows: 
$a call isalnum on ch and return the result.
$A call isalpha on ch and return the result.
$b call isblank on ch and return the result.
$c call iscntrl on ch and return the result.
$d call isdigit on ch and return the result.
$g call isgraph on ch and return the result.
$l call islower on ch and return the result.
$p call isprint on ch and return the result.
$P call ispunct on ch and return the result.
$s call isspace on ch and return the result.
$u call isupper on ch and return the result.
$x call isxdigit on ch and return the result.
@ must be followed by a list of characters, strchr is called on that list.
^ must be followed by a list of characters, strchr is called on that list,
  and it's logical opposite is returned.
. matches any character and always returns true.
empty string or NULL: matches nothing and always returns false.
A ! prefix to any of these means to apply the 
C ! operator to the rest of the expression's result.
If expression starts with any other character, FALSE is returned as well.
*/
int cstr_simplematch(const char * expression, int ch) {
  char first;
  if (!expression) return FALSE;
  first = expression[0];
  switch(first) {
    case '\0': return FALSE;
    case '!' : return !cstr_simplematch(expression+1, ch);
    case '@' : return (strchr(expression+1, ch) != NULL);
    case '^' : return (strchr(expression+1, ch) == NULL);
    case '.' : return TRUE;
    case '$' : return cstr_charis(expression+1, ch);
    default: return FALSE;
  }
  // can't reach here    
}

USTR * ustrlistnode_ustr(USTRListNode * self) {
  if(!self) return NULL;
  return self->ustr;
}


USTRListNode * ustrlistnode_alloc(void) {
  return STRUCT_ALLOC(USTRListNode);
}

USTRListNode * ustrlistnode_init(USTRListNode * self, USTR * ustr) {
  if(!self) return NULL;
  self->ustr = al_ustr_dup(ustr);
  badlistnode_initempty(&self->list);
  return self;
}

USTRListNode * ustrlistnode_new(USTR * ustr) {
  return ustrlistnode_init(ustrlistnode_alloc(), ustr);
}

USTRListNode * ustrlistnode_done(USTRListNode * self) {
  if(!self) return NULL;
  al_ustr_free(self->ustr);
  badlistnode_initempty(&self->list);
  return self;
}

USTRListNode * badlistnode_ustrlistnode(BadListNode * elem) {
  if(!elem) return NULL;
  return bad_container(elem, USTRListNode, list);
}

USTRListNode * ustrlistnode_free(USTRListNode * self) {
  if(!self) return NULL;
  return mem_free(ustrlistnode_free(self));
}


USTRList * ustrlist_alloc(void) {
  USTRList * self;
  self = STRUCT_ALLOC(USTRList);
  return self;
}

USTRList * ustrlist_init(USTRList * self) {
  if (!self) return self;
  badlist_init(self);
  return self;
}

USTRList * ustrlist_new() {
  return ustrlist_init(ustrlist_alloc());
}

USTRList * ustrlist_done(USTRList * self) {
  BadListNode      * elem;
  USTRListNode * node;
  if(!self) return NULL;
  elem = badlist_head(self); 
  while(elem) {
    node = badlistnode_ustrlistnode(elem);
    elem = badlistnode_next(elem); 
    // here, elem is already pointing to next so node is safe to free
    ustrlistnode_free(node);
  }
  ustrlist_init(self);
  return self;
}


USTRList * ustrlist_free(USTRList * self) {
  if(!ustrlist_done(self)) return NULL;
  mem_free(self);
  return NULL;
}

USTRList * ustrlist_addnode(USTRList * self, USTRListNode * node) {
  if(!self) return NULL;
  badlist_add(self, &node->list);
  return self;
}

USTRList * ustrlist_removenode(USTRList * self, USTRListNode * node) {
  if(!self) return NULL;
  badlist_remove(self, &node->list);
  return self;
}


USTRListNode * ustrlist_addustr(USTRList * self, USTR * ustr) {
  USTRListNode * node = ustrlistnode_new(ustr);
  if(!node) return NULL;
  ustrlist_addnode(self, node);
  return node;
}

int ustrlist_size(USTRList * self) {
  return badlist_size(self);
}











