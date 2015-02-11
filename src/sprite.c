#include "monolog.h"
#include "image.h"
#include "sprite.h"
#include "rebox.h"
#include "mem.h"
#include "dynar.h"
#include "image.h"
#include "bad.h"
#include "flags.h"
#include "fifi.h"

/* Define this to see how the sprites are being loaded. */
#define SPRITE_LOAD_DISPLAY
#define SPRITEFRAME_OWNEDFLAG  1

/* The tile height and width to be used when drawing a sprite. */
#define SPRITE_TILE_WIDE 32
#define SPRITE_TILE_HIGH 32

/* Sprite atlases could be faster, but they waste a lot of memory since 
 * sprites often have a lot of transparent space. Therefore, the Sprite system 
 * uses a non-atlas layered cell design. 
 * 
 * In this design, there is a single sprite list that contains a 
 * list of sprites. 
 * 
 * In the eruta engine sprites are a set of graphics that pertains to the same 
 * characer or object. 
 * 
 * Sprites only store the static graphical data and layout information of that 
 * character or object. Any dynamic information is stored in SpriteState 
 * which allows reuse of the same sprite with different physics simulation 
 * Things.
 * 
 * There are several orthogonal concerns with regards to sprites namely 
 * action, direction, animation, layering and drawing effects. 
 * 
 * Any but the simplest of sprites is animated. This means that the look of the 
 * sprite changes over time. Thi is achieved by displaying a different frame
 * of the sprite after a set amount of time passes.
 * 
 * Any but the simplest of characters may need to display different types of
 * animations. For example, the character may walk, attack, stand, etc. 
 * And each of these animations can have the character
 * face in different directions such as as east, west, north, south, etc.
 * In the Eruta engine, the different animations of a Sprite are called 
 * SpriteActions. A SpriteAction is characterized by it's direction and type, 
 * or "pose". 
 * 
 * For example there may be 4 actions with type/pose SPRITE_WALK each for 
 * the 4 direction SPRITE_NORTH, SPRITE_EAST, SPRITE_SOUTH, SPRITE_WEST. However
 * some actions have no direction (SPRITE_NO_DIRECTION) or are applicable 
 * for all directions (SPRITE_ALL_DIRECTIONS). 
 * 
 * Furthermore, to allow more flexibility and to limit art requirements, the 
 * frames of the animations may consist of several layers of clipped bitmaps 
 * drawn in order. Like this it becomes possible, for example, to keep the body
 * and different hairstyles in different image files and then make several 
 * character sprites by loading and superimposing those layers.   
 * In the Eruta engine such clipped bitmaps are called SpriteCells.
 * 
 * Finally, a sprite might be drawn with several effects in place such 
 * as coloration or rotation, although these belong more correctly to 
 * the state of the sprite.
 * 
 * Another concern is the contrast between loading and drawing the sprite.
 *
 * Loading Sprites
 * 
 * Sprites must be loaded from one or more bitmaps.
 * 
 * The simplest yet least convenient way is to put every cell of the sprite
 * into a different bitmap file and load every SpriteCell one at the time. 
 * 
 * Sprite sheets are more convenient. A sprite sheet contains one layer of 
 * the sprite. The sheet is filled with evenly-spaced cells in various poses,
 * directions and frames of animation. The layout of the SpriteCells may vary.
 * That is why SpriteLayout data is used to organize this in a flexible yet
 * fast way.
 * 
 * Since the order of loading the SpriteCells varies in the format of the input
 * the Sprite's data structure must allow SpriteCells to be loaded randomly.
 *   
 * However, to draw sprites efficiency is important. The internal structure 
 * needs to enable this.
 * 
 * Design question: What is then neccesary internally to draw a sprite? 
 * 1) It makes sense to have frames. When the sprite isn't updated though 
 * animation the same frame will be rendered many times before the update of
 * the sprite requires the next frame to be shown. 
 * 2) Frames must contain layered SpriteCells, preferrably in an array for fast
 * lookup. However, it isn't neccesarily so that frames must OWN those cells.
 * There might also be cells that are reused between the different animations
 * or frames. Hence, it makes sense to keep a simple dynamic list of cells
 * around that does own the cells. 
 *   
 * Crrently when a sprite is drawn, is drawn performing it's current action. 
 * Sprites every sprite has actions (animation series),
 * every series has frames, and every frame has layers of trimmed 
 * bitmaps. Each layer has an owned, trimmed bitmap with on consist of mostly 
 * empty space the relevant 
 * sprite fragment and their offset to the sprite proper.
 * 
 * Currently the organisation is Sprite > Action[*] > Frame[*] > Cell[32]
 * One problem with that is that it's a 3 pointer deep lookup. The second 
 * problem is that in almost all cases a sprite will be loaded in layer per 
 * layer order, but the display has to happen based on the order of the 
 * current sprite action.  
 * 
 *
 * 
 * 
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



/* A SpriteCell is a single layer of a single frame of animation of a sprite.
 size is NOT the current size of the image, but the original size of the layer's 
 frames. For example, a sprite can be loaded as having frame sizes of 64x64, however,
 due to cropping, the image in image may be smaller (and offset will be set to
 compensate this at drawing).
*/
struct SpriteCell_ {
  Image * image;
  Point   offset;
  Point   size;
  int     index;
  int     drawflags;
  int     tinted;
  Color   tint;
};


