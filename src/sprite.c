#include "image.h"
#include "sprite.h"
#include "rebox.h"
#include "mem.h"
#include "dynar.h"
#include "image.h"
#include "bad.h"
#include "flags.h"


#define SPRITEFRAME_OWNEDFLAG  1

/* The tile height and width to be used when drawing a sprite. */
#define SPRITE_TILE_WIDE 32
#define SPRITE_TILE_HIGH 32

/* To use the sprites from LPC as placeholders, 
 and also for the real sprites, which will have a similar layout, 
 I 'll try a non-atlas layered fragment approach first. 
 In this approach every sprite has actions (animation series),
 every series has frames, and every frame has layers of trimmed 
 layers. Each layer has an owned, trimmed bitmap with only the relevant 
 sprite fragment and their offset to the sprite proper.
*/


/* ULPCSS has the following layers:
 * accessories
 * behind_body (quiver)
 * belt
 * body 
 * eyes (in body layer)
 * feet
 * hair
 * hands
 * head
 * legs
 * torso (however, there seem to be overlap possibilities)
 * weapons
 * 
 * All in all, having 32 possible layers is more than enough it seems, at 
 * least for now. 
 * 
 */



/* A SpriteLayer is a single layer of a single frame of animation of a sprite.
 size is NOT the current size of the image, but the original size of the layer's 
 frames. For example, a sprite can be loaded as having frame sizes of 64x64, however,
 due to cropping, the image in image may be smaller (and offset will be set to
 compensate this at drawing).
*/
struct SpriteLayer_ {
  Image * image;
  Point   offset;
  Point   size;
  int     index;
  int     drawflags;
};


/* A SpriteFrame is a single frame of animation of a sprite.
 * It consists of a set of layers, a duration, flags, and the almount of used layers.  
 */
struct SpriteFrame_ {
  int                index;
  Dynar            * layers;
  int                layers_used;
  double             duration;
  int                flags;
};

struct SpriteAction_ {
  int                index;
  int                type;
  int                flags;
  Dynar            * frames;
  int                frames_used;
  SpriteFrame      * frame_now;
};


/* A sprite has several actions and frames. */
struct Sprite_ {
  Dynar            * actions;
  int                actions_used;
};

/* The list of all loaded sprites. Some may be drawn and some not. */
struct SpriteList_ {
  Dynar * sprites;
  int     sprites_used;
};






/* Allocates a sprite layer. */
SpriteLayer * 
spritelayer_alloc() {
  return STRUCT_ALLOC(SpriteLayer);
}

/* Initialize a sprite layer. */
SpriteLayer *
spritelayer_initall(SpriteLayer * self, int index, Image * image, Point size, Point offset) {
  if (!self) return NULL;
  self->image           = image;
  self->index           = index;
  self->offset          = offset;
  self->size            = size;
  self->drawflags       = 0;
  return self;
}

SpriteLayer * spritelayer_new(int index, Image * image, Point size, Point offset) {
  return spritelayer_initall(spritelayer_alloc(), index, image, size, offset);
} 

/* Draw a sprite layer with point at as the relevant game object's core position. */
void spritelayer_draw(SpriteLayer * self, Point * at) {
  Point real, delta, aid;
  /*    - size_w/2 + tile_w / 2   */
  if(!self) return;
  delta = cpv(-self->size.x / 2 + SPRITE_TILE_WIDE / 2, 
              -self->size.y + SPRITE_TILE_HIGH);
  aid  = cpvadd((*at), self->offset);
  real = cpvadd(aid, delta);  
  /* Adjust for tile size and frame size. */
  al_draw_bitmap(self->image, real.x, real.y, self->drawflags);
}

/* Cleans up a layer after use, also frees the layer's Image. */
SpriteLayer * 
spritelayer_done(SpriteLayer * self) {
  if (!self) return NULL;
  if (self->image) al_destroy_bitmap(self->image);
  self->image           = NULL;
  self->drawflags       = 0;
  self->index           = -1;
  self->offset          = point(0.0, 0.0);
  return self;
}

/* Cleans up and frees memory used by a sprite layer.*/
SpriteLayer * spritelayer_free(SpriteLayer * self) {
  spritelayer_done(self);
  return mem_free(self);
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
  return self->layers_used;
}


SpriteFrame * spriteframe_init(SpriteFrame * self, 
                                int index, int flags,
                                double duration, int nlayers) {
  if (!self) return NULL;
  self->index           = index;
  self->layers          = dynar_newptr(nlayers);
  dynar_putnullall(self->layers);
  self->layers_used     = 0;
  self->duration        = duration;
  self->flags           = flags;
  return self;
} 

