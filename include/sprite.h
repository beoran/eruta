#ifndef sprite_H_INCLUDED
#define sprite_H_INCLUDED

#include "eruta.h"
#include "image.h"

typedef struct SpriteCell_      SpriteCell;
typedef struct SpriteFrame_     SpriteFrame;
typedef struct SpriteAction_    SpriteAction;   
typedef struct Sprite_          Sprite;
typedef struct SpriteLayout_    SpriteLayout;
typedef struct SpriteState_     SpriteState;



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


/* Load type of sprites. */
enum SpriteLoadType_ {
  SPRITE_LOAD_ULPCSS_NORMAL             = 0,
  SPRITE_LOAD_ULPCSS_OVERSIZED_SLASH    = 1,
  SPRITE_LOAD_ULPCSS_OVERSIZED_STAB     = 2,
};


/* Amount of potential actions that a sprite has by default at creation. */
#define SPRITE_NACTIONS_DEFAULT 32

/* Amount of potential frame that a spriteaction has by default at creation. */
#define SPRITEACTION_NFRAMES_DEFAULT 16

/* Amount of potential layers that a spriteframe has by default at creation. */
#define SPRITEFRAME_NLAYERS_DEFAULT 16

int spriteframe_maxlayers(SpriteFrame * self);


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

/** Sprite cleanup walker decaration, needed for spritelist.c  */
void * sprite_cleanup_walker(void * data, void * extra);

void sprite_draw(Sprite * self, Point * at);
Sprite * sprite_now_(Sprite * self, int actionnow, int framenow);
void sprite_update(Sprite * self, double dt);

Sprite * sprite_tint_layer(Sprite * self, int layerindex, Color color);

SpriteFrame * spriteaction_newframe
  (SpriteAction * self, int index, double duration);

SpriteAction * sprite_newaction
  (Sprite * self, int actionindex, int type, int flags);

SpriteCell * sprite_load_cell_from
  (Sprite * self, int pose, int direction, int layeri, 
    Image * source, Point size, Point where, double duration);

SpriteFrame * sprite_newframe
  (Sprite * self, int actionindex, int frameindex, double duration);

Sprite * sprite_loadlayer_ulpcss_vpath
  (Sprite * self, int layerindex, char * vpath, int oversized);


double spriteframe_duration(SpriteFrame * me);

int spriteaction_is_pose(SpriteAction * self, int pose, int direction);


void spritecell_draw_tinted(SpriteCell * self, Point * at, Color tint);

void spritecell_draw(SpriteCell * self, Point * at);
 



#endif



