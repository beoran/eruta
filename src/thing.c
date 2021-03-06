
/*
 A Thing is anything that "physically exists" in an Area, that is, a "level". 
*/



#include "thing.h"
#include "area.h"
#include "flags.h"
#include "draw.h"
#include "mem.h"
#include "monolog.h"
#include "thing_struct.h"



#define THING_WALL_MASS   INFINITY
#define THING_ACTOR_MASS  1.0


/** Gets the ID of the thing. Returns negative on error or for a
thing that doesn't belong to an area yet.*/
int thing_id(Thing * thing) {
  if (!thing) return -3;
  return thing->id;
}

/** Sets the ID of the thing. Returns negative on error or 
for a thing that doesn't belong to an area yet (as inicated by newid) 
*/
int thing_id_(Thing * thing, int newid) {
  if (!thing) return -3;
  return thing->id = newid;
}


/** Sets the z value of the Thing. This influences which layer it
and, if set, it's Shape is in. Logically,
there are 4 layers in every area, 2 bottom ones and 2 top ones. 
Layer 0 is for the floor or walls itself, layer 1 for things that
are standing or lying on the floor, or for edges,
layer 2 is for things on the "top floor", that is, another 
floor above the current one which may be used for bridges, 
overpasess, etc. Layer 3 is then for things standing
or lying on that top layer.
*/
Thing * thing_z_(Thing * self, int z) {
  if (!self) return NULL;
  self->z = z;
  if(self->hull) {
    bumphull_layers_(self->hull, 1 << z);
  }  
  return self;
}

/** Sets an individual flag on the Thing. */
int thing_setflag(Thing * self, int flag) {
  return flags_set(&self->flags, flag);
}

/** Unsets an individual flag on the Thing. */
int thing_unsetflag(Thing * self, int flag) {
  register int wflags = self->flags;
  return flags_unset(&self->flags, flag);
}

/** Sets or unsets an individual flag on the Thing. 
If set is true the flag is set, if false it's unset. */
int thing_flag_(Thing * self, int flag, int set) {
  return flags_put(&self->flags, flag, set);
}

/** Checks if an individual flag is set. */
int thing_flag(Thing * self, int flag) {
  return flags_get(self->flags, flag);
}

/** Uninitializes a thing. */
Thing * thing_done(Thing * self) {
  if(!self) return NULL;
  bumphull_free(self->hull); 
  bumpbody_free(self->physical); 
  self->physical= NULL;
  self->hull    = NULL;
  self->area    = NULL;
  self->z       = -1;
  self->kind    = THING_UNUSED;
  spritestate_done(&self->spritestate);
  return self;
}

/** Frees a thing. */
Thing * thing_free(Thing * self) {
  thing_done(self);
  mem_free(self);
  return NULL;
}


/** Allocates a Thing. */
Thing * thing_alloc() {
  return STRUCT_ALLOC(Thing);
}

/* Sets the sprite of a thing. */
Sprite * thing_sprite_(Thing * thing, Sprite * sprite) { 
  if (!thing) return NULL;
  return spritestate_sprite_(&thing->spritestate, sprite);
}

/* Gets the sprite of a thing. */
Sprite * thing_sprite(Thing * thing) { 
  if (!thing) return NULL;
  return spritestate_sprite(&thing->spritestate);
}



/* Sets the pose of the sprite state of a thing. The direction is 
kept the same as it was. */
int thing_pose_(Thing * thing, int pose) { 
  if (!thing) return -3;
  return spritestate_pose_(&thing->spritestate, pose);
}

/* Sets the direction of the sprite state of a thing. The pose is 
kept the same as it was. */
int thing_direction_(Thing * thing, int direction) { 
  if (!thing) return -3;
  return spritestate_direction_(&thing->spritestate, direction);
}


/** Generic initialization of a thing Initializes a Thing. 
Sets the given values and some flags. Links the Shape given
to the Thing if shape is not NULL. 
Does NOT call area_addthing on the given area. 
Returns null if that failed, but does no cleanup.  */
Thing * thing_init_generic(Thing * self, Area * area, int id, int kind, int z,
                          BumpBody * body, BumpHull * shape) {
  if(!self) return NULL;
  self->kind    = kind;
  self->id      = id;
  self->area    = area;
  self->physical= body;
  self->hull    = shape;
  self->flags   = 0;
  thing_z_(self, z);
  spritestate_init(&self->spritestate, NULL, self);
  
  /* If the thing has a body and it is not static, 
  assume it is owned by this thing and must be freed when calling 
  thing_done.
  */
  bumpbody_data_(self->physical, self);
  return self;
}

