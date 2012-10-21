#include <libxml/parser.h>

#include "eruta.h"
#include "state.h"
#include "react.h"
#include "image.h"
#include "tile.h"
#include "tilepane.h"
#include "tilemap.h"
#include "tileio.h"
#include "sound.h"
#include "silut.h"
#include "fifi.h"
#include "ui.h"
#include "hatab.h"
#include "beo.h"
#include "assert.h"



#define SCREEN_W 640
#define SCREEN_H 480
ALLEGRO_FONT* font;
ALLEGRO_TRANSFORM identity;
ALLEGRO_BITMAP* buffer;
ALLEGRO_BITMAP* texture;
ALLEGRO_COLOR   solid_white;

#define abort_example perror
#define ERUTA_TEST_SHEET      "data/image/tile/tiles_village_1000.png"
#define ERUTA_MAINMENU_IMAGE  "data/image/background/eruta_mainmenu.png"
/** For some reason, onlt wav music plays?
It seems there is a bug in init_dynlib() in the ogg driver nbut only
if Allegro is compiled in the default RelWithDbg mode.
***/
#define ERUTA_TEST_MUSIC "data/music/musictest.ogg"
#define ERUTA_MAP_TEST "data/map/map_0001.tmx"
// #define RUN_TESTS 1

/*
#ifdef RUN_TESTS

#endif
*/



/* Testing function for al_get_standard_path */
void puts_standard_path(int path, char * name) {
  ALLEGRO_PATH * testpath;
  testpath =  al_get_standard_path(path);
  puts(name);
  puts(al_path_cstr(testpath, ALLEGRO_NATIVE_PATH_SEP));
  al_destroy_path(testpath);
}
  

React * main_react_key_down(React * self, ALLEGRO_KEYBOARD_EVENT * event) {  
  State * state = (State *) self->data;
  Camera * camera = NULL;
  if (!state) return NULL;
  camera = state_camera(state);
  if (!camera) return NULL;
  switch(event->keycode) {
    case ALLEGRO_KEY_UP:
      camera_speed_xy_(camera,  0, -1);
    break;
    case ALLEGRO_KEY_DOWN:
      camera_speed_xy_(camera,  0, +1);
    break;
    case ALLEGRO_KEY_LEFT:
      camera_speed_xy_(camera, -1,  0);
    break;
    case ALLEGRO_KEY_RIGHT:
      camera_speed_xy_(camera, +1,  0);
    break;
    case ALLEGRO_KEY_F1:
      console_active_(state_console(state), TRUE);
    break;
    case ALLEGRO_KEY_F2:
      console_active_(state_console(state), FALSE);
    break;  
    case ALLEGRO_KEY_ESCAPE:
    case ALLEGRO_KEY_ENTER:
      state_done(state);
    default:
    break;
  }
  return self;
}


React * main_react_key_up(React * self, ALLEGRO_KEYBOARD_EVENT * event) {
  Point os;
  State * state = (State *) self->data;
  Camera * camera = NULL;
  if (!state) return NULL;
  camera = state_camera(state);
  if (!camera) return NULL;
  os = camera_speed(camera);
  switch(event->keycode) {
    case ALLEGRO_KEY_UP:
      camera_speed_xy_(camera, os.x, 0);
    break;
    case ALLEGRO_KEY_DOWN:
      camera_speed_xy_(camera, os.x, 0);
    break;
    case ALLEGRO_KEY_LEFT:
      camera_speed_xy_(camera, 0,  os.y);
    break;
    case ALLEGRO_KEY_RIGHT:
      camera_speed_xy_(camera, 0,  os.y);
    break;
    default:
    break;
  }
  return self;
}



