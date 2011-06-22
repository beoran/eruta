#include "si_mem.h"
#include "si_cursor.h"
#include "si_tree.h"
#include "si_block.h"

/* A simple, unbalanced tree. */

/* The design priciple of memory allocation of Si is: 
ever data structure frees the data it has allocated itself when calling 
sixxxx_free(). however, any pointers stored in the structure by the user must
be handled by the user.
*/

struct SiTree_ {
  SiTree  * parent;
  SiBlock * children;
  void    * data;
};

/** Allocates a new sitree. */
SiTree * sitree_alloc() {
  return SI_ALLOCATE(SiTree);
}

/** Creates a new SiTree tree node . */
SiTree * sitree_new(SiTree * parent, void * data) {
  return sitree_init(sitree_alloc(), parent, data);
}

/** Initializes a si tree node. */
SiTree * sitree_init(SiTree * self, SiTree * parent, void * data) {
  if(!self) return NULL;
  self->children = siblock_newempty(sizeof(SiTree *));
  self->parent   = parent;
  self->data     = data;
  return self;
}

/* Gets the parent node of self. */
SiTree * sitree_parent(SiTree * self) {
  return self->parent;
}

/* Gets the data of self. */
SiTree * sitree_data(SiTree * self) {
  return self->data;
}

/* Adds a child node to self. */
SiTree * sitree_add(SiTree * self, SiTree * child) {
  return siblock_add(self->children, child);
}

/* Gets the index-th child  node of self. */
SiTree * sitree_get(SiTree * self, int index) {
  return (SiTree *) siblock_get(self->children, index);
}

/* Gets the amount of children this Tree node has. */
size_t sitree_size(SiTree * self) {
  return siblock_size(self->children);
}

SiCursor * sitree_cursor(SiTree * self) {
  return NULL;
}

SiCursor * sitree_next(SiCursor * self) {
  return NULL;
}












