#include <string.h>
#include "bad.h"

/*
* License: bad.h and bad.c are 
*
* Copyright (C) 2012-2013 beoran (beoran@beoran.net)
*
* Permission is hereby granted, free of charge, to any person obtaining a copy 
* of this software and associated documentation files (the "Software"), to deal
* in the Software without restriction, including without limitation the rights 
* to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
* copies of the Software, and to permit persons to whom the Software is 
* furnished to do so, subject to the following conditions: 
*
* The above copyright notice and this permission notice shall be included in 
* all copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
* SOFTWARE.
*
*/

int bad_mini(int one, int two) {
  return (one < two) ? one : two;  
}

double bad_mind(double one, double two) {
  return (one < two) ? one : two;  
}

float bad_minf(float one, float two) {
  return (one < two) ? one : two;  
}


int bad_maxi(int one, int two) {
  return (one > two) ? one : two;  
}

double bad_maxd(double one, double two) {
  return (one > two) ? one : two;
}

float bad_maxf(float one, float two) {
  return (one > two) ? one : two;
}



int bad_comparei(int one, int two) {
  if (one < two) return -1;
  if (one > two) return  1;
  return 0;
}

int bad_compared(double one, double two) {
  if (one < two) return -1;
  if (one > two) return  1;
  return 0;
}

int bad_comparef(float one, float two) {
  if (one < two) return -1;
  if (one > two) return  1;
  return 0;
}


int bad_clampi(int value , int min, int max) {
  if (value < min) return min;
  if (value > max) return max;
  return value;
}


double bad_clampd(double value , double min, double max) {
  if (value < min) return min;
  if (value > max) return max;
  return value;
}


float bad_clampf(float value , float min, float max) {
  if (value < min) return min;
  if (value > max) return max;
  return value;
}

int bad_outofboundsi(int value, int min, int max) {
  if (value < min)  return TRUE;
  return (value >= max); 
}


int bad_outofboundsd(int value, double min, double max) {
  if (value < min)  return TRUE;
  return (value >= max); 
}

int bad_outofboundsf(int value, float min, float max) {
  if (value < min)  return TRUE;
  return (value >= max); 
}



/**
* Struct: BadListNode
*
* BadListNode is an intrusive doubly linked list.
* These lists do not contain any pointers to the data member that is part of 
* the list, rather, you use them by including them into the struct that 
* needs to be included in the intrusive linked list, hence the "intrusive".
* Use the macro bad_container to get the containing struct back.
*
* For example : struct Foo { int value ; BadListNode list } ...
* struct Foo foo, bar; badlistnode_init(&foo.list); badlistnode_init(&bar.list);
* badlistnode_add(&foo.list, &bar.list);
* 
*/

/**
* Function: badlistnode_initall
*
* Fully initializes a non-NULL intrusive linked list.
*
* Parameters:
*   self - BadListNode to initialize
*   next - Next BadListNode list node to link to or NULL
*   prev - Previous BadListNode list node to link to or NULL
*
* Returns: 
*   self
*/
BadListNode * badlistnode_initall(BadListNode * self, BadListNode * next, BadListNode * prev) {
  if (!self) return NULL;
  self->next = next;
  self->prev = prev;
  return self;
}

/**
* Function: badlistnode_init
*
* Initializes the intrusive linked list. Next and prev are set to NULL.
*
* Parameters:
*   self - BadListNode to initialize
*
* Returns: 
*   self
*/
BadListNode * badlistnode_init(BadListNode * self) {
  return badlistnode_initall(self, NULL, NULL);
}


/**
* Function: badlistnode_initempty
*
* Initializes the intrusive linked list to be empty. Next is set to self
* to signal this to badlistnode_isempty.
*
* Parameters:
*   self - BadListNode to initialize as empty
*
* Returns: 
*   self
*/
BadListNode * badlistnode_initempty(BadListNode * self) {
  return badlistnode_initall(self, self, NULL);
}

/**
* Function: badlistnode_isempty
*
* Returns true if the list is empty, false if not.
* 
* Parameters:
*   self - BadListNode to check.
*
* Returns: 
*   TRUE if empty, FALSE if not. A NULL list is also empty.
*/
bool badlistnode_isempty(BadListNode * self) {
  if(!self) return TRUE;  
  return badlistnode_next(self) == self;
}


