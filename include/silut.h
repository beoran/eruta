#ifndef silut_H_INCLUDED
#define silut_H_INCLUDED

/** A Silut record  (String Integer LookUp Table consists of an integer and a constant character string. A Silut with string geing NULL is considered
to be the a guard record.
*/
struct Silut_;
typedef struct Silut_ Silut;

/** A Silut record  (String Integer LookUp Table consists of an integer and a constant character string. A Silut with string set to NULL is considered
to be the a guard record.
*/
struct Silut_ {
  int           integer;
  const char *  string;
};

/** Use this macro as the last guard element) of a Silut
lookup table. */
#define SILUT_DONE { -1, NULL } 

/* This file was generated with:
'cfunctions -c -aoff -n -w silut_proto src/silut.c' */
#ifndef CFH_SILUT_PROTO
#define CFH_SILUT_PROTO

/* From 'src/silut.c': */

int silut_len (Silut lut[] );

Silut * silut_lsearchi (Silut lut[] , int integer );

Silut * silut_lsearchcstr (Silut lut[] , const char * string );

int silut_comparecstr (const void * one , const void * two );

int silut_compareint (const void * one , const void * two );

Silut * silut_bsearchcstr (Silut lut[] , const char * string );

Silut * silut_bsearchint (Silut lut[] , int integer );

#endif /* CFH_SILUT_PROTO */


#endif