/* A SpriteFrame is a single frame of animation of a sprite.
 * It consists of a set of cells, a duration, flags, and the amount 
 * of used cells.  
 */
struct SpriteFrame_ {
  int                index;
  Dynar            * cells;
  int                cells_used;
  double             duration;
};

struct SpriteAction_ {
  int                index;
  int                type;
  int                directions;
  Dynar            * frames;
  int                frames_used;
  SpriteFrame      * frame_now;
};


/* A sprite has several actions and frames. */
struct Sprite_ {
  Dynar            * actions;
  int                actions_used;
  int                index;
};


/* Sprite Cells. */

/* Allocates a sprite cell. */
SpriteCell * 
spritecell_alloc() {
  return STRUCT_ALLOC(SpriteCell);
}

/* Initialize a sprite cell. */
SpriteCell *
spritecell_initall(SpriteCell * self, int index, Image * image, Point size, Point offset) {
  if (!self) return NULL;
  self->image           = image;
  self->index           = index;
  self->offset          = offset;
  self->size            = size;
  self->drawflags       = 0;
  self->tinted          = 0;
  self->tint            = al_map_rgb(255, 255, 255);
  return self;
}

SpriteCell * spritecell_new(int index, Image * image, Point size, Point offset) {
  return spritecell_initall(spritecell_alloc(), index, image, size, offset);
} 

/* Draw a sprite cell with point at as the relevant game object's core position. */
void spritecell_draw(SpriteCell * self, Point * at) {
  Point real, delta, aid;
  /*    - size_w/2 + tile_w / 2   */
  if(!self) return;
  delta = bevec(-self->size.x / 2 + SPRITE_TILE_WIDE / 2, 
              -self->size.y + SPRITE_TILE_HIGH);
  aid  = bevec_add((*at), self->offset);
  real = bevec_add(aid, delta);  
  /* Adjust for tile size and frame size. */
  al_draw_tinted_bitmap(self->image, self->tint, real.x, real.y, self->drawflags);
  
}

/* Cleans up a layer after use, also frees the layer's Image. */
SpriteCell * 
spritecell_done(SpriteCell * self) {
  if (!self) return NULL;
  if (self->image) al_destroy_bitmap(self->image);
  self->image           = NULL;
  self->drawflags       = 0;
  self->index           = -1;
  self->offset          = bevec(0.0, 0.0);
  return self;
}

/* Cleans up and frees memory used by a sprite cell.*/
SpriteCell * spritecell_free(SpriteCell * self) {
  spritecell_done(self);
  return mem_free(self);
}


