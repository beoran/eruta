#ifndef spritelayout_H_INCLUDED
#define spritelayout_H_INCLUDED

/* The layout of the sprite. Each int * pointer field is a -1 terminated array. */ 
struct SpriteLayout_ {
  int * per_row;
  int * row_type;
  int * row_dir;
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

int spritelayout_actions(SpriteLayout * layout);

Sprite * spritelayout_loadactionlayer(SpriteLayout * layout, Sprite * sprite, 
  Image * source, int actionindex, int layerindex);
 
Sprite * spritelayout_loadlayer
(SpriteLayout * layout, Sprite * sprite, int layerindex, Image * source);

SpriteLayout * spritelayout_for(int load_type);


#endif




