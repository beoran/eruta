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
int bad_mini(int one, int two);
double bad_mind(double one, double two);
int bad_maxi(int one, int two);
double bad_maxd(double one, double two);
int bad_comparei(int one, int two);
int bad_compared(double one, double two);

/* Macro: bad_container(PTR, TYPE, MEMBER)
 This macro returns, for the given pointer, a pointer to a containing struct
 of type TYPE, in which PTR is a member named MEMBER. 
 This enables cool ways of type genericity and extension in plain C.
*/

#define bad_container(PTR, TYPE, MEMBER) \
        ((TYPE *)(((char *)(PTR)) - offsetof(TYPE, MEMBER)))

typedef struct BadList_ BadList;

typedef int BadListCompare(BadList * one, BadList * two);
typedef int BadListEach(BadList * elem, void * data);
typedef int BadListSearchValue(BadList * elem, void * data);

struct BadList_ {
  struct BadList_ * next;
  struct BadList_ * prev;
};

/** Returns the list node for this list element is part of, 
for the give list element, and data type*/
#define badlist_getdata(LIST, TYPE, MEMBER)     \
        badlist_data(LIST, offsetof(TYPE, MEMBER))

/** Shorthand for INLI_DATA(LIST, DATA, list) */
#define badlist_listdata(LIST, TYPE) inli_getdata(LIST, TYPE, list)

BadList * badlist_initempty(BadList * self);
bool   badlist_isempty(BadList * self);
BadList * badlist_initall(BadList * self , BadList * next , BadList * prev );
BadList * badlist_init(BadList * self);
BadList * badlist_unlink(BadList * self);
BadList * badlist_add(BadList * self , BadList * other );
BadList * badlist_next(BadList * self );
BadList * badlist_prev(BadList * self );
BadList * badlist_first(BadList * self );
BadList * badlist_last(BadList * self );
BadList * badlist_push(BadList * self , BadList * other );
BadList * badlist_unshift(BadList * self , BadList * other );
BadList * badlist_shift(BadList * self );
BadList * badlist_pop(BadList * self );

void * badlist_data(BadList * self , int offset);

BadList * 
badlist_search(BadList * self, BadListCompare * compare, BadList * tofind);

BadList * 
badlist_remove(BadList * self, BadListCompare * compare, BadList * toremove);

int badlist_each(BadList * self, BadListEach * each, void * data);

BadList * 
badlist_searchvalue(BadList * self, BadListSearchValue * compare, void * tofind);


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
                             BadAatreeCompare compare);
                             
BadAatree * badaatree_search(BadAatree * self, BadAatree * node,
                             BadAatreeCompare compare);


BadAatree * badaatree_leveldownall(BadAatree * self);

BadAatree * badaatree_successor(BadAatree * self);

BadAatree * badaatree_predecessor(BadAatree * self);

BadAatree * badaatree_delete(BadAatree * self, BadAatree * node, 
                             BadAatreeCompare * compare, 
                             BadAatreeSetValue * set);


typedef struct BadNodetree_ BadNodetree;

/*
 * Struct: BadNodetree
 * 
 * A BadNodetree is a tree where every node can have any number of children.
 * 
 */
struct BadNodetree_ {
  BadNodetree * parent;
  BadNodetree * child;
  BadNodetree * first_sibling;
  BadNodetree * last_sibling;
  BadNodetree * next_sibling;
  BadNodetree * previous_sibling;
};




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


#endif /* BAD_H_INCLUDED */