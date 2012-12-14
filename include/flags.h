#ifndef flags_H_INCLUDED
#define flags_H_INCLUDED

/* This file was generated with:
'cfunctions -c -aoff -n -w flags_proto src/flags.c' */
#ifndef CFH_FLAGS_PROTO
#define CFH_FLAGS_PROTO

/* From 'src/flags.c': */

int flags_set (int * self , int flag );

int flags_setvalue (int self , int flag );

int flags_unset (int * self , int flag );

int flags_unsetvalue (int self , int flag );

int flags_put (int * self , int flag , int set );

int flags_putvalue (int self , int flag , int set );

int flags_get (int self , int flag );

int flags_getptr (int * self , int flag );

#endif /* CFH_FLAGS_PROTO */


#endif




