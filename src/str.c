
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







