
#include <string.h>

#include "hatab.h"
#include "mem.h"
#include "dynar.h"


#define HATAB_ROOM_DEFAULT    128
#define HATAB_CELLAR_DEFAULT 16

/* TODO: the current hash table structure isn't too good. 
 Empty pails are kept in the hash chains. Better would be to use doubly 
 linked pails, and drop unused pails from the doubly linked list. 
 Also, the cellar can be replaced by a pail storage, that 
 keeps all the pails. The hash table's lookup table then 
 only stores *pointers* to pails in the pail storage. In theory,
 the pail storage could even be external if that was needed. 
 */

/* Allocate a new hash tab pair. */
HatabPair * hatabpair_alloc() {
  return STRUCT_ALLOC(HatabPair);  
}


/* Initialize a hash tab pair. */
HatabPair * hatabpair_init(HatabPair * self, void * key, void * value) {
  if(!self) return NULL;
  self->key     = key;
  self->value   = value;
  return self;
}

/* Make a new hash tab pair. */
HatabPair * hatabpair_new(HatabPair * self, void * key, void * value) {
  return hatabpair_init(hatabpair_alloc(), key, value);  
}


/* Free the pair. Contrary to normal, does not clean up the contents of the pair!. */
HatabPair * hatabpair_free(HatabPair * self) {
  return mem_free(self);
}

/* Apply destructor to pair. */
HatabPair * hatabpair_destroy(HatabPair * self, HatabPairFree * destructor) {
  if (destructor) destructor(self);
  return hatabpair_free(self);
}


/* Get the key of a hash tab pair. */
void * hatabpair_key(HatabPair * self) {
  if(!self) return NULL;
  return self->key;
}

/* Get the value of a hash tab pair. */
void * hatabpair_value(HatabPair * self) {
  if(!self) return NULL;
  return self->value;
}


/* Returns nonzero if the pair is empty, that is, has a NULL key, zero if not . */
int hatabpair_empty_p(HatabPair * self) {
  if(!self) return TRUE;
  if(!self->key) return TRUE;
  return FALSE;
}