SpriteFrame * spriteframe_alloc() {
  return STRUCT_ALLOC(SpriteFrame);
}

/* Gts a layer for this frame or null if not availamle. */
SpriteLayer * spriteframe_layer(SpriteFrame * self, int index) {
  if(!self) return NULL;
  return dynar_getptr(self->layers, index);  
}


SpriteFrame * spriteframe_done(SpriteFrame * self) {
  int index; 
  SpriteLayer * layer;
  if(!self) return NULL;
  self->flags           = 0;
  self->duration        = 0.0;
  self->layers_used     = -1;
  for (index = 0; index < dynar_size(self->layers); index++) {
    layer = spriteframe_layer(self, index);
    spritelayer_free(layer);
  }
  dynar_free(self->layers);
  self->layers          = NULL;
  self->index           = -1;
  return self;
}

SpriteFrame * spriteframe_free(SpriteFrame * self) {
  spriteframe_done(self);
  return mem_free(self);
}


/* Sets a Layer for this frame, frees the old layer if it was set. */
SpriteLayer * spriteframe_layer_(SpriteFrame * self, int index, SpriteLayer * layer) 
{
  SpriteLayer * oldlayer;
  if(!self) return NULL;
  oldlayer = spriteframe_layer(self, index);
  spritelayer_free(oldlayer);
  if(dynar_putptr(self->layers, index, layer)) { 
    return layer;
  }
  return NULL;
}

SpriteLayer * spriteframe_newlayer(SpriteFrame * self, int index, 
                                   Image * image, Point size, Point offset) {
  SpriteLayer * layer = spritelayer_new(index, image, size, offset);
  SpriteLayer * aid;
  aid = spriteframe_layer_(self, index, layer);
  if (aid) return aid;
  /* Could not set layer, free it */
  return spritelayer_free(layer);
}


int spriteaction_maxframes(SpriteAction *self) {
  if(!self) return 0;
  return dynar_size(self->frames);
}

int spriteaction_frames(SpriteAction *self) {
  if(!self) return 0;
  return self->frames_used;
}

SpriteFrame * 
spriteaction_frame(SpriteAction *self, int index) {
  if(!self) return 0;
  return dynar_getptr(self->frames, index);
}

/* Sets a frame for this action, frees the old frame if it was set. 
 Returns frame set or NULL if it could not be set. 
 */
SpriteFrame * 
spriteaction_frame_(SpriteAction *self, int index, SpriteFrame * frame) {
  SpriteFrame * oldframe;
  if(!self) return 0;
  oldframe = spriteaction_frame(self, index);
  spriteframe_free(oldframe);
  if (dynar_putptr(self->frames, index, frame)) { 
    return frame;
  } 
  return NULL;
}


SpriteAction * 
spriteaction_initall(SpriteAction * self, int index, int type, int flags, int nframes) {
  int aid;
  self->index           = index;
  self->type            = type;
  self->frames          = dynar_new(nframes, sizeof(SpriteFrame *));
  self->frames_used     = 0;
  self->flags           = flags;
  self->frame_now       = NULL;
  dynar_putnullall(self->frames);
  return self;
}

SpriteAction * 
spriteaction_init(SpriteAction * self, int index, int type, int flags) {
  return spriteaction_initall(self, index, type, flags, SPRITEACTION_NFRAMES_DEFAULT);
}

