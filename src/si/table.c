
#include "si_mem.h"
#include "si_table.h"
#include "si_number.h"
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


/** The algorithm implemented is a coalesced hash table. */
struct SiTable_  {
  SiMem        * mem;
  uint32_t       main;   // Room of the main table
  uint32_t       cellar; // Room in the of cellar
  uint32_t       bindex; // Bucket index
  uint32_t       cindex; // Cellar index
  SiTableClass * klass;
};


static SiTableClass sitable_defaultclass = {    
    (SiTableKeyHasher *)      sitable_hash_cstr,
    (SiTableKeyComparer *)    strcmp,
};

/** Room in the table's mem block, in amount of pails. */
size_t sitable_room(SiTable * table) {
  if(!table) return 0;
  return simem_room(table->mem) / sizeof(SiTablePail);
}

/** Room in the main table, in amount of pails. */
uint32_t sitable_main(SiTable * table) {
  if(!table) return 0; 
  return table->main;
}

/** Room in the table's cellar, in amount of pails. */
uint32_t sitable_cellar(SiTable * table) {
  if(!table) return 0; 
  return table->cellar;
}

SiTable * sitable_done(SiTable * self) {
  simem_free(self->mem);
  self->mem   = NULL;
  self->cellar = self->bindex = self->cindex = 0;
  return self;
}

SiTable * sitable_free(SiTable * self) {
  simem_free(self->mem);
  return si_free(self);
}

// Gets a pointer to the pail at the given index, whihc is not checked 
// and should be valid.
static SiTablePail * sitable_getpail(SiTable * self, uint32_t index) {
  return (SiTablePail *) 
      simem_getelement(self->mem, index, sizeof(SiTablePail));
}

/** Empties all entries in the table. */
SiTable * sitable_clear(SiTable * self) {
  int index, size;
  size = sitable_room(self);
  for(index = 0; index < size; index++) {
    void * data = sitable_getpail(self, index);
    sitablepail_initempty(data);
  }
  return self;
}

SiTable * sitable_initroom(SiTable * self, SiTableClass * klass, uint32_t room)
{
  if(!self) return NULL;
  self->klass   = klass ? klass : (&sitable_defaultclass);
  self->main    = si_max_uint32(room    , SITABLE_ROOM_DEFAULT);
  self->cellar  = si_max_uint32(room / 8,  SITABLE_CELLAR_DEFAULT);
  // use given room, but ignore it if it is smalle rtha certain defaults.
  self->bindex  = self->cindex = 0;
  self->mem   = NULL;
  self->mem   = simem_newelement(self->main+self->cellar, sizeof(SiTablePail));
  if(!self->mem) return sitable_done(self);
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
  index   = hash % sitable_main(self);
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

/** Removes a value that matches key from a hash table. returns NULL if 
delete failed, or erased value. */
void * sitable_drop(SiTable * self, void * key) {
  uint32_t hash, index;
  SiTablePail * pail;
  if(!self) return NULL;
  hash    = sitable_hash(self, key);
  index   = hash % sitable_main(self);
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
  // double the size of the mem block...
  SiMem * aid = simem_realloc(self->mem, sitable_room(self) * 2 * sizeof(SiTablePail));
  if(!aid) return NULL;
  return self;
}


/** Stores a value in the hash table. */
void * sitable_set(SiTable * self, void * key, void * value) {
  uint32_t hash, index;
  SiTablePail * pail, * nextpail = NULL;
  if(!self) return NULL;
  hash    = sitable_hash(self, key);
  index   = hash % sitable_main(self);
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
    if(self->cindex >= self->cellar) {  // the cellar is full.
      if(!sitable_grow(self)) return NULL; 
     // grow the hashtable, return null if that failed. 
    }

    nextpail = sitable_getpail(self, self->main + self->cindex);
    // XXX: should handle this!
    if(!nextpail) return NULL;
    pail->next = nextpail;
    pail = nextpail;
    self->cindex++; // next cellar position.
  } 
  self->bindex ++; // one more bucket in use. 
  return sitablepail_init(pail, key, value, NULL, hash);
}







