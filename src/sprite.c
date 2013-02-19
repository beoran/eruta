#include "image.h"
#include "sprite.h"
#include "rebox.h"
#include "mem.h"
#include "dynar.h"
#include "image.h"
#include "bad.h"


#define SPRITEFRAME_OWNEDFLAG  1

/* To use the sprites from LPC as placeholders, 
 and also for the real sprites, which will have a similar layout, 
 I 'll try a non-atlas layered fragment approach first. 
 In this approach every sprite has actions (animation series),
 every series has frames, and every frame has layers of trimmed 
 layers. Each layer has an owned, trimmed bitmap with only the relevant 
 sprite fragment and their offset to the sprite proper.
*/


/* A SpriteLayer is a single layer of a single frame of animation of a sprite */
struct SpriteLayer_ {
  Point   offset;
  Image * image;
  int     index;
  int     drawflags;
};


/* A SpriteFrame is a single frame of animation of a sprite.
 * It consists of a set of layers, a duration, flags, and the almount of used layers.  
 */
struct SpriteFrame_ {
  int                index;
  Dynar            * layers;
  int                used_layers;
  double             duration;
  int                flags;
};

struct SpriteAction_ {
  int                index;
  int                flags;
  Dynar            * frames;
  int                used_frames;
  SpriteFrame      * frame_now;
};


/* A sprite has several actions and frames. */
struct Sprite_ {
  SpriteAction     * action_now;
  SpriteFrame      * frame_now;
  int                frame_index;
  int                action_index;
  int                actions_used;
  double             time;
  Dynar            * actions;
  int used_actions;
};

/** The list of all loaded sprites. Some may be drawn and some not. */
struct SpriteList_ {
  Dynar * sprites;
  int     sprites_used;
}; 

/* Initialize a sprite layer. */
SpriteLayer *
spritelayer_initall(SpriteLayer * self, int index, Image * image, Point offset) {
  if (!self) return NULL;
  self->image           = image;
  self->index           = index;
  self->offset          = offset;
  self->drawflags       = 0;
  return self;
}

/* Draw a sprite layer with point at as the relevant game object's core position. */
void spritelayer_draw(SpriteLayer * self, Point * at) {
  Point real;
  if(!self) return;
  real = cpvadd((*at), self->offset);
  al_draw_bitmap(self->image, real.x, real.y, self->drawflags);
}

SpriteLayer * 
spritelayer_done(SpriteLayer * self) {
  if(!self) return NULL;
  al_destroy_bitmap(self->image);
  self->image           = NULL;
  self->drawflags       = 0;
  self->index           = -1;
  self->offset          = point(0.0, 0.0);
  return self;
}

SpriteLayer *
spritelayer_drawflags_(SpriteLayer * self, int drawflags) {
  if (!self) return NULL;
  self->drawflags = drawflags;
  return self;
}


int spriteframe_maxlayers(SpriteFrame * self) {
  return dynar_size(self->layers);
}

int spriteframe_layers(SpriteFrame * self) {
  return self->used_layers;
}


#define SPRITE_BEGINLAYERS       8
#define SPRITE_BEGINACTIONS      8
#define SPRITE_BEGINFRAMES       4

SpriteFrame * spriteframe_init(SpriteFrame * self, 
                                int index, int nlayers, int flags,
                                double duration) {
  if (!self) return NULL;
  self->index           = index;
  self->layers          = dynar_new(nlayers, sizeof(SpriteLayer *));
  self->used_layers     = 0;
  self->duration        = duration;
  self->flags           = flags;
  return self;
} 

SpriteFrame * spriteframe_alloc() {
  return STRUCT_ALLOC(SpriteFrame);
}

SpriteFrame * spriteframe_done(SpriteFrame * self) {
  int index; 
  SpriteLayer * layer;
  if(!self) return NULL;
  self->flags           = 0;
  self->duration        = 0.0;
  self->used_layers     = -1;
  for (index = 0; index < dynar_size(self->layers) ; index) {
    layer = dynar_getraw(self->layers, index);
    spritelayer_done(layer);
  }
  dynar_free(self->layers);
  self->layers          = NULL;
  self->index           = -1;
  return self;
}

