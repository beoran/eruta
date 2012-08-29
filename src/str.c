
#include "eruta.h"
#include "str.h"


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



