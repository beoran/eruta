/** Common data definitions for basewars. */
#ifndef _BASEWARS_H_
#define _BASEWARS_H_

#include <stdarg.h>
#include <SDL.h>
#include <SDL_ttf.h>

#if SDL_VERSION_ATLEAST(1, 3, 0)
#else
#error "SDL 1.3 NOT found!"
#endif

#include "chipmunk.h"
#include "lua.h"
#include "lauxlib.h"
#include "gy.h"
#include "ch.h"




#define BATHING_NAME_SIZE      32
#define BABEING_INVENTORY_SIZE 32

/** Anything interactive (players, items, foes, bullets) 
that is in the game. */
typedef struct BaThing_ {  
  char name[BATHING_NAME_SIZE];
} BaThing;

/** Item that can be picked up. */
typedef struct BaItem_ {
  BaThing thing;
} BaItem;

/** Weapon that can be held or picked up. */
typedef struct BaWeapon_ {
  BaThing thing;
  int8_t power;
  int8_t level;
} BaWeapon;

/** Bullet or shot (or damage area). */
typedef  struct BaBullet_ {
  BaThing thing;
} BaBullet;

/** Trap placed somewhere.  */
typedef struct BaTrap_ {
  BaThing thing;
} BaTrap;

/** An active being, either foe or player. */
typedef struct BaBeing_ {
  BaThing thing;
  int8_t health_now;
  int8_t health_max;
  int8_t armor;
  BaThing * inventory[BABEING_INVENTORY_SIZE];
} BaBeing;

/** A player character. */
typedef struct BaPlayer_ {
  BaBeing being;
} BaPlayer;

/** A foe. */
typedef struct BaFoe_ {
  BaBeing being;
} BaFoe;


/** A simplified sprite structure. */
typedef struct BaSprite_ BaSprite;

enum BaSpriteFlags_ {
  BaSpriteActive = 1 << 0,
};

struct BaSprite_ {
  GyTexture * texture; /* Texture that contains the sprite's frames.     */
  BaSprite  * next;    /* Drawing order of sprites.                      */
  BaSprite  * last;    /* Drawing order of sprites.                      */
  BaItem    * data;    /* Locical object pointer.                        */
  cpBody    * body;    /* Physical body of this sprite.                  */
  uint32_t    flags;   /* Flags of the sprite.                           */
  uint16_t    xs, xy;  /* Indicators for the active frame of the sprite. */
  int16_t     x, y;    /* Drawing (screen) coordinates of the sprite.    */
  
};

#define BAMAP_LAYERS  2
#define BAMAP_SPRITES_PLAYERS  16
#define BAMAP_SPRITES_PWEAPONS 16
#define BAMAP_SPRITES_PBULLETS 16
#define BAMAP_SPRITES_POWERUPS 16

#define BAMAP_SPRITES_FOES     16
#define BAMAP_SPRITES_FWEAPONS 16

#define BAMAP_SPRITES 1024


/** A map is a map of a mission, with all that involves. */
typedef struct BaMap_ BaMap;
struct BaMap_ {
  GyTexture * texture;                // Texture used for layers.
  GyLayer   * layers[BAMAP_LAYERS];   // Layers for background, and main.
  cpSpace   * space;                  // Physical space.
  BaSprite    sprites[BAMAP_SPRITES]; // Sprites for drawing.
  GyTiledata* tiledata;               // Information about the files.  
  int         texture_id; // texture ID
  int         gridw;
  int         gridh;
};



typedef struct GyMain_ GyMain;
typedef struct GyMode_ GyMode;

struct GyMain_ {  
  GyCanvas    * canvas;
  GyWindow    * window;
  GyJoystick ** joysticks;
  int           busy;
  GyMode      * mode;
  lua_State   * lua;
  BaMap         map;
};

GyMain * bamain_get();


/** Loads a texture of the given category and index. Uses the active canvas. */
GyTexture * batexture_load(const char * category, int index);

/** Map functionality. */

/* Gets a pointer to the game's current map. */
BaMap * bamap_getcurrent();
BaMap* bamap_initempty(BaMap * self);
BaMap* bamap_init(BaMap * self, int textureid, int w, int h);
BaMap * bamap_done(BaMap * self);

/** Sets tile info for given the map and tile. */
int bamap_tileinfo_(BaMap * self, int tile, int info);

/** Gets tile info for given the map and tile. */
int bamap_tileinfo(BaMap * self, int tile);

/** Returns a pointer to the layer at index or NULL if out of range. */
GyLayer * bamap_layer(BaMap * self, int layer);

/** Makes a physical wall at the given tile coordinates. */
cpShape * bamap_makewall(BaMap * self, int tx, int ty);

/** Checks if a tile index is a wall tile. */
int bamap_tilewall(BaMap * self, int tile);

 
#define BAMAP_ERROR -2

int bamap_set(BaMap * self, int l, int x, int y, int tile);
int bamap_setslab(BaMap * self, int l, int x, int y, int w, int h, int tile);
int bamap_fill(BaMap * self, int l, int tile);
int bamap_get(BaMap * self, int l, int x, int y);
BaMap * bamap_load(lua_State * lua, int index);
GyLayer * gylayer_savefile(GyLayer *layer, int layerid, FILE *fout);
BaMap * bamap_savefile(BaMap * map, FILE * fout);
BaMap * bamap_save(BaMap * map, int index);

void bamap_draw(BaMap * map, GyWindow * window, int x, int y);


/** Initializes the Lua state we use for the game. */
int lb_init(lua_State * lua);



#endif
