#include "si_test.h"
#include "gy.h"
#include <time.h>

#define CLOSE_TIME 3

TEST_FUNC(gynofont) {
  GyGame    * game;
  GyScreen  * screen;
  GyBdf     * bdfe, *bdfj;
  time_t      time_now = 0, time_start = time(NULL);
  game        = gygame_make();
  TEST_NOTNULL(game);
  screen      = gygame_openscreen(game, 640, 480, 0);
  bdfj        = gybdf_load("../../data/font/18x18ja.bdf");
  bdfe        = gybdf_load("../../data/font/10x20.bdf");
  TEST_NOTNULL(screen);
  TEST_NOTNULL(bdfj);
  TEST_NOTNULL(bdfe);
  TEST_NOTNULL(gybdf_find(bdfj, 'A'));
  TEST_NOTNULL(gybdf_find(bdfe, 'A'));
  do {
    GyEvent ev;
    while (gyevent_fetch(&ev)) {
      if(ev.kind == GY_EVENT_QUIT) goto all_done;
      time_start = time(NULL);
    }
    gybdf_putc(gyscreen_image(screen), 200, 200, 'B', bdfj, GyWhite, GyBlack);
    gybdf_putc(gyscreen_image(screen), 300, 300, 'B', bdfe, GyWhite, GyBlack); 
     
    gygame_nextframe(game);
    gygame_update(game);
    time_now   = time(NULL);
  } while (difftime(time_now, time_start) < CLOSE_TIME);
  all_done:

  /*Program * prog = program_new(640, 480, FALSE);
  TEST_NOTNULL(prog);
  program_free(prog);
*/  
  gybdf_free(bdfj);
  gybdf_free(bdfe);
  gygame_free(game); 
  TEST_DONE();
}

int main(void) {
  TEST_INIT();
  TEST_RUN(gynofont);
  TEST_REPORT();
}



