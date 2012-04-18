#ifndef widget_H_INCLUDED
#define widget_H_INCLUDED


/** Parent/child tree */
struct Pachi_ {
  void * parent;
  void * child;
  void * sibling;
};

#define PACHI_DECLARE()     struct Pachi_ pachi_
#define PACHI_CHILD(OBJ)    ((OBJ).pachi_->child)
#define PACHI_PARENT(OBJ)   ((OBJ).pachi_->parent)
#define PACHI_SIBLING(OBJ)  ((OBJ).pachi_->sibling)

#define PACHI_CHILD_(OBJ, SET)    ((OBJ).pachi_->child    = (SET))
#define PACHI_PARENT_(OBJ, SET)   ((OBJ).pachi_->parent   = (SET))
#define PACHI_SIBLING_(OBJ), SET  ((OBJ).pachi_->sibling  = (SET))


#include "widget_proto.h"


#endif




