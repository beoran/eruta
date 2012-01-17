#ifndef SI_TABLE_H
#define SI_TABLE_H

#include "si.h"
/** Simple hash table.  */

struct SiTable_;
typedef struct SiTable_ SiTable; 

/* Jenkins hash function. */
uint32_t sihash_jenkins(char *key, size_t len);

typedef int SiTableKeyComparer(void * key1, void * key2);
typedef uint32_t SiTableKeyHasher(void * key);

struct SiTableClass_ {
  SiTableKeyHasher   * hash;
  SiTableKeyComparer * compare; 
};

typedef struct SiTableClass_ SiTableClass; 

SiTable * sitable_done(SiTable * self); 

SiTable * sitable_free(SiTable * self);

/** Empties all entries in the table. */
SiTable * sitable_clear(SiTable * self);

SiTable * sitable_initroom(SiTable * self, SiTableClass * klass, uint32_t room);

SiTable * sitable_alloc();

SiTable * sitable_init(SiTable * self, SiTableClass * klass);


SiTable * sitable_newroom(SiTableClass * klass, uint32_t room);

SiTable * sitable_new(SiTableClass * klass);

/** Calculates the hash value of the given data pointer using self->klass.hash*/
uint32_t sitable_hash(SiTable * self, void * ptr);

/** Compares the given data pointers using self->klass.compare*/
int sitable_compare(SiTable * self, void * pa, void * pb);

/** Returns the amount of room still available in the table. */
uint32_t sitable_room(SiTable * self);

/** Gets a value from a hash table. */
void * sitable_get(SiTable * self, void * key);

/** Removes a value from a hash table. */
void * sitable_drop(SiTable * self, void * key);

/** Stores a value in the hash table. */
void * sitable_set(SiTable * self, void * key, void * value);




#endif


