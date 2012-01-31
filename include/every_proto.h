#ifndef EVERY_PROTO_H
#define EVERY_PROTO_H
/*
This file was autogenerated from src/every.c
by bin/genproto on 2012-01-31 09:33:01 +0100
Please do not hand edit.
*/

/** Allocates an uninitialized every object. */
Every * every_alloc();

/** Deinitializes an every object.  */
Every * every_done(Every * self);

/** Frees an every object. */
Every * every_free(Every * self);

/** Initializes an every object. */
Every * every_init(Every * self, EveryActs * acts);

/** Creates a new every object. */
Every * every_new(EveryActs * acts);

/** Gets the current object pointed to by the Every object. */
void * every_get(Every * self);

/** Puts an object at the current position if possible.
* Returns NULL if the put failed.
*/
void * every_put(Every * self, void * data);

/** Moves on to the next object and returns it.
* Return NULL if no next object.
*/
Every * every_next(Every * self, void * data);

/** Initializes an EachElement */
Each * each_init(Each * self, void * on, void * data);

/** Moves on to next element, incrementing index. */
Each * each_next(Each * self, void * now);

/** Gets now pointer of each struct */
void * each_now(Each * self);

/** Gets on pointer of each struct */
void * each_on(Each * self);

/** Gets extra data pointer of each struct */
void * each_extra(Each * self);

/** Gets index pointer of each struct, -1 if the struct is NULL. */
int each_index(Each * self);

#endif // EVERY_PROTO_H

