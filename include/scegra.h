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
};


ScegraStyle * scegrastyle_initempty(ScegraStyle * self);

int scegra_make_box(int id, BeVec pos, BeVec siz, BeVec round, ScegraStyle style);


int scegra_z(int index);
int scegra_z_(int index, int z);

int scegra_make_text(int id, BeVec pos, const char * text, ScegraStyle style);
int scegra_make_image(int id, BeVec pos, int image_id, ScegraStyle style);

int scegra_visible_(int index,int is_visible);
int scegra_image_id_(int index,int rindex);
int scegra_font_id_(int index,int rindex);
int scegra_background_image_id_(int index,int rindex);
int scegra_speed_(int index,float x,float y);
int scegra_size_(int index,float w,float h);
int scegra_position_(int index,float x,float y);
int scegra_border_thickness_(int index,float t);
int scegra_background_color_(int index,int r,int g,int b,int a);
int scegra_border_color_(int index,int r,int g,int b,int a);
int scegra_color_(int index,int r,int g,int b,int a);
int scegra_z_(int index,int z);
int scegra_z(int index);
int scegra_make_image(int id,BeVec pos,int image_id,ScegraStyle style);
int scegra_make_text(int id,BeVec pos,const char *text,ScegraStyle style);
int scegra_make_box(int id,BeVec pos,BeVec siz,BeVec round,ScegraStyle style);
int scegra_disable_node(int index);
int scegra_get_id(int index);
ScegraNode *scegra_get_node(int index);
int scegra_out_of_bounds(int index);
void scegra_draw();
void scegra_update(double dt);
int scegranode_compare_for_drawing(const void *p1,const void *p2);
void scegra_init();
int scegra_nodes_max();
ScegraStyle *scegrastyle_initempty(ScegraStyle *self);
void scegra_draw_image(ScegraNode *self);
void scegra_draw_text(ScegraNode *self);
void scegra_draw_box(ScegraNode *self);
void scegra_update_generic(ScegraNode *self,double dt);



#endif




