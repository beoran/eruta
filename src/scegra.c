

#include "scegra.h"
#include "eruta.h"
#include "image.h"
#include "rebox.h"
#include "bad.h"
#include "draw.h"
#include "str.h"

/* A very simple scene graph, mainly for drawing the UI that will be managed from 
 * the scripting side of things. To avoid memory allocation, limit the amount of 
 * vertices allowed to SCERGRA_VERTEX_MAX (32)
 */

struct ScegraRound_ {
  float cx, cy, rx, ry, start_theta, delta_theta;
};

struct ScegraBitmap_ {
  float sx, sy, sw, sh;
  float cx, cy, dx, dy, xscale, yscale, angle;
  int flags;
};

struct ScegraBox_ {
  float x1, y1, x2, y2, rx, ry; 
};

struct ScegraLine_ {
  float x1, y1, x2, y2;
}; 

struct ScegraTriangle_ {
  float x1, y1, x2, y2, x3, y3;
}; 

struct ScegraSpline_ {
  float vertices[8];
}; 

struct ScegraPixel_ {
  float x1, y1;
}; 

struct ScegraPolygon_ { 
  float vertices[SCEGRA_VERTEX_MAX]; 
  int   vertex_count;
};

struct ScegraPrim_ { 
  ALLEGRO_VERTEX vertices[SCEGRA_VERTEX_MAX];
  int vertex_count;
};


struct ScegraText_ {
  float x1, x2, y, diff;
  int flags;
  char *text;
  int text_owned, from, until;
};



/** Union: ScegraData. 
 * A union of different types of vertex data for drawing the various primitives
 * and bitmaps.
 */
union ScegraData_ {
  struct ScegraPixel_           pixel;
  struct ScegraLine_            line;
  struct ScegraTriangle_        triangle;
  struct ScegraBox_             box;
  struct ScegraBitmap_          bitmap;
  struct ScegraPolygon_         polygon;
  struct ScegraRound_           round;
  struct ScegraSpline_          spline;
  struct ScegraPrim_            prim;
  struct ScegraText_            text;
};


/* Struct: ScegraStyle 
* Describes the style of a Scene graph node.
*/
struct ScegraStyle_ {
  ALLEGRO_COLOR         color;
  int                   thickness;
  ALLEGRO_BITMAP      * bitmap;
  ALLEGRO_FONT        * font;
  int                   bitmap_owned;
  int                   font_owned;
  ALLEGRO_LINE_JOIN     join_style; 
  float                 miter_limit;
};


struct ScegraNode_ {
  int                   id;
  int                   z;
  int                   step;
  int                   kind;
  struct ScegraStyle_   style;
  union   ScegraData_   data;
  ScegraDraw          * draw;
  ScegraUpdate        * update;
};


/* Scene graph struct. */
struct Scegra_ {
  ScegraNode * nodes;
  int          node_max;
};


ScegraNode * 
scegranode_initall(ScegraNode * node, int id, ScegraStyle style, 
                   ScegraDraw * draw, ScegraUpdate * update) {
  if(!node) return NULL;
  node->id      = id;
  node->z       = id;
  node->style   = style;
  node->draw    = draw;
  node->update  = update;
  node->step    = 0;
  return node;
}

ScegraNode * 
scegranode_done(ScegraNode * self) {
  if (!self) return NULL;
  self->id = -1;
  self->z  = -1;
  return self;
}




Scegra * 
scegra_init(Scegra * self) {
  if (!self) return NULL;
  self->nodes = NULL;
  return self;
}

/*
void scegra_drawframe(ScegraNode * self) {
  draw_frame(self->box.at.x, self->box.at.y,
             self->box.size.x, self->box.size.y, 
             self->style.bordersize, self->style.forecolor, 
             self->style.backcolor);
}



void scegra_drawroundframe(ScegraNode * self) {
  draw_roundframe(self->box.at.x  , self->box.at.y,
                  self->box.size.x, self->box.size.y, 
                  self->style.bordersize, self->style.forecolor, 
                  self->style.backcolor);
}
*/


/* 
 * ALLEGRO drawing functions: 
 * 
al_draw_arc (3)      - Allegro 5 API
al_draw_bitmap (3)   - Allegro 5 API
al_draw_bitmap_region (3) - Allegro 5 API
al_draw_circle (3)   - Allegro 5 API
al_draw_ellipse (3)  - Allegro 5 API
al_draw_elliptical_arc (3) - Allegro 5 API
al_draw_filled_circle (3) - Allegro 5 API
al_draw_filled_ellipse (3) - Allegro 5 API
al_draw_filled_pieslice (3) - Allegro 5 API
al_draw_filled_polygon (3) - Allegro 5 API
al_draw_filled_polygon_with_holes (3) - Allegro 5 API
al_draw_filled_rectangle (3) - Allegro 5 API
al_draw_filled_rounded_rectangle (3) - Allegro 5 API
al_draw_filled_triangle (3) - Allegro 5 API
al_draw_indexed_prim (3) - Allegro 5 API
al_draw_justified_text (3) - Allegro 5 API
al_draw_justified_textf (3) - Allegro 5 API
al_draw_justified_ustr (3) - Allegro 5 API
q (3)     - Allegro 5 API
al_draw_pieslice (3) - Allegro 5 API
al_draw_pixel (3)    - Allegro 5 API
al_draw_polygon (3)  - Allegro 5 API
al_draw_polygon_with_holes (3) - Allegro 5 API
al_draw_polyline (3) - Allegro 5 API
al_draw_polyline_ex (3) - Allegro 5 API
al_draw_prim (3)     - Allegro 5 API
al_draw_rectangle (3) - Allegro 5 API
al_draw_ribbon (3)   - Allegro 5 API
al_draw_rotated_bitmap (3) - Allegro 5 API
al_draw_rounded_rectangle (3) - Allegro 5 API
al_draw_scaled_bitmap (3) - Allegro 5 API
al_draw_scaled_rotated_bitmap (3) - Allegro 5 API
al_draw_soft_line (3) - Allegro 5 API
al_draw_soft_triangle (3) - Allegro 5 API
al_draw_spline (3)   - Allegro 5 API
al_draw_text (3)     - Allegro 5 API
al_draw_textf (3)    - Allegro 5 API
al_draw_tinted_bitmap (3) - Allegro 5 API
al_draw_tinted_bitmap_region (3) - Allegro 5 API
al_draw_tinted_rotated_bitmap (3) - Allegro 5 API
al_draw_tinted_scaled_bitmap (3) - Allegro 5 API
al_draw_tinted_scaled_rotated_bitmap (3) - Allegro 5 API
al_draw_tinted_scaled_rotated_bitm... (3) - Allegro 5 API
al_draw_triangle (3) - Allegro 5 API
al_draw_ustr (3)     - Allegro 5 API
al_draw_vertex_buffer (3) - Allegro 5 API


So the most important drawing functions I'll implement and
group together are: 
* Circle / arc drawing and Ellipse / elliptical arc drawing.
* Rectangle drawing and Rounded rectangle drawing.
* Triangle drawing.
* Polygon drawing.
* Line drawing.
* Point drawing.
* Spline Drawing.
* Bitmap Drawing.


*/











