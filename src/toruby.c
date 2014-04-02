/*
* toruby.c contains all functionality that Eruta exposes to 
* mruby. All functions are prefixed with tr_
*/

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


/* Documentation of mrb_get_args: 
 
  retrieve arguments from mrb_state.

  mrb_get_args(mrb, format, ...)

  returns number of arguments parsed.

  format specifiers:

   o: Object [mrb_value]
   S: String [mrb_value]
   A: Array [mrb_value]
   H: Hash [mrb_value]
   s: String [char*,int]
   z: String [char*]
   a: Array [mrb_value*,mrb_int]
   f: Float [mrb_float]
   i: Integer [mrb_int]
   b: Binary [int]
   n: Symbol [mrb_sym]
   &: Block [mrb_value]
   *: rest argument [mrb_value*,int]
   |: optional
 */



/**
* helper macros 
*/

#define RH_WRAP(RUBY, TYPE, OBJECT)                         \
mrb_obj_value(Data_Wrap_Struct(RUBY,                        \
              RUBY->object_class, &toruby_##TYPE, OBJECT));


void tr_Font_free(mrb_state * state, void * ptr) {
  printf("Freeing font %p\n", ptr);
  font_free((Font*)ptr);
}

struct mrb_data_type toruby_Font_type = { "Font", tr_Font_free };

/*
mrb_value toruby_Font(mrb_state* mrb, mrb_value sel, mrb_value rname) {
  Fonr * font = RSTRING(name);
}
*/

void tr_Path_free(mrb_state * state, void * ptr) {
  printf("Freeing path %p\n", ptr);
  al_destroy_path((Path*)ptr);
}

struct mrb_data_type toruby_Path = { "Path", tr_Path_free };

/* gets a data path based on a vpath. */
mrb_value tr_Path(Ruby * ruby, mrb_value self, struct RClass * klass) {
  ALLEGRO_PATH * path = NULL;
  const char * vpath = NULL; int vlen = 0; 
  mrb_get_args(ruby, "s", &vpath, &vlen);
  printf("Making path for : %s, %d", vpath, vlen);
  path = fifi_data_vpath(vpath);
  if(!path) return mrb_nil_value();
  return mrb_obj_value(Data_Wrap_Struct(ruby, klass, &toruby_Path, path));
}

/* Test method. */
static mrb_value tr_test(mrb_state * mrb, mrb_value self) {
  printf("Hello from a mruby test!\n");
  return self;
}


/* Stops the engine by calling state_done */
static mrb_value tr_state_done(mrb_state * mrb, mrb_value self) {
  State   * state   = NULL;
  state             = state_get();
  if (state) {
    state_done(state);
    return self;
  } else {
    return mrb_nil_value();
  }
}

/* Writes to console or to stdout if console is not available. */
static mrb_value tr_log(mrb_state * mrb, mrb_value self) {
  
  State   * state   = NULL;
  BBConsole * console = NULL;
  state             = state_get();
  if (state) {
    console = state_console(state);
  } else {
    puts("No state?!");
  }
  mrb_value text    = mrb_nil_value();
  mrb_get_args(mrb, "S", &text);
  if(console) {
    bbconsole_puts(console, RSTRING_PTR(text));
  } else {
    puts(RSTRING_PTR(text));
  }
  return self;
}

/* Loads another script from the script directory. Reports to the 
 console or stderr if no console available. */
static mrb_value tr_script(mrb_state * mrb, mrb_value self) {
  int res; 
  char * command;
  State * state         = state_get();
  BBConsole * console   = state_console(state);
  mrb_value text        = mrb_nil_value();
  mrb_get_args(mrb, "S", &text);
  command               = mrb_str_to_cstr(mrb, text);
  
  if(console) {
    res = rh_runfilename_console(console, command, mrb);
  } else {
    res = rh_runfilename_stderr(command, mrb);    
  }
  return mrb_fixnum_value(res);
}

/* Sets the active map for the engine   */
static mrb_value tr_active_map_(mrb_state * mrb, mrb_value self) {
  Tilemap * map = NULL;
  int id;
  State * state   = state_get();
  mrb_int index   = -1;
  mrb_get_args(mrb, "i", &index); 
  // Negative index means "disable the map" 
  id = state_active_map_id_(state, index);
  if (index < 0) {
    return mrb_nil_value();
  }
  return mrb_fixnum_value(id);
} 


/* Gets the active map for the state   */
static mrb_value tr_active_map(mrb_state * mrb, mrb_value self) {
  int id;
  State * state   = state_get();
  id = state_active_map_id(state);
  return mrb_fixnum_value(id);  
} 



/* Wraps an Allegro event for use in ruby into an mruby hash. */
static mrb_value tr_eventvalues(mrb_state * mrb   , ALLEGRO_EVENT * event, 
                          mrb_value * values, int size) {
  int result;
  mrb_value aid;
  aid = mrb_hash_new(mrb);
  // mrb_hash_set(mrb, aid, mrb_intern(mrb, "type"), );
  return aid;
}


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

static mrb_value tr_newthing(mrb_state * mrb, mrb_value self) {
  int thing  = -1;
  State * state    = state_get();
  int result;
  mrb_int   index, kind, x, y, z, w, h;
  mrb_get_args(mrb, "iiiiiii", &index, &kind, &x, &y, &z, &w, &h);  
  if ((index<0) || (kind < 0)) {
    return mrb_nil_value();
  } 
  thing = state_newthingindex(state, index, kind, x, y, z, w, h);
  return mrb_fixnum_value(thing);
}

static mrb_value tr_camera_track(mrb_state * mrb, mrb_value self) {
  State * state    = state_get();
  int result;
  mrb_int   thing_index;
  mrb_get_args(mrb, "i", &thing_index);  
  result = state_camera_track_(state, thing_index);
  return mrb_fixnum_value(result);
}

static mrb_value tr_lockin_maplayer(mrb_state * mrb, mrb_value self) {
  State * state    = state_get();
  int result;
  mrb_int   layer;
  mrb_get_args(mrb, "i", &layer);  
  result = state_lockin_maplayer(state, layer);
  return mrb_fixnum_value(result);
}

/* Obsolete, tile maps will be loaded through store. 
static mrb_value tr_loadtilemap_vpath(mrb_state * mrb, mrb_value self) {
  State * state    = state_get();
  int result;
  mrb_value rvpath;
  mrb_get_args(mrb, "S", &rvpath);  
  result = state_loadtilemap_vpath(state, mrb_str_to_cstr(mrb, rvpath));
  return mrb_fixnum_value(result);
}
*/

static mrb_value tr_thing_sprite_(mrb_state * mrb, mrb_value self) {
  State * state    = state_get();
  int result;
  mrb_int thing, sprite;
  mrb_get_args(mrb, "ii", &thing, &sprite);  
  result = state_thing_sprite_(state, thing, sprite);
  return mrb_fixnum_value(result);
}

static mrb_value tr_thing_pose_(mrb_state * mrb, mrb_value self) {
  State * state    = state_get();
  int result;
  mrb_int thing, pose;
  mrb_get_args(mrb, "ii", &thing, &pose);  
  result = state_thing_pose_(state, thing, pose);
  return mrb_fixnum_value(result);
}

static mrb_value tr_thing_direction_(mrb_state * mrb, mrb_value self) {
  State * state    = state_get();
  int result;
  mrb_int thing, direction;
  mrb_get_args(mrb, "ii", &thing, &direction);  
  result = state_thing_direction_(state, thing, direction);
  return mrb_fixnum_value(result);
}

static mrb_value tr_thing_v_(mrb_state * mrb, mrb_value self) {
  State * state    = state_get();
  Thing * thing    = NULL;
  int result;
  mrb_int thingid; mrb_float x, y;
  mrb_get_args(mrb, "iff", &thingid, &x, &y); 
  thing = state_thing(state, thingid);
  if (!thing) {
    return mrb_nil_value();
  }
  thing_v_(thing, bevec(x , y)); 
  return mrb_fixnum_value(thingid);
}

/* Converts a bevec to an array of 2 floats */
mrb_value bevec2mrb(mrb_state * mrb, BeVec vec) { 
    mrb_value vals[2];
    mrb_value marr;
    vals[0] = mrb_fixnum_value(vec.x);
    vals[1] = mrb_fixnum_value(vec.y);
    marr = mrb_ary_new_from_values(mrb, 2, vals);
    return marr;
}

static mrb_value tr_thing_v(mrb_state * mrb, mrb_value self) {
  State * state    = state_get();
  Thing * thing    = NULL;
  BeVec result;
  mrb_int thingid; mrb_float x, y;
  mrb_get_args(mrb, "i", &thingid); 
  thing = state_thing(state, thingid);
  if (!thing) {
    return mrb_nil_value();
  }
  result = thing_v(thing); 
  return bevec2mrb(mrb, result);
}

/* Allows to set a color of a loaded image as the mask color. Somewhat slow. */
static mrb_value tr_image_mask_to_alpha(mrb_state * mrb, mrb_value self) {
  State * state    = state_get();
  Thing * thing    = NULL;
  BeVec result;
  int store_index, r, g, b;
  mrb_int thingid; mrb_float x, y;
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
  mrb_get_args(mrb, "iiii", &store_index, &r, &g, &b); 
  return mrb_fixnum_value(state_image_average_to_alpha(state, store_index, r, g, b));
}



#ifdef OBSOLETE_ 

static mrb_value tr_actorindex_(mrb_state * mrb, mrb_value self) {
  State * state    = state_get();
  int result;
  mrb_int thing;
  mrb_get_args(mrb, "i", &thing);  
  result = state_actorindex_(state, thing);
  return mrb_fixnum_value(result);
}


static mrb_value tr_actorindex(mrb_state * mrb, mrb_value self) {
  State * state    = state_get();
  Thing * thing;
  mrb_int result;
  
  thing = state_actor(state);
  if (!thing) {
    return mrb_nil_value();
  }
  return mrb_fixnum_value(thing_id(thing));
}

#endif

/*
int state_newthingindex(State * state, int kind, 
                        int x, int y, int z, int w, int h);
int state_thing_sprite_(State * state, int thing_index, int sprite_index); 
int state_thing_pose_(State * state, int thing_index, int pose);
int state_thing_direction_(State * state, int thing_index, int direction);

int state_actorindex_(State * self, int thing_index);
Thing * state_actor(State * self);
*/

/*
tr_getornewsprite
tr_newsprite
tr_sprite
tr_sprite_loadulpcss
tr_newthing
tr_camera_track
tr_lockin_maplayer 
tr_loadtilemap_vpath
tr_thing_sprite_
tr_thing_pose_
tr_thing_direction_
tr_actorindex_
tr_actorindex
*/

static mrb_value tr_store_kind(mrb_state * mrb, mrb_value self) {
  mrb_int index = -1;
  mrb_get_args(mrb, "i", &index);  
  return mrb_fixnum_value(store_kind(index));
}

static mrb_value tr_store_load_bitmap(mrb_state * mrb, mrb_value self) {
  char * vpath = NULL; 
  mrb_int index    = -1;
  mrb_get_args(mrb, "iz", &index, &vpath);  
  return rh_bool_value(store_load_bitmap(index, (const char *) vpath));
}

static mrb_value tr_store_load_bitmap_flags(mrb_state * mrb, mrb_value self) {
  char * vpath = NULL; 
  mrb_int index    = -1;
  mrb_int flags    =  0;
  mrb_get_args(mrb, "izi", &index, &vpath, &flags);  
  return rh_bool_value(store_load_bitmap_flags(index, (const char *)vpath, flags));
}

static mrb_value tr_store_load_sample(mrb_state * mrb, mrb_value self) {
  char * vpath = NULL; 
  mrb_int index    = -1;
  mrb_get_args(mrb, "iz", &index, &vpath);  
  return rh_bool_value(store_load_sample(index, (const char *) vpath));
}

static mrb_value tr_store_load_audio_stream(mrb_state * mrb, mrb_value self) {
  char * vpath          = NULL; 
  mrb_int  index          =   -1;
  size_t buffer_count   =    0;
  int    samples        =    0;
  int    bc             =    0;
  mrb_get_args(mrb, "izii", &index, &vpath, &bc, &samples );
  buffer_count          = (size_t) bc;
  return rh_bool_value(
    store_load_audio_stream(index, (const char *)vpath, buffer_count, samples)
  );
}

static mrb_value tr_store_load_bitmap_font(mrb_state * mrb, mrb_value self) {
  char * vpath = NULL; 
  mrb_int index    = -1;
  mrb_get_args(mrb, "iz", &index, &vpath);  
  return rh_bool_value(store_load_bitmap_font(index, (const char *) vpath));
}

static mrb_value tr_store_load_bitmap_font_flags(mrb_state * mrb, mrb_value self) {
  char * vpath = NULL; 
  mrb_int index    = -1;
  mrb_int flags    =  0;
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
  mrb_get_args(mrb, "iz", &index, &vpath);  
  return rh_bool_value(store_load_tilemap(index, (const char *)vpath));
}


static mrb_value tr_store_drop(mrb_state * mrb, mrb_value self) {
  mrb_int index    = -1;
  mrb_get_args(mrb, "i", &index);  
  return rh_bool_value(store_drop(index));
}

static mrb_value tr_store_index_ok(mrb_state * mrb, mrb_value self) {
  mrb_int index    = -1;
  mrb_get_args(mrb, "i", &index);  
  return rh_bool_value(store_index_ok(index));
}

static mrb_value tr_store_max(mrb_state * mrb, mrb_value self) {
  return rh_bool_value(store_max());
}

static mrb_value tr_store_get_bitmap_format(mrb_state * mrb, mrb_value self) {
  mrb_int index    = -1;
  int     value    = -1;
  mrb_get_args(mrb, "i", &index);  
  if(store_get_bitmap_format(index, &value)) return mrb_fixnum_value(value);
  return mrb_nil_value();
}

static mrb_value tr_store_get_bitmap_flags(mrb_state * mrb, mrb_value self) {
  mrb_int index    = -1;
  int     value    = -1;
  mrb_get_args(mrb, "i", &index);  
  if(store_get_bitmap_flags(index, &value)) return mrb_fixnum_value(value);
  return mrb_nil_value();
}

static mrb_value tr_store_get_bitmap_height(mrb_state * mrb, mrb_value self) {
  mrb_int index    = -1;
  int     value    = -1;
  mrb_get_args(mrb, "i", &index);  
  if(store_get_bitmap_height(index, &value)) return mrb_fixnum_value(value);
  return mrb_nil_value();
}

static mrb_value tr_store_get_bitmap_width(mrb_state * mrb, mrb_value self) {
  mrb_int index    = -1;
  int     value    = -1;
  mrb_get_args(mrb, "i", &index);  
  if(store_get_bitmap_width(index, &value)) return mrb_fixnum_value(value);
  return mrb_nil_value();
}

static mrb_value tr_store_get_text_width(mrb_state * mrb, mrb_value self) {
  mrb_int index    = -1;
  int     value    = -1;
  char  * text     = NULL;     
  mrb_get_args(mrb, "iz", &index, &text);  
  if(store_get_text_width(index, text, &value)) return mrb_fixnum_value(value);
  return mrb_nil_value();
}

static mrb_value tr_store_get_text_dimensions(mrb_state * mrb, mrb_value self) {
  mrb_int index    = -1;
  Rebox   value    ;
  char  * text     = NULL;     
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
  mrb_get_args(mrb, "i", &index);  
  if(store_get_font_line_height(index, &value)) return mrb_fixnum_value(value);
  return mrb_nil_value();
}

static mrb_value tr_store_get_font_ascent(mrb_state * mrb, mrb_value self) {
  mrb_int index    = -1;
  int     value    = -1;
  mrb_get_args(mrb, "i", &index);  
  if(store_get_font_ascent(index, &value)) return mrb_fixnum_value(value);
  return mrb_nil_value();
}


static mrb_value tr_store_get_font_descent(mrb_state * mrb, mrb_value self) {
  mrb_int index    = -1;
  int     value    = -1;
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



#define TORUBY_0_ICALLER(NAME, TOCALL)                                       \
  static mrb_value NAME(mrb_state * mrb, mrb_value self) {                  \
  return mrb_fixnum_value(TOCALL());                                        \
  }

TORUBY_0_ICALLER(tr_scegra_nodes_max, scegra_nodes_max)

#define TORUBY_0_FGETTER(NAME, TOCALL)                                      \
  static mrb_value NAME(mrb_state * mrb, mrb_value self) {                  \
  return mrb_float_value(mrb, TOCALL());                                    \
  }

TORUBY_0_FGETTER(tr_get_time, al_get_time)



#define SCEGRA_ICALLER(NAME, TOCALL)                                        \
  static mrb_value NAME(mrb_state * mrb, mrb_value self) {                  \
  int index;                                                                \
  mrb_get_args(mrb, "i", &index);                                           \
  return mrb_fixnum_value(TOCALL(index));                                   \
  }

SCEGRA_ICALLER(tr_scegra_z, scegra_z)
SCEGRA_ICALLER(tr_scegra_disable_node, scegra_disable_node)
SCEGRA_ICALLER(tr_scegra_get_id, scegra_get_id)
SCEGRA_ICALLER(tr_scegra_out_of_bounds, scegra_out_of_bounds)


#define SCEGRA_ISETTER(NAME, TOCALL)                                        \
  static mrb_value NAME(mrb_state * mrb, mrb_value self) {                  \
  int index, value;                                                         \
  mrb_get_args(mrb, "ii", &index, &value);                                  \
  return mrb_fixnum_value(TOCALL(index, value));                            \
  }

SCEGRA_ISETTER(tr_scegra_z_, scegra_z_)
SCEGRA_ISETTER(tr_scegra_visible_, scegra_visible_)
SCEGRA_ISETTER(tr_scegra_image_id_, scegra_image_id_)
SCEGRA_ISETTER(tr_scegra_font_id_, scegra_font_id_)
SCEGRA_ISETTER(tr_scegra_background_image_id_, scegra_background_image_id_)
SCEGRA_ISETTER(tr_scegra_border_thickness_, scegra_border_thickness_)





#define SCEGRA_PSETTER(NAME, TOCALL)                                         \
  static mrb_value NAME(mrb_state * mrb, mrb_value self) {                   \
  int index, x, y;                                                           \
  mrb_get_args(mrb, "iii", &index, &x, &y);                                  \
  return mrb_fixnum_value(TOCALL(index, x, y));                              \
  }

SCEGRA_PSETTER(tr_scegra_speed_, scegra_speed_)
SCEGRA_PSETTER(tr_scegra_size_ , scegra_size_)
SCEGRA_PSETTER(tr_scegra_position_, scegra_position_)
  
#define SCEGRA_CSETTER(NAME, TOCALL)                                         \
  static mrb_value NAME(mrb_state * mrb, mrb_value self) {                   \
  int index, r, g, b, a;                                                     \
  mrb_get_args(mrb, "iiiii", &index, &r, &g, &b, &a);                        \
  return mrb_fixnum_value(TOCALL(index, r, g, b, a));                        \
  }

SCEGRA_CSETTER(tr_scegra_background_color_, scegra_background_color_)
SCEGRA_CSETTER(tr_scegra_border_color_, scegra_border_color_)
SCEGRA_CSETTER(tr_scegra_color_, scegra_color_)
  
#define SCEGRA_FSETTER(NAME, TOCALL)                                        \
  static mrb_value NAME(mrb_state * mrb, mrb_value self) {                  \
  int index; mrb_float value;                                               \
  mrb_get_args(mrb, "if", &index, &value);                                  \
  return mrb_fixnum_value(TOCALL(index, value));                            \
  }

SCEGRA_FSETTER(tr_scegra_angle_, scegra_angle_)

static mrb_value tr_scegra_make_box(mrb_state * mrb, mrb_value self) {
  mrb_int id            = -1, sindex = -1;
  mrb_int x             =  0, y      =  0;
  mrb_int w             = 32, h      = 32;
  mrb_int rx            =  4, ry     =  4;  
  mrb_get_args(mrb, "iiiiiiii", &id, &x, &y, &w, &h, &rx, &ry, &sindex);
  return mrb_fixnum_value(scegra_make_box_style_from(id, bevec(x, y), bevec(w, h), bevec(rx, ry), sindex));
}

static mrb_value tr_scegra_make_text(mrb_state * mrb, mrb_value self) {
  char * str            = NULL;
  mrb_int id            = -1, size   =  0, sindex = -1;
  mrb_int x             =  0, y      =  0;
  mrb_get_args(mrb, "iiisi", &id, &x, &y, &str, &size, &sindex);
  return mrb_fixnum_value(scegra_make_text_style_from(id, bevec(x, y), str, sindex));
}

static mrb_value tr_scegra_make_image(mrb_state * mrb, mrb_value self) {
  char * str            = NULL;
  mrb_int id            = -1, image_id  =  -1, sindex = -1;
  mrb_int x             =  0, y      =  0;
  mrb_get_args(mrb, "iiiii", &id, &x, &y, &image_id, &sindex);
  return mrb_fixnum_value(scegra_make_image_style_from(id, bevec(x, y), image_id, sindex));
}

#define TR_WRAP_NOARG_BOOL(NAME, TOCALL)                                       \
static mrb_value NAME(mrb_state * mrb, mrb_value self) {                       \
  return rh_bool_value(TOCALL());                                              \
}

#define TR_WRAP_I_BOOL(NAME, TOCALL)                                           \
static mrb_value NAME(mrb_state * mrb, mrb_value self) {                       \
  mrb_int i1;                                                                  \
  mrb_get_args(mrb, "i", &i1);                                                 \
  return rh_bool_value(TOCALL(i1));                                            \
}

#define TR_WRAP_B_BOOL(NAME, TOCALL)                                            \
static mrb_value NAME(mrb_state * mrb, mrb_value self) {                       \
  mrb_value b1;                                                                \
  mrb_get_args(mrb, "o", &b1);                                                 \
  return rh_bool_value(TOCALL(rh_tobool(b1)));                                 \
}

#define TR_WRAP_I_INT(NAME, TOCALL)                                             \
static mrb_value NAME(mrb_state * mrb, mrb_value self) {                       \
  mrb_int i1;                                                                  \
  mrb_get_args(mrb, "i", &i1);                                                 \
  return mrb_fixnum_value(TOCALL(i1));                                          \
}

#define TR_WRAP_II_INT(NAME, TOCALL)                                           \
static mrb_value NAME(mrb_state * mrb, mrb_value self) {                      \
  mrb_int i1, i2;                                                             \
  mrb_get_args(mrb, "ii", &i1, &i2);                                          \
  return mrb_fixnum_value(TOCALL(i1, i2));                                     \
}

#define TR_WRAP_III_INT(NAME, TOCALL)                                          \
static mrb_value NAME(mrb_state * mrb, mrb_value self) {                      \
  mrb_int i1, i2, i3;                                                         \
  mrb_get_args(mrb, "iii", &i1, &i2, &i3)                                     \
  return mrb_fixnum_value(TOCALL(i1, i2, i3));                                 \
}


#define TR_WRAP_IIII_INT(NAME, TOCALL)                                          \
static mrb_value NAME(mrb_state * mrb, mrb_value self) {                       \
  mrb_int i1, i2, i3, i4;                                                      \
  mrb_get_args(mrb, "iiii", &i1, &i2, &i3, &i4);                               \
  return mrb_fixnum_value(TOCALL(i1, i2, i3, i4));                              \
}

#define TR_WRAP_IIIIB_INT(NAME, TOCALL)                                         \
static mrb_value NAME(mrb_state * mrb, mrb_value self) {                       \
  mrb_int i1, i2, i3, i4;                                                      \
  mrb_value b5;                                                                \
  mrb_get_args(mrb, "iiiib", &i1, &i2, &i3, &i4, &b5);                         \
  return mrb_fixnum_value(TOCALL(i1, i2, i3, i4, rh_tobool(b5)));               \
}


#define TR_WRAP_NOARG_INT(NAME, TOCALL)                                         \
static mrb_value NAME(mrb_state * mrb, mrb_value self) {                       \
  return mrb_fixnum_value(TOCALL());                                            \
}


TR_WRAP_NOARG_BOOL(tr_show_fps, global_state_show_fps)
TR_WRAP_NOARG_BOOL(tr_show_graph, global_state_show_graph)
TR_WRAP_NOARG_BOOL(tr_show_area, global_state_show_area)
TR_WRAP_B_BOOL(tr_show_fps_, global_state_show_fps_)
TR_WRAP_B_BOOL(tr_show_graph_, global_state_show_graph_)
TR_WRAP_B_BOOL(tr_show_area_, global_state_show_area_)
TR_WRAP_B_BOOL(tr_show_mouse_cursor_, scegra_show_system_mouse_cursor)



TR_WRAP_NOARG_INT(tr_playing_samples_max, audio_playing_samples_max)
TR_WRAP_IIIIB_INT(tr_play_sound_ex, audio_play_sound_ex)
TR_WRAP_I_INT(tr_play_sound, audio_play_sound)
TR_WRAP_I_BOOL(tr_stop_sound,audio_stop_sound)

TR_WRAP_I_BOOL(tr_set_music, audio_set_music)
TR_WRAP_NOARG_BOOL(tr_play_music, audio_play_music)
TR_WRAP_NOARG_BOOL(tr_stop_music, audio_stop_music)
TR_WRAP_NOARG_BOOL(tr_music_playing_p, audio_music_playing_p)

TR_WRAP_II_INT(tr_scegra_image_flags_, scegra_image_flags_);
TR_WRAP_II_INT(tr_scegra_text_flags_,  scegra_text_flags_);


#define TR_METHOD(MRB, CLASS, NAME, IMPL, FLAGS)\
        mrb_define_method((MRB), (CLASS), (NAME), (IMPL), (FLAGS))

#define TR_METHOD_ARGC(MRB, CLASS, NAME, IMPL, ARGC)\
        mrb_define_method((MRB), (CLASS), (NAME), (IMPL), ARGS_REQ(ARGC))

#define TR_METHOD_NOARG(MRB, CLASS, NAME, IMPL)\
        mrb_define_method((MRB), (CLASS), (NAME), (IMPL), ARGS_NONE())

#define TR_CLASS_METHOD(MRB, CLASS, NAME, IMPL, FLAGS)\
        mrb_define_class_method((MRB), (CLASS), (NAME), (IMPL), (FLAGS))

#define TR_CLASS_METHOD_ARGC(MRB, CLASS, NAME, IMPL, ARGC)\
        mrb_define_class_method((MRB), (CLASS), (NAME), (IMPL), ARGS_REQ(ARGC))

#define TR_CLASS_METHOD_NOARG(MRB, CLASS, NAME, IMPL)\
        mrb_define_class_method((MRB), (CLASS), (NAME), (IMPL), ARGS_NONE())

#define TR_CONST_INT(MRB, CLASS, NAME, VALUE) \
      mrb_define_const((MRB), (CLASS), (NAME), mrb_fixnum_value(VALUE))
        

/* Initializes the functionality that Eruta exposes to Ruby. */
int tr_init(mrb_state * mrb) {
  // luaL_dostring(lua, "print 'Hello!' ");
  struct RClass *krn;
  struct RClass *pth;
  struct RClass *sto;
  struct RClass *gra;
  struct RClass *spr;
  struct RClass *thi;
  struct RClass *eru;
  struct RClass *aud;
  
  eru = mrb_define_module(mrb, "Eruta");
  pth = mrb_define_class_under(mrb, eru, "Path"  , mrb->object_class);
  MRB_SET_INSTANCE_TT(pth, MRB_TT_DATA);
  /* Storage. */
  sto = mrb_define_class_under(mrb, eru, "Store" , mrb->object_class);
  /* Scene graph. */
  gra = mrb_define_class_under(mrb, eru, "Graph" , mrb->object_class);
  /* Entities. */
  thi = mrb_define_class_under(mrb, eru, "Thing" , mrb->object_class);
  /* Sprites */
  spr = mrb_define_class_under(mrb, eru, "Sprite", mrb->object_class);
  /* Audio */
  aud = mrb_define_class_under(mrb, eru, "Audio" , mrb->object_class);
  
  
  /* Define some constants. */
  TR_CONST_INT(mrb, eru, "FLIP_HORIZONTAL", ALLEGRO_FLIP_HORIZONTAL); 
  TR_CONST_INT(mrb, eru, "FLIP_VERTICAL"  , ALLEGRO_FLIP_VERTICAL); 
  TR_CONST_INT(mrb, eru, "ALIGN_LEFT"     , ALLEGRO_ALIGN_LEFT);
  TR_CONST_INT(mrb, eru, "ALIGN_CENTRE"   , ALLEGRO_ALIGN_CENTRE);
  TR_CONST_INT(mrb, eru, "ALIGN_CENTER"   , ALLEGRO_ALIGN_CENTER);
  TR_CONST_INT(mrb, eru, "ALIGN_RIGHT"    , ALLEGRO_ALIGN_RIGHT);
  TR_CONST_INT(mrb, eru, "ALIGN_INTEGER"  , ALLEGRO_ALIGN_INTEGER);
  TR_CLASS_METHOD_NOARG(mrb, eru, "quit"  , tr_state_done);
  
  
  
  krn = mrb_module_get(mrb, "Kernel");
  if(!krn) return -1;
  TR_METHOD_ARGC(mrb, krn, "test",  tr_test, 1);
  TR_METHOD_ARGC(mrb, krn, "log" , tr_log , 1);
  TR_METHOD_ARGC(mrb, krn, "script" , tr_script , 1);
  TR_METHOD_ARGC(mrb, krn, "sprite_getornew", tr_getornewsprite, 1);
  TR_METHOD_ARGC(mrb, krn, "sprite_new", tr_newsprite, 1);
  TR_METHOD_ARGC(mrb, krn, "sprite_get", tr_sprite, 1);
  TR_METHOD_ARGC(mrb, krn, "sprite_loadulpcss", tr_sprite_loadulpcss, 3);
  TR_METHOD_ARGC(mrb, krn, "sprite_tint_rgba", tr_sprite_tint, 6);
  TR_METHOD_ARGC(mrb, krn, "thing_new"    , tr_newthing, 7);
  TR_METHOD_ARGC(mrb, krn, "camera_track" , tr_camera_track, 1);
  TR_METHOD_ARGC(mrb, krn, "camera_lockin", tr_lockin_maplayer, 1);
  /* TR_METHOD_ARGC(mrb, krn, "tilemap_load" , tr_loadtilemap_vpath, 1); */
  TR_METHOD_ARGC(mrb, krn, "thing_sprite_", tr_thing_sprite_, 2);
  TR_METHOD_ARGC(mrb, krn, "thing_pose_"  , tr_thing_pose_, 2);
  TR_METHOD_ARGC(mrb, krn, "thing_direction_", tr_thing_direction_, 2);
  TR_METHOD_NOARG(mrb, krn, "active_map", tr_active_map);
  TR_METHOD_ARGC(mrb, krn, "active_map_", tr_active_map_, 1);
  
  TR_CLASS_METHOD_NOARG(mrb, eru, "active_map", tr_active_map);
  TR_CLASS_METHOD_ARGC(mrb, eru, "active_map_", tr_active_map_, 1);
  TR_CLASS_METHOD_NOARG(mrb, eru, "show_fps", tr_show_fps);
  TR_CLASS_METHOD_NOARG(mrb, eru, "show_area", tr_show_area);
  TR_CLASS_METHOD_NOARG(mrb, eru, "show_graph", tr_show_graph);
  TR_CLASS_METHOD_ARGC(mrb, eru, "show_fps="  , tr_show_fps_, 1);
  TR_CLASS_METHOD_ARGC(mrb, eru, "show_area=" , tr_show_area_, 1);
  TR_CLASS_METHOD_ARGC(mrb, eru, "show_graph=", tr_show_graph_, 1);
  TR_CLASS_METHOD_ARGC(mrb, eru, "show_mouse_cursor=", tr_show_mouse_cursor_, 1);
  
  
  TR_CLASS_METHOD_NOARG(mrb, eru, "time", tr_get_time);
  
  /* 
  TR_METHOD_ARGC(mrb, krn, "actor_index_", tr_actorindex_, 1);
  TR_METHOD_NOARG(mrb, krn, "actor_index", tr_actorindex);
  */
  
  TR_CLASS_METHOD_ARGC(mrb, thi, "thing_new", tr_newthing, 7);
  TR_CLASS_METHOD_ARGC(mrb, thi, "v"        , tr_thing_v , 1);
  TR_CLASS_METHOD_ARGC(mrb, thi, "v_"       , tr_thing_v_, 3);
  
  TR_CLASS_METHOD_ARGC(mrb, spr, "get_or_new"    , tr_getornewsprite, 1);
  TR_CLASS_METHOD_ARGC(mrb, spr, "sprite_new"    , tr_newsprite, 1);
  TR_CLASS_METHOD_ARGC(mrb, spr, "get"           , tr_sprite, 1);
  TR_CLASS_METHOD_ARGC(mrb, spr, "load_ulpcss"   , tr_sprite_loadulpcss, 3);
  TR_CLASS_METHOD_ARGC(mrb, spr, "tint_rgba"     , tr_sprite_tint, 6);


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
  
  
  
  
  TR_CLASS_METHOD_ARGC(mrb, sto, "drop"           , tr_store_drop, 1);
  TR_CLASS_METHOD_ARGC(mrb, sto, "bitmap_flags"   , tr_store_get_bitmap_flags, 1);
  TR_CLASS_METHOD_ARGC(mrb, sto, "bitmap_width"   , tr_store_get_bitmap_width, 1);
  TR_CLASS_METHOD_ARGC(mrb, sto, "bitmap_height"  , tr_store_get_bitmap_height, 1);
  
  TR_CLASS_METHOD_ARGC(mrb, sto, "font_ascent"    , tr_store_get_font_ascent, 1);
  TR_CLASS_METHOD_ARGC(mrb, sto, "font_descent"   , tr_store_get_font_descent, 1);
  TR_CLASS_METHOD_ARGC(mrb, sto, "font_line_height", tr_store_get_font_line_height, 1);
  TR_CLASS_METHOD_ARGC(mrb, sto, "text_dimensions", tr_store_get_text_dimensions, 2);
  TR_CLASS_METHOD_ARGC(mrb, sto, "text_width"     , tr_store_get_text_width, 2);
  
  TR_CLASS_METHOD_ARGC(mrb, gra, "nodes_max"       , tr_scegra_nodes_max, 0);
  TR_CLASS_METHOD_ARGC(mrb, gra, "z"               , tr_scegra_z, 1);
  TR_CLASS_METHOD_ARGC(mrb, gra, "disable"         , tr_scegra_disable_node, 1);
  TR_CLASS_METHOD_ARGC(mrb, gra, "id"              , tr_scegra_get_id, 1);
  TR_CLASS_METHOD_ARGC(mrb, gra, "out_of_bounds?"  , tr_scegra_out_of_bounds, 1);
  
  TR_CLASS_METHOD_ARGC(mrb, gra, "z_"              , tr_scegra_z_, 2);
  TR_CLASS_METHOD_ARGC(mrb, gra, "visible_"        , tr_scegra_visible_, 2);
  TR_CLASS_METHOD_ARGC(mrb, gra, "image_"          , tr_scegra_image_id_, 2);
  TR_CLASS_METHOD_ARGC(mrb, gra, "font_"           , tr_scegra_font_id_, 2);
  TR_CLASS_METHOD_ARGC(mrb, gra, "background_image_", tr_scegra_background_image_id_, 2);
  TR_CLASS_METHOD_ARGC(mrb, gra, "border_thickness_", tr_scegra_border_thickness_, 2);
  TR_CLASS_METHOD_ARGC(mrb, gra, "speed_"          , tr_scegra_speed_, 3);
  TR_CLASS_METHOD_ARGC(mrb, gra, "size_"           , tr_scegra_size_, 3);
  TR_CLASS_METHOD_ARGC(mrb, gra, "position_"       , tr_scegra_position_, 3);
  TR_CLASS_METHOD_ARGC(mrb, gra, "speed_"          , tr_scegra_speed_, 3);
  
  TR_CLASS_METHOD_ARGC(mrb, gra, "image_flags_"      , tr_scegra_image_flags_, 2);
  TR_CLASS_METHOD_ARGC(mrb, gra, "text_flags_"       , tr_scegra_text_flags_ , 2);
  TR_CLASS_METHOD_ARGC(mrb, gra, "angle_"           , tr_scegra_angle_, 2);
  TR_CLASS_METHOD_ARGC(mrb, gra, "background_color_", tr_scegra_background_color_, 4);
  TR_CLASS_METHOD_ARGC(mrb, gra, "border_color_"    , tr_scegra_border_color_, 4);
  TR_CLASS_METHOD_ARGC(mrb, gra, "color_"           , tr_scegra_color_       , 4);
  TR_CLASS_METHOD_ARGC(mrb, gra, "make_box"         , tr_scegra_make_box,  8); 
  TR_CLASS_METHOD_ARGC(mrb, gra, "make_image"       , tr_scegra_make_image, 5); 
  TR_CLASS_METHOD_ARGC(mrb, gra, "make_text"        , tr_scegra_make_text, 5); 
  
  // Audio 
  TR_CLASS_METHOD_NOARG(mrb, aud, "playing_sounds_max", tr_playing_samples_max);
  TR_CLASS_METHOD_ARGC(mrb, aud , "play_sound_ex"     , tr_play_sound_ex, 4);
  TR_CLASS_METHOD_ARGC(mrb, aud , "play_sound"        , tr_play_sound, 1);
  TR_CLASS_METHOD_ARGC(mrb, aud , "stop_sound"        , tr_stop_sound, 1);
  TR_CLASS_METHOD_ARGC(mrb, aud , "music_id="         , tr_set_music, 1);
  TR_CLASS_METHOD_NOARG(mrb, aud , "play_music"       , tr_play_music);
  TR_CLASS_METHOD_NOARG(mrb, aud , "stop_music"       , tr_stop_music);
  TR_CLASS_METHOD_NOARG(mrb, aud , "music_playing?"   , tr_music_playing_p);
  
   
  // must restore gc area here ????
  mrb_gc_arena_restore(mrb, 0);
  
  return 0;
}








