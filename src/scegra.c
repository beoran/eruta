

#include "scegra.h"
#include "eruta.h"
#include "bevec.h"
#include "image.h"
#include "rebox.h"
#include "bad.h"
#include "draw.h"
#include "str.h"
#include "flags.h"
#include "store.h"
#include "state.h"
#include "laytext.h"
#include <string.h>



#define SCEGRA_TEXT_MAX 128

enum ScegraNodeFlags_ {
  SCEGRA_NODE_HIDE      = 1
};


/* A very simple scene graph, mainly for drawing the UI that will be managed from 
 * the scripting side of things. To avoid memory allocation, limit the amount of 
 * vertices allowed to SCERGRA_VERTEX_MAX (32) and characters allowed for a 
 * string to 128. (enough to fill the screen with a font of 5 pixels wide).
 * Of course this is all for a LINE of text, the scegra will use multiple text 
 * text lines for multiline displays.
 */

struct ScegraRound_ {
  float start_theta, delta_theta;
};

struct ScegraBitmap_ {
  BeVec src_pos, src_size;
  float angle;
  int flags;
  int image_id;
};

struct ScegraBox_ {
  BeVec round; 
};

struct ScegraLine_ {
  float _unused;
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
  char text[SCEGRA_TEXT_MAX];
  int from, until;
};

struct ScegraLongText_ {
  float x1, x2, y, diff;
  int flags;
  char * text;
  int from, until;
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
  struct ScegraLongText_        longtext;
};

typedef union ScegraData_ ScegraData;






struct ScegraNode_ {
  int                   id;
  int                   z;
  int                   step;
  int                   kind;
  int                   flags;
  BeVec                 pos;
  BeVec                 size;
  BeVec                 speed;
  struct ScegraStyle_   style;
  union   ScegraData_   data;
  ScegraDraw          * draw;
  ScegraUpdate        * update;
  ScegraDone          * done;
};


/* Scene graph struct. */
struct Scegra_ {
  ScegraNode * nodes;
  int          node_max;
};


#define SCEGRA_NODES_MAX 10000

/* Static storage for nodes. */
static ScegraNode   scegra_nodes[SCEGRA_NODES_MAX];

/* Nodes pointers sorted in drawing order. */
static ScegraNode * scegra_nodes_todraw[SCEGRA_NODES_MAX];

/* How many nodes to draw this time. */
static int scegra_to_draw = 0;


ScegraNode * 
scegranode_done(ScegraNode * self) {
  if (!self) return NULL;
  if (self->done) self->done(self);
  self->id = -1;
  self->z  = -1;
  return self;
}




ScegraNode * 
scegranode_initall(ScegraNode * node, int id, BeVec pos, BeVec siz, 
                   ScegraData data,
                   ScegraStyle style, ScegraDraw * draw, ScegraUpdate * update) {
  if(!node) return NULL;
  scegranode_done(node);
  node->id      = id;
  node->z       = id;
  node->pos     = pos;
  node->size    = siz;
  node->speed   = bevec0();
  node->data    = data;
  node->style   = style;
  node->draw    = draw;
  node->update  = update;
  node->step    = 0;
  node->flags   = 0;
  node->done    = NULL;
  return node;
}



ScegraNode * 
scegranode_longtext_done(ScegraNode * self) {
   free(self->data.longtext.text);
   self->data.longtext.text = NULL;
}

void scegra_update_generic(ScegraNode * self, double dt) {
  self->pos = bevec_add(self->pos, bevec_mul(self->speed, dt));
}

void scegra_draw_box(ScegraNode * self) {
  BeVec p2;
  p2 = bevec_add(self->pos, self->size);
  float thick = self->style.border_thickness;
  
  if (self->style.background_image_id >= 0) {
    ALLEGRO_BITMAP * bmp = store_get_bitmap(self->style.background_image_id);
    /* XXX: need another param for corner size or is the autodetect OK? */
    image_blitscale9(bmp, self->pos.x, self->pos.y, self->size.x, self->size.y,
                     -1, -1);
   // self->data.box.round.x, self->data.box.round.y);
  } else {   
    al_draw_filled_rounded_rectangle(self->pos.x, self->pos.y, p2.x, p2.y,
      self->data.box.round.x, self->data.box.round.y, self->style.background_color
    );
  }
  if (self->style.border_thickness > 0.0f) {
      al_draw_rounded_rectangle(self->pos.x + thick/2, self->pos.y + thick/2, p2.x - thick/2, p2.y - thick/2,
    self->data.box.round.x - thick/4,   self->data.box.round.y - thick/4, self->style.border_color,    
    thick);
  }
}


