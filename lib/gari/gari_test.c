#include "gari.h"
#include "testing.h"
#include <unistd.h>



#define BUFFER_SIZE 123

TEST_FUNC(game) {  
  GariGame * game;
  GariScreen * screen;
  GariImage * sim;
  int i, j, rep;
  game   = gari_game_make();
  TEST_NOTNULL(game);
  screen = gari_screen_make(game, 640, 480, 0);
  TEST_NOTNULL(screen);
  sim    = gari_screen_image(screen);
  gari_image_fill(sim, 0);  
 for (rep = 0 ; rep < 10; rep++) { 

  for (i = 0 ; i < 640; i++) { 
    for (j = 0 ; j < 480; j++) {
      gari_image_putpixel(sim, i, j, 1234567890);
      // gari_game_update(game);
    }
  }    
  }
  gari_game_update(game);
  
  sleep(3);
  
  gari_game_free(game);
  
  
  TEST_DONE();
}



int main(void) {
  TEST_INIT(); 
  TEST_RUN(game);
  TEST_REPORT();
}








