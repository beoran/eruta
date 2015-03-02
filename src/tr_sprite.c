

#include "eruta.h"
#include "toruby.h"
#include "rh.h"
#include "state.h"
#include "image.h"
#include "fifi.h"
#include "store.h"
#include "scegra.h"
#include "sound.h"
#include "spritestate.h"
#include <mruby/hash.h>
#include <mruby/class.h>
#include <mruby/data.h>
#include <mruby/array.h>
#include "tr_macro.h"
#include "tr_sprite.h"


static mrb_value tr_getornewsprite(mrb_state * mrb, mrb_value self) {
  Sprite * sprite = NULL;
  State * state   = state_get();
  mrb_int index   = -1;
  mrb_get_args(mrb, "i", &index);
  (void) self;

   
  if (index < 0) {
    return mrb_nil_value();
  }
  sprite = state_getornewsprite(state, index);
  if(!sprite) {
    return mrb_nil_value();
  }
  return mrb_fixnum_value(index);  
} 

static mrb_value tr_newsprite(mrb_state * mrb, mrb_value self) {
  Sprite * sprite = NULL;
  State * state   = state_get();
  mrb_int  index  = -1;
  mrb_get_args(mrb, "i", &index);
  (void) self;

  if (index < 1) {
    return mrb_nil_value();
  }
  sprite = state_newsprite(state, index);
  if(!sprite) {
    return mrb_nil_value();
  }
  return mrb_fixnum_value(index);
} 

static mrb_value tr_sprite(mrb_state * mrb, mrb_value self) {
  Sprite * sprite = NULL;
  State * state   = state_get();
  mrb_int index   = -1;
  (void) self;
  
  mrb_get_args(mrb, "i", &index); 
  if (index < 0) {
    return mrb_nil_value();
  }
  sprite = state_sprite(state, index);
  if(!sprite) {
    return mrb_nil_value();
  }
  return mrb_fixnum_value(index);
} 


static mrb_value tr_sprite_load_builtin(mrb_state * mrb, mrb_value self) {
  Sprite * sprite  = NULL;
  State * state    = state_get();
  char * vpath;
  int result;
  mrb_int   rindex = -1;
  mrb_int   rlayer = -1;
  mrb_int   rlayout=  SPRITE_LOAD_ULPCSS_NORMAL;
  (void) self;

  mrb_value rvpath = mrb_nil_value();
  mrb_get_args(mrb, "iiS|i", &rindex, &rlayer, &rvpath, &rlayout); 
  if ((rindex<0) || (rlayer<0)) {
    return mrb_nil_value();
  }
  vpath = mrb_str_to_cstr(mrb, rvpath);
  result =
  state_sprite_load_builtin(state, rindex, rlayer, vpath, rlayout);
  return mrb_fixnum_value(result);
}


TR_WRAP_I_INT(tr_sprite_get_unused_id, state_get_unused_sprite_id);

TR_SPRITE_II_INT(tr_sprite_action_index_for, sprite_action_index_for);

/** Initialize mruby bindings to sprite functionality.
 * Eru is the parent module, which is normally named "Eruta" on the
 * ruby side. */
