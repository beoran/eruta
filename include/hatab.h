#ifndef hatab_H_INCLUDED
#define hatab_H_INCLUDED

#include "mem.h"

// Hash function for the hash table. 
typedef int (HatabHash)(void * key);
// Comparison function for the hash table 
typedef int (HatabCompare)(void * one, void * two);


struct HatabActs_;
typedef struct HatabActs_ HatabActs;



// function pointers that determine the hash table's functioning, 
// especially how it compares and hashes keys
struct HatabActs_ {
  HatabCompare  * compare;
  HatabHash     * hash;
  MemDestructor * free_value;
  MemDestructor * free_key;
};

typedef struct Hatab_ Hatab;

#include <stdint.h> 
// need stdint for uint32_t

/* This file was generated with:
'cfunctions -c -aoff -n -w hatab_proto src/hatab.c' */
#ifndef CFH_HATAB_PROTO
#define CFH_HATAB_PROTO

/* From 'src/hatab.c': */

uint32_t hatab_jenkins (char *key , size_t len );

uint32_t hatab_hash_cstr (void * key );

uint32_t hatab_hash_uint32 (void * key );

int hatab_cellarfull_p (Hatab * self );

int hatab_pailsfull_p (Hatab * self );

Hatab * hatab_done (Hatab * self );

Hatab * hatab_free (Hatab * self );

Hatab * hatab_clear (Hatab * self );

Hatab * hatab_initroom (Hatab * self , HatabActs * acts , int pails , int cellars );

/* Macro definitions for C extensions for Cfunctions. */

/* 
   Copyright (C) 1998 Ben K. Bullock.

   This header file is free software; Ben K. Bullock gives unlimited
   permission to copy, modify and distribute it. 

   This header file is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
*/


#ifndef C_EXTENSIONS_H
#define C_EXTENSIONS_H

/* Only modern GNU C's have `__attribute__'.  The keyword `inline'
   seems to have been around since the beginning, but it does not work
   with the `-ansi' option, which defines `__STRICT_ANSI__'.  I expect
   that `__attribute__' does not work with `-ansi' either.  Anyway
   both of these failure cases are being lumped together here for the
   sake of brevity. */

#if defined (__GNUC__) && ( __GNUC__ >= 2 ) && ( __GNUC_MINOR__ > 4 ) && \
   ! defined (__STRICT_ANSI__)

/* Macro definitions for Gnu C extensions to C. */

#define X_NO_RETURN __attribute__((noreturn))
#define X_PRINT_FORMAT(a,b) __attribute__((format(printf,a,b)))
#define X_CONST __attribute__((const))
#define X_INLINE

#else /* Not a modern GNU C */

#define X_NO_RETURN 
#define X_PRINT_FORMAT(a,b) 
#define X_CONST

#endif /* GNU C */

/* The following `#define' is not a mistake.  INLINE is defined to
   nothing for both GNU and non-GNU compilers.  When Cfunctions sees
   `INLINE' it copies the whole of the function into the header file,
   prefixed with `extern inline' and surrounded by an `#ifdef
   X_INLINE' wrapper.  In order to inline the function in GNU C, only
   `X_INLINE' needs to be defined. There is also a normal prototype
   for the case that X_INLINE is not defined.  The reason for copying
   the function with a prefix `extern inline' into the header file is
   explained in the GNU C manual and the Cfunctions manual. */

#define INLINE
#define NO_RETURN void
#define NO_SIDE_FX
#define PRINT_FORMAT(a,b)
#define LOCAL

/* Prototype macro for `traditional C' output. */

#ifndef PROTO
#if defined(__STDC__) && __STDC__ == 1
#define PROTO(a) a
#else
#define PROTO(a) ()
#endif /* __STDC__ */
#endif /* PROTO */

#endif /* ndef C_EXTENSIONS_H */
Hatab * hatab_alloc PROTO ((void));

Hatab * hatab_init (Hatab * self , HatabActs * acts );

Hatab * hatab_newroom (HatabActs * acts , int pails , int cellars );

Hatab * hatab_new (HatabActs * acts );

uint32_t hatab_hash (Hatab * self , void * ptr );

int hatab_compare (Hatab * self , void * pa , void * pb );

void * hatab_get (Hatab * self , void * key );

void * hatab_drop (Hatab * self , void * key );

Hatab * hatab_grow (Hatab * self );

void * hatab_put (Hatab * self , void * key , void * value );

#endif /* CFH_HATAB_PROTO */


#endif




