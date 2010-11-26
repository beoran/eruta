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
RBH_CLASS_DEFINE(Game, GariGame);
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

#define GARI_GAME_WRAP(GAME)   RBH_WRAP(Game, GAME, gari_game_free)
#define GARI_GAME_UNWRAP(GAME) RBH_UNWRAP(Game, GAME)

/** Allocates a new Gari game. */
VALUE rbgari_game_new(VALUE self) {
  GariGame * game = gari_game_make();
  return GARI_GAME_WRAP(game);
}

/** Updates the game and game screen. */
VALUE rbgari_game_update(VALUE self) {
  GariGame * game = GARI_GAME_UNWRAP(self);
  return RBH_TOBOOL(gari_game_update(game));
}   

/** Sets the frames of the game back to 0. */
VALUE rbgari_game_resetframes(VALUE self) {
  GariGame * game = GARI_GAME_UNWRAP(self);
  return RBH_TOBOOL(gari_game_resetframes(game));
}   


/** Gets the total amount of frames displayed since the start of the game, 
   or since gari_game_resetframes was called. */
VALUE rbgari_game_frames(VALUE self) {
  GariGame * game = GARI_GAME_UNWRAP(self);
  return RBH_INT_NUM(gari_game_frames(game));
}   


/** Advances the game's frame counter. */
VALUE rbgari_game_nextframe(VALUE self) {
  GariGame * game = GARI_GAME_UNWRAP(self);
  return RBH_TOBOOL(gari_game_nextframe(game));
}   

/** Starts FPS counter. gari_game_resetframes also calls this. */
VALUE rbgari_game_startfps(VALUE self) {
  GariGame * game = GARI_GAME_UNWRAP(self);
  return RBH_TOBOOL(gari_game_startfps(game));
}  
 
/** Returns calculated fps after calling startfps . */
VALUE rbgari_game_fps(VALUE self) {
  GariGame * game = GARI_GAME_UNWRAP(self);
  return RBH_DOUBLE_NUM(gari_game_fps(game));
}  

#define GARI_SCREEN_WRAP(SCREEN) RBH_WRAP(Screen, SCREEN, NULL)  
#define GARI_SCREEN_UNWRAP(SCREEN) RBH_UNWRAP(Screen, SCREEN)


/** Opens the game main game screen or window with the given color depth. 
   Must be done before using any image functions. */   
VALUE rbgari_game_openscreendepth(VALUE vgame, VALUE vwide, VALUE vhigh, VALUE vfull, VALUE vdeep) { 
  return GARI_SCREEN_WRAP(gari_game_openscreendepth(GARI_GAME_UNWRAP(vgame), 
                          RBH_INT(vwide), RBH_INT(vhigh), RBH_BOOL_INT(vfull),
                          RBH_INT(vdeep)));
}

/** Opens the game main game screen or window. 
   Must be done before using any image functions. */
VALUE rbgari_game_openscreen(VALUE vgame, VALUE vwide, VALUE vhigh, VALUE vfull) { 
  return GARI_SCREEN_WRAP(gari_game_openscreen(GARI_GAME_UNWRAP(vgame), 
                          RBH_INT(vwide), RBH_INT(vhigh), RBH_BOOL_INT(vfull)));
}

/** Retuns the current main game screen. */
VALUE rbgari_game_screen(VALUE vgame) { 
  return GARI_SCREEN_WRAP(gari_game_screen(GARI_GAME_UNWRAP(vgame)));
}

/** Checks if the main game screen is in fullscreen mode. */
VALUE rbgari_game_fullscreen(VALUE vgame) {  
  return RBH_TOBOOL(gari_game_fullscreen(GARI_GAME_UNWRAP(vgame)));
}  