/** Initializes a rectangular, static, non-rotating Thing. Uses the 
area's static body, and makes a new rectangular shape for it. 
Returns NULL on error. Uses the area's static body. 
thing_initstatic isn't needed since bump understands tile maps.
*/

/** Initializes a rectangular, dynamic, non-rotating Thing, with mass 1 
and makes a new body  and rectangular shape for it. Returns NULL on error. 
Uses the area's static body. */
Thing * thing_init_dynamic(Thing * self, Area * area, 
                       int id, int kind, int x, int y, int z, int w, int h) { 
    BumpBody  * body    = NULL; 
    BumpHull  * shape   = NULL; 
    BeVec     pos       , delta;
    BumpAABB    bounds;
    if(!self) return NULL;
    if(!area) return NULL;
    pos                 = bevec(x, y);
    body                = bumpbody_new(pos, 1.0);
    if(!body) goto out_of_memory;    
    // dynamic things ARE positioned correctly and do not use an offset
    // the object's shape is locally around the body
    bounds              = bumpaabb(x, y, w, h);
    delta               = bevec0();
    shape               = bumphull_newall(body, delta, bounds,  1 << z, kind);
    if(!shape) goto out_of_memory;    
    return thing_init_generic(self, area, id, kind, z, body, shape);
    
    out_of_memory:
    bumpbody_free(body);
    thing_done(self); 
    return NULL; 
}



/** Allocates and initializes a new static Thing. Not needed anymore. */
/* thing_newstatic */


/** Allocates and initializes a new dynamic Thing. */
Thing * thing_new_dynamic(Area * area, 
                       int id, int kind, 
                       int x, int y, int z, int w, int h) {
  Thing * self = thing_alloc();
  if(!thing_init_dynamic(self, area, id, kind, x, y, z, w, h)) {
    return thing_free(self); 
  }
  return self;
}

/** Accessors for thing's properties, forwarded to the body 
of the thing. **/


/** Position of the thing. */
BeVec thing_p(Thing * self) {
  return bumpbody_p(self->physical);
}


/** Bounds box of the thing */
BumpAABB thing_aabb(Thing *self) {
  return bumphull_aabb(self->hull);
}

/** Width  of thing. */
int thing_w(Thing * self) {
  return thing_aabb(self).hs.x * 2;
}

/** Height of thing. */
int thing_h(Thing * self) {
  return thing_aabb(self).hs.y * 2;
}

/** Position X coordinates (top left). */
int thing_x(Thing * self) {
  return thing_p(self).x - thing_aabb(self).hs.x ;
}

/** Position Y coordinates (top left). */
int thing_y(Thing * self) {
  return thing_p(self).y - thing_aabb(self).hs.y ;
}

/** Position X coordinates (center). */
int thing_cx(Thing * self) {
  return thing_p(self).x;
}

/** Position Y coordinates (center). */
int thing_cy(Thing * self) {
  return thing_p(self).y;
}

/** Layer of thing. */
int thing_z(Thing * self) {
  return self->z;
}

/** Velocity of the thing. */
BeVec thing_v(Thing * self) {
  return bumpbody_v(self->physical);
}

/** Speed X coordinates. */
int thing_vx(Thing * self) {
  return thing_v(self).x;
}

/** Speed Y coordinates. */
int thing_vy(Thing * self) {  
  return thing_v(self).y;
}

/** Set velocity. */
void thing_v_(Thing * self, BeVec v) {
  bumpbody_v_(self->physical, v);
}

/** Set velocity by xy. */
void thing_vxy_(Thing * self, int vx, int vy) {
  BeVec v = bevec(vx, vy);
  thing_v_(self, v);
}

/** Set x velocity only, leaving y unchanged . */
void thing_vx_(Thing * self, int vx) {
  BeVec v = thing_v(self);
  v.x     = vx;
  thing_v_(self, v);
}

/** Set y velocity only, leaving x unchanged . */
void thing_vy_(Thing * self, int vy) {
  BeVec v = thing_v(self);
  v.y     = vy;
  thing_v_(self, v);
}

/** Sets position of thing's body. */
void thing_p_(Thing * self, BeVec p) {
  bumpbody_p_(self->physical, p);
}

/** Adds delta to the position of thing's body. */
void thing_deltap(Thing * self, BeVec delta) {
  BeVec old = thing_p(self);
  thing_p_(self, bevec_add(old, delta));
}

/** Set position by xy. */
void thing_pxy_(Thing * self, int x, int y) {
  BeVec p = bevec(x, y);
  thing_p_(self, p);
}

/** Set x velocity only, leaving y unchanged . */
void thing_x_(Thing * self, int x) {
  BeVec p = thing_p(self);
  p.x     = x;
  thing_p_(self, p);
}

