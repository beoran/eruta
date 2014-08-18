#ifndef laytext_H_INCLUDED
#define laytext_H_INCLUDED


#include "dynar.h"

/* Type of the callback that should calulate the dimensions of an 
 * arbitrary text string encoded in utf-8. It should only take into consideration up to bytes bytes of that string */
typedef int laytext_callback(char * str, int bytes, void * extra, float * w, float * h);

Dynar * laytext_layout(char * str, float max_width, laytext_callback * callback, void * extra);


/** Laytext lays out texts using shards. A shard is a reference to an individual segment of text that
 * has a single consistent one-line position, style,  color, font and font scaling.
 * The text layout is prepared and rendered through callbacks.
 */
typedef struct LaytextShard_ LaytextShard;

/** Text layout information. */
typedef struct Laytext_ Laytext;

/** Text layout fragment information. */
struct LaytextShard_ {
  struct LaytextShard_ * next;
  struct LaytextShard_ * previous;
  char * text;
  int    bytes;
  float  x; 
  float  y;
  float  r;
  float  g;
  float  b;
  float  a;
  void * font;  
};



/* Type of the callback that should calulate the dimensions of an 
 * arbitrary text string encoded in utf-8. It should only take into consideration up to bytes bytes of that string */
typedef int laytext_size_getter(char * str, int bytes, void * font_data, float * w, float * h);

/* Type of the callback that will be used to draw a shard n arbitrary text string encoded in utf-8 at a given position
*  It should only take into consideration up to bytes bytes of that string.
*  r, g, b, and a are color values ranging from 0.0 to 1.0 
*/
typedef int laytextshard_drawer(LaytextShard * shard);


struct Laytext_ {
  LaytextShard * first;
  LaytextShard * last;
  char * text;
  int   start_line;
  int   end_line;
  int   start_pos;
  int   end_pos;
  int   animation;
  int   lines_per_page;
  int   page;
  float speed;
  laytext_size_getter * get_size;
  laytextshard_drawer * draw;
};



/* Laytext * laytext_new(char * text, int x, int y, int w, int h, void * font_data, */
Laytext * laytext_init(Laytext *  self, char * text, laytext_size_getter * get_size, laytextshard_drawer * draw);

Laytext * laytext_update(Laytext *  self, char * text);

void laytext_done(Laytext *  self);






#endif




