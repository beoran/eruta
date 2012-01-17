/** Cursor (iterator) types. */
#ifndef SI_CURSOR_H
#define SI_CURSOR_H

#include <stdlib.h>

struct SiCursor_;
typedef struct SiCursor_ SiCursor;

struct SiCursorClass_;
typedef struct SiCursorClass_ SiCursorClass;

typedef SiCursor * (SiCursorNextFunc)(SiCursor * cursor);
typedef void *     (SiCursorDataFunc)(SiCursor * cursor);
typedef void *     (SiCursorFunc)(SiCursor * cursor, void * extra);


/** Stuct for the flexible methods of the SiCursor. */
struct SiCursorClass_ {
  SiCursorNextFunc * next;
  SiCursorDataFunc * data;
};

/** Initializes a cursor for the given collection, active element, index, and
cursor class. 
*/
SiCursor * sicursor_init(SiCursor * self, void * collection, 
                         void * active  , size_t index,
                         SiCursorClass * klass); 

/** allocates a new cursor. */
SiCursor * sicursor_alloc();

/** Creates a new cursor for the given collection, active element, index, and
cursor class. */ 
SiCursor * sicursor_new(void * collection, void * active, 
                        size_t index, SiCursorClass * klass);

/** Frees the cursor. */
SiCursor * sicursor_free(SiCursor * self); 

/** Returns the collection the cursor is walking through.
* Most collections /cannot/ be modified while walking them. 
*/
void * sicursor_collection(SiCursor * self);

/** Returns the currently active element of the cursor. */
void * sicursor_active(SiCursor * self);

/** Gets the current active element for this iterator, cast to the given type.*/
#define SICURSOR_ACTIVE(CURSOR, TYPE) ((TYPE*)sicursor_active(CURSOR))


/** Sets the currently active element of the cursor. 
* Only for use in collection SiCursorClass methods. 
*/
void * sicursor_active_(SiCursor * self, void * active);

/** Returns the currently active index of the cursor. */
size_t sicursor_index(SiCursor * self);

/** Sets the currently active index of the cursor. 
* Only for use in collection SiCursorClass methods. 
*/
size_t sicursor_index_(SiCursor * self, size_t index);

/** Updates the cursor to the next element and returns it.
* When the last element is reached, this function /frees/ the cursor, and 
* returns NULL. This means that the cursor cannot be used anymore after 
* iterating. Copy out any data you still need.   
*/
SiCursor * sicursor_next(SiCursor * self);

/** Gets the current active data for this iterator. */
void * sicursor_data(SiCursor * self); 

/** Gets the current active data for this iterator, cast to the given type. */
#define SICURSOR_DATA(CURSOR, TYPE) ((TYPE*)sicursor_data(CURSOR))

/** Iterates over the colllection that the cursor is for, 
calling func(cursor) each time until the end is reached. */
void * sicursor_each(SiCursor * cursor, SiCursorFunc * func, void * extra); 




#endif