/** Set x velocity only, leaving y unchanged . */
void thing_y_(Thing * self, int y) {
  BeVec p = thing_p(self); 
  p.y     = y;
  thing_p_(self, p);
}

/** Applies a force on the center of gravity of the thing. */
void thing_applyforce(Thing * thing, const BeVec f) {
  bumpbody_applyforce(thing->physical, f);
}

/** Applies an impulse on the center of gravity of the thing. */
void thing_applyimpulse(Thing * thing, const BeVec f) {
  bumpbody_applyimpulse(thing->physical, f);
}

/* Resets the force on this thing to 0. */
void thing_resetforces(Thing * thing) {
  bumpbody_resetforces(thing->physical);
}

/* Returns the position of self, works both for static and dynamic things. 
 * Redundant but kept for backcompat.
 */
BeVec thing_sdp(Thing * self) {
  return thing_p(self);
}


/* Draws a thing to the current active drawing target, corresponding 
to it's shape and kind and taking the camera into account.  Useful for 
checking or debugging the physics. */
void thing_draw(Thing * self, Camera * camera) {
  int cx      ; 
  int cy      ;
  int drawx, x;
  int drawy, y;
  int w, h    ;
  int sw, sh  ;
  int sx, sy  ;
  int t       = 2;
  Color color;
  // don't draw null things.
  if(!self) return;
  cx          = camera_at_x(camera);
  cy          = camera_at_y(camera);
  w           = self->size.x;
  h           = self->size.y;
  if ((self->physical) && (self->hull)) {
    x         = bumpbody_p(self->physical).x - bumphull_aabb(self->hull).hs.x;
    y         = bumpbody_p(self->physical).y - bumphull_aabb(self->hull).hs.y;
    w         = bumphull_aabb(self->hull).hs.x * 2;
    h         = bumphull_aabb(self->hull).hs.y * 2;
  } 
  
  color     = color_rgba(0xee, 0x44, 0x00, 0x88);
  t         = 8;
  /* Do not draw out of camera range. */
  if(!camera_cansee(camera, x, y, w, h)) {
    return;
  }
  drawx = x - cx;
  drawy = y - cy;
  /* Draw sprite if available... */
  if(spritestate_can_draw_p(&self->spritestate)) {
    BeVec spriteat = bevec(drawx, drawy);
    /* Draw elliptical shadow under sprite unless disabled */
    if (!(self->flags & THING_FLAGS_NO_SHADOW)) { 
      color     = color_rgbaf(0.0, 0.0, 0.0, 0.5);
      sx        = drawx + w / 2;
      sy        = drawy + ((4*h)/5);
      sw        = (4 * w) / 11;
      sh        = h / 3;
      al_draw_filled_ellipse(sx, sy, sw, sh, color);
    }
    spritestate_draw(&self->spritestate, &spriteat);
  } else {
    /* ...  otherwise debug box to alert scripters of it's absence. */
    draw_box(drawx, drawy, w, h, color, t);
  }
}

/* Returns the direction the sprite of this thing is currently facing */
int thing_direction(Thing * self) {
  if (!self) return SPRITE_NO_DIRECTION;
  return spritestate_direction(&self->spritestate);
}

/* Returns the pose of the sprite of this thing */
int thing_pose(Thing * self) {
  if (!self) return SPRITE_STAND;
  return spritestate_pose(&self->spritestate);
}

/* Changes the pose to newpose but only if it's oldpose. 
 * Returns negative if the pose wasn't changed.  */
int thing_poseifold_(Thing * self, int oldpose, int newpose) {
  if (thing_pose(self) == oldpose) {
    return thing_pose_(self, newpose);
  } 
  return -1;
}



/* Updates the thing, especialy it's spite state direction and animation. */
void thing_update(Thing * self, double dt) {
  /* Update the things layer if uit has no support under hit's feet and
  it's level is greater than 1 and it's flying. */
  if ((thing_z(self) > 1) && (self->hull)) {
    if(!bumphull_support(self->hull)) {
      /* No support, drop down. */
      thing_z_(self, 1);
    }
  }
  
  if(!flags_get(self->flags, THING_FLAGS_LOCK_DIRECTION)) { 
    int newdir; 
    BeVec vel = thing_v(self);
    double magnitude = bevec_length(vel);
    if (fabs(magnitude) > 0.00) { 
     /* could change dir if moving. TODO: take the old direction into
        account for more control of facing when moving diagonally. */
      if (vel.x < -0.01) {
        newdir = SPRITE_WEST;
      } else if (vel.x > 0.01) {
        newdir = SPRITE_EAST;
      } else if (vel.y < -0.01) {
        newdir = SPRITE_NORTH;
      } else if (vel.y > 0.01) {
        newdir = SPRITE_SOUTH;
      } else { /* Not much motion, don't change dir */
        newdir = thing_direction(self);
      }
      /* Change direction. spritestate_pose will see if nothing changed and do nothing. in that case */
      thing_poseifold_(self, SPRITE_STAND, SPRITE_WALK);
      thing_direction_(self, newdir);
    } else {
      thing_poseifold_(self, SPRITE_WALK, SPRITE_STAND);
      /* This won't work on attacking, etc. */
    }
  } else {
    LOG("Thing direction locked!");
  }
  
  spritestate_update(&self->spritestate, dt);
}


