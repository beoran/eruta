/**
* @mainpage Gy Game Library
*
* Gy is a simple, fast, cross platform game library targeted at 2D games,
* written in plain C. For now, it uses SDL as a backend, but that might 
* change later to use more low-level back-ends. It's goals are:
*
* 1) Targeted to make writing 2D games as easy as possible.
* 2) A simple API, that hides the details as much as possible.
* 3) Fast, giving good performance everywhere possible.
* 4) Cross platform. Enough said.
* 5) Complete yet concise. All you need to write 2D games, but nothing more.
* 6) Release under Zlib license (or similar permissive).
* 7) Easy to embed in Ruby with bindings provided.
*
* A C compiler which supports some C99 fratures, particularly
* stdint.h, __VA_ARGS__, and one line comments is needed to compile Gy.
*
* More details are documented in gari.h
* 
* @file gari.h
*
*/

#ifndef _GY_H_
#define _GY_H_

#include <stdlib.h>
#include <stdint.h>

/* 
  Some platforms will need a __declspec(dllexport) or something for 
  functions in a shared library. Not used yet since I didn't get a chance to try it there yet.
*/
#ifndef GY_EXPORT_FUNC
#define GY_EXPORT_FUNC extern
#endif
  
/*
* Some platforms may require a change in calling convention
*/  
#ifndef GY_CALL_FUNC
#define GY_CALL_FUNC
#endif 

/* All in a handy wrapper macro */
#define GY_FUNC(RESULT) GY_EXPORT_FUNC RESULT GY_CALL_FUNC
 
/* An another wrapper macro to help typedefing such functions */
#define GY_FUNCTYPE(RESULT) typedef RESULT GY_CALL_FUNC


/** Default color depth to open the creen with. Normally 32.  */
#ifndef GY_DEFAULT_DEPTH 
#define GY_DEFAULT_DEPTH 32
#endif

/** Of course we need fake booleans if we don' talready have them :p. */
#ifndef FALSE
#define FALSE (0)
#endif

#ifndef TRUE
#define TRUE (!(FALSE))
#endif

/** Common alpha values */
#define GY_ALPHA_OPAQUE      255
#define GY_ALPHA_SOLID       255
#define GY_ALPHA_HALF        128
#define GY_ALPHA_CLEAR       0
#define GY_ALPHA_TRANSLUCENT 0


/** Dye. A Dye is a device and image dependent representation of a color 
    that fits inside a uint32_t types.
    Dyes are used in lower level API's. 
*/
typedef uint32_t GyDye;

/** GyColor models colors as RBGA values in an sRGBA color space.
* GyColor should be used as if it is immutable.
* Colors are used in stead of Dyes in higher level API's.  
*/
struct GyColor_ {
  uint8_t r, g, b, a;
};

typedef struct GyColor_ GyColor;

/** A few default colors. */
extern const GyColor GyWhite;
extern const GyColor GyBlack;


/** Frees the memory associated with a GyColor allocated though
*   gycolor_rgba or gycolor_rgb.  Returns NULL (guaranteed).
*/
GyColor * gycolor_free(GyColor * color); 

/** Allocates a new color and initializes it with the given r, g, b and a values
*/
GyColor * gycolor_rgba(uint8_t r, uint8_t g, uint8_t b, uint8_t a);

/** Allocates a new color and initializes it with the given r, g, b values
*   a will be set to GY_ALPHA_OPAQUE.
*/ 
GyColor * gycolor_rgb(uint8_t r, uint8_t g, uint8_t b);  


/** Returns a GyColor struct initialized with the given 
* r, g, b, a components.
*/
GyColor gycolora(uint8_t r, uint8_t g, uint8_t b, uint8_t a);


/** Returns a GyColor struct initialized with the given r, g, b components. 
* The a component will be set to GY_ALPHA_SOLID.
*/
GyColor gycolor(uint8_t r, uint8_t g, uint8_t b);

/** Gets the r component of a GyColor. */
uint8_t gycolor_r(GyColor * rgba);

/** Gets the g component of a GyColor. */
uint8_t gycolor_g(GyColor * rgba);

/** Gets the b component of a GyColor. */
uint8_t gycolor_b(GyColor * rgba);

/** Gets the a component of a GyColor. */
uint8_t gycolor_a(GyColor * rgba);


/** Alpha Levels. Alpha Levels are models as simple uint8_t types. */
typedef uint8_t GyAlpha;

/** Images. Images arevisuals that can be dwawn to. */
struct GyImage_; 
typedef struct GyImage_ GyImage;

/** A screen or window. Not directly drawable. */
struct GyScreen_; 
typedef struct GyScreen_ GyScreen;

/** Game info. */
struct GyGame_; 
typedef struct GyGame_ GyGame;

/** Drawing information */
struct GyDraw_; 
typedef struct GyDraw_ GyDraw;




/** Drawing callback function. */
typedef int GyDrawFunction(GyDraw * data, int px, int py); 



/** 
* Initializes Gy Game and returns it. Must be done before anything else. 
*/
GyGame * gygame_make();

/** Finishes and cleans up a gari game when we're done with it. */
void gygame_free(GyGame * game);

/** Updates the game screen, FPS, etc. */
GyGame * gygame_update(GyGame * game); 

/** Sets the frames of the game back to 0.*/
GyGame * gygame_resetframes(GyGame * game); 

/** Gets the total amount of frames displayed since the start of the game, 
   or since gygame_resetframes was called. */
uint32_t gygame_frames(GyGame * game);

