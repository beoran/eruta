#include "gari.h"

#define GARI_INTERN_ONLY
#include "gari_intern.h"


GariImage * gari_image_wrap(SDL_Surface * surface) {
  return (GariImage *)(surface);
}

void gari_image_free(GariImage * image) {
  if (!image) {return;} 
  SDL_FreeSurface(GARI_IMAGE_SURFACE(image));
}


GariColor gari_image_maprgb(GariImage *img, GariRGBA rgba) {
  return  gari_surface_maprgb(GARI_IMAGE_SURFACE(img), rgba);
}

GariColor gari_image_maprgba(GariImage *img, GariRGBA rgba) {
  return  gari_surface_maprgba(GARI_IMAGE_SURFACE(img), rgba);
}

GariRGBA gari_image_getrgb(GariImage *img, GariColor color) {
  return gari_surface_getrgb(GARI_IMAGE_SURFACE(img), color);
}

GariRGBA gari_image_getrgba(GariImage *img, GariColor color) {
  return gari_surface_getrgba(GARI_IMAGE_SURFACE(img), color);
}

GariColor gari_image_rgb(GariImage *img, uint8_t r, uint8_t g, uint8_t b) {
  return SDL_MapRGB(GARI_IMAGE_FORMAT(img), r, g, b); 
}

GariColor gari_image_rgba(GariImage *img, 
          uint8_t r, uint8_t g, uint8_t b, uint8_t a) {
  return  SDL_MapRGBA(GARI_IMAGE_FORMAT(img), r, g, b, a); 
}


/** Optimizes the image for drawing to the screen. */
GariImage * gari_image_optimize(GariImage * image, int mode, GariColor colorkey) {
  SDL_Surface * surface;
  surface = gari_image_surface(image);
  if (!SDL_GetVideoSurface()) {
    return NULL; 
    // SDL_DisplayFormat won't work unless there is an active video surface.
  };
  
  switch (mode) {
    case GariImageAlpha:  
      return gari_image_wrap(SDL_DisplayFormatAlpha(surface));
      
    case GariImageColorkey:
      SDL_SetColorKey(surface, SDL_SRCCOLORKEY | SDL_RLEACCEL, colorkey);
      return gari_image_wrap(SDL_DisplayFormat(surface));
    case GariImageSolid:
    default:  
      return gari_image_wrap(SDL_DisplayFormat(surface));
  }

}

/** Loads the image from the named file.  */
GariImage * gari_image_loadraw(char * filename) {
  GariImage * result;
  result = gari_image_wrap(IMG_Load(filename));
  return result;
}



/** Loads the image from the named file, and optimizes it for display 
on the active video sorface if it was already set. */
/*
GariImage * gari_image_load(char * filename) {
  GariImage * normal, * faster;
  normal = gari_image_loadraw(filename);
  if (!normal) { return NULL ; }
  faster = gari_image_optimize(   
}
*/

/** Blits one image to another one with the given coordinates. */
void gari_image_blit(GariImage * dst, int dstx, int dsty, GariImage * src) {
  SDL_Surface *sdlsrc, *sdldst;
  SDL_Rect    dstrect;
  dstrect.x = dstx;
  dstrect.y = dsty;
  sdlsrc    = GARI_IMAGE_SURFACE(src);
  sdldst    = GARI_IMAGE_SURFACE(dst);
  SDL_BlitSurface(src, NULL, dst, &dstrect);  
}

/* Make new images for various colour depths. */

GariImage * gari_image_make8(int w, int h, int mode) {
    SDL_Surface* result = NULL;
    result = SDL_CreateRGBSurface(SDL_SWSURFACE, w, h, 8, 3 << 5, 3 << 2, 3, 0);    
    return gari_image_wrap(result);
}

GariImage * gari_image_make12(int w, int h, int mode) {
    SDL_Surface* result = NULL;
    result = SDL_CreateRGBSurface(SDL_SWSURFACE, w, h, 12, 15 << 8, 15 << 4, 15, 0);    
    return gari_image_wrap(result);
}

