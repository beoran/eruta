#include "eruta.h"
#include "mem.h"
#include "dynar.h"
#include "silut.h"
#include "tile.h"
#include "monolog.h"

#ifndef TILE_MAXFRAME
#define TILE_MAXFRAME 32
#endif

#ifndef TILE_MAXPROGRAM
#define TILE_MAXPROGRAM 64
#endif

/**
* A tile set 
*/
struct Tileset_ {
  Dynar    * tiles;   
  // Tile     * tiles;
  Image    * sheet;
  // size_t     size;
  size_t     last;
  int        w;
  int        h;
  /* Offset of tile set in TMX map file. Used to calculate correct tile offsets. */
  int        firstgid;
};

typedef struct TileFrame_ TileFrame;

/**
 * A Tiled-style animation fames of a tile.
 */
struct TileFrame_ {
  /* Tile set index for this frame of animation. */
  int index;
  /* Time this animation frame is shown. */
  double duration;
};

/** 
* A single tile from a tile map.
* Tiles can be animated. This works like this: a tile has an animation
* pointer and offset which points to the next tile to be drawn in the tileset.
*
* A tile can only hold up to TILE_FRAMES tiles pointers in itself.
*/
struct Tile_ {
  Tileset     * set;  
  int           index;
  /* Tileset this tile belongs to + index in the tile set. */  
  int           flags;
  /* Information about the tile's properties. */
  int           kind;
  /* Index of currently active animation pointer for this tile. */
  int           anim;
  /* Offset to the tile to skip to when animating. If this is
  0 the tile is not animated. If nonzero, the tile will skip to
  the tile in the same tile set set with index index + anim.
  May be negative to "roll back" an animation to it's begin. */  
  int           active;
  /* For unanimated tiles, active is set to the index of the tile itself.
   For animated tiles, it is set to the index of tile that currently should
   be displayed in stead of this tile due to animation.
  */
  double        wait;
  /* Time in s to wait before jumping to the next frame of this tile. */  
  double        time;
  /* Time since last animation in s. */
  /* Sub-position in the tile sheet image of the tileset. */
  Point         now;
  /* Automatic blending activation and priority. */
  int           blend;
  /* Mask number to use for automatic blending, if any. */
  int           blend_mask;
  /* Automatic lighting activation flag. */
  int           light;
  /* Automatic lighting mask nuymber. */
  int           light_mask;
  /* Automatic shadow activation flag. */
  int           shadow;
  /* Automatic shadow mask number. */
  int           shadow_mask;
  /* Tiled-style animation frames. */
  Dynar       * frames;
  /* Active frame for TMX-style animations. */
  int           active_frame;
};

/* NOTE: Tiles could be implemented using sub bitmaps as they seem to be
* slightly faster if they are preallocated. however the speed gain would
* be around 2%, so it's not a priority yet. It could simplify some of
* the code, though.
*/


/* Cleans up a tile. */
void tile_done(Tile * tile) {
  if (!tile) return;
  if (tile->frames) {
    dynar_free(tile->frames);
  }
  tile->frames = NULL;
}

/* for dynar_destroy */
void * tile_destroy(void * tile) {
  tile_done(tile);
  return NULL;
}


/** Cleans up a tileset, and empties it.  */
void tileset_done(Tileset * set) {
  Tile * tile;
  
  if(!set) return;
  if (set->tiles) {
    al_destroy_bitmap(set->sheet);
    set->sheet = NULL;
    dynar_destroy_structs_and_free(set->tiles, tile_destroy);
    set->tiles = NULL;
    set->w      = -1;
    set->h      = -1;
  }
}

/** Deallocates a tileset */
void tileset_free(Tileset * set) {
  if(!set) return;
  tileset_done(set);
  mem_free(set);
}

/** Retuns the amount of tiles in the tile set. */
int tileset_size(Tileset * set) {
  return dynar_size(set->tiles);
}

/* Returns the firstgid of the tile set, useful for TMX maps. */
int tileset_firstgid(Tileset * set) {
  return set->firstgid;
}