/** Advances the game's frame counter. */
GyGame * gygame_nextframe(GyGame * game);

/** Starts FPS counter. gygame_resetframes also calls this. */
GyGame * gygame_startfps(GyGame * game);

/** Returns calculated fps after calling startfps . */
double gygame_fps(GyGame * game);

/** Reports on the game's FPS on stderr.*/
void gygame_report(GyGame * game);


/** Opens the game main game screen or window. Must be done before using any image functions, or before calling game.update . */
GyScreen * gygame_openscreendepth(GyGame * game, int wide, int high, int fullscreen, int depth); 

/** Opens the game main game screen or window. 
   Must be done before using any image functions. */   
GyScreen * gygame_openscreen(GyGame * game, int wide, int high, 
                                  int fullscreen);

// Checks if a screen is the fullscreen screen or not
int gyscreen_fullscreen(GyScreen * screen);

/** Retuns the current main game screen. */
GyScreen * gygame_screen(GyGame * game);

/** Returns the drawable image for the screen. */
GyImage * gyscreen_image(GyScreen * screen);

/** Checks if the main game screen is in fullscreen mode. */ 
int gygame_fullscreen(GyGame * game);

/** Can be used to set or unset fullscreen after opening the screen. */
GyScreen * gygame_fullscreen_(GyGame * game, int fullscreen); 

/** Sets the keyboard repeat delay and interval in ms, 
*   or disable with delay 0. */
GyGame * gygame_keyrepeat(GyGame * game, int delay, int interval);

/** Sets the system mouse cursor to invisible or visible. */
int gyscreen_showcursor_(GyScreen * screen, int show);
/** Gets the visibility of the system mouse cursor. */
int gyscreen_showcursor(GyScreen * screen);


/** Image mode, this deterines how to optimize the image for drawing to the screen, and whether the image has any transparency. */
enum GyImageMode_ { 
  GyImageSolid      = 0,
  GyImageColorkey   = 1,
  GyImageAlpha      = 2
};


/** Makes an empty gari image with given bits per pixels (in depth), 
    but supporting either alpha, or not, depending on mode.
    The video mode must have been set and the screen must have been opened.
    You must call gyimage_free when you're done with the generated 
    GyImage. May return NULL on error.
*/
GyImage * gyimage_makedepth(int w, int h, int depth, int mode);


/** Makes an empty gari image with the same bits per pixels as the screen, 
    but supporting either alpha, or not, or a colorkey depending on mode.
    The video mode must have been set and the screen must have been opened.
    You must call gyimage_free when you're done with the generated 
    GyImage. May return NULL on error.
*/
GyImage * gyimage_make(int w, int h, int mode, GyDye dyekey);

/** Disposes of an image. */
void gyimage_free(GyImage * image);

/** Image loading functions. */

/** Loads the image from the named file. */
GyImage * gyimage_loadraw(char * filename);


/** Loads the image from the named file, and optimizes it for display
as a solid image on the active video surface, which must already have been 
opened. If the acceleration fails, this returns the unaccellerated image. 
Return NULL if the file could not be read. */
GyImage * gyimage_loadsolid(char * filename);

/** Loads the image from the named file and tries to optimize it for display on the game's screen. This requires the game's screen to have been opened first. */
GyImage * gyimage_load(GyGame * game, char * filename);


/*/ Saves a GyImage as a to a file in filename in BMP format. Return NULL if 
saving failed, img on success. */
GyImage * gyimage_savebmp(GyImage * img, const char * filename);

/** Saves a GyImage as a to a file in filename in PNG format. Return NULL if 
saving failed, img on success. */
GyImage * gyimage_savepng(GyImage * img, const char * filename);

/** Optimizes the image for drawing to the screen. */
GyImage * gyimage_optimize(GyImage * image, int mode, GyDye dyekey);

/** Copies a part of the source image and returns a new destination image,
or nil it it ran out of memory or otherwise was incorrect. */
GyImage * gyimage_copypart(GyImage *src, int x, int y, int w, int h);

/** Returns true if any pixel of the image has any per-pixel transparency, 
lower than amin 
false if not. */
int gyimage_hasalpha_p(GyImage * img, unsigned char amin);

/** Returns the width of the image. */
int gyimage_w(GyImage * img);
 
/** Returns the height of the image. */
int gyimage_h(GyImage * img);

/** Gets the color depth in bits per pixels of this image. Returns -1 on error.
*/
int gyimage_depth(GyImage * img);

/** Locks image before drawing. Needed before any _nolock drawing function. */
void gyimage_lock(GyImage * image); 
/** Unlocks image after drawing. Needed after any _nolock drawing function. */
void gyimage_unlock(GyImage * image); 

/** Converts a color expressed in rgb components to a GyDye. */
GyDye gyimage_rgb(GyImage *img, uint8_t r, uint8_t g, uint8_t b);

/** Converts a color expressed in rgba components to a GyDye. */
GyDye gyimage_rgba(GyImage *img, 
          uint8_t r, uint8_t g, uint8_t b, uint8_t a);


          
/** 
  Concstructs a color expressed as an RGBA quadruplet and returns it as a 
  struct.
*/
GyColor gycolora(uint8_t r, uint8_t g, uint8_t b, uint8_t a); 

GyColor gycolor(uint8_t r, uint8_t g, uint8_t b);

/** 
* Converts a GyColor to a gari dye for the given image. 
* If the color's A is solid, then it maps as a solid color. 
* otherwise, it maps as a transparent color.  
*/
GyDye gycolor_dye(GyColor color, GyImage * image); 