SpriteFrame * spriteframe_free(SpriteFrame * self) {
  spriteframe_done(self);
  return NULL;
}



int spriteaction_maxframes(SpriteAction *self) {
  if(!self) return 0;
  return dynar_size(self->frames);
}

int spriteaction_frames(SpriteAction *self) {
  if(!self) return 0;
  return self->used_frames;
}

SpriteFrame * 
spriteaction_frame(SpriteAction *self, int index) {
  if(!self) return 0;
  return dynar_getptr(self->frames, index);
}

SpriteFrame * 
spriteaction_frame_(SpriteAction *self, int index, SpriteFrame * frame) {
  if(!self) return 0;
  return dynar_putptr(self->frames, index, frame);
}


SpriteAction * 
spriteaction_init(SpriteAction * self, int index, int flags, int nframes) {
  int aid;
  self->index           = index;
  self->frames          = dynar_new(nframes, sizeof(SpriteFrame *));
  self->used_frames     = 0;
  self->flags           = flags;
  self->frame_now       = NULL;
  for (aid = 0; aid < spriteaction_frames(self); aid++) {
    spriteaction_frame_(self, aid, NULL); 
  }
  return self;
}


SpriteAction * 
spriteaction_done(SpriteAction * self) {
  int aid;
  if(!self) return NULL;
  for (aid = 0; aid < spriteaction_frames(self); aid++) {
    SpriteFrame * frame = spriteaction_frame(self, aid);
    spriteframe_free(frame); 
  }
  dynar_free(self->frames);
  self->frames = NULL;
  self->frame_now = NULL;
  return self;
}

SpriteAction * spriteaction_free(SpriteAction * self) {
  spriteaction_done(self);
  return mem_free(self);
}


SpriteAction * spriteaction_alloc() {
  return STRUCT_ALLOC(SpriteAction);
}

SpriteLayer * spritelayer_alloc() {
  return STRUCT_ALLOC(SpriteLayer);
}

SpriteAction * spriteaction_new(int index, int nframes, int flags) {
  return spriteaction_init(spriteaction_alloc(), index, nframes, flags);
}

SpriteFrame * spriteframe_new(int index, int nlayers, int flags, double duration) {
  return spriteframe_init(spriteframe_alloc(), index, nlayers, flags, duration);  
}




int sprite_maxactions(Sprite *self) {
  if(!self) return 0;
  return dynar_size(self->actions);
}

SpriteAction * sprite_action(Sprite *self, int index) {
  if(!self) return 0;
  return dynar_getptr(self->actions, index);
}

SpriteAction * sprite_action_(Sprite *self, int index, SpriteAction * action) {
  if(!self) return 0;
  return dynar_getptr(self->actions, index);
}


int sprite_frames(Sprite *self, int action) {
  if(!self) return 0;
  return spriteaction_frames(sprite_action(self, action));
}

SpriteFrame * sprite_frame(Sprite *self, int action, int index) {
  if(!self) return 0;
  return spriteaction_frame(sprite_action(self, action), index);
}


Sprite * sprite_init(Sprite * self, int index, int nactions) {
  int aid;
  if (!self) return NULL;
  // self->index        = index;
  self->action_now   = NULL;
  self->frame_now    = NULL;
  self->used_actions = 0;
  self->time         = 0.0; 
  self->actions      = dynar_new(nactions, sizeof(SpriteAction *));   
  for (aid = 0; aid < sprite_maxactions(self); aid++) {
    sprite_action_(self, aid, NULL);
  }   
  return self;
}


Sprite * sprite_done(Sprite * self) {
  int aid;
  if(!self) return NULL;
  for (aid = 0; aid < sprite_maxactions(self); aid++) {
    SpriteAction * act = sprite_action(self, aid); 
    spriteaction_free(act);
  }   
  dynar_free(self->actions);
  self->actions    = NULL; 
  self->action_now = NULL;
  self->frame_now  = NULL;
  return self;
}

Sprite * sprite_free(Sprite * self) {
  return mem_free(sprite_done(self));
  return NULL;
}

Sprite * sprite_alloc() {
  return STRUCT_ALLOC(Sprite);
}

Sprite * sprite_new(int index, int nactions) {
  return sprite_init(sprite_alloc(), index, nactions);
}










