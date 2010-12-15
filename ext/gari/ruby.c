/**
* Interface to the library for MRI Ruby.
*/

#include "gari.h"

// #include "SDL_mixer.h"

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
RBH_CLASS_DEFINE(Audio, GariAudioInfo);
RBH_CLASS_DEFINE(Camera, GariCamera);
RBH_CLASS_DEFINE(Color, GariColor);
RBH_CLASS_DEFINE(Dye, GariDye);
RBH_CLASS_DEFINE(Event, GariEvent);
RBH_CLASS_DEFINE(Font, GariFont);
RBH_CLASS_DEFINE(Flow, GariFlow);
RBH_CLASS_DEFINE(Game, GariGame);
RBH_CLASS_DEFINE(Image, GariImage);
RBH_CLASS_DEFINE(Joystick, GariJoystick);
RBH_CLASS_DEFINE(Layer, GariLayer);
RBH_CLASS_DEFINE(Music, GariMusic);
RBH_CLASS_DEFINE(Screen, GariScreen);
RBH_CLASS_DEFINE(Sheet, GariSheet);
RBH_CLASS_DEFINE(Sound, GariSound);
RBH_CLASS_DEFINE(Style, GariStyle);
RBH_CLASS_DEFINE(Tileset, GariTileset);
RBH_CLASS_DEFINE(Tile, GariTile);
RBH_CLASS_DEFINE(Vector, GariVector);

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


// converts a ruby string to utf-8
VALUE rb_str_to_utf8(VALUE text) { 
  return rb_str_export_to_enc(text, rb_utf8_encoding());
}


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

/** Checks if self is a full screen screen. */
VALUE rbgari_screen_fullscreen(VALUE vscreen) {  
  return RBH_TOBOOL(gari_screen_fullscreen(GARI_SCREEN_UNWRAP(vscreen)));
}  

VALUE rbgari_screen_showcursor_(VALUE vscreen, VALUE vshow) {
  GariScreen * screen = GARI_SCREEN_UNWRAP(vscreen);
  int show            = RBH_BOOL_INT(vshow);  
  return RBH_TOBOOL(gari_screen_showcursor_(screen, show)); 
}  

