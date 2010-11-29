/* Single pixel related functions (put, get, blend) */

#include "gari.h"

#define GARI_INTERN_ONLY
#include "gari_intern.h"

/**
Gari drawing functions.
*/

SDL_Surface * gari_image_surface(GariImage * img) {
  return GARI_IMAGE_SURFACE(img);
  // return ((SDL_Surface*)(img));
}

int gari_image_w(GariImage * img) {
  return GARI_IMAGE_W(img);
  // return (gari_image_surface(img))->w;
}

int gari_image_h(GariImage * img) {
  return GARI_IMAGE_H(img);
  // return (gari_image_surface(img))->h;
}

// Returns the amount of byrtes per pixel for this image's format (1,2,3,4)
int gari_image_bytesperpixel(GariImage * img) {
  SDL_Surface * surface = gari_image_surface(img);
  if(!img)             return 0;
  if(!surface)         return 0; 
  if(!surface->format) return 0;
  return surface->format->BytesPerPixel;
}

void gari_surface_lock(SDL_Surface * surface) {
  (void)GARI_SURFACE_LOCK(surface);
  // SDL_MUSTLOCK(surface) && SDL_LockSurface(surface);
  // use boolean short cut  
}

void gari_surface_unlock(SDL_Surface * surface) {
  GARI_SURFACE_UNLOCK(surface);
  // SDL_MUSTLOCK(surface) && (SDL_UnlockSurface(surface), 0)
  // use boolean short cut  
}


void gari_image_lock(GariImage * image) {
  (void)GARI_SURFACE_LOCK(GARI_IMAGE_SURFACE(image));
}

void gari_image_unlock(GariImage * image) {
  (void)GARI_SURFACE_UNLOCK(GARI_IMAGE_SURFACE(image));
}



// Returns nonzero if the pixel is outside the image boundaries
// and may /not/ be drawn to or read from using the PutPixelXXX
// GetPixelXXX and BlendPixelXXX series of functions.
// Returns FALSE if the pixel is safe to draw to.
// This may seeem conterintuitive, but normal use will be
// if surf.PixelOutside(x, y) { return }    
int gari_image_pixeloutside(GariImage * image, int x, int y) {
  return GARI_IMAGE_PIXELOUTSIDE(image, x, y);
  /*
  if (x < 0 || y < 0)            { return TRUE; } 
  if (x >= gari_image_w(image))  { return TRUE; }
  if (y >= gari_image_h(image))  { return TRUE; }
  return FALSE;
  */
}

// Returns TRUE if the pixel lies outside the clipping rectangle
// returns FALSE if not so and it may be drawn
int gari_image_pixelclip(GariImage * image, int x, int y) {
  return GARI_IMAGE_PIXELCLIP(image, x, y);  
}

// Returns true if the pixel lies outside of the image's boundaries or 
// clipping rectangle.
int gari_image_pixeloutorclip(GariImage * image, int x, int y) {
  if (GARI_IMAGE_PIXELOUTSIDE(image, x, y)) { return TRUE; } 
  return GARI_IMAGE_PIXELCLIP(image, x, y);
}



/** Generic pointer type suitable for pointer arithmetic. */
typedef uint8_t * GariPtr;

// Helpers for the drawing primitives. They return a pointer to the 
// location of the pixel

// Basically, the pixel is always located at  
// uintptr((y*(int(surface.pitch)) + x * bytesperpixel)) offset
// from the surface.pixels pointer
// only the case of 24 bits is more complicated 
// See http://www.libsdl.org/cgi/docwiki.cgi/Pixel_Access
// for more reference. 

// Returns a pointer that points to the location of the pixel 
// at x and y for a surface s with bbp8
// Does no checks for validity of x and y and does no clipping!
uint8_t * gari_surface_pixelptr8(SDL_Surface * surface, int x, int y) {
  return GARI_SURFACE_PIXELPTR8(surface, x, y);
}