/** Can be used to set or unset fullscreen after opening the screen. */
VALUE rbgari_game_fullscreen_(VALUE vgame, VALUE vfull) { 
  GariGame * game     = GARI_GAME_UNWRAP(vgame);
  int full            = RBH_BOOL_INT(vfull);
  GariScreen * screen = gari_game_fullscreen_(game, full);
  return GARI_SCREEN_WRAP(screen);
}

#define GARI_COLOR_WRAP(COLOR)   RBH_WRAP(Color, COLOR, gari_color_free)
#define GARI_COLOR_UNWRAP(COLOR) RBH_UNWRAP(Color, COLOR)

/** Makes a new color. */
VALUE rbgari_color_new(VALUE self, VALUE r, VALUE g, VALUE b, VALUE a) {
  GariColor * color =
    gari_color_rgba(RBH_UINT8(r), RBH_UINT8(g), RBH_UINT8(b), RBH_UINT8(a));    
  return GARI_COLOR_WRAP(color);
}

VALUE rbgari_color_newrgb(VALUE self, VALUE r, VALUE g, VALUE b) {
  GariColor * color =
    gari_color_rgb(RBH_UINT8(r), RBH_UINT8(g), RBH_UINT8(b));    
  return GARI_COLOR_WRAP(color);
}

/* Getters for the components of the color */  
RBH_GETTER_DEFINE(gari_color_r, Color, GariColor, RBH_UINT8_NUM);
RBH_GETTER_DEFINE(gari_color_g, Color, GariColor, RBH_UINT8_NUM);
RBH_GETTER_DEFINE(gari_color_b, Color, GariColor, RBH_UINT8_NUM);
RBH_GETTER_DEFINE(gari_color_a, Color, GariColor, RBH_UINT8_NUM);  
  
#define GARI_FONT_WRAP(F)   RBH_WRAP(Font, F, gari_font_free)
#define GARI_FONT_UNWRAP(F) RBH_UNWRAP(Font, F)  

VALUE rbgari_font_new(VALUE self, VALUE name, VALUE size) {
  char * nam = RBH_STRING(name);
  int sz = RBH_INT(size);
  GariFont * font = gari_font_load(nam, sz);
  return GARI_FONT_WRAP(font);
}

VALUE rbgari_font_error(VALUE self) {
  return RBH_STR_ASCII2(gari_font_error());
}

/** Returns the drawable image for the screen. XXX: do we need this?  
GariImage * gari_screen_image(GariScreen * screen);
*/


#define GARI_IMAGE_WRAP(IMG) RBH_WRAP(Image, IMG, gari_image_free)
#define GARI_IMAGE_UNWRAP(IMG) RBH_UNWRAP(Image, IMG)

#define GARI_DYE_WRAP(DYE) RBH_UINT_NUM(DYE)
#define GARI_DYE_UNWRAP(DYE) RBH_UINT(DYE)

/** 
* Converts a GariColor to a GariDye for the given image. 
*/
VALUE rbgari_color_dye(VALUE vcolor, VALUE vimg)  {
  GariColor * color = GARI_COLOR_UNWRAP(vcolor);
  GariImage * image = GARI_IMAGE_UNWRAP(vimg);
  return GARI_DYE_WRAP(gari_color_dye(*color, image));
}



/** Makes an empty gari image with given bits per pixels (in depth), 
    but supporting either alpha, or not, depending on mode.
    The video mode must have been set and the screen must have been opened.
    You must call gari_image_free when you're done with the generated 
    GariImage. May return nil on error.
*/
VALUE rbgari_image_newdepth(VALUE self , VALUE vw, VALUE vh, 
                            VALUE vdeep, VALUE vmode) {
  GariImage * i = gari_image_makedepth(RBH_INT(vw)    , RBH_INT(vh), 
                                       RBH_INT(vdeep) , RBH_INT(vmode));
  return GARI_IMAGE_WRAP(i);
} 