/** Initializes a given tileset with a given bitmap tile sheet and firstgid */
Tileset * tileset_init(Tileset * set, Image * sheet, int firstgid) {
  int size = 0, index = 0;
  if(!set)      return NULL;
  set->sheet    = sheet;
  set->firstgid = firstgid;
  if(!set->sheet)    {
    set->w      = -1;
    set->h      = -1;    
    set->tiles  = NULL;
    return NULL;
  }
  // allow re-init
  if (set->tiles) {
    tileset_done(set);
  } 
  set->w        = image_w(set->sheet);
  set->h        = image_h(set->sheet);
  size          = (set->w / TILE_W) * (set->h / TILE_H);
  set->tiles    = dynar_new(size, sizeof(Tile));
  set->last     = 0;
  // set->tiles    = mem_alloc(sizeof(Tile) * set->size);
  if (!set->tiles) {
    set->w      = -1;
    set->h      = -1;
    return NULL;
  }
  // now set up the tiles
  for(index = 0; index < dynar_size(set->tiles); index ++) {
    tile_init(tileset_get(set, index), set, index);
  }    
  return set;
}

/** Creates a new tileset with the given tile sheet image. */
Tileset * tileset_new(Image * sheet, int firstgid) {
  Tileset * set, * result;
  set                         = STRUCT_ALLOC(Tileset);
  if(!set) return NULL;
  result                      = tileset_init(set, sheet, firstgid);
  if(!result) {
     tileset_free(set);
     return NULL;
  }
  return result;
}


/*Macros that calculate the position of a tile in a tile set's sheet. */
#define TILE_SHEET_Y(TILE, SET)\
        (((TILE->active * TILE_W) / ((SET)->w)) * TILE_H)

#define TILE_SHEET_X(TILE, SET)\
        ((TILE->active * TILE_W) % ((SET)->w))
  
/** Recalculates the tile's position (now) in it's tile set. */
Tile * tile_recalculate(Tile * tile) {
  double x, y;
  // LOG_NOTE("Tile Recalculate: %p \n", tile);
  if (!tile) return NULL;
  // LOG_NOTE("Tile Recalculate: %p %d\n", tile->set, tile->active);
  if (!tile->set) return NULL;  
  // LOG_NOTE("Tile Recalculate set: %d %d\n", tile->set->w, tile->set->h);

  x = (double)TILE_SHEET_X(tile, tile->set);
  y = (double)TILE_SHEET_Y(tile, tile->set);
  // LOG_NOTE("Tile Recalculate: %lf, %lf -> %lf, %lf\n", tile->now.x, tile->now.y, x, y);
  tile->now = bevec(x, y); 
  return tile;
}

/** Initializes a tile to belong to a given tile set. */
Tile * tile_init(Tile * tile, Tileset * set, int index) {
  if(!tile) return NULL;
  if(!set) return NULL;
  tile->flags       = 0;
  tile->kind        = 0;
  tile->index       = index;
  tile->set         = set;
  tile->anim        = 0;
  tile->time        = 0.0;
  tile->wait        = 0.250;
  tile->active      = index;
  tile->blend       = 0;
  tile->blend_mask  = 0;
  tile->light       = 0;
  tile->light_mask  = 0;
  tile->shadow      = 0;
  tile->shadow_mask = 0;
  tile->frames      = NULL;
  tile->active_frame= 0;
  tile_recalculate(tile);
  return tile;
}

/** Gets a tile from a tile set by it's tile id. **/ 
Tile * tileset_get(Tileset * set, int index) {
  if(!set) return NULL;
  if (index < 0) return NULL;
  return dynar_getdata(set->tiles, index);
}

/** Sets the animation parameter of this tile */
Tile * tile_anim_(Tile * tile, int anim) {
  if(!tile) return NULL;
  tile->anim = anim;
  return tile;
}

/** Gets the animation parameter of this tile, or 0 if NULL */
int tile_anim(Tile * tile) {
  if(!tile) return 0;
  return tile->anim;
}

/** Sets the wait parameter of this tile in ms */
Tile * tile_wait_(Tile * tile, int wait) {
  if(!tile) return NULL;
  tile->wait = (double)wait / 1000.0;
  return tile;
}

/** Gets the wait parameter of this tile in ms, or -1 if NULL */
int tile_wait(Tile * tile) {
  if(!tile) return -1;
  return (int)(tile->wait * 1000.0);
}


/* Helper lookup table for the tile flag names */
static Silut tile_flagnames[] = {
  { TILE_WALL   , "wall"    },
  { TILE_WATER  , "water"   },
  { TILE_LEDGE  , "ledge"   },
  { TILE_STAIR  , "stair"   },
  { TILE_PUSH   , "push"    },
  { TILE_NORTH  , "north"   },
  { TILE_SOUTH  , "south"   },
  { TILE_EAST   , "east"    },
  { TILE_WEST   , "west"    },
  { TILE_ICE    , "ice"     },
  SILUT_DONE  
};

