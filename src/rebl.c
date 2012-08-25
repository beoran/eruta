
#include "rebl.h"



/* Rebl is a red/black tree structure  Doesn't work yet */

#include <stdlib.h>


#ifdef UNDER_CONSTRUCTION___

#define REBL_LESS(REBL, A, B)   (REBL->compare(A,B) < 1)
#define REBL_EQUAL(REBL, A, B)  (REBL->compare(A,B) == 0)

/* Red-Black tree colors */
enum ReblColor_ { BLACK, RED } ;

/* Red black tree node, also a red black tree in total. */
struct Rebl_ {
  struct Rebl_ *parent;       /* parent */  
  struct Rebl_ *left;         /* left child */
  struct Rebl_ *right;        /* right child */
    
  ReblColor     color;        /* node color (BLACK, RED) */
  void        * data;         /* data stored in node */
  ReblCompare * compare;      /* comparison/equality function to use. */
};


struct Rebl_ sentinel           = { NULL, NULL, NULL, BLACK, NULL, NULL};
#define SENTINEL &sentinel




#endif