/** 
* Function: badlistnode_unlink
*
* Unlinks self from the list it is part of. 
* Does NOT clean up any data asssociated with the container of the intrusive 
* list and also doesn't free self, since self should be part of the container 
* of the intrusive list. 
*
* Parameters:
*   self - BadListNode to remove from whole of list. May be NULL.
*
* Returns: 
*   self
*/
BadListNode * badlistnode_unlink(BadListNode * self) {  
  if(!self) return NULL;
  if(self->prev) { self->prev->next = self->next; }
  if(self->next) { self->next->prev = self->prev; }
  self->prev = NULL;
  self->next = NULL;
  return self;
}

/** 
* Function: badlistnode_add
*
* Appends other after self. 
*
* Parameters:
*   self - BadListNode to append to. IF NULL, returns other, since that becomes 
*          the base of the list.
*   other - BadListNode to append to self.
*
* Returns:
*   The new "first" element of the list.
*/
BadListNode * badlistnode_add(BadListNode * self, BadListNode * other) {  
  if(!self)  return other;
  if(!other) return self;
  self->next  = other;
  other->prev = self;
  return other;
}

/** 
* Function: badlistnode_next
*
* Returns the next element in the list
*
* Parameters:
*   self - BadListNode
*
* Returns: 
*   the next element in the list, or NULL if no next item. 
*/
BadListNode * badlistnode_next(BadListNode * self) {
  if(!self) return NULL;
  return self->next;
}

/** 
* Function: badlistnode_prev
*
* Returns the previous element in the list
*
* Parameters:
*   self - BadListNode
*
* Returns: 
*   the next element in the list, or NULL if no next item. 
*/
BadListNode * badlistnode_prev(BadListNode * self) {
  if(!self) return NULL;
  return self->prev;
}

/** 
* Function: badlistnode_first
*
* Returns the first element in the list, by dumb iteration.
*
* Parameters:
*   self - BadListNode
*
* Returns: 
*   the first link in the list, or NULL if self is NULL. 
*/
BadListNode * badlistnode_first(BadListNode * self) {
  BadListNode * aid = self; 
  if(!aid) return NULL;
  while (aid->prev) {
    aid = aid->prev;
  }
  return aid;  
}

/** 
* Function: badlistnode_last
*
* Returns the last element in the list, by dumb iteration.
*
* Parameters:
*   self - BadListNode
*
* Returns: 
*   the last link in the list, or NULL if self is NULL. 
*/
BadListNode * badlistnode_last(BadListNode * self) {
  BadListNode * aid = self; 
  if(!aid) return NULL;
  while (aid->next) {
    aid = aid->next;
  }
  return aid;  
}

/** 
* Function: badlistnode_push
*
* Appends other to the end of the list by dumb iteration.
*
* Parameters:
*   self - BadListNode
*
* Returns: 
*   other, or NULL if self or other is NULL. 
*/
BadListNode * badlistnode_push(BadListNode * self, BadListNode * other) {  
  BadListNode * aid;
  aid = badlistnode_last(self);
  return badlistnode_add(aid, other);
}


/** 
* Function: badlistnode_unshift 
*                
* Prepends other to the beginning of the list by dumb iteration.
*
* Parameters:
*   self - BadListNode
*
* Returns: 
*   other, or NULL if self or other is NULL. Other is now also the new 
*   beginning of the list.
*/
BadListNode * badlistnode_unshift(BadListNode * self, BadListNode * other) {  
  BadListNode * aid;
  aid = badlistnode_first(self);
  badlistnode_add(other, aid);
  return other;
}


/** 
* Function: badlistnode_shift
*
* Removes the first element from the list by dumb iteration.
*
* Parameters:
*   self - BadListNode
*
* Returns: 
*   list node that was removed, or NULL if self is NULL. 
*/
BadListNode * 
badlistnode_shift(BadListNode * self) {  
  BadListNode * aid;
  aid = badlistnode_first(self);
  return badlistnode_unlink(aid);
}


/** 
* Function: badlistnode_pop
*
* Removes the last element from the list by dumb iteration.
*
* Parameters:
*   self - BadListNode
*
* Returns: 
*   list node that was removed, or NULL if self is NULL. 
*/
BadListNode * 
badlistnode_pop(BadListNode * self) {  
  BadListNode * aid;
  aid = badlistnode_last(self);
  return badlistnode_unlink(aid);
}


