/**
* File: bad.h
*
* Bad means Beoran's Algorithms and Datastructures 
* (or Better Algorithms and Datastructures). 
* It's a generic library for C that has all sorts of useful data structures 
* and algorithms in it, under a permissive license.
* OK, BAD may be a bad name, but I hope you find the contents not too bad. :)
*
* One charactersistic of Bad is that it's data structures are normally 
* intrusive, that is they must be included in the struct that you want
* to put inside the given data structure. Also, normally the data
* structures are doubly linked for fast iteration in any direction.
* 
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

#ifndef BAD_H_INCLUDED
#define BAD_H_INCLUDED

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <stddef.h>

/* Const: FALSE.
* Of course we define our own boolean value for FALSE.
*/
#ifndef FALSE 
#define FALSE 0
#endif 

/* Const: TRUE.
* Of course we define our own boolean value for TRUE.
*/
#ifndef TRUE 
#define TRUE (!FALSE)
#endif 

/* Macro: bool 
* For integers that are used as boolean values.
*/
#ifndef bool 
#define bool int
#endif


/* Some simple numerical comparison, min, max, etc functions. */

int     bad_mini(int one, int two);
double  bad_mind(double one, double two); 
float   bad_minf(float one, float two); 
int     bad_maxi(int one, int two);
double  bad_maxd(double one, double two); 
float   bad_maxf(float one, float two); 
int     bad_comparei(int one, int two);
int     bad_compared(double one, double two);
int     bad_comparef(float one, float two);
int     bad_clampi(int value , int min, int max) ;
double  bad_clampd(double value , double min, double max); 
float   bad_clampf(float value , float min, float max); 
int bad_outofboundsi(int value, int min, int max);
int bad_outofboundsd(int value, double min, double max);
int bad_outofboundsf(int value, float min, float max);


/* Macro: bad_container(PTR, TYPE, MEMBER)
 This macro returns, for the given pointer, a pointer to a containing struct
 of type TYPE, in which PTR is a member named MEMBER. 
 This enables cool ways of type genericity and extension in plain C.
*/

#define bad_container(PTR, TYPE, MEMBER) \
        ((TYPE *)(((char *)(PTR)) - offsetof(TYPE, MEMBER)))

typedef struct BadListNode_     BadListNode;
typedef struct BadList_         BadList;


typedef int BadListNodeCompare(BadListNode * one, BadListNode * two);
typedef int BadListNodeEach(BadListNode * elem, void * data);
typedef int BadListNodeSearchValue(BadListNode * elem, void * data);


/* Struct: BadList_
 * BadListNode is a doubly linked list head. 
 * 
 */
struct BadList_ {
  struct BadListNode_ * head;
  struct BadListNode_ * tail;
  int                   size;
};


/* Struct: BadListNode_
 * BadListNode is a doubly linked list node. 
 * 
 */
struct BadListNode_ {
  struct BadListNode_ * next;
  struct BadListNode_ * prev;
};

/* Returns the list node for this list element is part of, 
for the give list element, and data type*/
#define badlistnode_getdata(LIST, TYPE, MEMBER)     \
        badlistnode_data(LIST, offsetof(TYPE, MEMBER))

/* Shorthand for INLI_DATA(LIST, DATA, list) */
#define badlistnode_listdata(LIST, TYPE) inli_getdata(LIST, TYPE, list)

BadListNode * badlistnode_initempty(BadListNode * self);
bool   badlistnode_isempty(BadListNode * self);
BadListNode * badlistnode_initall(BadListNode * self , BadListNode * next , BadListNode * prev );
BadListNode * badlistnode_init(BadListNode * self);
BadListNode * badlistnode_unlink(BadListNode * self);
BadListNode * badlistnode_add(BadListNode * self , BadListNode * other );
BadListNode * badlistnode_next(BadListNode * self );
BadListNode * badlistnode_prev(BadListNode * self );
BadListNode * badlistnode_first(BadListNode * self );
BadListNode * badlistnode_last(BadListNode * self );
BadListNode * badlistnode_push(BadListNode * self , BadListNode * other );
BadListNode * badlistnode_unshift(BadListNode * self , BadListNode * other );
BadListNode * badlistnode_shift(BadListNode * self );
BadListNode * badlistnode_pop(BadListNode * self );

void * badlistnode_data(BadListNode * self , int offset);

