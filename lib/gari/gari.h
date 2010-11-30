/**
* @mainpage Gari Game Library
*
* Gari is a simple, fast, cross platform game library targeted at 2D games,
* written in plain C. For now, it uses SDL as a backend, but that might 
* change later to use more low-level back-ends. It's goals are:
*
* 1) Targeted to make writing 2D games as easy as possible.
* 2) A simple API, that hides the details as much as possible.
* 3) Fast, giving good performance everywhere possible.
* 4) Cross platform. Enough said.
* 5) Complete yet concise. All you need to write 2D games, but nothing more.
* 6) Release under Zlib license (or similar permissive).
* 7) Easy to embed in Ruby with bindings provided (later).
*
* A C compiler which supports some C99 fratures, particularly
* stdint.h, __VA_ARGS__, and one line comments is needed to compile Gari.
*
* More details are documented in gari.h
* 
* @file gari.h
*
*/

#ifndef _GARI_H_
#define _GARI_H_

#include <SDL.h>
#include <stdlib.h>
#include <stdint.h>

/* 
  Some platforms will need a __declspec(dllexport) or something for 
  functions in a shared library.
*/
#ifndef GARI_EXPORT_FUNC
#define GARI_EXPORT_FUNC extern
#endif
  
/*
* Some platforms may require a change in calling convention
*/  
#ifndef GARI_CALL_FUNC
#define GARI_CALL_FUNC
#endif 

/* All in a handy wrapper macro */
#define GARI_FUNC(RESULT) GARI_EXPORT_FUNC RESULT GARI_CALL_FUNC
 
/* An another wrapper macro to help typedefing such functions */
#define GARI_FUNCTYPE(RESULT) typedef RESULT GARI_CALL_FUNC


/** Default color depth to open the creen with. Normally 32.  */
#ifndef GARI_DEFAULT_DEPTH 
#define GARI_DEFAULT_DEPTH 32
#endif

/** Of course we need fake booleans if we don' talready have them :p. */
#ifndef FALSE
#define FALSE (0)
#endif

#ifndef TRUE
#define TRUE (!(FALSE))
#endif

/** Common alpha values */
#define GARI_ALPHA_OPAQUE      255
#define GARI_ALPHA_SOLID       255
#define GARI_ALPHA_HALF        128
#define GARI_ALPHA_CLEAR       0
#define GARI_ALPHA_TRANSLUCENT 0


/** Dye. A Dye is a device and image dependent representation of a color 
    that fits inside a uint32_t types.
    Dyes are used in lower level API's. 
*/
typedef uint32_t GariDye;

/** GariColor models colors as RBGA values in an sRGBA color space.
* GariColor should be used as if it is immutable.
* Colors are used in stead of Dyes in higher level API's.  
*/
struct GariColor_ {
  uint8_t r, g, b, a;
};

typedef struct GariColor_ GariColor;

/** Frees the memory associated with a GariColor allocated though
*   gari_color_rgba or gari_color_rgb.  Returns NULL (guaranteed).
*/
GariColor * gari_color_free(GariColor * color); 

/** Allocates a new color and initializes it with the given r, g, b and a values
*/
GariColor * gari_color_rgba(uint8_t r, uint8_t g, uint8_t b, uint8_t a);

/** Allocates a new color and initializes it with the given r, g, b values
*   a will be set to GARI_ALPHA_OPAQUE.
*/ 
GariColor * gari_color_rgb(uint8_t r, uint8_t g, uint8_t b);  


/** Returns a GariColor struct initialized with the given 
* r, g, b, a components.
*/
GariColor gari_colora(uint8_t r, uint8_t g, uint8_t b, uint8_t a);


/** Returns a GariColor struct initialized with the given r, g, b components. 
* The a component will be set to GARI_ALPHA_SOLID.
*/
GariColor gari_color(uint8_t r, uint8_t g, uint8_t b);

/** Gets the r component of a GariColor. */
uint8_t gari_color_r(GariColor * rgba);

/** Gets the g component of a GariColor. */
uint8_t gari_color_g(GariColor * rgba);

