#ifndef _GARI_H_
#define _GARI_H_

#include <SDL.h>
#include <stdlib.h>
#include <stdint.h>

/* A few macros for memory allocation. Could allow libgc later. */
#ifndef gari_malloc
#define gari_malloc malloc
#endif 

#ifndef gari_free
#define gari_free free
#endif 

#define gari_allocate(TYPENAME) gari_malloc(sizeof(TYPENAME))


/** Default color depth to open the creen with. Normally 32.  */
#ifndef GARI_DEFAULT_DEPTH 
#define GARI_DEFAULT_DEPTH 32
#endif

/** Of course we need fake booleans :p. */
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

/* Colors. Colors are models as simple uint32_t types.    */
typedef uint32_t GariColor;

/* RBGA values that model colors in an RGBA color space.  */
struct GariRGBA_;
typedef struct GariRGBA_ GariRGBA;

/* Alpha Levels. Alpha Levels are models as simple uint8_t types. */
typedef uint8_t GariAlpha;

/* struct GariColor_; 
typedef struct GariColor_ GariColor; */

struct GariImage_; 
typedef struct GariImage_ GariImage;

struct GariScreen_; 
typedef struct GariScreen_ GariScreen;

/** Game info. */
struct GariGame_; 
typedef struct GariGame_ GariGame;

/* Drawing information */
struct GariDraw_; 
typedef struct GariDraw_ GariDraw;

/* Drawing callback function. */
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


/** Opens the game main game screen or window. Must be done before using any image functions. */
GariScreen * gari_screen_make(GariGame * game, int wide, int high, int fullscreen);

/** Retuns the current main game screen. */
GariScreen * gari_game_screen(GariGame * game);

/** Returns the drawable image for the screen. */
GariImage * gari_screen_image(GariScreen * screen);

/* Opens the game main game screen or window. 
   Must be done before using any image functions. */   
GariScreen * screen_init_depth(GariGame * game, int wide, int high, int fullscreen, int depth);   

/* Disposes of an image. */
int gari_image_free(GariImage * image);

/* Image loading functions. */
GariImage * gari_image_loadraw(char * filename);

/** Game is neeeded so the screen can be seen to optimize 
the image automatically on loading. */
GariImage * gari_image_load(GariGame * game, char * filename);

/** How to optimize the image for drawing to the screen. */
enum  { 
  GariImageOptimizeSolid      = 0,
  GariImageOptimizeColorkey   = 1,
  GariImageOptimizeAlpha      = 2
};

/** Optimizes the image for drawing to the screen. */
int gari_image_optimize(GariImage * image, int mode, GariColor colorkey);

/** Returns the width of the image. */
int gari_image_w(GariImage * img);
 
/** Returns the height of the image. */
int gari_image_h(GariImage * img);

/** Locks image before drawing. Needed before any _nolock drawing function. */
void gari_image_lock(GariImage * image); 
/** Unlocks image after drawing. Needed after any _nolock drawing function. */
void gari_image_unlock(GariImage * image); 



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
void gari_image_slab(GariImage * image, int x, int y, 
                      int w, int h, GariColor color);


/* Text drawing functions. Only support UTF-8. */
int gari_image_text(GariImage * dst, int x, int y,  
                    char * utf8, GariColor color);

/* Fast blit from image to image. */
int gari_image_blit(GariImage * dst, int dstx, int dsty, GariImage * src);

/* Copy part from image to image. */
int gari_image_copy(GariImage * dst, int dstx, int dsty, GariImage * src,
int srcx, int srcy, int w, int h);


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
typedef struct GariAnimation;




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
