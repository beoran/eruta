/*
This file is for internal use by Gy library ONLY. Don't use anything
you find in here outside of the library. 
*/
#ifndef _GARI_INTERN_H_
#define _GARI_INTERN_H_


#ifndef GARI_INTERN_ONLY
#error This include file is only for used inside the Gy Library source files.
#endif


#include "SDL.h"
#include "SDL_ttf.h"
#include "SDL_image.h"
#include "SDL_mixer.h"

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
#define GARI_ALLOC(TYPENAME) GARI_ALLOCATE(TYPENAME)


/* Initializes a gari game. External users will use gari_game_make. */
GyGame * gari_game_init(GyGame * game);


 

/* Macros that can be used to inline similar named functions. */

#define GARI_IMAGE_SURFACE(IMG) ((SDL_Surface*)(IMG))
#define GARI_IMAGE_W(IMG)       (GARI_IMAGE_SURFACE(IMG)->w)
#define GARI_IMAGE_H(IMG)       (GARI_IMAGE_SURFACE(IMG)->h)
#define GARI_IMAGE_FORMAT(IMG)  (GARI_IMAGE_SURFACE(IMG)->format)
#define GARI_IMAGE_DEPTH(IMG)   (GARI_IMAGE_FORMAT(IMG)->BitsPerPixel)
#define GARI_SURFACE_LOCK(S)    (SDL_MUSTLOCK(S) && (SDL_LockSurface(S)))
#define GARI_SURFACE_UNLOCK(S)  (SDL_MUSTLOCK(S) && (SDL_UnlockSurface(S), 0))
#define GARI_IMAGE_PIXELOUTSIDE(IMG, X, Y)\
        ( (!IMG)                        ||\
          (X < 0) || (Y < 0)            ||\
          (X >= GARI_IMAGE_W(IMG))      ||\
          (Y >= GARI_IMAGE_H(IMG))        \
        )
        
#define GARI_DRAW_IMAGE(DRAW) (DRAW->image)
#define GARI_DRAW_OUTSIDE(DRAW, X, Y)\
        GARI_IMAGE_PIXELOUTSIDE(GARI_DRAW_IMAGE(DRAW), X, Y) 

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
          
          
/* Clamping macros */
/* GARI_CLAMP returns VALUE clamped between MIN and MAX. */
#define GARI_CLAMP(VALUE, MIN, MAX) \
          (VALUE < MIN ? MIN : ( VALUE > MAX ? MAX: VALUE ) )

/** Syntactic guard macro that returns RET if COND is false. */
#define GUARI_GUARD(COND, RET) do { if(!(COND)) { return RET; } } while(0)

/** Syntactic macro that returns NULL if the given value is is NULL or 0*/
#define GARI_GUARD_NULL(VALUE) GUARI_GUARD(VALUE == NULL, NULL)

SDL_Surface * gari_image_surface(GyImage * img);
 
 
void gari_surface_lock(SDL_Surface * surface);
void gari_surface_unlock(SDL_Surface * surface);



GyColor gari_surface_getrgb(SDL_Surface * surface, GyDye color);

GyColor gari_surface_getrgba(SDL_Surface * surface, GyDye color); 
 

// maps the rgb components of a color to a color for this surface 
GyDye gari_surface_maprgb(SDL_Surface * surface, GyColor rgba); 


// maps the rgb components of a color to a color for this surface 
GyDye gari_surface_maprgba(SDL_Surface * surface, GyColor rgba); 



// Puts a pixel with the given color at the given coordinates
// Takes the clipping rectangle and surface bounds into consideration
// Does no locking, so lock around it! Only for 8 bits surfaces. 
void gari_image_putpixel8_nl(GyImage *img, int x, int y, GyDye color);

// Puts a pixel with the given color at the given coordinates
// Takes the clipping rectangle and surface bounds into consideration
// Does no locking, so lock around it! Only for 16 bits surfaces. 
void gari_image_putpixel16_nl(GyImage *img, int x, int y, GyDye color);

// Puts a pixel with the given color at the given coordinates
// Takes the clipping rectangle and surface bounds into consideration
// Does no locking, so lock around it! Only for 24 bits surfaces. 
void gari_image_putpixel24_nl(GyImage *img, int x, int y, GyDye color);


// Puts a pixel with the given color at the given coordinates
// Takes the clipping rectangle and surface bounds into consideration
// Does no locking, so lock around it! Only for 32 bits surfaces. 
void gari_image_putpixel32_nl(GyImage *img, int x, int y, GyDye color);


// Returns TRUE if the pixel lies outside the image
// returns FALSE if not so and it may be drawn
int gari_image_pixeloutside(GyImage * image, int x, int y); 

// Returns TRUE if the pixel lies outside the clipping rectangle
// returns FALSE if not so and it may be drawn
int gari_image_pixelclip(GyImage * image, int x, int y); 

// Returns true if the pixel lies outside of the image's boundaries or 
// clipping rectangle.
int gari_image_pixeloutorclip(GyImage * image, int x, int y);

// Wraps a SDL_Surface inside a GyImage.
GyImage * gari_image_wrap(SDL_Surface * surface);

// Gets the color of a pixel from this surface 
// Returns 0 if the pixel is outside of the clipping rectangle,
// or outside of the bounds of the surface
// Does no locking, so lock around it!
GyDye gari_image_getpixel_nolock(GyImage *img, int x, int y);

/** "Translates" a GyDye from one kind of image to another. */
GyDye gari_image_mapcolor(GyImage * dst, 
                              GyImage * src, GyDye color);
                              
    
    
void gari_image_dot_nolock(GyImage *img, int x, int y, GyColor color); 

GyColor gari_image_getdot_nolock(GyImage *img, int x, int y);

void gari_image_blenddot_nolock(GyImage *img, int x, int y, GyColor color);

/** Gets a tile from a tile set by it's tile id. **/
GyTile * gari_tileset_get(GyTileset * set, int index);


    
    

#endif
