#include "image.h"
#include "sprite.h"
#include "rebox.h"
#include "mem.h"
#include "bad.h"

#define SPRITEFRAME_OWNEDFLAG  1


/* A SpriteFrame is a single frame of animation of a sprite. 
 * It consists of a bounds rectangle, a image pointer, an index in the 
 * animation, and a duration in ms of the frame. The flags field 
 * registerrs if the image pointer is owned, etc.
 */
struct SpriteFrame_ {
  Point             at;  
  /* Current location of SpriteFrame relative to SpriteAction. */
  Rebox         bounds; 
  Image       * image;
  int           index;
  double        duration;
  int           flags;
};

/* Get Tarray interface, generate for SpriteFrame type */
#define TEMPLATE_T      SpriteFrame
#define TEMPLATE_NAME   SpriteFrameArray
#define TEMPLATE_PREFIX spriteframearray_
#include "tarray.h"

/* Get Tarray implementation, generate for SpriteFrame type */
#define TEMPLATE_IMPLEMENT
#define TEMPLATE_T      SpriteFrame
#define TEMPLATE_NAME   SpriteFrameArray
#define TEMPLATE_PREFIX spriteframearray_
#include "tarray.h"



struct SpriteAction_ {
  Point             at;  
  /* Current location of SpriteAction with regards to the sprite. */
  int index;
  int flags;
  SpriteFrameArray frames;
};


/* Get Tarray interface, generate for SpriteAction type */
#define TEMPLATE_T      SpriteAction
#define TEMPLATE_NAME   SpriteActionArray
#define TEMPLATE_PREFIX spriteactionarray_
#include "tarray.h"

/* Get Tarray implementation, generate for SpriteAction type */
#define TEMPLATE_IMPLEMENT
#define TEMPLATE_T      SpriteAction
#define TEMPLATE_NAME   SpriteActionArray
#define TEMPLATE_PREFIX spriteactionarray_
#include "tarray.h"

/* A sprite has several actions and frames. */
struct Sprite_ {
  Point             at;  
  /* Current location of Sprite. */
  Point             offset;
  /* Offset of child sprite to parent sprite. */
  Image           * sheet; 
  /* the sprite's sheet. It's an image "atlas" that holds all frames. */
  int               index;
  int               z;
  int               action; 
  int               frame;
  double            time;
  SpriteActionArray actions;
  BadChildtree      tree;
  /* Tree of child, sibling and parent sprites. */
  BadListNode       node;
  /* List node for listing drawing order. */
};


/* Get Tarray interface, generate for Sprite type */
#define TEMPLATE_T      Sprite
#define TEMPLATE_NAME   SpriteArray
#define TEMPLATE_PREFIX spritearray_
#include "tarray.h"

/* Get Tarray implementation, generate for Sprite type */
#define TEMPLATE_IMPLEMENT
#define TEMPLATE_T      Sprite
#define TEMPLATE_NAME   SpriteArray
#define TEMPLATE_PREFIX spritearray_
#include "tarray.h"


/** The list of all loaded sprites. Some may be drawn and some not. */
struct SpriteList_ {
  SpriteArray sprites;
}; 


#define SPRITE_BEGINACTIONS      8
#define SPRITEACTION_BEGINFRAME  4

SpriteFrame * spriteframe_initall(SpriteFrame * self, Point at, 
                                  Rebox box, Image * image, 
                                  int index, double duration, int flags) {
                                    
  if(!self) return NULL;
  self->at      = at;
  self->bounds  = box;
  self->image   = image;
  self->index   = index;
  self->duration= duration;
  self->flags   = flags;
  return self;
} 

SpriteFrame * spriteframe_alloc() {
  return STRUCT_ALLOC(SpriteFrame);
}

int spriteaction_frames(SpriteAction *self) {
  if(!self) return 0;
  return spriteframearray_size(&self->frames);
}

SpriteFrame * spriteaction_frame(SpriteAction *self, int index) {
  if(!self) return 0;
  return spriteframearray_getptr(&self->frames, index);
}


SpriteAction * spriteaction_init(SpriteAction * self, int index, int nframes, 
                                 int flags, Point at) {
  int aid;
  self->index  = index;
  spriteframearray_init(&self->frames, nframes); 
  self->flags  = flags;
  self->at     = at;
  for(aid = 0; aid < spriteaction_frames(self); aid++) {
    Point pzero = { 0.0, 0.0};
    Rebox bzero; 
    SpriteFrame * frame = spriteaction_frame(self, aid);
    rebox_init(&bzero, 0.0, 0.0, 0.0, 0.0);
    spriteframe_initall(frame, pzero, bzero, NULL, aid, 0.0, 0); 
  }
  return self;
}

int sprite_actions(Sprite *self) {
  if(!self) return 0;
  return spriteactionarray_size(&self->actions);
}

SpriteAction * sprite_action(Sprite *self, int index) {
  if(!self) return 0;
  return spriteactionarray_getptr(&self->actions, index);
}


int sprite_frames(Sprite *self, int action) {
  if(!self) return 0;
  return spriteaction_frames(sprite_action(self, action));
}

SpriteFrame * sprite_frame(Sprite *self, int action, int index) {
  if(!self) return 0;
  return spriteaction_frame(sprite_action(self, action), index);
}


Sprite * sprite_init(Sprite * self, int index, int nactions, Point at) {
  if (!self) return NULL;
  self->at      = at;
  self->index   = index;
  self->action  = 0; 
  self->frame   = 0;
  self->time    = 0.0; 
  spriteactionarray_init(&self->actions, nactions);   
  badchildtree_initnull(&self->tree);
  return self;
}









