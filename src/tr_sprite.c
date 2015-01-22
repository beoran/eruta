

#include "eruta.h"
#include "toruby.h"
#include "rh.h"
#include "state.h"
#include "image.h"
#include "fifi.h"
#include "store.h"
#include "scegra.h"
#include "sound.h"
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


static mrb_value tr_sprite_loadulpcss(mrb_state * mrb, mrb_value self) {
  Sprite * sprite  = NULL;
  State * state    = state_get();
  int result;
  mrb_int   rindex = -1;
  mrb_int   rlayer = -1;
  mrb_value rvpath = mrb_nil_value();
  mrb_get_args(mrb, "iiS", &rindex, &rlayer, &rvpath); 
  if ((rindex<0) || (rlayer<0)) {
    return mrb_nil_value();
  }
  result = 
  state_sprite_loadulpcss(state, rindex, rlayer, mrb_str_to_cstr(mrb, rvpath));
  return mrb_fixnum_value(result);
}

static mrb_value tr_sprite_tint(mrb_state * mrb, mrb_value self) {
  State * state    = state_get();
  int result       = 0;
  mrb_int   rindex = -1;
  mrb_int   rlayer = -1;
  mrb_int   rr     = 255;
  mrb_int   rg     = 255;
  mrb_int   rb     = 255;
  mrb_int   ra     = 255;
  mrb_get_args(mrb, "iiiiii", &rindex, &rlayer, &rr, &rg, &rb, &ra); 
  if ((rindex<0) || (rlayer<0)) {
    return mrb_nil_value();
  }
  result = 
  state_sprite_tintlayer(state, rindex, rlayer, rr, rg, rb, ra);
  return mrb_fixnum_value(result);
}


TR_WRAP_I_INT(tr_sprite_get_unused_id, state_get_unused_sprite_id);



/** Initialize mruby bindings to sprite functionality.
 * Eru is the parent module, which is normally named "Eruta" on the
 * ruby side. */
int tr_sprite_init(mrb_state * mrb, struct RClass * eru) {
  struct RClass *spr;
  /* Audio class/module and class/module methods. */
  spr = mrb_define_class_under(mrb, eru, "Sprite" , mrb->object_class);

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
 
  
  TR_CLASS_METHOD_ARGC(mrb, spr, "get_or_new"    , tr_getornewsprite, 1);
  TR_CLASS_METHOD_ARGC(mrb, spr, "sprite_new"    , tr_newsprite, 1);
  TR_CLASS_METHOD_ARGC(mrb, spr, "get"           , tr_sprite, 1);
  TR_CLASS_METHOD_ARGC(mrb, spr, "load_ulpcss"   , tr_sprite_loadulpcss, 3);
  TR_CLASS_METHOD_ARGC(mrb, spr, "tint_rgba"     , tr_sprite_tint, 6);
  TR_CLASS_METHOD_ARGC(mrb, spr, "get_unused_id" , tr_sprite_get_unused_id, 1);



  return 0;
}
  








