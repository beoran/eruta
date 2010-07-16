#include "gari.h"
#include "testing.h"
#include <unistd.h>
#include <stdio.h>



#define BUFFER_SIZE 123

TEST_FUNC(game) { 
  GariFont * font; 
  GariGame * game;
  GariScreen * screen;
  GariImage * sim;
  GariEvent ev;
  int i, j, rep, done;
  GariColor c1, c2, c3;
  game    = gari_game_make();
  TEST_NOTNULL(game);
  screen  = gari_screen_make(game, 640, 480, 0);
  TEST_NOTNULL(screen);
  sim     = gari_screen_image(screen);
  gari_image_fill(sim, 0);
  c1      = gari_image_rgb(sim, 255, 255, 0);
  c2      = gari_image_rgb(sim,   0,   0, 255);
  c3      = gari_image_rgb(sim,   0, 255, 0);
  font    = gari_font_load("../../share/font/liberationserif.ttf", 14);
  TEST_NOTNULL(font);
  gari_font_mode(font, GariFontBlended);
    
  gari_game_resetframes(game);
  done = FALSE;
  while (!done) { 
    while (gari_event_fetch(&ev)) {    
      fprintf(stderr, "Got event: kind: %d .\n", ev.kind);
      done = (ev.kind == GARI_EVENT_QUIT);
      if(done) break;
    }   
  
    for (i = 0 ; i < 640; i++) { 
      for (j = 0 ; j < 480; j++) {
        gari_image_putpixel(sim, i, j, c1);
        // gari_game_nextframe(game);
      }
    }
    gari_image_line(sim, 0, 0, 640, 480, c2);
    gari_image_putpixel(sim, 21, 181, c2);
    gari_image_slab(sim, -140, -140, 200, 200, c3);
    gari_font_draw(sim, 50, 50, "This is ök!", font, 127, 127, 127, 0, 0, 0);
    
    gari_game_nextframe(game);
    
    
    gari_game_update(game);
    
    
  }
  
  gari_game_update(game);
  fprintf(stderr, "Putpixel FPS: %lf\n", gari_game_fps(game));
  gari_game_report(game);
  
  gari_image_line(sim, 0, 0, 640, 480, c2);
  gari_game_update(game);
  
  gari_image_putpixel(sim, 21, 181, c2);
  gari_game_update(game);
  
  gari_game_free(game);
  
  
  TEST_DONE();
}



int main(void) {
  TEST_INIT(); 
  TEST_RUN(game);
  TEST_REPORT();
}