/** Gets the b component of a GariColor. */
uint8_t gari_color_b(GariColor * rgba);

/** Gets the a component of a GariColor. */
uint8_t gari_color_a(GariColor * rgba);


/** Alpha Levels. Alpha Levels are models as simple uint8_t types. */
typedef uint8_t GariAlpha;

/** Images. Images arevisuals that can be dwawn to. */
struct GariImage_; 
typedef struct GariImage_ GariImage;

/** A screen or window. Not directly drawable. */
struct GariScreen_; 
typedef struct GariScreen_ GariScreen;

/** Game info. */
struct GariGame_; 
typedef struct GariGame_ GariGame;

/** Drawing information */
struct GariDraw_; 
typedef struct GariDraw_ GariDraw;




/** Drawing callback function. */
typedef int GariDrawFunction(GariDraw * data, int px, int py); 



/** 
* Initializes Gari Game and returns it. Must be done before anything else. 
*/
GariGame * gari_game_make();

/** Finishes and cleans up a gari game when we're done with it. */
void gari_game_free(GariGame * game);

/** Updates the game screen, FPS, etc. */
GariGame * gari_game_update(GariGame * game); 

/** Sets the frames of the game back to 0.*/
GariGame * gari_game_resetframes(GariGame * game); 

/** Gets the total amount of frames displayed since the start of the game, 
   or since gari_game_resetframes was called. */
uint32_t gari_game_frames(GariGame * game);

/** Advances the game's frame counter. */
GariGame * gari_game_nextframe(GariGame * game);

/** Starts FPS counter. gari_game_resetframes also calls this. */
GariGame * gari_game_startfps(GariGame * game);

/** Returns calculated fps after calling startfps . */
double gari_game_fps(GariGame * game);

/** Reports on the game's FPS on stderr.*/
void gari_game_report(GariGame * game);


/** Opens the game main game screen or window. Must be done before using any image functions, or before calling game.update . */
GariScreen * gari_game_openscreendepth(GariGame * game, int wide, int high, int fullscreen, int depth); 

/** Opens the game main game screen or window. 
   Must be done before using any image functions. */   
GariScreen * gari_game_openscreen(GariGame * game, int wide, int high, 
                                  int fullscreen);

/** Retuns the current main game screen. */
GariScreen * gari_game_screen(GariGame * game);

/** Returns the drawable image for the screen. */
GariImage * gari_screen_image(GariScreen * screen);

/** Checks if the main game screen is in fullscreen mode. */ 
int gari_game_fullscreen(GariGame * game);

/** Can be used to set or unset fullscreen after opening the screen. */
GariScreen * gari_game_fullscreen_(GariGame * game, int fullscreen); 


/** Image mode, this deterines how to optimize the image for drawing to the screen, and whether the image has any transparency. */
enum GariImageMode_ { 
  GariImageSolid      = 0,
  GariImageColorkey   = 1,
  GariImageAlpha      = 2
};


/** Makes an empty gari image with given bits per pixels (in depth), 
    but supporting either alpha, or not, depending on mode.
    The video mode must have been set and the screen must have been opened.
    You must call gari_image_free when you're done with the generated 
    GariImage. May return NULL on error.
*/
GariImage * gari_image_makedepth(int w, int h, int depth, int mode);


/** Makes an empty gari image with the same bits per pixels as the screen, 
    but supporting either alpha, or not, or a colorkey depending on mode.
    The video mode must have been set and the screen must have been opened.
    You must call gari_image_free when you're done with the generated 
    GariImage. May return NULL on error.
*/
GariImage * gari_image_make(int w, int h, int mode, GariDye dyekey);

/** Disposes of an image. */
void gari_image_free(GariImage * image);

/** Image loading functions. */

/** Loads the image from the named file. */
GariImage * gari_image_loadraw(char * filename);

/** Loads the image from the named file and tries to optimize it for display on the game's screen. This requires the game's screen to have been opened first. */
GariImage * gari_image_load(GariGame * game, char * filename);

/** Optimizes the image for drawing to the screen. */
GariImage * gari_image_optimize(GariImage * image, int mode, GariDye dyekey);