/* Compares a thing with another, for drawing order. Things with low 
 z come before those with high z, and with same z, low y comes before high y. 
 qsort compatible. */
int thing_compare_for_drawing(const void * p1, const void * p2) {
  Thing * self  = *((Thing **) p1);
  Thing * other = *((Thing **) p2);
  BeVec pos1, pos2;
  if(!self) { 
    if(!other) {
      return 0;
    }
    /* Sort nulls to the back. */
    return 1;
  }
  if(!self) return -1;
  /* Compare Z if no nulls. */
  if (self->z < other->z) return -1;
  if (self->z > other->z) return  1;
  pos1 = thing_sdp(self);
  pos2 = thing_sdp(other); 
  /* compare y if z is equal */
  if ( pos1.y < pos2.y ) return -1;
  if ( pos1.y > pos2.y ) return 1;
  /* Compare by address or whatever*/
  if (((uintptr_t)p1) < ((uintptr_t)p2)) return -1;
  if (((uintptr_t)p1) > ((uintptr_t)p2)) return 1;
  if (((uintptr_t)p1) == ((uintptr_t)p2)) return 0;
  return 0;
}


/** Tints a layer in the sprite's state of this thing. */
int thing_tint_layer(Thing * me, int layer_index, Color color) {
  if (!me) return -1;
  return spritestate_tint_layer(&me->spritestate, layer_index, color);
} 


/** Hides or shows a layer in he sprite of this thing. */
int thing_hide_layer(Thing * me, int layer, int hidden) {
  if (!me) return -1;
  return spritestate_set_layer_hidden(&me->spritestate, layer, hidden);
}

/** Returns true if the layer of the sprite is hidden false if not. */
int thing_is_layer_hidden(Thing * me, int layer) { 
  return spritestate_get_layer_hidden(&me->spritestate, layer);
}

/** Sets the loop mode of the given action of the thing. */
int thing_set_action_loop(Thing * me, int action, int loopmode) { 
  return spritestate_set_action_loop(&me->spritestate, action, loopmode);
}

/** Gets the loop mode of the given action of the thing. */
int thing_get_action_loop(Thing * me, int action) { 
  return spritestate_get_action_loop(&me->spritestate, action);
}

/** Checks if the given action is done in case of actions set to one-shot 
 * loop mode only. 
 */
int thing_is_action_done(Thing * me, int action) { 
  return spritestate_is_action_done(&me->spritestate, action);
}


/** Sets the loop mode for the given pose and direction of the thing. */
int thing_set_pose_direction_loop
  (Thing * me, int pose, int direction, int loopmode) {
  return spritestate_set_pose_direction_loop(&me->spritestate, pose, direction, loopmode);
}

/** Gets the loop mode for the given pose and direction of the thing. */
int thing_get_pose_direction_loop
  (Thing * me, int pose, int direction) {
  return spritestate_get_pose_direction_loop(&me->spritestate, pose, direction);
}

/** Sets a flag on the main hull of the Thing. */
int thing_set_hull_flag(Thing * me, int flag) {
  if (!me) return 0;
  return bumphull_set_flag(me->hull, flag);
}

/** Unsets a flag for the main hull of the thing. */
int thing_unset_hull_flag(Thing * me, int flag) {
  if (!me) return 0;
  return bumphull_unset_flag(me->hull, flag);
}

/** Sets all flags for the main hull of the thing. */
int thing_hull_flags_(Thing * me, int flags) {
  if (!me) return 0;
  return bumphull_flags_(me->hull, flags);
}

/** Gets all flags for the main hull of the thing. */
int thing_hull_flags(Thing * me) {
  if (!me) return 0;
  return bumphull_flags(me->hull);
}

/** Returns the group of the main hull of the thing. */
int thing_hull_group(Thing * me) {
  if (!me) return -1;
  return bumphull_group(me->hull);
}

/** Sets the group of the main hull of the thing. */
int thing_hull_group_(Thing * me, int group) {
  if (!me) return -1;
  return bumphull_group_(me->hull, group);
}