void scegra_draw_text(ScegraNode * self) {
  Font * font;
  int flags;
  /*  Use default font if font not loeaded. */
  font = store_get_font(self->style.font_id);
  if (!font) font =  state_font(state_get());
  flags = self->data.text.flags | ALLEGRO_ALIGN_INTEGER;
  /* Draw the text twice, once offset in bg color to produce a shadow, 
   and once normally with foreground color. */
  al_draw_text(font, self->style.background_color, self->pos.x + 1, self->pos.y + 1, flags, self->data.text.text);
  al_draw_text(font, self->style.color, self->pos.x, self->pos.y, flags, self->data.text.text);
}


void scegra_draw_longtext(ScegraNode * self) {
  Font * font;
  int flags;
  /*  Use default font if font not loeaded. */
  font = store_get_font(self->style.font_id);
  if (!font) font =  state_font(state_get());
  flags = /*self->data.longtext.flags |*/ ALLEGRO_ALIGN_INTEGER;
  /* Draw the text twice, once offset in bg color to produce a shadow, 
   and once normally with foreground color. */
  draw_multi_line_text(font, self->style.background_color, 
                       self->pos.x + 1, self->pos.y + 1, 
                       self->size.x, self->size.y,
                       flags, self->data.longtext.text);
  
  draw_multi_line_text(font, self->style.color, 
                       self->pos.x, self->pos.y, 
                       self->size.x, self->size.y,
                       flags, self->data.longtext.text);
}

void scegra_draw_image(ScegraNode * self) {
  Image * image;
  int flags     = self->data.bitmap.flags;
  float angle   = self->data.bitmap.angle;
  float cx, cy, dx, dy, xscale, yscale;
  cx     = self->data.bitmap.src_pos.x + self->data.bitmap.src_size.x / 2;
  cy     = self->data.bitmap.src_pos.y + self->data.bitmap.src_size.y / 2;
  dx     = self->pos.x + self->size.x / 2;
  dy     = self->pos.y + self->size.y / 2;
  xscale = self->size.x / self->data.bitmap.src_size.x;
  yscale = self->size.y / self->data.bitmap.src_size.y;
  
  image = store_get_bitmap(self->data.bitmap.image_id);
  /* Draw missing image rectangle. */
  if (!image) { 
    BeVec p2;
    p2 = bevec_add(self->pos, self->size);
    al_draw_filled_rounded_rectangle(self->pos.x, self->pos.y, p2.x, p2.y,
      self->data.box.round.x, self->data.box.round.y, self->style.background_color
    );
  } else { 
    al_draw_tinted_scaled_rotated_bitmap(image, self->style.color, cx, cy, dx, dy, xscale, yscale, angle, flags);
  }
}


ScegraStyle * scegrastyle_initempty(ScegraStyle * self) {
  if (!self) return NULL;
  self->background_image_id     = -1;
  self->color                   = al_map_rgba_f(1.0,1.0,1.0,1.0);
  self->background_color        = al_map_rgba_f(0.0,0.0,0.0,0.0);
  self->border_color            = al_map_rgba_f(1.0,1.0,1.0,1.0);
  self->font_id                 = -1;
  self->line_join_style         = 0;
  self->line_miter_limit        = 1;
  self->border_thickness        = -1;
  return self;
}



ScegraNode * 
scegranode_init_box(ScegraNode * self, int id, BeVec pos, BeVec siz,  BeVec round, ScegraStyle style) {
  ScegraData data;
  if (!self) return NULL;
  data.box.round = round;
  scegranode_initall(self, id, pos, siz, data, style, scegra_draw_box, scegra_update_generic);
  return self;
}

