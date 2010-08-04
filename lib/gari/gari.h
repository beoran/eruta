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
* 5) Complete. All you need to write games, but nothing more.
* 6) Release under Zlib license (or similar permissive).
* 7) Easy to embed in Ruby with bindings provided (later).
*
* A C compiler which supports some C99 fratures, particularly
* stdint.h, and one line comments is needed to compile Gari.
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
#define GARI_ALPHA_TRANSLUCENT 0


struct GariSound_; 
typedef struct GariSound_ GariSound;

struct GariMusic_; 
typedef struct GariMusic_ GariMusic;

/** Colors. Colors are models as simple uint32_t types.    */
typedef uint32_t GariColor;

/** RBGA values that model colors in an RGBA color space.  */
struct GariRGBA_;
typedef struct GariRGBA_ GariRGBA;

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


/** Opens the game main game screen or window. Must be done before using any image functions. */
GariScreen * gari_screen_make(GariGame * game, int wide, int high, int fullscreen);

/** Retuns the current main game screen. */
GariScreen * gari_game_screen(GariGame * game);

/** Returns the drawable image for the screen. */
GariImage * gari_screen_image(GariScreen * screen);

/** Opens the game main game screen or window. 
   Must be done before using any image functions. */   
GariScreen * screen_init_depth(GariGame * game, int wide, int high, int fullscreen, int depth);   

/** Image mode, this deterines how to optimize the image for drawing to the screen, and whether the image has any transparency. */
enum GariImageMode_ { 
  GariImageSolid      = 0,
  GariImageColorkey   = 1,
  GariImageAlpha      = 2
};

/** Makes an empty gari image with given same bits per pixels (in depth), 
    but supporting either alpha, or not, or a colorkey depending on mode.
    The video mode must have been set and the screen must have been opened.
    You must call gari_image_free when you're done with the generated 
    GariImage. May return NULL on error.
*/


/** Makes an empty gari image with given same bits per pixels (in depth), 
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
GariImage * gari_image_make(int w, int h, int mode, GariColor colorkey);

/** Disposes of an image. */
void gari_image_free(GariImage * image);

/** Image loading functions. */

/** Loads the image from the named file. */
GariImage * gari_image_loadraw(char * filename);

/** Loads the image from the named file and tries to optimize it for display. */
GariImage * gari_image_load(GariGame * game, char * filename);



/** Optimizes the image for drawing to the screen. */
GariImage * gari_image_optimize(GariImage * image, int mode, GariColor colorkey);

/** Returns the width of the image. */
int gari_image_w(GariImage * img);
 
/** Returns the height of the image. */
int gari_image_h(GariImage * img);

/** Locks image before drawing. Needed before any _nolock drawing function. */
void gari_image_lock(GariImage * image); 
/** Unlocks image after drawing. Needed after any _nolock drawing function. */
void gari_image_unlock(GariImage * image); 

/** Converts a color espressed in rgb components to a GariColor. */
GariColor gari_image_rgb(GariImage *img, uint8_t r, uint8_t g, uint8_t b);

/** Converts a color espressed in rgba components to a GariColor. */
GariColor gari_image_rgba(GariImage *img, 
          uint8_t r, uint8_t g, uint8_t b, uint8_t a);


/* Drawing functions */

/* Drawing context, used for abstracting certain drawing functions.*/
struct GariDraw_ {    
  GariImage        *  image; // image we're drawing to. 
  GariDrawFunction *  draw;  // Drawing function to use.
  GariColor           color; // Color to draw with.
  GariAlpha           alpha; // alpha value to be used n drawing
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

/** Fills the image wth the given color. */
void gari_image_fill(GariImage * image,  GariColor color);

/**/
void gari_image_putpixel(GariImage * image, int x, int y, GariColor color);

void gari_draw_doline(GariDraw * draw, int x1, int y1, int w, int h);  
void gari_image_line(GariImage * image, int x1, int y1, 
                    int w, int h, GariColor color);

void gari_image_docircle(GariDraw * data, int x, int y, int d);  
void gari_image_circle(GariImage * image, int x, int y, 
                      int w, int h, GariColor color);

void gari_image_dodisk(GariDraw * data);  
void gari_image_disk(GariImage * image, int x, int y, 
                      int w, int h, GariColor color);

void gari_image_dobox(GariDraw * data); 
void gari_image_box(GariImage * image, int x, int y, 
                      int w, int h, GariColor color);
                      
     
void gari_image_doslab(GariDraw * data);

                      
/** Draws a slab, which is a filled rectange, on the image. */                  
void gari_image_slab(GariImage * image, int x, int y, 
                      int w, int h, GariColor color);

/** Draws a blended slab, which is a filled rectange, on the image. */
void gari_image_blendslab( GariImage * image, int x, int y, int w, int h,  
                      GariColor color, GariAlpha alpha);


/* Text drawing functions. Only support UTF-8. */
int gari_image_text(GariImage * dst, int x, int y,  
                    char * utf8, GariColor color);

/* Fast blit from image to image. */
void gari_image_blit(GariImage * dst, int dstx, int dsty, GariImage * src);

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
#define GARI_EVENT_EXPOSE         12
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
GariFont * gati_font_mode(GariFont * font, int mode);

/** Frees the memory allocated by the font. */
void gari_font_free(GariFont *font); 

/** Draws font with given colors. */
void gari_font_drawrgba(GariImage * image, int x, int y, char * utf8, GariFont * font, GariRGBA fg, GariRGBA bg); 

/** Draws font with given colors. */
void gari_font_draw(GariImage * image, int x, int y, char * utf8, GariFont * font, uint8_t fg_r, uint8_t fg_g, uint8_t fg_b, uint8_t bg_r, uint8_t bg_b, uint8_t bg_a);

/* Higher level functions and structs. */

/** A camera models a 2D point of view over a tile map, etc. */
struct GariCamera_; 
typedef struct GariCamera_ GariCamera;

/** A sprite is a fully aniated objectn that can be animated in different ways * depending on it's facing direction and actions.
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
struct GariAnimation_;
typedef struct GariAnimation_ GariAnimation;




/** 
 * A tile is a set of images that gets animated according to a 
 * fixed pattern, and displayed as a single part of a layer in a map. 
 */
struct GariTile_; 
typedef struct GariTile_ GariTile;

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












#endif
