#ifndef SI_TREE_H
#define SI_TREE_H

#include <stdlib.h>
#include "si_cursor.h"


struct SiTree_;
typedef struct SiTree_ SiTree;

/* A simple, unbalanced tree. */

/** Allocates a new sitree. */
SiTree * sitree_alloc();

/** Creates a new SiTree tree node . */
SiTree * sitree_new(SiTree * parent, void * data);

/** Initializes a si tree node. */
SiTree * sitree_init(SiTree * self, SiTree * parent, void * data);

/* Gets the parent node of self. */
SiTree * sitree_parent(SiTree * self);

/* Gets the data of self. */
SiTree * sitree_data(SiTree * self);

/* Adds a child node to self. */
SiTree * sitree_add(SiTree * self, SiTree * child);

/* Gets the index-th child  node of self. */
SiTree * sitree_get(SiTree * self, int index);

/* Gets the amount of children this Tree node has. */
size_t sitree_size(SiTree * self);

SiCursor * sitree_cursor(SiTree * self);

SiCursor * sitree_next(SiCursor * self);


#endif
