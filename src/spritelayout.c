
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
  6
};

/* Type info of the ULPCSS sprites. */
static int ulpcss_row_type[] = {
  SPRITE_CAST , SPRITE_CAST , SPRITE_CAST , SPRITE_CAST ,
  SPRITE_STAB , SPRITE_STAB , SPRITE_STAB , SPRITE_STAB ,
  SPRITE_WALK , SPRITE_WALK , SPRITE_WALK , SPRITE_WALK ,
  SPRITE_SLASH, SPRITE_SLASH, SPRITE_SLASH, SPRITE_SLASH,
  SPRITE_SHOOT, SPRITE_SHOOT, SPRITE_SHOOT, SPRITE_SHOOT,
  SPRITE_DOWN
};

/* Direction info of the ULPCSS sprites. */
static int ulpcss_row_direction[] = {
  SPRITE_NORTH , SPRITE_WEST , SPRITE_SOUTH, SPRITE_EAST,
  SPRITE_NORTH , SPRITE_WEST , SPRITE_SOUTH, SPRITE_EAST,
  SPRITE_NORTH , SPRITE_WEST , SPRITE_SOUTH, SPRITE_EAST,
  SPRITE_NORTH , SPRITE_WEST , SPRITE_SOUTH, SPRITE_EAST,
  SPRITE_NORTH , SPRITE_WEST , SPRITE_SOUTH, SPRITE_EAST,
  SPRITE_ALL
};

/* Frame duration info of ULPCSS sprites. */
static double ulpcss_row_duration[] = {
  0.1, 0.1, 0.1, 0.1,
  0.1, 0.1, 0.1, 0.1,
  0.2, 0.2, 0.2, 0.2,
  0.1, 0.1, 0.1, 0.1,  
  0.1, 0.1, 0.1, 0.1,
  0,1,
};

/* Layout info of the ULPCSS sprites. Has 21 rows of cells.*/
static SpriteLayout ulpcss_layout = {
  21,
  ulpcss_sprites_per_row, ulpcss_row_type, 
  ulpcss_row_direction  , ulpcss_row_duration,
  64, 64, 0
}; 


/* Layout info of the oversized ULPCSS sprites, for weapons only. */
static int ulpcss_oversized_sprites_per_row[] = {
  6, 6, 6, 6
};

/* Type info of the oversized ULPCSS sprites, for slashinhg weapons. */
static int ulpcss_oversized_slash_row_type[] = {
  SPRITE_SLASH, SPRITE_SLASH, SPRITE_SLASH, SPRITE_SLASH
};

/* Type info of the ULPCSS sprites, for stabbing weapons. */
static int ulpcss_oversized_stab_row_type[] = {
  SPRITE_STAB, SPRITE_STAB, SPRITE_STAB, SPRITE_STAB
};

/* Duration info of the ULPCSS sprites, for oversized weapons. */
static double ulpcss_oversized_duration[] = {
  0.1, 0.1, 0.1, 0.1
};

/* Direction info of the oversized ULPCSS sprites. */
static int ulpcss_oversized_row_direction[] = {
  SPRITE_NORTH , SPRITE_WEST , SPRITE_SOUTH, SPRITE_EAST
};

/* Layout of an oversized slashing weapon ULPCSS sprite. */
static SpriteLayout ulpcss_oversized_slash_layout = {
  4,
  ulpcss_oversized_sprites_per_row, ulpcss_oversized_slash_row_type,
  ulpcss_oversized_row_direction, ulpcss_oversized_duration,
  64 * 3, 64 * 3, 0
}; 

/* Layout of an oversized stabbing weapon ULPCSS sprite. */
static SpriteLayout ulpcss_oversized_stab_layout = {
  4,
  ulpcss_oversized_sprites_per_row, ulpcss_oversized_stab_row_type,
  ulpcss_row_direction, ulpcss_oversized_duration,
  64 * 3, 64 * 3, 0
}; 



/* Amount of rows in a layout. */
int spritelayout_rows(SpriteLayout * layout) {
  int index;
  if(!layout) return 0;
  if(!layout->per_row) return 0;
  return layout->rows;
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


/* Loads a single row and column of a sprite layout from the given image
 * into the given layer for a normal cell. */
int spritelayout_load_rc_normal(
  SpriteLayout * layout, Sprite * sprite, Image * source, 
  int layeri, int rowi, int coli, int pose, int direction) {
  
  Point where           = bevec(layout->size_x * rowi, layout->size_y * coli);
  Point size            = bevec(layout->size_x, layout->size_y);
  double duration       = layout->row_duration[rowi];
  SpriteCell * cell     = sprite_load_cell_from(
    sprite, pose, direction, layeri, source, size, where, duration);
  if (!cell) return -1;
  return 1;
}


/* Loads a single row and column of a sprite layout from the given image
 * into the given layer for a stand in walk cell. */
int spritelayout_load_rc_standinwalk(
  SpriteLayout * layout, Sprite * sprite, Image * source, 
  int layeri, int rowi, int coli, int pose, int direction) {
  
  Point where           = bevec(layout->size_x * rowi, layout->size_y * coli);
  Point size            = bevec(layout->size_x, layout->size_y);
  double duration       = layout->row_duration[rowi];
  SpriteCell * cell     =  sprite_load_cell_from(
    sprite, SPRITE_STAND, direction, layeri, source, size, where, duration);
  (void) pose;
  if (!cell) return -1;
  return 1;
}



/* Loads a single row and column of a sprite layout from the given image
 * into the given layer. */
int spritelayout_load_rc(
  SpriteLayout * layout, Sprite * sprite, Image * source, 
  int layeri, int rowi, int coli) {
  int pose              = layout->row_type[rowi];
  int direction         = layout->row_dir[rowi];
  
  /* special case for stand-in-walk cells. */
  if (   (layout->standinwalk >= 0) 
      && (coli == layout->standinwalk)
      && (pose == SPRITE_WALK)
     ) {
      return spritelayout_load_rc_standinwalk(
        layout, sprite, source, layeri, rowi, coli, pose, direction);       
  }
  /* Normal case */
  return spritelayout_load_rc_normal(
        layout, sprite, source, layeri, rowi, coli, pose, direction);
}



/* Loads a row of sprite cells into an action for the sprite, 
 * according to the layout. */
int spritelayout_load_row(
  SpriteLayout * layout, Sprite * sprite, Image * source, int layeri, int rowi){
  
  int col_max; 
  int coli;
  
  if (rowi > spritelayout_rows(layout)) return -1;
  
  col_max = layout->per_row[rowi];
  
  for (coli = 0 ; coli < col_max; coli++) {
    spritelayout_load_rc(layout, sprite, source, layeri, rowi, coli);
  }
  return col_max;
}

/* Loads sprite cells for a whole layer in the sprite. The layout info in the 
 * struct is used to correctly set up the sprite parts. 
 */ 
Sprite * spritelayout_load_layer
(SpriteLayout * layout, Sprite * sprite, Image * source, int layeri) {
  int stop;
  int rowi;

  stop        = spritelayout_rows(layout);
  for (rowi = 0 ; rowi < stop ; rowi++) {
    spritelayout_load_row(layout, sprite, source, layeri, rowi);
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