/** 
* Function: badlistnode_data
* 
* Parameters:
* self - list node to get the data of.
* offset - Use offsetof to calculate this offset.
*
* Returns:  
*   the data for this node of the singly linked list. 
* 
*/
void * 
badlistnode_data(BadListNode * self, int offset) {
  if(!self) return NULL;
  return (void *)((char *)self - offset);
}



/** 
* Function: badlistnode_search
* 
* Parameters:
* self   - list node to search.
* offset - Comparer function, get passed each element of the BadListNode * and 
*          tofind, must return 0 if the item searched for is found.
*
* Returns:  
*   the BadListNode found, or NULL if not found.
* 
*/
BadListNode * 
badlistnode_search(BadListNode * self, BadListNodeCompare * compare, BadListNode * tofind) {
  BadListNode * aid;
  if (badlistnode_isempty(self)) return NULL;
  for(aid = self; aid ; aid = aid->next) {
    int cmp = compare(aid, tofind);
    if (cmp == 0) return aid;
  }
  return NULL;
}

/** 
* Function: badlistnode_searchvalue
* 
* Parameters:
* self   - list node to search.
* offset - Comparer function, get passed each element of the BadListNode * and 
*          tofind, must return 0 if the item searched for is found.
* tofind - Arbitrary pointer to a VALUE (not a BadListNode *).
* Returns:  
*   the BadListNode found, or NULL if not found.
* 
*/
BadListNode * 
badlistnode_searchvalue(BadListNode * self, BadListNodeSearchValue * compare, void * tofind) {
  BadListNode * aid;
  if (badlistnode_isempty(self)) return NULL;
  for(aid = self; aid ; aid = aid->next) {
    int cmp = compare(aid, tofind);
    if (cmp == 0) return aid;
  }
  return NULL;
}

/*
* Function: badlistnode_unlink
*
* A shorthand for badlistnode_unlink(badlistnode_search(self, compare, toremove));
*/
BadListNode * 
badlistnode_remove(BadListNode * self, BadListNodeCompare * compare, BadListNode * toremove) {  
  return badlistnode_unlink(badlistnode_search(self, compare, toremove));
}


/*
 * Function: badlistnode_each
 * 
 * Iterates over all list elements starting from self. 
 * It's safe to unlink the element passed to BadListNodeEach. 
 * The iteration stops if BadListNodeEach returns non-zero.
 */
int badlistnode_each(BadListNode * self, BadListNodeEach * each, void * data) {
  BadListNode * aid, * next;
  int res = 0;
  aid = self;
  while(aid) {
    next = badlistnode_next(aid);
    res  = each(aid, data);
    if(res) return res;
    aid  = next;
  }
  return res;
}


BadList * badlist_init(BadList * self) {
  if (!self) return NULL;
  self->head = NULL;
  self->tail = NULL;
  self->size = 0;
  return self;
}

BadList * badlist_add(BadList * self, BadListNode * node) {
  if (!self) return NULL;
  if (!self->tail) { 
    self->head       = node;
    badlistnode_initall(node, NULL, NULL);
  } else {      
    self->tail->next = node;
    node->prev       = self->tail;
    node->next       = NULL;
  }
  self->tail         = node;
  self->size++;
  return self;
}

BadList * badlist_shift(BadList * self, BadListNode * node) {
  if (!self) return NULL;
  if (!self->head) { 
    self->tail       = node;
    badlistnode_initall(node, NULL, NULL);
  } else {      
    self->head->prev = node;
    node->prev       = NULL;
    node->next       = self->head;
  }
  self->head         = node;
  self->size++;
  return self;
}


BadList * badlist_remove(BadList * self, BadListNode * node) {
  if(self->tail == node) {
    self->tail = node->prev;
  } 
  if(self->head == node) {
    self->head = node->next;
  } 
  if (!self->tail) { self->head = NULL; }  
  if (!self->head) { self->tail = NULL; }
  badlistnode_unlink(node);
  self->size--;
  return self;
}


BadListNode * badlist_head(BadList * self) {
  if (!self) return NULL;
  return self->head;
}

BadListNode * badlist_tail(BadList * self) {
  if (!self) return NULL;
  return self->tail;
}

int badlist_size(BadList * self) {
  if (!self) return 0;
  return self->size;
}


BadBitree * 
badbitree_initall(BadBitree * self, BadBitree * left, BadBitree * right, 
                  BadBitree * up) {
  if(!self) return NULL;
  self->left  = left;
  self->right = right;
  self->up    = up;
  return self;
}

