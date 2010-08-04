/*
This file is for internal use by Gari library ONLY. Don't use anything
you find in here outside of the library. 
*/
#ifndef _GARI_INTERN_H_
#define _GARI_INTERN_H_


#ifndef GARI_INTERN_ONLY
#error This include file is only for used inside the Gari Library source files.
#endif


#include "SDL_ttf.h"
#include "SDL_image.h"

/* A few macros for memory allocation, so we can allow libgc later. */
#ifndef gari_malloc
#define gari_malloc malloc
#endif 

#ifndef gari_free
#define gari_free free
#endif 

#ifndef GARI_MALLOC
#define GARI_MALLOC(SIZE) (malloc(SIZE))
#endif 

#ifndef GARI_FREE
#define GARI_FREE(PTR) (free(PTR))
#endif 

#define gari_allocate(TYPENAME) gari_malloc(sizeof(TYPENAME))
#define GARI_ALLOCATE(TYPENAME) GARI_MALLOC(sizeof(TYPENAME))


/* Initializes a gari game. External users will use gari_game_make. */
GariGame * gari_game_init(GariGame * game);


 

/* Macros that can be used to inline similar named functions. */

#define GARI_IMAGE_SURFACE(IMG) ((SDL_Surface*)(IMG))
#define GARI_IMAGE_W(IMG)       (GARI_IMAGE_SURFACE(IMG)->w)
#define GARI_IMAGE_H(IMG)       (GARI_IMAGE_SURFACE(IMG)->h)
#define GARI_IMAGE_FORMAT(IMG)  (GARI_IMAGE_SURFACE(IMG)->format)
#define GARI_IMAGE_DEPTH(IMG)   (GARI_IMAGE_FORMAT(IMG)->BitsPerPixel)
#define GARI_SURFACE_LOCK(S)    (SDL_MUSTLOCK(S) && (SDL_LockSurface(S)))
#define GARI_SURFACE_UNLOCK(S)  (SDL_MUSTLOCK(S) && (SDL_UnlockSurface(S), 0))
#define GARI_IMAGE_PIXELOUTSIDE(IMG, X, Y)\
        ( (X < 0) || (Y < 0)            ||\
          (X >= GARI_IMAGE_W(IMG))      ||\
          (Y >= GARI_IMAGE_H(IMG))        \
        )

/* Macros for the clipping rectangle. */
#define GARI_IMAGE_CLIPRECT(IMG)   (GARI_IMAGE_SURFACE(IMG)->clip_rect)
#define GARI_IMAGE_CLIPRECT_X(IMG) (GARI_IMAGE_CLIPRECT(IMG).x)
#define GARI_IMAGE_CLIPRECT_Y(IMG) (GARI_IMAGE_CLIPRECT(IMG).y)
#define GARI_IMAGE_CLIPRECT_W(IMG) (GARI_IMAGE_CLIPRECT(IMG).w)
#define GARI_IMAGE_CLIPRECT_H(IMG) (GARI_IMAGE_CLIPRECT(IMG).h)
#define GARI_IMAGE_CLIPRECT_XMAX(IMG)           \
        (GARI_IMAGE_CLIPRECT_X(IMG) + GARI_IMAGE_CLIPRECT_W(IMG))
#define GARI_IMAGE_CLIPRECT_YMAX(IMG)           \
        (GARI_IMAGE_CLIPRECT_Y(IMG) + GARI_IMAGE_CLIPRECT_H(IMG))

#define GARI_IMAGE_PIXELCLIP(IMG, X, Y)             \
        ( ( X < GARI_IMAGE_CLIPRECT_X(IMG) )      ||\
          ( Y < GARI_IMAGE_CLIPRECT_Y(IMG) )      ||\
          ( X >= GARI_IMAGE_CLIPRECT_XMAX(IMG) )  ||\
          ( Y >= GARI_IMAGE_CLIPRECT_YMAX(IMG) )    \
        )