// Returns a pointer that points to the location of the pixel 
// at x and y for a surface s with bbp16
// Does no checks for validity of x and y and does no clipping!
uint16_t * gari_surface_pixelptr16(SDL_Surface * surface, int x, int y) {
  return GARI_SURFACE_PIXELPTR16(surface, x, y);
}

// Returns a pointer that points to the location of the pixel 
// at x and y for a surface s with bbp32
// Does no checks for validity of x and y and does no clipping!
uint32_t * gari_surface_pixelptr32(SDL_Surface * surface, int x, int y) {
  return GARI_SURFACE_PIXELPTR32(surface, x, y);
}

/* Struct to hold pointers for 24 bits case. */
struct PixelPtr_ {
  uint8_t * rptr;
  uint8_t * gptr;
  uint8_t * bptr;
  uint8_t * aptr;
};

typedef struct PixelPtr_ PixelPtr; 

// Returns a pointer that points to the location of the pixel 
// at x and y for a surface s with bbp24
// Does no checks for validity of x and y and does no clipping!
// Somewhat more complicated than the cases before
PixelPtr gari_surface_pixelptr24(SDL_Surface * surface, int x, int y) {
  PixelPtr res;
  SDL_PixelFormat *format = surface->format;
  GariPtr ptrbase = NULL;
  GariPtr pixels  = surface->pixels;
  size_t offset   = (((size_t)y) * surface->pitch)  + (((size_t)x) * 3);
  /* format          = surface->format; */
  ptrbase         = pixels + offset;
  res.rptr        = (uint8_t*)(ptrbase + (format->Rshift >> 3));  
  res.gptr        = (uint8_t*)(ptrbase + (format->Gshift >> 3));
  res.bptr        = (uint8_t*)(ptrbase + (format->Bshift >> 3));
  res.aptr        = (uint8_t*)(ptrbase + (format->Ashift >> 3)); 
  return res;
}


// Putpixel drawing primitives
// Each of them is optimized for speed in a different ituation .They 
// should be called only after locking the surface,
// They also do not do /any/ clipping or checking on x and y, so be 
// sure to check them with pixel_outsize.
// Puts a pixel to a surface with BPP 8   
void gari_surface_rpp8(SDL_Surface * surface, int x, int y, GariDye dye) {
  uint8_t * ptr = gari_surface_pixelptr8(surface, x, y); 
  *ptr          = (uint8_t)(dye);
}

// Puts a pixel to a surface with BPP 16
void gari_surface_rpp16(SDL_Surface * surface, int x, int y, GariDye dye) {
  uint16_t * ptr = gari_surface_pixelptr16(surface, x, y); 
  *ptr            = (uint16_t)(dye);
}

// Puts a pixel to a surface with BPP 32
void gari_surface_rpp32(SDL_Surface * surface, int x, int y, GariDye dye) {
  uint32_t * ptr  = gari_surface_pixelptr32(surface, x, y); 
  *ptr            = (uint32_t)(dye);
}

// Puts a pixel to a surface with BPP 24. Relatively slow!
void gari_surface_rpp24(SDL_Surface * surface, int x, int y, GariDye dye) {
  PixelPtr ptr    = gari_surface_pixelptr24(surface, x, y);
  SDL_PixelFormat *format = surface->format; 
  *(ptr.rptr)     = (uint8_t)(dye >> (GariDye)(format->Rshift));  
  *(ptr.gptr)     = (uint8_t)(dye >> (GariDye)(format->Gshift));
  *(ptr.bptr)     = (uint8_t)(dye >> (GariDye)(format->Bshift));
  *(ptr.aptr)     = (uint8_t)(dye >> (GariDye)(format->Ashift));  
}


// Puts a pixel depending on the BytesPerPixel of the target surface
// format. Still doesn't check the x and y coordinates for validity. 
void gari_surface_rppbpp(SDL_Surface * surface, int x, int y, GariDye dye) {
  switch (surface->format->BytesPerPixel) {
    case 1: gari_surface_rpp8(surface, x, y, dye); break; 
    case 2: gari_surface_rpp16(surface, x, y, dye); break;
    case 3: gari_surface_rpp24(surface, x, y, dye); break;
    case 4: gari_surface_rpp32(surface, x, y, dye); break;    
  }
}