BadBitree *
badbitree_init(BadBitree * self) {
  return badbitree_initall(self, NULL, NULL, NULL);
}

BadBitree *
badbitree_left(BadBitree * self) {
  if(!self) return NULL;
  return self->left;
}

BadBitree *
badbitree_right(BadBitree * self) {
  if(!self) return NULL;
  return self->right;
}

BadBitree *
badbitree_up(BadBitree * self) {
  if(!self) return NULL;
  return self->right;
}


BadBitree *
badbitree_up_(BadBitree * self, BadBitree * newup) {
  if(!self) return NULL;
  return self->up = newup;
}

BadBitree *
badbitree_left_(BadBitree * self, BadBitree * newleft) {
  if(!self) return NULL;
  badbitree_up_(newleft, self);
  return self->left = newleft;
}

BadBitree *
badbitree_right_(BadBitree * self, BadBitree * newright) {
  if(!self) return NULL;
  badbitree_up_(newright, self);
  return self->right = newright;
}

bool badbitree_isleaf(BadBitree * self) {
  if(!self) return FALSE;
  if(badbitree_right(self)) return FALSE;
  if(badbitree_left(self)) return FALSE;
  return TRUE;
}


BadAatree * 
badaatree_initall(BadAatree * self, BadAatree * left, BadAatree * right, 
                  BadAatree * up, int level) {
  if(!badbitree_initall(&self->tree, &left->tree, &right->tree, &up->tree)) 
    return NULL;
  self->level = level;
  return self;
}

BadAatree *
badaatree_init(BadAatree * self) {
  return badaatree_initall(self, NULL, NULL, NULL, 1);
}

#define bad_bi2aa(PTR) bad_container(PTR, BadAatree, tree)

BadAatree *
badaatree_left(BadAatree * self) {
  if(!self) return NULL;
  return bad_bi2aa(badbitree_left(&self->tree));
}

BadAatree *
badaatree_right(BadAatree * self) {
  if(!self) return NULL;
  return bad_bi2aa(badbitree_right(&self->tree));
}

BadAatree *
badaatree_up(BadAatree * self) {
  if(!self) return NULL;
  return bad_bi2aa(badbitree_up(&self->tree));
}

BadAatree *
badaatree_left_(BadAatree * self, BadAatree * other) {
  if(!self) return NULL;
  return bad_bi2aa(badbitree_left_(&self->tree, &other->tree));
}

BadAatree *
badaatree_right_(BadAatree * self, BadAatree * other) {
  if(!self) return NULL;
  return bad_bi2aa(badbitree_right_(&self->tree, &other->tree));
}

BadAatree *
badaatree_up_(BadAatree * self, BadAatree * other) {
  if(!self) return NULL;
  return bad_bi2aa(badbitree_up_(&self->tree, &other->tree));
}

int badaatree_level(BadAatree * self) { 
  if(!self) return -1;
  return self->level;
}

int badaatree_level_(BadAatree * self, int newlevel) { 
  if(!self) return -1;
  return self->level = newlevel;
}

bool badaatree_isleaf(BadAatree * self) {
  if(!self) return FALSE;
  return badbitree_isleaf(&self->tree);
}

BadAatree * badaatree_skew(BadAatree * self) { 
  BadAatree * left, * leftleft;
  if(!self) return NULL;
  left        = badaatree_left(self);
  if(!left) return self;
  leftleft    = badaatree_left(left);
  if(!leftleft) return self;
  if(badaatree_level(self) == badaatree_level(leftleft)) {
    BadAatree * left = badaatree_left(self);
    badaatree_left_(self, badaatree_right(left));
    badaatree_right_(left, self);
    return left;
  }
  return self;
}

BadAatree * badaatree_split(BadAatree * self) { 
  BadAatree * right, * rightright;
  if(!self) return NULL;
  right       = badaatree_right(self);
  if(!right) return self;
  rightright  = badaatree_right(right);
  if(!rightright)             return self;
  if (badaatree_level(self) == badaatree_level(rightright)) {
    badaatree_right_(self, badaatree_left(right));
    badaatree_left_(right, self);
    badaatree_level_(right, badaatree_level(right) + 1);
    return right;
  }
  return self;
}

