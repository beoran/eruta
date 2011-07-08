#include "si_mem.h"
#include "si_array.h"
#include "si_table.h"
#include "si_number.h"
#include "si_walker.h"
#include <string.h>


#define SITABLE_ROOM_DEFAULT 128
#define SITABLE_CELLAR_DEFAULT 16


/* Jenkins hash function. */
uint32_t sihash_jenkins(char *key, size_t len) {
  uint32_t hash, i;

  for(hash = i = 0; i < len; ++i) {
     hash += key[i];
     hash += (hash << 10);
     hash ^= (hash >> 6);
  }
  hash += (hash << 3);
  hash ^= (hash >> 11);
  hash += (hash << 15);
  return hash;
}

/** Hash function that hashes a C string. */
uint32_t sitable_hash_cstr(void * key) {
  char * cstr = (char *) key;
  return sihash_jenkins(cstr, strlen(cstr));
}

/** Hash function that hashes a uint32_t. */
uint32_t sitable_hash_uint32(void * key) {  
  return sihash_jenkins((char *)&key, sizeof(uint32_t));
}

struct SiTablePail_;
typedef struct SiTablePail_ SiTablePail;

/** Hash bucket */
struct SiTablePail_ {
  void          * key;
  void          * data;
  SiTablePail   * next;
  uint32_t        hash;
};

/** Initializes a hash table bucket, here abbrefviated to "pail". */
SiTablePail * sitablepail_init(SiTablePail * self, 
               void * key, void * data, void * next, uint32_t hash) {
  self->key   = key;
  self->data  = data;
  self->next  = next;
  self->hash  = hash;
  return self;
}

/** Accessor for the tablepail's data */
void * sitablepail_data(SiTablePail * self) {
  if(!self) return NULL;
  return self->data;
}

/** Accessor for the tablepail's key */
void * sitablepail_key(SiTablePail * self) {
  if(!self) return NULL;
  return self->key;
}

/** Gets the next pail in the chain.  */
SiTablePail * sitablepail_next(SiTablePail * self) {
  if(!self) return NULL;
  return self->next;
}

/** Sets the next pail following this one */
SiTablePail * sitablepail_next_(SiTablePail * self, SiTablePail * next) {
  if(!self) return NULL;
  return self->next = next;
}

/** Accessor for the tablepail's hash value */
uint32_t sitablepail_hash(SiTablePail * self) {
  if(!self) return 0;
  return self->hash;
}


/** Initializes a hash bucket to be empty. */
SiTablePail * sitablepail_initempty(SiTablePail * self) {
  return sitablepail_init(self, NULL, NULL, NULL, 0);
}

/** Returns zero if the hash of the bucket is equal to hash, nonzero if not.  */
int sitablepail_cmphash(SiTablePail * self, uint32_t hash) {
  return self->hash != hash;
}

/** Returns nonzero if the pail is empty, zero if not . */
int sitablepail_empty_p(SiTablePail * self) {
  return !(self->key);
}

/** empties a pail, without breaking the link chain */
SiTablePail * sitablepail_emptynobreak(SiTablePail * self) {
  return sitablepail_init(self, NULL, NULL, self->next, 0);
}



/** The algorithm implemented is a coalesced hash table. */
struct SiTable_  {
  SiArray      * pails  ;
  SiArray      * cellar ;
  SiTableClass * klass  ;
};


static SiTableClass sitable_defaultclass = {
    (SiTableKeyHasher *)      sitable_hash_cstr,
    (SiTableKeyComparer *)    strcmp,
};

/** Room in the table's pails array, in amount of pails. */
size_t sitable_pailsroom(SiTable * self) {
  if(!self) return 0;
  return siarray_room(self->pails);
}

/** Room in the table's cellar array, in amount of pails. */
size_t sitable_cellarroom(SiTable * self) {
  if(!self) return 0;
  return siarray_room(self->cellar);
}

/** Size of the table's cellar array, in amount of pails. */
size_t sitable_cellarsize(SiTable * self) {
  if(!self) return 0;
  return siarray_size(self->cellar);
}


/** Returns nonzero if the cellar is full, zero if it isn't. */
int sitable_cellarfull_p(SiTable * self) {
  if(!self) return 0;
  return siarray_full_p(self->cellar);
}



/** Returns the table's pails array. */
SiArray * sitable_pails(SiTable * self) {
  if(!self) return 0; 
  return self->pails;
}