/** Makes an empty gari image with the same bits per pixels as the screen, 
    but supporting either alpha, or not, or a colorkey depending on mode.
    The video mode must have been set and the screen must have been opened.
    You must call gari_image_free when you're done with the generated 
    GariImage. May return nil on error.
*/
VALUE rbgari_image_new(VALUE self, VALUE vw, VALUE vh, 
                       VALUE vmode, VALUE vdyekey) {
  GariImage * i = NULL; 
  /*gari_image_make(RBH_INT(vw), RBH_INT(vh), RBH_INT(vmode),
                                  GARI_DYE_UNWRAP(vdyekey));
  */
  return GARI_IMAGE_WRAP(i);
}

/** Loads the image from the named file. */
VALUE rbgari_image_loadraw(VALUE self, VALUE vname) {
  char * name     = RBH_STRING(vname);
  GariImage * img = gari_image_loadraw(name);
  return GARI_IMAGE_WRAP(img);
}

/* Perhaps implement this in ruby? 
VALUE rbgari_game_loadimage(VALUE vgame, VALUE vname) {  
  GariGame * game = GARI_GAME_UNWRAP(vgame);
  char * name     = RBH_STRING(vname);
  GariImage * img = gari_image_load(game, name);
  return GARI_IMAGE_WRAP(img);
}
*/

VALUE rbgari_image_optimize(VALUE self, VALUE vmode, VALUE vdye) { 
  GariImage * slow = GARI_IMAGE_UNWRAP(self);
  GariImage * fast = gari_image_optimize(slow, 
                        RBH_INT(vmode), GARI_DYE_UNWRAP(vdye));
  return GARI_IMAGE_WRAP(fast);
}

RBH_GETTER_DEFINE(gari_image_w, Image, GariImage, RBH_INT_NUM);
RBH_GETTER_DEFINE(gari_image_h, Image, GariImage, RBH_INT_NUM);

/** Drawing function wrappers */
VALUE rbgari_image_slab(VALUE self, VALUE vx, VALUE vy, 
                        VALUE vw  , VALUE vh, VALUE vc) {
  gari_image_slab(GARI_IMAGE_UNWRAP(self), RBH_INT(vx), RBH_INT(vy), 
                  RBH_INT(vw), RBH_INT(vh), *GARI_COLOR_UNWRAP(vc));
  return self;
} 
  
VALUE rbgari_image_blendslab(VALUE self, VALUE vx, VALUE vy, 
                        VALUE vw  , VALUE vh, VALUE vc) {
  gari_image_blendslab(GARI_IMAGE_UNWRAP(self), RBH_INT(vx), RBH_INT(vy), 
                  RBH_INT(vw), RBH_INT(vh), *GARI_COLOR_UNWRAP(vc));
  return self;
} 

VALUE rbgari_image_box(VALUE self, VALUE vx, VALUE vy, 
                        VALUE vw  , VALUE vh, VALUE vc) {
  gari_image_box(GARI_IMAGE_UNWRAP(self), RBH_INT(vx), RBH_INT(vy), 
                  RBH_INT(vw), RBH_INT(vh), *GARI_COLOR_UNWRAP(vc));
  return self;
}
  
  
VALUE rbgari_image_line(VALUE self, VALUE vx, VALUE vy, 
                        VALUE vw  , VALUE vh, VALUE vc) {
  gari_image_line(GARI_IMAGE_UNWRAP(self), RBH_INT(vx), RBH_INT(vy), 
                  RBH_INT(vw), RBH_INT(vh), *GARI_COLOR_UNWRAP(vc));
  return self;
}
 
VALUE rbgari_image_disk(VALUE self, VALUE vx, VALUE vy, 
                        VALUE vr  , VALUE vc) {
  gari_image_disk(GARI_IMAGE_UNWRAP(self), RBH_INT(vx), RBH_INT(vy), 
                  RBH_INT(vr),*GARI_COLOR_UNWRAP(vc));
  return self;
}
 