/** Gets the value of the flags of a tile. */
int tile_flags(Tile * tile) {
  if(!tile) return 0;
  return tile->flags;
}

/** Sets the flags on a tile. */
Tile * tile_flags_(Tile * tile, int flags) {
  if(!tile) return NULL;
  tile->flags = flags;
  return tile;
}

/** Sets a single flag on a tile. */
Tile * tile_setflag(Tile * tile, int flag) {
  if(!tile) return NULL;
  tile->flags = BIT_SETFLAG(tile->flags, flag);
  return tile;
}

/** Removes a single flag on a tile. */
Tile * tile_unflag(Tile * tile, int flag) {
  if(!tile) return NULL;
  tile->flags = BIT_UNFLAG(tile->flags, flag);
  return tile;
}

/** Checks a single flag on a tile. */
int tile_isflag(Tile * tile, int flag) {
  if(!tile) return 0;
  return BIT_ISFLAG(tile->flags, flag);
}

/** Sets a tile's flags from a property string.
* This uses an internal lookup table.
*/
Tile * tile_property_(Tile * tile, char * property) {
  if(!tile) return NULL;
  Silut * aid = silut_lsearchcstr(tile_flagnames, property);
  if(!aid) return NULL;  
  return tile_setflag(tile, aid->integer);
}

/** Initializes a tile's frame of animation. */
TileFrame * tileframe_init(TileFrame * me, int index, double duration) {
  if (!me) return NULL;
  me->index    = index;
  me->duration = duration;
  return me;
}

/** Gets the nth frame of Tiled style animations for this tile 
 * or NULL if no such animation frame. */
TileFrame * tile_frame(Tile * tile, int index) {
  if (!tile) return NULL;
  if (!tile->frames) return NULL;
  return dynar_getdata(tile->frames, index);
} 

/** Gets the amount of Tiled style animations for this tile, or 0 if none. */
int tile_frame_count(Tile * tile) {
  if (!tile) return 0;
  if (!tile->frames) return 0;
  return dynar_size(tile->frames);
}

/** Adds a Tiled-style animation frame to the tile. */
Tile * tile_add_animation_frame(Tile * tile, int index, double duration) {
  if (tile->frames) {
    int size;
    size = dynar_size(tile->frames);
    if (!dynar_size_(tile->frames, size + 1)) return NULL;
    tileframe_init(dynar_getdata(tile->frames, size), index, duration);
  } else {
    tile->frames = dynar_new(1, sizeof(struct TileFrame_));
    if (!tile->frames) return NULL;
    tileframe_init(dynar_getdata(tile->frames, 0), index, duration);
  }
  return tile;
} 


/** Rewinds a tile's animations. */
void tile_rewindanime(Tile * tile) {
  if (!tile) return;
  tile->active        = tile->index;
  tile->active_frame  = 0;
  // Finally recalculate tile position.
  tile_recalculate(tile);
}

/** Updates a tile to animate it using classic style animation.*/
void tile_update_classic(Tile * tile, double dt) {
  int active = 0;
  Tile * aidtile = NULL;
  Tile * nowtile = tileset_get(tile->set, tile->active);
  // nowtile is the tile that is currently active, that is shown.
  // in stead of ourself, but it also may be ourself.
  if(!nowtile) return;
  tile->time    += dt; // advance animation time of tile. 
  // Don't animate if not enough time has passed
  if(tile->time < tile->wait) return;
  // if we get here, reset animation time.
  tile->time     = 0.0;
  // take the animation parameter and add it to the active  
  active  = tile->active + nowtile->anim;
  aidtile = tileset_get(tile->set, active);
  // Check if there is such a tile.
  if(!aidtile) return;
  // If there is no such tile, don't change the active tile of this tile.
  tile->active = active;  
  // Finally recalculate tile position.
  tile_recalculate(tile);
}