BadAatree * badaatree_insert(BadAatree * self, BadAatree * node, 
                             BadAatreeMethods * methods) { 
  int cmp;
  BadAatree * aid; 
  if(!self) { return node; } 
  cmp = methods->compare(self, node);
  if(cmp < 0) {
    aid = badaatree_insert(badaatree_left(self), node, methods);
    badaatree_left_(self, aid);
  } else if (cmp > 0) {
    aid = badaatree_insert(badaatree_right(self), node, methods);
    badaatree_right_(self, aid);
  } else { /* Ignore duplicates for now,... */
  }
  self = badaatree_skew(self);
  self = badaatree_split(self);
  return self;
}

BadAatree * badaatree_search(BadAatree * self, BadAatree * node,
                             BadAatreeMethods * methods) { 
  int cmp;
  BadAatree * aid; 
  if(!self) { return NULL; } 
  cmp = methods->compare(self, node);
  if(cmp < 0) {
    return badaatree_search(badaatree_left(self), node, methods);
  } else if (cmp > 0) {
    return badaatree_search(badaatree_right(self), node, methods);
  } else { /* Found the item! */
    return self;
  }
}


BadAatree * badaatree_leveldownall(BadAatree * self) {
  BadAatree * right = badaatree_right(self);
  int lowest = bad_mini(badaatree_level(self), badaatree_level(right)) + 1;
  if (lowest < badaatree_level(self)) {
    badaatree_level_(self, lowest);
    if (lowest < badaatree_level(right)) {
      badaatree_level_(right, lowest);
    }
  }
  return self;
}

BadAatree * badaatree_successor(BadAatree * self) {
  BadAatree * aid, *next;
  if(!self) return NULL;
  aid  = badaatree_right(self); 
  if(!aid) return NULL;
  next = badaatree_left(aid);
  while (next) {
    aid  = next;
    next = badaatree_left(aid); 
  };
  return aid;
}

BadAatree * badaatree_predecessor(BadAatree * self) {
  BadAatree * aid, *next;
  if(!self) return NULL;
  aid  = badaatree_left(self); 
  if(!aid) return NULL;
  next = badaatree_right(aid);
  while (next) {
    aid  = next;
    next = badaatree_right(aid); 
  };
  return aid;
}

BadAatree * badaatree_delete(BadAatree * self, BadAatree * node, 
                             BadAatreeMethods * methods) {
  BadAatree * aid;
  int cmp;
  if(!self) { return self; } 
  cmp = methods->compare(self, node);
  if(cmp < 0) {
    aid = badaatree_delete(badaatree_left(self), node, methods);
    badaatree_left_(self, aid);
  } else if (cmp > 0) {
    aid = badaatree_delete(badaatree_right(self), node, methods);
    badaatree_right_(self, aid);
  } else { /* Found the value ! */
    if(badaatree_isleaf(self)) {
      badaatree_init(self); /* call init to unlink the tree's up pointer. */
      return NULL;
    } else if (!badaatree_left(self)) {
      BadAatree * 
      left  = badaatree_successor(self);
      aid   = badaatree_delete(left, badaatree_right(self), methods);
      badaatree_right_(self, aid);
      methods->setvalue(self, left);
    } else {
      BadAatree * 
      right = badaatree_predecessor(self);
      aid   = badaatree_delete(right, badaatree_left(self), methods);
      badaatree_left_(self, aid);
      methods->setvalue(self, right);
    }
  }
  /* Rebalance */
  badaatree_leveldownall(self);
  self  = badaatree_skew(self);
  aid   = badaatree_right(self);
  aid   = badaatree_skew(aid);
  badaatree_right_(self, aid);
  aid   = badaatree_right(badaatree_right(self));
  aid   = badaatree_skew(aid);
  badaatree_right_(badaatree_right(self), aid);
  self  = badaatree_split(self);
  aid   = badaatree_right(self);
  aid   = badaatree_skew(aid);
  badaatree_right_(self, aid);
  return self;
}

void badaatree_printgraph(BadAatree * self, int level) {
  if(level == 0) {
    printf("digraph { \n");
  }
  level++;
  if(badaatree_left(self)) { 
    printf("t%p -> t%p [color=red];\n", self, badaatree_left(self));
    badaatree_printgraph(badaatree_left(self), level);
  }
  if(badaatree_right(self)) {
    printf("t%p -> t%p [color=green];\n", self, badaatree_right(self));
    badaatree_printgraph(badaatree_right(self), level);
  }
  level--;
  if(level == 0) {
    printf("} \n");
  }
}

