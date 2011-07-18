#include "gy.h"

#define GY_INTERN_ONLY
#include "gy_intern.h"


GyImage * gyimage_wrap(SDL_Surface * surface) {
  return (GyImage *)(surface);
}

void gyimage_free(GyImage * image) {
  if (!image) {return;} 
  SDL_FreeSurface(GY_IMAGE_SURFACE(image));
}


GyDye gyimage_maprgb(GyImage *img, GyColor rgba) {
  return  gysurface_maprgb(GY_IMAGE_SURFACE(img), rgba);
}

GyDye gyimage_maprgba(GyImage *img, GyColor rgba) {
  return  gysurface_maprgba(GY_IMAGE_SURFACE(img), rgba);
}

GyColor gyimage_getrgb(GyImage *img, GyDye color) {
  return gysurface_getrgb(GY_IMAGE_SURFACE(img), color);
}

GyColor gyimage_getrgba(GyImage *img, GyDye color) {
  return gysurface_getrgba(GY_IMAGE_SURFACE(img), color);
}

GyDye gyimage_rgb(GyImage *img, uint8_t r, uint8_t g, uint8_t b) {
  return SDL_MapRGB(GY_IMAGE_FORMAT(img), r, g, b); 
}

GyDye gyimage_rgba(GyImage *img, 
          uint8_t r, uint8_t g, uint8_t b, uint8_t a) {
  return  SDL_MapRGBA(GY_IMAGE_FORMAT(img), r, g, b, a); 
}


/** Optimizes the image for drawing to the screen, possibly applying a color
key for transparency as well. */
GyImage * gyimage_optimize(GyImage * image, int mode, GyDye colorkey) {
  SDL_Surface * surface;
  surface = gyimage_surface(image);
  if (!SDL_GetVideoSurface()) {
    return NULL; 
    // SDL_DisplayFormat won't work unless there is an active video surface.
  };
  
  switch (mode) {
    case GyImageAlpha:  
      return gyimage_wrap(SDL_DisplayFormatAlpha(surface));
      
    case GyImageColorkey:
      SDL_SetColorKey(surface, SDL_SRCCOLORKEY | SDL_RLEACCEL, colorkey);
      return gyimage_wrap(SDL_DisplayFormat(surface));
    case GyImageSolid:
    default:  
      return gyimage_wrap(SDL_DisplayFormat(surface));
  }

}

/** Loads the image from the named file. Supports all formats 
SDL_image supports. */
GyImage * gyimage_loadraw(char * filename) {
  GyImage * result;
  result = gyimage_wrap(IMG_Load(filename));
  return result;
}

/** Loads the image from the named file, and optimizes it for display
as a solid image on the active video surface, which must already have been 
opened. If the acceleration fails, this returns the unaccellerated image. 
Return NULL if the file could not be read. */
GyImage * gyimage_loadsolid(char * filename) {
  GyImage * normal, * faster;
  normal = gyimage_loadraw(filename);
  if (!normal) { return NULL ;  }
  faster = gyimage_optimize(normal, GyImageSolid, 0);
  if (!faster) { return normal; }
  return faster;
}




/** Loads the image from the named file, and optimizes it for display 
on the active video sorface if it was already set. */
/*
GyImage * gyimage_load(char * filename) {
  GyImage * normal, * faster;
  normal = gyimage_loadraw(filename);
  if (!normal) { return NULL ; }
  faster = gyimage_optimize(   
}
*/

/** Blits one image to another one with the given coordinates. */
void gyimage_blit(GyImage * dst, int dstx, int dsty, GyImage * src) {
  SDL_Surface *sdlsrc, *sdldst;
  SDL_Rect    dstrect;
  dstrect.x = dstx;
  dstrect.y = dsty;
  sdlsrc    = GY_IMAGE_SURFACE(src);
  sdldst    = GY_IMAGE_SURFACE(dst);
  SDL_BlitSurface(sdlsrc, NULL, sdldst, &dstrect);  
}

/** Blits a part of one image to another one with the given coordinates. Takes the indicated parts of the image, as specified by srcx, srcy, srcw and srch  */
void gyimage_blitpart(GyImage * dst, int dstx, int dsty, GyImage * src,
int srcx, int srcy, int srcw, int srch) {
  SDL_Surface *sdlsrc, *sdldst;
  SDL_Rect    dstrect, srcrect;
  dstrect.x = dstx;  dstrect.y = dsty;
  srcrect.x = srcx;  srcrect.y = srcy;  srcrect.w = srcw;  srcrect.h = srch;  
  sdlsrc    = GY_IMAGE_SURFACE(src);
  sdldst    = GY_IMAGE_SURFACE(dst);
  SDL_BlitSurface(sdlsrc, &srcrect, sdldst, &dstrect);
}