/**  Updates a tile to anmate it using TMX style animation. */
void tile_update_tmx(Tile * tile, double dt) {
  int active = 0;
  TileFrame * frame = NULL;
  Tile * aidtile = NULL;
  
  frame = tile_frame(tile, tile->active_frame);
  if (!frame) { /* Animation overshoit itself somehow??? */
    tile->active_frame = 0;
    frame = tile_frame(tile, tile->active_frame);
    if (!frame) return;
  }
  
  tile->time    += dt; // advance animation time of tile. 
  // Don't animate if not enough time has passed
  if(tile->time < frame->duration) return;
  // advance the animation frame, loop it around if needed. 
  tile->active_frame++;
  if (tile->active_frame >= tile_frame_count(tile)) {
    tile->active_frame = 0;
  }
  // Get new tile frame
  frame = tile_frame(tile, tile->active_frame);
  // If we get here, reset animation time.
  tile->time     = 0.0;
  if (!frame) return;
  
  // Get the active tile t use from the animation frame
  active  = frame->index;
  aidtile = tileset_get(tile->set, active);
  // Check if there is such a tile.
  if(!aidtile) return;
  // If there is no such tile, don't change the active tile of this tile.
  tile->active = active; 
  // Finally recalculate tile position.
  tile_recalculate(tile);
  // tile->now = aidtile->now;
  // LOG_NOTE("TMX Anim: %d (%d: set(%d, %d)): (x,y)=(%lf, %lf)\n", tile->index, tile->active, tile->set->w, tile->set->h, tile->now.x, tile->now.y);
}

/* Animates the tile. If it has a TMX style animation, that takes 
 * precedence, otherwise, use the classic style animation. */
void tile_update(Tile * tile, double dt) {
  if (tile->frames) {
    tile_update_tmx(tile, dt);
  } else {
    tile_update_classic(tile, dt);
  }
} 


/** Updates all tiles in a tile set so they all get animated. */
void tileset_update(Tileset * set, double dt) {
  int index, size;
  if (!set) return;
  if (!set->tiles) return;
  size = tileset_size(set);
  for (index = 0; index <  size; index++) {
    Tile * tile = tileset_get(set, index); 
    tile_update(tile, dt);
  }  
} 


/** Draw a tile to the current active drawing target at the
given coordinates. Does nothing if tile is NULL.  */
void tile_draw(Tile * tile, int x, int y, int drawflags) {  
  Tileset * set;
  Image * sheet;
  Color dcolor = al_map_rgb(0xee, 0xee, 0x00);
  if (!tile) return;
  set   =  tile->set;
  sheet = set->sheet;
  float dx      = (float) x;
  float dy      = (float) y; 
  float sx      = (float) tile->now.x;
  float sy      = (float) tile->now.y;
  float sw      = (float) TILE_W;
  float sh      = (float) TILE_H;
  al_draw_bitmap_region(sheet, sx, sy, sw, sh, dx, dy, drawflags);
  // debugging solid tiles
#ifdef TILE_SHOW_SOLID  
  if (tile_isflag(tile, TILE_WALL)) {
    al_draw_rectangle(dx, dy, dx+TILE_W, dy+TILE_H, dcolor, 2);
  } 
#endif // TILE_SHOW_SOLID
  
  // al_draw_bitmap(sheet, dx, dy, 0);
}


/* Used for drawing masked tiles. */
static Image  * tile_mask_buffer = NULL;   


