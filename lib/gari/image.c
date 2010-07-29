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

/* Returns the masks for an image with given color depth and alpha.
* XXX: this is only for little endian?  
*/
GariRGBA gari_image_masks(int depth, int alpha) {
  GariRGBA mask;
  mask.a = 0;
   
  switch(depth) {
    case 8:
      mask.r = 0xFF >> 6 << 5; mask.g = 0xFF >> 5 << 2; mask.b = 0xFF >> 6;
    break;

    case 12:
      mask.r = 0xFF >> 4 << 8; mask.g = 0xFF >> 4 << 4; mask.b = 0xFF >> 4;
    break;
    
    case 15:
      mask.r = 0xFF >> 6 << 5; mask.g = 0xFF >> 5 << 2; mask.b = 0xFF >> 6;
    break;
    
    case 16:
      if (alpha) {
        mask.r = 0xF << 8 ; mask.g = 0xF << 4; mask.b = 0xF;
        mask.a = 0xF << 12;
      } else {
      mask.r = 0xFF >> 3 << 10; mask.g = 0xFF >> 3 << 5; mask.b = 0xFF >> 3;
      }      
    break;
    
    case 24:
    case 32:
    default:
      mask.r = 0xFF << 16; mask.g = 0xFF << 8; mask.b = 0xFF;
      mask.a = 0xFF << 24; 
    break;
  }
  
  return mask;
} 



/** Makes an empty gari image with the same bits per pixels as the screen, 
    but supporting either alpha, or not, or a colorkey depending on mode.
    The video mode must have been set and the screen must have been opened. 
*/
GariImage * gari_image_makedepth(int w, int h, int depth, 
                                 int mode, GariColor colorkey) {
  GariImage * result;
  GariRGBA mask;
  SDL_Surface * video, * aid;
  Uint32 rmask, gmask, bmask, amask;
  int bits;
  Uint32 flags;
  video = SDL_GetVideoSurface();
  if(!video) return NULL;  
  mask      = gari_image_masks(depth, mode == GariImageAlpha);
  rmask     = mask.r;
  gmask     = mask.g;
  bmask     = mask.b;
  amask     = mask.a;
        
/*  
#if SDL_BYTEORDER == SDL_BIG_ENDIAN
  rmask = 0xff000000;
  gmask = 0x00ff0000;
  bmask = 0x0000ff00;
#else
  rmask = 0x000000ff;
  gmask = 0x0000ff00;
  bmask = 0x00ff0000;  
#endif
*/

 flags =  SDL_SWSURFACE;
  switch (mode) {
    case GariImageAlpha:
      aid = SDL_CreateRGBSurface(SDL_SWSURFACE | SDL_SRCALPHA, w, h, bits, 
                                 rmask, gmask, bmask, amask);
      if (aid) {
        SDL_SetAlpha(aid, SDL_SRCALPHA, SDL_ALPHA_OPAQUE);
      }
      break;
    case GariImageColorkey:      
      aid = SDL_CreateRGBSurface(SDL_SWSURFACE | SDL_SRCCOLORKEY, w, h, bits, 
                                 rmask, gmask, bmask, amask);
      if (aid) {
        SDL_SetColorKey(aid, SDL_RLEACCEL | SDL_SRCCOLORKEY, colorkey);
      }
      break;
      
    case GariImageSolid:
      aid = SDL_CreateRGBSurface(SDL_SWSURFACE | SDL_SRCALPHA, w, h, bits, 
                                 rmask, gmask, bmask, amask);
      break;                           
    default:  
      return NULL;
  }
  
  return gari_image_wrap(aid);
} 