SDL_Surface * sdl_image_copypart(SDL_Surface * src, int x, int y, int w, int h) {
  SDL_Surface * result = NULL;
  SDL_Rect    dstrect, srcrect;
  SDL_PixelFormat * format = NULL;
  Uint32 flagbase = SDL_SWSURFACE|SDL_HWSURFACE|SDL_SRCCOLORKEY|SDL_SRCALPHA;
  Uint32 aflags   = 0;
  Uint32 myflags  = 0;
  if(!src) return NULL;
  format = src->format;
  if(!format) return NULL;
  dstrect.x = 0;  dstrect.y = 0;
  srcrect.x = x;  srcrect.y = y;  srcrect.w = w;  srcrect.h = h;
  // store alpha flags
  aflags    = src->flags & (SDL_SRCCOLORKEY | SDL_RLEACCEL);
  SDL_SetAlpha(src, 0, format->alpha); // disable alpha on src
  myflags   = flagbase & src->flags;
  result    = SDL_CreateRGBSurface(myflags, w, h, format->BitsPerPixel,
                                   format->Rmask, format->Gmask ,format->Bmask,
                                   format->Amask);
  
  SDL_SetAlpha(src, aflags, format->alpha); // re-enable alpha on src
  if(!result) return NULL;
  // copy pixels
  SDL_BlitSurface(src, &srcrect, result, &dstrect);
  // copy alpha flagbase and color key
  SDL_SetColorKey(result, src->flags & (SDL_SRCCOLORKEY|SDL_RLEACCEL), 
                  format->colorkey);
  SDL_SetAlpha(result, src->flags & (SDL_SRCALPHA|SDL_RLEACCEL),
                  format->alpha);
  return result;
}

/** Copies a part of the source image and returns a new destination image,
or nil it it ran out of memory or otherwise was incorrect. */
GyImage * gyimage_copypart(GyImage *src, int x, int y, int w, int h) {
  SDL_Surface * surf = gyimage_surface(src);
  SDL_Surface * res  = sdl_image_copypart(surf, x, y, w, h);
  return gyimage_wrap(res);
}


/** Copys exactly one pixel from src to dst using SDL_BlitSurface. 
Hence, neither src nor dst may be locked according to SDL_BlitSurface's
documentation. */
void gyimage_copypixel(GyImage * dst, int dstx, int dsty, 
                                 GyImage * src, int srcx, int srcy) {
  SDL_Surface *sdlsrc, *sdldst;
  SDL_Rect    dstrect, srcrect;
  dstrect.x = dstx;
  dstrect.y = dsty;
  srcrect.x = srcx;
  srcrect.y = srcy;
  srcrect.w = 1;
  srcrect.h = 1;  
  sdlsrc    = GY_IMAGE_SURFACE(src);
  sdldst    = GY_IMAGE_SURFACE(dst);
  SDL_BlitSurface(sdlsrc, &srcrect, sdldst, &dstrect);  
}


/* Make new images for various colour depths. */

GyImage * gyimage_make8(int w, int h, int mode) {
    SDL_Surface* result = NULL;
    result = SDL_CreateRGBSurface(SDL_SWSURFACE, w, h, 8, 3 << 5, 3 << 2, 3, 0);    
    return gyimage_wrap(result);
}

GyImage * gyimage_make12(int w, int h, int mode) {
    SDL_Surface* result = NULL;
    result = SDL_CreateRGBSurface(SDL_SWSURFACE, w, h, 12, 15 << 8, 15 << 4, 15, 0);    
    return gyimage_wrap(result);
}

GyImage * gyimage_make15(int w, int h, int mode) {
    SDL_Surface* result = NULL;
    result = SDL_CreateRGBSurface(SDL_SWSURFACE, w, h, 15, 3 << 5, 3 << 2, 3, 0);    
    return gyimage_wrap(result);
}