#define GARI_SURFACE_PIXELS(SURFACE) ((SURFACE)->pixels)
#define GARI_SURFACE_PITCH(SURFACE)  ((SURFACE)->pitch)
#define GARI_PTR_TYPE                uint8_t*
#define GARI_TO_PTR(VAL)             ((GARI_PTR_TYPE)(VAL)) 
#define GARI_X_OFFSET(X, C)          (((size_t)(X)) << (C)) 
#define GARI_Y_OFFSET(Y, C)          (((size_t)(Y)) *  (C))
#define GARI_XY_OFFSET(X, Y, XC, YC)                \
          (GARI_X_OFFSET(X, XC) + GARI_Y_OFFSET(Y, YC))
/* Macros for pointer access. 24 bits is too complicated for macro inlining. */
#define GARI_SURFACE_PIXELPTR8(SURFACE, X, Y)       \
         ((uint8_t *)(GARI_TO_PTR(GARI_SURFACE_PIXELS(SURFACE)) +\
           GARI_XY_OFFSET(X, Y, 0, GARI_SURFACE_PITCH(SURFACE)) ))

#define GARI_SURFACE_PIXELPTR16(SURFACE, X, Y)       \
         ((uint16_t *)(GARI_TO_PTR(GARI_SURFACE_PIXELS(SURFACE)) +\
           GARI_XY_OFFSET(X, Y, 1, GARI_SURFACE_PITCH(SURFACE)) ))

#define GARI_SURFACE_PIXELPTR32(SURFACE, X, Y)       \
         ((uint32_t *)(GARI_TO_PTR(GARI_SURFACE_PIXELS(SURFACE)) +\
           GARI_XY_OFFSET(X, Y, 2, GARI_SURFACE_PITCH(SURFACE)) ))

/* color mapping */

#define GARI_IMAGE_RGB(IMG, R, G, B) \
          (SDL_MapRGB(GARI_IMAGE_FORMAT(IMG), R, G, B)) 

#define GARI_IMAGE_RGBA(IMG, R, G, B, A) \
          (SDL_MapRGBA(GARI_IMAGE_FORMAT(img), R, G, B, A))


SDL_Surface * gari_image_surface(GariImage * img);
 
 
void gari_surface_lock(SDL_Surface * surface);
void gari_surface_unlock(SDL_Surface * surface);

// definition of RGBA
struct GariRGBA_ {
  uint8_t r, g, b, a;
};


GariRGBA gari_surface_getrgb(SDL_Surface * surface, GariColor color);

GariRGBA gari_surface_getrgba(SDL_Surface * surface, GariColor color); 
 

// maps the rgb components of a color to a color for this surface 
GariColor gari_surface_maprgb(SDL_Surface * surface, GariRGBA rgba); 


// maps the rgb components of a color to a color for this surface 
GariColor gari_surface_maprgba(SDL_Surface * surface, GariRGBA rgba); 



// Puts a pixel with the given color at the given coordinates
// Takes the clipping rectangle and surface bounds into consideration
// Does no locking, so lock around it! Only for 8 bits surfaces. 
void gari_image_putpixel8_nl(GariImage *img, int x, int y, GariColor color);

// Puts a pixel with the given color at the given coordinates
// Takes the clipping rectangle and surface bounds into consideration
// Does no locking, so lock around it! Only for 16 bits surfaces. 
void gari_image_putpixel16_nl(GariImage *img, int x, int y, GariColor color);

// Puts a pixel with the given color at the given coordinates
// Takes the clipping rectangle and surface bounds into consideration
// Does no locking, so lock around it! Only for 24 bits surfaces. 
void gari_image_putpixel24_nl(GariImage *img, int x, int y, GariColor color);


// Puts a pixel with the given color at the given coordinates
// Takes the clipping rectangle and surface bounds into consideration
// Does no locking, so lock around it! Only for 32 bits surfaces. 
void gari_image_putpixel32_nl(GariImage *img, int x, int y, GariColor color);


// Wraps a SDL_Surface inside a GariImage.
GariImage * gari_image_wrap(SDL_Surface * surface);

// Gets the color of a pixel from this surface 
// Returns 0 if the pixel is outside of the clipping rectangle,
// or outside of the bounds of the surface
// Does no locking, so lock around it!
GariColor gari_image_getpixel_nolock(GariImage *img, int x, int y);

/** "Translates" a GariColor from one kind of image to another. */
GariColor gari_image_mapcolor(GariImage * dst, 
                              GariImage * src, GariColor color);

#endif
