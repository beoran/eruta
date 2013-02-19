#ifndef sprite_H_INCLUDED
#define sprite_H_INCLUDED

#include "image.h"

typedef struct SpriteLayer_     SpriteLayer;
typedef struct SpriteFrame_     SpriteFrame;
typedef struct SpriteAction_    SpriteAction;   
typedef struct Sprite_          Sprite;
typedef struct SpriteList_      SpriteList;



/* Flags of a Sprite, frame or layer. */
enum SpriteFlags_ {
  /* Zero flag means sprite , frame, etc not in use. */
  SPRITE_EMPTY          = 0,
  /* Sprite elemnt is in use if this is set, not if not. */ 
  SPRITE_ACTIVE         = 1,
  /* Direction flags */
  SPRITE_NO_DIRECTION   = 0,
  SPRITE_SOUTH          = 1 << 8,
  SPRITE_EAST           = 1 << 9,
  SPRITE_NORTH          = 1 << 1,
  SPRITE_WEST           = 1 << 11,
  SPRITE_ALL            = SPRITE_SOUTH + SPRITE_EAST + SPRITE_NORTH + SPRITE_WEST
};

/* Type of the sprite action. The numbers correspond with the 
 * row in the liberated pixel cup sprite sheet compilation divided by 4, 
 * which is used  placeholder art now. The real art will have the same structure 
 * for it's sprite sheets.
 * 
 * To limit art requirements, emotions will be showed with emoticons and through
 * the character portraits in the dialogues.
 * 
 */
enum SpriteActionType_ {
  SPRITE_CAST           = 0,
  SPRITE_STAB           = 1,
  SPRITE_WALK           = 2,
  SPRITE_SLASH          = 3,
  SPRITE_SHOOT          = 4,
  SPRITE_DOWN           = 5, 
  
  /* The positions below are not in the LPC 
   * sprite sheet but could be synthesized or used in the real art. */  
  SPRITE_RUN            = 6,
  SPRITE_HURT           = 7,
  SPRITE_STAND          = 8,
  SPRITE_DEFEND         = 9,
}; 


Sprite * sprite_init(Sprite * self, int index, int nactions);
Sprite * sprite_done(Sprite * self);
Sprite * sprite_free(Sprite * self);
Sprite * sprite_alloc();
Sprite * sprite_new(int index, int nactions);











#endif



