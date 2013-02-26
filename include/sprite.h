#ifndef sprite_H_INCLUDED
#define sprite_H_INCLUDED

#include "image.h"

typedef struct SpriteLayer_     SpriteLayer;
typedef struct SpriteFrame_     SpriteFrame;
typedef struct SpriteAction_    SpriteAction;   
typedef struct Sprite_          Sprite;
typedef struct SpriteList_      SpriteList;
typedef struct SpriteLayout_    SpriteLayout;

/* The layout of the sprite. Each field is a -1 terminated array. */ 
struct SpriteLayout_ {
  int * per_row;
  int * row_type;
  int * row_dir;
};





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


/* Amount of potential actions that a sprite has by default at creation. */
#define SPRITE_NACTIONS_DEFAULT 16

/* Amount of potential frame that a spriteaction has by default at creation. */
#define SPRITEACTION_NFRAMES_DEFAULT 16

/* Amount of potential layers that a spriteframe has by default at creation. */
#define SPRITEFRAME_NLAYERS_DEFAULT 16


int sprite_maxactions(Sprite *self);
SpriteAction * sprite_action(Sprite *self, int index);
SpriteAction * sprite_action_(Sprite *self, int index, SpriteAction * action);
int sprite_frames(Sprite *self, int action);
SpriteFrame * sprite_frame(Sprite *self, int action, int index);
Sprite * sprite_initall(Sprite * self, int index, int nactions);
Sprite * sprite_init(Sprite * self, int index);
Sprite * sprite_done(Sprite * self);
Sprite * sprite_free(Sprite * self);
Sprite * sprite_alloc();
Sprite * sprite_new(int index);
SpriteFrame * spriteaction_newframe(SpriteAction * self, int index, 
                                    int flags, double duration);

/* Adds a new action to the sprite. Any old action at the 
 same index is freed. Returns the new action or NULL on error. */
SpriteAction * sprite_newaction(Sprite * self, int actionindex, int type, int flags);

/* Adds a frame to the sprite. The action must already exist. */
SpriteFrame * sprite_newframe(Sprite * self, int actionindex, int frameindex,
                              int flags, double duration);




#endif



