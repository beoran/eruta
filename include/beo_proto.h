/* This file was generated with:
'cfunctions -c -aoff -n -m -w beo_proto src/beo.c' */
#ifndef CFH_BEO_PROTO
#define CFH_BEO_PROTO

/* From 'src/beo.c': */
/* Low level initializer for a beovalue. */

#line 68 "src/beo.c"
BeoValue * beovalue_initall (BeoValue * self , int type , BeoValueUnion data );

#line 76 "src/beo.c"
#line 1 "/usr/local/share/cfunctions/c-extensions.h"
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
BeoValue * beovalue_alloc PROTO ((void));
/** returns nonzero if the value is of the given type, zero if not. 
Also returns 0 if value is NULL. */

#line 83 "src/beo.c"
int beo_type_p (BeoValue * value , int type );
/** Constructor for beovalue. */

#line 90 "src/beo.c"
BeoValue * beovalue_newall (int type , BeoValueUnion data );
/** Frees the beovalue uses the given destructor to clean up the data 
too. Can handle strings on it's own.  */

#line 97 "src/beo.c"
BeoValue * beovalue_free_destroy (BeoValue * self , MemDestructor * destroy );
/** Initializes a beovalue with a long value */

#line 105 "src/beo.c"
BeoValue * beovalue_initlong (BeoValue * value , long l );
/** Creates a new beovalue with an long inside it. 
Must be freed with beovalue_free_destroy */

#line 113 "src/beo.c"
BeoValue * beovalue_newlong (long l );
/** Initializes a beovalue with a double value */

#line 118 "src/beo.c"
BeoValue * beovalue_initdouble (BeoValue * value , double d );
/** Creates a new beovalue with a double inside it. 
Must be freed with beovalue_free_destroy */

#line 126 "src/beo.c"
BeoValue * beovalue_newdouble (double d );
/** Initializes a beovalue with a c string */

#line 132 "src/beo.c"
BeoValue * beovalue_initstr (BeoValue * value , char * s );
/** Creates a new beovalue with an string inside it. 
Must be freed with beovalue_free_destroy */

#line 140 "src/beo.c"
BeoValue * beovalue_newstr (char * s );
/** Initializes a beovalue with a unicode string. 
The unicode string will be duplicated internally except if u is NULL,
so u must still be freed with ustr_free if it was allocated. */

#line 147 "src/beo.c"
BeoValue * beovalue_initustr (BeoValue * value , USTR * u );
/** Creates a new beovalue with an string inside it. 
Must be freed with beovalue_free_destroy */

#line 155 "src/beo.c"
BeoValue * beovalue_newustr (USTR * s );
/** Creates a new fail value with an (optional) error string. */

#line 160 "src/beo.c"
BeoValue * beovalue_initfail (BeoValue * value , char * error );
/** Creates a new beovalue with an FAIL inside it. 
Must be freed with beovalue_free_destroy */

#line 168 "src/beo.c"
BeoValue * beovalue_newfail (char * error );
/* Initializes a new OK value. */

#line 173 "src/beo.c"
BeoValue * beovalue_initok (BeoValue * value );
/** Creates a new beovalue with an OK value inside it. 
Must be freed with beovalue_free_destroy */

#line 181 "src/beo.c"
BeoValue * beovalue_newok PROTO ((void));
/** Creates a new beovalue with a pointer inside it. */

#line 186 "src/beo.c"
BeoValue * beovalue_initptr (BeoValue * value , void * ptr );
/** Creates a new beovalue with a pointer inside it. 
Must be freed with beovalue_free_destroy */

#line 194 "src/beo.c"
BeoValue * beovalue_newptr (void * ptr );
/** Converts the BeoValue to a double. Stores true in ok if ok. 
Stores false in OK and returns 0 in case there was a conversion error. 
OK is ignored if it is NULL. */

#line 202 "src/beo.c"
double beovalue_getdouble (BeoValue * value , int * ok );
/** Converts the BeoValue to a long. Stores true in ok if ok. 
Stores false in OK and returns 0 in case there was a conversion error. 
OK is ignored if it is NULL. Decimal numbers are assumed. */

#line 213 "src/beo.c"
long beovalue_getlong (BeoValue * value , int * ok );
/** Increases the refence count of self and returns self.  */

#line 225 "src/beo.c"
BeoValue * beo_ref (BeoValue * self );
/** Decreses the reference count. Calls MemDestructor if it's not null. 
Finally calls beovalue_free. Retuns NULL if the value was finalized, 
not-null if not. */

#line 234 "src/beo.c"
BeoValue * beo_unref (BeoValue * self , MemDestructor destroy );

#line 255 "src/beo.c"
BeoFrame * beoframe_alloc PROTO ((void));
/* Fress the frame, burt not it's parent. */

#line 260 "src/beo.c"
BeoFrame * beoframe_free (BeoFrame * self );

#line 268 "src/beo.c"
BeoFrame * beoframe_init (BeoFrame * self , BeoFrame * parent );

#line 276 "src/beo.c"
BeoFrame * beoframe_new (BeoFrame * parent );
/* Sets the value value as the result of the current frame.
Increases the reference count of value. */

#line 282 "src/beo.c"
BeoValue * beoframe_result_ (BeoFrame * self , BeoValue * value );
/* Gets the value value of the result of the current frame.
Does not increase the refcount. */

#line 289 "src/beo.c"
BeoValue * beoframe_result (BeoFrame * self , BeoValue * value );
/* Sets a variable n the current frame with the given name. 
Does NOT increase the reference count.
*/

#line 298 "src/beo.c"
BeoValue * beoframe_localvar_ (BeoFrame * self , char * name , BeoValue * value );
/* Gets a variable in the current frame. Null if not found.  */

#line 303 "src/beo.c"
BeoValue * beoframe_localvar (BeoFrame * self , char * name );

#line 339 "src/beo.c"
Beo * beo_register (Beo * self , char * name , BeoFunction * func );
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

#line 559 "src/beo.c"
extern BeoLexerRule BEO_LEXER_RULES[];

#line 562 "src/beo.c"
BeoLexerToken beolexer_lexchar (BeoLexer * self , int ch );

#endif /* CFH_BEO_PROTO */
