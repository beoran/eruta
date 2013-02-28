#include "image.h"
#include "sprite.h"
#include "rebox.h"
#include "mem.h"
#include "dynar.h"
#include "image.h"
#include "bad.h"
#include "flags.h"


#define SPRITEFRAME_OWNEDFLAG  1

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
struct SpriteData_ {
  SpriteAction     * action_now;
  SpriteFrame      * frame_now;
  int                frame_index;
  int                action_index;
  int                actions_used;
  double             time;
  Dynar            * actions;
};

/* The list of all loaded sprites. Some may be drawn and some not. */
struct SpriteList_ {
  Dynar * sprites;
  int     sprites_used;
};


/* SpriteState contains dynamic information about a sprite. This was   
 * separated out of SpriteDatato allow one sprites to be reused and shown by 
 * several Things. 
 */
struct SpriteState_ {
  SpriteData           * sprite;
  SpriteAction     * action_now;
  SpriteFrame      * frame_now;
  int                frame_index;
  int                action_index;
  double             time;
};





/* Allocates a sprite layer. */
SpriteLayer * 
spritelayer_alloc() {
  return STRUCT_ALLOC(SpriteLayer);
}

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

SpriteLayer * spritelayer_new(int index, Image * image, Point offset) {
  return spritelayer_initall(spritelayer_alloc(), index, image, offset);
} 

