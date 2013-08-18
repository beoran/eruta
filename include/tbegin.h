/* 

This file contains helper definitions that allow compile time generic templates
for ANSI C to be implemented and used easily. 

Usage for implementing templates: 

In the header file that contains the template, do as in the following example:

#include <some_system_library.h>
#include <another_system_library.h>
#include <etc.h>

#include <tbegin.h>


#ifdef TEMPLATE_OK

#ifndef TEMPLATE_IMPLEMENT

struct TEMPLATE_STRUCT {
  TEMPLATE_T foo;
};

TEMPLATE_FUNC(foo)(TEMPLATE_NAME * self, TEMPLATE_T elem);

#else

TEMPLATE_FUNC(foo)(TEMPLATE_NAME * self, TEMPLATE_T elem) {
  self->foo = elem;
}

#endif

#endif

#include <tend.h>


Then, to use a template, include in some header file or C file: : 

#define TEMPLATE_T int
#define TEMPLATE_NAME MyFoo
#include tfoo.h

This will generate the function prototypes and structs needed.

Then in some C file, write: 

#define TEMPLATE_T int
#define TEMPLATE_NAME MyMatrix
#define TEMPLATE_IMPLEMENT
#include tmatrix.h

This will generate the implementation of the functions.



Optionally the following template parameters may also be defined.
TEMPLATE_ALLOC(SIZE)        : custom malloc-compatible allocation function
TEMPLATE_REALLOC(PTR, SIZE) : custom realloc-compatible allocation function
TEMPLATE_FREE(PTR)          : custom free-compatible allocation function
TEMPLATE_PREFIX             : prefix for the functions gererated. 
                              Defaults to TEMPLATE_NAME.

                              

The technique used to implement this kind of ANSI C templates is also known as 
"X-Macro", since it relies on the availability certain macros that are yet 
undefined in this file and must be supplied by the programmer.

*/

/* Type of elememts. */
#ifndef TEMPLATE_T
#error Please define TEMPLATE_T as an element type to use in this template.
#undef TEMPLATE_OK
#endif

/* Additional, yet optional element type. */
#ifdef TEMPLATE_NEEDS_U
#ifndef TEMPLATE_U
#error Please define TEMPLATE_U as an element type to use in this template.
#undef TEMPLATE_OK
#endif
#endif

/* Additional, yet optional element type. */
#ifdef TEMPLATE_NEEDS_V
#ifndef TEMPLATE_V
#error Please define TEMPLATE_V as an element type to use in this template.
#undef TEMPLATE_OK
#endif
#endif

/* Additional, yet optional element type. */
#ifdef TEMPLATE_NEEDS_W
#ifndef TEMPLATE_W
#error Please define TEMPLATE_W as an element type to use this template.
#undef TEMPLATE_OK
#endif
#endif


/* Name of generated struct. */
#ifndef TEMPLATE_NAME
#error Please define TEMPLATE_NAME to be the name of the main templated type.
#undef TEMPLATE_OK
#endif

/* Prefix for generated function names. Defaults to TEMPLATE_NAME. */
#ifndef TEMPLATE_PREFIX
#define TEMPLATE_PREFIX TEMPLATE_NAME
#endif

#ifndef TEMPLATE_ALLOC
#define TEMPLATE_ALLOC(SIZE) calloc(SIZE, 1)
#endif 

#ifndef TEMPLATE_FREE
#define TEMPLATE_FREE(MEM) free(MEM)
#endif 

/* needed C library functions */
#include <stdlib.h>


/* Helper macros. */ 
#define TSTR_HELPER(S)      #S
/* Stringify macro */
#define TSTR(S)             TSTR_HELPER(S)
#define TJOIN_HELPER(A, B)  A##B
/* Token pasting (joining) macro. */
#define TJOIN(A,B)          TJOIN_HELPER(A,B)
#define TSTRUCT(NAME)       TJOIN(NAME, _struct)
#define TFUNC(NAME, FUNC)   TJOIN(NAME, FUNC)

/* Specific helper macros. */
#define TEMPLATE_STRUCT      TSTRUCT(TEMPLATE_NAME)
#define TEMPLATE_FUNC(FUNC)  TFUNC(TEMPLATE_PREFIX, FUNC)


#ifndef FALSE
#define FALSE 0
#endif

#ifndef TRUE
#define TRUE (!FALSE)
#endif

/* Don't proceed unless the essential template parameters are set. */
#if defined(TEMPLATE_NAME) && defined(TEMPLATE_T) 
#define TEMPLATE_OK 1
#else
#undef TEMPLATE_OK
#endif