/** Checks if two gariColors are equal. Colors will sort by ascending a,
r, g and b */
int gycolor_cmp(GyColor c1, GyColor c2);

/** Converts a GyDye to a GyColor for the given image. */
GyColor gydye_color(GyDye dye, GyImage * image);

/** Optimizes a color for use with the given image. For 24 or 32 bpp images 
this does nothing. However, other image depths, such as 16, 15 or 8 bits don't have enough resolution to display 3 or 4 rgb(a) components correctly, so the 
colors used are reduced. All in all, this fuction is a round-trip between 
gycolor_dye and gydye_color.  
*/
GyColor gycolor_optimize(GyColor color, GyImage * image);
         

/** Drawing functions. */
typedef void GyPutPixelFunc(GyImage * img, int x, int y, GyDye dye);
typedef GyDye GyGetPixelFunc(GyImage * img, int x, int y);
typedef void GyBlendPixelFunc(GyImage * img, int x, int y, GyDye dye, GyAlpha alpha);

/** Returns a function that can be used to put a pixel on this GyImage.
* The function returned does no locking. May return NULL if the format does 
* not use 1,2,3 or 4 bytes per pixel.    
*/
GyPutPixelFunc * gyimage_putpixelfunc_nl(GyImage * image);

/** Returns a function that can be used to get a pixel from this GyImage.
* The function returned does no locking. May return NULL if the format does 
* not use 1,2,3 or 4 bytes per pixel.
*/    
GyGetPixelFunc * gyimage_getpixelfunc_nl(GyImage * image); 

/** Returns a function that can be used to blend a pixel from this GyImage.
* The function returned does no locking. May return NULL if the format does 
* not use 1,2,3 or 4 bytes per pixel.
*/    
GyBlendPixelFunc * gyimage_blendpixelfunc_nl(GyImage * image); 


/* Drawing context, used for abstracting certain drawing functions.*/
struct GyDraw_ {
  GyImage        *  image; // image we're drawing to. 
  GyDrawFunction *  draw;  // Generic drawing function to use.
  GyGetPixelFunc *  getpixel; // Get pixel function (for the image) 
  GyPutPixelFunc *  putpixel; // Put pixel function (for the image)
  GyBlendPixelFunc *blendpixel; // Blend pixel function (for the image)
  GyColor           color; // Color to draw with.
   GyDye             dye;   // Dye to draw with.
  GyAlpha           alpha; // alpha value to be used when drawing
  void *              other; // other user-defined data.
};

/* Constructs drawing information */
GyDraw * gydrawdata_init(
                              GyDraw          * data,
                              GyImage         * image,
                              GyDrawFunction  * func,
                              GyColor color,
                              GyAlpha alpha
                             ); 
                             
/** Sets the clipping rectangle of the image. 
* Clipping applies to all drawing functions. 
*/
GyImage * gyimage_setclip(GyImage * img, 
              int x, int y, int w, int h); 
  
/** Gets the clipping rectangle of the image. 
* Clipping applies to all drawing functions. 
*/
GyImage * gyimage_getclip(GyImage * img, 
              int *x, int *y, int *w, int *h);

/** Quickly fills the image or screen with the given dye */
void gyimage_filldye(GyImage * image,  GyDye dye);
 
/** Quickly draws a rectangle with the given dye, without blending. */
void gyimage_fillrectdye(GyImage * image, int x, int y, 
                          int w, int h, GyDye dye);
                          
/** Quickly fills the image with the given color. */
void gyimage_fill(GyImage * image,  GyColor color);

/** Quickly draws a rectangle with the given dye, without blending. */
void gyimage_fillrect(GyImage * image, int x, int y, 
                          int w, int h, GyColor color); 
                          

/** Low level putpixel function.         */
void gyimage_putpixel(GyImage * image, int x, int y, GyDye dye);

/** High level level putpixel function.  */
void gyimage_dot(GyImage * image, int x, int y, GyColor color);

/** High level blend pixel function */
void gyimage_blenddot(GyImage * image, int x, int y, GyColor color);

/** High level getpixel function */
GyColor gyimage_getdot(GyImage * image, int x, int y);

/** Draws an arbitrary line on the image starting at x1, y1, and fitting inside
the rectangle with the size w and h.     */
void gyimage_line(GyImage * image, int x1, int y1, 
                     int w, int h, GyColor color);

/** Optimized case of gyimage_line, draws horizontal lines with 
    a width of w only. */
void gyimage_hline(GyImage * image, int x, int y, int w, GyColor color);

/** Optimized case of gyimage_line, draws vertical lines with 
   a height h only. */
void gyimage_vline(GyImage * image, int x, int y, int h, GyColor color); 

/** Draws a hoop, that is, an empty circle, on the image. */
void gyimage_hoop(GyImage * image, int x, int y, int r, GyColor color);

/** Draws a disk, or a filled circle, on the image. */
void gyimage_disk(GyImage * image, int x, int y, int r, GyColor color);

/** Draws a box, or an open rectangle on the image, starting at (x, y) */
void gyimage_box(GyImage * image, int x, int y, 
                                       int w, int h, GyColor color);

/** Draws a slab, which is a filled rectange, on the image. */
void gyimage_slab(GyImage * image, int x, int y, 
                      int w, int h, GyColor color);

