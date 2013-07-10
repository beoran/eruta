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

static mrb_value tr_loadtilemap_vpath(mrb_state * mrb, mrb_value self) {
  State * state    = state_get();
  int result;
  mrb_value rvpath;
  mrb_get_args(mrb, "S", &rvpath);  
  result = state_loadtilemap_vpath(state, mrb_str_to_cstr(mrb, rvpath));
  return mrb_fixnum_value(result);
}


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
  mrb_get_args(mrb, "iziii", &index, &vpath, &h, &flags);  
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


#define TR_METHOD(MRB, CLASS, NAME, IMPL, FLAGS)\
        mrb_define_method((MRB), (CLASS), (NAME), (IMPL), (FLAGS))

#define TR_CLASS_METHOD(MRB, CLASS, NAME, IMPL, FLAGS)\
        mrb_define_class_method((MRB), (CLASS), (NAME), (IMPL), (FLAGS))

        

/* Initializes the functionality that Eruta exposes to Ruby. */
int tr_init(mrb_state * mrb) {
  // luaL_dostring(lua, "print 'Hello!' ");
  struct RClass *krn;
  struct RClass *pth;
  struct RClass *sto;
  pth = mrb_define_class(mrb, "Path", mrb->object_class);
  MRB_SET_INSTANCE_TT(pth, MRB_TT_DATA);
  sto = mrb_define_class(mrb, "Store", mrb->object_class);

  
  krn = mrb_class_get(mrb, "Kernel");
  if(!krn) return -1;
  mrb_define_method(mrb, krn, "test", 
                    tr_test, ARGS_REQ(1));
  mrb_define_method(mrb, krn, "log" , tr_log , ARGS_REQ(1));
  mrb_define_method(mrb, krn, "script" , tr_script , ARGS_REQ(1));
  mrb_define_method(mrb, krn, "sprite_getornew", tr_getornewsprite, ARGS_REQ(1));
  mrb_define_method(mrb, krn, "sprite_new", tr_newsprite, ARGS_REQ(1));
  mrb_define_method(mrb, krn, "sprite_get", tr_sprite, ARGS_REQ(1));
  mrb_define_method(mrb, krn, "sprite_loadulpcss", tr_sprite_loadulpcss, ARGS_REQ(3));
  mrb_define_method(mrb, krn, "sprite_tint_rgba", tr_sprite_tint, ARGS_REQ(6));
  mrb_define_method(mrb, krn, "thing_new"    , tr_newthing, ARGS_REQ(7));
  mrb_define_method(mrb, krn, "camera_track" , tr_camera_track, ARGS_REQ(1));
  mrb_define_method(mrb, krn, "camera_lockin", tr_lockin_maplayer, ARGS_REQ(1));
  mrb_define_method(mrb, krn, "tilemap_load" , tr_loadtilemap_vpath, ARGS_REQ(1));
  mrb_define_method(mrb, krn, "thing_sprite_", tr_thing_sprite_, ARGS_REQ(2));
  mrb_define_method(mrb, krn, "thing_pose_"  , tr_thing_pose_, ARGS_REQ(2));
  mrb_define_method(mrb, krn, "thing_direction_", tr_thing_direction_, ARGS_REQ(2));
  mrb_define_method(mrb, krn, "actor_index_", tr_actorindex_, ARGS_REQ(1));
  mrb_define_method(mrb, krn, "actor_index", tr_actorindex, ARGS_NONE());

  mrb_define_method(mrb, krn, "store_kind", tr_store_kind, ARGS_REQ(1));
  mrb_define_method(mrb, krn, "load_bitmap", tr_store_load_bitmap, ARGS_REQ(2));
  
  TR_CLASS_METHOD(mrb, sto, "kind", tr_store_kind, ARGS_REQ(1) );
  TR_CLASS_METHOD(mrb, sto, "load_bitmap", tr_store_load_bitmap, ARGS_REQ(2) );
  TR_CLASS_METHOD(mrb, sto, "load_bitmap_flags", 
                  tr_store_load_bitmap_flags, ARGS_REQ(3) );
  TR_CLASS_METHOD(mrb, sto, "drop", tr_store_drop, ARGS_REQ(1));
  TR_CLASS_METHOD(mrb, sto, "bitmap_flags", tr_store_get_bitmap_flags, ARGS_REQ(1));
  TR_CLASS_METHOD(mrb, sto, "bitmap_width", tr_store_get_bitmap_width, ARGS_REQ(1));
  TR_CLASS_METHOD(mrb, sto, "bitmap_height", tr_store_get_bitmap_height, ARGS_REQ(1));
   
  // must restore gc area here ????
  mrb_gc_arena_restore(mrb, 0);
  
  return 0;
}








