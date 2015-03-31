
#include "eruta.h"
#include "dynar.h"
#include "sprite.h"
#include "spritelist.h"


/* Sprite list functions. */
/* The list of all loaded sprites. Some may be drawn and some not. */
struct SpriteList_ {
  Dynar * sprites;
  int     sprites_used;
};


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


/** Deletes a sprite with thje given id from the sprite list and frees it. 
 * Returns negative on failure (sprite didn't exist), or 0
 * on success.  */
int spritelist_delete_sprite(SpriteList * self, int index) {
  Sprite * sprite;
  if (!self) return -1;  
  sprite = spritelist_sprite(self, index);
  if (!sprite) return -2;
  sprite_free(sprite);
  dynar_putptr(self->sprites, index, NULL); 
  return 0;
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

/** Makes a new sprite and returns it */
Sprite * spritelist_new_sprite(SpriteList * self) {
  Sprite * sprite;
  int index;
  if (!self) return NULL;
  index = spritelist_get_unused_sprite_id(self);  
  if (index < 0) return NULL;  
  sprite = sprite_new(index);  
  if(!sprite) return NULL;
  return spritelist_sprite_(self, index, sprite);
}

/** Makes a new sprite and returns it's ID or negative on error
 * (ie, cannot make more sprites).
*/
int spritelist_new_sprite_id(SpriteList * self) {
  Sprite * sprite = spritelist_new_sprite(self);
  return sprite_id(sprite);
}


/* Makes a new sprite if it doesn't exist yet or otherwise returns the old one 
 at the given index. 
Sprite * spritelist_getornew(SpriteList * self, int index) {
  Sprite * sprite = spritelist_sprite(self, index);
  if (sprite) return sprite;
  return spritelist_newsprite(self, index);
} 
*/

/* Loads a sprite cell in ulpcss format into the sprite with 
 the given index in the sprite list. If no sprite exists yet,
 it will be created. The sprite s returned on sucess of NULL on failure. 
*/

Sprite * spritelist_loadlayer_ulpcss_vpath(
  SpriteList * self, int index,  int layerindex, char * vpath) { 
  Sprite * sprite;
  int oversize = FALSE;
  if (!self) return NULL;
  if (strstr(vpath, "oversize")) { oversize = TRUE; } 
  sprite = spritelist_sprite(self, index);
  if (!sprite) return NULL;
  if (!sprite_loadlayer_ulpcss_vpath(sprite, layerindex, vpath, oversize)) 
    return NULL;
  return sprite;
}


/* Returns the first unused sprite ID larger than minimum. */
int spritelist_get_unused_sprite_id(SpriteList * self) {
  int index, stop;
  if (!self) return -1;  
  stop = dynar_size(self->sprites);
  for (index = 0; index < stop; index++) {
    Sprite * sprite = spritelist_sprite(self, index);
    if (!sprite) {
      return index;
    }
  }
  return -3;
}


/* Loads a layer of a sprite from a vpath using one of the built in layout
 * loaders, normally ULPCSS format. */
int spritelist_load_sprite_layer_with_builtin_layout
  (SpriteList * me, int sprite_index,
  int layer_index, char * vpath, int layout) {

  Sprite * sprite = spritelist_sprite(me, sprite_index);
  if (!sprite) return -1;
  if(sprite_loadlayer_ulpcss_vpath(sprite, layer_index, vpath, layout)) { 
    return sprite_index;
  } else {
    return -2;
  }
}


/* Loads a layer of a sprite from a vpath using a custom layout. */
int spritelist_load_sprite_layer_with_layout
  (SpriteList * me, int sprite_index,
  int layer_index, char * vpath, SpriteLayout * layout) {

  Sprite * sprite = spritelist_sprite(me, sprite_index);
  if (!sprite) return -1;
  if(sprite_loadlayer_vpath(sprite, layout, layer_index, vpath)) { 
    return sprite_index;
  } else {
    return -2;
  }
}





