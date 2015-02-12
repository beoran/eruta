
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
#include "tr_store.h"

static mrb_value tr_store_kind(mrb_state * mrb, mrb_value self) {
  mrb_int index = -1;
  (void) self;
  mrb_get_args(mrb, "i", &index);  
  return mrb_fixnum_value(store_kind(index));
}

static mrb_value tr_store_load_bitmap(mrb_state * mrb, mrb_value self) {
  char * vpath = NULL; 
  mrb_int index    = -1;
  (void) self;

  mrb_get_args(mrb, "iz", &index, &vpath);  
  return rh_bool_value(store_load_bitmap(index, (const char *) vpath));
}

static mrb_value tr_store_load_bitmap_flags(mrb_state * mrb, mrb_value self) {
  char * vpath = NULL; 
  mrb_int index    = -1;
  mrb_int flags    =  0;
  (void) self;

  mrb_get_args(mrb, "izi", &index, &vpath, &flags);  
  return rh_bool_value(store_load_bitmap_flags(index, (const char *)vpath, flags));
}

static mrb_value tr_store_load_sample(mrb_state * mrb, mrb_value self) {
  char * vpath = NULL; 
  mrb_int index    = -1;
  (void) self;

  mrb_get_args(mrb, "iz", &index, &vpath);  
  return rh_bool_value(store_load_sample(index, (const char *) vpath));
}

static mrb_value tr_store_load_audio_stream(mrb_state * mrb, mrb_value self) {
  char * vpath          = NULL; 
  mrb_int  index          =   -1;
  size_t buffer_count   =    0;
  int    samples        =    0;
  int    bc             =    0;
  (void) self;

  mrb_get_args(mrb, "izii", &index, &vpath, &bc, &samples );
  buffer_count          = (size_t) bc;
  return rh_bool_value(
    store_load_audio_stream(index, (const char *)vpath, buffer_count, samples)
  );
}

static mrb_value tr_store_load_bitmap_font(mrb_state * mrb, mrb_value self) {
  char * vpath = NULL; 
  mrb_int index    = -1;
  (void) self;

  mrb_get_args(mrb, "iz", &index, &vpath);  
  return rh_bool_value(store_load_bitmap_font(index, (const char *) vpath));
}

static mrb_value tr_store_load_bitmap_font_flags(mrb_state * mrb, mrb_value self) {
  char * vpath = NULL; 
  mrb_int index    = -1;
  mrb_int flags    =  0;
  (void) self;

  mrb_get_args(mrb, "izi", &index, &vpath, &flags);  
  return rh_bool_value(
    store_load_bitmap_font_flags(index, (const char *)vpath, flags)
  );
}


static mrb_value tr_store_load_ttf_font(mrb_state * mrb, mrb_value self) {
  char * vpath     = NULL; 
  mrb_int index    = -1;
  mrb_int h        = 10;  
  mrb_int flags    =  0;
  (void) self;

  mrb_get_args(mrb, "izii", &index, &vpath, &h, &flags);
  return rh_bool_value(
    store_load_ttf_font(index, (const char *)vpath, h, flags)
  );
}


static mrb_value tr_store_load_ttf_font_stretch(mrb_state * mrb, mrb_value self) {
  char * vpath     = NULL; 
  mrb_int index    = -1;
  mrb_int w        = 10;
  mrb_int h        = 10;  
  mrb_int flags    =  0;
  (void) self;

  mrb_get_args(mrb, "iziii", &index, &vpath, &w, &h, &flags);  
  return rh_bool_value(
    store_load_ttf_font_stretch(index, (const char *)vpath, w, h, flags)
  );
}

static mrb_value tr_store_load_tilemap(mrb_state * mrb, mrb_value self) {
  char * vpath     = NULL; 
  mrb_int index    = -1;
  mrb_int h        = 10;  
  mrb_int flags    =  0;
  (void) self;

  mrb_get_args(mrb, "iz", &index, &vpath);  
  return rh_bool_value(store_load_tilemap(index, (const char *)vpath));
}


static mrb_value tr_store_drop(mrb_state * mrb, mrb_value self) {
  mrb_int index    = -1;
  (void) self;
  mrb_get_args(mrb, "i", &index);  
  return rh_bool_value(store_drop(index));
}

static mrb_value tr_store_index_ok(mrb_state * mrb, mrb_value self) {
  mrb_int index    = -1;
  (void) self;
  mrb_get_args(mrb, "i", &index);  
  return rh_bool_value(store_index_ok(index));
}

static mrb_value tr_store_max(mrb_state * mrb, mrb_value self) {
  (void) self; (void) mrb;
  return rh_bool_value(store_max());
}

static mrb_value tr_store_get_bitmap_format(mrb_state * mrb, mrb_value self) {
  mrb_int index    = -1;
  int     value    = -1;
  (void) self;
  mrb_get_args(mrb, "i", &index);  
  if(store_get_bitmap_format(index, &value)) return mrb_fixnum_value(value);
  return mrb_nil_value();
}