// getpixel primitives (RawGetPixel)
// for 8 bit surfaces
GariDye gari_surface_rgp8(SDL_Surface * surface, int x, int y) {
  uint8_t * ptr = gari_surface_pixelptr8(surface, x, y); 
  return (GariDye)(*ptr);
}

GariDye gari_surface_rgp16(SDL_Surface * surface, int x, int y) {
  uint16_t * ptr = gari_surface_pixelptr16(surface, x, y); 
  return (GariDye)(*ptr);
}

GariDye gari_surface_rgp32(SDL_Surface * surface, int x, int y) {
  uint32_t * ptr = gari_surface_pixelptr32(surface, x, y); 
  return (GariDye)(*ptr);
}

// Gets a pixel from a surface with BPP 24. Relatively slow!
GariDye gari_surface_rgp24(SDL_Surface * surface, int x, int y) {
  PixelPtr ptr    = gari_surface_pixelptr24(surface, x, y);
  SDL_PixelFormat *format = surface->format; 
  GariDye dye = 0;
  dye   = (*ptr.rptr) << format->Rshift;
  dye  |= (*ptr.gptr) << format->Gshift;
  dye  |= (*ptr.bptr) << format->Bshift;
  dye  |= (*ptr.aptr) << format->Ashift;
  return dye;
}


// Puts a pixel depending on the BytesPerPixel of the target surface
// format. Still doesn't check the x and y coordinates for validity. 
GariDye gari_surface_rgpbpp(SDL_Surface * surface, int x, int y) {
  switch (surface->format->BytesPerPixel) {
    case 1: return gari_surface_rgp8(surface , x, y); break; 
    case 2: return gari_surface_rgp16(surface, x, y); break;
    case 3: return gari_surface_rgp24(surface, x, y); break;
    case 4: return gari_surface_rgp32(surface, x, y); break;    
  }
  return (GariDye)(0);
}

// Helps blending two dyes
GariDye gari_help_blend(GariDye old, GariDye dye,
  GariDye rmask, GariDye gmask, GariDye bmask,
  GariDye amask, GariAlpha alpha)  {
  GariDye oldr, oldb, oldg, olda, colr, colb, colg, cola, r, b, g , a;
  oldr = old & rmask;
  oldg = old & gmask;
  oldb = old & bmask;
  olda = old & amask;
  colr = dye & rmask;
  colg = dye & gmask;
  colb = dye & bmask;
  cola = dye & amask;
  // we add to every component
  //    (new - old) * (alpha / 256)  
  // or ((new - old) * alpha) >> 8)
  r    = (oldr + (((colr - oldr) * alpha) >> 8)) & rmask; 
  g    = (oldg + (((colg - oldg) * alpha) >> 8)) & gmask;
  b    = (oldb + (((colb - oldb) * alpha) >> 8)) & bmask;
  a    = 0;
  if (amask > 0)  {
    a    = (olda + (((cola - olda) * alpha) >> 8)) & amask;
  }
  return  r | g | b | a;
}

// Helps blending one component (r,g,b, or a) of a dye
uint8_t gari_blend_component(uint8_t oldc, uint8_t newc, GariAlpha alpha)  {
  return oldc + (((newc-oldc)*alpha) >> 8);
}




// Blends a pixel with the one already there to a surface with BPP 8
// taking into account the value of alpha
void gari_surface_rbp8(SDL_Surface * surface, int x, int y, 
                       GariDye dye, GariAlpha alpha) {    
  GariPtr   ptr   = gari_surface_pixelptr8(surface, x, y);
  GariDye oldcol  = (uint32_t)(*ptr);
  GariColor  old  = gari_surface_getrgb(surface, oldcol);
  // Messing with the palette would probably be faster, 
  // but it's cleaner to do it like this.
  GariColor col   = gari_surface_getrgb(surface, dye);
  GariColor nec;
  GariDye newcol;
  nec.r   = gari_blend_component(old.r, col.r, alpha);
  nec.g   = gari_blend_component(old.g, col.g, alpha);
  nec.b   = gari_blend_component(old.b, col.b, alpha);
  newcol  = gari_surface_maprgb(surface, nec);
  // And we have to do the same for the new color anyway, so... 
  *ptr    = (uint8_t)(newcol);
}

