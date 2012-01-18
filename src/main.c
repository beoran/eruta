#include "eruta.h"
#include "game.h"
#include "image.h"
#include "tile.h"
#include "sound.h"

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


int main(void) {
    Image   * sheet;
    Tileset * tileset;
    Tile    * tile;
    Game    * game;
    Music   * music;
    game = game_alloc();
    if(!game_init(game, FALSE)) {
      perror(game_errmsg(game));
      return 1;
    }
    music = music_load(ERUTA_TEST_MUSIC);
    if(!music) perror(ERUTA_TEST_MUSIC);
    
    sheet = image_load(ERUTA_TEST_SHEET);
    if(!sheet) {
      perror(ERUTA_TEST_SHEET);
      return 1;
    }
    tileset = tileset_new(sheet);
    tile    = tileset_tile(tileset);
    tile_addframe(tile, 2);
    // tile_addframe(tile, 3);
    //tile_addanime(tile, TILE_ANIME_NEXT);
    //tile_addanime(tile, TILE_ANIME_REWIND);    
    while (game_busy(game)) {
      ALLEGRO_EVENT event;
      while(game_poll(game, &event)) {        
        switch (event.type) {
          case ALLEGRO_EVENT_DISPLAY_CLOSE:
          case ALLEGRO_EVENT_KEY_DOWN:  {
            game_done(game);
            break;
           }
        } 
      }
      al_clear_to_color(game_color(game, ERUTA_BLACK));
      al_draw_line(0, 0, SCREEN_W, SCREEN_H, game_color(game, ERUTA_WHITE), 7);
      tile_draw(tile, 200, 300);
      tile_update(tile);
      al_flip_display();
   }
   game_done(game);
   game_free(game);
   return 0;
}
