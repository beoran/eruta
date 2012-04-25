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
#undef RUN_TESTS

#ifdef RUN_TESTS

const char * xml_test_doc = 
"<?xml ?> "
"<!-- Comment should be ignored -->"
"<map high=\"100\" wide=\"200\">"
"<layer>"
"0,1,2,3\n"
"0,1,0,0\n"
"0,0,1,0\n"
"</layer>"
"</map>";

int test_xml(void) {
  Xml * xml, *xml1, *xml2, *xml3, *xml4, *xml5, *xml6, *xml7, *xml8;
  xml  = xml_newcstr("map", "");
  // xml1 = xml_newcstr();
  // xml2 = xml_newcstr("wide", "100");
  // xml3 = xml_newcstr("layer", "");
  // xml4 = xml_newcstr("#text", "A Text node");
  // xml5 = xml_newcstr("tileset", "");
  // xml_addchild(xml, xml5);
  
  xml1 = xml_newattributecstr(xml, "high", "200");
  xml2 = xml_newattributecstr(xml, "wide", "100");
  xml3 = xml_newchildcstr(xml, "layer");
  xml4 = xml_newtextcstr(xml3, "A Text Node.");
  xml5 = xml_newchildcstr(xml3, "tileset");
  xml6 = xml_newchildcstr(xml3, "tileset");
  
  xml8 = xml_findchild_cstr(xml, "layer");
  printf("%p %p %d\n", xml3, xml8, xml3 == xml8);
  xml7 = xml_findchild_cstr(xml8, "tileset");
  printf("%p %p %d\n", xml5, xml7, xml5 == xml7);
  xml8 = xml_findsibling_cstr(xml7, "tileset");
  printf("%p %p %d\n", xml6, xml8, xml6 == xml8);
  
  
  
  xml_free(xml); // Should free all the linked rest too.
  // and again
  xml = xml_newcstr("map", NULL);
  xml_newattributecstr(xml, "wide",  "500");
  xml_newattributecstr(xml, "high",  "400");
  printf("%s\n", xml_findattribute_cstrcstr(xml, "wide"));
  printf("%s\n", xml_findattribute_cstrcstr(xml, "high"));
  printf("%p\n", xml_findattribute_cstrcstr(xml, "blah"));
  
  xml_free(xml); // Should free all the linked rest too.
  
  return 0;
}

#endif

int test_silut(void) {
  Silut * aid;
  static Silut lut[] = {
    { 0, "zero"},
    { 2, "two" },
    { 1, "one" },
    SILUT_DONE
  };
  
  aid = silut_lsearchi(lut, 55);
  assert(!aid);
  aid = silut_lsearchcstr(lut, "banana");
  assert(!aid);
  aid = silut_lsearchi(lut, 2);
  assert(aid);
  assert(aid->integer == 2);
  assert(!strcmp(aid->string, "two"));
  aid = silut_lsearchcstr(lut, "two");
  assert(aid);
  assert(aid->integer == 2);
  assert(!strcmp(aid->string, "two"));
  return 0;  
}


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
    default:
      state_done(state);
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
      state_done(state);
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
    
    
    state = state_alloc();
    /*Point      mp = { -100, -100};
    Point      mv = {0    , 0};*/
    if((!(state)) || (!state_init(state, FALSE))) {
      perror(state_errmsg(state));
      return 1;
    }

    /** Initialises the reactor, the game state is it's data. */
    react_initempty(&react, state);
    react.keyboard_key_up = main_react_key_up;
    react.keyboard_key_down = main_react_key_down;
    
    puts_standard_path(ALLEGRO_EXENAME_PATH, "ALLEGRO_EXENAME_PATH:");

    camera = state_camera(state);
    music = music_load("musictest.ogg");
    if(!music) perror("musictest.ogg");

    border = fifi_loadbitmap("border_004.png",
                            "image", "ui", "background", NULL);
    sheet = fifi_loadbitmap("tiles_village_1000.png", "image", "tile", NULL);
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

    
  while(state_busy(state)) { 
    // tile_addframe(tile, 3);
    //tile_addanime(tile, TILE_ANIME_NEXT);
    //tile_addanime(tile, TILE_ANIME_REWIND);
      react_poll(&react, state);
      
      al_clear_to_color(COLOR_BLACK);
      al_draw_line(0, 0, SCREEN_W, SCREEN_H, COLOR_WHITE, 7);
      
      if(map) tilemap_draw(map, camera);
      // tilepane_draw(tilepane, camera);
      // tilepane_draw(tilepane, camera);
      //tilepane_draw(tilepane, camera);
      //tilepane_draw(tilepane, camera);
      
      // camera_speed_(camera, mv);
      camera_update(camera);
      
      tile_draw(tile, 200, 300);
      // tile_update(tile);
      state_frames_update(state);
      draw_roundframe(2, 2, 630, 30, 2,
      COLOR_WHITE, color_rgba(0, 0, 64, 192));

      
      if (border) { 
        image_blitscale9(border, 10, 400, 200, 30, -1, -1);
        image_blitscale9(border, 220, 300, 400, 150, -1, -1);
      }
      
      al_draw_textf(state_font(state), COLOR_WHITE,
                        10, 10, 0, "FPS: question p? %lf, %d", state_fps(state), state_frames(state));
                        
   
      al_flip_display();
   }
   tilepane_free(tilepane);
   tileset_free(tileset);
   camera_free(camera);
   state_done(state);
   state_free(state);
   return 0;
}


int main(void) {
  // init xml parser
  LIBXML_TEST_VERSION
#ifdef RUN_TESTS
  if(!test_xml()) return 0;
#else
  test_silut();
  return real_main();
#endif
  // cleanup xml parser
  xmlCleanupParser();
}