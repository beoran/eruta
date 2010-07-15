#include "gari.h"

#define GARI_INTERN_ONLY
#include "gari_intern.h"


GariImage * gari_image_wrap(SDL_Surface * surface) {
  return (GariImage *)(surface);
}

GariColor gari_image_maprgb(GariImage *img, GariRGBA rgba) {
  return  gari_surface_maprgb(GARI_IMAGE_SURFACE(img), rgba);
}

GariColor gari_image_rgb(GariImage *img, uint8_t r, uint8_t g, uint8_t b) {
  return SDL_MapRGB(GARI_IMAGE_FORMAT(img), r, g, b); 
}

GariColor gari_image_rgba(GariImage *img, 
          uint8_t r, uint8_t g, uint8_t b, uint8_t a) {
  return  SDL_MapRGBA(GARI_IMAGE_FORMAT(img), r, g, b, a); 
}


/** Optimizes the image for drawing to the screen. */
int gari_image_optimize(GariImage * image, int mode, GariColor colorkey) {
  SDL_Surface * surface;
  surface = gari_image_surface(image);
  switch (mode) {
    case GariImageOptimizeAlpha:  
      return gari_image_wrap(SDL_DisplayFormatAlpha(surface));
      
    case GariImageOptimizeColorkey:
      SDL_SetColorKey(surface, SDL_SRCCOLORKEY | SDL_RLEACCEL, colorkey);
      return gari_image_wrap(SDL_DisplayFormat(surface));
    case GariImageOptimizeSolid:
    default:  
      return gari_image_wrap(SDL_DisplayFormat(surface));
  }

}



