
#include <stdlib.h>
#include <string.h>
#include "silut.h"

/* This file contails lookup helper functions that
* look up an integer based on a string or a string based on an integer,
* making use of a lookup table of Silut records.
*/



static Silut test_lut[] = {
  { 1, "one"   },
  { 2, "two"   },
  { 3, "three" },
  { -1, NULL   }
};


/** Lenght of an array of silut. NULL is the guard clause. */
int silut_len(Silut lut[]) {
  int index = 0;
  Silut * aid;
  if(!lut) return -1;
  for(index = 0, aid = lut + index;
      aid && aid->string ;
      index++, aid = lut + index)
  {}
  return index;
}

/** Looks up a string with an integer key integer in the lookup table by
* simple linear search. The look up table needs not be sorted in any way,
* but it's somewhat slower.
* The look up table should have NULL as it's final element.
* If not found, NULL is returned. If found the matching record is returned.
*/
Silut * silut_lsearchi(Silut lut[], int integer) {
  int index = 0;
  Silut * aid;
  if(!lut) return NULL;
  for(index = 0, aid = lut + index;
      aid && aid->string ;
      index++, aid = lut + index)
  {
    if(aid->integer == integer) return aid;
  }
  return NULL;
}

/** Looks up an integer with an integer key integer in the lookup table by
* simple linear search. The look up table needs not be sorted in any way,
* but it's somewhat slower.
* The look up table should have NULL as it's final element.
* strcmp is used to compare the strings.
* If not found, NULL is returned. If found the matching record is returned.
*/
Silut * silut_lsearchcstr(Silut lut[], const char * string) {
  int index = 0;
  Silut * aid;
  if(!lut) return NULL;  
  for(index = 0, aid = lut + index;
      aid && aid->string ;
      index++, aid = lut + index)
  {
    if(!strcmp(aid->string, string)) return aid;
  }
  return NULL;
}