GariImage * gari_image_make15(int w, int h, int mode) {
    SDL_Surface* result = NULL;
    result = SDL_CreateRGBSurface(SDL_SWSURFACE, w, h, 15, 3 << 5, 3 << 2, 3, 0);    
    return gari_image_wrap(result);
}


GariImage * gari_image_make16(int w, int h, int mode) {
    SDL_Surface* result = NULL;
    if (mode == GariImageAlpha) {     
     // 4 - 4 - 4 - 4 
    #if SDL_BYTEORDER == SDL_BIG_ENDIAN
      result = SDL_CreateRGBSurface(SDL_SWSURFACE, w, h, 16, 0xf000, 0x0f00, 0x00f0, 0x000f);
    #else
      result = SDL_CreateRGBSurface(SDL_SWSURFACE, w, h, 16, 0x00f0, 0x000f, 0xf000, 0x0f00);
    #endif
      // SDL_SetAlpha(result, 0, SDL_ALPHA_OPAQUE);
    } else {
      // 5 - 5 - 5 
      result = SDL_CreateRGBSurface(SDL_SWSURFACE, w, h, 16, 31 << 10, 31 << 5, 31, 0);
    }    
    SDL_SetAlpha(result, 0, SDL_ALPHA_OPAQUE);
    return gari_image_wrap(result);
}


GariImage * gari_image_make24(int w, int h, int mode) {
    SDL_Surface* result = NULL;
    #if SDL_BYTEORDER == SDL_BIG_ENDIAN
      result = SDL_CreateRGBSurface(SDL_SWSURFACE, w, h, 24, 0xff0000, 0x00ff00, 0x0000ff, 0);
    #else
      result = SDL_CreateRGBSurface(SDL_SWSURFACE, w, h, 24, 0x0000ff, 0x00ff00, 0xff0000, 0);
    #endif
    // SDL_SetAlpha(result, 0, SDL_ALPHA_OPAQUE);
    return gari_image_wrap(result);
}

GariImage * gari_image_make32(int w, int h, int mode) {
    SDL_Surface* result = NULL;
    #if SDL_BYTEORDER == SDL_BIG_ENDIAN
      result = SDL_CreateRGBSurface(SDL_SWSURFACE, w, h, 32, 0xff000000, 
				    0x00ff0000, 0x0000ff00, 0x000000ff);
    #else
      result = SDL_CreateRGBSurface(SDL_SWSURFACE, w, h, 32, 
					0x000000ff, 0x0000ff00, 0x00ff0000, 0xff000000);
    #endif
    // SDL_SetAlpha(result, 0, SDL_ALPHA_OPAQUE);
    return gari_image_wrap(result);
}


/** Makes an empty gari image with the given color depth and alpha mode. 
The video mode must have been set and the screen must have been opened. 
*/
GariImage * gari_image_makedepth(int w, int h, int depth, int mode) {
  GariImage * result;
  SDL_Surface * aid;
  switch(depth) {
    case 8: return gari_image_make8(w, h, mode);
    case 12: return gari_image_make12(w, h, mode);
    case 15: return gari_image_make15(w, h, mode);
    case 16: return gari_image_make16(w, h, mode);
    case 24: return gari_image_make24(w, h, mode);
    case 32: 
    default:   
	    return gari_image_make32(w, h, mode);    
  }
} 


/** Transforms a GariColor from one kind of image to another. */
GariColor gari_image_mapcolor(GariImage * dst, 
                              GariImage * src, GariColor color) {
  GariRGBA rgba;
  if (GARI_IMAGE_FORMAT(dst)  == GARI_IMAGE_FORMAT(src)) {
    return color; // no mappping if format identical.
  } 
  rgba = gari_image_getrgba(src, color);
  return gari_image_maprgba(dst, rgba);
}
   