// Blends a pixel a pixel from a surface with BPP 24, taking into
// consideration the value of alpha. Relatively slow!
void gari_surface_rbp24(SDL_Surface * surface, int x, int y, 
                       GariDye dye, GariAlpha alpha) {
  SDL_PixelFormat * format = surface->format;
  PixelPtr  ptr   = gari_surface_pixelptr24(surface, x, y);
  GariColor  old, col, nec;
  // GariDye newcol;
  old.r     = *ptr.rptr;
  old.g     = *ptr.gptr;
  old.b     = *ptr.bptr;
  old.a     = *ptr.aptr;
  col.r     = (uint8_t)((dye >> format->Rshift) & 0xff);
  col.g     = (uint8_t)((dye >> format->Gshift) & 0xff);
  col.b     = (uint8_t)((dye >> format->Bshift) & 0xff);
  col.a     = (uint8_t)((dye >> format->Ashift) & 0xff);
  nec.r     = gari_blend_component(old.r, col.r, alpha);
  nec.g     = gari_blend_component(old.g, col.g, alpha);
  nec.b     = gari_blend_component(old.b, col.b, alpha);
  nec.a     = gari_blend_component(old.a, col.a, alpha);
  *ptr.rptr = nec.r;
  *ptr.bptr = nec.g;
  *ptr.gptr = nec.b;
  *ptr.aptr = nec.a; 
}


// Blends a pixel with the one already there to a surface with BPP 16
// taking into account the value of alpha
void gari_surface_rbp16(SDL_Surface * surface, int x, int y, 
                       GariDye dye, GariAlpha alpha)  {
  SDL_PixelFormat * format = surface->format;                         
  uint16_t * ptr           = gari_surface_pixelptr16(surface, x, y);
  GariDye old            = (GariDye)(*ptr);
  GariDye newcol         = gari_help_blend(old, dye, 
                             format->Rmask, format->Gmask,
                             format->Bmask, format->Amask,
                             alpha);
  *ptr                    = (uint16_t)(newcol);
}

// Blends a pixel with the one already there to a surface with BPP 32
// taking into account the value of alpha
void gari_surface_rbp32(SDL_Surface * surface, int x, int y, 
                       GariDye dye, GariAlpha alpha)  {
  SDL_PixelFormat * format = surface->format;                         
  uint32_t * ptr           = gari_surface_pixelptr32(surface, x, y);
  GariDye old            = (GariDye)(*ptr);
  GariDye newcol         = gari_help_blend(old, dye, 
                             format->Rmask, format->Gmask,
                             format->Bmask, format->Amask,
                             alpha);
  *ptr                     = (newcol);
}

// Blends a pixel depending on the BytesPerPixel of the target surface
// format. Still doesn't check the x and y coordinates for validity. 
void gari_surface_rbpbpp(SDL_Surface * surface, int x, int y, 
                          GariDye dye, GariAlpha alpha) {
  switch (surface->format->BytesPerPixel) {
    case 1: gari_surface_rbp8(surface, x, y, dye, alpha); break; 
    case 2: gari_surface_rbp16(surface, x, y, dye, alpha); break;
    case 3: gari_surface_rbp24(surface, x, y, dye, alpha); break;
    case 4: gari_surface_rbp32(surface, x, y, dye, alpha); break;    
  }
}




