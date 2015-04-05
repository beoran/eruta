// #include <libxml/parser.h>

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
/* #include "beo.h" */
#include "assert.h"
#include "str.h"
#include "sprite.h"
#include "alps.h"
#include "store.h"
#include "scegra.h"
#include "callrb.h"



#define SCREEN_W 640
#define SCREEN_H 480
ALLEGRO_FONT* font;
ALLEGRO_TRANSFORM identity;
ALLEGRO_BITMAP* buffer;
ALLEGRO_BITMAP* texture;
ALLEGRO_COLOR   solid_white;


/** For some reason, onlt wav music plays?
It seems there is a bug in init_dynlib() in the ogg driver but only
if Allegro is compiled in the default RelWithDbg mode.
***/


/* Testing function for al_get_standard_path */
void puts_standard_path(int path, char * name) {
  ALLEGRO_PATH * testpath;
  testpath =  al_get_standard_path(path);
  puts(name);
  puts(al_path_cstr(testpath, ALLEGRO_NATIVE_PATH_SEP));
  al_destroy_path(testpath);
}
 
/* Low level keys, not handled by the script. */ 
React * main_react_key_down(React * self, ALLEGRO_KEYBOARD_EVENT * event) {  
  State * state = state_get();
  /*
  Point f = bevec0();  
  Camera * camera = NULL;
  if (!state) return NULL;
  camera = state_camera(state);
  if (!camera) return NULL;
  */ 
  switch(event->keycode) {
    /* Console control */
    case ALLEGRO_KEY_F1:
    case ALLEGRO_KEY_F3:
      /* Toggle the console here. */
      bbconsole_active_(state_console(state), !bbconsole_active(state_console(state)));
    break;
    case ALLEGRO_KEY_F2:
      /* Hide the console. */
      bbconsole_active_(state_console(state), FALSE);
    break;  
    case ALLEGRO_KEY_F5:
      /* Reload main script (and hence all other scripts that it loads)  on F5 */
      rh_load_main();
      callrb_on_reload();
    break;    
    /* Emergency exit keys. */
    case ALLEGRO_KEY_F12:
    case ALLEGRO_KEY_ESCAPE:
      state_done(state);
      return self;
    break;
    /* Otherwise it's up to the script. */
    default:
      return NULL;
    break;
  }
  return self;
}

/* Low level key releases not handled by the script. */
React * main_react_key_up(React * self, ALLEGRO_KEYBOARD_EVENT * event) {
  Point f               = bevec0();
  State * state         = (State *) self->data;
  Camera * camera       = NULL;
  if (!state) return NULL;
  camera                = state_camera(state);
  if (!camera) return NULL;
  switch(event->keycode) {
    default:
      return NULL;
    break;
  }
  return self;
}


/* The real main loop of the Eruta engine. Most of the work happens in state.c,
 * and the main.rb script, though. */
int real_main(void) {
  Image    * border   = NULL;
  Image    * sheet    = NULL;
  Tileset  * tileset  = NULL;
  Tile     * tile     = NULL;
  State    * state    = NULL;
  Camera   * camera   = NULL;
  Tilepane * tilepane = NULL;
  Tilemap  * map      = NULL;
  Thing    * actor    = NULL;
  Tracker  * tracker          = NULL;
  Tracker  * maptracker       = NULL;
  Sprite   * sprite           = NULL;
  SpriteState * spritestate   = NULL;
  AlpsShower shower;
  int        actor_id         = -1;
  int        sprite_id        = -1;
  int        npc1_id          = -1;
  int        npc2_id          = -1;
  
  
  React    react;
  ALLEGRO_COLOR myblack = {0.0, 0.0, 0.0, 1.0};
    
  state = state_alloc();
  state_set(state); 
  if((!(state)) || (!state_init(state, FALSE))) {
    perror(state_errmsg(state));
    return 1;
  }
  
  alpsshower_init(&shower, state_camera(state), 100, 1.0, bevec(10.0, 10000.0));

  /* Initializes the reactor, the game state is it's data. */
  react_initempty(&react, state);
  react.keyboard_key_up   = main_react_key_up;
  react.keyboard_key_down = main_react_key_down;
  
  puts_standard_path(ALLEGRO_EXENAME_PATH, "ALLEGRO_EXENAME_PATH:");

  camera  = state_camera(state);

  /* Finally initialize ruby and call the ruby startup function. */
  rh_load_main();
  callrb_on_start();
    
  /* Main game loop, controlled by the State object. */  
  while(state_busy(state)) { 
      Point spritenow = bevec(100, 120); 
      react_poll(&react, state);
      alpsshower_update(&shower, state_frametime(state));
      state_update(state);
      state_draw(state);
      /* alpsshower_draw(&shower, camera); */ 
      state_flip_display(state);
   }
   state_done(state);
   state_free(state); 
   return 0;
}


int main(int argc, char* argv[]) {
  int res; // init xml parser
  // LIBXML_TEST_VERSION
  (void) argc, (void) argv;
  res = real_main();
  // cleanup xml parser
  // xmlCleanupParser();
  return res;
}