ScegraNode * 
scegranode_init_text(ScegraNode * self, int id, BeVec pos, const char * text, ScegraStyle style) {
  ScegraData data;
  if (!self) return NULL;
  strncpy(data.text.text, text, SCEGRA_TEXT_MAX);
  data.text.text[SCEGRA_TEXT_MAX - 1] = '\0';
  scegranode_initall(self, id, pos, bevec0(), data, style, scegra_draw_text, scegra_update_generic);
  return self;
}


ScegraNode * 
scegranode_init_longtext
(ScegraNode * self, int id, BeVec pos, BeVec siz, char * text, ScegraStyle style) {
  ScegraData data;
  if (!self) return NULL;
  data.longtext.text  =  strdup(text);
  scegranode_initall(self, id, pos, siz, data, style, scegra_draw_longtext, scegra_update_generic);
  self->done = scegranode_longtext_done;
  return self;
}


ScegraNode * 
scegranode_init_image_ex(
  ScegraNode * self, int id, BeVec pos, BeVec siz, int image_id, ScegraStyle style,
  BeVec spos, BeVec ssiz, float angle, int flags) {
  ScegraData data;
  if (!self) return NULL;
  data.bitmap.angle     = angle;
  data.bitmap.image_id  = image_id;
  data.bitmap.flags     = flags;
  data.bitmap.src_pos   = spos;
  data.bitmap.src_size  = ssiz; 
  scegranode_initall(self, id, pos, siz, data, style, scegra_draw_image, scegra_update_generic);
  return self;
}

ScegraNode * 
scegranode_init_image(ScegraNode * self, int id, BeVec pos,  int image_id, ScegraStyle style) {
  int w, h;
  BeVec siz;
  if(!store_get_bitmap_width(image_id, &w)) w = 0;
  if(!store_get_bitmap_height(image_id, &h)) h = 0;
  siz = bevec(w, h);  
  return scegranode_init_image_ex(self, id, pos, siz, image_id, style, bevec0(), siz, 0.0, 0);
}




/* Returns maximum amount of nodes. */
int scegra_nodes_max() {
  return SCEGRA_NODES_MAX;
}

/* Initializes the simple 2D scene graph */
void scegra_init() {
  int index;  
  for (index = 0; index < SCEGRA_NODES_MAX; index++) {
    ScegraNode * node = scegra_nodes + index;
    node->id = -1; /* Negative id means unused; */
  }
  scegra_to_draw = 0;
}

/* End the use of the simple 2D scene graph */
void scegra_done() {
  int index;  
  for (index = 0; index < SCEGRA_NODES_MAX; index++) {
    ScegraNode * node = scegra_nodes + index;
    scegranode_done(node);
  }
  scegra_to_draw = 0;
}


/* Compares a scene graph node with another, for drawing order. Things with low 
 z come before those with high z, and with same z, low id comes before high id. 
 qsort compatible. No NULL values allowed!!! */
static inline int scegranode_compare_for_drawing(const void * p1, const void * p2) {
  register const ScegraNode  * self  = *((ScegraNode **) p1);
  register const ScegraNode  * other = *((ScegraNode **) p2);
  register int self_val, other_val;
  /* Compare Z if no nulls. */
  self_val  = self->z; 
  other_val = other->z;
  if (self_val < other_val) return -1;
  if (self_val > other_val) return  1;  
  /* compare id if z is equal */
  self_val  = self->id; 
  other_val = other->id;
  if (self_val < other_val) return -1;
  if (self_val > other_val) return  1;  
  return 0;
}


/* Updates the 2d scene graph. */
void scegra_update(double dt) {
  register int index;
  scegra_to_draw = 0;
  
  for (index = 0; index < SCEGRA_NODES_MAX; index++) {
    register ScegraNode * node = scegra_nodes + index;
    if (node->id < 0) continue; /* Negative id means unused; */
  
    if(node->update) {
      node->update(node, dt);
    }
    /* Only add node to draw array if not hidden and drawable. */
    if((!flags_get(node->flags, SCEGRA_NODE_HIDE)) && (node->draw)) {    
      scegra_nodes_todraw[scegra_to_draw] = node;
      scegra_to_draw++;
    }
  }  
  /* Sort scene graph nodes for drawing. */
  qsort(scegra_nodes_todraw, scegra_to_draw, 
        sizeof(ScegraNode*), scegranode_compare_for_drawing); 
}


