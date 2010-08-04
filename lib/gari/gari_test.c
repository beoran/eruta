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
  GariImage * tim, * mim, * oim, * bim;
  GariEvent ev;
  int i, j, rep, done;
  GariDye c1, c2, c3, c4, cg, pixel;
  game    = gari_game_make();
  TEST_NOTNULL(game);
  screen  = gari_screen_make(game, 640, 480, 0);
  TEST_NOTNULL(screen);
  sim     = gari_screen_image(screen);  
  c1      = gari_image_rgb(sim,  255, 255, 0);
  c2      = gari_image_rgb(sim,    0,   0, 255);
  c3      = gari_image_rgb(sim,    0, 255, 0);
  c4      = gari_image_rgba(sim,   0,   0, 0, 0);
  cg      = gari_image_rgba(sim, 127, 127, 127, 127);
  gari_image_slab(sim, 0, 0, 640, 480, c1);  
  font    = gari_font_load("../../share/font/liberationserif.ttf", 14);
  TEST_NOTNULL(font);
  gari_font_mode(font, GariFontBlended);
  
  tim     = gari_image_loadraw("../../share/image/tile_aqua.png");  
  TEST_NOTNULL(tim);
  
  bim     = gari_image_loadraw("../../share/image/ui/background/blue.png");
  TEST_NOTNULL(bim);
        
  
  
  mim     = gari_image_makedepth(24, 48, 16, GariImageAlpha);
  TEST_NOTNULL(mim); 
  gari_image_box(mim,  0,  0, 24, 48, c4);
  gari_image_box(mim, 10, 10, 10, 10, c2);
  oim     = gari_image_optimize(mim, GariImageAlpha, 0);

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
    gari_image_box(sim, 40, 70, 200, 100, c3);
    gari_image_box(sim, 300, 300, -100, -200, c3);
    gari_image_blit(sim, 300, 300, tim);
    gari_image_blit(sim, 350, 350, mim);
    gari_image_blit(sim, 380, 380, oim);
    gari_image_scaleblit(sim, 400, 100, 100, 100, bim, 
                              0, 0, 32, 32);
                              // gari_image_w(bim) , gari_image_h(bim));
    gari_image_blendslab(sim, 1, 1, 200, 200, cg, 255);
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








