/**
 * Author: David Chisnall
 */
#include "sarray.h"

#ifndef __SELECTOR_H_INCLUDED__
#define __SELECTOR_H_INCLUDED__
/**
 * Define the SEL type for Objective-C to use.
 */
typedef uint32_t SEL;
/**
 * Initialise the selector-handling part of the library.  Must be called before
 * any other function in this module.
 */
void objc_selector_init();
/**
 * Look up a selector value from a type and name.
 */
typedef SEL (*lookup_typed_selector_t)(char * name, char * types);
extern lookup_typed_selector_t lookup_typed_selector;
/**
 * Register a typed selector.  A type of NULL is used to indicate an untyped
 * selector.  The function returns the SEL value for the registered selector.
 * Subsequent calls to this function will return the same value for the
 * lifespan of a program.
 */
typedef SEL (*register_typed_selector_t)(char * name, char * types);
extern register_typed_selector_t register_typed_selector;
/**
 * Look up the name of the specified selector.
 */
char * name_for_selector(SEL uid);
/**
 * Look up the type string associated with a given selector.
 */
char * types_for_selector(SEL uid);

#endif //__SELECTOR_H_INCLUDED__