BadChildtree *
badchildtree_initall(BadChildtree * self, BadChildtree * parent, BadChildtree * child,
                    BadChildtree * next_sibling, BadChildtree * previous_sibling) {
 if(!self) return NULL;
 self->parent           = parent;
 self->child            = child;
 self->next             = next_sibling;
 self->previous         = previous_sibling;
 return self;
}

BadChildtree * 
badchildtree_initnull(BadChildtree * self) {
  return badchildtree_initall(self, NULL, NULL, NULL, NULL);
}

/* An empty badchildtree is simply a NULL tree. */
bool 
badchildtree_isempty(BadChildtree * self) {
  return !self;
}
  
BadChildtree * 
badchildtree_parent(BadChildtree * self) {
  if (badchildtree_isempty(self)) return NULL;
  return self->parent;
}


BadChildtree * 
badchildtree_child(BadChildtree * self) {
  if (badchildtree_isempty(self)) return NULL;
  return self->child;
}


BadChildtree * 
badchildtree_next(BadChildtree * self) {
  if (badchildtree_isempty(self)) return NULL;
  return self->next;
}

/* Returns the last sibling of self, or NULL if no siblings or if self is NULL. */
BadChildtree * 
badchildtree_lastsibling(BadChildtree * self) {
  BadChildtree * aid, * next; 
  if (badchildtree_isempty(self)) return NULL;
  aid   = self;
  next  = badchildtree_next(aid);
  /* Skip until the end of the list. */
  while(next) {
    aid   = next;
    next  = badchildtree_next(aid);
  }
  return aid;
}

/* Returns the last child of the tree, or NULL if no children or nself is NULL. */
BadChildtree * 
badchildtree_lastchild(BadChildtree * self) {
  if (badchildtree_isempty(self)) return NULL;
  return badchildtree_lastsibling(self->child);
}


/* Returns the previous sibling of self or NULL if no previous child. */
BadChildtree * 
badchildtree_previous(BadChildtree * self) {
  if (badchildtree_isempty(self)) return NULL;
  return self->previous;
}


/* Inserts the sibling as a sibling of self, right after self. Sets parent
 link of the sibling too. */
BadChildtree * 
badchildtree_insertsibling(BadChildtree * self, BadChildtree * sibling) {
  BadChildtree * next;
  if (badchildtree_isempty(self)) return sibling;  
  if (badchildtree_isempty(sibling)) return self;  
  next                          = self->next; 
  self->next                    = sibling;
  sibling->next                 = next;
  sibling->previous             = self;
  if (next) {
    next->previous              = sibling;    
  }
  sibling->parent               = self->parent;
  return self;
}

/* Appends sibling as the last sibling of self. 
 * Returns the new root element of the tree. If self is NULL, sibling is 
 * returned as that will be the new root of the BadChildtree. 
 */
BadChildtree * 
badchildtree_appendsibling(BadChildtree * self, BadChildtree * sibling) {
  BadChildtree * last;
  if (badchildtree_isempty(self)) return sibling;
  last = badchildtree_lastsibling(self);
  if(!last) {
    last = self; 
  }
  badchildtree_insertsibling(last, sibling);  
  return self;
}


/* Appends child as the last child sibling of self. 
 * Returns the new root element of the tree. If self is NULL, child is 
 * returned as that will be the new root of the BadChildtree. 
 */
BadChildtree * 
badchildtree_appendchild(BadChildtree * self, BadChildtree * child) {
  BadChildtree * last;
  if (badchildtree_isempty(self)) return child;
  last = badchildtree_lastchild(self);
  if(!last) {
    self->child = child;
  } else { 
    badchildtree_insertsibling(last, child);
  }
  child->parent = self;
  return self;
} 



/* Shows the tree in a text form on stdout, suitable for dot to concert to a graph. 
 */
void badchildtree_printgraph(BadChildtree * self, int level) {
  BadChildtree * aid; 
  if(level == 0) {
    printf("digraph { \n");
  }
  level++;
  for (aid = badchildtree_child(self) ; aid ; aid = badchildtree_next(aid)) { 
    printf("t%p -> t%p [color=red];\n", self, aid);
    if (badchildtree_parent(aid) == self) {
      printf("t%p -> t%p [color=blue];\n", aid, badchildtree_parent(aid));
    }    
    badchildtree_printgraph(aid, level);
  }
  level--;
  if(level == 0) {
    printf("} \n");
  }
}




