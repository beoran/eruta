#ifndef scegra_H_INCLUDED
#define scegra_H_INCLUDED


#include "eruta.h"


typedef struct Scegra_          Scegra;
typedef struct ScegraNode_      ScegraNode;
typedef struct ScegraStyle_     ScegraStyle;
typedef struct ScegraPoints_    ScegraPoints;


typedef void ScegraDraw  (ScegraNode * node);
typedef void ScegraUpdate(ScegraNode * node, double dt);
typedef void ScegraDone  (ScegraNode * node);


#define SCEGRA_VERTEX_MAX 32


/* A very simple scene graph, mainly for drawing the UI that will be managed from 
 * the scripting side of things. Only one "global" scene graph is supported.
 */

void scegra_init();
void scegra_done();
void scegra_update(double dt);
void scegra_draw();

/* Struct: ScegraStyle 
* Describes the style of a Scene graph node.
*/
struct ScegraStyle_ {
  ALLEGRO_COLOR         color;
  ALLEGRO_COLOR         background_color;  
  ALLEGRO_COLOR         border_color;
  /* This is a store_xxx id of the background bitmap to show or negative if none.  */
  int                   background_image_id;  
  /* Border thickness, 0 or negative if none. */
  float                 border_thickness;
  /* This is a store_xxx id of the font to use or negative if none.  */
  int                   font_id;
  ALLEGRO_LINE_JOIN     line_join_style; 
  float                 line_miter_limit;
  /* Flags to use when drawing text. */
  int                   text_flags;
  /* Flags to use when drawing images. */
  int                   image_flags;
  /* Margin between box and contents.  */
  float                 margin;
};


ScegraStyle * scegrastyle_initempty(ScegraStyle * self);

int scegra_make_box(int id, BeVec pos, BeVec siz, BeVec round, ScegraStyle style);


int scegra_z(int index);
int scegra_z_(int index, int z);

int scegra_make_text(int id, BeVec pos, BeVec siz, const char * text, ScegraStyle style);
int scegra_make_longtext(int id, BeVec pos, BeVec siz, char * text, ScegraStyle style);
int scegra_make_image(int id, BeVec pos, BeVec siz,int image_id, ScegraStyle style);

int scegra_visible_(int index, int is_visible);
int scegra_image_id_(int index, int rindex);
int scegra_font_id_(int index, int rindex);
int scegra_background_image_id_(int index,int rindex);
int scegra_speed_(int index, float x, float y);
int scegra_size_(int index, float w, float h);
int scegra_position_(int index, float x, float y);
int scegra_border_thickness_(int index, float t);
int scegra_background_color_(int index, int r, int g, int b, int a);
int scegra_border_color_(int index, int r, int g, int b, int a);
int scegra_color_(int index, int r, int g, int b, int a);
int scegra_z_(int index, int z);
int scegra_margin_(int index, float m);

int scegra_speed(int index, float * x, float * y);
int scegra_size(int index, float * w, float * h);
int scegra_position(int index, float *x, float * y);
int scegra_border_thickness(int index, float * t);
int scegra_background_color(int index, int * r, int * g,int * b,int * a);
int scegra_border_color(int index,int * r,int * g,int * b, int * a);
int scegra_color(int index, int * r, int * g, int * b,int * a);
int scegra_image_bitmap_size(int index, int * w, int * h);
int scegra_margin(int index, float * m);

int scegra_make_image(int id, BeVec pos, BeVec siz, int image_id, ScegraStyle style);
int scegra_make_text(int id, BeVec pos, BeVec siz, const char *text, ScegraStyle style);
int scegra_make_box(int id, BeVec pos, BeVec siz, BeVec round, ScegraStyle style);
int scegra_disable_node(int index);
int scegra_get_id(int index);
ScegraNode *scegra_get_node(int index);
int scegra_out_of_bounds(int index);
void scegra_draw();
void scegra_update(double dt);

void scegra_init();
int scegra_nodes_max();
int scegra_id_in_use_p(int index);
int scegra_get_free_id(int minimum);

ScegraStyle *scegrastyle_initempty(ScegraStyle *self);
void scegra_draw_image(ScegraNode *self);
void scegra_draw_text(ScegraNode *self);
void scegra_draw_longtext(ScegraNode *self);
void scegra_draw_box(ScegraNode *self);
void scegra_update_generic(ScegraNode *self, double dt);

int scegra_angle_(int index, float angle);

int scegra_make_box_style_from(int id, BeVec pos, BeVec siz, BeVec round, int sindex);
int scegra_make_text_style_from(int id, BeVec pos, BeVec siz, const char * text, int sindex);
int scegra_make_longtext_style_from(int id, BeVec pos, BeVec siz, const char * text, int sindex);
int scegra_make_image_style_from(int id, BeVec pos, BeVec siz, int image_id, int sindex);

int scegra_image_flags_(int index, int flags);
int scegra_text_flags_(int index, int flags);
int scegra_text_(int index, const char * text);


int scegra_line_stop_(int index, int stop);
int scegra_line_start_(int index, int start);
int scegra_delay_(int index, double delay);
int scegra_line_stop(int index);
int scegra_line_start(int index);
double scegra_delay(int index);

int scegra_page_lines_(int index, int lines);
int scegra_page_lines(int index);
int scegra_paused_(int index, int paused);
int scegra_paused(int index);
int scegra_page(int index);
int scegra_page_(int index, int page);
int scegra_next_page(int index);
int scegra_previous_page(int index);
int scegra_last_page(int index);
int scegra_at_end(int index);

int scegra_show_system_mouse_cursor(int show);



#endif




