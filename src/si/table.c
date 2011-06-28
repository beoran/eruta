
#include "si_mem.h"
#include "si_table.h"
#include "si_block.h"
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


uint32_t sitable_hash_cstr(void * key) {
  char * cstr = (char *) key;
  return sihash_jenkins(cstr, strlen(cstr));
}

uint32_t sitable_hash_uint32(void * key) {  
  return sihash_jenkins((char *)&key, sizeof(uint32_t));
}


typedef int SiTableKeyComparer(void * key1, void * key2);
typedef uint32_t SiTableKeyHasher(void * key);

struct SiTableClass_ {
  SiTableKeyComparer * compare; 
  SiTableKeyHasher   * hash;
};

typedef struct SiTableClass_ SiTableClass; 

struct SiHashBucket_;
typedef struct SiHashBucket_ SiHashBucket;

/** Hash bucket */
struct SiHashBucket_ {
  void          * key;
  void          * value;
  uint32_t        hash;
  SiHashBucket  * next;
};


/** The algorithm implemented is a coalesced hash table. */
struct SiTable_  {
  SiBlock      * block;
  uint32_t       room;
  uint32_t       cellar;  
  uint32_t       bindex; // Bucket index
  uint32_t       cindex; // Cellar index
  SiTableClass * klass;
};


static SiTableClass sitable_defaultclass = { 
        sitable_hash_cstr, 
        (SiTableKeyHasher *) strcmp
};

SiTable * sitable_done(SiTable * self) {
  siblock_free(self->block);
  self->block = NULL;
  self->room  = self->cellar = self->bindex = self->cindex = 0;
  return self;
}

SiTable * sitable_free(SiTable * self) {
  siblock_free(self->block);
  return si_free(self);
}

SiTable * sitable_initr(SiTable * self, SiTableClass * klass, uint32_t room) {
  if(!self) return NULL;
  self->klass   = klass ? klass : (&sitable_defaultclass);
  self->room    = room ? room  : SITABLE_ROOM_DEFAULT;
  self->cellar  = room / 8;
  if(self->cellar < SITABLE_CELLAR_DEFAULT)
  self->cellar = SITABLE_CELLAR_DEFAULT;

  self->bindex  = self->cindex = 0;
  self->block   = NULL;
  self->block   = siblock_new(self->room + self->cellar,sizeof(SiHashBucket));
  if(!self->block) return sitable_done(self);
  return self;
}


SiTable * sitable_alloc() {
  return SI_ALLOCATE(SiTable);
}

SiTable * sitable_init(SiTable * self, SiTableClass * klass) {
  return sitable_initr(self, 128, klass);
}

SiTable * sitable_newr(SiTableClass * klass, uint32_t room) {
  SiTable * aid;
  SiTable * self = sitable_alloc();
  if(!self) return NULL;
  aid = sitable_initr(self, klass, room);
  if(!aid) return sitable_free(self);
  return self;
}

SiTable * sitable_new(SiTableClass * klass) {
  
}


 