static mrb_value tr_store_get_bitmap_flags(mrb_state * mrb, mrb_value self) {
  mrb_int index    = -1;
  int     value    = -1;
  (void) self;
  mrb_get_args(mrb, "i", &index);  
  if(store_get_bitmap_flags(index, &value)) return mrb_fixnum_value(value);
  return mrb_nil_value();
}

static mrb_value tr_store_get_bitmap_height(mrb_state * mrb, mrb_value self) {
  mrb_int index    = -1;
  int     value    = -1;
  (void) self;
  mrb_get_args(mrb, "i", &index);  
  if(store_get_bitmap_height(index, &value)) return mrb_fixnum_value(value);
  return mrb_nil_value();
}

static mrb_value tr_store_get_bitmap_width(mrb_state * mrb, mrb_value self) {
  mrb_int index    = -1;
  int     value    = -1;
  (void) self;
  mrb_get_args(mrb, "i", &index);  
  if(store_get_bitmap_width(index, &value)) return mrb_fixnum_value(value);
  return mrb_nil_value();
}

static mrb_value tr_store_get_text_width(mrb_state * mrb, mrb_value self) {
  mrb_int index    = -1;
  int     value    = -1;
  char  * text     = NULL;
  (void) self;
  mrb_get_args(mrb, "iz", &index, &text);  
  if(store_get_text_width(index, text, &value)) return mrb_fixnum_value(value);
  return mrb_nil_value();
}

static mrb_value tr_store_get_text_dimensions(mrb_state * mrb, mrb_value self) {
  mrb_int index    = -1;
  Rebox   value    ;
  char  * text     = NULL;
  (void) self;

  mrb_get_args(mrb, "iz", &index, &text);  
  if(store_get_text_dimensions(index, text, &value)) {
    mrb_value vals[4];
    mrb_value marr;
    vals[0] = mrb_fixnum_value(value.at.x);
    vals[1] = mrb_fixnum_value(value.at.y);
    vals[2] = mrb_fixnum_value(value.size.x);
    vals[3] = mrb_fixnum_value(value.size.y);
    marr = mrb_ary_new_from_values(mrb, 4, vals);
    return marr;
  } 
  return mrb_nil_value();
}

static mrb_value tr_store_get_font_line_height(mrb_state * mrb, mrb_value self) {
  mrb_int index    = -1;
  int     value    = -1;
  (void) self;

  mrb_get_args(mrb, "i", &index);  
  if(store_get_font_line_height(index, &value)) return mrb_fixnum_value(value);
  return mrb_nil_value();
}

static mrb_value tr_store_get_font_ascent(mrb_state * mrb, mrb_value self) {
  mrb_int index    = -1;
  int     value    = -1;
  (void) self;

  mrb_get_args(mrb, "i", &index);  
  if(store_get_font_ascent(index, &value)) return mrb_fixnum_value(value);
  return mrb_nil_value();
}


static mrb_value tr_store_get_font_descent(mrb_state * mrb, mrb_value self) {
  mrb_int index    = -1;
  int     value    = -1;
  (void) self;

  mrb_get_args(mrb, "i", &index);  
  if(store_get_font_descent(index, &value)) return mrb_fixnum_value(value);
  return mrb_nil_value();
}

static mrb_value tr_store_grab_font(mrb_state * mrb, mrb_value self) {
  mrb_int index         = -1;
  mrb_int bmp_index     = -1;
  mrb_int count         =  0;
  mrb_value * rarray    = NULL;
  int       * ranges    = NULL; 
  int     value         = -1;
  Resor * res           = NULL;
  int     i             ;
  (void) self;

  mrb_get_args(mrb, "iiA", &index, &bmp_index, &count, &rarray);
  if (count < 2) return mrb_nil_value();
  
  ranges = calloc(sizeof(int), count); 
  for (i = 0 ; i < count; i ++) {
    /** XXX: use correct API. */
    ranges[i] = (rarray[i].value.i);
  }
  res    = store_grab_font(index, bmp_index, count, ranges);
  free(ranges);
  
  return rh_bool_value(res);
} 

/* Allows to set a color of a loaded image as the mask color. Somewhat slow. */
static mrb_value tr_image_mask_to_alpha(mrb_state * mrb, mrb_value self) {
  State * state    = state_get();
  Thing * thing    = NULL;
  BeVec result;
  int store_index, r, g, b;
  mrb_int thingid; mrb_float x, y;
  (void) self;
  
  mrb_get_args(mrb, "iiii", &store_index, &r, &g, &b); 
  return mrb_fixnum_value(state_image_mask_to_alpha(state, store_index, r, g, b));
}

/* Allows to set the colors of a loaded image with the image itselmf acting as a mask. 
 * Somewhat slow. */
static mrb_value tr_image_average_to_alpha(mrb_state * mrb, mrb_value self) {
  State * state    = state_get();
  Thing * thing    = NULL;
  BeVec result;
  int store_index, r, g, b;
  mrb_int thingid; mrb_float x, y;
  (void) self;
  
  mrb_get_args(mrb, "iiii", &store_index, &r, &g, &b); 
  return mrb_fixnum_value(state_image_average_to_alpha(state, store_index, r, g, b));
}


