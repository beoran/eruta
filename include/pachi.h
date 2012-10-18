#ifndef pachi_H_INCLUDED
#define pachi_H_INCLUDED

typedef struct Pachi_ Pachi;

/** Embeddable parent/child tree. Uses offsetof for good measure. :) */
struct Pachi_ {
  Pachi * parent;
  Pachi * child;
  Pachi * next;
};


#define PACHI_DECLARE()     struct Pachi_ pachi
#define PACHI_CHILD(OBJ)    ((OBJ).pachi->child)
#define PACHI_PARENT(OBJ)   ((OBJ).pachi->parent)
#define PACHI_NEXT(OBJ)     ((OBJ).pachi->next)

#define PACHI_CHILD_(OBJ, SET)    ((OBJ).pachi->child    = (SET))
#define PACHI_PARENT_(OBJ, SET)   ((OBJ).pachi->parent   = (SET))
#define PACHI_NEXT_(OBJ, SET)     ((OBJ).pachi->next     = (SET))

#define PACHI_DATA(PACHI, TYPE, NAME) (&(PACHI) - offsetof(TYPE, NAME)); 


#include "pachi_proto.h"


#endif




