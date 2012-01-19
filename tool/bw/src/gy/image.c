#include "gy.h"

#define GY_INTERN_ONLY
#include "gy_intern.h"

/** We lie to the compiler about GyImage. We say it's a struct, but just
* cast a pointer from SDL_Surface to a pointer to GyImage and 
* prevent the overhead. 
*/

GyImage * gyimage_wrap(SDL_Surface * surface) {
  return (GyImage *)(surface);
}

void gyimage_free(GyImage * image) {
  if (!image) {return;} 
  SDL_FreeSurface(GY_IMAGE_SURFACE(image));
}

/** Returns the per surface alpha of the SDL_Surface. Returns 0 if the image is NULL; */
uint8_t gysurface_alpha(SDL_Surface * surf) {
#if SDL_VERSION_ATLEAST(1, 3, 0)
   Uint8 alpha = 0;
   if(!surf) return 0;
   SDL_GetSurfaceAlphaMod(surf, &alpha);
   return alpha;
#else // SDL < 1.3.0
   if(!surf) return 0;
   return surf->format->alpha;
#endif 
}

/** Returns the color key of the surface. Return 0 if the image is NULL.*/
GyDye gysurface_colorkey(SDL_Surface * surf) {
#if SDL_VERSION_ATLEAST(1, 3, 0)
   GyDye colorkey = 0;
   if(!surf) return 0;
   SDL_GetColorKey(surf, &colorkey);
   return colorkey;
#else
   if(!surf) return 0;
   return surf->format->colorkey;
#endif 
}


/** Returns the per surface alpha of the image. Returns 0 if the image is NULL; */
uint8_t gyimage_alpha(GyImage * image) {
  SDL_Surface * surface = gyimage_surface(image);
  return gysurface_alpha(surface);
}

/** Returns the color key of the image. Return 0 if the image is NULL.*/
GyDye gyimage_colorkey(GyImage * image) {
  SDL_Surface * surface = gyimage_surface(image);
  return gysurface_colorkey(surface);
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
  SDL_Surface * surf;
  GyImage * result;
  surf = IMG_Load(filename);
  if(!surf) {
    printf("Image load failed: %s: %s\n", filename, IMG_GetError());
    return NULL;
  }
  result = gyimage_wrap(surf);
  return result;
}


/** Loads the image in filename, and then ptimizes the image for drawing to
* the screen, possibly applying a color key for transparency as well.
* Returns the unaccellerated image if optimization failed. This could 
* happen if the screen is not open yet.
*/
GyImage * gyimage_loadoptimize(char * filename, int mode, GyDye colorkey) {
  GyImage * normal, * faster;
  normal = gyimage_loadraw(filename);
  if (!normal) { return NULL ;  }
  faster = gyimage_optimize(normal, mode, colorkey);
  if (!faster) { return normal; }
  return faster;
}


/** Loads the image from the named file, and optimizes it for display
as a solid image on the active video surface, which must already have been 
opened. If the acceleration fails, this returns the unaccellerated image. 
Return NULL if the file could not be read. */
GyImage * gyimage_loadsolid(char * filename) {
  return gyimage_loadoptimize(filename, GyImageSolid, 0);
}

