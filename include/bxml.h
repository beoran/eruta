#ifndef bxml_H_INCLUDED
#define bxml_H_INCLUDED


#include "str.h"
#include "ifa.h"

/* This file was generated with:
'cfunctions -c -aoff -n -w bxml_proto src/bxml.c' */
#ifndef CFH_BXML_PROTO
#define CFH_BXML_PROTO

/* From 'src/bxml.c': */


#ifdef BXML_COMMENT
BXML * bxml_init (BXML * self , int kind );

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
BXML * bxml_alloc PROTO ((void));

BXML * bxml_new (int kind );

BXML * bxml_done (BXML * self );

BXML * bxml_free (BXML * self );

BXML * bxml_addsibling (BXML * self , BXML * other );

BXML * bxml_addattribute (BXML * self , BXML * attr );

BXML * xml_addchild (BWML * self , BXML * child );

BXMLState bxmlparser_push (BXMLParser * self , BXMLState state );

BXMLState bxmlparser_peek (BXMLParser * self );

BXMLState bxmlparser_pop (BXMLParser * self );

BXMLState bxmlparser_put (BXMLParser * self , BXMLState state );

BXMLParser * bxmlparser_init (BXMLParser * self );

BXMLState bxmlparser_parse_start (BXMLParser * self , int ch );

BXMLState bxmlparser_parse_tagstart (BXMLParser * self , int ch );

BXMLState bxmlparser_parse_tagname (BXMLParser * self , int ch );

BXMLState bxmlparser_parse_attrlist (BXMLParser * self , int ch );

BXMLState bxmlparser_parse_attrname (BXMLParser * self , int ch );

else BXMLState bxmlparser_parse_nameend (BXMLParser * self , int ch );

else BXMLState bxmlparser_parse_valstart (BXMLParser * self );

BXMLState bxmlparser_parse_value (BXMLParser * self , int ch , int quote );

BXMLState bxmlparser_parse_entity (BXMLParser * self , int ch , USTR * target );

BXMLState bxmlparser_parse_text (BXMLParser * self , int ch , BXML ** result );

BXMLResult bxmlparser_parse_entity (BXMLParser * self , int ch , BXML ** result );

BXMLResult bxmlparser_parse_comment (BXMLParser * self , int ch , BXML ** result );

BXMLResult bxmlparser_parse_declare (BXMLParser * self , int ch , BXML ** result );

BXMLResult bxmlparser_parse_dispatch (BXMLParser * self , int ch );

BXMLResult bxmlparser_parse_core (BXMLParser * self , int ch , BXML ** result );

BXML * bxml_parse_strn (char * str , size_t size );

BXML * bxml_parse_str (char * str );

BXML * bxml_readfile (char * filename );

#endif

#endif /* CFH_BXML_PROTO */


#endif




