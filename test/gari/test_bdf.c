#include "si_test.h"
#include "gari.h"
#include <time.h>

#define CLOSE_TIME 3

TEST_FUNC(gari_nofont) {
  GariGame    * game;
  GariScreen  * screen;
  GariBdf     * bdfe, *bdfj;
  time_t      time_now = 0, time_start = time(NULL);
  game        = gari_game_make();
  TEST_NOTNULL(game);
  screen      = gari_game_openscreen(game, 640, 480, 0);
  bdfj        = gari_bdf_load("../../data/font/18x18ja.bdf");
  bdfe        = gari_bdf_load("../../data/font/10x20.bdf");
  TEST_NOTNULL(screen);
  TEST_NOTNULL(bdfj);
  TEST_NOTNULL(bdfe);
  TEST_NOTNULL(gari_bdf_find(bdfj, 'A'));
  TEST_NOTNULL(gari_bdf_find(bdfe, 'A'));
  do {
    GariEvent ev;
    while (gari_event_fetch(&ev)) {
      if(ev.kind == GARI_EVENT_QUIT) goto all_done;
      time_start = time(NULL);
    }
    gari_bdf_putc(gari_screen_image(screen), 200, 200, 'B', bdfj, GariWhite, GariBlack);
    gari_bdf_putc(gari_screen_image(screen), 300, 300, 'B', bdfe, GariWhite, GariBlack); 
     
    gari_game_nextframe(game);
    gari_game_update(game);
    time_now   = time(NULL);
  } while (difftime(time_now, time_start) < CLOSE_TIME);
  all_done:

  /*Program * prog = program_new(640, 480, FALSE);
  TEST_NOTNULL(prog);
  program_free(prog);
*/  
  gari_bdf_free(bdfj);
  gari_bdf_free(bdfe);
  gari_game_free(game); 
  TEST_DONE();
}

int main(void) {
  TEST_INIT();
  TEST_RUN(gari_nofont);
  TEST_REPORT();
}