/** Returns the width of the image. */
int gari_image_w(GariImage * img);
 
/** Returns the height of the image. */
int gari_image_h(GariImage * img);

/** Locks image before drawing. Needed before any _nolock drawing function. */
void gari_image_lock(GariImage * image); 
/** Unlocks image after drawing. Needed after any _nolock drawing function. */
void gari_image_unlock(GariImage * image); 

/** Converts a color expressed in rgb components to a GariDye. */
GariDye gari_image_rgb(GariImage *img, uint8_t r, uint8_t g, uint8_t b);

/** Converts a color expressed in rgba components to a GariDye. */
GariDye gari_image_rgba(GariImage *img, 
          uint8_t r, uint8_t g, uint8_t b, uint8_t a);
/** 
* Converts a GariColor to a GariDye for the given image. 
*/
GariDye gari_color_dye(GariColor color, GariImage * image);
          
/** 
  Concstructs a color expressed as an RGBA quadruplet and returns it as a 
  struct.
*/          
GariColor gari_rgba(uint8_t r, uint8_t g, uint8_t b, uint8_t a); 
         

/** 
* Converts a GariColor to a gari dye for the given image. 
* If the color's A is solid, then it uses SDL_MapRGB internally.
* otherwise, it uses SDL_MapRGBA internally. 
*/
GariDye gari_color_dye(GariColor color, GariImage * image); 

/** Checks if two gariColors are equal. Colors will sort by ascending a,
r, g and b */
int gari_color_cmp(GariColor c1, GariColor c2);

/** Converts a GariDye to a GariColor for the given image. */
GariColor gari_dye_color(GariDye dye, GariImage * image);         

/** Drawing functions. */
typedef void GariPutPixelFunc(GariImage * img, int x, int y, GariDye dye);
typedef GariDye GariGetPixelFunc(GariImage * img, int x, int y);
typedef void GariBlendPixelFunc(GariImage * img, int x, int y, GariDye dye, GariAlpha alpha);

/** Returns a function that can be used to put a pixel on this GariImage.
* The function returned does no locking. May return NULL if the format does 
* not use 1,2,3 or 4 bytes per pixel.    
*/
GariPutPixelFunc * gari_image_putpixelfunc_nl(GariImage * image);

/** Returns a function that can be used to get a pixel from this GariImage.
* The function returned does no locking. May return NULL if the format does 
* not use 1,2,3 or 4 bytes per pixel.
*/    
GariGetPixelFunc * gari_image_getpixelfunc_nl(GariImage * image); 

/** Returns a function that can be used to blend a pixel from this GariImage.
* The function returned does no locking. May return NULL if the format does 
* not use 1,2,3 or 4 bytes per pixel.
*/    
GariBlendPixelFunc * gari_image_blendpixelfunc_nl(GariImage * image); 


/* Drawing context, used for abstracting certain drawing functions.*/
struct GariDraw_ {
  GariImage        *  image; // image we're drawing to. 
  GariDrawFunction *  draw;  // Generic drawing function to use.
  GariGetPixelFunc *  getpixel; // Get pixel function (for the image) 
  GariPutPixelFunc *  putpixel; // Put pixel function (for the image)
  GariBlendPixelFunc *blendpixel; // Blend pixel function (for the image)
  GariColor           color; // Color to draw with.
  GariDye             dye;   // Dye to draw with.
  GariAlpha           alpha; // alpha value to be used when drawing
  void *              other; // other user-defined data.
};

/* Constructs drawing information */
GariDraw * gari_drawdata_init(
                              GariDraw          * data,
                              GariImage         * image,
                              GariDrawFunction  * func,
                              GariColor color,
                              GariAlpha alpha
                             ); 
                             
/** Sets the clipping rectangle of the image. 
* Clipping applies to all drawing functions. 
*/
GariImage * gari_image_setclip(GariImage * img, 
              int x, int y, int w, int h); 
  
/** Gets the clipping rectangle of the image. 
* Clipping applies to all drawing functions. 
*/
GariImage * gari_image_getclip(GariImage * img, 
              int *x, int *y, int *w, int *h);

