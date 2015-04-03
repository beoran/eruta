
#include "eruta.h"
#include "mem.h"
#include "sprite.h"
#include "spritestate.h"
#include "bad.h"
#include "monolog.h"
#include "callrb.h"

/* Sprite state layer functions. */
SpriteStateLayer * spritestatelayer_init_empty(SpriteStateLayer * me) {
  if (!me) return NULL;
  me->flags = 0;
  me->tint  = al_map_rgba(1.0, 1.0, 1.0, 0.0);
  return me;
}

/* Sprite state action functions. */
SpriteStateAction * spritestateaction_init_empty(SpriteStateAction * me) {
  if (!me) return NULL;
  me->done          = FALSE;
  me->loop          = SPRITESTATE_ACTION_LOOP;
  me->next_action   = -1;
  return me;
}


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

  /* No cleanup of sprite as it is not owned by the sprite state. */
  return self->sprite;
}

/* Clean up the sprite state. 
 * No cleanup of the sprite itself since the Sprite * is owned by the sprite 
 * list.
*/
SpriteState * spritestate_done(SpriteState * self) { 
  int index;
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
  int index;
  
  if(!self) return NULL;
  spritestate_sprite_(self, sprite);
  self->speedup = 1.0;
    
  for (index = 0 ; index < SPRITESTATE_LAYER_MAX; index ++) {
    spritestatelayer_init_empty(self->layers + index);
  }
  
  for (index = 0 ; index < SPRITESTATE_ACTION_MAX; index ++) {
    spritestateaction_init_empty(self->actions + index);
  }
  
  
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
  if (!frame) return;  
  
  al_hold_bitmap_drawing(true);
  stop = spriteframe_maxlayers(frame);
  for (index = 0; index < stop ; index++) {
    if ((BIT_ISFLAG(me->layers[index].flags, SPRITESTATE_LAYER_HIDDEN))) {
      continue; /* Skip invisible layers. */
    }
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


/* Advances the frame of the sprite state if needed, depending on the 
*  looping and stop mode.
*/
void spritestate_next_frame(SpriteState * self) {
  int next = self->frame_index + 1;
  int action;
  int action_loop;
  
  /* Loop if needed. */
  if (next >= sprite_framesused(self->sprite, self->action_index)) {
    action_loop = spritestate_get_action_loop(self, self->action_index);
    /* It's a one-shot action. Do NOT cycle. */
    if (action_loop & SPRITESTATE_ACTION_ONESHOT) { 
      /* If it's a stop action, leave it there, otherwise go back to standing pose. */
      if (action_loop & SPRITESTATE_ACTION_STOP) {
        /* First time at end of a one shot stop action...  */
        if (!self->actions[self->action_index].done) {
          /* Notify scripting of end of this loop. */
          callrb_sprite_event(self, action_loop, NULL); 
          /* Set action to done toprevent double emitting the event above. */
          self->actions[self->action_index].done = TRUE;
        }
      } else {
        /* Go to back to first frame of standing pose with current direction. */
        action = sprite_action_index_for
          (self->sprite, SPRITE_WALK, self->direction_now);
        spritestate_pose_(self, SPRITE_WALK);
        spritestate_now_(self, action, 0);
        callrb_sprite_event(self, action_loop, NULL); 
        /* Action to done. */
        self->actions[self->action_index].done = TRUE;
      }
    } else {
      /* Loop back to first frame.*/
      spritestate_now_(self, self->action_index, 0);
      /* Set action to not done. */
      self->actions[self->action_index].done = FALSE;
    }
  } else {
    /* Advance to next frame. */
    spritestate_now_(self, self->action_index, next);
    /* Set action to not done. */
    self->actions[self->action_index].done = FALSE;
  }
}


/* Updates the spritestate. 
 * dt is the time passed since the last update in seconds (usuallu around 0.02). */
void spritestate_update(SpriteState * self, double dt) {
  Sprite * sprite;
  if (!self) return;
  sprite = self->sprite;
  if (!sprite) return;
  if(!self->frame_now) { 
    LOG_WARNING("NULL current sprite frame!: %d\n", self->action_index);
    // try to restore back to first frame if out of whack somehow.
    spritestate_now_(self, self->action_index, 0);
    return;
  }
  self->time += (self->speedup * dt);
  if(self->time > spriteframe_duration(self->frame_now)) {
    spritestate_next_frame(self);
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
    if (spriteaction_matches_pose(action, pose, direction)) {
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

/* Returns true if the layer or self are out of bounds, false if not. */
int spritestate_is_bad_layer(SpriteState * self, int layer) {
  if (!self) return TRUE;
  if (layer < 0) return TRUE;
  if (layer > SPRITESTATE_LAYER_MAX) return TRUE;
  return FALSE;
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

/* Sets the tint color for a layer of the sprite. */
int spritestate_tint_layer(SpriteState * self, int layer, Color color) {
  if (spritestate_is_bad_layer(self, layer)) return -1;
  self->layers[layer].tint = color;
  self->layers[layer].flags = BIT_SETFLAG(self->layers[layer].flags, SPRITESTATE_LAYER_TINTED);
  return layer;    
}

/** Unsets the tint color for a layer of the sprite. */
int spritestate_remove_tint_layer(SpriteState * self, int layer) {
  if (!spritestate_is_layer_tinted(self, layer)) return -1;
  self->layers[layer].flags = BIT_UNFLAG(self->layers[layer].flags, SPRITESTATE_LAYER_TINTED);
  return layer;    
}

/** Returns TRUE if the layer of the sprite is tinted, FALSE if not. */
int spritestate_is_layer_tinted(SpriteState * self, int layer) {
  if (spritestate_is_bad_layer(self, layer)) return FALSE;
  if (!(BIT_ISFLAG(self->layers[layer].flags, SPRITESTATE_LAYER_TINTED))) 
    return FALSE;
  return TRUE;
}

/** Returns apointer to the tint of the layer or NULL if not tinted or 
 * if the layer is out of bounds. */
Color * spritestate_get_layer_tint(SpriteState * self, int layer) {
  if (!spritestate_is_layer_tinted(self, layer)) return NULL;
  return &(self->layers[layer].tint);
}

/** Hides or unhides the given layer of the sprite. */
int spritestate_set_layer_hidden(SpriteState * self, int layer, int hidden) {
  if (hidden) {
    self->layers[layer].flags = BIT_SETFLAG(self->layers[layer].flags, SPRITESTATE_LAYER_HIDDEN);
  } else {
    self->layers[layer].flags = BIT_UNFLAG(self->layers[layer].flags, SPRITESTATE_LAYER_HIDDEN);    
  }
  return spritestate_get_layer_hidden(self, layer);
}

/** Returns true if the sprite's layer is hidden false if not. */
int spritestate_get_layer_hidden(SpriteState * self, int layer) {
  if (spritestate_is_bad_layer(self, layer)) return TRUE;
  return ((BIT_ISFLAG(self->layers[layer].flags, SPRITESTATE_LAYER_HIDDEN)));
}

/* Returns true if the action or self are out of bounds, false if not. */
int spritestate_is_bad_action(SpriteState * self, int action) {
  if (!self) return TRUE;
  if (action < 0) return TRUE;
  if (action > SPRITESTATE_ACTION_MAX) return TRUE;
  return FALSE;
}

/** Sets the loop mode for the given action of the sprite. */
int spritestate_set_action_loop(SpriteState * self, int action, int loopmode) {
  if (spritestate_is_bad_action(self, action)) return -1;
  self->actions[action].loop = loopmode;
  return self->actions[action].loop;
}

/** Gets the loop mode for the given action of the sprite. */
int spritestate_get_action_loop(SpriteState * self, int action) {
  if (spritestate_is_bad_action(self, action)) return -1;
  return self->actions[action].loop;
}

/** Returns true if the last set action is done, false if not. */
int spritestate_is_action_done(SpriteState * self, int action) {
  if (spritestate_is_bad_action(self, action)) return -1;
  return self->actions[action].done;
}

/** Sets the loop mode for the given pose and direction of the sprite. */
int spritestate_set_pose_direction_loop
  (SpriteState * self, int pose, int direction, int loopmode) {
  int action = sprite_action_index_for(self->sprite, pose, direction);
  return spritestate_set_action_loop(self, action, loopmode);
}

/** Sets the loop mode for the given pose and direction of the sprite. */
int spritestate_get_pose_direction_loop
  (SpriteState * self, int pose, int direction) {
  int action = sprite_action_index_for(self->sprite, pose, direction);
  return spritestate_get_action_loop(self, action);
}


