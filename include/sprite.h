#ifndef sprite_H_INCLUDED
#define sprite_H_INCLUDED

#include "eruta.h"
#include "image.h"

typedef struct SpriteLayer_     SpriteLayer;
typedef struct SpriteFrame_     SpriteFrame;
typedef struct SpriteAction_    SpriteAction;   
typedef struct Sprite_          Sprite;
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


/* SpriteState contains dynamic information about a sprite. This was   
 * separated out of Sprite to allow one sprite to be reused and shown differently 
 * by several Things. 
 */
struct SpriteState_ {
  Sprite           * sprite;
  SpriteAction     * action_now;
  SpriteFrame      * frame_now;
  int                frame_index;
  int                action_index;
  int                pose_now;
  int                direction_now;
  double             time;
  double             speedup;
};






/* Flags of a Sprite, frame or layer. */
enum SpriteFlags_ {
  /* Zero flag means sprite , frame, etc not in use. */
  SPRITE_EMPTY          = 0,
  /* Sprite element is in use if this is set, not if not. */ 
  SPRITE_ACTIVE         = 1,
  /* Sprite element does not need to be updated. */
  SPRITE_FREEZE         = 2,
  /* Direction flags */
  SPRITE_NO_DIRECTION   = 0,
  SPRITE_SOUTH          = 1 << 8,
  SPRITE_EAST           = 1 << 9,
  SPRITE_NORTH          = 1 << 10,
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
  SPRITELAYER_BEHINDBODY = 0,
  SPRITELAYER_BODY       = 1,
  SPRITELAYER_HEAD       = 2,
  SPRITELAYER_EYES       = 3,
  SPRITELAYER_HAIR       = 4,
  SPRITELAYER_HANDS      = 5,
  SPRITELAYER_FEET       = 6,
  SPRITELAYER_LEGS       = 7,
  SPRITELAYER_TORSO      = 8,
  SPRITELAYER_ACCESSORIES= 9,
  SPRITELAYER_WEAPONS    = 10, 
};




/* Amount of potential actions that a sprite has by default at creation. */
#define SPRITE_NACTIONS_DEFAULT 32

/* Amount of potential frame that a spriteaction has by default at creation. */
#define SPRITEACTION_NFRAMES_DEFAULT 16

/* Amount of potential layers that a spriteframe has by default at creation. */
#define SPRITEFRAME_NLAYERS_DEFAULT 16

/* Amount of potential sprites that a SpriteList can contain at creation. */
#define SPRITELIST_NSPRITES_DEFAULT 10000


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

void sprite_draw(Sprite * self, Point * at);
Sprite * sprite_now_(Sprite * self, int actionnow, int framenow);
void sprite_update(Sprite * self, double dt);


SpriteFrame * spriteaction_newframe
(SpriteAction * self, int index, int flags, double duration);
SpriteAction * sprite_newaction
(Sprite * self, int actionindex, int type, int flags);
SpriteFrame * sprite_newframe
(Sprite * self, int actionindex, int frameindex, int flags, double duration);
Sprite * sprite_loadlayer_ulpcss_vpath
(Sprite * self, int layerindex, char * vpath, int oversized);


SpriteState * spritestate_alloc();
Sprite      * spritestate_sprite_(SpriteState * self, Sprite * sprite);
SpriteState * spritestate_free(SpriteState * self);
Sprite      * spritestate_sprite(SpriteState * self);
SpriteState * spritestate_init(SpriteState * self, Sprite * sprite);
SpriteState * spritestate_done(SpriteState * self);
double        spritestate_speedup(SpriteState * self);
double        spritestate_speedup_(SpriteState * self, double speedup); 
SpriteState * spritestate_new(Sprite *  sprite);
void          spritestate_draw(SpriteState * self, Point * at);
SpriteState * spritestate_now_(SpriteState * self, int actionnow, int framenow);
void          spritestate_update(SpriteState * self, double dt);
int           spriteaction_ispose(SpriteAction * self, int pose, int direction);
int           spritestate_pose_(SpriteState * self, int pose);
int           spritestate_pose(SpriteState * self);
int           spritestate_direction(SpriteState * self);
int           spritestate_direction_(SpriteState * self, int direction);
int           spritestate_posedirection_
              (SpriteState * self, int pose, int direction);


/* Sprite list functions. */
SpriteList * spritelist_alloc();
SpriteList * spritelist_initall(SpriteList * self, int maxsprites);
SpriteList * spritelist_init(SpriteList * self);
SpriteList * spritelist_done(SpriteList * self);
SpriteList * spritelist_new();
SpriteList * spritelist_free(SpriteList * self);
Sprite     * spritelist_sprite(SpriteList * self, int index);
Sprite     * spritelist_sprite_(SpriteList * self, int index, Sprite * sprite);
Sprite     * spritelist_newsprite(SpriteList * self, int index);
Sprite     * spritelist_getornew(SpriteList * self, int index);
Sprite     * spritelist_loadlayer_ulpcss_vpath(
              SpriteList * self, int index,  int layerindex, char * vpath);

/* Applies a tint to a whole layer of a sprite. */
Sprite * sprite_tintlayer(Sprite * self, int layerindex, Color color);



#endif