BadListNode * 
badlistnode_search(BadListNode * self, BadListNodeCompare * compare, BadListNode * tofind);

BadListNode * 
badlistnode_remove(BadListNode * self, BadListNodeCompare * compare, BadListNode * toremove);

int badlistnode_each(BadListNode * self, BadListNodeEach * each, void * data);

BadListNode * 
badlistnode_searchvalue(BadListNode * self, BadListNodeSearchValue * compare, void * tofind);


BadList *       badlist_init  (BadList *  self);
BadList *       badlist_add   (BadList *  self, BadListNode * node);
BadList *       badlist_shift (BadList *  self, BadListNode * node);
BadList *       badlist_remove(BadList *  self, BadListNode * node);
BadListNode *   badlist_head  (BadList *  self);
BadListNode *   badlist_tail  (BadList *  self);
int             badlist_size  (BadList *  self);



typedef struct BadBitree_ BadBitree;

typedef int BadBitreeCompare(BadBitree * one, BadBitree * two);


struct BadBitree_ {
  struct BadBitree_  * up;
  struct BadBitree_  * left;
  struct BadBitree_  * right;
};

typedef struct BadAatree_ BadAatree;
typedef int BadAatreeCompare(BadAatree * one, BadAatree * two);
typedef int BadAatreeSetValue(BadAatree * to, BadAatree * from);
typedef int BadAatreeCompareKey(BadAatree * self, void * key);
typedef void * BadAatreeGetKey(BadAatree * self);


typedef struct BadAatreeMethods_ BadAatreeMethods;

struct BadAatreeMethods_ {
  BadAatreeCompare    * compare;
  BadAatreeSetValue   * setvalue;
  BadAatreeCompareKey * comparekey;
  BadAatreeCompareKey * getkey;  
};

struct BadAatree_ {
  struct BadBitree_    tree;
  int                  level;
};


BadBitree * 
badbitree_initall(BadBitree * self, BadBitree * left, BadBitree * right, 
                  BadBitree * up);

BadBitree *
badbitree_init(BadBitree * self);

BadBitree *
badbitree_left(BadBitree * self);

BadBitree *
badbitree_right(BadBitree * self);

BadBitree *
badbitree_up(BadBitree * self);


BadBitree *
badbitree_up_(BadBitree * self, BadBitree * newup);
BadBitree *
badbitree_left_(BadBitree * self, BadBitree * newleft);

BadBitree *
badbitree_right_(BadBitree * self, BadBitree * newright);

bool badbitree_isleaf(BadBitree * self);

BadAatree * 
badaatree_initall(BadAatree * self, BadAatree * left, BadAatree * right, 
                  BadAatree * up, int level);

BadAatree *
badaatree_init(BadAatree * self);

BadAatree *
badaatree_left(BadAatree * self);

BadAatree *
badaatree_right(BadAatree * self);

BadAatree *
badaatree_up(BadAatree * self);

BadAatree *
badaatree_left_(BadAatree * self, BadAatree * other);

BadAatree *
badaatree_right_(BadAatree * self, BadAatree * other);

BadAatree *
badaatree_up_(BadAatree * self, BadAatree * other);

int badaatree_level(BadAatree * self);

int badaatree_level_(BadAatree * self, int newlevel);

bool badaatree_isleaf(BadAatree * self);

BadAatree * badaatree_skew(BadAatree * self);

BadAatree * badaatree_split(BadAatree * self);

BadAatree * badaatree_insert(BadAatree * self, BadAatree * node, 
                             BadAatreeMethods * methods);
                             
BadAatree * badaatree_search(BadAatree * self, BadAatree * node,
                             BadAatreeMethods * methods);

BadAatree * badaatree_searchkey(BadAatree * self, void * key,
                                BadAatreeMethods * methods);

BadAatree * badaatree_leveldownall(BadAatree * self);

BadAatree * badaatree_successor(BadAatree * self);

BadAatree * badaatree_predecessor(BadAatree * self);

BadAatree * badaatree_delete(BadAatree * self, BadAatree * node, 
                             BadAatreeMethods * methods);


BadAatree * badaatree_deletekey(BadAatree * self, void * key, 
                                BadAatreeMethods * methods);

typedef struct BadChildtree_ BadChildtree;

/*
 * Struct: BadChildtree
 * 
 * A BadChildtree is a tree where every node can have any number of children.
 * 
 */