int tr_sprite_init(mrb_state * mrb, struct RClass * eru) {
  struct RClass *spr;
  struct RClass *lay;
  struct RClass *sta;
 
  /* Sprite class/module and class/module methods. */
  spr = mrb_define_class_under(mrb, eru, "Sprite" , mrb->object_class);
  /* Layer sub- class/module and class/module methods. */
  lay = mrb_define_module_under(mrb, spr, "Layer");
  sta = mrb_define_module_under(mrb, spr, "State");

  /*
  TR_METHOD_ARGC(mrb, krn, "sprite_getornew", tr_getornewsprite, 1);
  TR_METHOD_ARGC(mrb, krn, "sprite_new", tr_newsprite, 1);
  TR_METHOD_ARGC(mrb, krn, "sprite_get", tr_sprite, 1);
  TR_METHOD_ARGC(mrb, krn, "sprite_loadulpcss", tr_sprite_loadulpcss, 3);
  TR_METHOD_ARGC(mrb, krn, "sprite_tint_rgba", tr_sprite_tint, 6);
  */

  /* Direction constants of a sprite. */
 
  TR_CONST_INT(mrb, spr, "NO_DIRECTION", SPRITE_NO_DIRECTION); 
  TR_CONST_INT(mrb, spr, "SOUTH", SPRITE_SOUTH); 
  TR_CONST_INT(mrb, spr, "EAST", SPRITE_EAST); 
  TR_CONST_INT(mrb, spr, "NORTH", SPRITE_NORTH); 
  TR_CONST_INT(mrb, spr, "WEST", SPRITE_WEST);
  TR_CONST_INT(mrb, spr, "ALL_DIRECTIONS", SPRITE_ALL);

  /* Action constants of a sprite */
  TR_CONST_INT_EASY(mrb, spr, SPRITE_, CAST);
  TR_CONST_INT_EASY(mrb, spr, SPRITE_, STAB);
  TR_CONST_INT_EASY(mrb, spr, SPRITE_, WALK);
  TR_CONST_INT_EASY(mrb, spr, SPRITE_, SLASH);
  TR_CONST_INT_EASY(mrb, spr, SPRITE_, SHOOT);
  TR_CONST_INT_EASY(mrb, spr, SPRITE_, DOWN);
  TR_CONST_INT_EASY(mrb, spr, SPRITE_, RUN);
  TR_CONST_INT_EASY(mrb, spr, SPRITE_, HURT);
  TR_CONST_INT_EASY(mrb, spr, SPRITE_, STAND);
  TR_CONST_INT_EASY(mrb, spr, SPRITE_, DEFEND);


  /* Sprite layer suggested uses. */
  TR_CONST_INT_EASY(mrb, lay, SPRITELAYER_, BEHINDBODY);
  TR_CONST_INT_EASY(mrb, lay, SPRITELAYER_, BODY);
  TR_CONST_INT_EASY(mrb, lay, SPRITELAYER_, HEAD);
  TR_CONST_INT_EASY(mrb, lay, SPRITELAYER_, EYES);
  TR_CONST_INT_EASY(mrb, lay, SPRITELAYER_, EARS);
  TR_CONST_INT_EASY(mrb, lay, SPRITELAYER_, HAIR);
  TR_CONST_INT_EASY(mrb, lay, SPRITELAYER_, HANDS);
  TR_CONST_INT_EASY(mrb, lay, SPRITELAYER_, FEET);
  TR_CONST_INT_EASY(mrb, lay, SPRITELAYER_, LEGS);
  TR_CONST_INT_EASY(mrb, lay, SPRITELAYER_, TORSO);
  TR_CONST_INT_EASY(mrb, lay, SPRITELAYER_, BELT);
  TR_CONST_INT_EASY(mrb, lay, SPRITELAYER_, ACCESSORY);
  TR_CONST_INT_EASY(mrb, lay, SPRITELAYER_, BLADE);
  TR_CONST_INT_EASY(mrb, lay, SPRITELAYER_, KNIFE);
  TR_CONST_INT_EASY(mrb, lay, SPRITELAYER_, CLAW);
  TR_CONST_INT_EASY(mrb, lay, SPRITELAYER_, STAFF);
  TR_CONST_INT_EASY(mrb, lay, SPRITELAYER_, GUN);
  TR_CONST_INT_EASY(mrb, lay, SPRITELAYER_, CANNON);
  TR_CONST_INT_EASY(mrb, lay, SPRITELAYER_, POLEARM);
  TR_CONST_INT_EASY(mrb, lay, SPRITELAYER_, BOW);
  
  
  /* Sprite state constants. */
  TR_CONST_INT_EASY(mrb, sta, SPRITESTATE_, ACTION_LOOP);
  TR_CONST_INT_EASY(mrb, sta, SPRITESTATE_, ACTION_ONESHOT);
  TR_CONST_INT_EASY(mrb, sta, SPRITESTATE_, ACTION_STOP);
  TR_CONST_INT_EASY(mrb, sta, SPRITESTATE_, ACTION_ONESTOP);
  TR_CONST_INT_EASY(mrb, sta, SPRITESTATE_, ACTION_MAX);
  TR_CONST_INT_EASY(mrb, sta, SPRITESTATE_, LAYER_MAX);
  
  

  /* Built in sprite layout constants. */
  TR_CONST_INT_EASY(mrb, spr, SPRITE_, LOAD_ULPCSS_NORMAL);
  TR_CONST_INT_EASY(mrb, spr, SPRITE_, LOAD_ULPCSS_OVERSIZED_STAB);
  TR_CONST_INT_EASY(mrb, spr, SPRITE_, LOAD_ULPCSS_OVERSIZED_SLASH);
   
  
  TR_CLASS_METHOD_ARGC(mrb, spr, "get_or_new"    , tr_getornewsprite, 1);
  TR_CLASS_METHOD_ARGC(mrb, spr, "sprite_new"    , tr_newsprite, 1);
  TR_CLASS_METHOD_ARGC(mrb, spr, "get"           , tr_sprite, 1);
  TR_CLASS_METHOD_OPTARG(mrb, spr, "load_builtin", tr_sprite_load_builtin, 3, 1);
  TR_CLASS_METHOD_ARGC(mrb, spr, "get_unused_id" , tr_sprite_get_unused_id, 1);
  TR_CLASS_METHOD_ARGC(mrb, spr, "action_id_for" , tr_sprite_action_index_for, 3);



  return 0;
}
  