SpriteAction * 
spriteaction_done(SpriteAction * self) {
  int aid;
  if(!self) return NULL;
  for (aid = 0; aid < spriteaction_maxframes(self); aid++) {
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

SpriteAction * spriteaction_newall(int index, int type, int flags, int nframes) {
  return spriteaction_initall(spriteaction_alloc(), index, type, flags, nframes);
}

SpriteAction * spriteaction_new(int index, int type, int flags) {
  return spriteaction_init(spriteaction_alloc(), index, type, flags);
}

SpriteFrame * spriteframe_newall(int index, int flags, double duration, int nlayers) {
  return spriteframe_init(spriteframe_alloc(), index, flags, duration, nlayers);  
}

SpriteFrame * spriteframe_new(int index, int flags, double duration) {
  return spriteframe_newall(index, flags, duration, SPRITEFRAME_NLAYERS_DEFAULT);  
}


/* Gets the used actions of a sprite. */
int sprite_actionsused(Sprite * sprite) {
  if (!sprite) return 0;
  return (sprite->actions_used);
}

/* Sets the used actions of a sprite. */
int sprite_actionsused_(Sprite * sprite, int used) {
  if (!sprite) return 0;
  return (sprite->actions_used = used);
}


/* Gets the used frames of a sprite action. */
int spriteaction_framesusedold(SpriteAction * self) {
  if (!self) return 0;
  return (self->frames_used);
}

/* Sets the used frames of a sprite action. */
int spriteaction_framesused(SpriteAction * self) {
  int index ;
  if (!self) return 0;
  for(index = 0; index < spriteaction_maxframes(self) ; index++) {
    if(!spriteaction_frame(self, index)) return index;
  }
  return index;
}

/* Gets the used layers of a frame. */
int spriteframe_layersused(SpriteFrame * self) {
  if (!self) return 0;
  return (self->layers_used);
}

/* Sets the used layers of a sprite. */
int sprite_layersused_(SpriteFrame * self, int used) {
  if (!self) return 0;
  return (self->layers_used = used);
}


int sprite_maxactions(Sprite *self) {
  if(!self) return 0;
  return dynar_size(self->actions);
}

SpriteAction * sprite_action(Sprite *self, int index) {
  if(!self) return 0;
  return dynar_getptr(self->actions, index);
}

/* Set an action to the sprite. Any old action at the 
 same index is freed. */
SpriteAction * sprite_action_(Sprite *self, int index, SpriteAction * action) {
  SpriteAction * oldact;
  if(!self) return 0;
  oldact = sprite_action(self, index);
  if (oldact) {
    spriteaction_free(oldact);
  }
  if(dynar_putptr(self->actions, index, action)) { 
    return action;
  }
  return NULL;
}

int sprite_frames(Sprite *self, int action) {
  if(!self) return 0;
  return spriteaction_frames(sprite_action(self, action));
}

/* Returns the sprite's index'th frame of the action'th action of this sprite,
 or NULL if no such action or frame. */
SpriteFrame * sprite_frame(Sprite *self, int action, int index) {
  if(!self) return 0;
  return spriteaction_frame(sprite_action(self, action), index);
}


/** Returns a layer of the sprite that's at the given the action, frame,
 * and layer indexes of NULL if not found. */
SpriteLayer * sprite_layer(Sprite * self, int action ,int frame, int layer) {
  return spriteframe_layer(sprite_frame(self, action, frame), layer);
} 
 
Sprite * sprite_initall(Sprite * self, int index, int nactions) {
  int aid;
  if (!self) return NULL;
  self->actions_used = 0;
  self->actions      = dynar_newptr(nactions); 
  dynar_putnullall(self->actions);
  return self;
}


Sprite * sprite_init(Sprite * self, int index) {
  return sprite_initall(self, index,  SPRITE_NACTIONS_DEFAULT);
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
  return self;
}

Sprite * sprite_free(Sprite * self) {
  return mem_free(sprite_done(self));
  return NULL;
}

Sprite * sprite_alloc() {
  return STRUCT_ALLOC(Sprite);
}

Sprite * sprite_new(int index) {
  return sprite_init(sprite_alloc(), index);
}

/* Adds a new frame to an action. Any old frame at the same index is freed. 
 Returns the new frame or NULL on error. */
SpriteFrame * spriteaction_newframe(SpriteAction * self, int index, 
                                    int flags, double duration) {
  SpriteFrame * frame, * aid;
  frame = spriteframe_new(index, flags, duration);
  if(!frame) return NULL;
  aid = spriteaction_frame_(self, index, frame);
  /* Free if could not set. */
  if (aid) return aid;
  return spriteframe_free(frame);
}

/* Adds a new action to the sprite. Any old action at the 
 same index is freed. Returns the new action or NULL on error. */
SpriteAction * sprite_newaction(Sprite * self, int actionindex, int type, int flags) {
  SpriteAction * action, *aid;
  action = spriteaction_new(actionindex, type, flags);
  if(!action) return NULL;
  aid = sprite_action_(self, actionindex, action);
  if (aid) return aid;
  /* Free if action could not be set. */
  return spriteaction_free(action);
}

/* Adds a frame to the sprite. The action must already exist. */
SpriteFrame * sprite_newframe(Sprite * self, int actionindex, int frameindex,
                              int flags, double duration) {
  SpriteAction * action;
  action = sprite_action(self, actionindex);
  if (!action) return NULL;
  return spriteaction_newframe(action, frameindex, flags, duration);
}

/* Adds a layer to a sprite. The action and frame must already exist. */
SpriteLayer * sprite_newlayer(Sprite * self, int actionindex, int frameindex,
                              int layerindex, Image * image, Point size, 
                              Point offset) {
  SpriteFrame * frame;
  SpriteLayer * aid; 
  frame = sprite_frame(self, actionindex, frameindex);
  if(!frame) return NULL;
  aid = spriteframe_newlayer(frame, layerindex, image, size, offset);
  return aid;
}


/* Returns the maximum amount of frames this sprite can currently have
 *for the given action index. */
int sprite_maxframes(Sprite * self, int actionindex) {
  SpriteAction * action;
  action = sprite_action(self, actionindex);
  if (!action) return 0;
  return spriteaction_maxframes(action);  
}

/* Returns the amount of frames used frames of this sprite */
int sprite_framesused(Sprite * self, int actionindex) {
  SpriteAction * action;
  action = sprite_action(self, actionindex);
  if (!action) return 0;
  return spriteaction_framesused(action);  
}


/* Returns the maximum amount of layers this sprite can currently have
 * for the given action and frame index. */
int sprite_maxlayers(Sprite * self, int actionindex, int frameindex) {
  SpriteFrame * frame;
  frame = sprite_frame(self, actionindex, frameindex);
  if (!frame) return 0;
  return spriteframe_maxlayers(frame);  
}

/* Sprite action cleanup walker. */
void * spriteaction_cleanwalk(void * ptr, void * extra) {
  SpriteAction * action = (SpriteAction *) ptr;
  if (!action) { return action; } 
  return spriteaction_free(action);
}

/* Sprite action destructor. */
void * spriteaction_destructor(void * ptr) {
  return spriteaction_free((SpriteAction *) ptr);
} 

/* Sprite frame destructor. */
void * spriteframe_destructor(void * ptr) {
  return spriteframe_free((SpriteFrame *) ptr);
} 

/* Sprite layer destructor. */
void * spritelayer_destructor(void * ptr) {
  return spritelayer_free((SpriteLayer *) ptr);
} 


/* Change the amount of layers the sprite frame can have. Returns self 
 on success, null if failed. */
SpriteFrame * spriteframe_maxlayers_(SpriteFrame * self, int newlayers) {
  int last, toclean, index;
  Dynar * aid;
  if(!self) return NULL;
  aid = dynar_resize(self->layers, newlayers, spritelayer_destructor);  
  if(!aid) return NULL;
  return self;
}


/* Change the amount of frames the sprite action can have. Returns self 
 on success, null if failed. */
SpriteAction * spriteaction_maxframes_(SpriteAction * self, int newframes) {
  int last, toclean, index;
  Dynar * aid;
  if(!self) return NULL;
  aid = dynar_resize(self->frames, newframes, spriteframe_destructor);  
  if(!aid) return NULL;
  return self;
}


/* Change the amount of actions the sprite can have. Returns self 
 on success, null if failed. */
Sprite * sprite_maxactions_(Sprite * self, int newactions) {
  int last, toclean, index;
  Dynar * aid;
  if(!self) return NULL;
  aid = dynar_resize(self->actions, newactions, spriteaction_destructor);  
  if(!aid) return NULL;
  return self;
}

/* Draws the sprite frame at the given location. */
void spriteframe_draw(SpriteFrame * self, Point * at) {
  int index, stop;
  SpriteLayer * layer;
  if (!self) return;
  stop = spriteframe_maxlayers(self);
  for (index = 0; index < stop ; index++) {
    layer = spriteframe_layer(self, index);
    spritelayer_draw(layer, at);
  }
}


/** Loads a layer of the sprite from the given image. 
* The layer's image will be duplicated. The indicated tile sizes and 
* locations will be used. Returns the layer on success of NULL if something 
* went wrong.
*/
SpriteLayer * sprite_loadlayerfrom(Sprite * self, int actionindex, 
                                  int frameindex, int layerindex, 
                                  Image * source, Point size, Point where) {
  SpriteLayer * res;
  Point offset;
  Image * aid;
  ALLEGRO_COLOR black, glass;
  black = al_map_rgba(0,0,0,255);
  glass = al_map_rgba(0,0,0,0);
  // al_set_new_bitmap_flags
  aid = al_create_bitmap(size.x, size.y);
  if(!aid) { 
    fprintf(stderr, "Cannot allocate bitmap for: %d %d %d\n", actionindex, frameindex, layerindex);
    return NULL;
  }  
  offset = cpv(0,0); 
  al_set_target_bitmap(aid);
  al_clear_to_color(glass);  
  al_draw_bitmap_region(source, where.x, where.y, size.x, size.y, 0, 0, 0);
  al_set_target_backbuffer(al_get_current_display());
  #ifdef SPRITE_LOAD_DISPLAY
  al_clear_to_color(black);
  al_draw_bitmap(aid, 
                 -32 + al_get_display_width(al_get_current_display()) / 2, 
                 -32 + al_get_display_height(al_get_current_display()) / 2,
                 0);
  al_flip_display();
  #endif
  // usleep(100000);
  
  res = sprite_newlayer(self, actionindex, frameindex, layerindex, aid, size, offset);
  if(!res) {
    al_destroy_bitmap(aid);
    return NULL;
  }
  return res;
}

/* Layout info of the ULPCSS sprites. */
static int ulpcss_sprites_per_row[] = {
  7, 7, 7, 7,  
  8, 8, 8, 8,  
  9, 9, 9, 9,  
  6, 6, 6, 6,  
  13, 13, 13, 13,  
  6, 
  -1 
};

/* Type info of the ULPCSS sprites. */
static int ulpcss_row_type[] = {
  SPRITE_CAST , SPRITE_CAST , SPRITE_CAST , SPRITE_CAST ,
  SPRITE_STAB , SPRITE_STAB , SPRITE_STAB , SPRITE_STAB ,
  SPRITE_WALK , SPRITE_WALK , SPRITE_WALK , SPRITE_WALK ,
  SPRITE_SLASH, SPRITE_SLASH, SPRITE_SLASH, SPRITE_SLASH,
  SPRITE_SHOOT, SPRITE_SHOOT, SPRITE_SHOOT, SPRITE_SHOOT,
  SPRITE_DOWN ,
  -1 
};

/* Direction info of the ULPCSS sprites. */
static int ulpcss_row_direction[] = {
  SPRITE_NORTH , SPRITE_WEST , SPRITE_SOUTH, SPRITE_EAST,
  SPRITE_NORTH , SPRITE_WEST , SPRITE_SOUTH, SPRITE_EAST,
  SPRITE_NORTH , SPRITE_WEST , SPRITE_SOUTH, SPRITE_EAST,
  SPRITE_NORTH , SPRITE_WEST , SPRITE_SOUTH, SPRITE_EAST,
  SPRITE_NORTH , SPRITE_WEST , SPRITE_SOUTH, SPRITE_EAST,
  SPRITE_ALL  ,
  -1
};


static SpriteLayout ulpcss_layout = {
  ulpcss_sprites_per_row, ulpcss_row_type, ulpcss_row_direction,
  64, 64, 0
}; 

static SpriteLayout ulpcss_oversized_layout = {
  ulpcss_sprites_per_row, ulpcss_row_type, ulpcss_row_direction,
  64 * 3, 64 * 3, 0
}; 


/* Gets the action for the sprite, or creates it if it doesn't exist yet. */
SpriteAction * sprite_actiongetnew
(Sprite * self, int actionindex, int actiontype, int actionflags) {
  SpriteAction * action = sprite_action(self, actionindex);
  if (action) return action;
  return sprite_newaction(self, actionindex, actiontype, actionflags);
}

/* Gets the frame for the sprite, or creates it if it doesn't exist yet. */
SpriteFrame * sprite_framegetnew
(Sprite * self, int actionindex, int frameindex, int flags, double duration) {
  SpriteFrame * frame = sprite_frame(self, actionindex, frameindex);
  if (frame) return frame;
  return sprite_newframe(self, actionindex, frameindex, SPRITE_ACTIVE, 0.2);
}

/* Loads a frame from an image, and creates it if it doesn't exist yet. */
SpriteLayer * sprite_loadframelayerfrom
(Sprite * self, int actionindex, int frameindex, int layerindex, 
 Image * source, Point size, Point where, int frameflags, double duration) {
  SpriteFrame * frame;  
  frame  = sprite_framegetnew(
            self, actionindex, frameindex, SPRITE_ACTIVE, 0.2);
  if (!frame) return NULL;
  return sprite_loadlayerfrom(self, actionindex, frameindex, layerindex,
            source, size, where);
}



/* Amount of actions in a layout. */
int spritelayout_actions(SpriteLayout * layout) {
  int index;
  if(!layout) return 0;
  if(!layout->per_row) return 0;
  for(index = 0; layout->per_row[index] >= 0; index ++) {}
  return index;
}


/** Loads the stand-in-walk layers for sprites with layouts that have them. 
* It's ugly but it's needed because of how the ULPCSS sprite sheets are 
* organized...
*/
Sprite * sprite_loadlayerlayout_standinwalk(Sprite * self, 
                                            int layerindex, 
                                            Image * source, 
                                            int oversized, 
                                            SpriteLayout * layout) {
  return NULL;
}


/* Loads sprite layers for an action of a sprite from an image.  The layout info 
 * in the SpriteLayout struct determines how to load the sprite.
 */ 
Sprite * spritelayout_loadactionlayer
(SpriteLayout * layout, Sprite * sprite, 
 Image * source, int actionindex, int layerindex) {
    int frameindex;
    Point where           = cpv(0, layout->size_y * actionindex);
    Point size            = cpv(layout->size_x, layout->size_y);
    int inrow             = layout->per_row[actionindex];
    int actionflags       = layout->row_dir[actionindex];
    int actiontype        = layout->row_type[actionindex];
    int stand_in_walk     = ((actiontype == SPRITE_WALK) && (layout->standinwalk >= 0));
    SpriteAction * action = 
    sprite_actiongetnew(sprite, actionindex, actiontype, actionflags);
    
    if (!action) { 
      fprintf(stderr, "Error allocating sprite action %d!\n", actionindex);
      return NULL;
    }
    /* Load frames. */
    for (frameindex = 0; frameindex < inrow; frameindex++) {
      SpriteFrame * frame;
      SpriteLayer * ok;
      /* 
        Special case for stand-in-walk. 
      */      
      if (stand_in_walk  && (frameindex == layout->standinwalk)) {
        /* Make an action for the stand-in-walk frame. */
        int walkaction = spritelayout_actions(layout) + actionindex % 4; 
        /* XXX: It's a bit of a hack, assumes that there will be 4 consecutive walk 
         * actions. */
        action = sprite_actiongetnew(sprite, walkaction, SPRITE_STAND, actionflags);
        /* Load stand frame */
        ok = sprite_loadframelayerfrom(sprite, walkaction, frameindex, 
              layerindex, source, size, where, SPRITE_ACTIVE, 2.0);
        stand_in_walk = 1;
        printf("Stand in walk found: %p %p at %d...\n", action, ok, walkaction);
        /* skip standing frames. */
      } else {
        ok = sprite_loadframelayerfrom(sprite, actionindex, 
                                       frameindex - stand_in_walk, 
              layerindex, source, size, where, SPRITE_ACTIVE, 2.0);
        if (!ok) { 
        fprintf(stderr, 
                "Error loading sprite layer! %d %d\n", 
                actionindex, frameindex);
        }
      }
      where.x    += layout->size_x;
    }
  return sprite;
}


/* Loads a sprite layer for the whole sprite.  The layout info in the struct is 
 * used to correctly set  up the sprite parts. 
 */ 
Sprite * spritelayout_loadlayer
(SpriteLayout * layout, Sprite * sprite, int layerindex, Image * source) {
  int stop;
  int actionindex, walkaction;
  Point where, size;
  actionindex = 0;
  stop        = spritelayout_actions(layout);
  walkaction  = stop - 1;

  for (actionindex = 0 ; actionindex < stop ; actionindex++ ) {
    spritelayout_loadactionlayer(layout, sprite, source, actionindex, layerindex);
  }
  return sprite;
}


/** Loads sprite layer with the ulpcss layout. */
Sprite * sprite_loadlayer_ulpcss
(Sprite * self, int layerindex, Image * source, int oversized) {
  SpriteLayout * layout;
  layout = (oversized ? &ulpcss_oversized_layout : &ulpcss_layout); 
  return spritelayout_loadlayer(layout, self, layerindex, source);
} 

/** Loads sprite layer from file with the ulpcss layout. 
The file name is in FIFI vpath format (subdir of data) */
Sprite * sprite_loadlayer_ulpcss_vpath
(Sprite * self, int layerindex, char * vpath, int oversized) {
  Sprite * res; 
  Image * image;
  image = fifi_loadbitmap_vpath(vpath);
  if(!image) return NULL;
  res = sprite_loadlayer_ulpcss(self, layerindex, image, oversized);
  al_destroy_bitmap(image);
  return res;
}




/* Sprite list is a storage location for all sprites. 
 * 
 */


/* Sprite state functions. */

SpriteState * spritestate_alloc() {
  return STRUCT_ALLOC(SpriteState);
}

Sprite * spritestate_sprite_(SpriteState * self, Sprite * sprite) {
  if(!self) return NULL;
  self->sprite          = sprite;
  self->action_index    = -1;
  self->action_now      = NULL;
  self->frame_now       = NULL;
  self->frame_index     = -1;
  self->time            = 0.0;
  self->pose_now        = SPRITE_STAND;
  self->direction_now   = SPRITE_ALL;
  /* No cleanup, sprite is not owned by the sprite state. */
  return self->sprite;
}

  /* No cleanup since the Sprite * is owned by the sprite list. 
  This function exists for future extensio only and should be called to 
  prepare for any owned pointers if those turn out to be needed.
  */
SpriteState * spritestate_done(SpriteState * self) { 
  return self;
} 

SpriteState * spritestate_free(SpriteState * self) {
  spritestate_done(self);
  return mem_free(self);
}


Sprite * spritestate_sprite(SpriteState * self) {
  if(!self) return NULL;
  return self->sprite;
}

SpriteState * spritestate_init(SpriteState * self, Sprite * sprite) {
  if(!self) return NULL;
  spritestate_sprite_(self, sprite);
  self->speedup = 1.0;
  return self;
}


double spritestate_speedup(SpriteState * self) {
  if (!self) return 0.0;
  return self->speedup;
}

double spritestate_speedup_(SpriteState * self, double speedup) {
  if (!self) return 0.0;
  return self->speedup = speedup;
}

SpriteState * spritestate_new(Sprite *  sprite) {
  return spritestate_init(spritestate_alloc(), sprite);
}


/* Draw the SpriteState at the given location. This takes
  the current action, frame, layers and offsets into consideration. */
void spritestate_draw(SpriteState * self, Point * at) {
  if(!self) return;
  spriteframe_draw(self->frame_now, at);
}

/* Sets the spritestate's current action and current frame. Returns 
 self if ok, NULL if out of bounds. */
Sprite * 
spritestate_now_(SpriteState * self, int actionnow, int framenow) {
  SpriteAction * action;
  SpriteFrame  * frame;
  Sprite * sprite;
  if (!self) return NULL;
  sprite = self->sprite;
  if (!sprite) return NULL;
  if (bad_outofboundsi(actionnow, 0, sprite_maxactions(sprite))) return NULL; 
  frame = sprite_frame(sprite, actionnow, framenow);
  if (!frame) { return NULL; }
  self->frame_index   = framenow;
  self->action_index  = actionnow;
  self->frame_now     = frame;
  self->time          = 0.0;
  return self;
}


/* Updates the spritestate. 
 * dt is the time passed since the last update in seconds (usuallu around 0.02). */
void spritestate_update(SpriteState * self, double dt) {
  Sprite * sprite;
  if (!self) return;
  sprite = self->sprite;
  if (!sprite) return;
  if(!self->frame_now) { 
    fprintf(stderr, "NULL current sprite frame!: %d\n", self->action_index);
    // try to restore back to first frame if out of whack somehow.
     spritestate_now_(self, self->action_index, 0);
    return;
  }
  self->time += (self->speedup * dt);
  if(self->time > self->frame_now->duration) {
    int next = self->frame_index + 1;
    if (next >= sprite_framesused(sprite, self->action_index)) {
      next = 0;
      self->frame_index = 0;
    }
    self->time = 0.0;
    spritestate_now_(self, self->action_index, next);
  }
}

/* Returns nonzero if this action matches the pose and direction. */
int spriteaction_ispose(SpriteAction * self, int pose, int direction) {
  if (!self)              return FALSE;
  if (self->type != pose) return FALSE;
  //return TRUE;
  printf("Flags: %d & %d\n", self->flags, direction);
  return self->flags & direction;
}


/* Sets the spritestate current pose and direction. Will reset the frame to 0. 
 * Returns negative if the pose could not be found in this sprite;
 * Otherwise returns the index of the used sprite action.
 * Returns -2 without effect if the pose and direction are identical to the ones
 * currently set.
 */
int spritestate_posedirection_(SpriteState * self, int pose, int direction) {
  int max, index;
  SpriteAction * action;
  Sprite * sprite;
  if (!self)    return -1;
  sprite = self->sprite;
  if (!sprite)  return -1;
  if ((self->pose_now == pose) && (self->direction_now == direction)) {
    /* Do nothing if the pose and ditr are not actually changed. */
    return -2;
  }
  
  self->pose_now        = pose;
  self->direction_now   = direction;
  max = sprite_maxactions(sprite);
  for (index = 0; index < max; index ++) {
    action = sprite_action(sprite, index);
    if (spriteaction_ispose(action, pose, direction)) {
      spritestate_now_(self, index, 0);
      return index;
    }  
  }
  return -1;
};

/* Changes the direction but keeps the pose. 
 * No effect if direction is the current active one.*/
int spritestate_direction_(SpriteState * self, int direction) {
  if (!self) return -1; 
  return spritestate_posedirection_(self, self->pose_now, direction);
}

/* Changes the pose but keeps the direction. 
 * No effect if pose is the current active one.
 */
int spritestate_pose_(SpriteState * self, int pose) {
  if (!self) return -1; 
  return spritestate_posedirection_(self, pose, self->direction_now);
}



/* Returns the current active pose of the sprite state.*/
int spritestate_pose(SpriteState * self) {
  if (!self) return SPRITE_STAND; 
  return self->pose_now;
}

/* Returns the current active direction of the sprite state.*/
int spritestate_direction(SpriteState * self) {
  if (!self) return SPRITE_NO_DIRECTION; 
  return self->direction_now;
}


/** Sprite cleanup walker */
void * sprite_cleanup_walker(void * data, void * extra) {
  Sprite * sprite = (Sprite *) data;
  sprite_free(data);
  return NULL;
}

/* Sprite list functions. */

/* Allocate a sprite list. */
SpriteList * spritelist_alloc() {
  return STRUCT_ALLOC(SpriteList);
}

SpriteList * spritelist_initall(SpriteList * self, int maxsprites) {
  if (!self) return NULL;
  self->sprites         = dynar_newptr(maxsprites);
  self->sprites_used    = 0;
  dynar_putnullall(self->sprites);
  return self;
}

SpriteList * spritelist_init(SpriteList * self) {
  return spritelist_initall(self, SPRITELIST_NSPRITES_DEFAULT);
}

SpriteList * spritelist_done(SpriteList * self) {
  if (!self) return NULL;
  /* Clean up using a walker callback. */
  dynar_walkptr(self->sprites, sprite_cleanup_walker, NULL);
  self->sprites      = dynar_free(self->sprites);
  self->sprites_used = 0;
  return self;
}

SpriteList * spritelist_new() {
  return spritelist_init(spritelist_alloc());
}


/* Frees the memory associated to the sprite list and all sprites in it. */
SpriteList * spritelist_free(SpriteList * self) {
  return mem_free(spritelist_done(self));
}

/* Gets a sprite from a sprite list. Returns NULL if not found;*/
Sprite * spritelist_sprite(SpriteList * self, int index)  {
  if (!self) return NULL;
  return dynar_getptr(self->sprites, index);
}

/* Puts a sprite into a sprite list. If there was a sprite there already it will 
 * be freed */
Sprite * spritelist_sprite_(SpriteList * self, int index, Sprite * sprite) {
  Sprite * old;
  if (!self) return NULL;
  old = spritelist_sprite(self, index);
  sprite_free(old);
  dynar_putptr(self->sprites, index, sprite);  
  return sprite;
}

/* Makes a new sprite and puts in the sprite list at the given index. */
Sprite * spritelist_newsprite(SpriteList * self, int index) {
  Sprite * sprite;
  if (!self) return NULL;
  sprite = sprite_new(index);
  if(!sprite) return NULL;
  return spritelist_sprite_(self, index, sprite);
}

/* Makes a new sprite if it doesn't exist yet or otherwise returns the old one 
 at the given index. */
Sprite * spritelist_getornew(SpriteList * self, int index) {
  Sprite * sprite = spritelist_sprite(self, index);
  if (sprite) return sprite;
  return spritelist_newsprite(self, index);
} 

/* Loads a sprite layer in ulpcss format into the sprite with 
 the given index in the sprite list. If no sprite exists yet,
 it will be created. The sprite s returned on sucess of NULL on failure. 
*/

Sprite * spritelist_loadlayer_ulpcss_vpath(
  SpriteList * self, int index,  int layerindex, char * vpath) { 
  Sprite * sprite;
  int oversize = FALSE;
  if (!self) return NULL;
  if (strstr(vpath, "oversize")) { oversize = TRUE; } 
  sprite = spritelist_getornew(self, index);
  if (!sprite) return NULL;
  if (!sprite_loadlayer_ulpcss_vpath(sprite, layerindex, vpath, oversize)) 
    return NULL;
  return sprite;
}







