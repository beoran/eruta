/*
* toruby.c helps expose functionality from C to Mruby for Eruta.
* All functions are prefixed with tr_.
* Look at the tr_*.c files.
* */

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
#include "tr_audio.h"
#include "tr_graph.h"
#include "tr_store.h"

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
   z: String [char*] nul terminated
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



TORUBY_0_FGETTER(tr_get_time, al_get_time)
TR_WRAP_NOARG_BOOL(tr_show_fps, global_state_show_fps)
TR_WRAP_NOARG_BOOL(tr_show_graph, global_state_show_graph)
TR_WRAP_NOARG_BOOL(tr_show_area, global_state_show_area)
TR_WRAP_B_BOOL(tr_show_fps_, global_state_show_fps_)
TR_WRAP_B_BOOL(tr_show_graph_, global_state_show_graph_)
TR_WRAP_B_BOOL(tr_show_area_, global_state_show_area_)
TR_WRAP_B_BOOL(tr_show_mouse_cursor_, scegra_show_system_mouse_cursor)



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
  TR_METHOD_ARGC(mrb, krn, "camera_track" , tr_camera_track, 1);
  TR_METHOD_ARGC(mrb, krn, "camera_lockin", tr_lockin_maplayer, 1);
  /*
  */ 
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
  

  
  /* Set up submodules. */
  tr_sprite_init(mrb, eru);
  tr_thing_init(mrb, eru);
  tr_store_init(mrb, eru);
  tr_graph_init(mrb, eru);
  tr_audio_init(mrb, eru);

   
  // must restore gc area here ????
  mrb_gc_arena_restore(mrb, 0);
  
  return 0;
}