/* Draws the 2d scene graph. */
void scegra_draw() {
  int index;  
  /* All nodes in the todraw will be drawn up to scegra_to_draw,
   there are no other drawable bodes in there. */
  for (index = 0; index < scegra_to_draw; index++) {
    ScegraNode * node = scegra_nodes_todraw[index];
    node->draw(node);
  }
}

/* Returns true if out of bounds for the scene graph, or false if ok. */
int scegra_out_of_bounds(int index) {
  if (index < 0)                  return TRUE;
  if (index > scegra_nodes_max()) return TRUE;
  return FALSE;
}

/* Returns a node from the scene graph or NULL if out of bounds. */
ScegraNode * scegra_get_node(int index) {
  if (scegra_out_of_bounds(index)) return NULL;
  return scegra_nodes + index;
}

/* Returns the ID of the scene graph node at index. -1 means it's free, 
 *-2 means out of range.  */
int scegra_get_id(int index) {
  ScegraNode * node = scegra_get_node(index);
  if (!node) return -2;
  return node->id;
}

/* Sets the scene graph node as not in use.  */
int scegra_disable_node(int index) {
  ScegraNode * node = scegra_get_node(index);
  if (!node) return -2;
  scegranode_done(node);
  return node->id = -1; 
}

/* Initializes the node as a box. */
int scegra_make_box(int id, BeVec pos, BeVec siz, BeVec round, ScegraStyle style) {
  ScegraNode * node = scegra_get_node(id);
  if (!node) return -2;
  scegranode_init_box(node, id, pos, siz, round, style);
  return node->id;
}


/* Initialies the node at index as a text. Text can only span one line and must be short.
 Line wrapping must be implemented by making many text nodes for every line of text. */
int scegra_make_text(int id, BeVec pos, const char * text, ScegraStyle style) {
  ScegraNode * node = scegra_get_node(id);
  if (!node) return -2;  
  scegranode_init_text(node, id, pos, text, style);
  return node->id;
}

/* Initialies the node at index as a long text. Long text is arbitrary size and will
 * wrap automatically to the width of the scegra element.  */
int scegra_make_longtext(int id, BeVec pos, BeVec siz, char * text, ScegraStyle style) {
  ScegraNode * node = scegra_get_node(id);
  if (!node) return -2;  
  scegranode_init_longtext(node, id, pos, siz, text, style);
  return node->id;
}


/* Initializes the node at index as an image node. */
int scegra_make_image(int id, BeVec pos, int image_id, ScegraStyle style) {
  ScegraNode * node = scegra_get_node(id);
  if (!node) return -2;  
  scegranode_init_image(node, id, pos, image_id, style);
  return node->id;
} 

/* Gets the style of the scegra object, or if not active, gets a default style.  */
ScegraStyle scegra_get_style(int sindex) {
  ScegraStyle style;
  ScegraNode * snode = scegra_get_node(sindex);
  if (snode && (snode->id >= 0)) {
    style = snode->style;
  } else {
    scegrastyle_initempty(&style);
  }  
  return style;
}


/* Initializes the node as a box with a style copied from the node at sindex, 
 * or if that is not in use, a default style. */
int scegra_make_box_style_from(int id, BeVec pos, BeVec siz, BeVec round, int sindex) {
   return scegra_make_box(id, pos, siz, round, scegra_get_style(sindex));
}

/* Initializes the node as a text with a style copied from the node at sindex, 
 * or if that is not in use, a default style. */
int scegra_make_text_style_from(int id, BeVec pos, const char * text, int sindex) {
   return scegra_make_text(id, pos, text, scegra_get_style(sindex));
}

/* Initializes the node as a text long with a style copied from the node at sindex, 
 * or if that is not in use, a default style. */
int scegra_make_longtext_style_from(int id, BeVec pos, BeVec siz, const char * text, int sindex) {
   return scegra_make_longtext(id, pos, siz, text, scegra_get_style(sindex));
}


/* Initializes the node as a image with a style copied from the node at sindex, 
 * or if that is not in use, a default style. */