SpriteCell *
spritecell_drawflags_(SpriteCell * self, int drawflags) {
  if (!self) return NULL;
  self->drawflags = drawflags;
  return self;
}

SpriteCell * 
spritecell_tint_(SpriteCell * self, Color tint) {
  if (!self) return NULL;  
  self->tinted = true;
  self->tint   = tint;
  return self;
} 


/* Sprite Frames. */


int spriteframe_maxlayers(SpriteFrame * self) {
  return dynar_size(self->cells);
}

int spriteframe_cells(SpriteFrame * self) {
  return self->cells_used;
}


SpriteFrame * spriteframe_init(SpriteFrame * self, 
                                int index, double duration, int ncells) {
  if (!self) return NULL;
  self->index           = index;
  self->cells           = dynar_newptr(ncells);
  dynar_putnullall(self->cells);
  self->cells_used      = 0;
  self->duration        = duration;
  return self;
} 

SpriteFrame * spriteframe_alloc() {
  return STRUCT_ALLOC(SpriteFrame);
}

/* Gets a cell for this frame or null if not availamle. */
SpriteCell * spriteframe_cell(SpriteFrame * self, int index) {
  if(!self) return NULL;
  return dynar_getptr(self->cells, index);  
}


SpriteFrame * spriteframe_done(SpriteFrame * self) {
  int index; 
  SpriteCell * layer;
  if(!self) return NULL;
  self->duration        = 0.0;
  self->cells_used     = -1;
  for (index = 0; index < dynar_size(self->cells); index++) {
    layer = spriteframe_cell(self, index);
    spritecell_free(layer);
  }
  dynar_free(self->cells);
  self->cells          = NULL;
  self->index           = -1;
  return self;
}

SpriteFrame * spriteframe_free(SpriteFrame * self) {
  spriteframe_done(self);
  return mem_free(self);
}

double spriteframe_duration(SpriteFrame * me) {
  if (!me) return -1.0;
  return me->duration;
}


/* Sets a Layer for this frame, frees the old layer if it was set. */
SpriteCell * spriteframe_cell_(SpriteFrame * self, int index, SpriteCell * layer) 
{
  SpriteCell * oldlayer;
  if(!self) return NULL;
  oldlayer = spriteframe_cell(self, index);
  spritecell_free(oldlayer);
  if(dynar_putptr(self->cells, index, layer)) { 
    return layer;
  }
  return NULL;
}

