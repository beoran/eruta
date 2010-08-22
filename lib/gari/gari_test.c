#include "gari.h"
#include "testing.h"
#include <unistd.h>
#include <stdio.h>



#define BUFFER_SIZE 123



TEST_FUNC(game) { 
  GariFlow * flow;
  GariFont * font; 
  GariGame * game;
  GariRuby * ruby;
  GariScreen * screen;
  GariImage * sim;
  GariImage * tim, * mim, * oim, * bim;
  GariEvent ev;
  int i, j, rep, done;
  GariDye c1, c2, c3, c4, cg, pixel;
  GariColor white, green, black, yellow;
  
  
  
  ruby 	  = gari_ruby_new();
  TEST_NOTNULL(ruby);
  
  white   = gari_color(255,255, 255, GARI_ALPHA_SOLID);
  green   = gari_color(0  ,255, 0  , GARI_ALPHA_SOLID);
  black   = gari_color(0  ,0  , 0  , GARI_ALPHA_SOLID);
  yellow  = gari_color(255,255, 0  , GARI_ALPHA_SOLID);  
  
  game    = gari_game_make();
  TEST_NOTNULL(game);
  screen  = gari_screen_make(game, 640, 480, 0);
  TEST_NOTNULL(screen);
  gari_audio_init(game);
  
  
  flow    = gari_flow_make(1000);
  TEST_NOTNULL(flow);
  sim     = gari_screen_image(screen);  
  c1      = gari_image_rgb(sim,  255, 255, 0);
  c2      = gari_image_rgb(sim,    0,   0, 255);
  c3      = gari_image_rgb(sim,    0, 255, 0);
  c4      = gari_image_rgba(sim,   0,   0, 0, 0);
  cg      = gari_image_rgba(sim, 127, 127, 127, 127);
  gari_image_slab(sim, 0, 0, 640, 480, yellow);  
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
    gari_image_slab(sim, -140, -140, 200, 200, green);
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
    gari_flow_activate(flow, 10, GariFlowSnow, 0, 0, white, NULL, NULL); 
    gari_flow_update(flow, 1);
    gari_flow_draw(flow, sim);
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
  
  gari_flow_free(flow);
  
  gari_audio_done(game);    
  gari_game_free(game);
  gari_ruby_free(ruby);
  
  
  TEST_DONE();
}


int gari_ruby_do(const char * cmd) { 
  int result;
  rb_protect((VALUE (*)())rb_eval_string, (VALUE) cmd,
	     &result);
  if (result != 0) {
    VALUE lasterr, m;
    lasterr = rb_gv_get("$!");  
    m = rb_obj_as_string(lasterr);
    fprintf(stderr, "Ruby error: %s\n", RSTRING_PTR(m));
  }
  ruby_cleanup(result);
  return result;
}


//RUBY_GLOBAL_SETUP

int main(int argc, char * argv[]) {
  int result;
/*  RUBY_INIT_STACK
  ruby_init();
  // ruby_script(argv[0]);
  // ruby_init_loadpath();
  
  {
  	//	Ruby Options are just like /usr/bin/ruby
	//	interpreter name, script name, argv ...
	char*	options[]	=	{ "", "-e", "puts 'hello'"  };
	void*	node		=	ruby_options( 2, options );
	char*	options2[]	=	{ "", "-e", "puts 'world'"  };
	void*	node2; 
        result = ruby_run_node( node  );
        ruby_cleanup(result);
        node2 =	ruby_options( 2, options2 );	
        ruby_run_node( node2 );
  }
  //	Ruby Options are just like /usr/bin/ruby
  //	interpreter name, script name, argv ...
  /*
  { 	  
    char*	options[]	=	{ "", "-e", "puts 'hello';"  };
    void*	node		=	ruby_options( 2, options);
    char*	options2[]	=	{ "", "-e", "puts 'world';"  };
    void*	node2		=	ruby_options( 2, options);        
    ruby_run_node( node );
    ruby_run_node( node2 );
  }  
  /*ruby_init();
  { 	  
    char*	options[]	=	{ "", "-e", "puts 'world';"  };
    void*	node		=	ruby_options( 2, options);
    ruby_run_node( node );
  }  
*/

 
  
  TEST_INIT(); 
  TEST_RUN(game);
  TEST_REPORT();
}








