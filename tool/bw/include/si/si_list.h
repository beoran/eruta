#ifndef SI_LIST_H
#define SI_LIST_H

#include "si_cursor.h"
#include "si_walker.h"

struct SiList_;
typedef struct SiList_ SiList;

/** Links one list to the other. Returns the first list. */
SiList * silist_linkto(SiList * self, SiList * next);

/** Allocates a list. */
SiList * silist_alloc();

/** Initializes an empty linklist. */
SiList * silist_new(void * data);

/** Initializes self as an empty linklist. */
SiList * silist_init(SiList * self, void * data);

/** Gets the data item associated with this list node. */
void * silist_data(SiList * self);

/** Gets the next item on the list */
SiList * silist_next(SiList * self);

/** Gets the previous item on the list */
SiList * silist_previous(SiList * self); 

/** Frees self and self only. */
SiList * silist_free_self(SiList * self);

/** Frees self and all forwardly linked links. Does not free backwards links. */
SiList * silist_free(SiList * self);

/** Frees all forwardly linked links only. */
SiList * silist_done(SiList * self);

/** Returns the last item of the list */
SiList * silist_last(SiList * self);

/** Add a list to the end of a list */
SiList * silist_addlist(SiList * self, SiList * next);

/** Creates a new link, stores a pointer to data in it, 
and appends it to this list. */
SiList * silist_add(SiList * self, void * data);

/** Gets a cursor to traverse the list. */
SiCursor * silist_cursor(SiList * self);

/** Gets the index-th element in the list. This is an O(index) operation.
* Returns NULL if the list does not have index items in it.  
*/
SiList * silist_getlist(SiList * self, size_t index); 

/** Gets the data of index-th element in the list. This is an O(index)
operation. Returns NULL if the list does not have index items in it. */
void * silist_get(SiList * self, size_t index);

/** Walks over the list using a SiWalker callback. */
void * silist_walk(SiList * self, SiWalker * walker, void * extra);



#endif