// Puts a pixel with the given dye at the given coordinates
// Takes the clipping rectangle and surface bounds into consideration
// Does no locking, so lock around it! Only for 8 bits surfaces. 
void gari_image_putpixel8_nl(GariImage *img, int x, int y, GariDye dye) {
  SDL_Surface * s = gari_image_surface(img);
  if(gari_image_pixeloutorclip(img, x, y)) { return; } 
if(gari_image_pixeloutorclip(img, x, y)) { return; } 
  if(gari_image_pixeloutside(img, x, y)) { return ; }   
  gari_surface_rpp8(s, x, y , dye);
}

// Puts a pixel with the given dye at the given coordinates
// Takes the clipping rectangle and surface bounds into consideration
// Does no locking, so lock around it! Only for 16 bits surfaces. 
void gari_image_putpixel16_nl(GariImage *img, int x, int y, GariDye dye) {
  SDL_Surface * s = gari_image_surface(img);
  if(gari_image_pixeloutorclip(img, x, y)) { return; }    
  gari_surface_rpp16(s, x, y , dye);
}

// Puts a pixel with the given dye at the given coordinates
// Takes the clipping rectangle and surface bounds into consideration
// Does no locking, so lock around it! Only for 24 bits surfaces. 
void gari_image_putpixel24_nl(GariImage *img, int x, int y, GariDye dye) {
  SDL_Surface * s = gari_image_surface(img);
  if(gari_image_pixeloutorclip(img, x, y)) { return; }    
  gari_surface_rpp24(s, x, y , dye);
}


// Puts a pixel with the given dye at the given coordinates
// Takes the clipping rectangle and surface bounds into consideration
// Does no locking, so lock around it! Only for 32 bits surfaces. 
void gari_image_putpixel32_nl(GariImage *img, int x, int y, GariDye dye) {
  SDL_Surface * s = gari_image_surface(img);
  if(gari_image_pixeloutorclip(img, x, y)) { return; } 
  gari_surface_rpp32(s, x, y , dye);
}

// Puts a pixel with the given dye at the given coordinates
// Takes the clipping rectangle and surface bounds into consideration
// Does no locking, so lock around it! 
void gari_image_putpixel_nolock(GariImage *img, int x, int y, GariDye dye) {
  SDL_Surface * s = gari_image_surface(img);
  if(gari_image_pixeloutorclip(img, x, y)) { return; } 
  gari_surface_rppbpp(s, x, y , dye);
}


// Gets the dye of a pixel from this surface.
// Returns 0 if the pixel is outside of the clipping rectangle,
// or outside of the bounds of the surface.
// Does no locking, so lock around it! Only for 8 bits GariImages.
GariDye gari_image_getpixel8_nl(GariImage *img, int x, int y) {
  GariDye dye = 0;
  SDL_Surface * s = gari_image_surface(img);
  if(gari_image_pixeloutorclip(img, x, y)) { return dye; }
  dye = gari_surface_rgp8(s, x, y);  
  return dye;
}

// Gets the dye of a pixel from this surface. 
// Returns 0 if the pixel is outside of the clipping rectangle,
// or outside of the bounds of the surface
// Does no locking, so lock around it! Only works for 16 bits GariImages
GariDye gari_image_getpixel16_nl(GariImage *img, int x, int y) {
  GariDye dye = 0;
  SDL_Surface * s = gari_image_surface(img);
  if(gari_image_pixeloutorclip(img, x, y)) { return dye; }
  dye = gari_surface_rgp16(s, x, y);  
  return dye;
}

// Gets the dye of a pixel from this surface. 
// Returns 0 if the pixel is outside of the clipping rectangle,
// or outside of the bounds of the surface
// Does no locking, so lock around it! Only works for 24 bits GariImages
GariDye gari_image_getpixel24_nl(GariImage *img, int x, int y) {
  GariDye dye = 0;
  SDL_Surface * s = gari_image_surface(img);
  if(gari_image_pixeloutorclip(img, x, y)) { return dye; }  
  dye = gari_surface_rgp24(s, x, y);  
  return dye;
}

