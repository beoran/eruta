

#include "scegra.h"
#include "eruta.h"
#include "image.h"
#include "rebox.h"
#include "bad.h"
#include "draw.h"
#include "str.h"

/* A very simple scene graph, mainly for drawing the UI that will be managed from 
 * the scripting side of things. 
 */

/* Struct: ScegraStyle 
* Describes the style of a Scene graph node.
*/
struct ScegraStyle_ {
  Color     forecolor;
  Color     backcolor;
  Font    * font;
  Image   * image;
  int       bordersize;
};

struct ScegraPoints_ { 
  Point   point;
  BadList list;
};

struct ScegraNode_ {
  int             id;
  int             z;
  int             step;
  Rebox           box;
  ScegraStyle     style;
  ScegraPoints  * points;
  BadList         list;  
  USTR          * text;
  ScegraDraw    * draw;
  ScegraUpdate  * update;
};


/* Scene graph struct. */
struct Scegra_ {
  BadList * nodes;  
};

ScegraNode * scegranode_frombadlist(BadList * elem) {
  if (!elem) return NULL;
  return bad_container(elem, ScegraNode, list);
}


ScegraNode * 
scegranode_initall(ScegraNode * node, int id, Rebox box, ScegraStyle style, 
                   USTR * text, ScegraDraw * draw, ScegraUpdate * update) {
  if(!node) return NULL;
  node->id      = id;
  node->z       = id;
  node->box     = box;
  node->style   = style;
  node->text    = text;
  node->draw    = draw;
  node->update  = update;
  node->step    = 0;
  node->text    = ustr_dup(text);
  node->points  = NULL;
  badlist_initempty(&node->list);
  return node;
}

ScegraNode * 
scegranode_done(ScegraNode * self) {
  if (!self) return NULL;
  self->id = -1;
  if (self->text) ustr_free(self->text);
  // Todo, free node list 
  badlist_unlink(&self->list);
  return self;
}




Scegra * 
scegra_init(Scegra * self) {
  if (!self) return NULL;
  self->nodes = NULL;
  return self;
}

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