/** Blends an arbitrary line on the image starting at x1, y1, and fitting inside
the rectangle with the size w and h.     */
void gyimage_blendline(GyImage * image, int x1, int y1, 
                     int w, int h, GyColor color);
                     
/** Blends a box, which is an open rectange, on the image. */
void gyimage_blendbox( GyImage * image, int x, int y, int w, int h,  
                      GyColor color);                     

/** Draws a blended slab, which is a filled rectange, on the image. */
void gyimage_blendslab( GyImage * image, int x, int y, int w, int h,  
                      GyColor color);

/** Draws a blended disk, which is a filled circle, on the image. */       
void gyimage_blenddisk(GyImage * image, int x, int y, int r, GyColor color);

/** Draws a blended hoop, which is a circle outline, on the image. */       
void gyimage_blendhoop(GyImage * image, int x, int y, int r, GyColor color);

/** Draws a blended floodfill. */
void gyimage_blendflood(GyImage * image, int x, int y, GyColor color);


/* Text drawing functions. Only support UTF-8. */
int gyimage_text(GyImage * dst, int x, int y,  
                    char * utf8, GyColor color);

/* Fast blit from image to image. */
void gyimage_blit(GyImage * dst, int dstx, int dsty, GyImage * src);

/** Blits a part of one image to another one with the given coordinates. Takes the indicated parts of the image, as specified by srcx, srcy, srcw and srch  */
void gyimage_blitpart(GyImage * dst, int dstx, int dsty, GyImage * src,
int srcx, int srcy, int srcw, int srch);

/* Copy part from image to image. */
int gyimage_copy(GyImage * dst, int dstx, int dsty, GyImage * src,
int srcx, int srcy, int w, int h);

/* Copy pixel from image to image. */
void gyimage_copypixel(GyImage * dst, int dstx, int dsty, 
                          GyImage * src, int srcx, int srcy);

/** Blits (a part of) an image to another one, 
    scaling it arbitrarily to the wanted size. Uses no 
    interpolation nor smoothing. 
*/
void gyimage_scaleblit(GyImage * dst, int dstx, int dsty, 
                                      int dsth, int dstw, 
                                      GyImage * src,
                                      int srcx, int srcy, 
                                      int srch, int srcw);

/** Flood fills (a part of) the image with color starting from x and y. */
void gyimage_flood(GyImage * image, int x, int y, GyColor color);


/* Input event handling. */


#define GY_EVENT_NONE           0
#define GY_EVENT_ACTIVE         1
#define GY_EVENT_KEYDOWN        2
#define GY_EVENT_KEYUP          3
#define GY_EVENT_MOUSEPRESS     4
#define GY_EVENT_MOUSERELEASE   5
#define GY_EVENT_MOUSEMOVE      6
#define GY_EVENT_MOUSESCROLL    7
#define GY_EVENT_JOYMOVE        8
#define GY_EVENT_JOYPRESS       9
#define GY_EVENT_JOYRELEASE    10
#define GY_EVENT_JOYHAT        11
#define GY_EVENT_JOYBALL       12
#define GY_EVENT_RESIZE        13
#define GY_EVENT_EXPOSE        14
#define GY_EVENT_QUIT          15
#define GY_EVENT_USER          16
#define GY_EVENT_SYSTEM        17
#define GY_EVENT_LAST          18
#define GY_JOYHAT_CENTER       0x00 
#define GY_JOYHAT_UP           0x01
#define GI_JOYHAT_RIGHT        0x02
#define GI_JOYHAT_DOWN         0x04
#define GI_JOYHAT_LEFT         0x08


/** The event structure. It's not a union for ease of wrapping in other 
languages, though only some of the fields may contain useful data depending 
on the event.kind field. */
struct GyEvent_ {
  uint8_t   kind;
  uint8_t   gain;
  uint8_t   which;
  uint8_t   sub;
  uint16_t  key;
  uint16_t  mod;
  uint16_t  unicode;
  uint16_t  x, y, xrel, yrel, w, h;
  uint16_t  button;
  int16_t   value;
};

struct GyEvent_;
typedef struct GyEvent_ GyEvent;


/** Gets an event from the event queue. Returns a special event with 
event.kind == GY_EVENT_NONE if no events are on the queue. 
*/
GyEvent gyevent_poll();

/** Polls the event queue and copies it to the given event  addres, 
* wich must be a valid address and not be null.
* Return NULL if the eevnt queue was ampty, non-null if there was an event fetched. 
*/
GyEvent * gyevent_fetch(GyEvent * event);

/** Polls the event queue and returns a newly alloctaded GyEvent with 
* the top event of the queue. Returns NULL, and allocates no memory if 
* the queue is empty.
*/
GyEvent * gyevent_pollnew();

/** Frees memory associated with a Gyevent alloctated by gyevent_pollnew.*/
void gyevent_free(GyEvent * event);

/* Fonts and text rendering. */

/** BDF font loading: */
typedef struct GyBdfGlyph_ GyBdfGlyph; 
typedef struct GyBdf_ GyBdf;

/** Loads a bdf font. */
GyBdf * gybdf_load(char * filename); 

/** Frees the memory allocated by the font. */
GyBdf * gybdf_free(GyBdf * font);

/** Draws a single utf8 character to a surface at the given coordinates 
using the font to a surface, depending on the font's settings. */
void gybdf_putc(GyImage * image, int x, int y, int utf8, GyBdf * font,  
              GyColor fg, GyColor bg);

struct GyFont_;
/** Opaque struct for TTF font handling. */
typedef struct GyFont_ GyFont;

