#ifndef hatab_H_INCLUDED
#define hatab_H_INCLUDED

#include "mem.h"

// Hash function for the hash table. 
typedef int (HatabHash)(void * key);
// Comparison function for the hash table 
typedef int (HatabCompare)(void * one, void * two);


struct HatabActs_;
typedef struct HatabActs_ HatabActs;



// function pointers that determine the hash table's functioning, 
// especially how it compares and hashes keys
struct HatabActs_ {
  HatabCompare  * compare;
  HatabHash     * hash;
  MemDestructor * free_value;
  MemDestructor * free_key;
};

typedef struct Hatab_ Hatab;

#include <stdint.h> 
// need stdint for uint32_t

uint32_t hatab_jenkins (char *key , size_t len );
uint32_t hatab_hash_cstr (void * key );
uint32_t hatab_hash_uint32 (void * key );

int hatab_cellarfull_p (Hatab * self );
int hatab_pailsfull_p (Hatab * self );
Hatab * hatab_done (Hatab * self );
Hatab * hatab_free (Hatab * self );
Hatab * hatab_clear (Hatab * self );
Hatab * hatab_initroom (Hatab * self , HatabActs * acts , int pails , int cellars );
Hatab * hatab_alloc (void);
Hatab * hatab_init (Hatab * self , HatabActs * acts );
Hatab * hatab_newroom (HatabActs * acts , int pails , int cellars );
Hatab * hatab_new (HatabActs * acts );
uint32_t hatab_hash (Hatab * self , void * ptr );
int hatab_compare (Hatab * self , void * pa , void * pb );
void * hatab_get (Hatab * self , void * key );
void * hatab_drop (Hatab * self , void * key );
Hatab * hatab_grow (Hatab * self );
void * hatab_put (Hatab * self , void * key , void * value );



#endif