// Gets the dye of a pixel from this surface. 
// Returns 0 if the pixel is outside of the clipping rectangle,
// or outside of the bounds of the surface
// Does no locking, so lock around it! Only works for 32 bits GariImages
GariDye gari_image_getpixel32_nl(GariImage *img, int x, int y) {
  GariDye dye = 0;
  SDL_Surface * s = gari_image_surface(img);
  if(gari_image_pixeloutorclip(img, x, y)) { return dye; }  
  dye = gari_surface_rgp32(s, x, y);  
  return dye;
}


// Gets the dye of a pixel from this surface 
// Returns 0 if the pixel is outside of the clipping rectangle,
// or outside of the bounds of the surface
// Does no locking, so lock around it!
GariDye gari_image_getpixel_nolock(GariImage *img, int x, int y) {
  GariDye dye = 0;
  SDL_Surface * s = gari_image_surface(img);
  if(gari_image_pixeloutorclip(img, x, y)) { return dye; }
  dye = gari_surface_rgpbpp(s, x, y);  
  return dye;
}


// Blends the dye of a pixel from this surface, 
// taking alpha into consideration. 
// Does no locking, so lock around it! Only works for 32 bits GariImages
void gari_image_blendpixel8_nl(GariImage *img, 
  int x, int y, GariDye dye, GariAlpha alpha) {
  SDL_Surface * s = gari_image_surface(img);
  if(gari_image_pixeloutorclip(img, x, y)) { return; }    
  gari_surface_rbp8(s, x, y , dye, alpha);  
}


// Blends the dye of a pixel from this surface, 
// taking alpha into consideration. 
// Does no locking, so lock around it! Only works for 32 bits GariImages
void gari_image_blendpixel16_nl(GariImage *img, 
  int x, int y, GariDye dye, GariAlpha alpha) {
  SDL_Surface * s = gari_image_surface(img);
  if(gari_image_pixeloutorclip(img, x, y)) { return; }    
  gari_surface_rbp16(s, x, y , dye, alpha);  
}


// Blends the dye of a pixel from this surface, 
// taking alpha into consideration. 
// Does no locking, so lock around it! Only works for 32 bits GariImages
void gari_image_blendpixel24_nl(GariImage *img, 
  int x, int y, GariDye dye, GariAlpha alpha) {
  SDL_Surface * s = gari_image_surface(img);
  if(gari_image_pixeloutorclip(img, x, y)) { return; }    
  gari_surface_rbp24(s, x, y , dye, alpha);  
}


// Blends the dye of a pixel from this surface, 
// taking alpha into consideration. 
// Does no locking, so lock around it! Only works for 32 bits GariImages
void gari_image_blendpixel32_nl(GariImage *img, 
  int x, int y, GariDye dye, GariAlpha alpha) {
  SDL_Surface * s = gari_image_surface(img);
  if(gari_image_pixeloutorclip(img, x, y)) { return; }    
  gari_surface_rbp32(s, x, y , dye, alpha);  
}



// Blends the dye of a pixel from this surface, 
// taking alpha into consideration. 
// Does no locking, so lock around it!
void gari_image_blendpixel_nolock(GariImage *img, 
  int x, int y, GariDye dye, GariAlpha alpha) {
  SDL_Surface * s = gari_image_surface(img);
  if(gari_image_pixeloutorclip(img, x, y)) { return; }    
  gari_surface_rbpbpp(s, x, y , dye, alpha);  
}


// Puts a pixel with the given dye at the given coordinates
// Takes the clipping rectangle and surface bounds into consideration
// Locks and unlocks the surface if that is needed for drawing
void gari_image_putpixel(GariImage *img, int x, int y, GariDye dye) {
  SDL_Surface * s = gari_image_surface(img);
  if(gari_image_pixeloutorclip(img, x, y)) { return; }
  gari_surface_lock(s);
  gari_surface_rppbpp(s, x, y , dye);  
  gari_surface_unlock(s);
}


/** Puts a pixel with the given dye at the given coordinates
* Takes the clipping rectangle and surface bounds into consideration
* Locks and unlocks the surface if that is needed for drawing
*/

