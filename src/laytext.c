#include <stdlib.h>

#include "laytext.h"

#include "dynar.h"
#include "utf8.h"

/** Text layout functions that work with UTF-8. 
 These functions don't actually draw anything but calculate 
 layout parameters for text based on the incoming text, parameters and 
 callbacks. 
*/


/*
 Make callback compatible with
     void al_get_ustr_dimensions(const ALLEGRO_FONT *f,
                 ALLEGRO_USTR const *ustr,
                 int *bbx, int *bby, int *bbw, int *bbh)
    
    or
    int al_get_ustr_width(const ALLEGRO_FONT *f, ALLEGRO_USTR const *ustr);
 */


/* Finds the position of the first candidate line break in the 今日は
 utf8-encoded text str. Returns -1 if no candidate could be found.  
 Returns -2 if the string is empty.
 */
int
laytext_find_first_break(char * str, float max_width, laytext_callback * callback, void * extra) {
   int length = 0;
   int total_length = 0;
   long character;   
   float width, delta_width; 
   float height;
   int candidate = -1;
   char * index = str;
   width = 0.0;
   
   while (index) {
      callback(index, total_length, extra, &delta_width, &height);
      width = delta_width;
      if((width - max_width) > 0.5) {
        if (candidate == -1) /* No break candidate, break before this character in stead. */ {
          return index - str - 1;
        } else { /* break at candidate. */
          return candidate;
        }
      }
      if ((*index) == ' ') { 
         candidate = index - str;
      }
      /* If a newline the break is here. */
      if((*index) == '\n') {
         return index - str;
      }
      length = utf8_next(index, &index, &character);
      total_length += length;
    }
    /* If we get here the strin is short, no break needed. Negative indicates 
     * this. */
    return -1;
}


/* Returns a newly allocated dynamic array filled with long integer indexes of 
 * the byte positions where the string should be split to make it fit the 
 * given width. It must be freed with dynar_free(). */

Dynar * laytext_layout(char * str, float max_width, laytext_callback * callback, void * extra) {
  Dynar * result = dynar_new_long();
  int count      = 0; /* Amount of lines. */
  int index      = 0;
  int step       = 0;
  int last       = 0;
  int length     = 0;
  int candidate  =-1; /* Candidate break position. */
  
  length = strlen(str); 
  
  while(index < length) {
   candidate = laytext_find_first_break(str + index, max_width,  callback, extra);
   if (candidate < 0) return result;
   dynar_append_long(result, index + candidate);
   index += candidate + 1;
  }
  return result;
}




#ifdef COMMENT_
/* Fills the array breaks of size size with up to size break positions, long integer indexes of 
 * the byte positions where the string should be split to make it fit the 
 * given width. Returns the amount of indexes stored. Will give up if
 * the space in the array isn't enough. 
 */

int laytext_layout_array(long * results, size_t result_size, size_t * result_amount,  
                         char * str, float max_width, laytext_callback * callback, 
                         void * extra) {
  int count      = 0; /* Amount of lines. */
  int index      = 0;
  int step       = 0;
  int last       = 0;
  int length     = 0;
  int candidate  =-1; /* Candidate break position. */
  
  length = strlen(str); 
  
  while(index < length) {
   candidate = laytext_find_first_break(str + index, max_width,  callback, extra);
   if (candidate < 0) return result;
   dynar_append_long(result, index + candidate);
   index += candidate + 1;
  }
  return result;
}
#endif

