/* Playing with a boxed C pointer. Avoids double frees or unintentional reuse.
*/
struct BadBoxptr_ {
  void * ptr_private;  
};


struct BadBoxptr_ badboxptr_make(void * ptr) {
  struct BadBoxptr_ self = {ptr};
  return self;
}

struct BadBoxptr_ badboxptr_malloc(size_t size) {
  return badboxptr_make(malloc(size));
}

void * badboxptr_borrow(struct BadBoxptr_ * self) {
  return self->ptr_private;
} 

int badboxptr_isnull(struct BadBoxptr_ * self) {
  return (!self->ptr_private);
} 

void * badboxptr_index(struct BadBoxptr_ * self, size_t offset) {
  if(badboxptr_isnull(self)) return NULL;
  return self->ptr_private + offset;
} 

void * badboxptr_rindex(struct BadBoxptr_ * self, size_t offset) {
  if(badboxptr_isnull(self)) return NULL;
  return self->ptr_private - offset;
} 


/* Takes ownership of data from box pointer "from". 
 * From box pointer will be set to NULL. 
 */
struct BadBoxptr_ * badboxptr_own(struct BadBoxptr_ * self, struct BadBoxptr_ * from) {
  self->ptr_private = from->ptr_private;
  from->ptr_private = NULL;
  return self->ptr_private;
} 


struct BadBoxptr_ * 
badboxptr_realloc(struct BadBoxptr_ * self, size_t size) {
  void * old = self->ptr_private;
  void * aid = realloc(old, size);
  if(aid) { self->ptr_private = aid; }
  return self;
}

void
badboxptr_free(struct BadBoxptr_ * self) {
  free(self->ptr_private);
  self->ptr_private = NULL;
}

/* Another more simple approach, but you cannot forget o use 
 * bad_xfree(&pointer), then...
 */
void bad_xfree(void ** ptrptr) {
  if(!ptrptr) return;
  free(*ptrptr);
  (*ptrptr) = NULL;
}

void* bad_xrealloc(void ** ptrptr, size_t size) {
  void*aid;
  if(!ptrptr) return NULL;
  aid = realloc(*ptrptr,size);
  if(aid) {
    (*ptrptr) = aid;
  }
  return aid;
}




struct BadVar_ badvar_makeint(int value) {
  struct BadVar_ result;
  result.type    = BADVAR_INT;
  result.value.i = value;
  return result;
}

struct BadVar_ badvar_makedouble(double value) {
  struct BadVar_ result;
  result.type      = BADVAR_DOUBLE;
  result.value.d   = value;
  return result;
}

struct BadVar_ badvar_makeptr(void * value) {
  struct BadVar_ result;
  result.type      = BADVAR_PTR;
  result.value.ptr = value;
  return result;
}


struct BadVar_ badvar_makecstr(char * value) {
  struct BadVar_ result;
  result.type      = BADVAR_PTR;
  result.value.ptr = value;
  return result;
}

struct BadVar_ badvar_makefptr(BadFunction * value) {
  struct BadVar_ result;
  result.type      = BADVAR_FPTR;
  result.value.fptr= value;
  return result;
}

void * badvar_ptr(BadVar self) {
  if (self.type != BADVAR_PTR) return NULL;
  return self.value.ptr;
}

BadFunction * badvar_fptr(BadVar self) {
  if (self.type != BADVAR_FPTR) return NULL;
  return self.value.fptr;
}

char * badvar_cstr(BadVar self) {
  if (self.type != BADVAR_CSTR) return NULL;
  return self.value.cstr;
}

int badvar_int(BadVar self) {
  if (self.type != BADVAR_INT) return 0;
  return self.value.i;
}

double badvar_double(BadVar self) {
  if (self.type != BADVAR_DOUBLE) return 0.0;
  return self.value.d;
}


void badvar_store(BadVar * self, void * ptr) {
  switch(self->type) { 
    case BADVAR_PTR   : (*((void **)ptr))        = self->value.ptr;
    break;
    case BADVAR_FPTR  : (*((BadFunction **)ptr)) = self->value.fptr;
    break; 
    case BADVAR_CSTR  : (*((char**)ptr))         = self->value.cstr;
    break; 
    case BADVAR_INT   : (*((int*)ptr))           = self->value.i;
    break;
    case BADVAR_DOUBLE: (*((double*)ptr))        = self->value.d;
    break;
    default: 
    break;
  }
}