/** Loads the image from the named file, and optimizes it for display
with an alpha channel. If the acceleration fails, this returns the unaccellerated image. Return NULL if the file could not be read. */
GyImage * gyimage_loadalpha(char * filename) {
  return gyimage_loadoptimize(filename, GyImageAlpha, 0);
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
  SDL_SetAlpha(src, 0, gysurface_alpha(src)); // disable alpha on src
  myflags   = flagbase & src->flags;
  result    = SDL_CreateRGBSurface(myflags, w, h, format->BitsPerPixel,
                                   format->Rmask, format->Gmask ,format->Bmask,
                                   format->Amask);
  
  SDL_SetAlpha(src, aflags, gysurface_alpha(src)); // re-enable alpha on src
  if(!result) return NULL;
  // copy pixels
  SDL_BlitSurface(src, &srcrect, result, &dstrect);
  // copy alpha flagbase and color key
  SDL_SetColorKey(result, src->flags & (SDL_SRCCOLORKEY|SDL_RLEACCEL), 
                  gysurface_colorkey(src));
  SDL_SetAlpha(result, src->flags & (SDL_SRCALPHA|SDL_RLEACCEL),
                  gysurface_alpha(src));
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
    #if SDL_VERSION_ATLEAST(1,3,0)  
      result =     /*Using the default masks for the depth: */
               SDL_CreateRGBSurface(SDL_SWSURFACE, w, h, 16, 0, 0, 0, 0);
     // 4 - 4 - 4 - 4 
    #elif SDL_BYTEORDER == SDL_BIG_ENDIAN
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


/** Applies an affine transformation to the image and blits it to the target
* at the given coordinates. 
*/
int gyimage_blitaffine(GyImage * dst, GyImage * src, int x, int y, 
GyAffine * affine) {
  int srcx, srcy, dstx, dsty;
  int w = gyimage_w(src);
  int h = gyimage_h(src);
  gyimage_lock(dst);  
  gyimage_lock(src);  
  for (dsty = y; dsty < (x + h) ; dsty++) {
    for (dstx = x; dstx < (x + w) ; dstx ++) {
       gyaffine_iapply(affine, dstx, dsty, &srcx, &srcy);
       gyimage_copypixel_nolock(dst, dstx, dsty, src, srcx, srcy);
    }
  }
  gyimage_unlock(dst);
  gyimage_unlock(src);
} 



#ifdef COMMENT

/** Applies an affine transformation and functions to the SDL_Surface src
and draws it to the surface dst taking the options into account. Based
on code in Starruby by Hajime Hoshii. 
*/

/**
Copyright (c) 2007-2008 Hajime Hoshi, Daigo Sato

Permission is hereby granted, free of charge, to any person
obtaining a copy of this software and associated documentation
files (the "Software"), to deal in the Software without
restriction, including without limitation the rights to use,
copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the
Software is furnished to do so, subject to the following
conditions:

The above copyright notice and this permission notice shall be
included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
OTHER DEALINGS IN THE SOFTWARE.
*/



typedef struct SDL_PointI {
  int x, y, z;
} SDL_PointI ;

typedef struct SDL_PointF {
  double x, y, z;
} SDL_PointF;

typedef SDL_PointI SDL_Vector;
typedef SDL_PointF SDL_VectorF;

typedef struct SDL_PerspectiveOptions {
  int cameraX;
  int cameraY;
  double cameraHeight;
  double cameraYaw;
  double cameraPitch;
  double cameraRoll;
  double viewAngle;
  int intersectionX;
  int intersectionY;
  int isLoop;
  // BlurType blurType;
  // Color blurColor;
} SDL_PerspectiveOptions;


#define MAX(x, y) (((x) >= (y)) ? (x) : (y))
#define MIN(x, y) (((x) <= (y)) ? (x) : (y))
#define DIV255(x) ((x) / 255)
#define ALPHA(src, dst, a) DIV255((dst << 8) - dst + (src - dst) * a)

/*
 * Return the pixel value at (x, y)
 * NOTE: The surface must be locked before calling this!
 */
Uint32 SDL_slow_getpixel(SDL_Surface *surface, int x, int y)
{
    int bpp = surface->format->BytesPerPixel;
    /* Here p is the address to the pixel we want to retrieve */
    Uint8 *p = (Uint8 *)surface->pixels + y * surface->pitch + x * bpp;

    switch(bpp) {
      case 1:
          return *p;
  
      case 2:
          return *(Uint16 *)p;
  
      case 3:
          if(SDL_BYTEORDER == SDL_BIG_ENDIAN)
              return p[0] << 16 | p[1] << 8 | p[2];
          else
              return p[0] | p[1] << 8 | p[2] << 16;
  
      case 4:
          return *(Uint32 *)p;
  
      default:
          return 0;       /* shouldn't happen, but avoids warnings */
    }
}

/*
 * Set the pixel at (x, y) to the given value
 * NOTE: The surface must be locked before calling this!
 */
void SDL_slow_putpixel(SDL_Surface *surface, int x, int y, Uint32 pixel)
{
    int bpp = surface->format->BytesPerPixel;
    /* Here p is the address to the pixel we want to set */
    Uint8 *p = (Uint8 *)surface->pixels + y * surface->pitch + x * bpp;

    switch(bpp) {
    case 1:
        *p = pixel;
        break;

    case 2:
        *(Uint16 *)p = pixel;
        break;

    case 3:
        if(SDL_BYTEORDER == SDL_BIG_ENDIAN) {
            p[0] = (pixel >> 16) & 0xff;
            p[1] = (pixel >> 8) & 0xff;
            p[2] = pixel & 0xff;
        } else {
            p[0] = pixel & 0xff;
            p[1] = (pixel >> 8) & 0xff;
            p[2] = (pixel >> 16) & 0xff;
        }
        break;

    case 4:
        *(Uint32 *)p = pixel;
        break;
    }
}

void SDL_SoftRenderBlend(SDL_Surface * dst, SDL_Surface * src, 
                         int srcx, int srcy, int dstx, int dsty,
                         const SDL_TextureRenderingOptions* options) {
  Uint8  srcR, srcG, srcB, srcA;
  Uint32 modR = options->toneRed, modG = options->toneGreen, 
         modB = options->toneBlue;
  Uint32 colR, colG, colB;          
  Uint8  dstR, dstG, dstB, dstA;
  Uint16 totalAlpha, otherAlpha;  
  Uint8  alpha = options->alpha;
  Uint32 dstpixel;
  Uint32 srcpixel = SDL_slow_getpixel(src, srcx, srcy);
  SDL_GetRGBA(srcpixel, src->format, &srcR, &srcG, &srcB, &srcA);
  /** Modify colors based on toning. */
  colR = (srcR * modR) / 255;
  colG = (srcG * modG) / 255;
  colB = (srcB * modB) / 255;

  if(options->blendmode == SDL_BLENDMODE_NONE) { // no blending  
      dstR = (Uint8) colR;
      dstG = (Uint8) colG;
      dstB = (Uint8) colB;
  } else {     
      Uint32 dstpixel = SDL_slow_getpixel(dst, dstx, dsty);
      SDL_GetRGBA(dstpixel, dst->format, &dstR, &dstG, &dstB, &dstA);
      totalAlpha      = (alpha * srcA) / 255;
      otherAlpha      = 255 - totalAlpha;
      
    switch (options->blendmode) {  
      case SDL_BLENDMODE_BLEND: // (src * A) + (dst * (1-A))
      dstR = (Uint8) (( colR * totalAlpha ) + (dstR * (otherAlpha)) / 255);
      dstG = (Uint8) (( colG * totalAlpha ) + (dstG * (otherAlpha)) / 255);
      dstB = (Uint8) (( colB * totalAlpha ) + (dstB * (otherAlpha)) / 255);
      dstA = (Uint8) (( srcA * totalAlpha ) + (dstA * (otherAlpha)) / 255);
      break; 
      case SDL_BLENDMODE_ADD: // dst = (src * A) + dst
      dstR = (Uint8) (( colR * totalAlpha ) / 255 + dstR);
      dstG = (Uint8) (( colG * totalAlpha ) / 255 + dstG);
      dstB = (Uint8) (( colB * totalAlpha ) / 255 + dstB);
      dstA = (Uint8) (( srcA * totalAlpha ) / 255 + dstA);
      break;
      case SDL_BLENDMODE_MOD: 
      dstR = (Uint8) (( colR * dstR ) / 255);
      dstG = (Uint8) (( colG * dstG ) / 255);
      dstB = (Uint8) (( colB * dstB ) / 255);
      dstA = (Uint8) (( srcA * dstA ) / 255);
      break;
      default:
      break;
    }
  }
  dstpixel = SDL_MapRGBA(dst->format, dstR, dstG, dstB, dstA);
  SDL_slow_putpixel(dst, dstx, dsty, dstpixel);
}



void SDL_SoftRender(SDL_Surface * dst , SDL_Surface *src, 
                   SDL_Rect * dstrect, SDL_Rect * srcrect,
                   const SDL_TextureRenderingOptions* options) {  
  const double angle  = options->angle;
  const int centerX   = options->centerX;
  const int centerY   = options->centerY;
  int srcX            = srcrect->x;
  int srcY            = srcrect->y;
  int srcHeight       = srcrect->h;
  int srcWidth        = srcrect->w;
  int dstX            = dstrect->x;
  int dstY            = dstrect->y;
  int dstHeight       = dstrect->h;
  int dstWidth        = dstrect->w;
  int i, j;
  const double scaleX = (double)dstrect->w / (double)srcrect->w;
  const double scaleY = (double)dstrect->h / (double)srcrect->h;  
  SDL_AffineMatrix mat;
  SDL_LockSurface(src); 
  SDL_LockSurface(dst);
  mat.a  = options->matrix.a;
  mat.b  = options->matrix.b;
  mat.c  = options->matrix.c;
  mat.d  = options->matrix.d;
  mat.tx = options->matrix.a * (options->matrix.tx - centerX)
    + options->matrix.b * (options->matrix.ty - centerY);
  mat.ty = options->matrix.c * (options->matrix.tx - centerX)
    + options->matrix.d * (options->matrix.ty - centerY);
  if (dstrect->w != srcrect->w) {
    mat.a  *= scaleX;
    mat.b  *= scaleX;
    mat.tx *= scaleX;
  }
  if (dstrect->h != srcrect->h) {
    mat.c  *= scaleY;
    mat.d  *= scaleY;
    mat.ty *= scaleY;
  }
  if (angle != 0) {
    const double a  = mat.a;
    const double b  = mat.b;
    const double c  = mat.c;
    const double d  = mat.d;
    const double tx = mat.tx;
    const double ty = mat.ty;
    const double cosAngle = cos(angle);
    const double sinAngle = sin(angle);
    mat.a  = cosAngle * a  - sinAngle * c;
    mat.b  = cosAngle * b  - sinAngle * d;
    mat.c  = sinAngle * a  + cosAngle * c;
    mat.d  = sinAngle * b  + cosAngle * d;
    mat.tx = cosAngle * tx - sinAngle * ty;
    mat.ty = sinAngle * tx + cosAngle * ty;
  }
  mat.tx += centerX + dstX;
  mat.ty += centerY + dstY;
  const double det = mat.a * mat.d - mat.b * mat.c;
  if (det == 0) {
    SDL_UnlockSurface(src); 
    SDL_UnlockSurface(dst);
    return;
  }
  const double dstX00 = mat.tx;
  const double dstY00 = mat.ty;
  const double dstX01 = mat.b * srcHeight + mat.tx;
  const double dstY01 = mat.d * srcHeight + mat.ty;
  const double dstX10 = mat.a * srcWidth  + mat.tx;
  const double dstY10 = mat.c * srcWidth  + mat.ty;
  const double dstX11 = mat.a * srcWidth  + mat.b * srcHeight + mat.tx;
  const double dstY11 = mat.c * srcWidth  + mat.d * srcHeight + mat.ty;
  double dstX0 = MIN(MIN(MIN(dstX00, dstX01), dstX10), dstX11);
  double dstY0 = MIN(MIN(MIN(dstY00, dstY01), dstY10), dstY11);
  double dstX1 = MAX(MAX(MAX(dstX00, dstX01), dstX10), dstX11);
  double dstY1 = MAX(MAX(MAX(dstY00, dstY01), dstY10), dstY11);
  const int dstTextureWidth  = dst->w;
  const int dstTextureHeight = dst->h;
  if (dstTextureWidth <= dstX0 || dstTextureHeight <= dstY0 ||
      dstX1 < 0 || dstY1 < 0) {
    return;
  }
  SDL_AffineMatrix matInv = {
    .a = mat.d  / det,
    .b = -mat.b / det,
    .c = -mat.c / det,
    .d = mat.a  / det,
  };
  matInv.tx = -(matInv.a * mat.tx + matInv.b * mat.ty);
  matInv.ty = -(matInv.c * mat.tx + matInv.d * mat.ty);
  double srcOX = matInv.a * (dstX0 + 0.5) + matInv.b * (dstY0 + 0.5)
    + matInv.tx + srcX;
  double srcOY = matInv.c * (dstX0 + 0.5) + matInv.d * (dstY0 + 0.5)
    + matInv.ty + srcY;
  double srcDXX = matInv.a;
  double srcDXY = matInv.c;
  double srcDYX = matInv.b;
  double srcDYY = matInv.d;

  if (dstX0 < 0) {
    srcOX -= dstX0 * srcDXX;
    srcOY -= dstX0 * srcDXY;
    dstX0 = 0;
  }
  if (dstY0 < 0) {
    srcOX -= dstY0 * srcDYX;
    srcOY -= dstY0 * srcDYY;
    dstY0 = 0;
  }
  const int dstX0Int = (int)dstX0;
  const int dstY0Int = (int)dstY0;
  dstWidth  = MIN(dstTextureWidth,  (int)dstX1) - dstX0Int;
  dstHeight = MIN(dstTextureHeight, (int)dstY1) - dstY0Int;

  const Uint32 srcOX16  = (Uint32)(srcOX  * (1 << 16));
  const Uint32 srcOY16  = (Uint32)(srcOY  * (1 << 16));
  const Uint32 srcDXX16 = (Uint32)(srcDXX * (1 << 16));
  const Uint32 srcDXY16 = (Uint32)(srcDXY * (1 << 16));
  const Uint32 srcDYX16 = (Uint32)(srcDYX * (1 << 16));
  const Uint32 srcDYY16 = (Uint32)(srcDYY * (1 << 16));
  
  if (src == dst) return -1;
  /** The case of src and dst being equal is omitted and not handled here. */

  const int srcX2               = srcX + srcWidth;
  const int srcY2               = srcY + srcHeight;
  const int srcTextureWidth     = src->w;
  for (j = 0; j < dstHeight; j++) {
    Uint32 srcI16 = srcOX16 + j * srcDYX16;
    Uint32 srcJ16 = srcOY16 + j * srcDYY16;
    for (i = 0; i < dstWidth;
         i++, dst++, srcI16 += srcDXX16, srcJ16 += srcDXY16) {
      const Uint32 srcI = srcI16 >> 16;
      const Uint32 srcJ = srcJ16 >> 16;
      if (srcX <= srcI && srcI < srcX2 && srcY <= srcJ && srcJ < srcY2) {
        SDL_SoftRenderBlend(dst, src, srcI, srcJ, i, j, options);
      } else if ((srcI < srcX && srcDXX <= 0) ||
                 (srcX2 <= srcI && 0 <= srcDXX) ||
                 (srcJ < srcY && srcDXY <= 0) ||
                 (srcY2 <= srcJ && 0 <= srcDXY)) {
        break;
      }
    }
  }
  SDL_UnlockSurface(src); 
  SDL_UnlockSurface(dst);

}

#endif







