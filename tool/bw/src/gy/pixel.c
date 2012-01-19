/* Single pixel related functions (put, get, blend) */

#include "gy.h"

#define GY_INTERN_ONLY
#include "gy_intern.h"

/**
Gy drawing functions.
*/

SDL_Surface * gyimage_surface(GyImage * img) {
  return GY_IMAGE_SURFACE(img);
  // return ((SDL_Surface*)(img));
}

int gyimage_w(GyImage * img) {
  return GY_IMAGE_W(img);
  // return (gyimage_surface(img))->w;
}

int gyimage_h(GyImage * img) {
  return GY_IMAGE_H(img);
  // return (gyimage_surface(img))->h;
}

// Returns the amount of byrtes per pixel for this image's format (1,2,3,4)
int gyimage_bytesperpixel(GyImage * img) {
  SDL_Surface * surface = gyimage_surface(img);
  if(!img)             return 0;
  if(!surface)         return 0; 
  if(!surface->format) return 0;
  return surface->format->BytesPerPixel;
}

void gysurface_lock(SDL_Surface * surface) {
  (void)GY_SURFACE_LOCK(surface);
  // SDL_MUSTLOCK(surface) && SDL_LockSurface(surface);
  // use boolean short cut  
}

void gysurface_unlock(SDL_Surface * surface) {
  GY_SURFACE_UNLOCK(surface);
  // SDL_MUSTLOCK(surface) && (SDL_UnlockSurface(surface), 0)
  // use boolean short cut  
}


void gyimage_lock(GyImage * image) {
  (void)GY_SURFACE_LOCK(GY_IMAGE_SURFACE(image));
}

void gyimage_unlock(GyImage * image) {
  (void)GY_SURFACE_UNLOCK(GY_IMAGE_SURFACE(image));
}



// Returns nonzero if the pixel is outside the image boundaries
// and may /not/ be drawn to or read from using the PutPixelXXX
// GetPixelXXX and BlendPixelXXX series of functions.
// Returns FALSE if the pixel is safe to draw to.
// This may seeem conterintuitive, but normal use will be
// if surf.PixelOutside(x, y) { return }    
int gyimage_pixeloutside(GyImage * image, int x, int y) {
  return GY_IMAGE_PIXELOUTSIDE(image, x, y);
  /*
  if (x < 0 || y < 0)            { return TRUE; } 
  if (x >= gyimage_w(image))  { return TRUE; }
  if (y >= gyimage_h(image))  { return TRUE; }
  return FALSE;
  */
}

// Returns TRUE if the pixel lies outside the clipping rectangle
// returns FALSE if not so and it may be drawn
int gyimage_pixelclip(GyImage * image, int x, int y) {
  return GY_IMAGE_PIXELCLIP(image, x, y);  
}

// Returns true if the pixel lies outside of the image's boundaries or 
// clipping rectangle.
int gyimage_pixeloutorclip(GyImage * image, int x, int y) {
  if (GY_IMAGE_PIXELOUTSIDE(image, x, y)) { return TRUE; } 
  return GY_IMAGE_PIXELCLIP(image, x, y);
}



/** Generic pointer type suitable for pointer arithmetic. */
typedef uint8_t * GyPtr;

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
uint8_t * gysurface_pixelptr8(SDL_Surface * surface, int x, int y) {
  return GY_SURFACE_PIXELPTR8(surface, x, y);
}

// Returns a pointer that points to the location of the pixel 
// at x and y for a surface s with bbp16
// Does no checks for validity of x and y and does no clipping!
uint16_t * gysurface_pixelptr16(SDL_Surface * surface, int x, int y) {
  return GY_SURFACE_PIXELPTR16(surface, x, y);
}