/** Returns the table's cellar array. */
SiArray * sitable_cellar(SiTable * self) {
  if(!self) return 0; 
  return self->cellar;
}

/** Cleans up and empties a table. */
SiTable * sitable_done(SiTable * self) {
  if(!self) return NULL;
  siarray_free(self->pails);
  siarray_free(self->cellar);
  self->pails   = self->cellar = NULL;
  return self;
}

/* Frees a table */
SiTable * sitable_free(SiTable * self) {
  sitable_done(self);
  return si_free(self);
}

// Gets a pointer to the (pails) pail at the given index, 
// which is not checked and should be valid.
static SiTablePail * sitable_getpail(SiTable * self, uint32_t index) {
  return (SiTablePail *) 
      siarray_getdata(self->pails, index);
}

// Gets a pointer to the cellar pail at the given index, 
// which is not checked and should be valid.
static SiTablePail * sitable_getcellarpail(SiTable * self, uint32_t index) {
  return (SiTablePail *) 
      siarray_getdata(self->cellar, index);
}

/** Returns the next available cellar cell, or NULL if the cellar is full. */
SiTablePail * sitable_getnextcellar(SiTable * self) {
  size_t size;
  if(siarray_full_p(self->cellar)) return NULL;
  size = sitable_cellarsize(self);
  siarray_size_(self->cellar, size + 1); // Size 1 up 
  return sitable_getcellarpail(self, size);
}


/** Empties all entries in the table. */
SiTable * sitable_clear(SiTable * self) {
  int index, size;
  size = sitable_pailsroom(self);
  for(index = 0; index < size; index++) {
    void * data = sitable_getpail(self, index);
    sitablepail_initempty(data);
  }
  size = sitable_cellarroom(self);
  for(index = 0; index < size; index++) {
    void * data = sitable_getcellarpail(self, index);
    sitablepail_initempty(data);
  }  
  return self;
}


/** Handes one table pail to be walked. */
void * sitable_walkonepail(SiTable * self, size_t index, 
                        SiWalker * walker, void * extra) {
    SiTablePail * pail = sitable_getpail(self, index);
    if(!pail) return NULL;
    if(sitablepail_empty_p(pail)) return NULL;
    return walker(sitablepail_data(pail), sitablepail_key(pail), self, extra);
}

/** Handes one cellar table pail to be walked. */
void * sitable_walkonecellar(SiTable * self, size_t index, 
                        SiWalker * walker, void * extra) {
    SiTablePail * pail = sitable_getcellarpail(self, index);
    if(!pail) return NULL;
    if(sitablepail_empty_p(pail)) return NULL;
    return walker(sitablepail_data(pail), sitablepail_key(pail), self, extra);
}


/** Walks over the table. */
void * sitable_walk(SiTable * self, SiWalker * walker, void * extra) {
  size_t index, size;
  size = sitable_pailsroom(self);
  for(index = 0; index < size; index++) {
    void * data = sitable_walkonepail(self, index, walker, extra);
    if(data) return data;
  }
  size = sitable_cellarroom(self);
  for(index = 0; index < size; index++) {
    void * data = sitable_walkonecellar(self, index, walker, extra);
    if(data) return data;
  }
  return NULL;
}





/** Initializes the table with the given room. */
SiTable * sitable_initroom(SiTable * self, SiTableClass * klass, uint32_t room)
{
  size_t rpails, rcellar;
  if(!self) return NULL;
  self->klass   = klass ? klass : (&sitable_defaultclass);
  rpails        = si_max_uint32(room    , SITABLE_ROOM_DEFAULT);
  rcellar       = si_max_uint32(room / 8,  SITABLE_CELLAR_DEFAULT);
  self->pails   = siarray_new(rpails, sizeof(SiTablePail));
  self->cellar  = siarray_new(rcellar, sizeof(SiTablePail));
  if((!self->pails) || (!self->cellar)) return sitable_done(self);
  sitable_clear(self);
  return self;
}

SiTable * sitable_alloc() {
  return SI_ALLOCATE(SiTable);
}

SiTable * sitable_init(SiTable * self, SiTableClass * klass) {
  return sitable_initroom(self, klass, SITABLE_ROOM_DEFAULT);
}

SiTable * sitable_newroom(SiTableClass * klass, uint32_t room) {
  SiTable * aid;
  SiTable * self = sitable_alloc();
  if(!self) return NULL;
  aid = sitable_initroom(self, klass, room);
  if(!aid) return sitable_free(self);  
  return self;
}