SpriteCell * spriteframe_newlayer(SpriteFrame * self, int index, 
                                   Image * image, Point size, Point offset) {
  SpriteCell * layer = spritecell_new(index, image, size, offset);
  SpriteCell * aid;
  aid = spriteframe_cell_(self, index, layer);
  if (aid) return aid;
  /* Could not set layer, free it */
  return spritecell_free(layer);
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
spriteaction_initall(SpriteAction * self, 
  int index, int type, int directions, int nframes) {
  
  int aid;
  self->index           = index;
  self->type            = type;
  self->frames          = dynar_new(nframes, sizeof(SpriteFrame *));
  self->frames_used     = 0;
  self->directions      = directions;
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

/* Returns nonzero if this action matches the pose and direction. */
int spriteaction_is_pose(SpriteAction * self, int pose, int direction) {
  if (!self)              return FALSE;
  if (self->type != pose) return FALSE;
  //return TRUE;
  return self->directions & direction;
}



SpriteFrame * spriteframe_newall(int index, double duration, int ncells) {
  return spriteframe_init(spriteframe_alloc(), index, duration, ncells);  
}

SpriteFrame * spriteframe_new(int index, double duration) {
  return spriteframe_newall(index, duration, SPRITEFRAME_NLAYERS_DEFAULT);  
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

/* Gets the used cells of a frame. */
int spriteframe_cells_used(SpriteFrame * self) {
  if (!self) return 0;
  return (self->cells_used);
}

/* Sets the used layers of a sprite. */
int sprite_cells_used_(SpriteFrame * self, int used) {
  if (!self) return 0;
  return (self->cells_used = used);
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
SpriteCell * sprite_layer(Sprite * self, int action ,int frame, int layer) {
  return spriteframe_cell(sprite_frame(self, action, frame), layer);
} 
 
Sprite * sprite_initall(Sprite * self, int index, int nactions) {
  int aid;
  if (!self) return NULL;
  self->actions_used = 0;
  self->actions      = dynar_newptr(nactions);
  self->index        = index; 
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
                                    double duration) {
  SpriteFrame * frame, * aid;
  frame = spriteframe_new(index, duration);
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
                              double duration) {
  SpriteAction * action;
  action = sprite_action(self, actionindex);
  if (!action) return NULL;
  return spriteaction_newframe(action, frameindex, duration);
}

/* Adds a layer to a sprite. The action and frame must already exist. */
SpriteCell * sprite_newlayer(Sprite * self, int actionindex, int frameindex,
                              int layerindex, Image * image, Point size, 
                              Point offset) {
  SpriteFrame * frame;
  SpriteCell * aid; 
  frame = sprite_frame(self, actionindex, frameindex);
  if(!frame) return NULL;
  aid = spriteframe_newlayer(frame, layerindex, image, size, offset);
  if ((size.x == 0) || (size.x > 64.0)) { 
  LOG_NOTE("New sprite cell %p %d %d %d %p, (%f, %f), (%f, %f)\n",
    self, actionindex, frameindex, layerindex, image, size.x, size.y, offset.x, offset.y
  );
  }
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
  (void) extra;
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
void * spritecell_destructor(void * ptr) {
  return spritecell_free((SpriteCell *) ptr);
} 


/* Change the amount of cells the sprite frame can have. Returns self 
 on success, null if failed. */
SpriteFrame * spriteframe_maxcells_(SpriteFrame * self, int newcells) {
  int last, toclean, index;
  Dynar * aid;
  if(!self) return NULL;
  aid = dynar_resize(self->cells, newcells, spritecell_destructor);  
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
  SpriteCell * layer;
  if (!self) return;
  al_hold_bitmap_drawing(true);
  stop = spriteframe_maxlayers(self);
  for (index = 0; index < stop ; index++) {
    layer = spriteframe_cell(self, index);
    spritecell_draw(layer, at);
    if (index == SPRITELAYER_WEAPONS) {
      Color col = al_map_rgb(255,125,64);
      al_draw_rectangle(at->x, at->y, at->x+10, at->y+10, col, 3); 
    }
  }
  al_hold_bitmap_drawing(false);
}


/** Loads a layer of the sprite from the given image. 
* The layer's image will be duplicated. The indicated tile sizes and 
* locations will be used. Returns the layer on success of NULL if something 
* went wrong.
*/
SpriteCell * sprite_loadlayerfrom(Sprite * self, int actionindex, 
                                  int frameindex, int layerindex, 
                                  Image * source, Point size, Point where) {
  SpriteCell * res;
  Point offset;
  Image * aid;
  ALLEGRO_COLOR black, glass, white;
  black = al_map_rgba(0,0,0,255);
  white = al_map_rgba(255,255,255,255);
  glass = al_map_rgba(0,0,0,0);
  // al_set_new_bitmap_flags
  aid = al_create_bitmap(size.x, size.y);
  if(!aid) { 
    fprintf(stderr, "Cannot allocate bitmap for: %d %d %d\n", actionindex, frameindex, layerindex);
    return NULL;
  }  
  offset = bevec(0,0); 
  al_set_target_bitmap(aid);
  al_clear_to_color(glass);  
  al_draw_bitmap_region(source, where.x, where.y, size.x, size.y, 0, 0, 0);
  #ifdef SPRITE_LOAD_DISPLAY
  al_draw_rectangle(0, 0, size.x, size.y, white, 2);
  #endif
  
  al_set_target_backbuffer(al_get_current_display());
  #ifdef SPRITE_LOAD_DISPLAY
  { int x, y;
  al_clear_to_color(black);
  x = 0; // -32 + al_get_display_width(al_get_current_display())  / 2;
  y = 0; // -32 + al_get_display_height(al_get_current_display()) / 2;
  
  al_draw_bitmap(aid, x, y, 0);
  al_draw_rectangle(x, y, size.x, size.y, white, 1);
  al_flip_display(); 
  } 
  #endif
  // usleep(100000);
  
  res = sprite_newlayer(self, actionindex, frameindex, layerindex, aid, size, offset);
  if(!res) {
    LOG_ERROR("Could not make new sprite cell: %d\n", layerindex);
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

/* Layout info of the ULPCSS sprites. */
static SpriteLayout ulpcss_layout = {
  ulpcss_sprites_per_row, ulpcss_row_type, ulpcss_row_direction,
  64, 64, 0
}; 


/* Layout info of the oversized ULPCSS sprites, for weapons only. */
static int ulpcss_oversized_sprites_per_row[] = {
  6, 6, 6, 6,  
  -1 
};

/* Type info of the oversized ULPCSS sprites, for slashinhg weapons. */
static int ulpcss_oversized_slash_row_type[] = {
  SPRITE_SLASH, SPRITE_SLASH, SPRITE_SLASH, SPRITE_SLASH,
  -1 
};

/* Type info of the ULPCSS sprites, for stabbing weapons. */
static int ulpcss_oversized_stab_row_type[] = {
  SPRITE_STAB, SPRITE_STAB, SPRITE_STAB, SPRITE_STAB,
  -1 
};


/* Direction info of the oversized ULPCSS sprites. */
static int ulpcss_oversized_row_direction[] = {
  SPRITE_NORTH , SPRITE_WEST , SPRITE_SOUTH, SPRITE_EAST,
  -1
};

/* Layout of an oversized slashing weapon ULPCSS sprite. */
static SpriteLayout ulpcss_oversized_slash_layout = {
  ulpcss_oversized_sprites_per_row, ulpcss_oversized_slash_row_type,
  ulpcss_oversized_row_direction,
  64 * 3, 64 * 3, 0
}; 

/* Layout of an oversized stabbing weapon ULPCSS sprite. */
static SpriteLayout ulpcss_oversized_stab_layout = {
  ulpcss_oversized_sprites_per_row, ulpcss_oversized_stab_row_type,
  ulpcss_row_direction,
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
  (void) flags;
  return sprite_newframe(self, actionindex, frameindex, duration);
}

/* Loads a frame from an image, and creates it if it doesn't exist yet. */
SpriteCell * sprite_loadframelayerfrom
(Sprite * self, int actionindex, int frameindex, int layerindex, 
 Image * source, Point size, Point where, int frameflags, double duration) {
  SpriteFrame * frame;  
  frame  = sprite_framegetnew(
            self, actionindex, frameindex, SPRITE_ACTIVE, duration);
  if (!frame) return NULL;
  (void) frameflags;
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
Sprite * sprite_loadlayerlayout_standinwalk(Sprite * self, 
                                            int layerindex, 
                                            Image * source, 
                                            int oversized, 
                                            SpriteLayout * layout) {
  return NULL;
}
*/

/* Loads sprite cells for an action of a sprite from an image.  The layout info 
 * in the SpriteLayout struct determines how to load the sprite.
 */ 
Sprite * spritelayout_loadactionlayer
(SpriteLayout * layout, Sprite * sprite, 
 Image * source, int actionindex, int layerindex) {
    int frameindex;
    Point where           = bevec(0, layout->size_y * actionindex);
    Point size            = bevec(layout->size_x, layout->size_y);
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
      SpriteCell * ok;
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
              layerindex, source, size, where, SPRITE_ACTIVE, 0.2);
        stand_in_walk = 1;
        /* skip standing frames. */
      } else {
        ok = sprite_loadframelayerfrom(sprite, actionindex, 
                                       frameindex - stand_in_walk, 
              layerindex, source, size, where, SPRITE_ACTIVE, 0.2);
        if (!ok) { 
        fprintf(stderr, 
                "Error loading sprite cell! %d %d\n", 
                actionindex, frameindex);
        }
      }
      where.x    += layout->size_x;
    }
  return sprite;
}


/* Loads a sprite cell for the whole sprite.  The layout info in the struct is 
 * used to correctly set  up the sprite parts. 
 */ 
Sprite * spritelayout_loadlayer
(SpriteLayout * layout, Sprite * sprite, int layerindex, Image * source) {
  int stop;
  int actionindex, walkaction;

  actionindex = 0;
  stop        = spritelayout_actions(layout);
  walkaction  = stop - 1;

  for (actionindex = 0 ; actionindex < stop ; actionindex++ ) {
    spritelayout_loadactionlayer(layout, sprite, source, actionindex, layerindex);
  }
  return sprite;
}

/** Returns a layout for a load type. Returns NULL on error or unknown type. */
SpriteLayout * sprite_layout_for(int load_type) {
  switch (load_type) {
    case SPRITE_LOAD_ULPCSS_NORMAL:
      return &ulpcss_layout;
    case SPRITE_LOAD_ULPCSS_OVERSIZED_SLASH:
      return &ulpcss_oversized_slash_layout;
    case SPRITE_LOAD_ULPCSS_OVERSIZED_STAB:
      return &ulpcss_oversized_stab_layout;
    default:
      return NULL;
  }
}

/** Loads a sprite cell from file with the given layout data. 
The file name is in FIFI vpath format (subdir of data) */
Sprite * sprite_loadlayer_vpath
(Sprite * self, SpriteLayout * layout, int layerindex, char * vpath) {
  Sprite * res; 
  Image * image;
  image = fifi_loadbitmap_vpath(vpath);
  if(!image) return NULL;
  res = spritelayout_loadlayer(layout, self, layerindex, image);
  al_destroy_bitmap(image);
  return res;
}


/** Loads sprite cell with the ulpcss layout. */
Sprite * sprite_loadlayer_ulpcss
(Sprite * self, int layerindex, Image * source, int load_type) {
  SpriteLayout * layout;
  layout = sprite_layout_for(load_type);
  if (!layout) return NULL; 
  return spritelayout_loadlayer(layout, self, layerindex, source);
} 

/** Loads an ULPCSS sprite cell from file with the given load type. 
The file name is in FIFI vpath format (subdir of data) */
Sprite * sprite_loadlayer_ulpcss_vpath
(Sprite * self, int layerindex, char * vpath, int load_type) {
  Sprite * res; 
  Image * image;
  image = fifi_loadbitmap_vpath(vpath);
  if(!image) return NULL;
  res = sprite_loadlayer_ulpcss(self, layerindex, image, load_type);
  al_destroy_bitmap(image);
  return res;
}

/* Applies a tint to a whole layer of a sprite */
Sprite * sprite_tintlayer(Sprite * self, int layerindex, Color color) {
  int actionstop, framestop;
  int actionindex, frameindex;
  SpriteAction * action;
  SpriteFrame * frame;
  SpriteCell * layer;
  actionstop        =  dynar_size(self->actions);
  for (actionindex  = 0 ; actionindex < actionstop ; actionindex++ ) {
    action          = sprite_action(self, actionindex);
    if(!action) continue;
    framestop       = dynar_size(action->frames);
    for (frameindex = 0 ; frameindex < framestop ; frameindex++) {
      frame         = spriteaction_frame(action, frameindex);
      if(!frame) continue;
      layer         = spriteframe_cell(frame, layerindex);
      if(!layer) continue;
      spritecell_tint_(layer, color);
    }
  }
  return self; 
}


/** Sprite cleanup walker */
void * sprite_cleanup_walker(void * data, void * extra) {
  Sprite * sprite = (Sprite *) data;
  sprite_free(data);
  (void) extra;
  return NULL;
}


/* Sprite list is a storage location for all sprites. 
 * 
 */