int scegra_make_image_style_from(int id, BeVec pos, int image_id, int sindex) {
   return scegra_make_image(id, pos, image_id, scegra_get_style(sindex));
}




/* Returns the Z level of the scene graph node at index. -1 means it's free, 
 *-2 means out of range.  */
int scegra_z(int index) {
  ScegraNode * node = scegra_get_node(index);
  if (!node) return -2;
  if (node->id < 0) return -1;
  return node->z;
}

/* Sets the Z level of the scene graph node at index. -1 means it's free, 
 *-2 means out of range.  */
int scegra_z_(int index, int z) {
  ScegraNode * node = scegra_get_node(index);
  if (!node) return -2;
  if (node->id < 0) return -1;
  return node->z = z;
}

/* Sets the FG color of the scegra node (used to draw, e.g. text or lines (but not borders) */
int scegra_color_(int index, int r, int g, int b, int a) {
  ScegraNode * node = scegra_get_node(index);
  if (!node) return -2;
  if (node->id < 0) return -1;
  node->style.color = al_map_rgba(r, g, b, a);  
  return node->z;
}

/* Sets the border color of the scegra node (used to draw, e.g. text or lines (but not borders) */
int scegra_border_color_(int index, int r, int g, int b, int a) {
  ScegraNode * node = scegra_get_node(index);
  if (!node) return -2;
  if (node->id < 0) return -1;
  node->style.border_color = al_map_rgba(r, g, b, a);  
  return node->z;
}

/* Sets the background color of the scegra node (used to draw, e.g. text or lines (but not borders) */
int scegra_background_color_(int index, int r, int g, int b, int a) {
  ScegraNode * node = scegra_get_node(index);
  if (!node) return -2;
  if (node->id < 0) return -1;
  node->style.background_color = al_map_rgba(r, g, b, a);  
  return node->z;
}

/* Sets the border thickness. Set 0 or negative to disable border.  */
int scegra_border_thickness_(int index, float t) {
  ScegraNode * node = scegra_get_node(index);
  if (!node) return -2;
  if (node->id < 0) return -1;
  node->style.border_thickness = t;  
  return node->z;
}

/* Sets the scegra node's position . */
int scegra_position_(int index, float x, float y) {
  ScegraNode * node = scegra_get_node(index);
  if (!node) return -2;
  if (node->id < 0) return -1;
  node->pos = bevec(x, y);  
  return node->z;
}

/* Sets the scegra node's size. */
int scegra_size_(int index, float w, float h) {
  ScegraNode * node = scegra_get_node(index);
  if (!node) return -2;
  if (node->id < 0) return -1;
  node->size = bevec(w, h);  
  return node->z;
}

/* Sets the scegra node's speed. */
int scegra_speed_(int index, float x, float y) {
  ScegraNode * node = scegra_get_node(index);
  if (!node) return -2;
  if (node->id < 0) return -1;
  node->speed = bevec(x, y);  
  return node->z;
}


/* Sets the scegra node's background image id . */
int scegra_background_image_id_(int index, int rindex) {
  ScegraNode * node = scegra_get_node(index);
  if (!node) return -2;
  if (node->id < 0) return -1;
  node->style.background_image_id  = rindex;  
  return node->z;
}

/* Sets the scegra node's background font id . */
int scegra_font_id_(int index, int rindex) {
  ScegraNode * node = scegra_get_node(index);
  if (!node) return -2;
  if (node->id < 0) return -1;
  node->style.font_id  = rindex;  
  return node->z;
}

/* Sets the scegra node's foreground image id . */
int scegra_image_id_(int index, int rindex) {
  ScegraNode * node = scegra_get_node(index);
  if (!node) return -2;
  if (node->id < 0) return -1;
  node->data.bitmap.image_id = rindex;
  return node->z;
}


/* Sets the scegra node  to be visible or hidden depending on is_visible. */
int scegra_visible_(int index, int is_visible) {
  int result;
  ScegraNode * node = scegra_get_node(index);
  if (!node) return -2;  
  if (node->id < 0) return -1;
  result            = flags_get(node->flags, SCEGRA_NODE_HIDE) ? 1 : 0; 
  flags_put(&node->flags, SCEGRA_NODE_HIDE, !is_visible); 
  return result;
}


