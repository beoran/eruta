#ifndef sprite_H_INCLUDED
#define sprite_H_INCLUDED

#include "eruta.h"
#include "image.h"

/* Typedefs for the rather complex sprite subsystem... */

typedef struct SpriteLayer_     SpriteLayer;
typedef struct SpriteFrame_     SpriteFrame;
typedef struct SpriteAction_    SpriteAction;
typedef struct Sprite_          Sprite;
typedef struct SpriteData_      SpriteData;
typedef struct SpriteList_      SpriteList;
typedef struct SpriteLayout_    SpriteLayout;
typedef struct SpriteState_     SpriteState;


/* The layout of the sprite. Each pointer field is a -1 terminated array. */ 
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





/* Flags of a Sprite, frame or layer. */
enum SpriteFlags_ {
  /* Zero flag means sprite , frame, etc not in use. */
  SPRITE_EMPTY          = 0,
  /* Sprite elemnt is in use if this is set, not if not. */ 
  SPRITE_ACTIVE         = 1,
  /* Sprite element does not need to be updated. */
  SPRITE_FREEZE         = 2,
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

/* Sprite layer suggested uses. */
enum SpriteLayerKind_   { 
  SPRITELAYER_BEHINDBODY,
  SPRITELAYER_BODY,
  SPRITELAYER_HEAD,
  SPRITELAYER_EYES,
  SPRITELAYER_HAIR,
  SPRITELAYER_HANDS,
  SPRITELAYER_FEET,
  SPRITELAYER_LEGS,
  SPRITELAYER_TORSO,
  SPRITELAYER_ACCESSORIES,
  SPRITELAYER_WEAPONS, 
};




/* Amount of potential actions that a sprite has by default at creation. */
#define SPRITE_NACTIONS_DEFAULT 32

/* Amount of potential frame that a spriteaction has by default at creation. */
#define SPRITEACTION_NFRAMES_DEFAULT 16

/* Amount of potential layers that a spriteframe has by default at creation. */
#define SPRITEFRAME_NLAYERS_DEFAULT 16


int spritedata_maxactions(SpriteData *self);
SpriteAction * spritedata_action(SpriteData *self, int index);
SpriteAction * spritedata_action_(SpriteData *self, int index, SpriteAction * action);
int spritedata_frames(SpriteData *self, int action);
SpriteFrame * spritedata_frame(SpriteData *self, int action, int index);
SpriteData * spritedata_initall(SpriteData * self, int index, int nactions);
SpriteData * spritedata_init(SpriteData * self, int index);
SpriteData * spritedata_done(SpriteData * self);
SpriteData * spritedata_free(SpriteData * self);
SpriteData * spritedata_alloc();
SpriteData * spritedata_new(int index);

void spritedata_draw(SpriteData * self, Point * at);
SpriteData * spritedata_now_(SpriteData * self, int actionnow, int framenow);
void spritedata_update(SpriteData * self, double dt);


SpriteFrame * spriteaction_newframe
(SpriteAction * self, int index, int flags, double duration);
SpriteAction * spritedata_newaction
(SpriteData * self, int actionindex, int type, int flags);
SpriteFrame * spritedata_newframe
(SpriteData * self, int actionindex, int frameindex, int flags, double duration);
SpriteData * spritedata_loadlayer_ulpcss_vpath
(SpriteData * self, int layerindex, char * vpath, int oversized);

int spritedata_pose_(SpriteData * self, int pose, int direction);





#endif



