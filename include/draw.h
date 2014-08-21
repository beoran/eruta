#ifndef draw_H_INCLUDED
#define draw_H_INCLUDED

#include "eruta.h"

#include "image.h"

/* This file was generated with:
'cfunctions -c -aoff -n -w draw_proto src/draw.c' */
#ifndef CFH_DRAW_PROTO
#define CFH_DRAW_PROTO

/* From 'src/draw.c': */

void image_blitscale9 (Image * img , int xx , int yy , int new_w , int new_h , int corner_w , int corner_h );

void draw_slab (int x , int y , int w , int h , Color col );

void draw_roundslab (int x , int y , int w , int h , int rx , int ry , Color col );

void draw_box (int x , int y , int w , int h , Color col , int tt );

void draw_roundbox (int x , int y , int w , int h , int rx , int ry , Color col , int tt );

void draw_frame (int xx , int yy , int ww , int hh , int tt , Color fg , Color bg );

void draw_roundframe (int xx , int yy , int ww , int hh , int tt , Color fg , Color bg );

bool draw_convert_average_to_alpha(ALLEGRO_BITMAP *bitmap, ALLEGRO_COLOR mask_color);


void draw_multi_line_text(ALLEGRO_FONT * font, ALLEGRO_COLOR color, 
                          float x, float y, float w, float h, 
                          int flags,
                          char * text);


typedef struct DrawScrollingText_ {
  char * text;
  float x, y, w, h;
  int line_start, line_now, line_stop, line_pos, paused, line_scroll;
  float line_height;
  int flags;
  ALLEGRO_FONT * font;
  ALLEGRO_COLOR color;
} DrawScrollingText;


void draw_scrolling_text(DrawScrollingText * scrolling_text);


#endif /* CFH_DRAW_PROTO */


#endif