SiTable * sitable_new(SiTableClass * klass) {
  return sitable_newroom(klass, SITABLE_ROOM_DEFAULT);
}

/** Calculates the hash value of the given data pointer using self->klass.hash*/
uint32_t sitable_hash(SiTable * self, void * ptr) {
  return self->klass->hash(ptr);
}

/** Compares the given data pointers using self->klass.compare*/
int sitable_compare(SiTable * self, void * pa, void * pb) {
  return self->klass->compare(pa, pb);
}

/** Checks if the given bucket contains the given key with the given hash. */
int sitable_pailok(SiTable * self, SiTablePail * pail, 
                   void * key, uint32_t hash) {
  if(!pail) return FALSE;
  if(sitablepail_cmphash(pail, hash)) return FALSE;
  if(sitable_compare(self, pail->key, key)) return FALSE;
  return TRUE;
}


/** Gets a value that matches key from a hash table. */
void * sitable_get(SiTable * self, void * key) {
  uint32_t hash, index;
  SiTablePail * pail;
  if(!self) return NULL;
  hash    = sitable_hash(self, key);
  index   = hash % sitable_pailsroom(self);
  pail    = sitable_getpail(self, index);
  if (!pail)  return NULL; 
  while(pail) {
    if(sitable_pailok(self, pail, key, hash)) return pail->data;
    // return the pail if it's OK. 
    pail = pail->next; // Follow chain of linked pails.
  }
  // If we get here, no more links, and not found. Return null. 
  return NULL;
}

/** Removes a value that matches key from a hash table. Returns NULL if 
delete failed, or erased value. */
void * sitable_drop(SiTable * self, void * key) {
  uint32_t hash, index;
  SiTablePail * pail;
  if(!self) return NULL;
  hash    = sitable_hash(self, key);
  index   = hash % sitable_pailsroom(self);
  pail    = sitable_getpail(self, index);
  if (!pail)  return NULL; 
  while(pail) {
    if(sitable_pailok(self, pail, key, hash)) {
      void * data = pail->data;
      sitablepail_emptynobreak(pail); 
      // set the pail as empty, but don't break it's next links. 
      // this allows the empty pai lto be reused, and the linked data stays
      // available and linked.
      return data;
    }
    // return the pail if it's OK. 
    pail = pail->next; // Follow chain of linked pails.
  }
  // If we get here, no more links, and not found. Return null. 
  return NULL;
}

SiTable * sitable_grow(SiTable * self) {
  // double the size of the cellar block and the pails block, so collisions 
  // can be handled 
  // NOTE: should also grow table but rehash is slow...
  void * mok = siarray_grow(sitable_cellar(self), sitable_cellarroom(self)*2);
  /*void * cok = siarray_grow(sitable_pails(self), sitable_pailsroom(self)*2);*/
  if(!mok) return NULL;
  // if(!cok) return NULL; // XXX: rehash the table here!  
  return self;
}



/** Stores a value in the hash table. */
void * sitable_set(SiTable * self, void * key, void * value) {
  uint32_t hash, index;
  SiTablePail * pail, * nextpail = NULL;
  if(!self) return NULL;
  hash    = sitable_hash(self, key);
  index   = hash % sitable_pailsroom(self);
  pail    = sitable_getpail(self, index);
  if (!pail) return NULL;
  while(pail->next && !sitablepail_empty_p(pail))  {
      pail = pail->next;
      // Follow chain of linked, nonempty pails. If the pail is empty or
      // the chain is broken, we can store the data in the pail.
  }   
  // If we get here, no more links, so we have to add the value either here,
  // or add a new linked pail.
  // No next pail, so so link to new one.
  if (!pail->next) {
    if(sitable_cellarfull_p(self)) {  // The cellar is full.
      if(!sitable_grow(self)) return NULL; 
     // Grow the hashtable, return null if that failed. 
    }
    nextpail = sitable_getnextcellar(self);
    // If we din't ge ta next pail, the cellar is full. 
    // Grow the table and try again.
    if(!nextpail) {
      if(!sitable_grow(self)) return NULL;
      nextpail = sitable_getnextcellar(self);
      if(!nextpail) return NULL;
    } 
    pail->next = nextpail;
    pail = nextpail;
  } 
  siarray_sizeup(self->pails); // one more bucket in use. 
  return sitablepail_init(pail, key, value, NULL, hash);
}