/* Sets the scegra node's drawing angle for images. */
int scegra_angle_(int index, float angle) {
  ScegraNode * node = scegra_get_node(index);
  if (!node) return -2;
  if (node->id < 0) return -1;
  node->data.bitmap.angle = angle;
  return node->z;
}

/* Sets the scegra node's drawing flags for images. */
int scegra_image_flags_(int index, int flags) {
  ScegraNode * node = scegra_get_node(index);
  if (!node) return -2;
  if (node->id < 0) return -1;
  node->data.bitmap.flags = flags;
  return node->z;
}


/* Sets the scegra node's drawing flags for texts. */
int scegra_text_flags_(int index, int flags) {
  ScegraNode * node = scegra_get_node(index);
  if (!node) return -2;
  if (node->id < 0) return -1;
  node->data.text.flags = flags;
  return node->z;
}

/* Somewhat unrelated, show or hide the system mouse cursor. 
 * Returns the state after calling, true means show, false means not shown.
 */
int scegra_show_system_mouse_cursor(int show) {
  ALLEGRO_DISPLAY * display;
  display = al_get_current_display();
  if(!display) return FALSE;
  if (show) {
    return al_show_mouse_cursor(display);
  } else {
    return (!al_hide_mouse_cursor(display));
  }
}


/** Gets the speed of the scregra node. Returns negative on error. */
int scegra_speed(int index, float * x, float * y) {
  ScegraNode * node = scegra_get_node(index);
  if (!x || !y)         return -3;
  if (!node)            return -2;
  if (node->id < 0)     return -1;
  (*x) = node->speed.x;
  (*y) = node->speed.y;
  return node->z;  
}

/** Gets the size of the scregra node. Returns negative on error. */
int scegra_size(int index, float * w, float * h) {
  ScegraNode * node = scegra_get_node(index);
  if (!w || !h)         return -3;
  if (!node)            return -2;
  if (node->id < 0)     return -1;
  (*w) = node->size.x;
  (*h) = node->size.y;
  return node->z;  
}

int scegra_position(int index, float *x, float * y) {
/** Gets the positiob of the scregra node. Returns negative on error. */
  ScegraNode * node = scegra_get_node(index);
  if (!x || !y)         return -3;
  if (!node)            return -2;
  if (node->id < 0)     return -1;
  (*x) = node->pos.x;
  (*y) = node->pos.y;
  return node->z;  
}

int scegra_border_thickness(int index, float * t);
int scegra_background_color(int index, int * r, int * g,int * b,int * a);
int scegra_border_color(int index,int * r,int * g,int * b, int * a);
int scegra_color(int index, int * r, int * g, int * b,int * a);

/** Returns the natural size of the bitmap of the image. Will be 
 * set to -1 if the bitmap isn't loaded yet.
 */
int scegra_image_bitmap_size(int index, int * w, int * h) {
  int image_id;
  ScegraNode * node = scegra_get_node(index);
  if (!w || !h)         return -3;
  if (!node)            return -2;
  if (node->id < 0)     return -1;
  image_id  = node->data.bitmap.image_id;
  if(!store_get_bitmap_width(image_id, w))  (*w) = -1;
  if(!store_get_bitmap_height(image_id, h)) (*h) = -1;
  return node->z;  
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
#define TUPLE(A,B) tuple_##A##B                   
#define DEFINE_TUPLE(A, B)  typedef struct TUPLE(A,B) { A head; B tail; } TUPLE(A,B)
#define TUPLE_HEAD(T)       (T).head
#define TUPLE_TAIL(T)       (T).tail
#define TUPLEPTR_HEAD(T)    (T)->head
#define TUPLEPTR_TAIL(T)    (T)->tail


typedef char * charp;

DEFINE_TUPLE(int, charp);

void scegra_foo() {
  TUPLE(int, charp) tuple;
  TUPLE_HEAD(tuple) = 1;
  TUPLE_TAIL(tuple) = "Hello";
  printf("%d %s\n", TUPLE_HEAD(tuple), TUPLE_TAIL(tuple));
}











