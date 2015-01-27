
#include "resque.h"

/* Resque is a Random element size queue. It is a quewue where elements of
 * any random size can be stored at one and and retrieved at the other.  */

typedef struct ResqueNode_ ResqueNode;
typedef struct Resque_ Resque;

typedef struct ResqueNode_ {
  struct ResqueNode_ * next;
  struct ResqueNode_ * prev;
  int type;
  size_t size;
  void * data;  
} ResqueNode;


struct Resque_ {
  ResqueNode * head, * tail;
};


ResqueNode *
resquenode_initall(ResqueNode * me, ResqueNode * next, ResqueNode * prev,
                int type, size_t size, void * data) {
  if (me) { 
    me->next = next;
    me->prev = prev;
    me->type = type;
    me->size = size;
    me->data = data;
  }
  return me;
}

void resquenode_done();

Resque * resque_init(Resque * me) {
  if (me) {
    me->head = NULL;
    me->tail = NULL;
  }
  return me;
}

void resque_done(Resque * me) {
  ResqueNode  * node, * prev;
  if (me) {
    node = me->prev;
    while (node) {
      prev = node->prev;
      resquenode_free(node);
      node = prev;
    }
  }
}