/* Draw a sprite layer with point at as the relevant game object's core position. */
void spritelayer_draw(SpriteLayer * self, Point * at) {
  Point real;
  if(!self) return;
  real = cpvadd((*at), self->offset);
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


#define SPRITE_BEGINLAYERS       8
#define SPRITE_BEGINACTIONS      8
#define SPRITE_BEGINFRAMES       4

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
                                   Image * image, Point offset) {
  SpriteLayer * layer = spritelayer_new(index, image, offset);
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
int spritedata_actionsused(SpriteData * sprite) {
  if (!sprite) return 0;
  return (sprite->actions_used);
}

/* Sets the used actions of a sprite. */
int spritedata_actionsused_(SpriteData * sprite, int used) {
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
int spritedata_layersused_(SpriteFrame * self, int used) {
  if (!self) return 0;
  return (self->layers_used = used);
}


int spritedata_maxactions(SpriteData *self) {
  if(!self) return 0;
  return dynar_size(self->actions);
}

SpriteAction * spritedata_action(SpriteData *self, int index) {
  if(!self) return 0;
  return dynar_getptr(self->actions, index);
}

/* Set an action to the sprite. Any old action at the 
 same index is freed. */
SpriteAction * spritedata_action_(SpriteData *self, int index, SpriteAction * action) {
  SpriteAction * oldact;
  if(!self) return 0;
  oldact = spritedata_action(self, index);
  if (oldact) {
    spriteaction_free(oldact);
  }
  if(dynar_putptr(self->actions, index, action)) { 
    return action;
  }
  return NULL;
}

int spritedata_frames(SpriteData *self, int action) {
  if(!self) return 0;
  return spriteaction_frames(spritedata_action(self, action));
}

/* Returns the sprite's index'th frame of the action'th action of this sprite,
 or NULL if no such action or frame. */
SpriteFrame * spritedata_frame(SpriteData *self, int action, int index) {
  if(!self) return 0;
  return spriteaction_frame(spritedata_action(self, action), index);
}


/** Returns a layer of the sprite that's at the given the action, frame,
 * and layer indexes of NULL if not found. */
SpriteLayer * spritedata_layer(SpriteData * self, int action ,int frame, int layer) {
  return spriteframe_layer(spritedata_frame(self, action, frame), layer);
} 
 
SpriteData * spritedata_initall(SpriteData * self, int index, int nactions) {
  int aid;
  if (!self) return NULL;
  // self->index        = index;
  self->action_now   = NULL;
  self->frame_now    = NULL;
  self->actions_used = 0;
  self->time         = 0.0; 
  self->actions      = dynar_newptr(nactions); 
  dynar_putnullall(self->actions);
  return self;
}


SpriteData * spritedata_init(SpriteData * self, int index) {
  return spritedata_initall(self, index,  SPRITE_NACTIONS_DEFAULT);
}


SpriteData * spritedata_done(SpriteData * self) {
  int aid;
  if(!self) return NULL;
  for (aid = 0; aid < spritedata_maxactions(self); aid++) {
    SpriteAction * act = spritedata_action(self, aid); 
    spriteaction_free(act);
  }   
  dynar_free(self->actions);
  self->actions    = NULL; 
  self->action_now = NULL;
  self->frame_now  = NULL;
  return self;
}

SpriteData * spritedata_free(SpriteData * self) {
  return mem_free(spritedata_done(self));
  return NULL;
}

SpriteData * spritedata_alloc() {
  return STRUCT_ALLOC(SpriteData);
}

SpriteData * spritedata_new(int index) {
  return spritedata_init(spritedata_alloc(), index);
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
SpriteAction * spritedata_newaction(SpriteData * self, int actionindex, int type, int flags) {
  SpriteAction * action, *aid;
  action = spriteaction_new(actionindex, type, flags);
  if(!action) return NULL;
  aid = spritedata_action_(self, actionindex, action);
  if (aid) return aid;
  /* Free if action could not be set. */
  return spriteaction_free(action);
}

/* Adds a frame to the sprite. The action must already exist. */
SpriteFrame * spritedata_newframe(SpriteData * self, int actionindex, int frameindex,
                              int flags, double duration) {
  SpriteAction * action;
  action = spritedata_action(self, actionindex);
  if (!action) return NULL;
  return spriteaction_newframe(action, frameindex, flags, duration);
}

/* Adds a layer to a sprite. The action and frame must already exist. */
SpriteLayer * spritedata_newlayer(SpriteData * self, int actionindex, int frameindex,
                              int layerindex, Image * image, Point offset) {
  SpriteFrame * frame;
  SpriteLayer * aid; 
  frame = spritedata_frame(self, actionindex, frameindex);
  if(!frame) return NULL;
  aid = spriteframe_newlayer(frame, layerindex, image, offset);
  return aid;
}


/* Returns the maximum amount of frames this sprite can currently have
 *for the given action index. */
int spritedata_maxframes(SpriteData * self, int actionindex) {
  SpriteAction * action;
  action = spritedata_action(self, actionindex);
  if (!action) return 0;
  return spriteaction_maxframes(action);  
}

/* Returns the amount of frames used frames of this sprite */
int spritedata_framesused(SpriteData * self, int actionindex) {
  SpriteAction * action;
  action = spritedata_action(self, actionindex);
  if (!action) return 0;
  return spriteaction_framesused(action);  
}


/* Returns the maximum amount of layers this sprite can currently have
 * for the given action and frame index. */
int spritedata_maxlayers(SpriteData * self, int actionindex, int frameindex) {
  SpriteFrame * frame;
  frame = spritedata_frame(self, actionindex, frameindex);
  if (!frame) return 0;
  return spriteframe_maxlayers(frame);  
}

/* SpriteData action cleanup walker. */
void * spriteaction_cleanwalk(void * ptr, void * extra) {
  SpriteAction * action = (SpriteAction *) ptr;
  if (!action) { return action; } 
  return spriteaction_free(action);
}

/* SpriteData action destructor. */
void * spriteaction_destructor(void * ptr) {
  return spriteaction_free((SpriteAction *) ptr);
} 

/* SpriteData frame destructor. */
void * spriteframe_destructor(void * ptr) {
  return spriteframe_free((SpriteFrame *) ptr);
} 

/* SpriteData layer destructor. */
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
SpriteData * spritedata_maxactions_(SpriteData * self, int newactions) {
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

/* Draw the sprite at the given location. This takes
  the current action, frame, layers and offsets into consideration. */
void spritedata_draw(SpriteData * self, Point * at) {
  if(!self) return;
  spriteframe_draw(self->frame_now, at);
}

/* Sets the sprite's current action and current frame. Returns 
 self if ok, NULL if out of bounds. */
SpriteData * 
spritedata_now_(SpriteData * self, int actionnow, int framenow) {
  SpriteAction * action;
  SpriteFrame  * frame;
  if (!self) return NULL;
  if (bad_outofboundsi(actionnow, 0, spritedata_maxactions(self))) return NULL; 
  frame = spritedata_frame(self, actionnow, framenow);
  if (!frame) {
    return NULL;
  }
  self->frame_index   = framenow;
  self->action_index  = actionnow;
  self->frame_now     = frame;
  self->time          = 0;
  return self;
}


/* Updates the sprite. dt is the time passed since the last update in doubles. */
void spritedata_update(SpriteData * self, double dt) {
  if (!self) return;  
  if(!self->frame_now) { 
    fprintf(stderr, "NULL current sprite frame!: %d\n", self->action_index);
    // try to restore back to first frame if out of whack somehow.
     spritedata_now_(self, self->action_index, 0);
    return;
  }
  self->time += dt;
  if(self->time > self->frame_now->duration) {
    int next = self->frame_index + 1;
    if (next >= spritedata_framesused(self, self->action_index)) {
      next = 0;
      self->frame_index = 0;
    }
    self->time = 0.0;
    spritedata_now_(self, self->action_index, next);
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


/* Sets the sprite current pose and direction. Will reset the frame to 0. 
 * Returns negative if the pose could not be found in this sprite;
 */
int spritedata_pose_(SpriteData * self, int pose, int direction) {
  int max, index;
  SpriteAction * action;
  max = spritedata_maxactions(self);
  for (index = 0; index < max; index ++) {
    action = spritedata_action(self, index);
    if (spriteaction_ispose(action, pose, direction)) {
      spritedata_now_(self, index, 0);
      return 0;
    }  
  }
  return -1;
};


/** Loads a layer of the sprite from the given image. 
* The layer's image will be duplicated. The indicated tile sizes and 
* locations will be used. Returns the layer on success of NULL if something 
* went wrong.
*/
SpriteLayer * spritedata_loadlayerfrom(SpriteData * self, int actionindex, 
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
  
  res = spritedata_newlayer(self, actionindex, frameindex, layerindex, aid, offset);
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
SpriteAction * spritedata_actiongetnew
(SpriteData * self, int actionindex, int actiontype, int actionflags) {
  SpriteAction * action = spritedata_action(self, actionindex);
  if (action) return action;
  return spritedata_newaction(self, actionindex, actiontype, actionflags);
}

/* Gets the frame for the sprite, or creates it if it doesn't exist yet. */
SpriteFrame * spritedata_framegetnew
(SpriteData * self, int actionindex, int frameindex, int flags, double duration) {
  SpriteFrame * frame = spritedata_frame(self, actionindex, frameindex);
  if (frame) return frame;
  return spritedata_newframe(self, actionindex, frameindex, SPRITE_ACTIVE, 0.2);
}

/* Loads a frame from an image, and creates it if it doesn't exist yet. */
SpriteLayer * spritedata_loadframelayerfrom
(SpriteData * self, int actionindex, int frameindex, int layerindex, 
 Image * source, Point size, Point where, int frameflags, double duration) {
  SpriteFrame * frame;  
  frame  = spritedata_framegetnew(
            self, actionindex, frameindex, SPRITE_ACTIVE, 0.2);
  if (!frame) return NULL;
  return spritedata_loadlayerfrom(self, actionindex, frameindex, layerindex,
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
SpriteData * spritedata_loadlayerlayout_standinwalk(SpriteData * self, 
                                            int layerindex, 
                                            Image * source, 
                                            int oversized, 
                                            SpriteLayout * layout) {
  return NULL;
}


/* Loads sprite layers for an action of a sprite from an image.  The layout info 
 * in the SpriteLayout struct determines how to load the sprite.
 */ 
SpriteData * spritelayout_loadactionlayer
(SpriteLayout * layout, SpriteData * sprite, 
 Image * source, int actionindex, int layerindex) {
    int frameindex;
    Point where           = cpv(0, layout->size_y * actionindex);
    Point size            = cpv(layout->size_x, layout->size_y);
    int inrow             = layout->per_row[actionindex];
    int actionflags       = layout->row_dir[actionindex];
    int actiontype        = layout->row_type[actionindex];
    int stand_in_walk     = ((actiontype == SPRITE_WALK) && (layout->standinwalk >= 0));
    SpriteAction * action = 
    spritedata_actiongetnew(sprite, actionindex, actiontype, actionflags);
    
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
        action = spritedata_actiongetnew(sprite, walkaction, SPRITE_STAND, actionflags);
        /* Load stand frame */
        ok = spritedata_loadframelayerfrom(sprite, walkaction, frameindex, 
              layerindex, source, size, where, SPRITE_ACTIVE, 2.0);
        stand_in_walk = 1;
        printf("Stand in walk found: %p %p at %d...\n", action, ok, walkaction);
        /* skip standing frames. */
      } else {
        ok = spritedata_loadframelayerfrom(sprite, actionindex, 
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
SpriteData * spritelayout_loadlayer
(SpriteLayout * layout, SpriteData * sprite, int layerindex, Image * source) {
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
SpriteData * spritedata_loadlayer_ulpcss
(SpriteData * self, int layerindex, Image * source, int oversized) {
  return spritelayout_loadlayer(&ulpcss_layout, self, layerindex, source);
} 

/** Loads sprite layer from file with the ulpcss layout. 
The file name is in FIFI vpath format (subdir of data) */
SpriteData * spritedata_loadlayer_ulpcss_vpath
(SpriteData * self, int layerindex, char * vpath, int oversized) {
  SpriteData * res; 
  Image * image;
  image = fifi_loadbitmap_vpath(vpath);
  if(!image) return NULL;
  res = spritedata_loadlayer_ulpcss(self, layerindex, image, oversized);
  al_destroy_bitmap(image);
  return res;
}

/* SpriteData state functions. */


SpriteState * spritestate_alloc() {
  return STRUCT_ALLOC(SpriteState);
}

SpriteState * spritestate_free(SpriteState * self) {
  /* No cleanup since the SpriteData * is owned by the sprite list. */
  return mem_free(self);
}

SpriteData * spritestate_spritedata_(SpriteState * self, SpriteData * sprite) {
  /* No cleanup, sprite is not owned by the sprite state. */
  self->sprite        = sprite;
  self->action_index  = 0;
  self->frame_index   = 0;
  self->action_now    = NULL;
  self->frame_now     = NULL; 
  self->time          = 0.0;
  return self->sprite;
}


SpriteData * spritestate_spritedata(SpriteState * self) {
  return self->sprite;
}

SpriteState * spritestate_init(SpriteState * self, SpriteData * sprite) {
  spritestate_spritedata_(self, sprite);
  return self;
}

/* Draws the sprite state at the given location.
   Draw the sprite at the given location. This takes
   the current action, frame, layers and offsets into consideration. */
void spritestate_draw(SpriteState * self, Point * at) {
  if(!self) return;
  spriteframe_draw(self->frame_now, at);
}

/* Sets the sprite states's current action and current frame. Returns 
 self if ok, NULL if out of bounds. */
SpriteState * 
spritestate_now_(SpriteState * self, int actionnow, int framenow) {
  SpriteAction * action;
  SpriteFrame  * frame;
  if (!self) return NULL;
  if (bad_outofboundsi(actionnow, 0, spritedata_maxactions(self->sprite))) 
    return NULL; 
  frame = spritedata_frame(self->sprite, actionnow, framenow);
  if (!frame) {
    return NULL;
  }
  self->frame_index   = framenow;
  self->action_index  = actionnow;
  self->frame_now     = frame;
  self->time          = 0.0;
  return self;
}

/* Updates the sprite state. dt is the time passed since the last update 
in seconds (normally in the orrer of 0.02s . */
void spritestate_update(SpriteState * self, double dt) {
  if (!self) return;  
  if(!self->frame_now) { 
    fprintf(stderr, "NULL current sprite frame!: %d\n", self->action_index);
    // try to restore back to first frame if out of whack somehow.
    spritestate_now_(self, self->action_index, 0);
    return;
  }
  self->time += dt;
  if(self->time > self->frame_now->duration) {
    int next = self->frame_index + 1;
    if (next >= spritedata_framesused(self->sprite, self->action_index)) {
      next = 0;
      self->frame_index = 0;
    }
    self->time = 0.0;
    spritedata_now_(self, self->action_index, next);
  }
}

/* Sets the sprite current pose and direction. Will reset the frame to 0. 
 * Returns negative if the pose could not be found in this sprite;
 */
int spritestate_pose_(SpriteState * self, int pose, int direction) {
  int max, index;
  SpriteAction * action;
  max = spritedata_maxactions(self->sprite);
  for (index = 0; index < max; index ++) {
    action = spritedata_action(self->sprite, index);
    if (spriteaction_ispose(action, pose, direction)) {
      spritestate_now_(self, index, 0);
      return 0;
    }  
  }
  return -1;
};



/* SpriteData list is a storage location for all sprites. 
 * 
 */






