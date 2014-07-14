
#include "utf8.h"
#include <stdlib.h>

/** Own utf-8 handling */
enum {
  UTF8_LEN_1 = 1 << 7,
  UTF8_BITS1 = 1 << 7, 
  UTF8_BITS2 = UTF8_BITS1 + (1 << 6)  + (1 << 5),
  UTF8_BITS3 = UTF8_BITS2 + (1 << 4),
  UTF8_BITS4 = UTF8_BITS3 + (1 << 3),
  UTF8_BITS5 = UTF8_BITS4 + (1 << 2),
  UTF8_BITS6 = UTF8_BITS5 + (1 << 1),
  UTF8_BITSX = (1 << 7)   + (1 << 6)
};

#define E -1 /* Shorthand for "error" */
static char utf8_length_table[256] = {
  0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
  1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
  1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
  1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
  1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
  1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
  1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
  1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
  E,E,E,E,E,E,E,E,E,E,E,E,E,E,E,E,
  E,E,E,E,E,E,E,E,E,E,E,E,E,E,E,E,
  E,E,E,E,E,E,E,E,E,E,E,E,E,E,E,E,
  E,E,E,E,E,E,E,E,E,E,E,E,E,E,E,E,
  2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,
  2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,
  3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,
  4,4,4,4,4,4,4,4,5,5,5,5,6,6,E,E
};
#undef E


/* Returns the length of the utf8 character that starts at 
 * str, or negative if str doesn ot point to the beginning byte of 
 * of a valid utf8 character.
 */
int utf8_decode_length(char * str) {  
  int index = (int)(*((unsigned char *) str));
  return utf8_length_table[index];
}

static long utf8_start_masks[7] = {  
  -1, UTF8_BITS1, UTF8_BITS2, UTF8_BITS3, 
  UTF8_BITS4, UTF8_BITS5, UTF8_BITS6,
};


/** Decodes a single utf8 character. */
int utf8_decode_one(char * str, long * result) {
  int index;
  unsigned char * ustr = (unsigned char *) str;
  if (!result)   return -1;  
  (*result)      = 0; /* Ensure result is always set. */
  int len        = utf8_decode_length(str);  
  if (len < 1)   return len;  
  if (len >= 7)  return len; 
  /* Get the unicode character bits from the first byte by masking off
   the length part*/
  long start = ((long) *ustr) & (~utf8_start_masks[len]);
  /** now keep shifting in the remaining bits from the other bytes. */
  for (index = 1; index < len; index ++) {    
    start    = (start << 6) + (ustr[index] & (~UTF8_BITSX));
  }
  (*result) = start;
  return len;
}

/** Length of the whole UTF-8 string in unicode characters. 
 * Returns a negative value on encoding error.
 */
int utf8_length(char * str) {
  int result = 0;
  int index  = 0;
  int aid;
  do  {
    aid      = utf8_decode_length(str + index);    
    if (aid < 0) { return -1; }
    index   += aid;
    result ++;
  } while (aid > 0);
  if (aid < 0) { return -1; }
  return result;  
}


/** Changes the pointer next to point to the next character 
 * in the utf-encoded string str and decodes the character 
 * in result. Returns the length in bytes of the character decoded, or less 
 * than 1 if at the end of the string. In that case, next is also set to NULL;
 */
int utf8_next(char * str, char ** next, long * result) {
   int length = utf8_decode_length(str);
   if (length < 1) { 
      (*next) = NULL;
      return length;
   }
   (*next) = str + length;
   return length;
}


/** Encodes a single utf8 character. 
 * Result must have at least 6 bytes of data available for storage. 
 * No \0 terminator is generated.
 * Returns the length of the encoded character.
 */
int utf8_encode_one(long character, char * result) {
  (void) result;
  (void) character;
   /*XXX: to do. */
  return 0;
}