/** Ways to render the font.  Default is GyFontSolid */
enum GyFontMode_ {
  GyFontSolid,
  GyFontShaded,
  GyFontBlended,
};

typedef enum GyFontMode_ GyFontMode;

/** Font styles and effects like, bold, italic, underline. */
enum GyFontStyle_ {
  GyFontNormal    = 0,
  GyFontItalic    = 1,
  GyFontBold      = 2, 
  GyFontUnderline = 4,
};

typedef enum GyFontStyle_ GyFontStyle;


/** Loads one of the fonts in a font file. */
GyFont * gyfont_loadindex(char * filename, int ptsize, long index);
/** Loads the first font in a font file. */
GyFont * gyfont_load(char * filename, int ptsize);

/** Sets the drawing mode of the font. */
GyFont * gyfont_mode_(GyFont * font, int mode);

/** Gets the drawing mode of the font. */
int gyfont_mode(GyFont * font);

/** Frees the memory allocated by the font. */
void gyfont_free(GyFont *font); 

/** Renders the font to a surface, depending on the font's settings. */
GyImage * gyfont_render(GyFont * font, char * utf8, 
    GyColor fgrgba, GyColor bgrgba);

/** Draws font with given colors. */
void gyfont_drawcolor(GyImage * image, int x, int y, char * utf8, 
                         GyFont  * font , GyColor fg, GyColor bg); 

/** Draws font with given color components. */
void gyfont_draw(GyImage * image, int x, int y, char * utf8, GyFont * font, uint8_t fg_r, uint8_t fg_g, uint8_t fg_b, uint8_t bg_r, uint8_t bg_b, uint8_t bg_a);

/** Draws font in printf style. Won't work on platforms that lack vsnprintf.
* Will draw up to 2000 bytes of characters.  
*/
void gyfont_printf(GyImage * image, int x, int y, GyFont * font, GyColor fg, GyColor bg, char * format, ...);

/** Returns a text with details about the last error in loading or 
handling a font. */
char * gyfont_error();

/** Returns the width that the given UTF-8 encoded text would be if it was rendered using gyfonr_render. */
int gyfont_renderwidth(GyFont * font, char * utf8); 

/** Returns the font's max height */
int gyfont_height(GyFont * font); 

/** Returns the font's font max ascent (y above origin)*/
int gyfont_ascent(GyFont * font); 

/** Returns the font's min descent (y below origin)*/
int gyfont_descent(GyFont * font);

/** Returns the font's recommended line spacing. */
int gyfont_lineskip(GyFont * font);

int gyfontstyle_tottf(int style);

int gyfontstyle_fromttf(int style); 

/** Returns the style of the font.  */
int gyfont_style(GyFont * font);

/** Sets the style of the font. Note that the style may be "faked" 
by the underlaying implementation. Use a suitably pre-modified font for 
better effects. */
void gyfont_style_(GyFont * font, int style); 

/** Loads font from a data buffer in memory */
GyFont * gyfont_dataindex(const unsigned char * data, int datasize, 
                               int ptsize, long index);

/** Loads font from a data buffer in memory */
GyFont * gyfont_data(const unsigned char * data, int datasize, int ptsize);

/** Built in public domain font data and size (tuffy) by Ulrich Tatcher. */
extern const int              data_font_tuffy_ttf_size;
extern const unsigned char *  data_font_tuffy_ttf;

/** Music and sound. */

#define GY_AUDIO_LOWFREQENCY 8000
#define GY_AUDIO_MEDIUMFREQENCY 22050
#define GY_AUDIO_HIGHFREQENCY 44100

struct GyAudioInfo_;
typedef struct GyAudioInfo_ GyAudioInfo;

/** Allocates a new audioinfo. */
GyAudioInfo * gyaudioinfo_new(int freq, int form, int chan); 

/** Frees an allocated audioinfo. */
GyAudioInfo * gyaudioinfo_free(GyAudioInfo * info); 

/** Gets the frequency of an audio info  */
int gyaudioinfo_frequency(GyAudioInfo * info);

/** Gets the amount of channels of an audio info  */
int gyaudioinfo_channels(GyAudioInfo * info); 

/** Gets the format of an audio info  */
int gyaudioinfo_format(GyAudioInfo * info); 

/** Queries the current state of the audio system. 
Returns NULL if it was not initialized yet. */
GyAudioInfo * gyaudio_query();

/** Initializes the audio subsystem for a game. 
Free it with gyaudioinfo_free */
GyGame * gyaudio_init(GyGame * game, int frequency);

/** Cleans up the audio subsystem for a game. */
void gyaudio_done(GyGame * game);

struct GySound_;
typedef struct GySound_ GySound;

/** Initialises and loads sound into an existing GySound record. */
GySound * gysound_init(GySound * sound, char * filename);

/** Creates a new GySound and loads the sound from a file. */
GySound * gysound_load(char * filename);

/** Deallocates the loaded sound, but not the wrapper GySound itself. */
GySound * gysound_done(GySound * sound);

/** Calls gysound_done and then frees the sound itself.*/
GySound * gysound_free(GySound * sound);

/** Plays a sound once. */
GySound * gysound_play(GySound * sound);


struct GyMusic_;
typedef struct GyMusic_ GyMusic;

/** Initialises and loads music into an existing GyMusic record. */
GyMusic * gymusic_init(GyMusic * music, char * filename);

/** Creates a new GyMusic and loads the music from a file. */
GyMusic * gymusic_load(char * filename);

