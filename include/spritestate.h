#ifndef spritestate_H_INCLUDED
#define spritestate_H_INCLUDED

#define SPRITESTATE_LAYER_MAX  64
#define SPRITESTATE_ACTION_MAX 32


/* Dynamic information about a sprite layer. */

enum SpriteStateLayerFlags_ {
  SPRITESTATE_LAYER_HIDDEN = 1 << 8,
  SPRITESTATE_LAYER_TINTED = 1 << 9,
};

enum SpriteStateActionFlags_ {
  SPRITESTATE_ACTION_LOOP    = 1 << 10,
  SPRITESTATE_ACTION_ONESHOT = 1 << 11,
  SPRITESTATE_ACTION_STOP    = 1 << 12,
  SPRITESTATE_ACTION_ONESTOP = SPRITESTATE_ACTION_STOP | SPRITESTATE_ACTION_ONESHOT,
};


typedef struct SpriteStateLayer_ SpriteStateLayer;

/* State for a single sprite layer. */
struct SpriteStateLayer_ {
    Color tint;
    int   flags;
};

typedef struct SpriteStateAction_ SpriteStateAction;

/* State for a single sprite action. */
struct SpriteStateAction_ {
  int loop;
  int next_action;
  int done;
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
  /* It's not worth while to use dynamical memory for these, IMO. */
  SpriteStateLayer   layers[SPRITESTATE_LAYER_MAX];
  SpriteStateAction  actions[SPRITESTATE_ACTION_MAX];
};





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

int spritestate_tint_layer(SpriteState * self, int layer, Color color);
int spritestate_remove_tint_layer(SpriteState * self, int layer);
int spritestate_is_layer_tinted(SpriteState * self, int layer);
Color * spritestate_get_layer_tint(SpriteState * self, int layer);

int spritestate_set_layer_hidden(SpriteState * self, int layer, int hidden);
int spritestate_get_layer_hidden(SpriteState * self, int layer);
int spritestate_remove_tint_layer(SpriteState * self, int layer);
int spritestate_is_layer_tinted(SpriteState * self, int layer);
Color * spritestate_get_layer_tint(SpriteState * self, int layer);

int spritestate_set_action_loop(SpriteState * self, int action, int loopmode);
int spritestate_get_action_loop(SpriteState * self, int action);
int spritestate_is_action_done(SpriteState * self, int action);

int spritestate_set_pose_direction_loop(SpriteState * self, int pose, int direction, int loopmode);
int spritestate_get_pose_direction_loop(SpriteState * self, int pose, int direction);


Sprite * spritestate_sprite(SpriteState * self);

#endif




