

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

enum ScegraNodeKind_ {
  SCEGRA_NODE_UNKNOWN = 0,
  SCEGRA_NODE_BITMAP,
  SCEGRA_NODE_BOX,
  SCEGRA_NODE_LINE,
  SCEGRA_NODE_TRIANGLE,
  SCEGRA_NODE_SPLINE,
  SCEGRA_NODE_PIXEL,
  SCEGRA_NODE_POLYGON,
  SCEGRA_NODE_PRIM,
  SCEGRA_NODE_TEXT,
  SCEGRA_NODE_LONGTEXT,
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
  char text[SCEGRA_TEXT_MAX];
  int from, until;
};

/* For long text that can scroll and display letter per letter. */
struct ScegraLongText_ {
  float x1, x2, y, diff;
  char * text;
  int line_start, line_stop, line_pos; /* Current text "window". */
  int line_max, line_height; /* text constants. */
  int page_lines; /* Text "window" size for one "page" of text in amount of lines. */
  int paused;
  double delay_total;
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
  double                delay;
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
#define SCEGRA_LONGTEXT_LINE_POS_MAX 99999

/* Static storage for nodes. */
static ScegraNode   scegra_nodes[SCEGRA_NODES_MAX];

/* Nodes pointers sorted in drawing order. */
static ScegraNode * scegra_nodes_todraw[SCEGRA_NODES_MAX];

/* How many nodes to draw this time. */
static int scegra_to_draw = 0;


void 
scegranode_done(ScegraNode * self) {
  if (!self) return;
  if (self->done) self->done(self);
  self->id = -1;
  self->z  = -1;
}




ScegraNode * 
scegranode_initall(ScegraNode * node, int kind, int id, BeVec pos, BeVec siz, 
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
  node->kind    = kind;
  node->delay   = 0.05;
  return node;
}

/* Gets pointer to the font for this scegra node, or to a default font. */
static Font * scegra_get_font(ScegraNode * self) {
  Font * font;
  /*  Use default font if font not loaded. */
  font = store_get_font(self->style.font_id);
  if (!font) font =  state_font(state_get());
  return font;
}


/** Gets the current text page for a longtext. */
int scegranode_page(ScegraNode * node) {
  struct ScegraLongText_ * lt; 
  if (!node) return -2;
  if (node->id < 0) return -1;
  if (node->kind != SCEGRA_NODE_LONGTEXT) return -3;
  lt = &node->data.longtext;
  return lt->line_start / lt->page_lines;
}


/** Gets the last page number for a longtext or negative on error. */
int scegranode_last_page(ScegraNode * node) {
  struct ScegraLongText_ * lt; 
  if (!node) return -2;
  if (node->id < 0) return -1;
  if (node->kind != SCEGRA_NODE_LONGTEXT) return -3;
  lt              = &node->data.longtext;
  return lt->line_max / lt->page_lines;
}

/** Advances long text to the given page. Automatically unpauses as well. */
int scegranode_page_(ScegraNode * node, int page) {
  struct ScegraLongText_ * lt;
  int line;
  if (!node) return -2;
  if (node->id < 0) return -1;
  if (node->kind != SCEGRA_NODE_LONGTEXT) return -3;
  if (page < 0) return -4;  
  lt              = &node->data.longtext;
  line            = page * lt->page_lines;
  /* Check for page overflow. */
  if (line >= lt->line_max) {
    return -5;    
  }  
  lt->paused     = false;
  lt->line_start = line;

  /* Negative delay is instant display. */
  if (node->delay < 0) {  
    lt->line_stop  = lt->line_start + lt->page_lines;
    lt->line_pos   = SCEGRA_LONGTEXT_LINE_POS_MAX;
  } else {
    lt->line_stop  = lt->line_start + 1;
    lt->line_pos   = 0;
  }  
  return page;  
}

void
scegranode_longtext_done(ScegraNode * self) {
   free(self->data.longtext.text);
   self->data.longtext.text = NULL;
}

void scegra_update_generic(ScegraNode * self, double dt) {
  self->pos = bevec_add(self->pos, bevec_mul(self->speed, dt));
}


/* This function is the helper callback that implements
 * updating scrolled/partial text for Longtexts. 
 */
static bool scegra_update_custom_partial_text(int line_num, const char *line,
  int size, void *extra) {
  ScegraNode * self = extra;
  struct ScegraLongText_ * st = &self->data.longtext;
  st->line_max = line_num;

  /* Don't draw lines before start but keep on drawing (for later lines) */
  if (line_num < st->line_start) return true;
  /* Don't draw lines after stop, but keep calculating to get correct amount of lines */
  if (line_num >= st->line_stop) return true;
  /* Reveal letter by letter on last line */
  if (line_num == (st->line_stop - 1)) {
    /* Advance the position automatically if not paused. */
    if (!st->paused) {
      st->line_pos++;
    }
    /* Reached eol, advance to next line. */
    if (st->line_pos >= size) {
      /* Is if the text window is full, pause, otherwise show the next line. */
      if ((st->line_stop - st->line_start) >= st->page_lines) {
        st->paused = true;
      } else { 
        st->line_stop++;
        st->line_pos = 0;
      }
    } 
  }
  (void) line;
  return true;
}


/* Updates the longtext, enables scrolling and per character display. */
void scegra_update_longtext(ScegraNode * self, double dt) {
  scegra_update_generic(self, dt);
  float width = self->size.x - self->style.margin * 2;
  struct ScegraLongText_ * st = &self->data.longtext;
  char * text      = self->data.longtext.text ?
              self->data.longtext.text : "NULL";

  /* Delay advence of characters somewhat. */
  st->delay_total += dt;
  if (st->delay_total < self->delay) return;
  st->delay_total = 0;
  
  st->line_max = 0;
  al_do_multiline_text(scegra_get_font(self), width,
      (const char *) text, scegra_update_custom_partial_text, self);
  st->line_max++;
  
  /* pause if the last line is reached, and prevent overflow. */
  if (st->line_stop > st->line_max) {
    st->paused    = true;
    st->line_pos  = SCEGRA_LONGTEXT_LINE_POS_MAX;
    st->line_stop = st->line_max;
  }

}

/* Returns TRUE if the longtext node is at the end of the text to display,
 * false if not (more text to display) */
int scegranode_longtext_at_end(ScegraNode * node) {
  struct ScegraLongText_ * lt; 
  if (!node) return FALSE;
  if (node->id < 0) return FALSE;
  if (node->kind != SCEGRA_NODE_LONGTEXT) return FALSE;
  lt              = &node->data.longtext;
  return  ((lt->line_stop >= lt->line_max) &&
           (lt->line_pos >= SCEGRA_LONGTEXT_LINE_POS_MAX));
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




/* Calculates the x and y position where to draw text, taking alignment and
 * margin into consideration. */
static BeVec
scegra_calculate_text_position(ScegraNode * self) {
  BeVec result;
  int flags = self->style.text_flags;
  result.x  = self->pos.x + self->style.margin;
  result.y  = self->pos.y + self->style.margin;

  if (flags & ALLEGRO_ALIGN_CENTER) {
    result.x  = self->pos.x + self->style.margin + self->size.x / 2;
  } else if (flags & ALLEGRO_ALIGN_RIGHT) {
    result.x  = self->pos.x + self->size.x - self->style.margin;
  }
  return result;
}

void scegra_draw_text(ScegraNode * self) {
  Font * font;
  int flags;
  BeVec pos = scegra_calculate_text_position(self);
  font      = scegra_get_font(self);
  flags     = self->style.text_flags | ALLEGRO_ALIGN_INTEGER;
  /* Draw the text twice, once offset in bg color to produce a shadow, 
   and once normally with foreground color. */
  al_draw_text(font, self->style.background_color, pos.x + 1, pos.y + 1,
    flags, self->data.text.text);
  al_draw_text(font, self->style.color, pos.x, pos.y,
    flags, self->data.text.text);
}




/** Calculates the amount of lines that scegra_draw_partial_text can draw
 * at most with this text. */
static int scegra_partial_text_lines(ALLEGRO_FONT * font, float w,
const char * text)
{
  (void) font; (void) w; (void) text;
  return 0; /* XXX todo */
}


/* This function is the helper callback that implements the actual drawing
 * for scegra_draw_partial_text.
 */
static bool scegra_draw_custom_partial_text(int line_num, const char *line,
  int size, void *extra) {
  float x, y;
  ScegraNode * self = extra;
  struct ScegraLongText_ * st = &self->data.longtext;
  ALLEGRO_USTR_INFO info;
  int real_size, flags;
  double width;
  ALLEGRO_FONT * font;
  BeVec pos;
  
  font      = scegra_get_font(self);
  flags     = self->style.text_flags | ALLEGRO_ALIGN_INTEGER;
  pos = scegra_calculate_text_position(self);
 

  
  /* Don't draw lines before start but keep on drawing (for later lines) */
  if (line_num < st->line_start) return true;
  /* Don't draw lines after stop, drawing is done */
  if (line_num >= st->line_stop) return false;
  real_size = size;
  /* calculate position */
  x  = pos.x;
  y  = pos.y + (st->line_height * (line_num - st->line_start));

  /* Reveal letter by letter on last line */
  if (line_num == (st->line_stop - 1)) { 
    real_size = (st->line_pos < size) ? st->line_pos : size;

    /* Draw continuation marker if paused. */
    if (st->paused) {
      float x1, y1, x2, y2, x3, y3;
      y2 = y  + 8;
      x2 = x + self->size.x - self->style.margin;
      x1 = x2 - 8;
      y1 = y2;
      x3 = x2 - 4;
      y3 = y2 + 8;
      al_draw_filled_triangle(x1, y1, x2, y2, x3, y3, self->style.color);      
    }
  }
  al_draw_ustr(font, self->style.background_color,
      x + 1, y + 1, flags, al_ref_buffer(&info, line, real_size));
  al_draw_ustr(font, self->style.color,
      x, y, flags, al_ref_buffer(&info, line, real_size));
                
  return true;
}


/* Allows to draw a multi line text partially, from line_start up to
 * line_stop 
 * Scraws scrolling text from a prefilled struct. */ 
void scegra_draw_partial_text(ScegraNode * self) {
  float width = self->size.x - self->style.margin * 2;
  struct ScegraLongText_ * st = &self->data.longtext;
  char * text      = self->data.longtext.text ?
              self->data.longtext.text : "NULL";
  /* It's a bit of a hack that this ends up here... */ 
  if (st->line_height < 1.0) {
    st->line_height = al_get_font_line_height(scegra_get_font(self));
  }
  
  al_do_multiline_text(scegra_get_font(self), width,
      (const char *) text, scegra_draw_custom_partial_text, self);

  
}


void scegra_draw_longtext(ScegraNode * self) {
  Font * font;
  int flags;
  const char * text;
  float width;
  BeVec pos = scegra_calculate_text_position(self);
  font      = scegra_get_font(self);
  flags     = self->style.text_flags | ALLEGRO_ALIGN_INTEGER;
  text      = self->data.longtext.text ?
              self->data.longtext.text : "NULL";
  width     = self->size.x - self->style.margin * 2;

  scegra_draw_partial_text(self);

  
  /* Draw the text twice, once offset in bg color to produce a shadow, 
   and once normally with foreground color. */
 /*  
  al_draw_multiline_text(font, self->style.background_color,
    pos.x + 1, pos.y + 1, width, 0, flags, text);
  al_draw_multiline_text(font, self->style.color,
    pos.x, pos.y, width, 0, flags, text);
  **/
}

void scegra_draw_image(ScegraNode * self) {
  Image * image;
  int flags     = self->style.image_flags;
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
  self->image_flags             = 0;
  self->text_flags              = 0;
  self->margin                  = 0;
  return self;
}



ScegraNode * 
scegranode_init_box(ScegraNode * self, int id, BeVec pos, BeVec siz,  BeVec round, ScegraStyle style) {
  ScegraData data;
  if (!self) return NULL;
  data.box.round = round;
  scegranode_initall(self, SCEGRA_NODE_BOX,
    id, pos, siz, data, style, scegra_draw_box, scegra_update_generic);
  return self;
}

ScegraNode * 
scegranode_init_text(ScegraNode * self, int id, BeVec pos, BeVec siz, const char * text, ScegraStyle style) {
  ScegraData data;
  if (!self) return NULL;
  strncpy(data.text.text, text, SCEGRA_TEXT_MAX);
  data.text.text[SCEGRA_TEXT_MAX - 1] = '\0';
  scegranode_initall(self, SCEGRA_NODE_TEXT,
    id, pos, siz, data, style, scegra_draw_text, scegra_update_generic);
  return self;
}



int scegranode_set_longtext_text(ScegraNode * self, const char * text) {
  free(self->data.longtext.text);
  self->data.longtext.text        = cstr_dup((char *)text);
  if (!self->data.longtext.text) return -1;
  self->data.longtext.line_start  = 0;
  self->data.longtext.line_stop   = 1;
  self->data.longtext.line_pos    = 0;
  self->data.longtext.delay_total = 0.0;
  scegranode_page_(self, 0);
  return 0;
}

ScegraNode * 
scegranode_init_longtext
(ScegraNode * self, int id, BeVec pos, BeVec siz, char * text, ScegraStyle style) {
  ScegraData data;
  if (!self) return NULL;
  memset(&data.longtext, 0, sizeof(data.longtext));
  data.longtext.page_lines = 4;
  data.longtext.line_start = 0;
  data.longtext.line_stop  = 1;
  data.longtext.paused     = 0;

  scegranode_initall(self, SCEGRA_NODE_LONGTEXT,
    id, pos, siz, data, style, scegra_draw_longtext, scegra_update_longtext);
  scegranode_set_longtext_text(self, text);
  
  self->done = scegranode_longtext_done;
  scegranode_page_(self, 0);
  
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
  data.bitmap.src_pos   = spos;
  data.bitmap.src_size  = ssiz; 
  scegranode_initall(self, SCEGRA_NODE_BITMAP,
    id, pos, siz, data, style, scegra_draw_image, scegra_update_generic);
  self->style.image_flags = flags;
  return self;
}

ScegraNode * 
scegranode_init_image(ScegraNode * self, int id, BeVec pos, BeVec siz, int image_id, ScegraStyle style) {
  int w, h;
  if (siz.x <= 0.0)  { if (store_get_bitmap_width(image_id, &w)) siz.x = w;  }
  if (siz.y <= 0.0)  { if (store_get_bitmap_height(image_id, &h)) siz.y = h; }
  return scegranode_init_image_ex(self, id, pos, siz, image_id, style, bevec0(), siz, 0.0, 0);
}


/* Returns nonzero if the scegranode is in use, false if not */
static int scegranode_in_use_p(ScegraNode * node) {
  return node->id != -1;
}

/* Returns nonzero if the scene graph node id is in use. Also returns
 * false if the ID is out of range. */
int scegra_id_in_use_p(int index) {
  ScegraNode * node = scegra_get_node(index);
  if (!node) return FALSE;
  return scegranode_in_use_p(node);
}

/* Returns the first free ID that is larger than the minimum. Returns
 * negative if no more ID's are free. */
int scegra_get_free_id(int minimum) {
  int index;
  if (minimum < 0) return -1;
  for (index = minimum; index < SCEGRA_NODES_MAX; index++) {
    if (!scegra_id_in_use_p(index)) {
      return index;
    }
  }
  return -1; 
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
int scegra_make_text(int id, BeVec pos, BeVec siz, const char * text, ScegraStyle style) {
  ScegraNode * node = scegra_get_node(id);
  if (!node) return -2;  
  scegranode_init_text(node, id, pos, siz, text, style);
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
int scegra_make_image(int id, BeVec pos, BeVec siz, int image_id, ScegraStyle style) {
  ScegraNode * node = scegra_get_node(id);
  if (!node) return -2;  
  scegranode_init_image(node, id, pos, siz, image_id, style);
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
int scegra_make_text_style_from(int id, BeVec pos, BeVec siz, const char * text, int sindex) {
   return scegra_make_text(id, pos, siz, text, scegra_get_style(sindex));
}

/* Initializes the node as a text long with a style copied from the node at sindex, 
 * or if that is not in use, a default style. */
int scegra_make_longtext_style_from(int id, BeVec pos, BeVec siz, const char * text, int sindex) {
   return scegra_make_longtext(id, pos, siz, (char *)text, scegra_get_style(sindex));
}


/* Initializes the node as a image with a style copied from the node at sindex, 
 * or if that is not in use, a default style. */
int scegra_make_image_style_from(int id, BeVec pos, BeVec siz, int image_id, int sindex) {
   return scegra_make_image(id, pos, siz, image_id, scegra_get_style(sindex));
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

/* Sets the margin. Set 0 or negative to disable border.  */
int scegra_margin_(int index, float m) {
  ScegraNode * node = scegra_get_node(index);
  if (!node) return -2;
  if (node->id < 0) return -1;
  node->style.margin = m;  
  return node->z;
}

/* Sets the scegra node's position. */
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
  node->style.image_flags = flags;
  return node->z;
}


/* Sets the scegra node's drawing flags for texts. */
int scegra_text_flags_(int index, int flags) {
  ScegraNode * node = scegra_get_node(index);
  if (!node) return -2;
  if (node->id < 0) return -1;
  node->style.text_flags = flags;
  return node->z;
}

/* Updates the scegra node's text if it is a text or longtext. Returns the node's z value
 * on sucess or negative on failure, if it is not a text or longtext node. */
int scegra_text_(int index, const char * text) {
  ScegraNode * node = scegra_get_node(index);
  if (!node) return -2;
  if (node->id < 0) return -1;
  if (node->kind == SCEGRA_NODE_TEXT) {
    strncpy(node->data.text.text, text, SCEGRA_TEXT_MAX);
  } else if (node->kind == SCEGRA_NODE_LONGTEXT) {
    scegranode_longtext_done(node);
    scegranode_set_longtext_text(node, text);
  } else {
    return -3;
  }
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


/** Sets the last line to display for a long text */
int scegra_line_stop_(int index, int stop) {
  ScegraNode * node = scegra_get_node(index);
  if (!node) return -2;
  if (node->id < 0) return -1;
  if (node->kind != SCEGRA_NODE_LONGTEXT) return -3;
  node->data.longtext.line_stop = stop;
  return node->z;
}

/** Sets the first line to display for a long text */
int scegra_line_start_(int index, int start) {
ScegraNode * node = scegra_get_node(index);
  if (!node) return -2;
  if (node->id < 0) return -1;
  if (node->kind != SCEGRA_NODE_LONGTEXT) return -3;
  node->data.longtext.line_start = start;
  return node->z;
}

/** Sets display delay between individual characters  for a long text */
int scegra_delay_(int index, double delay) {
  ScegraNode * node = scegra_get_node(index);
  if (!node) return -2;
  if (node->id < 0) return -1;
  if (node->kind != SCEGRA_NODE_LONGTEXT) return -3;
  node->delay = delay;
  return node->z;
}



/** Gets the last line to display for a long text  or negative on error*/
int scegra_line_stop(int index) {
  ScegraNode * node = scegra_get_node(index);
  if (!node) return -2;
  if (node->id < 0) return -1;
  if (node->kind != SCEGRA_NODE_LONGTEXT) return -3;
  return node->data.longtext.line_stop;
}

/** Gets the first line to display for a long text */
int scegra_line_start(int index) {
  ScegraNode * node = scegra_get_node(index);
  if (!node) return -2;
  if (node->id < 0) return -1;
  if (node->kind != SCEGRA_NODE_LONGTEXT) return -3;
  return node->data.longtext.line_start;
}

/** Gets display delay between individual characters  for a long text */
double scegra_delay(int index) {
  ScegraNode * node = scegra_get_node(index);
  if (!node) return -2;
  if (node->id < 0) return -1;
  if (node->kind != SCEGRA_NODE_LONGTEXT) return -3;
  return node->delay;
}


/** Sets amount of shown lines for a long text */
int scegra_page_lines_(int index, int lines) {
  ScegraNode * node = scegra_get_node(index);
  if (!node) return -2;
  if (node->id < 0) return -1;
  if (node->kind != SCEGRA_NODE_LONGTEXT) return -3;
  node->data.longtext.page_lines = lines;
  return node->z;
}

/** Gets amount of lines for a "page" of long text */
int scegra_page_lines(int index) {
  ScegraNode * node = scegra_get_node(index);
  if (!node) return -2;
  if (node->id < 0) return -1;
  if (node->kind != SCEGRA_NODE_LONGTEXT) return -3;
  return node->data.longtext.page_lines;
}

/** Sets paused state of long text */
int scegra_paused_(int index, int paused) {
  ScegraNode * node = scegra_get_node(index);
  if (!node) return -2;
  if (node->id < 0) return -1;
  if (node->kind != SCEGRA_NODE_LONGTEXT) return -3;
  node->data.longtext.paused = paused;
  return node->z;
}

/** Gets paused state of long text */
int scegra_paused(int index) {
  ScegraNode * node = scegra_get_node(index);
  if (!node) return -2;
  if (node->id < 0) return -1;
  if (node->kind != SCEGRA_NODE_LONGTEXT) return -3;
  return node->data.longtext.paused;
}

/** Gets the current text page for a longtext. */
int scegra_page(int index) {
  ScegraNode * node = scegra_get_node(index);
  return scegranode_page(node);
}

/** Gets the number of the last text page for a longtext. */
int scegra_last_page(int index) {
  ScegraNode * node = scegra_get_node(index);
  return scegranode_last_page(node);
}


/** Returns nonzero if the long text is at the end of it's display. */
int scegra_at_end(int index) {
  ScegraNode * node = scegra_get_node(index);
  return scegranode_longtext_at_end(node);
}

/** Advances long text to the given page. Automatically unpauses as well. */
int scegra_page_(int index, int page) {
  ScegraNode * node = scegra_get_node(index);
  return scegranode_page_(node, page);
}

/** Advances long text to the next page. Automatically unpauses as well. */
int scegra_next_page(int index) {
  int page = scegra_page(index);
  if (page < 0) return page;
  return scegra_page_(index, page + 1); 
}

/** Advances long text to the previous page. Automatically unpauses as well. */
int scegra_previous_page(int index) {
  int page = scegra_page(index);
  if (page < 0) return page;
  return scegra_page_(index, page - 1); 
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