/** Deallocates up the loaded music, but not the wrapper GyMusic itself. */
GyMusic * gymusic_done(GyMusic * music);

/** Calls gymusic_done and then frees the music itself.*/
GyMusic * gymusic_free(GyMusic * music);

/** Starts playing the music loop times (-1 means to keep on repeating) 
*   fading in after fade ms. 
*/
GyMusic * gymusic_fadein(GyMusic * music, int loops, int fade); 

/** Stops playing the music, fading out after fade ms. */
GyMusic * gymusic_fadeout(GyMusic * music, int fade); 


 


/* Higher level functions and structs. */

struct GyStyle_;
typedef struct GyStyle_ GyStyle;

GyStyle * gystyle_free(GyStyle * style); 

GyStyle * gystyle_new(GyColor   fore, 
                           GyColor   back,
                           GyFont  * font,
                           GyImage * image
                          );

GyFont * gystyle_font(GyStyle * style); 

GyColor gystyle_fore(GyStyle * style);
 
GyColor gystyle_back(GyStyle * style);

GyImage * gystyle_image(GyStyle * style); 




/** Camera  A camera models a 2D point of view over a tile map, etc. */
struct GyCamera_;
typedef struct GyCamera_ GyCamera;


/* Known particle engine effects: */
enum GyFlowKind_ {
  GyFlowSnow,
  GyFlowRain,
  GyFlowBlood,
  GyFlowSlash,
  GyFlowExplode,
  GyFlowDamage,
};
typedef enum GyFlowKind_ GyFlowKind;


/** A drop is a particle in the particle engine */
struct GyDrop_;
typedef struct GyDrop_ GyDrop;

/** A well is an initial configuration af a certain amount of GyDrop 
  particles in a GyFlow particle engine. */
struct GyWell_;
typedef struct GyWell_ GyWell;

/** A Flow is an instance of a particle engine. 
*/
struct GyFlow_;
typedef struct GyFlow_ GyFlow;

/** Particle engine init callback function. */
typedef GyDrop * GyDropInitFunction(GyDrop * data, GyWell * well); 

/** Particle engine draw callback function. */
typedef GyDrop * GyDropDrawFunction(GyDrop * data, GyImage * im); 

/** Particle engine update callback function. */
typedef GyDrop * GyDropUpdateFunction(GyDrop * data, int time); 

/** Allocates a new GyFlow particle engine with the given amount of 
*   particles available. Free it after use with gyflow_free(); 
*/
GyFlow * gyflow_make(size_t size);

/**
* Free and destroy a previously allocated GyFlow particle engine.
*/
void gyflow_free(GyFlow * flow); 

/**
* Updates the position of the pixels after time ticks passed.
*/
void gyflow_update(GyFlow * flow, int time); 
 
/** Draws all active particles from tthis flow to the screen. */
void gyflow_draw(GyFlow * flow, GyImage * im); 

/**
* Activates amount Drops (particles) in the GyFlow flow. 
* The particle will be of kind kind, and will be placed, depending on the 
* kind, at x a,d y, and displayed using given color, image or text 
*/
GyFlow * gyflow_activate(GyFlow * flow, 
      int amount, int kind, int x, int y, GyColor color, 
      GyImage * im, char * text);

/** Initializes the random number generator, 
    used mainly in the particle engine. */
void gyrandom_init(); 

/** Gets a random number between min and max. */
long gyrandom(long min, long max);




/** A sprite is a fully aniated object that can be animated in different ways * depending on it's facing direction and actions.
* For example, the sprite may have a walking action in 2 or 4 directions,
* which each have a different animation.
* A sprite may also be consisted of different parts or layers superimposed 
* over each other, for example, a character holding a weapon, etc.  
*/
struct GySprite_;
typedef struct GySprite_ GySprite;

/** An animation is a full cycle of an animation, 
*   of which sprites are made up. 
*/
struct GyAnime_;
typedef struct GyAnime_ GyAnime;

/** Anime can be described as a crude "program", 
  using the following constants: */
enum GyAnimeProgram_ {
  GyAnimeRewind,
  GyAnimeNext,
  GyAnimeWait
};


/** 
 * A tile is a set of images that gets animated according to a 
 * fixed pattern, and displayed as a single part of a layer in a map. 
 */
struct GyTile_; 
typedef struct GyTile_ GyTile;

/** Flags for tiles that deterine their in-game properties. */
enum GyTileFlags_ {   
    GyTileWalk = 1 << 0, // The tile can be walked over.
    GyTileJump = 1 << 1, // The tile can be jumped over.
    GyTileLedge= 1 << 2, // The tile will cause a jump when walked over.
    GyTileSwim = 1 << 3, // The tile is some kind of swimmable liquid.
    GyTilePain = 1 << 4, // Walking over the tile causes pain.
    GyTileUp   = 1 << 5, // The tile is a step-up stair to the next layer.
    GyTileDown = 1 << 6, // The tile is a step-down to the previous layer.    
    GyTileEdge = 1 << 7, // The tile blocks anything from being pushed inside
    GyTileSlip = 1 << 8, // The tile is slippery, like ice, etc.    
    GyTilePush = 1 << 9, // The tile will "push" the player, 
                           // like a conveyor belt, etc.
    // 1<<10, 1<<11 are reserved
    GyTileNorth= 1 <<12, // The direction of this tile's effect.
    GyTileEast = 1 <<13, // The direction of this tile's effect.
    GyTileSouth= 1 <<14, // The direction of this tile's effect.
    GyTileWest = 1 <<15, // The direction of this tile's effect.
};

