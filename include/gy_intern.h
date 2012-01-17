/*
This file is for internal use by Gy library ONLY. Don't use anything
you find in here outside of the library. 
*/
#ifndef _GY_INTERN_H_
#define _GY_INTERN_H_


#ifndef GY_INTERN_ONLY
#error This include file is only for used inside the Gy Library source files.
#endif


#include "SDL.h"
#include "SDL_ttf.h"
#include "SDL_image.h"
#include "SDL_mixer.h"

/* A few macros for memory allocation, so we can allow libgc later. */
#ifndef gymalloc
#define gymalloc malloc
#endif 

#ifndef gyfree
#define gyfree free
#endif 

#ifndef GY_MALLOC
#define GY_MALLOC(SIZE) (malloc(SIZE))
#endif 

#ifndef GY_FREE
#define GY_FREE(PTR) (free(PTR))
#endif 

#define gyallocate(TYPENAME) gymalloc(sizeof(TYPENAME))
#define GY_ALLOCATE(TYPENAME) GY_MALLOC(sizeof(TYPENAME))
#define GY_ALLOC(TYPENAME) GY_ALLOCATE(TYPENAME)


/* Initializes a gari game. External users will use gygame_make. */
GyGame * gygame_init(GyGame * game);


 

/* Macros that can be used to inline similar named functions. */

#define GY_IMAGE_SURFACE(IMG) ((SDL_Surface*)(IMG))
#define GY_IMAGE_W(IMG)       (GY_IMAGE_SURFACE(IMG)->w)
#define GY_IMAGE_H(IMG)       (GY_IMAGE_SURFACE(IMG)->h)
#define GY_IMAGE_FORMAT(IMG)  (GY_IMAGE_SURFACE(IMG)->format)
#define GY_IMAGE_DEPTH(IMG)   (GY_IMAGE_FORMAT(IMG)->BitsPerPixel)
#define GY_SURFACE_LOCK(S)    (SDL_MUSTLOCK(S) && (SDL_LockSurface(S)))
#define GY_SURFACE_UNLOCK(S)  (SDL_MUSTLOCK(S) && (SDL_UnlockSurface(S), 0))
#define GY_IMAGE_PIXELOUTSIDE(IMG, X, Y)\
        ( (!IMG)                        ||\
          (X < 0) || (Y < 0)            ||\
          (X >= GY_IMAGE_W(IMG))      ||\
          (Y >= GY_IMAGE_H(IMG))        \
        )
        
#define GY_DRAW_IMAGE(DRAW) (DRAW->image)
#define GY_DRAW_OUTSIDE(DRAW, X, Y)\
        GY_IMAGE_PIXELOUTSIDE(GY_DRAW_IMAGE(DRAW), X, Y) 

/* Macros for the clipping rectangle. */
#define GY_IMAGE_CLIPRECT(IMG)   (GY_IMAGE_SURFACE(IMG)->clip_rect)
#define GY_IMAGE_CLIPRECT_X(IMG) (GY_IMAGE_CLIPRECT(IMG).x)
#define GY_IMAGE_CLIPRECT_Y(IMG) (GY_IMAGE_CLIPRECT(IMG).y)
#define GY_IMAGE_CLIPRECT_W(IMG) (GY_IMAGE_CLIPRECT(IMG).w)
#define GY_IMAGE_CLIPRECT_H(IMG) (GY_IMAGE_CLIPRECT(IMG).h)
#define GY_IMAGE_CLIPRECT_XMAX(IMG)           \
        (GY_IMAGE_CLIPRECT_X(IMG) + GY_IMAGE_CLIPRECT_W(IMG))
#define GY_IMAGE_CLIPRECT_YMAX(IMG)           \
        (GY_IMAGE_CLIPRECT_Y(IMG) + GY_IMAGE_CLIPRECT_H(IMG))

#define GY_IMAGE_PIXELCLIP(IMG, X, Y)             \
        ( ( X < GY_IMAGE_CLIPRECT_X(IMG) )      ||\
          ( Y < GY_IMAGE_CLIPRECT_Y(IMG) )      ||\
          ( X >= GY_IMAGE_CLIPRECT_XMAX(IMG) )  ||\
          ( Y >= GY_IMAGE_CLIPRECT_YMAX(IMG) )    \
        )

#define GY_SURFACE_PIXELS(SURFACE) ((SURFACE)->pixels)
#define GY_SURFACE_PITCH(SURFACE)  ((SURFACE)->pitch)
#define GY_PTR_TYPE                uint8_t*
#define GY_TO_PTR(VAL)             ((GY_PTR_TYPE)(VAL)) 
#define GY_X_OFFSET(X, C)          (((size_t)(X)) << (C)) 
#define GY_Y_OFFSET(Y, C)          (((size_t)(Y)) *  (C))
#define GY_XY_OFFSET(X, Y, XC, YC)                \
          (GY_X_OFFSET(X, XC) + GY_Y_OFFSET(Y, YC))
