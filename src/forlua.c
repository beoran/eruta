/* Functions that extend lua for use with Eruta. */
#include "lh.h" 
#include "eruta.h"
#include "fifi.h"
#include "image.h"


/* All functions have a fl prefix that stands for "Function for Lua" */



int fl_test(lua_State * lua) {
  printf("Hello from test!\n");
  return 0;
}

/* Wrapping ALLEGRO_PATH (Path) */

int fl_Path_gc(Lua * lua) {
  ALLEGRO_PATH * path = NULL;
  path = lh_getself(lua, "Path");
  al_destroy_path(path);
  return 0;
}

int fl_Path_to_s(Lua * lua) {
  ALLEGRO_PATH * path = NULL;
  path = lh_getself(lua, "Path");
  if(!path) { lua_pushstring(lua, "<NULL> path"); }
  else { lua_pushstring(lua, PATH_CSTR(path)); }
  return 1;
}

void fl_Path_initdata(Lua *lua) {
  lh_datainit(lua, "Path", fl_Path_gc);
  lh_datamethod(lua, "Path", "to_s"  , fl_Path_to_s);
}

int fl_PathForData(Lua * lua) {
  ALLEGRO_PATH * path = NULL;
  const char * vpath = NULL;
  lh_scanargs(lua, "s", &vpath);
  path = fifi_data_vpath(vpath);
  lh_pushdata(lua, "Path", path);
  return 1;
}

/** Wrapping ALLEGRO_IMAGE (Image) */

int fl_Image_gc(Lua * lua) {
  Image * self = NULL;   
  self = lh_getself(lua, "Image");
  image_free(self);
  return 0;
}

int fl_Image_w(Lua * lua) {
  Image * self  = NULL;
  self          = lh_getself(lua, "Image");
  printf("Image_w: %p\n", self);
  if(!self) { lua_pushnil(lua);               }
  else      { lua_pushinteger(lua, image_w(self)); }
  return 1;
}

int fl_Image_h(Lua * lua) {
  Image * self  = NULL;
  self          = lh_getself(lua, "Image");
  if(!self) { lua_pushnil(lua);               }
  else      { lua_pushinteger(lua, image_h(self)); }
  return 1;
}

void fl_Image_initdata(Lua *lua) {
  lh_datainit(lua, "Image", fl_Image_gc);
  lh_datamethod(lua, "Image", "w"      , fl_Image_w);
  lh_datamethod(lua, "Image", "h"      , fl_Image_h);
  lh_datamethod(lua, "Image", "width"  , fl_Image_w);
  lh_datamethod(lua, "Image", "height" , fl_Image_h);
  lh_datamethod(lua, "Image", "wide"   , fl_Image_w);
  lh_datamethod(lua, "Image", "high"   , fl_Image_h);
}

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

/** Wrapping ALLEGRO_FONT (Font) */

int fl_Font_gc(Lua * lua) {
  Font * self = NULL;
  self = lh_getself(lua, "Font");
  font_free(self);
  return 0;
}

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

int fl_Font_h(Lua * lua) {
  Font * self  = NULL;
  self         = lh_getself(lua, "Font");
  if(!self) { lua_pushnil(lua);               }
  else      { lua_pushinteger(lua, font_lineheight(self)); }
  return 1;
}

void fl_Font_initdata(Lua *lua) {
  lh_datainit(lua, "Font", fl_Font_gc);
  lh_datamethod(lua, "Font", "w"      , fl_Font_w);
  lh_datamethod(lua, "Font", "h"      , fl_Font_h);
  lh_datamethod(lua, "Font", "width"  , fl_Font_w);
  lh_datamethod(lua, "Font", "height" , fl_Font_h);
  lh_datamethod(lua, "Font", "wide"   , fl_Font_w);
  lh_datamethod(lua, "Font", "high"   , fl_Font_h);
}

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
  
  lua_register(lua, "PathForData" , fl_PathForData);
  lua_register(lua, "Image"       , fl_Image);
  lua_register(lua, "Font"        , fl_Font);
  lua_register(lua, "log"         , fl_log);
  lh_globalint(lua, "TILE_WALL"   , 1);
  
  return 0;
}