/** Tile kinds that also help determine properties. */
enum GyTileKinds_ {
  GyTileFloor ,
  GyTileWall  ,
  GyTileWater ,
};

/** A tileset is a set of tiles that are used by a tile map. */
struct GyTileset_; 
typedef struct GyTileset_ GyTileset;

/** 
* A layer is a single layer in a 2D game tile map.
* A layer can consists of a background and/or tiles.
*/
struct GyLayer_; 
typedef struct GyLayer_ GyLayer;

/** 
* A tile map consists of individual tiles. Tiles are simply indirect 
* pointers to images, where the pointer can be changed to animate it. 
* The same idea is used for sprites, and unified in the GySheet struct.
*/
struct GySheet_;
typedef struct GySheet_ GySheet;

/** Sets the active image of the GySheet. */
GySheet * gysheet_image_(GySheet * sheet, GyImage * image);
 
/** Gets the active image of the GySheet. */
GyImage * gysheet_image(GySheet * sheet); 

/** Allocates a new GySheet that has imageas it's active image.  */
GySheet * gysheet_new(GyImage * image); 

/** Frees the memory associated with a previously allocated GySheet. */
GySheet * gysheet_free(GySheet * sheet); 

/** Draws a GySheet to an image. */
void gyimage_blitsheet(GyImage * dst, int x, int y, GySheet * sheet);

/** Initializes a GyLayer. Gridwide and gridhigh are the size of 
the tile map's grid. tilewide and tilehigh are the sizes of the tiles used. */
GyLayer * gylayer_init(GyLayer * layer,  
                           int gridwide, int gridhigh, 
                           int tilewide, int tilehigh); 

/** Allocates a new gari layer with the given parameters. */
GyLayer * gylayer_new(int gridwide, int gridhigh, 
                           int tilewide, int tilehigh);
                            
/** Deallocates memory allocated by gai_layer_new */ 
GyLayer * gylayer_free(GyLayer * layer); 

/** Returns TRUE if the given gridx and gridy are outside the grid
 Returns FALSE if inside the grid.
*/
int gylayer_outsidegrid(GyLayer * layer, int gridx, int gridy); 

/** Sets the tile at the given location to the given GySheet pointer, 
* which may be NULL. Returns the layer, or NULL on error.  
*/
GyLayer * gylayer_set(GyLayer * layer, 
                           int gridx, int gridy, GySheet * tile);

/** Returns the sheet in the layer's grid at the given grid coordinates,
* returns NULL if the fcoordinates are out of bounds or if it was an empty tile.
*/
GySheet * gylayer_get(GyLayer * layer, int gridx, int gridy); 

/** Draws the tile layer, with x and y as the top left corner, 
* to the given image. X and y may be negative. 
*/
void gylayer_draw(GyLayer * layer, GyImage * image, int x, int y); 

/** Returns the width of the layer in grid units. Returns -1 on error. */
int gylayer_gridwide(GyLayer * layer); 

/** Returns the height of the layer in grid units. Returns -1 on error. */
int gylayer_gridhigh(GyLayer * layer);

/** Returns the width of the layer's tiles in pixels. Returns -1 on error. */
int gylayer_tilewide(GyLayer * layer);

/** Returns the height of the layer's tiles in pixels. Returns -1 on error. */
int gylayer_tilehigh(GyLayer * layer);

/** Returns the width of the layer in pixels. Returns -1 on error. */
int gylayer_wide(GyLayer * layer);

/** Returns the height of the layer in pixels. Returns -1 on error. */
int gylayer_high(GyLayer * layer);


/**
* A map is the visual representation of the game's playfield, which 
* consists of several layers and may contain several sprites.  
*/
struct GyMap_; 
typedef struct GyMap_ GyMap;


/** Joystick handling. */
struct GyJoystick_ ;
typedef struct GyJoystick_ GyJoystick; 

/** Returns the amount of joysticks connected to this system. */
int gyjoy_amount();

/** Returns the name of the inde'th joystick or NULL if no such joystick. */
const char * gyjoy_nameindex(int index); 

/** Returns the name of a joystick object. */
const char * gyjoy_name(GyJoystick * joy);
 
/** Returns the index of a joystick object. */
int gyjoy_index(GyJoystick * joy);

/** Returns the amount of axes of a joystick object. */
int gyjoy_axes(GyJoystick * joy); 

/** Returns the amount of buttons of a joystick object. */
int gyjoy_buttons(GyJoystick * joy);

/** Returns the amount of buttons of a joystick object. */
int gyjoy_balls(GyJoystick * joy); 

/** Describes a joystick to stderr. */
void gyjoy_describe(GyJoystick * joy); 

/** Opens a joystick. Memory is allocated, so call gyjoy_free when done. */
GyJoystick * gyjoy_open(int index); 

/** Closes the joystick and frees associated memory. */
void gyjoy_free(GyJoystick * stick); 

/** Returns how many joysticks are available. */
int gygame_joysticks(GyGame * game); 

/** Returns the n-th joystick of the game. */
GyJoystick * gygame_joystick(GyGame * game, int index);


/** Time specific funtions.  */
/** Milliseconds since gygame_make was called */
uint32_t gygame_ticks(GyGame * game);

/** 2d vectors, for speedy vector math.  */

typedef float GyFloat; 

/** A two-dimensional vector. */
struct GyVector_;
typedef struct GyVector_ GyVector;

