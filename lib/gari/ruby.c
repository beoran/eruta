/**
* Interface to the library for MRI Ruby.
*/

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

#include "rubyhelp.h"

RBH_MODULE_DEFINE(Gari);
RBH_CLASS_DEFINE(Font, GariFont);
RBH_CLASS_DEFINE(Flow, GariFlow);
RBH_CLASS_DEFINE(Screen, GariScreen);
RBH_CLASS_DEFINE(Image, GariImage);
RBH_CLASS_DEFINE(Joystick, GariJoystick);
RBH_CLASS_DEFINE(Event, GariEvent);
RBH_CLASS_DEFINE(Dye, GariDye);
RBH_CLASS_DEFINE(Color, GariColor);
RBH_CLASS_DEFINE(Layer, GariLayer);
RBH_CLASS_DEFINE(Camera, GariCamera);
RBH_CLASS_DEFINE(Tileset, GariTileset);
RBH_CLASS_DEFINE(Tile, GariTile);

// RBH_UNWRAP_DEFINE(Font, GariFont); 

/*
static VALUE GariMod, GariFontClass, GariFlowClass, GariScreenClass;
static VALUE GariImageClass, GariEventClass, GariDyeClass, GariColorClass;
static VALUE GariLayerClass, GariCameraClass, GariTilesetClass, GariTileClass;
*/

// Data_Wrap_Struct(klass, mark, free, ptr)

/* 
All ruby wrappers for gari functions simply get an rb prefix, so it becomes 
rbgari 
*/

VALUE rbgari_color(VALUE rv, VALUE bv, VALUE gv, VALUE av) {
  return Qnil;
}

VALUE rbgari_font_free(VALUE self) {
  GariFont * font = RBH_UNWRAP(Font, self);
  gari_font_free(font);
  return Qnil;
}

VALUE rbgari_font_new(VALUE self, VALUE name, VALUE size) {
  GariFont * font = gari_font_load(RBH_STRING(name), RBH_INT(size));
  return RBH_WRAP(Font, font, rbgari_font_free);
}



void Init_gari() {
  RBH_MODULE(Gari);
  RBH_MODULE_CLASS(Gari, Color);
  RBH_MODULE_CLASS(Gari, Dye);
  RBH_MODULE_CLASS(Gari, Event);
  RBH_MODULE_CLASS(Gari, Flow);
  RBH_MODULE_CLASS(Gari, Font);
  RBH_MODULE_CLASS(Gari, Image);
  RBH_MODULE_CLASS(Gari, Joystick);
  RBH_MODULE_CLASS(Gari, Screen);
  RBH_MODULE_CLASS(Gari, Tile);
  RBH_MODULE_CLASS(Gari, Tileset);
  RBH_MODULE_CLASS(Gari, Camera);
  RBH_MODULE_CLASS(Gari, Layer);
  
  RBH_SINGLETON_METHOD(Font, new, rbgari_font_new, 2);
  RBH_CLASS_NUM_CONST(Font, Blended, GariFontBlended); 
  RBH_CLASS_NUM_CONST(Font,  Shaded, GariFontSolid);
  RBH_CLASS_NUM_CONST(Font,  Solid, GariFontShaded);
  

  
/*
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
*/  
}

void Init_libgari() {
  Init_gari();
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



  
  