void gari_image_dot(GariImage *img, int x, int y, GariColor color) {
  GariDye dye = gari_color_dye(color, img);
  gari_image_putpixel(img, x, y, dye);
}


// Gets the dye of a pixel from this surface 
// Returns 0 if the pixel is outside of the clipping rectangle,
// or outside of the bounds of the surface
// Locks and unlocks the surface if that is needed
GariDye gari_image_getpixel(GariImage *img, int x, int y) {
  GariDye dye = 0;
  SDL_Surface * s = gari_image_surface(img);
  if(gari_image_pixeloutorclip(img, x, y)) { return dye; }
  gari_surface_lock(s);
  dye = gari_surface_rgpbpp(s, x, y);  
  gari_surface_unlock(s);
  return dye;
}

// Blends the dye of a pixel from this surface, 
// taking alpha into consideration. 
// Locks and unlocks the surface if that is needed for drawing
void gari_image_blendpixel(GariImage *img, 
  int x, int y, GariDye dye, GariAlpha alpha) {
  SDL_Surface * s = gari_image_surface(img);
  if(gari_image_pixeloutorclip(img, x, y)) { return; }
  gari_surface_lock(s);
  gari_surface_rbpbpp(s, x, y , dye, alpha);  
  gari_surface_unlock(s);
}


/** Blends a pixel with the given dye at the given coordinates
* Takes the clipping rectangle and surface bounds into consideration
* Locks and unlocks the surface if that is needed for drawing
*/

void gari_image_blenddot(GariImage *img, int x, int y, GariColor color) {
  GariDye dye = gari_color_dye(color, img);
  gari_image_blendpixel(img, x, y, dye, color.a);
}

/** Gets a pixel with the given dye at the given coordinates
* Takes the clipping rectangle and surface bounds into consideration
* Locks and unlocks the surface if that is needed for drawing
*/

GariColor gari_image_getdot(GariImage *img, int x, int y) {
  GariDye dye = gari_image_getpixel(img, x, y);
  return gari_dye_color(dye, img);
}


/*
typedef void GariPutPixelFunc(GariImage * img, int x, int y, GariDye dye);
typedef GariDye GariGetPixelFunc(GariImage * img, int x, int y);
typedef void GariBlendPixelFunc(GariImage * img, int x, int y, GariDye dye, GariAlpha alpha);
*/

// Returns a function that can be used to put a pixel on this GariImage.
// The function returned does no locking. May return NULL if the format does 
// not use 1,2,3 or 4 bytes per pixel.    
GariPutPixelFunc * gari_image_putpixelfunc_nl(GariImage * image) {
  switch (gari_image_bytesperpixel(image)) {
    case 1: return gari_image_putpixel8_nl; 
    case 2: return gari_image_putpixel16_nl;
    case 3: return gari_image_putpixel24_nl;
    case 4: return gari_image_putpixel32_nl;
    default: return NULL;    
  }
}

// Returns a function that can be used to get a pixel from this GariImage.
// The function returned does no locking. May return NULL if the format does 
// not use 1,2,3 or 4 bytes per pixel.    
GariGetPixelFunc * gari_image_getpixelfunc_nl(GariImage * image) {
  switch (gari_image_bytesperpixel(image)) {
    case 1: return gari_image_getpixel8_nl; 
    case 2: return gari_image_getpixel16_nl;
    case 3: return gari_image_getpixel24_nl;
    case 4: return gari_image_getpixel32_nl;
    default: return NULL;    
  }
}

// Returns a function that can be used to blend a pixel from this GariImage.
// The function returned does no locking. May return NULL if the format does 
// not use 1,2,3 or 4 bytes per pixel.    
GariBlendPixelFunc * gari_image_blendpixelfunc_nl(GariImage * image) {
  switch (gari_image_bytesperpixel(image)) {
    case 1: return gari_image_blendpixel8_nl; 
    case 2: return gari_image_blendpixel16_nl;
    case 3: return gari_image_blendpixel24_nl;
    case 4: return gari_image_blendpixel32_nl;
    default: return NULL;    
  }
}


