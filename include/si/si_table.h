#ifndef SI_TABLE_H
#define SI_TABLE_H

#include "si.h"
/** Simple hash table.  */

struct SiTable_;
typedef struct SiTable_ SiTable; 

/** FNV hash function.  */
uint32_t sihash_fnv1a_32(char * buf, size_t size, uint32_t last);










#endif




