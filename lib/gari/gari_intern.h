#ifndef _GARI_INTERN_H_
#define _GARI_INTERN_H_


#ifndef GARI_INTERN_ONLY
#error This include file is only for used inside the Gari Library source files.
#endif

/* Initializes a gari game. Externa lusers will use gari_game_make. */
GariGame * gari_game_init(GariGame * game);


 

/* Macros that can be used to inline similar named functions. */

#define GARI_IMAGE_SURFACE(IMG) ((SDL_Surface*)(IMG))
#define GARI_IMAGE_W(IMG)       (GARI_IMAGE_SURFACE(IMG)->w)
#define GARI_IMAGE_H(IMG)       (GARI_IMAGE_SURFACE(IMG)->h)
#define GARI_IMAGE_FORMAT(IMG)  (GARI_IMAGE_SURFACE(IMG)->format)
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

// maps the rgb components of a color to a color for this surface 
GariColor gari_surface_maprgb(SDL_Surface * surface, GariRGBA rgba); 




#endif