/** Returns true if the cursor is shown, false if not. */
VALUE rbgari_screen_showcursor(VALUE vscreen) {
  GariScreen * screen = GARI_SCREEN_UNWRAP(vscreen);  
  return RBH_TOBOOL(gari_screen_showcursor(screen)); 
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

/** 
* Converts a GariDye to a GariColor for the given image. 
*/
VALUE rbgari_dye_color(VALUE vimg, VALUE vdye)  {
  GariImage * image = GARI_IMAGE_UNWRAP(vimg);
  GariDye dye = GARI_DYE_UNWRAP(vdye);
  GariColor aid = gari_dye_color(dye, image);
  GariColor * result = gari_color_rgba(aid.r, aid.g, aid.b, aid.a);    
  return GARI_COLOR_WRAP(result);
}

/** 
* Optimizes a  GariColor for the given image. 
*/
VALUE rbgari_color_optimize(VALUE vcolor, VALUE vimg)  {
  GariColor * color = GARI_COLOR_UNWRAP(vcolor);
  GariImage * image = GARI_IMAGE_UNWRAP(vimg);
  GariColor aid = gari_color_optimize(*color, image);
  GariColor * result = gari_color_rgba(aid.r, aid.g, aid.b, aid.a);    
  return GARI_COLOR_WRAP(result);
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


RBH_GETTER_DEFINE(gari_image_w    , Image, GariImage, RBH_INT_NUM);
RBH_GETTER_DEFINE(gari_image_h    , Image, GariImage, RBH_INT_NUM);
RBH_GETTER_DEFINE(gari_image_depth, Image, GariImage, RBH_INT_NUM);

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
  
VALUE rbgari_image_blendbox(VALUE self, VALUE vx, VALUE vy, 
                        VALUE vw  , VALUE vh, VALUE vc) {
  gari_image_blendbox(GARI_IMAGE_UNWRAP(self), RBH_INT(vx), RBH_INT(vy), 
                  RBH_INT(vw), RBH_INT(vh), *GARI_COLOR_UNWRAP(vc));
  return self;
}
  
  
VALUE rbgari_image_line(VALUE self, VALUE vx, VALUE vy, 
                        VALUE vw  , VALUE vh, VALUE vc) {
  gari_image_line(GARI_IMAGE_UNWRAP(self), RBH_INT(vx), RBH_INT(vy), 
                  RBH_INT(vw), RBH_INT(vh), *GARI_COLOR_UNWRAP(vc));
  return self;
}

VALUE rbgari_image_blendline(VALUE self, VALUE vx, VALUE vy, 
                        VALUE vw  , VALUE vh, VALUE vc) {
  gari_image_blendline(GARI_IMAGE_UNWRAP(self), RBH_INT(vx), RBH_INT(vy), 
                  RBH_INT(vw), RBH_INT(vh), *GARI_COLOR_UNWRAP(vc));
  return self;
}

 
VALUE rbgari_image_disk(VALUE self, VALUE vx, VALUE vy, 
                        VALUE vr  , VALUE vc) {
  gari_image_disk(GARI_IMAGE_UNWRAP(self), RBH_INT(vx), RBH_INT(vy), 
                  RBH_INT(vr),*GARI_COLOR_UNWRAP(vc));
  return self;
}
 
VALUE rbgari_image_blenddisk(VALUE self, VALUE vx, VALUE vy, 
                        VALUE vr  , VALUE vc) {
  gari_image_blenddisk(GARI_IMAGE_UNWRAP(self), RBH_INT(vx), RBH_INT(vy), 
                  RBH_INT(vr),*GARI_COLOR_UNWRAP(vc));
  return self;
}
 
 
VALUE rbgari_image_hoop(VALUE self, VALUE vx, VALUE vy, 
                        VALUE vr  , VALUE vc) {
  gari_image_hoop(GARI_IMAGE_UNWRAP(self), RBH_INT(vx), RBH_INT(vy), 
                  RBH_INT(vr),*GARI_COLOR_UNWRAP(vc));
  return self;
}


VALUE rbgari_image_blendhoop(VALUE self, VALUE vx, VALUE vy, 
                        VALUE vr  , VALUE vc) {
  gari_image_blendhoop(GARI_IMAGE_UNWRAP(self), RBH_INT(vx), RBH_INT(vy), 
                  RBH_INT(vr),*GARI_COLOR_UNWRAP(vc));
  return self;
}

VALUE rbgari_image_flood(VALUE self, VALUE vx, VALUE vy, VALUE vc) {
  gari_image_flood(GARI_IMAGE_UNWRAP(self), RBH_INT(vx), RBH_INT(vy), 
                  *GARI_COLOR_UNWRAP(vc));
  return self;
}

VALUE rbgari_image_blendflood(VALUE self, VALUE vx, VALUE vy, VALUE vc) {
  gari_image_flood(GARI_IMAGE_UNWRAP(self), RBH_INT(vx), RBH_INT(vy), 
                  *GARI_COLOR_UNWRAP(vc));
  return self;
}


VALUE rbgari_image_dot(VALUE self, VALUE vx, VALUE vy, VALUE vc) {
  gari_image_dot(GARI_IMAGE_UNWRAP(self), RBH_INT(vx), RBH_INT(vy), 
                  *GARI_COLOR_UNWRAP(vc));
  return self;
}

VALUE rbgari_image_getdot(VALUE self, VALUE vx, VALUE vy, VALUE vc) {  
  GariColor   color;
  GariColor * result;  
  color = gari_image_getdot(GARI_IMAGE_UNWRAP(self), RBH_INT(vx), RBH_INT(vy));
  result = gari_color_rgba(color.r, color.g, color.b, color.a);
  return GARI_COLOR_WRAP(result);
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

VALUE rbgari_image_setclip(VALUE self, VALUE vx, VALUE vy, VALUE vw, VALUE vh) {
  
  gari_image_setclip(GARI_IMAGE_UNWRAP(self), RBH_INT(vx), RBH_INT(vy), 
                                                  RBH_INT(vw), RBH_INT(vh)); 
  return self;             
}
  
VALUE rbgari_image_getclip(VALUE self) {
  int x, y, w, h;
  if(!gari_image_getclip(GARI_IMAGE_UNWRAP(self), &x, &y, &w, &h)) return Qnil;
  return rb_ary_new3(4, RBH_INT_NUM(x), RBH_INT_NUM(y), 
                        RBH_INT_NUM(w), RBH_INT_NUM(h)); 
}
  
#define GARI_EVENT_WRAP(EVE) RBH_WRAP(Event, EVE, gari_event_free)
#define GARI_EVENT_UNWRAP(EVE) RBH_UNWRAP(Event, EVE)
  
  
VALUE rbgari_event_poll(VALUE self) {
  GariEvent * event = gari_event_pollnew();
  if(event) return GARI_EVENT_WRAP(event);
  return Qnil;
}

/*
#define GARI_EVENT_NONE           0
#define GARI_EVENT_ACTIVE         1
#define GARI_EVENT_KEYDOWN        2
#define GARI_EVENT_KEYUP          3
#define GARI_EVENT_MOUSEPRESS     4
#define GARI_EVENT_MOUSERELEASE   5
#define GARI_EVENT_MOUSEMOVE      6
#define GARI_EVENT_MOUSESCROLL    7
#define GARI_EVENT_JOYMOVE        8
#define GARI_EVENT_JOYPRESS       9
#define GARI_EVENT_JOYRELEASE    10 
#define GARI_EVENT_RESIZE        11
#define GARI_EVENT_EXPOSE        12
#define GARI_EVENT_QUIT          13 
#define GARI_EVENT_USER          14 
#define GARI_EVENT_SYSTEM        15

sruct GariEvent_ {
  uint8_t   kind;
  uint8_t   gain;
  uint16_t  key;
  uint16_t  mod;
  uint16_t  unicode; 
  uint16_t  x, y, xrel, yrel, w, h;
  uint16_t  button;
  int16_t   value; 
  uint8_t   which; 
  uint8_t   axis;
};

*/

VALUE rbgari_event_kind(VALUE self) {
  return RBH_UINT8_NUM(GARI_EVENT_UNWRAP(self)->kind);
}

VALUE rbgari_event_gain(VALUE self) {
  return RBH_UINT8_NUM(GARI_EVENT_UNWRAP(self)->gain);
}

VALUE rbgari_event_key(VALUE self) {
  return RBH_UINT_NUM(GARI_EVENT_UNWRAP(self)->key);
}

VALUE rbgari_event_mod(VALUE self) {
  return RBH_UINT_NUM(GARI_EVENT_UNWRAP(self)->mod);
}

VALUE rbgari_event_unicode(VALUE self) {
  return RBH_UINT_NUM(GARI_EVENT_UNWRAP(self)->unicode);
}

VALUE rbgari_event_x(VALUE self) {
  return RBH_UINT_NUM(GARI_EVENT_UNWRAP(self)->x);
}

VALUE rbgari_event_y(VALUE self) {
  return RBH_UINT_NUM(GARI_EVENT_UNWRAP(self)->y);
}

VALUE rbgari_event_w(VALUE self) {
  return RBH_UINT_NUM(GARI_EVENT_UNWRAP(self)->w);
}

VALUE rbgari_event_h(VALUE self) {
  return RBH_UINT_NUM(GARI_EVENT_UNWRAP(self)->h);
}

VALUE rbgari_event_xrel(VALUE self) {
  return RBH_UINT_NUM(GARI_EVENT_UNWRAP(self)->xrel);
}

VALUE rbgari_event_yrel(VALUE self) {
  return RBH_UINT_NUM(GARI_EVENT_UNWRAP(self)->yrel);
}

VALUE rbgari_event_button(VALUE self) {
  return RBH_UINT_NUM(GARI_EVENT_UNWRAP(self)->button);
}

VALUE rbgari_event_value(VALUE self) {
  return RBH_INT_NUM(GARI_EVENT_UNWRAP(self)->value);
}

VALUE rbgari_event_which(VALUE self) {
  return RBH_UINT8_NUM(GARI_EVENT_UNWRAP(self)->which);
}

VALUE rbgari_event_axis(VALUE self) {
  return RBH_UINT8_NUM(GARI_EVENT_UNWRAP(self)->axis);
}


  
#define GARI_FONT_WRAP(F)   RBH_WRAP(Font, F, gari_font_free)
#define GARI_FONT_UNWRAP(F) RBH_UNWRAP(Font, F)

VALUE rbgari_font_new(VALUE self, VALUE name, VALUE size, VALUE index) {
  char * nam      = RBH_STRING(name);
  int sz          = RBH_INT(size);
  GariFont * font = gari_font_load(nam, sz);
  return GARI_FONT_WRAP(font);
}

VALUE rbgari_font_newindex(VALUE self, VALUE name, VALUE size, VALUE index) {
  char * nam      = RBH_STRING(name);
  int sz          = RBH_INT(size);
  GariFont * font = gari_font_loadindex(nam, sz, RBH_INT(index));  
  return GARI_FONT_WRAP(font);
}


VALUE rbgari_font_error(VALUE self) {
  return RBH_STR_ASCII2(gari_font_error());
}

VALUE rbgari_font_mode(VALUE self) {
  return RBH_INT_NUM(gari_font_mode(GARI_FONT_UNWRAP(self)));
}

VALUE rbgari_font_mode_(VALUE self, VALUE mode) {
  gari_font_mode_(GARI_FONT_UNWRAP(self), RBH_INT(mode));
  return mode;  
}

VALUE rbgari_font_draw(VALUE vimg, VALUE vx, VALUE vy, VALUE vutf8,
                       VALUE vfont, VALUE vfg, VALUE vbg) { 
  GariImage * image = GARI_IMAGE_UNWRAP(vimg);
  int x             = RBH_INT(vx);
  int y             = RBH_INT(vy);    
  char * utf8       = RSTRING_PTR(rb_str_to_utf8(vutf8));
  GariFont  * font  = GARI_FONT_UNWRAP(vfont); 
  GariColor * fg    = GARI_COLOR_UNWRAP(vfg);
  GariColor * bg    = GARI_COLOR_UNWRAP(vbg);
  gari_font_drawcolor(image, x, y, utf8, font, *fg, *bg);
  return vimg;
} 

VALUE rbgari_font_render(VALUE vfont, VALUE vutf8, VALUE vfg, VALUE vbg) {
  GariImage * image = NULL;
  char * utf8       = RSTRING_PTR(rb_str_to_utf8(vutf8));
  GariFont  * font  = GARI_FONT_UNWRAP(vfont); 
  GariColor * fg    = GARI_COLOR_UNWRAP(vfg);
  GariColor * bg    = GARI_COLOR_UNWRAP(vbg);
  image             = gari_font_render(font, utf8, *fg, *bg);
  return GARI_IMAGE_WRAP(image);
} 

VALUE rbgari_font_renderwidth(VALUE self, VALUE vutf8) {
  GariFont * font = GARI_FONT_UNWRAP(self);
  char * utf8     = RBH_CSTR(vutf8);
  return RBH_INT_NUM(gari_font_renderwidth(font, utf8));
} 

/* Getters for the font's various dimensions and style */  
RBH_GETTER_DEFINE(gari_font_height  , Font, GariFont, RBH_INT_NUM);
RBH_GETTER_DEFINE(gari_font_ascent  , Font, GariFont, RBH_INT_NUM);
RBH_GETTER_DEFINE(gari_font_descent , Font, GariFont, RBH_INT_NUM);
RBH_GETTER_DEFINE(gari_font_lineskip, Font, GariFont, RBH_INT_NUM);
RBH_GETTER_DEFINE(gari_font_style   , Font, GariFont, RBH_INT_NUM);
RBH_GETTER_DEFINE(gari_font_faces   , Font, GariFont, RBH_INT_NUM);

VALUE rbgari_font_style_(VALUE self, VALUE vstyle) { 
  gari_font_style_(GARI_FONT_UNWRAP(self), RBH_INT(vstyle));
  return self; 
}  

/** Music and sound. */
#define GARI_AUDIO_WRAP(AUD) RBH_WRAP(Audio, AUD, gari_audioinfo_free)
#define GARI_AUDIO_UNWRAP(AUD) RBH_UNWRAP(Audio, AUD)

/** Queries the current state of the audio system. 
Returns NULL if it was not initialized yet. */
VALUE rbgari_audio_query(VALUE self) {
  GariAudioInfo * info  = gari_audio_query();
  return GARI_AUDIO_WRAP(info);
}

RBH_GETTER_DEFINE(gari_audioinfo_frequency, Audio, GariAudioInfo, RBH_INT_NUM);
RBH_GETTER_DEFINE(gari_audioinfo_channels , Audio, GariAudioInfo, RBH_INT_NUM);
RBH_GETTER_DEFINE(gari_audioinfo_format   , Audio, GariAudioInfo, RBH_INT_NUM);


/** Initializes the audio subsystem for a game. 
Free it with gari_audioinfo_free */
GariGame * gari_audio_init(GariGame * game, int frequency);

/** Cleans up the audio subsystem for a game. */
void gari_audio_done(GariGame * game);

VALUE rbgari_game_openaudio(VALUE vgame, VALUE vfreq) {
  GariGame * res = gari_audio_init(GARI_GAME_UNWRAP(vgame), RBH_INT(vfreq));
  if(!res) return Qnil;
  return rbgari_audio_query(vgame);
}
  
#define GARI_SOUND_WRAP(SND) RBH_WRAP(Sound, SND, gari_sound_free)
#define GARI_SOUND_UNWRAP(SND) RBH_UNWRAP(Sound, SND)
  
/** Creates a new GariSound and loads the sound from a file. */
VALUE rbgari_sound_new(VALUE self, VALUE vname) { 
  char * name       = RBH_STRING(vname);
  GariSound * sound = gari_sound_load(name);
  return GARI_SOUND_WRAP(sound);
}  
 
/** Plays a sound once. */
VALUE rbgari_sound_play(VALUE self) { 
   GariSound * sound = GARI_SOUND_UNWRAP(self);
   if(gari_sound_play(sound)) return self;
   return Qnil;
}   

#define GARI_MUSIC_WRAP(MUS) RBH_WRAP(Music, MUS, gari_music_free)
#define GARI_MUSIC_UNWRAP(MUS) RBH_UNWRAP(Music, MUS)

/** Creates a new GariMusic and loads the music from a file. */
VALUE rbgari_music_new(VALUE self, VALUE vname) { 
  char * name       = RBH_STRING(vname);
  GariMusic * music = gari_music_load(name);
  return GARI_MUSIC_WRAP(music);
}

/** Starts playing the music loop times (-1 means to keep on repeating) 
*   fading in after fade ms. 
*/
VALUE rbgari_music_fadein(VALUE self, VALUE vloops, VALUE vfade) { 
  GariMusic * music = GARI_MUSIC_UNWRAP(self); 
  if(gari_music_fadein(music, RBH_INT(vloops), RBH_INT(vfade))) return self;
  return Qnil;
}

/** Stops playing the music, fading out after fade ms. */
VALUE rbgari_music_fadeout(VALUE self, VALUE vfade) { 
  GariMusic * music = GARI_MUSIC_UNWRAP(self); 
  if(gari_music_fadeout(music, RBH_INT(vfade))) return self;
  return Qnil;
}


/** Joystick handling. */
#define GARI_JOY_WRAP(JOY) RBH_WRAP(Joystick, JOY, gari_joy_free)
#define GARI_JOY_WRAP_NOFREE(JOY) RBH_WRAP(Joystick, JOY, NULL)
#define GARI_JOY_UNWRAP(JOY) RBH_UNWRAP(Joystick, JOY)


VALUE rbgari_joy_amount(VALUE self) {
  return RBH_INT_NUM(gari_joy_amount());
}
  
VALUE rbgari_game_numjoysticks(VALUE self) {
  return RBH_INT_NUM(gari_game_joysticks(GARI_GAME_UNWRAP(self)));
}

VALUE rbgari_game_joystick(VALUE self, int vindex) {
  GariJoystick * joy = 
                 gari_game_joystick(GARI_GAME_UNWRAP(self), RBH_INT(vindex));
  return GARI_JOY_WRAP_NOFREE(joy);
}

/** Returns the name of the inde'th joystick or NULL if no such joystick. */  
VALUE rbgari_joy_nameindex(VALUE self, VALUE vindex) {
  const char * name = gari_joy_nameindex(RBH_INT(vindex));
  if(name) return RBH_STR_UTF82(name);
  return Qnil;
}

/** Opens a joystick. Memory is allocated, so call gari_joy_free when done. */
VALUE rbgari_joy_new(VALUE self, VALUE vindex) {
  GariJoystick * joy = gari_joy_open(RBH_INT(vindex));
  if(joy) return GARI_JOY_WRAP(joy);
  return Qnil;
}

/** Returns the name of a joystick object. */
VALUE rbgari_joy_name(VALUE self) { 
  return RBH_STR_UTF82(gari_joy_name(GARI_JOY_UNWRAP(self)));
}

VALUE rbgari_joy_axes(VALUE self) { 
  return RBH_INT_NUM(gari_joy_axes(GARI_JOY_UNWRAP(self)));
}  

VALUE rbgari_joy_buttons(VALUE self) { 
  return RBH_INT_NUM(gari_joy_buttons(GARI_JOY_UNWRAP(self)));
}

VALUE rbgari_joy_balls(VALUE self) { 
  return RBH_INT_NUM(gari_joy_balls(GARI_JOY_UNWRAP(self)));
}

VALUE rbgari_joy_index(VALUE self) { 
  return RBH_INT_NUM(gari_joy_index(GARI_JOY_UNWRAP(self)));
}



#define GARI_STYLE_WRAP(STY) \
        RBH_WRAP_MARK(Style, STY, gari_style_free, NULL)
#define GARI_STYLE_UNWRAP(STY) RBH_UNWRAP(Style, STY)
   
VALUE rbgari_style_new(VALUE self, VALUE vfore, VALUE vback, 
                     VALUE vfont, VALUE vimage) {
  GariColor * fore = GARI_COLOR_UNWRAP(vfore);
  GariColor * back = GARI_COLOR_UNWRAP(vback);
  GariFont  * font = GARI_FONT_UNWRAP(vfont);
  GariImage * img  = GARI_IMAGE_UNWRAP(vimage);
  return GARI_STYLE_WRAP(gari_style_new(fore, back, font, img));  
}

VALUE rbgari_style_font(VALUE self) {
  return GARI_FONT_WRAP(gari_style_font(GARI_STYLE_UNWRAP(self)));  
}

#define GARI_SHEET_WRAP(SHE)   RBH_WRAP(Sheet, SHE, gari_sheet_free) 
#define GARI_SHEET_UNWRAP(SHE) RBH_UNWRAP(Sheet, SHE)

#define GARI_SHEET_WRAP_NOFREE(SHE) RBH_WRAP(Sheet, SHE, NULL)
#define GARI_IMAGE_WRAP_NOFREE(IMG) RBH_WRAP(Image, IMG, NULL)



VALUE rbgari_sheet_image_(VALUE self, VALUE vimage) {
  GariSheet * sheet = GARI_SHEET_UNWRAP(self);
  GariImage * image = NULL;
  if(NIL_P(image)) {
    gari_sheet_image_(sheet, NULL);
  } else {
    image = GARI_IMAGE_UNWRAP(vimage);
    gari_sheet_image_(sheet, image);
  } 
  // Also sets as instance variable so ruby knows to keep 
  // track of the reference to it when garbage collecting.
  rb_ivar_set(self, rb_intern("image"), vimage);
  return self;
}

VALUE rbgari_sheet_image(VALUE self) {
  GariSheet * sheet = GARI_SHEET_UNWRAP(self);
  return rb_ivar_get(self, rb_intern("image"));  
}

VALUE rbgari_sheet_new(VALUE self, VALUE vimage) {
  GariSheet * sheet = NULL;
  GariImage * image = NULL;
  VALUE instance;
  if(NIL_P(image)) {
    image = NULL;
  } else {
    image = GARI_IMAGE_UNWRAP(vimage);
  }  
  instance = GARI_SHEET_WRAP(gari_sheet_new(image));
  rb_ivar_set(instance, rb_intern("image"), vimage);
  return instance;
}  
   
VALUE rbgari_image_blitsheet(VALUE self, VALUE vx, VALUE vy, VALUE vsheet) {
  GariImage * image   = GARI_IMAGE_UNWRAP(self);
  GariSheet * sheet   = GARI_SHEET_UNWRAP(vsheet);
  int x               = RBH_INT(vx);
  int y               = RBH_INT(vy);
  gari_image_blitsheet(image, x, y, sheet);
  return Qnil;
}   
   
/** 
* A layer is a single layer in a 2D game tile map.
* A layer can consists of a background and/or tiles.
*/
#define GARI_LAYER_WRAP(LAY)  RBH_WRAP(Layer, LAY, gari_layer_free)
#define GARI_LAYER_UNWRAP(LAY) RBH_UNWRAP(Layer, LAY)

#define GARI_LAYER_IVARNAME "@tiles"

VALUE rbgari_layer_init(VALUE self, int gridwide, int gridhigh) {
  // Two levels of arrays, one array per row.
  int index;
  VALUE  vtiles      ;
  vtiles = rb_ary_new2(gridhigh); 
  for(index = 0; index < gridwide; index ++) {
    rb_ary_store(vtiles, index, rb_ary_new2(gridwide));
  } 
  return rb_ivar_set(self, rb_intern(GARI_LAYER_IVARNAME), vtiles); 
}


VALUE rbgari_layer_new(VALUE self, VALUE gw, VALUE gh, VALUE tw, VALUE th) {  
  int gridwide  = RBH_INT(gw); 
  int gridhigh  = RBH_INT(gh); 
  int tilewide  = RBH_INT(tw);
  int tilehigh  = RBH_INT(th);
  VALUE instance;
  instance      = GARI_LAYER_WRAP(gari_layer_new(gridwide,
                                  gridhigh,tilewide,tilehigh));
  rbgari_layer_init(instance, gridwide, gridhigh);  
  return instance;
}

VALUE rbgari_layer_outside(VALUE self, VALUE vgx, VALUE vgy) { 
  GariLayer * layer = GARI_LAYER_UNWRAP(self); 
  return RBH_TOBOOL(gari_layer_outsidegrid(layer, RBH_INT(vgx), RBH_INT(vgy))); 
}

VALUE rbgari_layer_tiles(VALUE self) {
  GariLayer * layer   = GARI_LAYER_UNWRAP(self);
  VALUE vtiles        = rb_ivar_get(self, rb_intern(GARI_LAYER_IVARNAME));
  return vtiles;
}

VALUE rbgari_layer_ivar_set(VALUE self, VALUE vx, VALUE vy, VALUE vsheet) {
  int y        = RBH_INT(vy);   
  int x        = RBH_INT(vx);
  VALUE varray = rbgari_layer_tiles(self);
  VALUE vrow   = Qnil;
  if(y < 0) return Qnil;  
  vrow = RARRAY_PTR(varray)[y];
  if(NIL_P(vrow)) return Qnil;
  if(x < 0) return Qnil;
  rb_ary_store(vrow, x, vsheet);
  return vsheet;
}

VALUE rbgari_layer_ivar_get(VALUE self, VALUE vx, VALUE vy) {
  int y        = RBH_INT(vy);
  int x        = RBH_INT(vx);
  VALUE varray = rbgari_layer_tiles(self);
  VALUE vrow   = Qnil;
  if (y < 0) return Qnil;
  vrow = RARRAY_PTR(varray)[y];
  if(NIL_P(vrow)) return Qnil;
  if (x < 0) return Qnil;
  return RARRAY_PTR(vrow)[x];
}



VALUE rbgari_layer_set(VALUE self, VALUE vx, VALUE vy, VALUE vsheet) { 
  GariLayer * layer     = GARI_LAYER_UNWRAP(self); 
  GariSheet * sheet     = NULL; 
  int x                 = RBH_INT(vx); 
  int y                 = RBH_INT(vy);
  if (gari_layer_outsidegrid(layer, x, y)) return Qnil;
  if(NIL_P(vsheet)) {
    sheet               = NULL;
  } else { 
    sheet               = GARI_SHEET_UNWRAP(vsheet);
  }
  
  if(!gari_layer_set(layer, x, y, sheet)) return Qnil;
  rbgari_layer_ivar_set(self, vx, vy, vsheet); 
  return self;
}

VALUE rbgari_layer_get(VALUE self, VALUE vx, VALUE vy, VALUE vsheet) { 
 GariLayer * layer   = GARI_LAYER_UNWRAP(self); 
// GariSheet * sheet   = NULL;
  int x               = RBH_INT(vx); 
  int y               = RBH_INT(vy);
 //  sheet               = gari_layer_get(layer, x, y);  
  if (gari_layer_outsidegrid(layer, x, y)) return Qnil;
  return rbgari_layer_ivar_get(self, vx, vy);
}

/** Draws the tile layer, with x and y as the top left corner, 
* to the given image. X and y may be negative. 
*/
VALUE rbgari_layer_draw(VALUE self, VALUE vimage, VALUE vx, VALUE vy) {
  GariImage * image   = GARI_IMAGE_UNWRAP(vimage);
  GariLayer * layer   = GARI_LAYER_UNWRAP(self);
  int x               = RBH_INT(vx);
  int y               = RBH_INT(vy);
  gari_layer_draw(layer, image, x, y);  
  return Qnil;
}   

/* Getters for the dimensions of the layer */  
RBH_GETTER_DEFINE(gari_layer_gridwide, Layer, GariLayer, RBH_INT_NUM);
RBH_GETTER_DEFINE(gari_layer_gridhigh, Layer, GariLayer, RBH_INT_NUM);
RBH_GETTER_DEFINE(gari_layer_tilewide, Layer, GariLayer, RBH_INT_NUM);
RBH_GETTER_DEFINE(gari_layer_tilehigh, Layer, GariLayer, RBH_INT_NUM);
RBH_GETTER_DEFINE(gari_layer_wide, Layer, GariLayer, RBH_INT_NUM);
RBH_GETTER_DEFINE(gari_layer_high, Layer, GariLayer, RBH_INT_NUM);


  
/** Gets the clipping rectangle of the image. 
* Clipping applies to all drawing functions. 
*/


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


/** Vector functions  */

#define GARI_VECTOR_WRAP(VEC)   RBH_WRAP(Vector, VEC, gari_vector_free) 
#define GARI_VECTOR_UNWRAP(VEC) RBH_UNWRAP(Vector, VEC)

/* Helper fuinction that allocates memory for a GariVector and wraps it as a value */

VALUE rbgari_vector(GariVector v1) {
  return GARI_VECTOR_WRAP(gari_vector_newvector(v1));
}

VALUE rbgari_vector_new(VALUE vself, VALUE vx, VALUE vy) {
  GariVector * v1 = gari_vector_new(RBH_FLOAT(vx), RBH_FLOAT(vy));
  return GARI_VECTOR_WRAP(v1);
}

VALUE rbgari_vector_equal(VALUE vself, VALUE vother, VALUE vdelta) {
  GariVector * v1 = GARI_VECTOR_UNWRAP(vself);
  GariVector * v2 = GARI_VECTOR_UNWRAP(vother);
  GariFloat delta = RBH_FLOAT(vdelta);
  return RBH_TOBOOL(gari_vector_equal(*v1, *v2, delta));
}

VALUE rbgari_vector_add(VALUE vself, VALUE vother) {
  GariVector * v1 = GARI_VECTOR_UNWRAP(vself);
  GariVector * v2 = GARI_VECTOR_UNWRAP(vother); 
  return rbgari_vector(gari_vector_add(*v1, *v2));
}

VALUE rbgari_vector_sub(VALUE vself, VALUE vother) {
  GariVector * v1 = GARI_VECTOR_UNWRAP(vself);
  GariVector * v2 = GARI_VECTOR_UNWRAP(vother); 
  return rbgari_vector(gari_vector_sub(*v1, *v2));
}

VALUE rbgari_vector_mul(VALUE vself, VALUE vf) {
  GariVector * v1 = GARI_VECTOR_UNWRAP(vself);
  GariFloat    f  = RBH_FLOAT(vf); 
  return rbgari_vector(gari_vector_mul(*v1, f));
}

VALUE rbgari_vector_div(VALUE vself, VALUE vf) {
  GariVector * v1 = GARI_VECTOR_UNWRAP(vself);
  GariFloat    f  = RBH_FLOAT(vf); 
  return rbgari_vector(gari_vector_div(*v1, f));
}

VALUE rbgari_vector_dot(VALUE vself, VALUE vother) {
  GariVector * v1 = GARI_VECTOR_UNWRAP(vself);
  GariVector * v2 = GARI_VECTOR_UNWRAP(vother); 
  return RBH_FLOAT_NUM(gari_vector_dot(*v1, *v2));
}

VALUE rbgari_vector_cross(VALUE vself, VALUE vother) {
  GariVector * v1 = GARI_VECTOR_UNWRAP(vself);
  GariVector * v2 = GARI_VECTOR_UNWRAP(vother); 
  return RBH_FLOAT_NUM(gari_vector_cross(*v1, *v2));
}

VALUE rbgari_vector_perp(VALUE vself) {
  GariVector * v1 = GARI_VECTOR_UNWRAP(vself); 
  return rbgari_vector(gari_vector_perp(*v1));
}

VALUE rbgari_vector_nperp(VALUE vself) {
  GariVector * v1 = GARI_VECTOR_UNWRAP(vself); 
  return rbgari_vector(gari_vector_nperp(*v1));
}

VALUE rbgari_vector_normalize(VALUE vself) {
  GariVector * v1 = GARI_VECTOR_UNWRAP(vself); 
  return rbgari_vector(gari_vector_normalize(*v1));
}

VALUE rbgari_vector_lensq(VALUE vself) {
  GariVector * v1 = GARI_VECTOR_UNWRAP(vself); 
  return RBH_FLOAT_NUM(gari_vector_lensq(*v1));
}

VALUE rbgari_vector_len(VALUE vself) {
  GariVector * v1 = GARI_VECTOR_UNWRAP(vself); 
  return RBH_FLOAT_NUM(gari_vector_len(*v1));
}

VALUE rbgari_vector_angle(VALUE vself) {
  GariVector * v1 = GARI_VECTOR_UNWRAP(vself); 
  return RBH_FLOAT_NUM(gari_vector_angle(*v1));
}

VALUE rbgari_vector_project(VALUE vself, VALUE vother) {
  GariVector * v1 = GARI_VECTOR_UNWRAP(vself);
  GariVector * v2 = GARI_VECTOR_UNWRAP(vother); 
  return rbgari_vector(gari_vector_project(*v1, *v2));
}

VALUE rbgari_vector_rotate(VALUE vself, VALUE vother) {
  GariVector * v1 = GARI_VECTOR_UNWRAP(vself);
  GariVector * v2 = GARI_VECTOR_UNWRAP(vother); 
  return rbgari_vector(gari_vector_rotate(*v1, *v2));
}

VALUE rbgari_vector_unrotate(VALUE vself, VALUE vother) {
  GariVector * v1 = GARI_VECTOR_UNWRAP(vself);
  GariVector * v2 = GARI_VECTOR_UNWRAP(vother); 
  return rbgari_vector(gari_vector_unrotate(*v1, *v2));
}

VALUE rbgari_vector_lerp(VALUE vself, VALUE vother, VALUE vt) {
  GariVector * v1 = GARI_VECTOR_UNWRAP(vself);
  GariVector * v2 = GARI_VECTOR_UNWRAP(vother);
  GariFloat    t  = RBH_FLOAT(vt);
  return rbgari_vector(gari_vector_lerp(*v1, *v2, t));
}

VALUE rbgari_vector_max(VALUE vself, VALUE vmax) {
  GariVector * v1 = GARI_VECTOR_UNWRAP(vself);  
  GariFloat   max = RBH_FLOAT(vmax);
  return rbgari_vector(gari_vector_max(*v1, max));
}

VALUE rbgari_vector_dist(VALUE vself, VALUE vother) {
  GariVector * v1 = GARI_VECTOR_UNWRAP(vself);
  GariVector * v2 = GARI_VECTOR_UNWRAP(vother);  
  return RBH_FLOAT_NUM(gari_vector_dist(*v1, *v2));
}

VALUE rbgari_vector_distsq(VALUE vself, VALUE vother) {
  GariVector * v1 = GARI_VECTOR_UNWRAP(vself);
  GariVector * v2 = GARI_VECTOR_UNWRAP(vother);  
  return RBH_FLOAT_NUM(gari_vector_distsq(*v1, *v2));
}

VALUE rbgari_vector_near(VALUE vself, VALUE vother, VALUE vd) {
  GariVector * v1 = GARI_VECTOR_UNWRAP(vself);
  GariVector * v2 = GARI_VECTOR_UNWRAP(vother);
  GariFloat    d  = RBH_FLOAT(vd);
  return RBH_TOBOOL(gari_vector_near(*v1, *v2, d));
}

VALUE rbgari_vector_fromangle(VALUE self, VALUE vangle) { 
  GariFloat    a  = RBH_FLOAT(vangle);
  return rbgari_vector(gari_vector_fromangle(a));
}   

VALUE rbgari_vector_x(VALUE vself, VALUE vother, VALUE vd) {
  GariVector * v1 = GARI_VECTOR_UNWRAP(vself);
  return RBH_FLOAT_NUM(v1->x);
}

VALUE rbgari_vector_y(VALUE vself, VALUE vother, VALUE vd) {
  GariVector * v1 = GARI_VECTOR_UNWRAP(vself);
  return RBH_FLOAT_NUM(v1->y);
}



/** Entry point for Ruby. */
void Init_gari() {
  RBH_MODULE(Gari);
  RBH_MODULE_CLASS(Gari, Game);
  RBH_MODULE_CLASS(Gari, Audio);
  RBH_MODULE_CLASS(Gari, Sound);
  RBH_MODULE_CLASS(Gari, Music);  
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
  RBH_MODULE_CLASS(Gari, Style);
  RBH_MODULE_CLASS(Gari, Sheet);
  RBH_MODULE_CLASS(Gari, Vector);
  
  
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
  
  RBH_METHOD(Screen, showcursor , rbgari_screen_showcursor  , 0);
  RBH_METHOD(Screen, showcursor=, rbgari_screen_showcursor_ , 1);
  
  RBH_SINGLETON_METHOD(Image, loadraw, rbgari_image_loadraw, 1);
  RBH_SINGLETON_METHOD(Image, new    , rbgari_image_newdepth, 4);
    
  RBH_CLASS_NUM_CONST(Image, SOLID, GariImageSolid);
  RBH_CLASS_NUM_CONST(Image, COLORKEY, GariImageColorkey);
  RBH_CLASS_NUM_CONST(Image, ALPHA, GariImageAlpha);

  // RBH_METHOD(Game, loadimage  , rbgari_game_loadimage, 1);
  
  RBH_GETTER(Image, w         , gari_image_w);
  RBH_GETTER(Image, h         , gari_image_h);
  RBH_GETTER(Image, depth     , gari_image_depth);
  RBH_METHOD(Image, clip?     , rbgari_image_getclip    , 0);
  RBH_METHOD(Image, clip!     , rbgari_image_setclip    , 4);
  
  RBH_METHOD(Image, optimize  , rbgari_image_optimize   , 2);
  RBH_METHOD(Image, slab      , rbgari_image_slab       , 5);
  RBH_METHOD(Image, box       , rbgari_image_box        , 5);
  RBH_METHOD(Image, line      , rbgari_image_line       , 5);
  RBH_METHOD(Image, disk      , rbgari_image_disk       , 4);
  RBH_METHOD(Image, hoop      , rbgari_image_hoop       , 4);
  RBH_METHOD(Image, flood     , rbgari_image_flood      , 3);
  RBH_METHOD(Image, blendslab , rbgari_image_blendslab  , 5);  
  RBH_METHOD(Image, blendbox  , rbgari_image_blendbox   , 5);
  RBH_METHOD(Image, blendline , rbgari_image_blendline  , 5);
  RBH_METHOD(Image, blenddisk , rbgari_image_blenddisk  , 4);
  RBH_METHOD(Image, blendhoop , rbgari_image_blendhoop  , 4);
  RBH_METHOD(Image, blendflood, rbgari_image_blendflood , 3);

  RBH_METHOD(Image, dot       , rbgari_image_dot        , 3);
  RBH_METHOD(Image, getdot    , rbgari_image_getdot     , 2);
  RBH_METHOD(Image, blit      , rbgari_image_blit       , 3);
  RBH_METHOD(Image, blitpart  , rbgari_image_blitpart   , 7);
  RBH_METHOD(Image, blitscale , rbgari_image_scaleblit  , 9);
  
  RBH_METHOD(Image, dyecolor  , rbgari_dye_color        , 1); 
  // RBH_METHOD(Image,   , rbgari_image_ , );
  RBH_METHOD(Screen, fullscreen, rbgari_screen_fullscreen, 0);
  
  
  RBH_METHOD(Color, dye       , rbgari_color_dye, 1);
  
  
  RBH_SINGLETON_METHOD(Color, new , rbgari_color_new, 4);
  RBH_SINGLETON_METHOD(Color, rgba, rbgari_color_new, 4);
  RBH_SINGLETON_METHOD(Color, rgb , rbgari_color_newrgb, 3);
  RBH_GETTER(Color, r, gari_color_r);
  RBH_GETTER(Color, g, gari_color_g);
  RBH_GETTER(Color, b, gari_color_b);
  RBH_GETTER(Color, a, gari_color_a);
  RBH_METHOD(Color, optimize, rbgari_color_optimize, 1);
  RBH_METHOD(Color, dye     , rbgari_color_dye     , 1); 
  
  
  RBH_CLASS_NUM_CONST(Event, NONE         , GARI_EVENT_NONE);
  RBH_CLASS_NUM_CONST(Event, ACTIVE       , GARI_EVENT_ACTIVE);
  RBH_CLASS_NUM_CONST(Event, KEYDOWN      , GARI_EVENT_KEYDOWN);
  RBH_CLASS_NUM_CONST(Event, KEYUP        , GARI_EVENT_KEYUP);
  RBH_CLASS_NUM_CONST(Event, MOUSEPRESS   , GARI_EVENT_MOUSEPRESS);
  RBH_CLASS_NUM_CONST(Event, MOUSERELEASE , GARI_EVENT_MOUSERELEASE);
  RBH_CLASS_NUM_CONST(Event, MOUSEMOVE    , GARI_EVENT_MOUSEMOVE);
  RBH_CLASS_NUM_CONST(Event, MOUSESCROLL  , GARI_EVENT_MOUSESCROLL);
  RBH_CLASS_NUM_CONST(Event, JOYMOVE      , GARI_EVENT_JOYMOVE);
  RBH_CLASS_NUM_CONST(Event, JOYPRESS     , GARI_EVENT_JOYPRESS);
  RBH_CLASS_NUM_CONST(Event, JOYRELEASE   , GARI_EVENT_JOYRELEASE);
  RBH_CLASS_NUM_CONST(Event, RESIZE       , GARI_EVENT_RESIZE);
  RBH_CLASS_NUM_CONST(Event, EXPOSE       , GARI_EVENT_EXPOSE);
  RBH_CLASS_NUM_CONST(Event, QUIT         , GARI_EVENT_QUIT);
  RBH_CLASS_NUM_CONST(Event, USER         , GARI_EVENT_USER);
  RBH_CLASS_NUM_CONST(Event, SYSTEM       , GARI_EVENT_SYSTEM);
  
  RBH_SINGLETON_METHOD(Event, poll        , rbgari_event_poll, 0);
  RBH_METHOD(Event, kind    , rbgari_event_kind     , 0);
  RBH_METHOD(Event, gain    , rbgari_event_gain     , 0);
  RBH_METHOD(Event, key     , rbgari_event_key      , 0);
  RBH_METHOD(Event, mod     , rbgari_event_mod      , 0);
  RBH_METHOD(Event, unicode , rbgari_event_unicode  , 0);
  RBH_METHOD(Event, x       , rbgari_event_x        , 0);
  RBH_METHOD(Event, y       , rbgari_event_y        , 0);
  RBH_METHOD(Event, xrel    , rbgari_event_xrel     , 0);
  RBH_METHOD(Event, yrel    , rbgari_event_yrel     , 0);
  RBH_METHOD(Event, w       , rbgari_event_w        , 0);
  RBH_METHOD(Event, h       , rbgari_event_h        , 0);
  RBH_METHOD(Event, button  , rbgari_event_button   , 0);
  RBH_METHOD(Event, value   , rbgari_event_value    , 0);
  RBH_METHOD(Event, which   , rbgari_event_which    , 0);
  RBH_METHOD(Event, axis    , rbgari_event_axis     , 0);
  
  RBH_CLASS_NUM_CONST(Font, ALPHA_SOLID, GARI_ALPHA_SOLID);
  RBH_CLASS_NUM_CONST(Font, ALPHA_CLEAR, GARI_ALPHA_CLEAR);
  
  RBH_SINGLETON_METHOD(Font , new     , rbgari_font_new      , 2);
  RBH_SINGLETON_METHOD(Font , newindex, rbgari_font_newindex , 3);
  RBH_SINGLETON_METHOD(Font , error , rbgari_font_error , 0);
  RBH_METHOD(Font , mode    , rbgari_font_mode  , 0);
  RBH_METHOD(Font , mode=   , rbgari_font_mode_ , 1);
  RBH_METHOD(Image, text    , rbgari_font_draw  , 6);
  
  RBH_METHOD(Font , render  , rbgari_font_render, 3);
  RBH_METHOD(Font , width_of, rbgari_font_renderwidth, 1);
  
  RBH_GETTER(Font, height   , gari_font_height);
  RBH_GETTER(Font, ascent   , gari_font_ascent);
  RBH_GETTER(Font, descent  , gari_font_descent);
  RBH_GETTER(Font, lineskip , gari_font_lineskip);
  RBH_GETTER(Font, style    , gari_font_style);
  RBH_GETTER(Font, faces    , gari_font_faces);
  
  RBH_METHOD(Font, style=   , rbgari_font_style_, 1); 
  
  RBH_CLASS_NUM_CONST(Font  , BLENDED   , GariFontBlended); 
  RBH_CLASS_NUM_CONST(Font  , SHADED    , GariFontSolid);
  RBH_CLASS_NUM_CONST(Font  , SOLID     , GariFontShaded);
  RBH_CLASS_NUM_CONST(Font  , NORMAL    , GariFontNormal);
  RBH_CLASS_NUM_CONST(Font  , ITALIC    , GariFontItalic);
  RBH_CLASS_NUM_CONST(Font  , BOLD      , GariFontBold);
  RBH_CLASS_NUM_CONST(Font  , UNDERLINE , GariFontUnderline);
  
  RBH_METHOD(Game           , openaudio , rbgari_game_openaudio , 1);
  RBH_SINGLETON_METHOD(Audio, open      , rbgari_game_openaudio , 1);
  
  RBH_CLASS_NUM_CONST(Audio , DEFAULT_FREQUENCY                 , 22050);
  RBH_GETTER(Audio, frequency          , gari_audioinfo_frequency);
  RBH_GETTER(Audio, channels           , gari_audioinfo_channels);
  RBH_GETTER(Audio, format             , gari_audioinfo_format);
  
  RBH_SINGLETON_METHOD(Sound, new       , rbgari_sound_new      , 1);
  RBH_METHOD(Sound          , play      , rbgari_sound_play     , 0);
  RBH_SINGLETON_METHOD(Music, new       , rbgari_music_new      , 1);
  RBH_METHOD(Music          , fade_in   , rbgari_music_fadein   , 0);
  RBH_METHOD(Music          , fade_out  , rbgari_music_fadeout  , 0);
  
  RBH_SINGLETON_METHOD(Joystick , amount    , rbgari_joy_amount         , 0);
  RBH_SINGLETON_METHOD(Joystick , name      , rbgari_joy_nameindex      , 1);
  RBH_SINGLETON_METHOD(Joystick , new       , rbgari_joy_new            , 1);
  RBH_METHOD(Joystick           , name      , rbgari_joy_name           , 0);
  RBH_METHOD(Joystick           , axes      , rbgari_joy_axes           , 0);
  RBH_METHOD(Joystick           , buttons   , rbgari_joy_buttons        , 0);
  RBH_METHOD(Joystick           , balls     , rbgari_joy_balls          , 0);
  RBH_METHOD(Joystick           , index     , rbgari_joy_index          , 0);
  
  RBH_METHOD(Game               , joysticks , rbgari_game_numjoysticks  , 0);
  RBH_METHOD(Game               , joystick  , rbgari_game_joystick      , 1);
    
  RBH_SINGLETON_METHOD(Style    , new       , rbgari_style_new          , 4);
  RBH_METHOD(Style              , font      , rbgari_style_font         , 0);
  
  RBH_SINGLETON_METHOD(Sheet    , new         , rbgari_sheet_new        , 1);
  RBH_METHOD(Sheet              , image=      , rbgari_sheet_image_     , 1);
  RBH_METHOD(Sheet              , image       , rbgari_sheet_image      , 0);
  RBH_METHOD(Image              , blitsheet   , rbgari_image_blitsheet  , 3);
  
  RBH_SINGLETON_METHOD(Layer    , new         , rbgari_layer_new        , 4);  
  RBH_GETTER(Layer, w           , gari_layer_wide);
  RBH_GETTER(Layer, h           , gari_layer_high);
  RBH_GETTER(Layer, tilewide    , gari_layer_tilewide);
  RBH_GETTER(Layer, tilehigh    , gari_layer_tilehigh);
  RBH_GETTER(Layer, gridwide    , gari_layer_gridwide);
  RBH_GETTER(Layer, gridhigh    , gari_layer_gridhigh);  
  RBH_METHOD(Layer              , outside?    , rbgari_layer_outside    , 2);
  RBH_METHOD(Layer              , set         , rbgari_layer_set        , 3);
  RBH_METHOD(Layer              , get         , rbgari_layer_get        , 2);
  RBH_METHOD(Layer              , draw        , rbgari_layer_draw       , 3);
  
  RBH_SINGLETON_METHOD(Vector   , new         , rbgari_vector_new       , 2);
  RBH_SINGLETON_METHOD(Vector   , angle       , rbgari_vector_fromangle , 1);  
  RBH_METHOD(Vector             , equal_delta?, rbgari_vector_equal     , 2);
  RBH_METHOD(Vector             , +           , rbgari_vector_add       , 1);
  RBH_METHOD(Vector             , -           , rbgari_vector_sub       , 1);
  RBH_METHOD(Vector             , *           , rbgari_vector_mul       , 1);
  RBH_METHOD(Vector             , /           , rbgari_vector_div       , 1);
  RBH_METHOD(Vector             , dot         , rbgari_vector_dot       , 1);
  RBH_METHOD(Vector             , cross       , rbgari_vector_cross     , 1);
  RBH_METHOD(Vector             , perp        , rbgari_vector_perp      , 0);
  RBH_METHOD(Vector             , nperp       , rbgari_vector_nperp     , 0);
  RBH_METHOD(Vector             , normalize   , rbgari_vector_normalize , 0);
  RBH_METHOD(Vector             , lensq       , rbgari_vector_lensq     , 0);
  RBH_METHOD(Vector             , len         , rbgari_vector_len       , 0);
  RBH_METHOD(Vector             , length      , rbgari_vector_len       , 0);
  RBH_METHOD(Vector             , l           , rbgari_vector_len       , 0);
  RBH_METHOD(Vector             , a           , rbgari_vector_angle     , 0);
  RBH_METHOD(Vector             , angle       , rbgari_vector_angle     , 0);
  RBH_METHOD(Vector             , project     , rbgari_vector_project   , 1);
  RBH_METHOD(Vector             , rotate      , rbgari_vector_rotate    , 1);
  RBH_METHOD(Vector             , unrotate    , rbgari_vector_unrotate  , 1);
  RBH_METHOD(Vector             , lerp        , rbgari_vector_lerp      , 2);
  RBH_METHOD(Vector             , max         , rbgari_vector_max       , 1);
  RBH_METHOD(Vector             , dist        , rbgari_vector_dist      , 1);
  RBH_METHOD(Vector             , distsq      , rbgari_vector_distsq    , 1);
  RBH_METHOD(Vector             , near        , rbgari_vector_near      , 2);
  RBH_METHOD(Vector             , x           , rbgari_vector_x         , 0);
  RBH_METHOD(Vector             , y           , rbgari_vector_y         , 0);
  
}

/** Alternative entry point for Ruby. */
void Init_libgari() {
  Init_gari();
}

