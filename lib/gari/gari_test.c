#include "gari.h"
#include "testing.h"
#include <unistd.h>
#include <stdio.h>



#define BUFFER_SIZE 123



TEST_FUNC(game) { 
  GariFlow * flow;
  GariFont * font; 
  GariGame * game;
  GariScreen * screen;
  GariImage * sim;
  GariImage * tim, * mim, * oim, * bim;
  GariEvent ev;
  int i, j, rep, done;
  GariDye pixel;
  GariColor c1, c2, c3, c4, cg, white, green, black, yellow, red, blue;
  GariColor *cdyn;
  cdyn    = gari_color_rgba(10,20,30,40);
  TEST_NOTNULL(cdyn);
  TEST_INTEQ(10, gari_color_r(cdyn));
  TEST_INTEQ(20, gari_color_g(cdyn));
  TEST_INTEQ(30, gari_color_b(cdyn));
  TEST_INTEQ(40, gari_color_a(cdyn));
  
  white   = gari_color(255,255, 255);
  green   = gari_color(0  ,255, 0);
  black   = gari_color(0  ,0  , 0);
  yellow  = gari_color(255,255, 0);  
  red     = gari_color(255, 0, 0);
  blue    = gari_color(0  , 0, 255);
  red     = blue;
  
  game    = gari_game_make();
  TEST_NOTNULL(game);
  screen  = gari_screen_make(game, 640, 480, 0);
  TEST_NOTNULL(screen);
  gari_audio_init(game);
  
  
  flow    = gari_flow_make(1000);
  TEST_NOTNULL(flow);
  sim     = gari_screen_image(screen);
  c1      = gari_color(255  , 255, 0        );
  c2      = gari_color(0    ,   0, 255      );
  c3      = gari_color(0    , 255, 0        );
  c4      = gari_colora(0   ,   0, 0  , 0   );
  cg      = gari_colora(127 , 127, 127, 127 );
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
 
    gari_image_slab(sim, 0, 0, 640, 480, c1);    
    gari_image_line(sim, 0, 0, 640, 480, c2);
    gari_image_dot(sim, 21, 181, c2);
    gari_image_slab(sim, -140, -140, 200, 200, green);

    gari_image_box(sim, 40, 70, 200, 100, c3);
    gari_image_box(sim, 300, 300, -100, -200, c3);
    gari_image_blit(sim, 300, 300, tim);
    gari_image_blit(sim, 350, 350, mim);
    gari_image_blit(sim, 380, 380, oim);
    gari_image_scaleblit(sim, 400, 100, 100, 50, bim, 
                              0, 0, 32, 32);
                              // gari_image_w(bim) , gari_image_h(bim));
    gari_image_disk(sim, 400, 400, 50, white);
    gari_image_hoop(sim, 400, 400, 50, *cdyn);
    gari_image_flood(sim, 250, 250, red);
    
    gari_image_blendslab(sim, 1, 1, 200, 200, cg, 255);
    gari_font_drawcolor(sim, 50, 50, "日本語　This is ök!", font, white, black); 
    gari_font_printf(sim, 20, 20, font, white, black,  
                     "FPS: %ld", (int)gari_game_fps(game));
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
  
  gari_image_dot(sim, 21, 181, c2);
  gari_game_update(game);
  
  gari_flow_free(flow);
  gari_image_free(bim);
  gari_image_free(tim);
  gari_font_free(font);
  
  gari_audio_done(game);
  gari_game_free(game);
  TEST_NULL(gari_color_free(cdyn));
  
  
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
  GariColor red, green;
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