GyImage * gyimage_make16(int w, int h, int mode) {
    SDL_Surface* result = NULL;
    if (mode == GyImageAlpha) {
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
    return gyimage_wrap(result);
}


GyImage * gyimage_make24(int w, int h, int mode) {
    SDL_Surface* result = NULL;
    #if SDL_BYTEORDER == SDL_BIG_ENDIAN
      result = SDL_CreateRGBSurface(SDL_SWSURFACE, w, h, 24, 0xff0000, 0x00ff00, 0x0000ff, 0);
    #else
      result = SDL_CreateRGBSurface(SDL_SWSURFACE, w, h, 24, 0x0000ff, 0x00ff00, 0xff0000, 0);
    #endif
    // SDL_SetAlpha(result, 0, SDL_ALPHA_OPAQUE);
    return gyimage_wrap(result);
}

GyImage * gyimage_make32(int w, int h, int mode) {
    SDL_Surface* result = NULL;
    #if SDL_BYTEORDER == SDL_BIG_ENDIAN
      result = SDL_CreateRGBSurface(SDL_SWSURFACE, w, h, 32, 0xff000000, 
				    0x00ff0000, 0x0000ff00, 0x000000ff);
    #else
      result = SDL_CreateRGBSurface(SDL_SWSURFACE, w, h, 32, 
					0x000000ff, 0x0000ff00, 0x00ff0000, 0xff000000);
    #endif
    // SDL_SetAlpha(result, 0, SDL_ALPHA_OPAQUE);
    return gyimage_wrap(result);
}


/** Makes an empty gy image with the given color depth and alpha mode. 
The video mode must have been set and the screen must have been opened. 
*/
GyImage * gyimage_makedepth(int w, int h, int depth, int mode) {
  GyImage * result;
  SDL_Surface * aid;
  switch(depth) {
    case 8: return gyimage_make8(w, h, mode);
    case 12: return gyimage_make12(w, h, mode);
    case 15: return gyimage_make15(w, h, mode);
    case 16: return gyimage_make16(w, h, mode);
    case 24: return gyimage_make24(w, h, mode);
    case 32: 
    default:   
	    return gyimage_make32(w, h, mode);
  }
} 


/** Transforms a GyDye from one kind of image to another. */
GyDye gyimage_mapcolor(GyImage * dst, 
                              GyImage * src, GyDye color) {
  GyColor rgba;
  if (GY_IMAGE_FORMAT(dst)  == GY_IMAGE_FORMAT(src)) {
    return color; // no mappping if format identical.
  } 
  rgba = gyimage_getrgba(src, color);
  return gyimage_maprgba(dst, rgba);
}

/** Sets the clipping rectangle of the image to the given parameters. */
GyImage * gyimage_setclip(GyImage * img, int x, int y, 
  int w, int h) {
  SDL_Surface * surf = gyimage_surface(img);
  SDL_Rect     rect  = { 0, 0, 0, 0 } ;
  if(!surf) return NULL;
  
  rect.x = x;
  rect.y = y;
  rect.w = w;
  rect.h = h;
  SDL_SetClipRect(surf, &rect);  
  return img; 
} 

/* Gets the clipping array parameters. */
GyImage * gyimage_getclip(GyImage * img, 
  int *x, int *y, int *w, int *h) {
  SDL_Surface * surf = gyimage_surface(img);
  SDL_Rect      rect = { 0, 0, 0, 0 } ;
  if(!surf || !x || !y ||!w || !h) return NULL;
  SDL_GetClipRect(surf, &rect);
  (*x) = rect.x;
  (*y) = rect.y;
  (*w) = rect.w;
  (*h) = rect.h;
  return img;
}


/** Gets the color depth in bits per pixels of this image.
* Returns -1 on error.
*/
int gyimage_depth(GyImage * img) {
  SDL_Surface * surf = gyimage_surface(img);
  if(!surf)         return -1;  
  if(!surf->format) return -1;
  return surf->format->BitsPerPixel;
}


GyImage * gyimage2image_convert(GyImage * srci, 
                               GyImage * dsti, Uint32 flags) {
  SDL_Surface * src = gyimage_surface(srci);
  SDL_Surface * dst = gyimage_surface(dsti);
  if((!src) || (!dst)) return NULL;
  return gyimage_wrap(SDL_ConvertSurface(src, dst->format, flags));
}


/** Returns true if any pixel of the image has any per-pixel transparency, 
lower than amin 
false if not. */
int gyimage_hasalpha_p(GyImage * img, unsigned char amin) {
  GyDye dye;
  GyColor color;
  int x, y, w, h;
  int result = FALSE;
  SDL_Surface * surf = gyimage_surface(img);
  if (!surf->flags & SDL_SRCALPHA) return FALSE;
  w = gyimage_w(img);
  h = gyimage_h(img);
  gyimage_lock(img);
  for (y = 0; y < h; x++) {
    for (x = 0; x < w; y++) {
      dye   = gyimage_getpixel_nolock(img, x, y);
      color = gydye_color(dye, img);
      if (color.a < amin) {
        result = TRUE;
        break;
      }
    }
  }
  gyimage_unlock(img);
  return result;
} 

