#include <libxml/parser.h>

#include "eruta.h"
#include "state.h"
#include "image.h"
#include "tile.h"
#include "tilepane.h"
#include "tilemap.h"
#include "tileio.h"
#include "sound.h"
#include "silut.h"
#include "assert.h"


#define SCREEN_W 640
#define SCREEN_H 480
ALLEGRO_FONT* font;
ALLEGRO_TRANSFORM identity;
ALLEGRO_BITMAP* buffer;
ALLEGRO_BITMAP* texture;
ALLEGRO_COLOR   solid_white;

#define abort_example perror
#define ERUTA_TEST_SHEET "data/image/tile/tiles_village_1000.png"
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


int real_main(void) {
    Image    * sheet    = NULL;
    Tileset  * tileset  = NULL;
    Tile     * tile     = NULL;
    State    * state    = NULL;
    Music    * music    = NULL;
    Camera   * camera   = NULL;
    Tilepane * tilepane = NULL;
    Tilemap  * map      = NULL;
    
    state = state_alloc();
    Point      mp = { -100, -100}, mv = {0, 0};
    if((!(state)) || (!state_init(state, FALSE))) {
      perror(state_errmsg(state));
      return 1;      
    }
    camera = camera_new(-100, -100, SCREEN_W, SCREEN_H);
    if(!camera) {
      perror("Could not allocate Camera.");
    } 
    //music = music_load(ERUTA_TEST_MUSIC);
    //if(!music) perror(ERUTA_TEST_MUSIC);
    
    sheet = image_load(ERUTA_TEST_SHEET);
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

    map = tilemap_load(ERUTA_MAP_TEST);
    if(!map) {
      puts("Map is NULL!");
    }

    
    
    // tile_addframe(tile, 3);
    //tile_addanime(tile, TILE_ANIME_NEXT);
    //tile_addanime(tile, TILE_ANIME_REWIND);
    while (state_busy(state)) {    
      ALLEGRO_EVENT event;
      
      while(state_poll(state, &event)) {
      switch (event.type) {
          case ALLEGRO_EVENT_DISPLAY_CLOSE:
            state_done(state);
            break;
            
          case ALLEGRO_EVENT_KEY_DOWN:
            switch(event.keyboard.keycode) {
              case ALLEGRO_KEY_UP:
                mv.y = -1;
              break;
              case ALLEGRO_KEY_DOWN:
                mv.y = +1;
              break;
              case ALLEGRO_KEY_LEFT:
                mv.x = -1;
              break;
              case ALLEGRO_KEY_RIGHT:
                mv.x = +1;
              break;
              default:
                state_done(state);
              break;
            }
            break;
         
        case ALLEGRO_EVENT_KEY_UP: 
          switch(event.keyboard.keycode) {
              case ALLEGRO_KEY_UP:
                mv.y = 0;
              break;
              case ALLEGRO_KEY_DOWN:
                mv.y = 0;
              break;
              case ALLEGRO_KEY_LEFT:
                mv.x = 0;
              break;
              case ALLEGRO_KEY_RIGHT:
                mv.x = 0;
              break;
              default:
              break;
          }
          break;
          
        default:
            break;
        }  
      }
      al_clear_to_color(state_color(state, STATE_BLACK));
      al_draw_line(0, 0, SCREEN_W, SCREEN_H, state_color(state, STATE_WHITE), 7);
      
      if(map) tilemap_draw(map, camera);
      // tilepane_draw(tilepane, camera);
      // tilepane_draw(tilepane, camera);
      //tilepane_draw(tilepane, camera);
      //tilepane_draw(tilepane, camera);
      
      camera_speed_(camera, mv);
      camera_update(camera);
      
      tile_draw(tile, 200, 300);
      // tile_update(tile);
      state_frames_update(state);
      al_draw_textf(state_font(state), state_color(state, STATE_WHITE),
                        10, 10, 0, "FPS: %lf, %d", state_fps(state), state_frames(state));
   
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