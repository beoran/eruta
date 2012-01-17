/*
* This header file has several useful macros.
*/

#ifndef SI_MACRO_H
#define SI_MACRO_H

#define SI_PP_PASTE_2(A, B) A##B
#define SI_PP_PASTE(A, B) SI_PP_PASE_2(A, B)

#define SI_API extern
/* Can be used for __declspec junk if your platform needs it */

#if defined(__WIN32__)
  #define SI_OS_WIN32 "WIN32" 
  #define SI_OS SI_OS_WIN32
#else 
/* Assule POSIX os by default for API's. */
  #define SI_OS_POSIX "POSIX"
  #define SI_OS SI_OS_POSIX
#endif




#endif