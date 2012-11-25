/* This file was generated with:
'cfunctions -c -aoff -n -w inli_proto src/inli.c' */
#ifndef CFH_INLI_PROTO
#define CFH_INLI_PROTO

/* From 'src/inli.c': */

#line 29 "src/inli.c"
Inli * inli_initall (Inli * self , Inli * next , Inli * prev );

#line 47 "src/inli.c"
Inli * inli_init (Inli * self );

#line 65 "src/inli.c"
Inli * inli_remove (Inli * self );

#line 86 "src/inli.c"
Inli * inli_add (Inli * self , Inli * other );

#line 104 "src/inli.c"
Inli * inli_next (Inli * self );

#line 120 "src/inli.c"
Inli * inli_prev (Inli * self );

#line 136 "src/inli.c"
Inli * inli_first (Inli * self );

#line 156 "src/inli.c"
Inli * inli_last (Inli * self );

#line 176 "src/inli.c"
Inli * inli_push (Inli * self , Inli * other );

#line 194 "src/inli.c"
Inli * inli_unshift (Inli * self , Inli * other );

#line 212 "src/inli.c"
Inli * inli_shift (Inli * self );

#line 230 "src/inli.c"
Inli * inli_pop (Inli * self );

#line 248 "src/inli.c"
void * inli_data (Inli * self , int offset );

#endif /* CFH_INLI_PROTO */