TR_WRAP_I_INT(tr_store_get_unused_id, store_get_unused_id);


/** Initialize mruby bindings to data storage functionality.
 * Eru is the parent module, which is normally named "Eruta" on the
 * ruby side. */
int tr_store_init(mrb_state * mrb, struct RClass * eru) {
  struct RClass *sto;
  struct RClass *krn;
  
  /* Audio class/module and class/module methods. */
  sto = mrb_define_class_under(mrb, eru, "Store" , mrb->object_class);
  krn = mrb_module_get(mrb, "Kernel");

  
  if (krn) {
    TR_METHOD_ARGC(mrb, krn, "store_kind", tr_store_kind, 1);
    TR_METHOD_ARGC(mrb, krn, "load_bitmap", tr_store_load_bitmap, 2);
    TR_METHOD_ARGC(mrb, krn, "load_bitmap", tr_store_load_bitmap, 2);  
    TR_METHOD_ARGC(mrb, krn, "load_bitmap_flags", 
                         tr_store_load_bitmap_flags, 3);
    TR_METHOD_ARGC(mrb, krn, "load_audio_stream", tr_store_load_audio_stream, 4);
    TR_METHOD_ARGC(mrb, krn, "load_sample", tr_store_load_sample, 2);
    TR_METHOD_ARGC(mrb, krn, "load_ttf_font", tr_store_load_ttf_font, 4);
    TR_METHOD_ARGC(mrb, krn, "load_ttf_stretch", tr_store_load_ttf_font_stretch, 5);
    TR_METHOD_ARGC(mrb, krn, "load_bitmap_font", tr_store_load_bitmap_font, 2);
    TR_METHOD_ARGC(mrb, krn, "load_bitmap_font_flags", tr_store_load_bitmap, 3);
    TR_METHOD_ARGC(mrb, krn, "load_tilemap", tr_store_load_tilemap, 3);
  }
 
  
  TR_CLASS_METHOD_ARGC(mrb, sto, "kind", tr_store_kind, 1);
  TR_CLASS_METHOD_ARGC(mrb, sto, "load_bitmap", tr_store_load_bitmap, 2);  
  TR_CLASS_METHOD_ARGC(mrb, sto, "load_bitmap_flags", 
                       tr_store_load_bitmap_flags, 3);
  TR_CLASS_METHOD_ARGC(mrb, sto, "load_audio_stream", tr_store_load_audio_stream, 4);
  TR_CLASS_METHOD_ARGC(mrb, sto, "load_sample", tr_store_load_sample, 2);
  TR_CLASS_METHOD_ARGC(mrb, sto, "load_ttf_font", tr_store_load_ttf_font, 4);
  TR_CLASS_METHOD_ARGC(mrb, sto, "load_ttf_stretch", tr_store_load_ttf_font_stretch, 5);
  TR_CLASS_METHOD_ARGC(mrb, sto, "load_bitmap_font", tr_store_load_bitmap_font, 2);
  TR_CLASS_METHOD_ARGC(mrb, sto, "load_bitmap_font_flags", tr_store_load_bitmap, 3);
  TR_CLASS_METHOD_ARGC(mrb, sto, "load_tilemap", tr_store_load_tilemap, 2);
  TR_CLASS_METHOD_ARGC(mrb, sto, "mask_to_alpha", tr_image_mask_to_alpha, 4);
  TR_CLASS_METHOD_ARGC(mrb, sto, "average_to_alpha", tr_image_average_to_alpha, 4);
  
  
  
  
  TR_CLASS_METHOD_ARGC(mrb, sto, "drop"             , tr_store_drop, 1);
  TR_CLASS_METHOD_ARGC(mrb, sto, "bitmap_flags"     , tr_store_get_bitmap_flags, 1);
  TR_CLASS_METHOD_ARGC(mrb, sto, "bitmap_width"     , tr_store_get_bitmap_width, 1);
  TR_CLASS_METHOD_ARGC(mrb, sto, "bitmap_height"    , tr_store_get_bitmap_height, 1);
  
  TR_CLASS_METHOD_ARGC(mrb, sto, "font_ascent"      , tr_store_get_font_ascent, 1);
  TR_CLASS_METHOD_ARGC(mrb, sto, "font_descent"     , tr_store_get_font_descent, 1);
  TR_CLASS_METHOD_ARGC(mrb, sto, "font_line_height" , tr_store_get_font_line_height, 1);
  TR_CLASS_METHOD_ARGC(mrb, sto, "text_dimensions"  , tr_store_get_text_dimensions, 2);
  TR_CLASS_METHOD_ARGC(mrb, sto, "text_width"       , tr_store_get_text_width, 2);
  TR_CLASS_METHOD_ARGC(mrb, sto, "get_unused_id"    , tr_store_get_unused_id, 1);


  return 0;
}
  




