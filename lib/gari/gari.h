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




struct GariSound_; 
typedef struct GariSound_ GariSound;

struct GariMusic_; 
typedef struct GariMusic_ GariMusic;

/* Colors. Colors are models as simple uint32_t types. */
typedef uint32_t GariColor;

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
typedef int GariDrawFunction(GariImage * image, GariDraw * data); 

/* Drawing information */
struct GariDraw_ {
  int                 x, y;
  GariDrawFunction *  draw;
  GariColor           color;
  GariAlpha           alpha;
};

/* Constructs drawing information */
GariDraw * gari_drawdata_init(
                              GariDraw * data, 
                              GariDrawFunction * func, 
                              GariColor color,
                              GariAlpha alpha
                             ); 

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

/* Game is neeeded so the screen can be seen to optimize and load the image. */
GariImage * gari_image_load(GariGame * game, char * filename);

/* Optimizes the image for drawing to the screen. */
enum  { 
  GariImageOptimizeSolid      = 0,
  GariImageOptimizeColorkey   = 1,
  GariImageOptimizeAlpha      = 2
};

int gari_image_optimize(GariImage * image, int mode, GariColor colorkey);


/* Drawing functions */

/** Fills the image wth the given color. */
void gari_image_fill(GariImage * image,  GariColor color);

/**/
void gari_image_putpixel(GariImage * image, int x, int y, GariColor color);

int gari_image_doline(GariImage * image, GariDraw * data);  
int gari_image_line(GariImage * image, int x, int y, 
                    int w, int h, GariColor color);

int gari_image_docircle(GariImage * image, GariDraw * data);  
int gari_image_circle(GariImage * image, int x, int y, 
                      int w, int h, GariColor color);

int gari_image_dodisk(GariImage * image, GariDraw * data);  
int gari_image_disk(GariImage * image, int x, int y, 
                      int w, int h, GariColor color);

int gari_image_dobox(GariImage * image, GariDraw * data); 
int gari_image_box(GariImage * image, int x, int y, 
                      int w, int h, GariColor color);

int gari_image_doslab(GariImage * image, GariDraw * data);
int gari_image_slab(GariImage * image, int x, int y, 
                      int w, int h, GariColor color);


/* Text drawing functions. Only support UTF-8. */
int gari_image_text(GariImage * dst, int x, int y,  
                    char * utf8, GariColor color);

/* Fast blit from image to image. */
int gari_image_blit(GariImage * dst, int dstx, int dsty, GariImage * src);

/* Copy part from image to image. */
int gari_image_copy(GariImage * dst, int dstx, int dsty, GariImage * src,
int srcx, int srcy, int w, int h);














#endif