struct BadChildtree_ {
  BadChildtree * parent;
  BadChildtree * child;  
  BadChildtree * next;
  BadChildtree * previous;
};


BadChildtree *
badchildtree_initall(BadChildtree * self, BadChildtree * parent, BadChildtree * child,
                    BadChildtree * next_sibling, BadChildtree * previous_sibling); 
BadChildtree * 
badchildtree_initnull(BadChildtree * self);
bool 
badchildtree_isempty(BadChildtree * self); 
BadChildtree * 
badchildtree_parent(BadChildtree * self); 
BadChildtree * 
badchildtree_child(BadChildtree * self); 
BadChildtree * 
badchildtree_next(BadChildtree * self);
BadChildtree * 
badchildtree_lastsibling(BadChildtree * self);
BadChildtree * 
badchildtree_lastchild(BadChildtree * self);
BadChildtree * 
badchildtree_previous(BadChildtree * self);
BadChildtree * 
badchildtree_insertsibling(BadChildtree * self, BadChildtree * sibling);
BadChildtree * 
badchildtree_appendsibling(BadChildtree * self, BadChildtree * sibling);
BadChildtree * 
badchildtree_appendchild(BadChildtree * self, BadChildtree * child);
void badchildtree_printgraph(BadChildtree * self, int level);



typedef struct BadVar_     BadVar;
typedef struct BadVarList_ BadVarList;

typedef void BadFunction(void);
typedef BadFunction * BadFunctionPtr;



union BadVarUnion_ {
  void        * ptr;
  BadFunction * fptr;
  char        * cstr;
  int           i;
  double        d;
};

enum BadVarEnum_ {
  BADVAR_NONE   =  0 ,
  BADVAR_PTR    = 'p',
  BADVAR_FPTR   = 'P',
  BADVAR_CSTR   = 's',
  BADVAR_INT    = 'i',
  BADVAR_DOUBLE = 'f',
};  
 

struct BadVar_ {
  enum  BadVarEnum_  type;
  union BadVarUnion_ value;
};


struct BadVar_ badvar_makeint(int value);
struct BadVar_ badvar_makedouble(double value);
struct BadVar_ badvar_makeptr(void * value);
struct BadVar_ badvar_makecstr(char * value);
struct BadVar_ badvar_makefptr(BadFunction * value);

void * badvar_ptr(BadVar self);
BadFunction * badvar_fptr(BadVar self);
char * badvar_cstr(BadVar self);
int badvar_int(BadVar self);
double badvar_double(BadVar self);

void badvar_store(BadVar * self, void * ptr);
BadVar * badvar_load(BadVar * self, int type, void * ptr) ;
BadVar badvar_make(int type, void * valptr) ;

int badvar_toarrayva(int argc, BadVar argv[], char * fmt, va_list args) ;
int badvar_toarraylen(int argc, BadVar argv[], char * fmt, ...);
int badvar_toarray(BadVar argv[], char * fmt, ...);
int badvar_fromarrayva(BadVar argv[], int argc, va_list args);
int badvar_fromarray(BadVar argv[], int argc, ...);


/* a "Stab" is a String Table, that is, a table of void * pointers 
 index by strings. The Stab does not own it's pointers
 but it does own copies of the string keys, which 
 it will clean up when needed. */
typedef struct BadStab_ BadStab;
typedef struct BadStabEntry_ BadStabEntry;

struct BadStabEntry_ {
  char * key;
  void * value;
};  

struct BadStab_ {
  BadStabEntry * entries;
  int size;
};

BadStab * badstab_init(BadStab * self);
BadStab * badstab_done(BadStab * self);
void * badstab_put(BadStab * self, char * key, void * value);   
void * badstab_get(BadStab * self, char * key);   


void * badgar_new(size_t nmemb, size_t size);
void * badgar_resize(void ** arr, size_t *  nmemb, size_t size, int delta);
void * badgar_get(void * arr, size_t nmemb, size_t size, size_t index);
void * badgar_put(void * arr, size_t nmemb, size_t size, size_t index, void * data);
void * badgar_free(void * arr);

void * badpar_new(size_t nmemb);
void * badpar_resize(void ** arr, size_t * nmemb, int delta);
void * badpar_get(void * arr, size_t nmemb, size_t index);
void * badpar_put(void * arr, size_t nmemb, size_t index, void * data);
void * badpar_free(void * arr);


#endif /* BAD_H_INCLUDED */