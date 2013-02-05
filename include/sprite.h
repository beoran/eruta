#ifndef sprite_H_INCLUDED
#define sprite_H_INCLUDED

#include "image.h"

typedef struct SpriteFrame_     SpriteFrame;
typedef struct SpriteAction_    SpriteAction;   
typedef struct Sprite_          Sprite;

/* The direction of a SpriteAction. */
enum SpriteActionDirection_ {
  SPRITE_NO_DIRECTION   = 0,
  SPRITE_SOUTH          = 1,
  SPRITE_EAST           = 2,
  SPRITE_NORTH          = 4,
  SPRITE_WEST           = 8,
  SPRITE_ALL            = SPRITE_SOUTH + SPRITE_EAST + SPRITE_NORTH + SPRITE_WEST
};

/* Type of the sprite action. */
enum SpriteActionType_ {
  SPRITE_STAND          = 1,
  SPRITE_WALK           = 2,
  SPRITE_RUN            = 3,
  SPRITE_ATTACK         = 4,
  SPRITE_HURT           = 5,
  SPRITE_DOWN           = 6,
  SPRITE_DEFEND         = 7,
  SPRITE_ARTS           = 8, 
}; 

Sprite * sprite_new();






#endif