/** Fills the image with the given dye. Ignores clipping. */
void gari_image_fill(GariImage * image,  GariDye dye);

/** Low level putpixel function.         */
void gari_image_putpixel(GariImage * image, int x, int y, GariDye dye);

/** High level level putpixel function.  */
void gari_image_dot(GariImage * image, int x, int y, GariColor color);

/** High level blend pixel function */
void gari_image_blenddot(GariImage * image, int x, int y, GariColor color);

/** High level getpixel function */
GariColor gari_image_getdot(GariImage * image, int x, int y);

/** Draws an arbitrary line on the image starting at x1, y1, and fitting inside
the rectangle with the size w and h.     */
void gari_image_line(GariImage * image, int x1, int y1, 
                     int w, int h, GariColor color);

/** Optimized case of gari_image_line, draws horizontal lines with 
    a width of w only. */
void gari_image_hline(GariImage * image, int x, int y, int w, GariColor color);

/** Optimized case of gari_image_line, draws vertical lines with 
   a height h only. */
void gari_image_vline(GariImage * image, int x, int y, int h, GariColor color); 

/** Draws a hoop, that is, an empty circle, on the image. */
void gari_image_hoop(GariImage * image, int x, int y, int r, GariColor color);

/** Draws a disk, or a filled circle, on the image. */
void gari_image_disk(GariImage * image, int x, int y, int r, GariColor color);

/** Draws a box, or an open rectangle on the image, starting at (x, y) */
void gari_image_box(GariImage * image, int x, int y, 
                                       int w, int h, GariColor color);

/** Draws a slab, which is a filled rectange, on the image. */
void gari_image_slab(GariImage * image, int x, int y, 
                      int w, int h, GariColor color);

/** Blends an arbitrary line on the image starting at x1, y1, and fitting inside
the rectangle with the size w and h.     */
void gari_image_blendline(GariImage * image, int x1, int y1, 
                     int w, int h, GariColor color);
                     
/** Blends a box, which is an open rectange, on the image. */
void gari_image_blendbox( GariImage * image, int x, int y, int w, int h,  
                      GariColor color);                     

/** Draws a blended slab, which is a filled rectange, on the image. */
void gari_image_blendslab( GariImage * image, int x, int y, int w, int h,  
                      GariColor color);

/** Draws a blended disk, which is a filled circle, on the image. */       
void gari_image_blenddisk(GariImage * image, int x, int y, int r, GariColor color);

/** Draws a blended hoop, which is a circle outline, on the image. */       
void gari_image_blendhoop(GariImage * image, int x, int y, int r, GariColor color);

/** Draws a blended floodfill. */
void gari_image_blendflood(GariImage * image, int x, int y, GariColor color);


/* Text drawing functions. Only support UTF-8. */
int gari_image_text(GariImage * dst, int x, int y,  
                    char * utf8, GariColor color);

/* Fast blit from image to image. */
void gari_image_blit(GariImage * dst, int dstx, int dsty, GariImage * src);

/** Blits a part of one image to another one with the given coordinates. Takes the indicated parts of the image, as specified by srcx, srcy, srcw and srch  */
void gari_image_blitpart(GariImage * dst, int dstx, int dsty, GariImage * src,
int srcx, int srcy, int srcw, int srch);

/* Copy part from image to image. */
int gari_image_copy(GariImage * dst, int dstx, int dsty, GariImage * src,
int srcx, int srcy, int w, int h);

/** Blits (a part of) an image to another one, 
    scaling it arbitrarily to the wanted size. Uses no 
    interpolation nor smoothing. 
*/
void gari_image_scaleblit(GariImage * dst, int dstx, int dsty, 
                                      int dsth, int dstw, 
                                      GariImage * src,
                                      int srcx, int srcy, 
                                      int srch, int srcw);

/** Flood fills (a part of) the image with color starting from x and y. */
void gari_image_flood(GariImage * image, int x, int y, GariColor color);


/* Input event handling. */


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


