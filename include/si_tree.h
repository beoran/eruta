#ifndef SI_TREE_H
#define SI_TREE_H

#include <stdlib.h>
#include "si_cursor.h"
#include "si_walker.h"


struct SiTree_;
typedef struct SiTree_ SiTree;

/* A simple, unbalanced tree. */

/** Allocates a new sitree. */
SiTree * sitree_alloc();

/** Creates a new SiTree tree node . */
SiTree * sitree_new(void * data);

SiTree * sitree_done(SiTree * self);
SiTree * sitree_free(SiTree * self);

/** Allocates a new sitree. */
SiTree * sitree_alloc();

/** Creates a new SiTree tree node with a parent. */
SiTree * sitree_newparent(SiTree * parent, void * data);

/** Initializes a SiTree node. */
SiTree * sitree_init(SiTree * self, void * data);

/** Initializes a SiTree node with a parent. */
SiTree * sitree_initparent(SiTree * self, SiTree * parent, void * data);

/* Gets the parent node of self. */
SiTree * sitree_parent(SiTree * self);

/* Gets the data of self. */
SiTree * sitree_data(SiTree * self);

/* Adds a child node to self. Returns child. */
SiTree * sitree_addchild(SiTree * self, SiTree * child);

/* Adds data as a child node to self. Returns the child node. */
SiTree * sitree_add(SiTree * self, void * data);

/* Gets the index-th child  node of self. */
SiTree * sitree_get(SiTree * self, size_t index);

/* Gets the amount of children this Tree node has. */
size_t sitree_size(SiTree * self);

SiCursor * sitree_cursor(SiTree * self);

SiCursor * sitree_next(SiCursor * self);


#endif