/** Draw a tile into the given bitmap, which should be of size TILE_W, TILE_H 
 * applying the given mask bitmap, where the mask will 
be flipped and rotated as per the given mask_flags. The mask bitmap
should be white, but with different alpha levels on the white 
which will be applied as the mask. Does nothing if tile is NULL.  
This requires al_hold_bitmap_drawing to be turned off!
*/
void tile_draw_masked_to
(Image * result, Tile * tile, Image * mask, float angle, int mask_flags) {  
  /* This function need a mask buffer. */
  Tileset * set;
  Image * sheet;
  ALLEGRO_BITMAP * target;
  Color dcolor = al_map_rgb(0xee, 0x00, 0xee);
  float dx, dy, sx, sy, sw, sh;
  int bmpflags;
  
  if (!tile) return;
  
  /* Create a 32x32 tile bitmap that will be reused thanks to 
   it being static. And leaked at program shutdown, but I don't care :p. */
  if (!tile_mask_buffer) { 
    bmpflags         = al_get_new_bitmap_flags();
    al_set_new_bitmap_flags(ALLEGRO_CONVERT_BITMAP);
    tile_mask_buffer = al_create_bitmap(TILE_W, TILE_H);
    al_set_new_bitmap_flags(bmpflags);
  } 
  
  /* Keep the target bitmap. */
  target = al_get_target_bitmap();
  
  /* Copy the tile into the buffer.  */
  al_set_target_bitmap(tile_mask_buffer);
  set     = tile->set;
  sheet   = set->sheet;
  dx      = 0.0;
  dy      = 0.0; 
  sx      = (float) tile->now.x;
  sy      = (float) tile->now.y;
  sw      = (float) TILE_W;
  sh      = (float) TILE_H;
  /* Set blender to copy mode. */
  // al_clear_to_color(al_map_rgba_f(0,0,0,0));

  al_set_blender(ALLEGRO_ADD, ALLEGRO_ONE, ALLEGRO_ZERO);  
  al_draw_bitmap_region(sheet, sx, sy, sw, sh, 0, 0, 0);
  
  /* Draw the mask over the tile, taking the alpha of the mask  */
  al_set_blender(ALLEGRO_ADD, ALLEGRO_ZERO, ALLEGRO_ALPHA);
  al_draw_bitmap(mask, 0, 0, mask_flags);

  /* Restore normal Allegro blending. */
  al_set_blender(ALLEGRO_ADD, ALLEGRO_ONE, ALLEGRO_INVERSE_ALPHA);
  
  sx = 0.0;
  sy = 0.0;
  if (angle != 0.0) {
    sx = TILE_H / 2.0;
    sy = TILE_W / 2.0;
    dx += sx;
    dy += sy;
  } 
  
  /* Draw the tile mask buffer to the result bitmap */
  al_set_target_bitmap(result);
  al_draw_rotated_bitmap(tile_mask_buffer, sx, sy, dx, dy, angle, 0);
  /* And restore the target bitmap. */ 
  al_set_target_bitmap(target);
}

/** Tile's index. Returns -1 if tile is NULL; */
int tile_index(Tile * tile) { 
  if (!tile) return -1;
  return tile->index;
}

/**  Information about the tile's properties. Return -1 if tile is NULL. */
int tile_kind(Tile * tile) { 
  if (!tile) return -1;
  return tile->kind;
}

/** Information about tile's blending properties and priority.
 * Zero means no blending, positive is a blend priority.
 */
int tile_blend(Tile * tile) {
  if (!tile) return 0;
  return tile->blend;
}

/** Set the tile's blending and priority */
int tile_blend_(Tile * tile, int priority) {
  if (!tile) return 0;
  return tile->blend = priority;
}

/** Information about tile's blending mask
 * Returns 0 for the default mask if not set.
 */
int tile_blend_mask(Tile * tile) {
  if (!tile) return 0;
  return tile->blend_mask;
}

/** Set the tile's blending mask */
int tile_blend_mask_(Tile * tile, int mask) {
  if (!tile) return 0;
  if (mask < 0) mask = 0;
  if (mask > 2) mask = 0;
  return tile->blend_mask = mask;
}

/** Get the tile's light flag. Zero means no lighting. */
int tile_light(Tile * tile) {
  if (!tile) return 0;
  return tile->light;
}

/** Set the tile's light flag */
int tile_light_(Tile * tile, int value) {
  if (!tile) return 0;
  return tile->light = value;
}

/** Information about tile's lighting mask
 * Returns 0 for the default mask if not set.
 */
int tile_light_mask(Tile * tile) {
  if (!tile) return 0;
  return tile->light_mask;
}

/** Set the tile's light mask */
int tile_light_mask_(Tile * tile, int mask) {
  if (!tile) return 0;
  if (mask < 0) mask = 0;
  return tile->light_mask = mask;
}

/** Get the tile's shadow flag. Zero means no autoshadow. */
int tile_shadow(Tile * tile) {
  if (!tile) return 0;
  return tile->shadow;
}

/** Set the tile's light flag */
int tile_shadow_(Tile * tile, int value) {
  if (!tile) return 0;
  return tile->shadow = value;
}

/** Information about tile's shadow mask
 * Returns 0 for the default mask if not set.
 */
int tile_shadow_mask(Tile * tile) {
  if (!tile) return 0;
  return tile->shadow_mask;
}

/** Set the tile's shadow mask */
int tile_shadow_mask_(Tile * tile, int mask) {
  if (!tile) return 0;
  if (mask < 0) mask = 0;
  return tile->shadow_mask = mask;
}

