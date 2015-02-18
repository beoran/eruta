#ifndef spritelayout_H_INCLUDED
#define spritelayout_H_INCLUDED

/* The layout of the sprite. Each int * pointer field should be ponted to 
 * an array of size rows.  
 * In general, the cells of a sprite are expected to be layed out on a sprite 
 * sheet in rows of equal height, and in columns of equal width. 
 */ 
struct SpriteLayout_ {
  int rows;
  int * per_row;
  int * row_type;
  int * row_dir;
  double * row_duration;
  /* The maximum sizes of the individual frames or layers.  */
  int size_x;
  int size_y;  
  /* If not negative, this means the standinwaltk'th frame of the 
   walking animation is in fact the standing position. This frame should be skipped 
   for walking and added to a separate standing position. 
   The standing actions will be placed after all other actions of the sprite.
   */
  int standinwalk;
  
};

#define DEFINE_SPRITELAYOUT(NAME, ROWS, PER_ROW, ROW_POSE, ROW_DIR,            \
                            SIZE_X, SIZE_Y, STANDINWALK)                       \
  struct SpriteLayout_ NAME = { ROWS  , PER_ROW, ROW_POSE, ROW_DIR,            \
                                SIZE_X, SIZE_Y , STANDINWALK } 

int spritelayout_rows(SpriteLayout * layout);

Sprite * spritelayout_loadactionlayer(SpriteLayout * layout, Sprite * sprite, 
  Image * source, int actionindex, int layerindex);
 
SpriteLayout * spritelayout_for(int load_type);

Sprite * spritelayout_load_layer
(SpriteLayout * layout, Sprite * sprite, Image * source, int layeri);


#endif




