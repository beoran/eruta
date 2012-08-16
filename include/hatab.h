#ifndef hatab_H_INCLUDED
#define hatab_H_INCLUDED

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
};

#include <stdint.h> 
// need stdint for uint32_t

#include "hatab_proto.h"


#endif




