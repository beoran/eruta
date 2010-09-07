#include "gari.h"

#include "SDL_mixer.h"

#define GARI_INTERN_ONLY
#include "gari_intern.h"  

/** Ruby interface for Gari (and vice versa). 
* It seems that it's difficult to embed ruby 1.9.x into C;  
* in stead I'll do it like this: 
*
* 1) Use the standard C foreign function interface for Ruby.
* 2) Write a small program that wraps the Ruby interpreter 
* wich loads the first script,,.
* 3) Which in turn will load the Gari library.
*/

#include "ruby.h"
#include "ruby/intern.h"

struct GariRuby_;

/*typedef struct GariRuby_ GariRuby; */

struct GariRuby_ {
  int wasinit;
  int error;
  int argc;
  char  *argv[32];
};

GariRuby * gari_ruby_init(GariRuby * ruby) {
  if(!ruby)         return NULL;
  if(ruby->wasinit) return ruby;
  // ruby->argc    = 0;
  // ruby->argv[0] = "gari";
  
  // ruby_sysinit(&ruby->argc, &ruby->argv);
  
  
  ruby_init();
  ruby_init_loadpath();
  ruby->wasinit = TRUE;
  
  
  return ruby;
} 

GariRuby * gari_ruby_new() {
  GariRuby * ruby;
  ruby = GARI_ALLOCATE(GariRuby);
  if(!ruby) return NULL;  
  if(!gari_ruby_init(ruby)) {
    GARI_FREE(ruby);
    return NULL;
  }
  return ruby;  
}



GariRuby * gari_ruby_showerror(GariRuby * ruby) {
  VALUE lasterr, m;
  if (!ruby)   	  	{ return NULL; }
  if (!ruby->error)	{ return ruby; }
  lasterr = rb_gv_get("$!");
  /* message */
  m = rb_obj_as_string(lasterr);
  printf("Ruby error: %s\n", RSTRING_PTR(m));
}


GariRuby * gari_ruby_done(GariRuby * ruby) {
  if(!ruby)           return NULL;
  if(!ruby->wasinit)  return NULL;
  ruby_finalize();
  ruby->wasinit = FALSE;
}

void gari_ruby_free(GariRuby * ruby) {
  if(!ruby) return NULL;
  gari_ruby_done(ruby);
}


GariRuby * gari_ruby_eval(GariRuby * ruby, char * str) {
 int result;
 rb_eval_string(str); 
 // rb_eval_string_protect(str, &result); 
 // ruby->error = result; 
 return ruby;
}

int gari_ruby_exec(GariRuby * ruby, char * filename) {
 rb_load_file(filename);
 // return ruby_exec();
}


/*
RUBY_GLOBAL_SETUP
 
int main(int argc, char *argv[])
{
    ruby_sysinit(&argc, &argv);
    {
        RUBY_INIT_STACK;
        ruby_init();
        return ruby_run_node(ruby_options(argc, argv));
    }
}
*/



// Data_Wrap_Struct(klass, mark, free, ptr)



Init_gari() {
  VALUE GariMod, GariFontClass, GariFlowClass, GariScreenClass;
  VALUE GariImageClass, GariEventClass, GariDyeClass, GariColorClass;
  VALUE GariLayerClass, GariCameraClass, GariTilesetClass, GariTileClass;
  GariMod  	  = rb_define_module("Gari");
  GariColorClass  = rb_define_class_under(GariMod, "Color"   , rb_cObject);
  GariDyeClass    = rb_define_class_under(GariMod, "Dye"     , rb_cObject);
  GariEventClass  = rb_define_class_under(GariMod, "Event"   , rb_cObject);  
  GariFontClass   = rb_define_class_under(GariMod, "Font"    , rb_cObject);
  GariFlowClass   = rb_define_class_under(GariMod, "Flow"    , rb_cObject);
  GariImageClass  = rb_define_class_under(GariMod, "Image"   , rb_cObject);
  GariScreenClass = rb_define_class_under(GariMod, "Screen"  , GariImageClass);
  GariTileClass   = rb_define_class_under(GariMod, "Tile"    , rb_cObject);
  GariTilesetClass= rb_define_class_under(GariMod, "Tileset" , rb_cObject);
  GariCameraClass = rb_define_class_under(GariMod, "Camera"  , rb_cObject);
  GariLayerClass  = rb_define_class_under(GariMod, "Layer"   , rb_cObject);
  
  
  
  
  
}

/*
  GariFlow * flow;
  GariFont * font; 
  GariGame * game;
  GariRuby * ruby;
  GariScreen * screen;
  GariImage * sim;
  GariImage * tim, * mim, * oim, * bim;
  GariEvent ev;
  int i, j, rep, done;
  GariDye pixel;
  GariColor c1, c2, c3, c4, cg, white, green, black, yellow, red;
  
  ruby 	  = gari_ruby_new();
  TEST_NOTNULL(ruby);
  
  white   = gari_color(255,255, 255);
  green   = gari_color(0  ,255, 0);
  black   = gari_color(0  ,0  , 0);
  yellow  = gari_color(255,255, 0);  
  red     = gari_color(255, 0, 0);
  
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
    gari_image_scaleblit(sim, 400, 100, 100, 100, bim, 
                              0, 0, 32, 32);
                              // gari_image_w(bim) , gari_image_h(bim));
    gari_image_disk(sim, 400, 400, 50, white);
    gari_image_hoop(sim, 400, 400, 50, black);
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
  gari_ruby_free(ruby);
*/  














