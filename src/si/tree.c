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
  SiBlock  * children;
  void    * data;
};

/** Cleans up a sitree, removing and freeing it's children. */
SiTree * sitree_done(SiTree * self) {
  size_t index, size;
  if(!self) return NULL;
  if(self->children) { 
    size = sitree_size(self);
    for(index = 0; index < size ; index++) {
      SiTree * child  = (SiTree *) sitree_get(self, index);
      sitree_free(child);
    }
    siblock_free(self->children);
    self->children = NULL;
  }
  return self;
}

SiTree * sitree_free(SiTree * self) {
  sitree_done(self);
  return si_free(self);
}

/** Allocates a new sitree. */
SiTree * sitree_alloc() {
  return SI_ALLOCATE(SiTree);
}

/** Creates a new SiTree tree node with a parent. */
SiTree * sitree_newparent(SiTree * parent, void * data) {
  return sitree_initparent(sitree_alloc(), parent, data);
}

/** Creates a new SiTree tree node. */
SiTree * sitree_new(void * data) {
  return sitree_newparent(NULL, data);
}

/** Initializes a SiTree node with a parent. */
SiTree * sitree_initparent(SiTree * self, SiTree * parent, void * data) {
  if(!self) return NULL;  
  self->parent   = parent;
  self->data     = data;
  self->children = NULL;
  return self;
}

/** Initializes a new SiTree tree node. */
SiTree * sitree_init(SiTree * self, void * data) {
  return sitree_initparent(self, NULL, data);
}

/* Gets the parent node of self. */
SiTree * sitree_parent(SiTree * self) {
  return self->parent;
}

/* Sets the parent node of self. */
SiTree * sitree_parent_(SiTree * self, SiTree * parent) {
  return self->parent = parent;
}


/* Gets the data of self. */
SiTree * sitree_data(SiTree * self) {
  return self->data;
}

/* Adds a child node to self. Returns child. */
SiTree * sitree_addchild(SiTree * self, SiTree * child) {
  if(!self->children) {
    self->children = siblock_newempty(sizeof(SiTree *));  
  }
  siblock_addptr(self->children, child);
  sitree_parent_(child, self);
  return child; 
}

/* Adds data as a child node to self. Returns the child node. */
SiTree * sitree_add(SiTree * self, void * data) {
  SiTree * child = sitree_newparent(self, data);
  return sitree_addchild(self, child);
}

/* Gets the index-th child  node of self. */
SiTree * sitree_get(SiTree * self, size_t index) {
  return (SiTree *) siblock_getptr(self->children, index);
}

/* Gets the amount of children this Tree node has. */
size_t sitree_size(SiTree * self) {
  return siblock_size(self->children);
}


/** Walks the tree depth-first, calling func for self and each sub-tree. */
void * sitree_walk(SiTree * self, SiWalker walker, void * data) {
  size_t index, size;
  void * res = walker(self, self, self, data);
  if (res) return res; 
  if (!self->children) return NULL;
  size = sitree_size(self);
  for(index = 0; index < size; index++) {
    SiTree * child = sitree_get(self, index);
    // Recursively walk the child (this calls walker for it as seen above)
    res = sitree_walk(child, walker, data);
    if (res) return res;
  }  
  return NULL;
}



SiCursor * sitreecursor_next(SiCursor * self) {
  /*
  SiTree * active  = SICURSOR_ACTIVE(self, SiTree *);
  if(active->children) {
    // get the first element 
    SiTree * next = (SiTree *)silist_data(active->children); 
  }  
  int index        = sicursor_index(self);
  if(siblock_empty_p(active->children)) {
    
  }

  if (siblock_intindex_ok(active->children, index)) {
    SiTree * child = siblock_get(active->children, (size_t)index);
    sicursor_index_(self, 0);
    sicursor_active_(self, child); 
    // depth-first traversal 
  }
  if(active->next) {
    sicursor_active_(self, active->next);
    return self;
  }
  return NULL;
*/ 
  return NULL;
}

void * sitreecursor_data(SiCursor * self) {
  return NULL;
}

static SiCursorClass sitree_cursor_class = {
  sitreecursor_next,
  sitreecursor_data
};

SiCursor * sitree_cursor(SiTree * self) {
  return sicursor_new( self, self, 0, &sitree_cursor_class);
}













