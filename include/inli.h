#ifndef inli_H_INCLUDED
#define inli_H_INCLUDED

/**
* Inli is an intrusive doubly linked list. 
* You will notice it has no reference to it's element,
* because it should be used like this: 
* struct Data_ { ... } 
* struct DataNode_ ( struct Data_ self; Inli list; } 
* INLI_DATA(&list, DataNode, list)  
*/

typedef struct Inli_ Inli;

struct Inli_ {
  Inli * next;
  Inli * prev;
};

/** Returns the list node for this list element is part of, 
for the give list element, and data type*/
#define INLI_DATA(LIST, TYPE, MEMBER)  inli_data(LIST, offsetof(TYPE, MEMBER))  

/** Shorthand for INLI_DATA(LIST, DATA, list) */
#define INLI_LISTDATA(LIST, TYPE) INLI_DATA(LIST, TYPE, list)



/* This file was generated with:
'cfunctions -c -aoff -n -w inli_proto src/inli.c' */
#ifndef CFH_INLI_PROTO
#define CFH_INLI_PROTO

/* From 'src/inli.c': */

Inli * inli_initall (Inli * self , Inli * next , Inli * prev );

Inli * inli_init (Inli * self );

Inli * inli_remove (Inli * self );

Inli * inli_add (Inli * self , Inli * other );

Inli * inli_next (Inli * self );

Inli * inli_prev (Inli * self );

Inli * inli_first (Inli * self );

Inli * inli_last (Inli * self );

Inli * inli_push (Inli * self , Inli * other );

Inli * inli_unshift (Inli * self , Inli * other );

Inli * inli_shift (Inli * self );

Inli * inli_pop (Inli * self );

void * inli_data (Inli * self , int offset );

#endif /* CFH_INLI_PROTO */




#endif




