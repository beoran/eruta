
#include "eruta.h"
#include "mem.h"
#include "sprite.h"
#include "spritestate.h"
#include "bad.h"


/* Sprite state functions. */

SpriteState * spritestate_alloc() {
  return STRUCT_ALLOC(SpriteState);
}

Sprite * spritestate_sprite_(SpriteState * self, Sprite * sprite) {
  int index;
  if(!self) return NULL;
  self->sprite          = sprite;
  self->action_index    = -1;
  self->action_now      = NULL;
  self->frame_now       = NULL;
  self->frame_index     = -1;
  self->time            = 0.0;
  self->pose_now        = SPRITE_STAND;
  self->direction_now   = SPRITE_ALL;
  for (index = 0; index < SPRITESTATE_LAYER_MAX; index++) {
    self->layer_tints[index]   = NULL;
  }
  
  /* No cleanup of sprite as it is not owned by the sprite state. */
  return self->sprite;
}

/* No cleanup of sprite since the Sprite * is owned by the sprite list. 
*/
SpriteState * spritestate_done(SpriteState * self) { 
  int index;
   
  for (index = 0; index < SPRITESTATE_LAYER_MAX; index++) {
    mem_free(self->layer_tints[index]);
  }
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


/* Draws the given layer of the sprite using the sprite state.   
 */
void spritestate_draw_frame(SpriteState * me, SpriteFrame * frame, Point * at) 
{
  int index, stop;
  SpriteCell * cell;
  Color * tint; 
  if (!me) return;
  al_hold_bitmap_drawing(true);
  stop = spriteframe_maxlayers(frame);
  for (index = 0; index < stop ; index++) {
    cell     = spriteframe_cell(frame, index);
    tint     = spritestate_get_layer_tint(me, index);
    if (tint) { 
      spritecell_draw_tinted(cell, at, (*tint));
    } else {
      spritecell_draw(cell, at);
    }
  }
  al_hold_bitmap_drawing(false);
}

/* Draw the SpriteState at the given location. This takes
  the current action, frame, layers and offsets into consideration. */
void spritestate_draw(SpriteState * self, Point * at) {
  if(!self) return;
  spritestate_draw_frame(self, self->frame_now, at);
}

/* Sets the spritestate's current action and current frame. Returns 
 self if ok, NULL if out of bounds. */
SpriteState * 
spritestate_now_(SpriteState * self, int actionnow, int framenow) {
  SpriteAction * action;
  SpriteFrame  * frame;
  Sprite       * sprite;
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
  if(self->time > spriteframe_duration(self->frame_now)) {
    int next = self->frame_index + 1;
    if (next >= sprite_framesused(sprite, self->action_index)) {
      next = 0;
      self->frame_index = 0;
    }
    self->time = 0.0;
    spritestate_now_(self, self->action_index, next);
  }
}


/* Sets the spritestate's current pose and direction. Will reset the frame to 0. 
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
    if (spriteaction_is_pose(action, pose, direction)) {
      spritestate_now_(self, index, 0);
      return index;
    }  
  }
  return -1;
}

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


int spritestate_tint_layer(SpriteState * self, int layer, Color color) {
  if (!self) return -1;
  if (layer < 0) return -2;
  if (layer > SPRITESTATE_LAYER_MAX) return -3;
  self->layer_tints[layer] = STRUCT_ALLOC(Color);
  if (!self->layer_tints[layer]) return -4;
  (*self->layer_tints[layer]) = color;
  return layer;    
}

int spritestate_remove_tint_layer(SpriteState * self, int layer) {
  if (!self) return -1;
  if (layer < 0) return -2;
  if (layer > SPRITESTATE_LAYER_MAX) return -3;
  mem_free(self->layer_tints[layer]);
  self->layer_tints[layer] = NULL;
  return layer;    
}


int spritestate_is_layer_tinted(SpriteState * self, int layer) {
  if (!self) return FALSE;
  if (layer < 0) return FALSE;
  if (layer > SPRITESTATE_LAYER_MAX) return FALSE;
  return (self->layer_tints[layer] != NULL);
}

Color * spritestate_get_layer_tint(SpriteState * self, int layer) {
  if (!self) return NULL;
  if (layer < 0) return NULL;
  if (layer > SPRITESTATE_LAYER_MAX) return NULL;
  return (self->layer_tints[layer]);
}



