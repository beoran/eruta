#ifndef spritelist_H_INCLUDED
#define spritelist_H_INCLUDED


/* Sprite list functions. */

typedef struct SpriteList_      SpriteList;
/* Amount of potential sprites that a SpriteList can contain at creation. */
#define SPRITELIST_NSPRITES_DEFAULT 10000


SpriteList * spritelist_alloc();
SpriteList * spritelist_initall(SpriteList * self, int maxsprites);
SpriteList * spritelist_init(SpriteList * self);
SpriteList * spritelist_done(SpriteList * self);
SpriteList * spritelist_new();
SpriteList * spritelist_free(SpriteList * self);
Sprite     * spritelist_sprite(SpriteList * self, int index);
Sprite     * spritelist_sprite_(SpriteList * self, int index, Sprite * sprite);

Sprite * spritelist_new_sprite(SpriteList * self);
int spritelist_new_sprite_id(SpriteList * self);

Sprite     * spritelist_loadlayer_ulpcss_vpath(
              SpriteList * self, int index,  int layerindex, char * vpath);
int spritelist_get_unused_sprite_id(SpriteList * self);

int spritelist_load_sprite_layer_with_builtin_layout
  (SpriteList * me, int isprite, int ilayer, char * vpath, int layout);

int spritelist_load_sprite_layer_with_layout
  (SpriteList * me, int isprite, int ilayer, char * vpath, SpriteLayout * layout);

int spritelist_tint_sprite_layer
  (SpriteList * me, int isprite, int ilayer, Color color); 

int spritelist_delete_sprite(SpriteList * self, int index);

#endif