BadVar * badvar_load(BadVar * self, int type, void * ptr) {
  self->type = type;
  switch(self->type) { 
    case BADVAR_PTR   : self->value.ptr  = ptr;
    break;
    case BADVAR_FPTR  : self->value.fptr = (BadFunction*) ptr;
    break; 
    case BADVAR_CSTR  : self->value.cstr = (char*) ptr;
    break; 
    case BADVAR_INT   : self->value.i    = (*((int *)ptr));
    break;
    case BADVAR_DOUBLE: self->value.d    = (*((double*)ptr));
    break;
    default: 
    break;
  }
  return self;
}

BadVar badvar_make(int type, void * valptr) {
  BadVar self;
  badvar_load(&self, type, valptr);
  return self;
}


int badvar_toarrayva(int argc, BadVar argv[], char * fmt, va_list args) {
  int index;
  for (index = 0; index < argc; index ++) {
    BadVar * var = argv + index;
    int type = (int) fmt[index];
    if (type == 0) return index;
    switch(type) { 
      case BADVAR_PTR : 
        argv[index] = badvar_makeptr(va_arg(args, void *)); 
      break;
      case BADVAR_FPTR  : 
        argv[index] = badvar_makefptr(va_arg(args, BadFunction *)); 
      break;
      case BADVAR_CSTR  : 
        argv[index] = badvar_makecstr(va_arg(args, char *)); 
      break;
      case BADVAR_INT   : 
        argv[index] = badvar_makeint(va_arg(args, int)); 
      break; 
      case BADVAR_DOUBLE: 
        argv[index] = badvar_makedouble(va_arg(args, double)); 
      break;
      default: 
      break;
    }
  }
  return index;
}

int badvar_toarraylen(int argc, BadVar argv[], char * fmt, ...) {
  va_list args;
  int result;
  va_start(args, fmt);
  result = badvar_toarrayva(argc, argv, fmt, args);
  va_end(args);
  return result;
}

int badvar_toarray(BadVar argv[], char * fmt, ...) {
  va_list args;
  int argc; 
  int result;
  argc = strlen(fmt);
  va_start(args, fmt);
  result = badvar_toarrayva(argc, argv, fmt, args);
  va_end(args);
  return result;
}


int badvar_fromarrayva(BadVar argv[], int argc, va_list args) {
  int index;
  for (index = 0; index < argc; index ++) {
    BadVar * var = argv + index;
    switch(var->type) { 
      case BADVAR_PTR : 
        (*va_arg(args, void **)) = badvar_ptr(argv[index]);
      break;
      case BADVAR_FPTR  : 
        (*va_arg(args, BadFunction **)) = badvar_fptr(argv[index]);
      break;
      case BADVAR_CSTR  : 
        (*va_arg(args, char **)) = badvar_cstr(argv[index]);
      break;
      case BADVAR_INT   : 
        (*va_arg(args, int *)) = badvar_int(argv[index]);
      break; 
      case BADVAR_DOUBLE: 
        (*va_arg(args, double *)) = badvar_double(argv[index]);
      break;
      default: 
      break;
    }
  }
  return index;
}

int badvar_fromarray(BadVar argv[], int argc, ...) {
  va_list args;
  int result;
  va_start(args, argc);
  result = badvar_fromarrayva(argv, argc, args);
  va_end(args);
  return result;
}

/** This may not be very useful since it's hard to define. */
struct BadVarList_ {
  struct BadVar_ var;
  struct BadListNode_    list;
};

struct BadVarList_ *
badvarlist_init(struct BadVarList_ * self, struct BadVar_ var) {
  self->var = var;
  badlistnode_init(&self->list);
  return self;
}  

BadVarList *
badvarlist_initva(BadVarList * self, char * format, va_list args) {
  if((!self) || (!format))  { return NULL; } 
  for( ; (*format) ; format++) {
    
    
  }
  return self;
}

BadVarList *
badvarlist_initf(BadVarList * self, char * format, ...) {
  BadVarList * result;
  va_list args;
  va_start(args, format);
  result = badvarlist_initva(self, format, args);
  va_end(args);
  return result;  
}

/*
 
This even works on gcc!

struct try_structfunc2_ { int value; int error ;} try_structfunc(int x, int y) {
  struct try_structfunc2_ result = { 1, 1};
  return result;
}
*/





