VALUE rbgari_image_hoop(VALUE self, VALUE vx, VALUE vy, 
                        VALUE vr  , VALUE vc) {
  gari_image_hoop(GARI_IMAGE_UNWRAP(self), RBH_INT(vx), RBH_INT(vy), 
                  RBH_INT(vr),*GARI_COLOR_UNWRAP(vc));
  return self;
}

VALUE rbgari_image_flood(VALUE self, VALUE vx, VALUE vy, VALUE vc) {
  gari_image_flood(GARI_IMAGE_UNWRAP(self), RBH_INT(vx), RBH_INT(vy), 
                  *GARI_COLOR_UNWRAP(vc));
  return self;
}

VALUE rbgari_image_dot(VALUE self, VALUE vx, VALUE vy, VALUE vc) {
  gari_image_dot(GARI_IMAGE_UNWRAP(self), RBH_INT(vx), RBH_INT(vy), 
                  *GARI_COLOR_UNWRAP(vc));
  return self;
}

VALUE rbgari_image_blit(VALUE self, VALUE vx, VALUE vy, VALUE vimg) {
  gari_image_blit(GARI_IMAGE_UNWRAP(self), RBH_INT(vx), RBH_INT(vy), 
                  GARI_IMAGE_UNWRAP(vimg));
  return self;
}

/** Blits a part of one image to another one with the given coordinates. Takes the indicated parts of the image, as specified by srcx, srcy, srcw and srch  */
VALUE rbgari_image_blitpart(VALUE self, VALUE vx, VALUE vy, VALUE vimg,
  VALUE vsx, VALUE vsy, VALUE vsw, VALUE vsh) {
  gari_image_blitpart(GARI_IMAGE_UNWRAP(self), RBH_INT(vx), RBH_INT(vy), 
                  GARI_IMAGE_UNWRAP(vimg),
                   RBH_INT(vsx), RBH_INT(vsy),
                    RBH_INT(vsw), RBH_INT(vsh)                  
                  );
  return self;
}

/** Scaled blit. */
VALUE rbgari_image_scaleblit(VALUE self, VALUE vx, VALUE vy,
  VALUE vdw, VALUE vdh, VALUE vimg,
  VALUE vsx, VALUE vsy, VALUE vsw, VALUE vsh) {
  gari_image_scaleblit(GARI_IMAGE_UNWRAP(self), RBH_INT(vx), RBH_INT(vy),
                  RBH_INT(vdw), RBH_INT(vdh),
                  GARI_IMAGE_UNWRAP(vimg),
                   RBH_INT(vsx), RBH_INT(vsy),
                    RBH_INT(vsw), RBH_INT(vsh)
                  );
  return self;
}


/*
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
*/