struct GyVector_     {
  GyFloat x, y;
};

typedef const GyVector GyCVector;

/** Makes a new GyVector. */
GyVector gyvector(GyFloat x, GyFloat y); 

/** Initializes a GyVector */
GyVector * gyvector_init(GyVector * vec, GyFloat x, GyFloat y);

/** Allocates and initializes a GyVector and returns it. Retuns NULL if out 
   of memory. */
GyVector * gyvector_new(GyFloat x, GyFloat y);

/** Allocates and initializes a Gyvectory and returns it. Retuns NULL if out 
   of memory. */
GyVector * gyvector_newvector(GyVector v1);

/** Frees a GyVector's allocated memory. Returns NULL. */
GyVector * gyvector_free(GyVector * vec);

/** Checks if a vector is zero. Returns nonzero if it is, zero if not. */
int gyvector_zero_p(const GyVector v1);

/** Returns a zero vector. */
GyVector gyvector_zero();

/** Compares two vectors and sees if they are within delta of each other. */
int gyvector_equal_delta(const GyVector v1, const GyVector v2, 
                            GyFloat delta);
                            
/** Compares two vectors for strict equality. */
int gyvector_equal(const GyVector v1, const GyVector v2);

/** Vector addition. */
GyVector gyvector_add(GyVector v1, GyVector v2); 

/** Vector substraction. */
GyVector gyvector_sub(GyVector v1, GyVector v2); 

/** Vector negation. */
GyVector gyvector_neg(GyVector v1); 

/** Scalar Vector multiplication. */
GyVector gyvector_mul(GyVector v1, GyFloat f); 

/** Scalar Vector division. */
GyVector gyvector_div(GyVector v1, GyFloat f); 

/** Vector dot product. */
GyFloat gyvector_dot(GyVector v1, GyVector v2); 

/** 
Cross product magnitude. The cross product of 2D x,y vectors is 
a 3D vector with a z component, so this function returns only the magnitude of
that z component.  
*/
GyFloat gyvector_cross(GyVector v1, GyVector v2);

/** Perpendicular vector, rotated by 90 degrees, anticlockwise. */
GyVector gyvector_perp(GyVector v1);

/** Perpendicular vector, rotated by -90 degrees, clockwise. */
GyVector gyvector_nperp(GyVector v1);

/** Returns the dot poduct of the vector and itself. */
GyFloat gyvector_dotself(GyVector v1);

/** Returns the squared length of the vector. Same as dotself, really. 
Useful for comparing lengths when speed is of importance.  */
GyFloat gyvector_lensq(GyVector v1);

/** Returns the length of the vector. */
GyFloat gyvector_len(GyVector v1);

/** Returns a vector that is the projection of v1 onto v2*/
GyVector gyvector_project(GyVector v1, GyVector v2);

/** Rotates v1 by v2. */
GyVector gyvector_rotate(GyVector v1, GyVector v2); 
 
/** Inverse rotation. */
GyVector gyvector_unrotate(GyVector v1, GyVector v2);

/** Linear interpolation on a line between between v1 and v2. Returns a vector 
that points to a point on the line between v1 and v2. */
GyVector gyvector_lerp(GyVector v1, GyVector v2, GyFloat tx);

/** Returns v1 normalized. Not safe for division by 0 in case length is 0. */
GyVector gyvector_normalize_unsafe(GyVector v1); 

/** Returns v1 normalized. Safe for division by 0 in case length is 0. */
GyVector gyvector_normalize(GyVector v1);

/** If the length of the vector is greater than max, this function 
returns a vector with the same direction as v1, but with the length 
of that vector limited to max. Otherwise, it returns v1  */
GyVector gyvector_max(GyVector v1, GyFloat max); 

/** Returns the distance between v1 and v2. */
GyFloat gyvector_dist(GyVector v1, GyVector v2);
 
/** Returns the squared distance between v1 and v2. */
GyFloat gyvector_distsq(GyVector v1, GyVector v2); 

/** Returns true if the distance between v1 and v2 is less than d. */
int gyvector_near(GyVector v1, GyVector v2, GyFloat d);

/** Retuns a unit vector that makes an angle a with the X axis. */
GyVector gyvector_fromangle(GyFloat f);
 
/** Returns the angle of the vector with the X axis in clockwise 
direction in radians */
GyFloat gyvector_angle(GyVector v1);
 

/** Time related functions, including a timer. */
struct GyTimer_;
typedef struct GyTimer_ GyTimer;

/** allocates a GyTimer. */
GyTimer * gytimer_alloc();

/** Frees the timer. */
GyTimer * gytimer_free(GyTimer * self);

uint32_t gytimer_ticks();

/** Starts or resets the timer. */
GyTimer * gytimer_start(GyTimer * self);

/** Sets the delay (wait time) of the timer. */
GyTimer * gytimer_delay_(GyTimer * self, uint32_t delay);

/** Gets the delay (wait time) of the timer. Returns 0 if self is NULL.*/
uint32_t gytimer_delay(GyTimer * self);

/** Initializes the timer with a delay in ms. */
GyTimer * gytimer_init(GyTimer * self, uint32_t delay);

/** Creates a new timer with the given delay. */
GyTimer * gytimer_new(uint32_t delay);

/** Returns amount of ticks (in ms) passed since the timer was started. */
uint32_t gytimer_passed(GyTimer * self);


/** Returns true if the delay of the timer has passed since 
gytimer_start was called, false if not. */
int gytimer_ring(GyTimer * self);


#endif