int real_main(void) {
    Image    * border   = NULL;
    Image    * sheet    = NULL;
    Tileset  * tileset  = NULL;
    Tile     * tile     = NULL;
    State    * state    = NULL;
    Music    * music    = NULL;
    Camera   * camera   = NULL;
    Tilepane * tilepane = NULL;
    Tilemap  * map      = NULL;
    React    react;
    ALLEGRO_COLOR myblack = {0.0, 0.0, 0.0, 1.0};

    
    
    state = state_alloc();
    state_set(state); 
    // set the state as the global state.
    /*Point      mp = { -100, -100};
    Point      mv = {0    , 0};*/
    if((!(state)) || (!state_init(state, FALSE))) {
      perror(state_errmsg(state));
      return 1;
    }
    


    /** Initialises the reactor, the game state is it's data. */
    react_initempty(&react, state);
    react.keyboard_key_up   = main_react_key_up;
    react.keyboard_key_down = main_react_key_down;
    
    puts_standard_path(ALLEGRO_EXENAME_PATH, "ALLEGRO_EXENAME_PATH:");

    camera  = state_camera(state);
    //music   = music_load("musictest.ogg");
    // if(!music) perror("musictest.ogg");

    border  = fifi_loadbitmap("border_004.png",
                            "image", "ui", "background", NULL);

    border  = fifi_loadbitmap_vpath("image/ui/background/border_004.png");
    sheet   = fifi_loadbitmap("tiles_village_1000.png", "image", "tile", NULL);
    // image_load(ERUTA_TEST_SHEET);
    if(!sheet) {
      perror(ERUTA_TEST_SHEET);
      return 1;
    }
    tileset = tileset_new(sheet);
    tile    = tileset_get(tileset, 4);
    tile_anim_(tile, -4);
    tile    = tileset_get(tileset, 0);
    tile_anim_(tile, 4);

    tilepane= tilepane_new(tileset, 100, 100);
    // tilepane_set(tilepane, 0, 0, tile);
    // tilepane_set(tilepane, 1, 1, tile);
    tilepane_fill(tilepane, tile);

    // map = tilemap_load(ERUTA_MAP_TEST);
    map = fifi_loadsimple((FifiSimpleLoader*) tilemap_load,
                          "map_0001.tmx", "map", NULL);
    if(!map) {
      puts("Map is NULL!");
    }

  // Try to load the main lua file.
  // lh_dofile_stderr(state_lua(state), "main.lua");
  // Call the on_start function.
  // lh_dofunction_myconsole_args(state_lua(state), "on_start", "s", "a string argument");

    
  while(state_busy(state)) { 
    // tile_addframe(tile, 3);
    //tile_addanime(tile, TILE_ANIME_NEXT);
    //tile_addanime(tile, TILE_ANIME_REWIND);
      react_poll(&react, state);
      
      // al_clear_to_color(COLOR_WHITE);
      // al_draw_line(0, 0, SCREEN_W, SCREEN_H, COLOR_WHITE, 7);
      
      
      if(map) tilemap_update(map, state_frametime(state));
      // tilepane_draw(tilepane, camera);
      // tilepane_draw(tilepane, camera);
      //tilepane_draw(tilepane, camera);
      //tilepane_draw(tilepane, camera);
      
      // camera_speed_(camera, mv);
      camera_update(camera);
      // call lua update callback 
      // lh_dofunction_myconsole_args(state_lua(state), "on_update", "s", "a string argument");
      
      
      if(map) tilemap_draw(map, camera);
      tile_draw(tile, 200, 300);
      // tile_update(tile);
      state_frames_update(state);
      draw_roundframe(2, 2, 630, 30, 2,
      COLOR_WHITE, color_rgba(0, 0, 64, 192));

      
      if (border) { 
        image_blitscale9(border, 10, 400, 200, 30, -1, -1);
        image_blitscale9(border, 220, 300, 400, 150, -1, -1);
      }

      // call lua drawing callback
      // lh_dofunction_myconsole_args(state_lua(state), "on_draw", "s", "a string argument");
      
      /* lh_callglobalstderr_args(state_lua(state), "on_draw", "s", "a string 
       argument"); */
      
      al_draw_textf(state_font(state), COLOR_WHITE,
                        10, 10, 0, "FPS: %lf, %d", state_fps(state), state_frames(state));
      // draw the console (will autohide if not active).
      console_draw(state_console(state));
   
      al_flip_display();
   }
   tilemap_free(map);
   tilepane_free(tilepane);
   tileset_free(tileset);
   image_free(border);
   // camera_free(camera); now released by state.
   state_done(state);
   state_free(state); 
   return 0;
}


int main(int argc, char* argv[]) {
  int res; // init xml parser
  // LIBXML_TEST_VERSION
  res = real_main();
  // cleanup xml parser
  // xmlCleanupParser();
  return res;
}
