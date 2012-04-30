/* Functions that extend lua for use with Eruta. */
#include "lh.h" 
#include "eruta.h"
#include "fifi.h"
#include "image.h"
#include "state.h"
#include "event.h"



/* All functions have a fl prefix that stands for "Function for Lua" */



int fl_test(lua_State * lua) {
  printf("Hello from test!\n");
  return 0;
}



#define PP_DOJOIN(A, B) A##B
#define PP_JOIN(A, B) PP_DOJOIN(A, B)

#define FORLUA_CNAME(NAME) fl_##NAME
#define FORLUA_MNAME(NAME) _##NAME
#define FORLUA_NAME(CNAME, MNAME)  \
        PP_JOIN(FORLUA_CNAME(CNAME), FORLUA_MNAME(MNAME))

/* Helper macro for property getter bodies. 
* Note: I love the C preprocessor for generating this kind of boilerplate 
* code for me. :)
*/
#define FH_PROPERTY_GET(LUA, CNAME, PUSHWHAT, GETTER)                   \
  do {                                                                  \
  CNAME * self = NULL;                                                  \
  self         = lh_getself(lua, #CNAME);                               \
  if(!self)     { lua_pushnil(lua);                                 }   \
  else          { PUSHWHAT(lua, GETTER(self));                      }   \
  return 1;                                                             \
  } while(0)

/* Helper macro for destuctor bodies. */
#define FH_DESTROY(LUA, CNAME, DTOR)                                    \
  do {                                                                  \
  CNAME * self = NULL;                                                  \
  self         = lh_getself(lua, #CNAME);                               \
  printf("Destroyed : %s %p with %s\n", #CNAME, self, #DTOR);           \
  DTOR(self);                                                           \
  return 0;                                                             \
  } while(0)

/* This maco generates a destructor for use in Lua. */
#define FORLUA_DESTRUCTOR(CNAME, DTOR)                                  \
  int FORLUA_NAME(CNAME, gc)(Lua * lua) {                               \
    FH_DESTROY(lua, CNAME, DTOR);                                       \
  }
  
/* This macro declares a destructor.  */
#define FORLUA_HASDESTRUCTOR(LUA, CNAME)                                \
  lh_datainit(LUA, #CNAME, FORLUA_NAME(CNAME, gc))                      \

/* This maco generates a getter method for use in Lua. */
#define FORLUA_GETTER(CNAME, MNAME, PUSHWHAT, TOCALL)                   \
  int FORLUA_NAME(CNAME, MNAME)(Lua * lua) {                            \
    FH_PROPERTY_GET(lua, CNAME, PUSHWHAT, TOCALL);                      \
  }

/* this macro declares a method. */
#define FORLUA_HASMETHOD(LUA, CNAME, MNAME)                             \
  lh_datamethod(lua, #CNAME, #MNAME, FORLUA_NAME(CNAME, MNAME))
  
/* this macro declares a method with an alternate name. */
#define FORLUA_HASALTMETHOD(LUA, CNAME, MNAME, ALT)                     \
  lh_datamethod(lua, #CNAME, #ALT, FORLUA_NAME(CNAME, MNAME))
  
  
/* Wrapping ALLEGRO_PATH (Path) */
int FORLUA_NAME(Path, gc)(Lua * lua);

/* destructor for Path  */
FORLUA_DESTRUCTOR(Path, al_destroy_path);

/* to_s method for Path */
FORLUA_GETTER(Path, to_s, lua_pushstring, PATH_CSTR);

/* set up Path methods */
void fl_Path_initdata(Lua *lua) {
  FORLUA_HASDESTRUCTOR(lua, Path);
  FORLUA_HASMETHOD(lua, Path, to_s);
}

/* gets a data bath based on a vpath. */
int fl_PathForData(Lua * lua) {
  ALLEGRO_PATH * path = NULL;
  const char * vpath = NULL;
  lh_scanargs(lua, "s", &vpath);
  path = fifi_data_vpath(vpath);
  lh_pushdata(lua, "Path", path);
  return 1;
}

/* Wrapping ALLEGRO_IMAGE (Image) */
FORLUA_DESTRUCTOR(Image, image_free);

FORLUA_GETTER(Image, w, lua_pushinteger, image_w);
FORLUA_GETTER(Image, h, lua_pushinteger, image_h);
// FORLUA_GETTER(Image, w, lua_pushinteger, image_w);


void fl_Image_initdata(Lua *lua) {
  FORLUA_HASDESTRUCTOR(lua, Image);
  FORLUA_HASMETHOD(lua, Image, w);
  FORLUA_HASMETHOD(lua, Image, h);
  FORLUA_HASALTMETHOD(lua, Image, w, width);
  FORLUA_HASALTMETHOD(lua, Image, w, wide);
  FORLUA_HASALTMETHOD(lua, Image, h, height);
  FORLUA_HASALTMETHOD(lua, Image, h, high);
}

/** Loading of images. */
int fl_Image(Lua *lua) {
  Image * self = NULL;
  const char * vpath = NULL;
  if(lh_scanargs(lua, "s", &vpath) < 1) {
    lua_pushnil(lua); return 1;
  }
  self = fifi_loadbitmap_vpath(vpath);
  if(!self) {
    lua_pushnil(lua); return 1;
  }
  lh_pushdata(lua, "Image", self);
  return 1;
}

/* Wrapping ALLEGRO_FONT (Font) */
FORLUA_DESTRUCTOR(Font, font_free);

/* width of text.  */
int fl_Font_w(Lua * lua) {
  Font * self  = NULL;
  const char * text  = NULL;
  self         = lh_getself(lua, "Font");
  text         = lua_tolstring(lua, 2, NULL);
  if(!text)      { lua_pushinteger(lua, 0);                    }
  else if(!self) { lua_pushnil(lua);                           }
  else           { lua_pushinteger(lua, font_textwidth(self, text)); }
  return 1;
}

FORLUA_GETTER(Font, h, lua_pushinteger, font_lineheight);

void fl_Font_initdata(Lua *lua) {
  FORLUA_HASDESTRUCTOR(lua, Font);
  FORLUA_HASMETHOD(lua, Font, w);
  FORLUA_HASMETHOD(lua, Font, h);
  FORLUA_HASALTMETHOD(lua, Font, w, width);
  FORLUA_HASALTMETHOD(lua, Font, h, height);
  FORLUA_HASALTMETHOD(lua, Font, w, wide);
  FORLUA_HASALTMETHOD(lua, Font, h, high);
}

/* Load a font */
int fl_Font(Lua *lua) {
  Font * self = NULL;
  const char * name  = NULL;
  int size           = 12;
  int flags          = 0;
  if(lh_scanargs(lua, "sii", &name, &size, &flags) < 1) {
    lua_pushnil(lua); return 1;
  }
  self = fifi_loadfont(name, size, flags);
  if(!self) {
    lua_pushnil(lua); return 1;
  }
  lh_pushdata(lua, "Font", self);
  return 1;
}

/* wrap Event */
FORLUA_DESTRUCTOR(Event, event_free);

/* tons of getters */
FORLUA_GETTER(Event, type       , lua_pushinteger, event_type);
FORLUA_GETTER(Event, timestamp  , lua_pushnumber , event_timestamp);
FORLUA_GETTER(Event, ismouse    , lua_pushboolean, event_ismouse);
FORLUA_GETTER(Event, isdisplay  , lua_pushboolean, event_isdisplay);
FORLUA_GETTER(Event, isjoystick , lua_pushboolean, event_isjoystick);
FORLUA_GETTER(Event, iskeyboard , lua_pushboolean, event_iskeyboard);
FORLUA_GETTER(Event, istimer    , lua_pushboolean, event_istimer);
FORLUA_GETTER(Event, x          , lua_pushinteger, event_x);
FORLUA_GETTER(Event, y          , lua_pushinteger, event_y);
FORLUA_GETTER(Event, z          , lua_pushinteger, event_z);
FORLUA_GETTER(Event, w          , lua_pushinteger, event_w);
FORLUA_GETTER(Event, dx         , lua_pushinteger, event_dx);
FORLUA_GETTER(Event, dy         , lua_pushinteger, event_dy);
FORLUA_GETTER(Event, dz         , lua_pushinteger, event_dz);
FORLUA_GETTER(Event, dw         , lua_pushinteger, event_dw);
FORLUA_GETTER(Event, width      , lua_pushinteger, event_width);
FORLUA_GETTER(Event, height     , lua_pushinteger, event_height);
FORLUA_GETTER(Event, orientation, lua_pushinteger, event_orientation);
FORLUA_GETTER(Event, stick      , lua_pushinteger, event_stick);
FORLUA_GETTER(Event, axis       , lua_pushinteger, event_axis);
FORLUA_GETTER(Event, pos        , lua_pushnumber , event_pos);
FORLUA_GETTER(Event, button     , lua_pushinteger, event_button);
FORLUA_GETTER(Event, keycode    , lua_pushinteger, event_keycode);
FORLUA_GETTER(Event, unichar    , lua_pushinteger, event_unichar);
FORLUA_GETTER(Event, modifiers  , lua_pushinteger, event_modifiers);
FORLUA_GETTER(Event, repeat     , lua_pushboolean, event_repeat);
FORLUA_GETTER(Event, pressure   , lua_pushnumber , event_pressure);
FORLUA_GETTER(Event, error      , lua_pushnumber , event_error);
FORLUA_GETTER(Event, count      , lua_pushinteger, event_count);


void fl_Event_initdata(Lua *lua) {
  FORLUA_HASDESTRUCTOR(lua, Event);
  FORLUA_HASMETHOD(lua, Event, type       );
  
  FORLUA_HASMETHOD(lua, Event, timestamp  );
  FORLUA_HASMETHOD(lua, Event, ismouse    );
  FORLUA_HASMETHOD(lua, Event, isdisplay  );
  FORLUA_HASMETHOD(lua, Event, isjoystick );
  FORLUA_HASMETHOD(lua, Event, iskeyboard );
  FORLUA_HASMETHOD(lua, Event, istimer    );

  FORLUA_HASMETHOD(lua, Event, x          );
  FORLUA_HASMETHOD(lua, Event, y          );
  FORLUA_HASMETHOD(lua, Event, z          );
  FORLUA_HASMETHOD(lua, Event, w          );
  FORLUA_HASMETHOD(lua, Event, dx         );
  FORLUA_HASMETHOD(lua, Event, dy         );
  FORLUA_HASMETHOD(lua, Event, dz         );
  FORLUA_HASMETHOD(lua, Event, dw         );
  FORLUA_HASMETHOD(lua, Event, width      );
  FORLUA_HASMETHOD(lua, Event, height     );
  
  FORLUA_HASMETHOD(lua, Event, orientation);
  FORLUA_HASMETHOD(lua, Event, stick      );
  FORLUA_HASMETHOD(lua, Event, axis       );
  FORLUA_HASMETHOD(lua, Event, pos        );
  FORLUA_HASMETHOD(lua, Event, button     );
  FORLUA_HASMETHOD(lua, Event, keycode    );
  FORLUA_HASMETHOD(lua, Event, unichar    );
  FORLUA_HASMETHOD(lua, Event, modifiers  );
  FORLUA_HASMETHOD(lua, Event, repeat     );
  FORLUA_HASMETHOD(lua, Event, pressure   );
  FORLUA_HASMETHOD(lua, Event, error      );
  FORLUA_HASMETHOD(lua, Event, count      );

}






/** Writes to console or to stdout if console is not available. */
int fl_log(lua_State * lua) {
  Console * console = lh_registry_getptr(lua, "eruta.state.console");
  const char * text = NULL;
  if(lh_scanargs(lua, "s", &text) < 1) {
    return 0;
  }
  if(console) {
    console_puts(console, text);
  } else {
    puts(text);
  }
  return 0;
}

/** Initializes the functionality that Eruta exposes to lua. */
int fl_init(lua_State * lua) {
  // luaL_dostring(lua, "print 'Hello!' ");
  lua_register(lua, "test"        , fl_test);
  // initialize the different classes of user data
  fl_Path_initdata(lua);
  fl_Image_initdata(lua);
  fl_Font_initdata(lua);
  fl_Event_initdata(lua);
  
  lua_register(lua, "PathForData" , fl_PathForData);
  lua_register(lua, "Image"       , fl_Image);
  lua_register(lua, "Font"        , fl_Font);
  lua_register(lua, "log"         , fl_log);
  lh_globalint(lua, "TILE_WALL"   , 1);
  
  return 0;
}




