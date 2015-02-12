
#include "eruta.h"
#include "sprite.h"
#include "spritelayout.h"

/* Custom and built-in sprite layouts and loading them. */


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
SpriteLayout * spritelayout_for(int load_type) {
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





