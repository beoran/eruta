/* File:   au_intern.h
 * Bjorn De Meyer, 2008
 * 
 * Internal header file containing common information that should only be used 
 * from inside the au runtime
 */

#ifndef _AU_INTERN_H
#define	_AU_INTERN_H
#ifndef _INSIDE_AU_ONLY_
#error "Do not include au_intern.h in any applications, it is for the internal use of Au system only "
#endif


#include <stdef.h>

/* Generic type definitions. */

typedef unsigned char       au_u8;
typedef   signed char       au_s8;
typedef unsigned short      au_u16;
typedef   signed short      au_s16;
typedef unsigned long       au_u32;
typedef   signed long       au_s32;
typedef          float      au_float;
typedef          double     au_double;


#ifndef AU_NO_LONG_NONG
typedef unsigned long long  au_uint64;
typedef long long           au_sint64;
#ifndef AU_NO_LONG_NONG







#endif	/* _AU_INTERN_H */