/* Jenkins hash function. */
uint32_t hatab_jenkins(char *key, size_t len) {
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
uint32_t hatab_hash_cstr(void * key) {
  char * cstr = (char *) key;
  return hatab_jenkins(cstr, strlen(cstr));
}

/** Hash function that hashes a uint32_t. */
uint32_t hatab_hash_uint32(void * key) {  
  return hatab_jenkins((char *)&key, sizeof(uint32_t));
}


/* A Pail is a hash bucket. This is internal to Hatab and should not be used 
outside of this file. */

struct Pail_;
typedef struct Pail_ Pail;

/* Hash bucket, contains a pair, the hash value, and  and a linked list to the next pail. */
struct Pail_ {
  HatabPair       pair;
  Pail          * next;
  Pail          * prev;
  uint32_t        hash;
};



/** A hash table. The algorithm implemented is a coalesced hash table. */
struct Hatab_  {
  Dynar      * lookup ;
  Dynar      * pails  ;
  int          lookup_used;
  int          pails_used;
  /* Acts of the hash table, for customization of the hash function, etc.  */
  HatabActs  * acts   ;
};

/* Initializes a hash table bucket, here abbreviated to "pail". */
static Pail * pail_init(Pail * self, 
                        void * key, void * data, void * next, 
                        void * prev, uint32_t hash) {
  hatabpair_init(&self->pair, key, data);
  self->next  = next;
  self->prev  = prev;
  self->hash  = hash;
  return self;
}

/* Initializes a hash table bucket, here abbreviated to "pail". */
static Pail * pail_initpair(Pail * self, HatabPair pair, 
                            void * next, void * prev, uint32_t hash) {
  self->pair  = pair;
  self->next  = next;
  self->prev  = prev;
  self->hash  = hash;
  return self;
}

/* Returns the next pail following this one.  */
static Pail * pail_next(Pail * self) {
  if(!self) return NULL;
  return self->next;
}

/* Returns the previous pail preceding this one.  */
static Pail * pail_prev(Pail * self) {
  if(!self) return NULL;
  return self->next;
}


/* Sets the next pail following this one. Also updates next->prev if needed.  */
static Pail * pail_next_(Pail * self, Pail * next) {
  if(!self) return NULL;
  self->next = next;
  if(next) {
    next->prev = self;
  }
  return next;
}

/* Sets the next pail preceding this one */
static Pail * pail_prev_(Pail * self, Pail * prev) {
  if(!self) return NULL;
  self->prev = prev;
  if(prev) {
    prev->next = self;
  }  
  return prev;
}


/* Gets the key of the pail. */
static void * pail_key(Pail * self) {
  return hatabpair_key(&self->pair);  
}

/* Gets the value of the pail. */
static void * pail_value(Pail * self) {
  return hatabpair_value(&self->pair);  
}


/* Initializes a hash bucket to be empty. */
static Pail * pail_initempty(Pail * self) {
  return pail_init(self, NULL, NULL, NULL, NULL, 0);
}

/* Returns zero if the hash of the bucket is equal to hash, nonzero if not. */
static int pail_cmphash(Pail * self, uint32_t hash) {
  return self->hash != hash;
}

/* Returns nonzero if the pail is empty, zero if not . */
static int pail_empty_p(Pail * self) {
  return hatabpair_empty_p(&self->pair);
}

/* Empties a pail, without breaking the link chain */
static Pail * pail_emptynobreak(Pail * self) {
  return pail_init(self, NULL, NULL, self->next, self->prev, 0);
}

/* Unlinks the pail, that is, removes itself from the pail chain it is in. 
 Returns the next pail in the chain, if any. Then empties the pail,
 so make suure to destroy the contents before calling thsi function. */
static Pail * pail_unlink(Pail * self) {
  /* Link next and prev together. */
  pail_next_(self->prev, self->next); 
  pail_prev_(self->next, self->prev);
  pail_initempty(self);
  return self->next;
}


/*  If the pail is in use, free the memory held by the key 
by using the HatabPairFree destructor in the acts struct. 
If the destructor is NULL, nothing happens. 
Does not break the link chain. Does nothing if the pail is already empty. */
static Pail * pail_done(Pail * self, HatabActs * acts) {
  if(pail_empty_p(self)) return self;
  if(acts) {
    if(acts->free_pair) acts->free_pair(&self->pair);
  }
  return pail_unlink(self);
}


/* Default operations for the hash table. */
static HatabActs hatab_default_acts = {
  (HatabCompare *)    strcmp,
  (HatabHash    *)    hatab_hash_cstr,
  NULL,
};


/** Returns nonzero if the pails is full, zero if it isn't. */
int hatab_pailsfull_p(Hatab * self) {
  if(!self) return 0;
  return self->pails_used >= dynar_size(self->pails);
}

/** Returns nonzero if the lookup array is full, zero if it isn't. */
int hatab_lookupfull_p(Hatab * self) {
  if(!self) return 0;
  return self->lookup_used >= dynar_size(self->lookup);
}

/** Cleans up and empties a table. */
Hatab * hatab_done(Hatab * self) {
  int index;
  if(!self) return NULL;
  dynar_free(self->lookup);
  dynar_free(self->pails);
  self->lookup       = self->pails      = NULL;
  self->lookup_used  = self->pails_used = 0;
  return self;
}

/** Frees a hash table */
Hatab * hatab_free(Hatab * self) {
  hatab_done(self);
  return mem_free(self);
}


/* Gets a pointer to the lookup pail at the given index,
which is not checked and should be valid. */
static Pail * hatab_getlookuppail(Hatab * self, uint32_t index) {
  return (Pail *) dynar_getptr(self->lookup, index);
}

/* Gets a pointer to the pails pail at the given index,
which is not checked and should be valid. */
static Pail * hatab_getpailspail(Hatab * self, uint32_t index) {
  return (Pail *) dynar_getdata(self->pails, index);
}

/* Returns the next empty pail cell, or NULL if the pails storage is full. */
static Pail * hatab_getnextemptypail(Hatab * self) {  
  int index, stop;
  stop = dynar_size(self->pails);
  for (index = 0; index < stop; index ++) {
    Pail * pail = hatab_getpailspail(self, index);
    if (pail_empty_p(pail)) { 
      self->pails_used++;
      return pail;
    }
  }
  return NULL;
}

/* Empties all entries in the table. */
Hatab * hatab_clear(Hatab * self) {
  int index, size;
  size      = dynar_size(self->lookup);
  /* Empty lookup table. */
  for(index = 0; index < size; index++) {
    dynar_putptr(self->lookup, index, NULL);
  }
  /* Clean up and free pails. */
  size      = dynar_size(self->pails);
  for(index = 0; index < size; index++) {
    void * data = hatab_getpailspail(self, index);
    pail_done(data, self->acts);
  }
  return self;
}

/* Initializes the dynars and pails of the table. */
Hatab * hatab_initpails(Hatab * self) {
  int index, size;
  size      = dynar_size(self->lookup);
  /* Empty lookup table. */
  for(index = 0; index < size; index++) {
    dynar_putptr(self->lookup, index, NULL);
  }
  /* Clean up and free pails. */
  size      = dynar_size(self->pails);
  for(index = 0; index < size; index++) {
    void * data = hatab_getpailspail(self, index);
    pail_initempty(data);
  }
  return self;
}


/** Initializes the table with the given room and pails space. */
Hatab * hatab_initroom(Hatab * self, HatabActs * acts, 
                        int pails, int space)
{
  if(!self)     return NULL;
  self->acts    = acts ? acts : (&hatab_default_acts);
  self->lookup  = dynar_new(pails , sizeof(Pail *));
  self->pails   = dynar_new(space, sizeof(Pail));
  if((!self->lookup) || (!self->pails)) return hatab_done(self);
  hatab_initpails(self);
  return self;
}

/** Allocates a Hatab. */
Hatab * hatab_alloc() {
  return STRUCT_ALLOC(Hatab);
}

/** Initializes the hatab with default room and pails space */
Hatab * hatab_init(Hatab * self, HatabActs * acts) {
  return hatab_initroom(self, acts, HATAB_ROOM_DEFAULT, HATAB_ROOM_DEFAULT * 2);
}

/** Makes a new hatab */
Hatab * hatab_newroom(HatabActs * acts, int pails, int space) {
  return hatab_initroom(hatab_alloc(), acts, pails, space);
}


/** Makes a new hatab with default room and pails space */
Hatab * hatab_new(HatabActs * acts) {
  return hatab_newroom(acts, HATAB_ROOM_DEFAULT, HATAB_ROOM_DEFAULT * 2);
}

/* Calculates the hash value of the given data pointer using self->acts->hash*/
uint32_t hatab_hash(Hatab * self, void * ptr) {
  return self->acts->hash(ptr);
}

/* Compares the given data pointers using self->acts->compare */
int hatab_compare(Hatab * self, void * pa, void * pb) {
  return self->acts->compare(pa, pb);
}

/* Checks if the given bucket contains the given key with the given hash. */
static int hatab_pailok(Hatab * self, Pail * pail, 
                   void * key, uint32_t hash) {
  if(!pail) return FALSE;
  if(pail_empty_p(pail))       return FALSE;
  if(pail_cmphash(pail, hash)) return FALSE;
  if(hatab_compare(self, pail_key(pail), key)) return FALSE;
  return TRUE;
}

/* Gets the pail that matches this key, or NULL if not found.  */
static Pail * hatab_findpail(Hatab * self, void * key) {
  uint32_t hash, index;
  Pail * pail;
  if(!self) return NULL;
  hash    = hatab_hash(self, key);
  index   = hash % dynar_size(self->lookup);
  pail    = hatab_getlookuppail(self, index);
  if (!pail)  return NULL; 
  while(pail) {
    if(hatab_pailok(self, pail, key, hash)) return pail;
    // return the pail if it's OK. 
    pail = pail_next(pail); // Follow the linked chain of pails.
  }
  // If we get here, no more links, and not found. Return null.
  return NULL;
}


/** Gets a value that matches key from a hash table. */
void * hatab_get(Hatab * self, void * key) {
  Pail * pail = hatab_findpail(self, key);
  if(!pail) return NULL;
  return pail_value(pail);
}

/** Removes a value that matches key from a hash table. 
 *  The value will also will be deleted if the methods are set correctly. */
void * hatab_drop(Hatab * self, void * key) {
  void * data;
  Pail * pail = hatab_findpail(self, key);
  if(!pail) return NULL;
  /* XXX: this can't work now... */
  pail_done(pail, self->acts); 
  return self;
}

/** Grows the hash table when needed. */
Hatab * hatab_grow(Hatab * self) {
  // double the size of the pails block, so collisions can be handled 
  // NOTE: should also grow lookup table but rehash is slow...
  void * mok = dynar_grow(self->pails, (dynar_size(self->pails))*2);
  /*void * cok = siarray_grow(hatab_pails(self), hatab_pailsroom(self)*2);*/
  if(!mok) return NULL;
  // if(!cok) return NULL; // XXX: rehash the table here!  
  return self;
}


/** Stores a value in the hash table. */
void * hatab_put(Hatab * self, void * key, void * value) {
  uint32_t hash, index;
  Pail * pail, * newpail = NULL;
  if(!self) return NULL;
  hash    = hatab_hash(self, key);
  index   = hash % dynar_size(self->lookup);
  pail    = hatab_getlookuppail(self, index);
  /* get a new pail */
  newpail = hatab_getnextemptypail(self);
  if(!newpail) { 
    if(hatab_pailsfull_p(self)) {  // The pails is full.
      if(!hatab_grow(self)) return NULL; 
      // Grow the hashtable, return null if that failed. 
    }
    newpail = hatab_getnextemptypail(self);
    if (!newpail) return NULL;
  }

  
  /* No pail set yet, set it in the array. */  
  if (!pail)  { 
    dynar_putptr(self->lookup, index, newpail);
  } else { 
    /* Pail already there. Go to end of linked list and add pail there. */
    while (pail->next)  {
      pail = pail->next;
      // Follow chain of linked, nonempty pails. If the pail is empty or
      // the chain is broken, we can store the data in the pail.
    }
    // XXX
    pail->next          = newpail;
    newpail->prev       = pail;
  }
  newpail->hash         = hash;
  hatabpair_init(&newpail->pair, key, value);
  return newpail; 
}


/* Prints a dot graph of the hash tab for debugging */
void hatab_printgraph(Hatab * self) {
  int index, stop, oldindex;
  Pail * pail, * oldpail, * nextpail,  *linkpail;
  pail = NULL; oldpail = NULL;
  oldindex = -1;
  printf("digraph { \n");
  stop = dynar_size(self->lookup);
  for(index = 0; index < stop; index ++) {
    pail = (Pail *) dynar_getptr(self->lookup, index);
    if (!pail) { 
      continue;
    }
    if (oldpail) {
       printf("t%p -> t%p [color=red];\n", oldpail, pail);
    }
    linkpail = pail ; nextpail = pail_next(linkpail);
    while (nextpail) {
      printf("t%p -> t%p [color=green];\n", linkpail, nextpail);
      linkpail = nextpail;
      nextpail = pail_next(linkpail); 
    } 
    oldpail = pail;  
  } 
  printf("} \n");
}