// Returns a pointer that points to the location of the pixel 
// at x and y for a surface s with bbp32
// Does no checks for validity of x and y and does no clipping!
uint32_t * gysurface_pixelptr32(SDL_Surface * surface, int x, int y) {
  return GY_SURFACE_PIXELPTR32(surface, x, y);
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
PixelPtr gysurface_pixelptr24(SDL_Surface * surface, int x, int y) {
  PixelPtr res;
  SDL_PixelFormat *format = surface->format;
  GyPtr ptrbase = NULL;
  GyPtr pixels  = surface->pixels;
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
void gysurface_rpp8(SDL_Surface * surface, int x, int y, GyDye dye) {
  uint8_t * ptr = gysurface_pixelptr8(surface, x, y); 
  *ptr          = (uint8_t)(dye);
}

// Puts a pixel to a surface with BPP 16
void gysurface_rpp16(SDL_Surface * surface, int x, int y, GyDye dye) {
  uint16_t * ptr = gysurface_pixelptr16(surface, x, y); 
  *ptr            = (uint16_t)(dye);
}

// Puts a pixel to a surface with BPP 32
void gysurface_rpp32(SDL_Surface * surface, int x, int y, GyDye dye) {
  uint32_t * ptr  = gysurface_pixelptr32(surface, x, y); 
  *ptr            = (uint32_t)(dye);
}

// Puts a pixel to a surface with BPP 24. Relatively slow!
void gysurface_rpp24(SDL_Surface * surface, int x, int y, GyDye dye) {
  PixelPtr ptr    = gysurface_pixelptr24(surface, x, y);
  SDL_PixelFormat *format = surface->format; 
  *(ptr.rptr)     = (uint8_t)(dye >> (GyDye)(format->Rshift));  
  *(ptr.gptr)     = (uint8_t)(dye >> (GyDye)(format->Gshift));
  *(ptr.bptr)     = (uint8_t)(dye >> (GyDye)(format->Bshift));
  *(ptr.aptr)     = (uint8_t)(dye >> (GyDye)(format->Ashift));  
}


// Puts a pixel depending on the BytesPerPixel of the target surface
// format. Still doesn't check the x and y coordinates for validity. 
void gysurface_rppbpp(SDL_Surface * surface, int x, int y, GyDye dye) {
  switch (surface->format->BytesPerPixel) {
    case 1: gysurface_rpp8(surface, x, y, dye); break; 
    case 2: gysurface_rpp16(surface, x, y, dye); break;
    case 3: gysurface_rpp24(surface, x, y, dye); break;
    case 4: gysurface_rpp32(surface, x, y, dye); break;    
  }
}


// getpixel primitives (RawGetPixel)
// for 8 bit surfaces
GyDye gysurface_rgp8(SDL_Surface * surface, int x, int y) {
  uint8_t * ptr = gysurface_pixelptr8(surface, x, y); 
  return (GyDye)(*ptr);
}

GyDye gysurface_rgp16(SDL_Surface * surface, int x, int y) {
  uint16_t * ptr = gysurface_pixelptr16(surface, x, y); 
  return (GyDye)(*ptr);
}

GyDye gysurface_rgp32(SDL_Surface * surface, int x, int y) {
  uint32_t * ptr = gysurface_pixelptr32(surface, x, y); 
  return (GyDye)(*ptr);
}

// Gets a pixel from a surface with BPP 24. Relatively slow!
GyDye gysurface_rgp24(SDL_Surface * surface, int x, int y) {
  PixelPtr ptr    = gysurface_pixelptr24(surface, x, y);
  SDL_PixelFormat *format = surface->format; 
  GyDye dye = 0;
  dye   = (*ptr.rptr) << format->Rshift;
  dye  |= (*ptr.gptr) << format->Gshift;
  dye  |= (*ptr.bptr) << format->Bshift;
  dye  |= (*ptr.aptr) << format->Ashift;
  return dye;
}


// Puts a pixel depending on the BytesPerPixel of the target surface
// format. Still doesn't check the x and y coordinates for validity. 
GyDye gysurface_rgpbpp(SDL_Surface * surface, int x, int y) {
  switch (surface->format->BytesPerPixel) {
    case 1: return gysurface_rgp8(surface , x, y); break; 
    case 2: return gysurface_rgp16(surface, x, y); break;
    case 3: return gysurface_rgp24(surface, x, y); break;
    case 4: return gysurface_rgp32(surface, x, y); break;    
  }
  return (GyDye)(0);
}

// Helps blending two dyes
GyDye gyhelp_blend(GyDye old, GyDye dye,
  GyDye rmask, GyDye gmask, GyDye bmask,
  GyDye amask, GyAlpha alpha)  {
  GyDye oldr, oldb, oldg, olda, colr, colb, colg, cola, r, b, g , a;
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
uint8_t gyblend_component(uint8_t oldc, uint8_t newc, GyAlpha alpha)  {
  return oldc + (((newc-oldc)*alpha) >> 8);
}




// Blends a pixel with the one already there to a surface with BPP 8
// taking into account the value of alpha
void gysurface_rbp8(SDL_Surface * surface, int x, int y, 
                       GyDye dye, GyAlpha alpha) {    
  GyPtr   ptr   = gysurface_pixelptr8(surface, x, y);
  GyDye oldcol  = (uint32_t)(*ptr);
  GyColor  old  = gysurface_getrgb(surface, oldcol);
  // Messing with the palette would probably be faster, 
  // but it's cleaner to do it like this.
  GyColor col   = gysurface_getrgb(surface, dye);
  GyColor nec;
  GyDye newcol;
  nec.r   = gyblend_component(old.r, col.r, alpha);
  nec.g   = gyblend_component(old.g, col.g, alpha);
  nec.b   = gyblend_component(old.b, col.b, alpha);
  newcol  = gysurface_maprgb(surface, nec);
  // And we have to do the same for the new color anyway, so... 
  *ptr    = (uint8_t)(newcol);
}

// Blends a pixel a pixel from a surface with BPP 24, taking into
// consideration the value of alpha. Relatively slow!
void gysurface_rbp24(SDL_Surface * surface, int x, int y, 
                       GyDye dye, GyAlpha alpha) {
  SDL_PixelFormat * format = surface->format;
  PixelPtr  ptr   = gysurface_pixelptr24(surface, x, y);
  GyColor  old, col, nec;
  // GyDye newcol;
  old.r     = *ptr.rptr;
  old.g     = *ptr.gptr;
  old.b     = *ptr.bptr;
  old.a     = *ptr.aptr;
  col.r     = (uint8_t)((dye >> format->Rshift) & 0xff);
  col.g     = (uint8_t)((dye >> format->Gshift) & 0xff);
  col.b     = (uint8_t)((dye >> format->Bshift) & 0xff);
  col.a     = (uint8_t)((dye >> format->Ashift) & 0xff);
  nec.r     = gyblend_component(old.r, col.r, alpha);
  nec.g     = gyblend_component(old.g, col.g, alpha);
  nec.b     = gyblend_component(old.b, col.b, alpha);
  nec.a     = gyblend_component(old.a, col.a, alpha);
  *ptr.rptr = nec.r;
  *ptr.bptr = nec.g;
  *ptr.gptr = nec.b;
  *ptr.aptr = nec.a; 
}


// Blends a pixel with the one already there to a surface with BPP 16
// taking into account the value of alpha
void gysurface_rbp16(SDL_Surface * surface, int x, int y, 
                       GyDye dye, GyAlpha alpha)  {
  SDL_PixelFormat * format = surface->format;                         
  uint16_t * ptr           = gysurface_pixelptr16(surface, x, y);
  GyDye old            = (GyDye)(*ptr);
  GyDye newcol         = gyhelp_blend(old, dye, 
                             format->Rmask, format->Gmask,
                             format->Bmask, format->Amask,
                             alpha);
  *ptr                    = (uint16_t)(newcol);
}

// Blends a pixel with the one already there to a surface with BPP 32
// taking into account the value of alpha
void gysurface_rbp32(SDL_Surface * surface, int x, int y, 
                       GyDye dye, GyAlpha alpha)  {
  SDL_PixelFormat * format = surface->format;                         
  uint32_t * ptr           = gysurface_pixelptr32(surface, x, y);
  GyDye old            = (GyDye)(*ptr);
  GyDye newcol         = gyhelp_blend(old, dye, 
                             format->Rmask, format->Gmask,
                             format->Bmask, format->Amask,
                             alpha);
  *ptr                     = (newcol);
}

// Blends a pixel depending on the BytesPerPixel of the target surface
// format. Still doesn't check the x and y coordinates for validity. 
void gysurface_rbpbpp(SDL_Surface * surface, int x, int y, 
                          GyDye dye, GyAlpha alpha) {
  switch (surface->format->BytesPerPixel) {
    case 1: gysurface_rbp8(surface, x, y, dye, alpha); break; 
    case 2: gysurface_rbp16(surface, x, y, dye, alpha); break;
    case 3: gysurface_rbp24(surface, x, y, dye, alpha); break;
    case 4: gysurface_rbp32(surface, x, y, dye, alpha); break;    
  }
}




// Puts a pixel with the given dye at the given coordinates
// Takes the clipping rectangle and surface bounds into consideration
// Does no locking, so lock around it! Only for 8 bits surfaces. 
void gyimage_putpixel8_nl(GyImage *img, int x, int y, GyDye dye) {
  SDL_Surface * s = gyimage_surface(img);
  if(gyimage_pixeloutorclip(img, x, y)) { return; } 
if(gyimage_pixeloutorclip(img, x, y)) { return; } 
  if(gyimage_pixeloutside(img, x, y)) { return ; }   
  gysurface_rpp8(s, x, y , dye);
}

// Puts a pixel with the given dye at the given coordinates
// Takes the clipping rectangle and surface bounds into consideration
// Does no locking, so lock around it! Only for 16 bits surfaces. 
void gyimage_putpixel16_nl(GyImage *img, int x, int y, GyDye dye) {
  SDL_Surface * s = gyimage_surface(img);
  if(gyimage_pixeloutorclip(img, x, y)) { return; }    
  gysurface_rpp16(s, x, y , dye);
}

// Puts a pixel with the given dye at the given coordinates
// Takes the clipping rectangle and surface bounds into consideration
// Does no locking, so lock around it! Only for 24 bits surfaces. 
void gyimage_putpixel24_nl(GyImage *img, int x, int y, GyDye dye) {
  SDL_Surface * s = gyimage_surface(img);
  if(gyimage_pixeloutorclip(img, x, y)) { return; }    
  gysurface_rpp24(s, x, y , dye);
}


// Puts a pixel with the given dye at the given coordinates
// Takes the clipping rectangle and surface bounds into consideration
// Does no locking, so lock around it! Only for 32 bits surfaces. 
void gyimage_putpixel32_nl(GyImage *img, int x, int y, GyDye dye) {
  SDL_Surface * s = gyimage_surface(img);
  if(gyimage_pixeloutorclip(img, x, y)) { return; } 
  gysurface_rpp32(s, x, y , dye);
}

// Puts a pixel with the given dye at the given coordinates
// Takes the clipping rectangle and surface bounds into consideration
// Does no locking, so lock around it! 
void gyimage_putpixel_nolock(GyImage *img, int x, int y, GyDye dye) {
  SDL_Surface * s = gyimage_surface(img);
  if(gyimage_pixeloutorclip(img, x, y)) { return; } 
  gysurface_rppbpp(s, x, y , dye);
}


// Gets the dye of a pixel from this surface.
// Returns 0 if the pixel is outside of the clipping rectangle,
// or outside of the bounds of the surface.
// Does no locking, so lock around it! Only for 8 bits GyImages.
GyDye gyimage_getpixel8_nl(GyImage *img, int x, int y) {
  GyDye dye = 0;
  SDL_Surface * s = gyimage_surface(img);
  if(gyimage_pixeloutorclip(img, x, y)) { return dye; }
  dye = gysurface_rgp8(s, x, y);  
  return dye;
}

// Gets the dye of a pixel from this surface. 
// Returns 0 if the pixel is outside of the clipping rectangle,
// or outside of the bounds of the surface
// Does no locking, so lock around it! Only works for 16 bits GyImages
GyDye gyimage_getpixel16_nl(GyImage *img, int x, int y) {
  GyDye dye = 0;
  SDL_Surface * s = gyimage_surface(img);
  if(gyimage_pixeloutorclip(img, x, y)) { return dye; }
  dye = gysurface_rgp16(s, x, y);  
  return dye;
}

// Gets the dye of a pixel from this surface. 
// Returns 0 if the pixel is outside of the clipping rectangle,
// or outside of the bounds of the surface
// Does no locking, so lock around it! Only works for 24 bits GyImages
GyDye gyimage_getpixel24_nl(GyImage *img, int x, int y) {
  GyDye dye = 0;
  SDL_Surface * s = gyimage_surface(img);
  if(gyimage_pixeloutorclip(img, x, y)) { return dye; }  
  dye = gysurface_rgp24(s, x, y);  
  return dye;
}

// Gets the dye of a pixel from this surface. 
// Returns 0 if the pixel is outside of the clipping rectangle,
// or outside of the bounds of the surface
// Does no locking, so lock around it! Only works for 32 bits GyImages
GyDye gyimage_getpixel32_nl(GyImage *img, int x, int y) {
  GyDye dye = 0;
  SDL_Surface * s = gyimage_surface(img);
  if(gyimage_pixeloutorclip(img, x, y)) { return dye; }  
  dye = gysurface_rgp32(s, x, y);  
  return dye;
}


// Gets the dye of a pixel from this surface 
// Returns 0 if the pixel is outside of the clipping rectangle,
// or outside of the bounds of the surface
// Does no locking, so lock around it!
GyDye gyimage_getpixel_nolock(GyImage *img, int x, int y) {
  GyDye dye = 0;
  SDL_Surface * s = gyimage_surface(img);
  if(gyimage_pixeloutorclip(img, x, y)) { return dye; }
  dye = gysurface_rgpbpp(s, x, y);  
  return dye;
}


// Blends the dye of a pixel from this surface, 
// taking alpha into consideration. 
// Does no locking, so lock around it! Only works for 32 bits GyImages
void gyimage_blendpixel8_nl(GyImage *img, 
  int x, int y, GyDye dye, GyAlpha alpha) {
  SDL_Surface * s = gyimage_surface(img);
  if(gyimage_pixeloutorclip(img, x, y)) { return; }    
  gysurface_rbp8(s, x, y , dye, alpha);  
}


// Blends the dye of a pixel from this surface, 
// taking alpha into consideration. 
// Does no locking, so lock around it! Only works for 32 bits GyImages
void gyimage_blendpixel16_nl(GyImage *img, 
  int x, int y, GyDye dye, GyAlpha alpha) {
  SDL_Surface * s = gyimage_surface(img);
  if(gyimage_pixeloutorclip(img, x, y)) { return; }    
  gysurface_rbp16(s, x, y , dye, alpha);  
}


// Blends the dye of a pixel from this surface, 
// taking alpha into consideration. 
// Does no locking, so lock around it! Only works for 32 bits GyImages
void gyimage_blendpixel24_nl(GyImage *img, 
  int x, int y, GyDye dye, GyAlpha alpha) {
  SDL_Surface * s = gyimage_surface(img);
  if(gyimage_pixeloutorclip(img, x, y)) { return; }    
  gysurface_rbp24(s, x, y , dye, alpha);  
}


// Blends the dye of a pixel from this surface, 
// taking alpha into consideration. 
// Does no locking, so lock around it! Only works for 32 bits GyImages
void gyimage_blendpixel32_nl(GyImage *img, 
  int x, int y, GyDye dye, GyAlpha alpha) {
  SDL_Surface * s = gyimage_surface(img);
  if(gyimage_pixeloutorclip(img, x, y)) { return; }    
  gysurface_rbp32(s, x, y , dye, alpha);  
}



// Blends the dye of a pixel from this surface, 
// taking alpha into consideration. 
// Does no locking, so lock around it!
void gyimage_blendpixel_nolock(GyImage *img, 
  int x, int y, GyDye dye, GyAlpha alpha) {
  SDL_Surface * s = gyimage_surface(img);
  if(gyimage_pixeloutorclip(img, x, y)) { return; }    
  gysurface_rbpbpp(s, x, y , dye, alpha);  
}


// Puts a pixel with the given dye at the given coordinates
// Takes the clipping rectangle and surface bounds into consideration
// Locks and unlocks the surface if that is needed for drawing
void gyimage_putpixel(GyImage *img, int x, int y, GyDye dye) {
  SDL_Surface * s = gyimage_surface(img);
  if(gyimage_pixeloutorclip(img, x, y)) { return; }
  gysurface_lock(s);
  gysurface_rppbpp(s, x, y , dye);  
  gysurface_unlock(s);
}


/** Puts a pixel with the given dye at the given coordinates
* Takes the clipping rectangle and surface bounds into consideration
* Locks and unlocks the surface if that is needed for drawing
*/

void gyimage_dot(GyImage *img, int x, int y, GyColor color) {
  GyDye dye = gycolor_dye(color, img);
  gyimage_putpixel(img, x, y, dye);
}


// Gets the dye of a pixel from this surface 
// Returns 0 if the pixel is outside of the clipping rectangle,
// or outside of the bounds of the surface
// Locks and unlocks the surface if that is needed
GyDye gyimage_getpixel(GyImage *img, int x, int y) {
  GyDye dye = 0;
  SDL_Surface * s = gyimage_surface(img);
  if(gyimage_pixeloutorclip(img, x, y)) { return dye; }
  gysurface_lock(s);
  dye = gysurface_rgpbpp(s, x, y);  
  gysurface_unlock(s);
  return dye;
}

// Blends the dye of a pixel from this surface, 
// taking alpha into consideration. 
// Locks and unlocks the surface if that is needed for drawing
void gyimage_blendpixel(GyImage *img, 
  int x, int y, GyDye dye, GyAlpha alpha) {
  SDL_Surface * s = gyimage_surface(img);
  if(gyimage_pixeloutorclip(img, x, y)) { return; }
  gysurface_lock(s);
  gysurface_rbpbpp(s, x, y , dye, alpha);  
  gysurface_unlock(s);
}


/** Blends a pixel with the given dye at the given coordinates
* Takes the clipping rectangle and surface bounds into consideration
* Locks and unlocks the surface if that is needed for drawing
*/

void gyimage_blenddot(GyImage *img, int x, int y, GyColor color) {
  GyDye dye = gycolor_dye(color, img);
  gyimage_blendpixel(img, x, y, dye, color.a);
}

/** Gets a pixel with the given dye at the given coordinates
* Takes the clipping rectangle and surface bounds into consideration
* Locks and unlocks the surface if that is needed for drawing
*/

GyColor gyimage_getdot(GyImage *img, int x, int y) {
  GyDye dye = gyimage_getpixel(img, x, y);
  return gydye_color(dye, img);
}


void gyimage_dot_nolock(GyImage *img, int x, int y, GyColor color) {
  GyDye dye = gycolor_dye(color, img);   
  gyimage_putpixel_nolock(img, x, y, dye);  
}


GyColor gyimage_getdot_nolock(GyImage *img, int x, int y) {
  GyDye dye = gyimage_getpixel_nolock(img, x, y);
  return gydye_color(dye, img);
}

void gyimage_blenddot_nolock(GyImage *img, int x, int y, GyColor color) {
  GyDye dye = gycolor_dye(color, img);
  gyimage_blendpixel_nolock(img, x, y, dye, color.a);
}


/*
typedef void GyPutPixelFunc(GyImage * img, int x, int y, GyDye dye);
typedef GyDye GyGetPixelFunc(GyImage * img, int x, int y);
typedef void GyBlendPixelFunc(GyImage * img, int x, int y, GyDye dye, GyAlpha alpha);
*/

// Returns a function that can be used to put a pixel on this GyImage.
// The function returned does no locking. May return NULL if the format does 
// not use 1,2,3 or 4 bytes per pixel.    
GyPutPixelFunc * gyimage_putpixelfunc_nl(GyImage * image) {
  switch (gyimage_bytesperpixel(image)) {
    case 1: return gyimage_putpixel8_nl; 
    case 2: return gyimage_putpixel16_nl;
    case 3: return gyimage_putpixel24_nl;
    case 4: return gyimage_putpixel32_nl;
    default: return NULL;    
  }
}

// Returns a function that can be used to get a pixel from this GyImage.
// The function returned does no locking. May return NULL if the format does 
// not use 1,2,3 or 4 bytes per pixel.    
GyGetPixelFunc * gyimage_getpixelfunc_nl(GyImage * image) {
  switch (gyimage_bytesperpixel(image)) {
    case 1: return gyimage_getpixel8_nl; 
    case 2: return gyimage_getpixel16_nl;
    case 3: return gyimage_getpixel24_nl;
    case 4: return gyimage_getpixel32_nl;
    default: return NULL;    
  }
}

// Returns a function that can be used to blend a pixel from this GyImage.
// The function returned does no locking. May return NULL if the format does 
// not use 1,2,3 or 4 bytes per pixel.    
GyBlendPixelFunc * gyimage_blendpixelfunc_nl(GyImage * image) {
  switch (gyimage_bytesperpixel(image)) {
    case 1: return gyimage_blendpixel8_nl; 
    case 2: return gyimage_blendpixel16_nl;
    case 3: return gyimage_blendpixel24_nl;
    case 4: return gyimage_blendpixel32_nl;
    default: return NULL;    
  }
}