/* Macros for pointer access. 24 bits is too complicated for macro inlining. */
#define GY_SURFACE_PIXELPTR8(SURFACE, X, Y)       \
         ((uint8_t *)(GY_TO_PTR(GY_SURFACE_PIXELS(SURFACE)) +\
           GY_XY_OFFSET(X, Y, 0, GY_SURFACE_PITCH(SURFACE)) ))

#define GY_SURFACE_PIXELPTR16(SURFACE, X, Y)       \
         ((uint16_t *)(GY_TO_PTR(GY_SURFACE_PIXELS(SURFACE)) +\
           GY_XY_OFFSET(X, Y, 1, GY_SURFACE_PITCH(SURFACE)) ))

#define GY_SURFACE_PIXELPTR32(SURFACE, X, Y)       \
         ((uint32_t *)(GY_TO_PTR(GY_SURFACE_PIXELS(SURFACE)) +\
           GY_XY_OFFSET(X, Y, 2, GY_SURFACE_PITCH(SURFACE)) ))

/* color mapping */

#define GY_IMAGE_RGB(IMG, R, G, B) \
          (SDL_MapRGB(GY_IMAGE_FORMAT(IMG), R, G, B)) 

#define GY_IMAGE_RGBA(IMG, R, G, B, A) \
          (SDL_MapRGBA(GY_IMAGE_FORMAT(img), R, G, B, A))
          
          
/* Clamping macros */
/* GY_CLAMP returns VALUE clamped between MIN and MAX. */
#define GY_CLAMP(VALUE, MIN, MAX) \
          (VALUE < MIN ? MIN : ( VALUE > MAX ? MAX: VALUE ) )

/** Syntactic guard macro that returns RET if COND is false. */
#define GUARI_GUARD(COND, RET) do { if(!(COND)) { return RET; } } while(0)

/** Syntactic macro that returns NULL if the given value is is NULL or 0*/
#define GY_GUARD_NULL(VALUE) GUARI_GUARD(VALUE == NULL, NULL)

SDL_Surface * gyimage_surface(GyImage * img);
 
 
void gysurface_lock(SDL_Surface * surface);
void gysurface_unlock(SDL_Surface * surface);



GyColor gysurface_getrgb(SDL_Surface * surface, GyDye color);

GyColor gysurface_getrgba(SDL_Surface * surface, GyDye color); 
 

// maps the rgb components of a color to a color for this surface 
GyDye gysurface_maprgb(SDL_Surface * surface, GyColor rgba); 


// maps the rgb components of a color to a color for this surface 
GyDye gysurface_maprgba(SDL_Surface * surface, GyColor rgba); 



// Puts a pixel with the given color at the given coordinates
// Takes the clipping rectangle and surface bounds into consideration
// Does no locking, so lock around it! Only for 8 bits surfaces. 
void gyimage_putpixel8_nl(GyImage *img, int x, int y, GyDye color);

// Puts a pixel with the given color at the given coordinates
// Takes the clipping rectangle and surface bounds into consideration
// Does no locking, so lock around it! Only for 16 bits surfaces. 
void gyimage_putpixel16_nl(GyImage *img, int x, int y, GyDye color);

// Puts a pixel with the given color at the given coordinates
// Takes the clipping rectangle and surface bounds into consideration
// Does no locking, so lock around it! Only for 24 bits surfaces. 
void gyimage_putpixel24_nl(GyImage *img, int x, int y, GyDye color);


// Puts a pixel with the given color at the given coordinates
// Takes the clipping rectangle and surface bounds into consideration
// Does no locking, so lock around it! Only for 32 bits surfaces. 
void gyimage_putpixel32_nl(GyImage *img, int x, int y, GyDye color);


// Returns TRUE if the pixel lies outside the image
// returns FALSE if not so and it may be drawn
int gyimage_pixeloutside(GyImage * image, int x, int y); 

// Returns TRUE if the pixel lies outside the clipping rectangle
// returns FALSE if not so and it may be drawn
int gyimage_pixelclip(GyImage * image, int x, int y); 

// Returns true if the pixel lies outside of the image's boundaries or 
// clipping rectangle.
int gyimage_pixeloutorclip(GyImage * image, int x, int y);

// Wraps a SDL_Surface inside a GyImage.
GyImage * gyimage_wrap(SDL_Surface * surface);

// Gets the color of a pixel from this surface 
// Returns 0 if the pixel is outside of the clipping rectangle,
// or outside of the bounds of the surface
// Does no locking, so lock around it!
GyDye gyimage_getpixel_nolock(GyImage *img, int x, int y);

/** "Translates" a GyDye from one kind of image to another. */
GyDye gyimage_mapcolor(GyImage * dst, 
                              GyImage * src, GyDye color);
                              
    
    
void gyimage_dot_nolock(GyImage *img, int x, int y, GyColor color); 

GyColor gyimage_getdot_nolock(GyImage *img, int x, int y);

void gyimage_blenddot_nolock(GyImage *img, int x, int y, GyColor color);

/** Gets a tile from a tile set by it's tile id. **/
GyTile * gytileset_get(GyTileset * set, int index);


    
    

#endif