/** Entry point for Ruby. */
void Init_gari() {
  RBH_MODULE(Gari);
  RBH_MODULE_CLASS(Gari, Game);
  RBH_MODULE_CLASS(Gari, Color);  
  RBH_MODULE_CLASS(Gari, Dye);
  RBH_MODULE_CLASS(Gari, Event);
  RBH_MODULE_CLASS(Gari, Flow);
  RBH_MODULE_CLASS(Gari, Font);
  RBH_MODULE_CLASS(Gari, Image);
  RBH_MODULE_CLASS(Gari, Joystick);
  // cScreen = rb_define_class_under(mGari, "Screen", cImage);
  RBH_MODULE_CLASS_SUPER(Gari, Screen, Image);
  RBH_MODULE_CLASS(Gari, Tile);
  RBH_MODULE_CLASS(Gari, Tileset);
  RBH_MODULE_CLASS(Gari, Camera);
  RBH_MODULE_CLASS(Gari, Layer);
  
  RBH_SINGLETON_METHOD(Game, new, rbgari_game_new         , 0);
  RBH_METHOD(Game, update       , rbgari_game_update      , 0);
  RBH_METHOD(Game, resetframes  , rbgari_game_resetframes , 0);
  RBH_METHOD(Game, frames       , rbgari_game_frames      , 0);
  RBH_METHOD(Game, nextframe    , rbgari_game_nextframe   , 0);
  RBH_METHOD(Game, startfps     , rbgari_game_startfps    , 0);
  RBH_METHOD(Game, fps          , rbgari_game_fps         , 0);
  RBH_METHOD(Game, openscreen   , rbgari_game_openscreen  , 3);
  RBH_METHOD(Game, openscreendepth  , rbgari_game_openscreendepth, 4);
  RBH_METHOD(Game, screen       , rbgari_game_screen      , 0);
  RBH_METHOD(Game, fullscreen   , rbgari_game_fullscreen  , 0);
  RBH_METHOD(Game, fullscreen=  , rbgari_game_fullscreen_ , 1);
  
  RBH_SINGLETON_METHOD(Image, loadraw, rbgari_image_loadraw, 1);
  RBH_SINGLETON_METHOD(Image, newdepth, rbgari_image_newdepth, 4);
    
  RBH_CLASS_NUM_CONST(Image, SOLID, GariImageSolid);
  RBH_CLASS_NUM_CONST(Image, COLORKEY, GariImageColorkey);
  RBH_CLASS_NUM_CONST(Image, ALPHA, GariImageAlpha);

  // RBH_METHOD(Game, loadimage  , rbgari_game_loadimage, 1);
  
  RBH_GETTER(Image, w         , gari_image_w);
  RBH_GETTER(Image, h         , gari_image_h);
  RBH_METHOD(Image, optimize  , rbgari_image_optimize , 2);
  RBH_METHOD(Image, slab      , rbgari_image_slab     , 5);
  RBH_METHOD(Image, blendslab , rbgari_image_blendslab, 5);
  RBH_METHOD(Image, box       , rbgari_image_box      , 5);
  RBH_METHOD(Image, line      , rbgari_image_line     , 5);
  RBH_METHOD(Image, disk      , rbgari_image_disk     , 4);
  RBH_METHOD(Image, hoop      , rbgari_image_hoop     , 4);
  RBH_METHOD(Image, flood     , rbgari_image_flood    , 3);
  RBH_METHOD(Image, dot       , rbgari_image_dot      , 3);
  RBH_METHOD(Image, blit      , rbgari_image_blit     , 3);
  RBH_METHOD(Image, blitpart  , rbgari_image_blitpart , 7);
  RBH_METHOD(Image, blitscale , rbgari_image_scaleblit, 9);
  // RBH_METHOD(Image,   , rbgari_image_ , );
  
  
  RBH_METHOD(Color, dye       , rbgari_color_dye, 1);
  
  
  RBH_SINGLETON_METHOD(Color, new , rbgari_color_new, 4);
  RBH_SINGLETON_METHOD(Color, rgba, rbgari_color_new, 4);
  RBH_SINGLETON_METHOD(Color, rgb , rbgari_color_newrgb, 3);
  RBH_GETTER(Color, r, gari_color_r);
  RBH_GETTER(Color, g, gari_color_g);
  RBH_GETTER(Color, b, gari_color_b);
  RBH_GETTER(Color, a, gari_color_a);
  RBH_CLASS_NUM_CONST(Font, ALPHA_SOLID, GARI_ALPHA_SOLID);
  RBH_CLASS_NUM_CONST(Font, ALPHA_CLEAN, GARI_ALPHA_CLEAR);
  
  RBH_SINGLETON_METHOD(Font , new   , rbgari_font_new   , 2);
  RBH_SINGLETON_METHOD(Font , error , rbgari_font_error , 0);
  RBH_CLASS_NUM_CONST(Font  , Blended, GariFontBlended); 
  RBH_CLASS_NUM_CONST(Font  ,  Shaded, GariFontSolid);
  RBH_CLASS_NUM_CONST(Font  ,  Solid, GariFontShaded);
}

/** Alternative entry point for Ruby. */
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



  
  














