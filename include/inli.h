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



#include "inli_proto.h"




#endif




