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
  int               faction; 
  int               frame;
  double            time;
  SpriteActionArray actions;
  BadList           children;
  /* Child sprites, if any. Child sprites move in synch with their
   parent sprite. */
  Sprite          * parent;
  /* Parent sprite if any. */
};




#define SPRITE_BEGINACTIONS      8
#define SPRITEACTION_BEGINFRAME  4

SpriteFrame * spriteframe_initall(SpriteFrame * self, Rebox box, Image * image, 
                                  int index, double duration, int flags) {
                                    
  if(!self) return NULL;
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


SpriteAction * spriteaction_init(SpriteAction * self) {
  return self;
}