/** The event structiure. It's not a union for ease of wrapping in other languages, though only some of the fields may contain useful data depending on the. event.kind field. */
struct GariEvent_ {
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
struct GariEvent_;
typedef struct GariEvent_ GariEvent;


/** Gets an event from the event queue. Returns a special event with 
event.kind == GARI_EVENT_NONE if no events are on the queue. 
*/
GariEvent gari_event_poll();

/** Polls the event queue and copies it to the given event  addres, 
* wich must be a valid address and not be null.
* Return NULL if the eevnt queue was ampty, non-null if there was an event fetched. 
*/
GariEvent * gari_event_fetch(GariEvent * event);

/** Polls the event queue and returns a newly alloctaded GariEvent with 
* the top event of the queue. Returns NULL, and allocates no memory if 
* the queue is empty.
*/
GariEvent * gari_event_pollnew();

/** Frees memory associated with a Garievent alloctated by gari_event_pollnew.*/
void gari_event_free(GariEvent * event);


/* Fonts and text rendering. */

struct GariFont_;
/** Opaque struct for font handling. */
typedef struct GariFont_ GariFont;

/** Ways to render the font. Default is GariFontSolid */
enum GariFontMode_ {
  GariFontSolid,
  GariFontShaded,
  GariFontBlended,
};

/** Loads one of the fonts in a font file. */
GariFont * gari_font_loadindex(char * filename, int ptsize, long index);
/** Loads the first font in a font file. */
GariFont * gari_font_load(char * filename, int ptsize);

/** Sets the drawing mode of the font. */
GariFont * gari_font_mode_(GariFont * font, int mode);

/** Gets the drawing mode of the font. */
int gari_font_mode(GariFont * font);

/** Frees the memory allocated by the font. */
void gari_font_free(GariFont *font); 

/** Draws font with given colors. */
void gari_font_drawcolor(GariImage * image, int x, int y, char * utf8, 
                         GariFont  * font , GariColor fg, GariColor bg); 

/** Draws font with given color components. */
void gari_font_draw(GariImage * image, int x, int y, char * utf8, GariFont * font, uint8_t fg_r, uint8_t fg_g, uint8_t fg_b, uint8_t bg_r, uint8_t bg_b, uint8_t bg_a);

/** Draws font in printf style. Won't work on platforms that lack vsnprintf.
* Will draw up to 2000 bytes of characters.  
*/
void gari_font_printf(GariImage * image, int x, int y, GariFont * font, GariColor fg, GariColor bg, char * format, ...);

/** Returns a text with details about the last error in loading or 
handling a font. */
char * gari_font_error();

/** Music and sound. */

#define GARI_AUDIO_LOWFREQENCY 8000
#define GARI_AUDIO_MEDIUMFREQENCY 22050
#define GARI_AUDIO_HIGHFREQENCY 44100

/** Initializes the audio subsystem for a game. */
GariGame * gari_audio_init(GariGame * game, int frequency);
/** Cleans up the audio subsystem for a game. */
void gari_audio_done(GariGame * game);

struct GariSound_;
typedef struct GariSound_ GariSound;

/** Initialises and loads sound into an existing GariSound record. */
GariSound * gari_sound_init(GariSound * sound, char * filename);

/** Creates a new GariSound and loads the sound from a file. */
GariSound * gari_sound_load(char * filename);

/** Deallocates the loaded sound, but not the wrapper GariSound itself. */
GariSound * gari_sound_done(GariSound * sound);

/** Calls gari_sound_done and then frees the sound itself.*/
GariSound * gari_sound_free(GariSound * sound);

/** Plays a sound once. */
GariSound * gari_sound_play(GariSound * sound);


struct GariMusic_;
typedef struct GariMusic_ GariMusic;

/** Initialises and loads music into an existing GariMusic record. */
GariMusic * gari_music_init(GariMusic * music, char * filename);

/** Creates a new GariMusic and loads the music from a file. */
GariMusic * gari_music_load(char * filename);

/** Deallocates up the loaded music, but not the wrapper GariMusic itself. */
GariMusic * gari_music_done(GariMusic * music);

/** Calls gari_music_done and then frees the music itself.*/
GariMusic * gari_music_free(GariMusic * music);

/** Starts playing the music loop times (-1 means to keep on repeating) 
*   fading in after fade ms. 
*/
GariMusic * gari_music_fadein(GariMusic * music, int loops, int fade); 

/** Stops playing the music, fading out after fade ms. */
GariMusic * gari_music_fadeout(GariMusic * music, int fade); 


 


/* Higher level functions and structs. */

/** Camera  A camera models a 2D point of view over a tile map, etc. */
struct GariCamera_;
typedef struct GariCamera_ GariCamera;


/* Known particle engine effects: */
enum GariFlowKind_ {
  GariFlowSnow,
  GariFlowRain,
  GariFlowBlood,
  GariFlowSlash,
  GariFlowExplode,
  GariFlowDamage,
};
typedef enum GariFlowKind_ GariFlowKind;


/** A drop is a particle in the particle engine */
struct GariDrop_;
typedef struct GariDrop_ GariDrop;

/** A well is an initial configuration af a certain amount of GariDrop 
  particles in a GariFlow particle engine. */
struct GariWell_;
typedef struct GariWell_ GariWell;

/** A Flow is an instance of a particle engine. 
*/
struct GariFlow_;
typedef struct GariFlow_ GariFlow;

/** Particle engine init callback function. */
typedef GariDrop * GariDropInitFunction(GariDrop * data, GariWell * well); 

/** Particle engine draw callback function. */
typedef GariDrop * GariDropDrawFunction(GariDrop * data, GariImage * im); 

/** Particle engine update callback function. */
typedef GariDrop * GariDropUpdateFunction(GariDrop * data, int time); 

/** Allocates a new GariFlow particle engine with the given amount of 
*   particles available. Free it after use with gari_flow_free(); 
*/
GariFlow * gari_flow_make(size_t size);

/**
* Free and destroy a previously allocated GariFlow particle engine.
*/
void gari_flow_free(GariFlow * flow); 

/**
* Updates the position of the pixels after time ticks passed.
*/
void gari_flow_update(GariFlow * flow, int time); 
 
/** Draws all active particles from tthis flow to the screen. */
void gari_flow_draw(GariFlow * flow, GariImage * im); 

/**
* Activates amount Drops (particles) in the GariFlow flow. 
* The particle will be of kind kind, and will be placed, depending on the 
* kind, at x a,d y, and displayed using given color, image or text 
*/
GariFlow * gari_flow_activate(GariFlow * flow, 
      int amount, int kind, int x, int y, GariColor color, 
      GariImage * im, char * text);

/** Initializes the random number generator, 
    used mainly in the particle engine. */
void gari_random_init(); 

/** Gets a random number between min and max. */
long gari_random(long min, long max);




/** A sprite is a fully aniated object that can be animated in different ways * depending on it's facing direction and actions.
* For example, the sprite may have a walking action in 2 or 4 directions,
* which each have a different animation.
* A sprite may also be consisted of different parts or layers superimposed 
* over each other, for example, a character holding a weapon, etc.  
*/
struct GariSprite_;
typedef struct GariSprite_ GariSprite;

/** An animation is a full cycle of an animation, 
*   of which sprites are made up. 
*/
struct GariAnime_;
typedef struct GariAnime_ GariAnime;

/** Anime can be described as a crude "program", 
  using the following constants: */
enum GariAnimeProgram_ {
  GariAnimeRewind,
  GariAnimeNext,
  GariAnimeWait
};


/** 
 * A tile is a set of images that gets animated according to a 
 * fixed pattern, and displayed as a single part of a layer in a map. 
 */
struct GariTile_; 
typedef struct GariTile_ GariTile;

/** Flags for tiles that deterine their in-game properties. */
enum GariTileFlags_ {   
    GariTileWalk = 1 << 0, // The tile can be walked over.
    GariTileJump = 1 << 1, // The tile can be jumped over.
    GariTileLedge= 1 << 2, // The tile will cause a jump when walked over.
    GariTileSwim = 1 << 3, // The tile is some kind of swimmable liquid.
    GariTilePain = 1 << 4, // Walking over the tile causes pain.
    GariTileUp   = 1 << 5, // The tile is a step-up stair to the next layer.
    GariTileDown = 1 << 6, // The tile is a step-down to the previous layer.    
    GariTileEdge = 1 << 7, // The tile blocks anything from being pushed inside
    GariTileSlip = 1 << 8, // The tile is slippery, like ice, etc.    
    GariTilePush = 1 << 9, // The tile will "push" the player, 
                           // like a conveyor belt, etc.
    // 1<<10, 1<<11 are reserved
    GariTileNorth= 1 <<12, // The direction of this tile's effect.
    GariTileEast = 1 <<13, // The direction of this tile's effect.
    GariTileSouth= 1 <<14, // The direction of this tile's effect.
    GariTileWest = 1 <<15, // The direction of this tile's effect.
};

/** Tile kinds that also help determine properties. */
enum GariTileKinds_ {
  GariTileFloor ,
  GariTileWall  ,
  GariTileWater ,
};

/** A tileset is a set of tiles that are used by a tile map. */
struct GariTileset_; 
typedef struct GariTileset_ GariTileset;

/** 
* A layer is a single layer in a 2D game tile map.
* A layer can consists of a background and/or tiles.
*/
struct GariLayer_; 
typedef struct GariLayer_ GariLayer;

/**
* A map is the visual representation of the game's playfield, which 
* consists of several layers and may contain several sprites.  
*/
struct GariMap_; 
typedef struct GariMap_ GariMap;


/** Joystick handling. */
struct GariJoystick_ ;
typedef struct GariJoystick_ GariJoystick; 

/** Returns the amount of joysticks connected to this system. */
int gari_joy_amount();

/** Returns the name of the inde'th joystick or NULL if no such joystick. */
const char * gari_joy_nameindex(int index); 

/** Returns the name of a joystick object */
const char * gari_joy_name(GariJoystick * joy); 

/** Returns the amount of axes of a joystick object. */
int gari_joy_axes(GariJoystick * joy); 

/** Returns the amount of buttons of a joystick object. */
int gari_joy_buttons(GariJoystick * joy);

/** Returns the amount of buttons of a joystick object. */
int gari_joy_balls(GariJoystick * joy); 

/** Describes a joystick to stderr. */
void gari_joy_describe(GariJoystick * joy); 

/** Opens a joystick. Memory is allocated, so call gari_joy_free when done. */
GariJoystick * gari_joy_open(int index); 

/** Closes the joystick and frees associated memory. */
void gari_joy_free(GariJoystick * stick); 


/** Ruby scripting. */
#include <ruby.h> // needed for certain macros in main.

struct GariRuby_;

typedef struct GariRuby_ GariRuby; 

// call this before main
#define GARI_RUBY_BEFORE_MAIN RUBY_GLOBAL_SETUP

// call this in main
#define GARI_RUBY_IN_MAIN RUBY_INIT_STACK

/* Since ruby 1.9.1 is sort of diffficult to embed, 
call this to initialize ruby at the start of the main of your program, 
passing argc and argv.  
*/

#define GARI_RUBY_INIT(ARGC, ARGV)		\
	VALUE gari_stack_variable;	 	\
	ruby_sysinit(&ARGC, &ARGV);		\
	ruby_init_stack(&gari_stack_variable);
    	
//	ruby_init();				
// return ruby_run_node(ruby_options(argc, argv));
    
    
/** Shows the last error after gari_ruby_eval */
GariRuby * gari_ruby_showerror(GariRuby * ruby);

/** Evaluates str as a ruby program. */
GariRuby * gari_ruby_eval(GariRuby * ruby, char * str); 

/** Executes the file in the filename as a ruby program. */
int gari_ruby_exec(GariRuby * ruby, char * filename);

/** Returns a new Ruby interpreter ready to use! :) */
GariRuby * gari_ruby_new();

/** Frees up the mem'ries of old. ruby interpreters that is. :) */
void gari_ruby_free(GariRuby * ruby);





#endif
