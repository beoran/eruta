
#include "si_mem.h"
#include "si_table.h"
#include "si_block.h"
#include <string.h>


/* Jenkins hash. */
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

/** Hash bucket */
struct SiBucket_ {
  void      * key;
  void      * value;
  uint32_t    hash;
};

typedef struct SiBucket_ SiBucket;

/** The algorithm implemented is a Coalesced hash table. */
struct SiTable_  {
  SiBlock     *  block;
  uint32_t       room;
  uint32_t       cellar;
  SiTableClass * klass;
};


static SiTableClass sitable_defaultclass = { sitable_hash_cstr, 
        (SiTableKeyHasher *)strcmp };

SiTable * sitable_done(SiTable * self) {
  siblock_free(self->block);
  self->block = NULL;
  self->room  = self->cellar = 0;
  return self;
}

SiTable * sitable_free(SiTable * self) {
  siblock_free(self->block);
  return si_free(self);
}

SiTable * sitable_initr(SiTable * self, uint32_t room, SiTableClass * klass) {
  if(!self) return NULL;
  self->klass   = klass ? klass : (&sitable_defaultclass);
  self->room    = room  ? room  : 128;
  self->cellar  = room / 8;
  if(self->cellar < 16) self->cellar  = 16;
  self->block   = NULL;
  self->block   = siblock_new(self->room + self->cellar,sizeof(SiBucket));
  if(!self->block) return sitable_done(self);
  return self;
}




 






