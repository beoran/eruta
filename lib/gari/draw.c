#include "gari.h"

/**
Gari drawing functions.
*/

SDL_Surface * gari_image_surface(GariImage * img) {
  return ((SDL_Surface*)(img));
}

int gari_image_w(GariImage * img) {
  return (gari_image_surface(img))->w;
}

int gari_image_h(GariImage * img) {
  return (gari_image_surface(img))->h;
}

// Returns nonzero if the pixel is outside the image boundaries
// and may /not/ be drawn to or read from using the PutPixelXXX
// GetPixelXXX and BlendPixelXXX series of functions.
// Returns FALSE if the pixel is safe to draw to.
// This may seeem conterintuitive, but normal use will be
// if surf.PixelOutside(x, y) { return }    
int gari_image_pixeloutside(GariImage * image, int x, int y) {
  if (x < 0 || y < 0)            { return TRUE; } 
  if (x >= gari_image_w(image))  { return TRUE; }
  if (y >= gari_image_h(image))  { return TRUE; }
  return FALSE;
}

// Returns TRUE if the pixel lies outside the clipping rectangle
// returns FALSE if not so and it may be drawn
int gari_image_pixelclip(GariImage * image, int x, int y) { 
  int minx, miny, maxx, maxy;
  SDL_Rect cliprect;
  SDL_GetClipRect(gari_image_surface(image), &cliprect);  
  minx     = cliprect.x;
  miny     = cliprect.y;
  maxx     = cliprect.x + cliprect.w;
  maxy     = cliprect.y + cliprect.h;
  if (x < minx)   { return TRUE; }
  if (y < miny)   { return TRUE; }
  if (x >= maxx)  { return TRUE; }
  if (y >= maxy)  { return TRUE; }
  return FALSE;
}

/** Generic pointer type suitable for pointer arithmetic. */
typedef char * GariPtr;

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
  GariPtr pixels = surface->pixels; 
  size_t offset = y * surface->pitch  + x;      
  return (uint8_t *)(pixels + offset);
}

// Returns a pointer that points to the location of the pixel 
// at x and y for a surface s with bbp16
// Does no checks for validity of x and y and does no clipping!
uint16_t * gari_surface_pixelptr16(SDL_Surface * surface, int x, int y) {
  GariPtr pixels = surface->pixels; 
  size_t offset = y * surface->pitch  + x<<1;
  return (uint16_t *)(pixels + offset);
}

// Returns a pointer that points to the location of the pixel 
// at x and y for a surface s with bbp32
// Does no checks for validity of x and y and does no clipping!
uint32_t * gari_surface_pixelptr32(SDL_Surface * surface, int x, int y) {
  GariPtr pixels = surface->pixels; 
  size_t offset = y * surface->pitch  + x<<2;
  return (uint32_t *)(pixels + offset);
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
  size_t offset   = y*surface->pitch + x*3;
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
void gari_surface_rpp8(SDL_Surface * surface, int x, int y, GariColor color) {
  uint8_t * ptr = gari_surface_pixelptr8(surface, x, y); 
  *ptr          = (uint8_t)(color);
}

// Puts a pixel to a surface with BPP 16
void gari_surface_rpp16(SDL_Surface * surface, int x, int y, GariColor color) {
  uint16_t * ptr = gari_surface_pixelptr16(surface, x, y); 
  *ptr            = (uint16_t)(color);
}

// Puts a pixel to a surface with BPP 32
void gari_surface_rpp32(SDL_Surface * surface, int x, int y, GariColor color) {
  uint32_t * ptr  = gari_surface_pixelptr32(surface, x, y); 
  *ptr            = (uint32_t)(color);
}

// Puts a pixel to a surface with BPP 24. Relatively slow!
void gari_surface_rpp24(SDL_Surface * surface, int x, int y, GariColor color) {
  PixelPtr ptr    = gari_surface_pixelptr24(surface, x, y);
  SDL_PixelFormat *format = surface->format; 
  *(ptr.rptr)     = (uint8_t)(color >> (GariColor)(format->Rshift));  
  *(ptr.gptr)     = (uint8_t)(color >> (GariColor)(format->Gshift));
  *(ptr.bptr)     = (uint8_t)(color >> (GariColor)(format->Bshift));
  *(ptr.aptr)     = (uint8_t)(color >> (GariColor)(format->Ashift));  
}

// Puts a pixel depending on the BytesPerPixel of the target surface
// format. Still doesn't check the x and y coordinates for validity. 
void gari_surface_rppbpp(SDL_Surface * surface, int x, int y, GariColor color) {
  switch (surface->format->BytesPerPixel) {
    case 1: gari_surface_rpp8(surface, x, y, color); break; 
    case 2: gari_surface_rpp16(surface, x, y, color); break;
    case 3: gari_surface_rpp24(surface, x, y, color); break;
    case 4: gari_surface_rpp32(surface, x, y, color); break;    
  }
}


// getpixel primitives (RawGetPixel)
// for 8 bit surfaces
GariColor gari_surface_rgp8(SDL_Surface * surface, int x, int y) {
  uint8_t * ptr = gari_surface_pixelptr8(surface, x, y); 
  return (GariColor)(*ptr);
}

GariColor gari_surface_rgp16(SDL_Surface * surface, int x, int y) {
  uint16_t * ptr = gari_surface_pixelptr16(surface, x, y); 
  return (GariColor)(*ptr);
}

GariColor gari_surface_rgp32(SDL_Surface * surface, int x, int y) {
  uint32_t * ptr = gari_surface_pixelptr32(surface, x, y); 
  return (GariColor)(*ptr);
}

// Gets a pixel from a surface with BPP 24. Relatively slow!
GariColor gari_surface_rgp24(SDL_Surface * surface, int x, int y) {
  PixelPtr ptr    = gari_surface_pixelptr24(surface, x, y);
  SDL_PixelFormat *format = surface->format; 
  GariColor color = 0;
  color   = (*ptr.rptr) << format->Rshift;
  color  |= (*ptr.gptr) << format->Gshift;
  color  |= (*ptr.bptr) << format->Bshift;
  color  |= (*ptr.aptr) << format->Ashift;
  return color;
}


// Puts a pixel depending on the BytesPerPixel of the target surface
// format. Still doesn't check the x and y coordinates for validity. 
GariColor gari_surface_rgpbpp(SDL_Surface * surface, int x, int y) {
  switch (surface->format->BytesPerPixel) {
    case 1: return gari_surface_rgp8(surface , x, y); break; 
    case 2: return gari_surface_rgp16(surface, x, y); break;
    case 3: return gari_surface_rgp24(surface, x, y); break;
    case 4: return gari_surface_rgp32(surface, x, y); break;    
  }
}

// Helps blending two colors
GariColor gari_help_blend(GariColor old, GariColor color,
  GariColor rmask, GariColor gmask, GariColor bmask,
  GariColor amask, GariAlpha alpha)  {
  GariColor oldr, oldb,oldg, olda, colr, colb, colg, cola, r, b, g , a;
  oldr = old & rmask;
  oldg = old & gmask;
  oldb = old & bmask;
  olda = old & amask;
  colr = color & rmask;
  colg = color & gmask;
  colb = color & bmask;
  cola = color & amask;
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

// Helps blending one component (r,g,b, or a) of a color
uint8_t gari_blend_component(uint8_t oldc, uint8_t newc, GariAlpha alpha)  {
  return oldc + (((newc-oldc)*alpha) >> 8);
}


struct GariRGBA_ {
  uint8_t r, g, b, a;
};

typedef struct GariRGBA_ GariRGBA; 

// gets the RGB components of a color for this surface
GariRGBA gari_surface_getrgb(SDL_Surface * surface, GariColor color) {
  GariRGBA result; 
  SDL_GetRGB(color, surface->format, &result.r, &result.g, &result.b);
  return result;
}

// maps the rgb components of a color to a color for this surface 
GariColor gari_surface_maprgb(SDL_Surface * surface, GariRGBA rgba) { 
  return SDL_MapRGB(surface->format, rgba.r, rgba.g, rgba.b);
}


// Blends a pixel with the one already there to a surface with BPP 8
// taking into account the value of alpha
void gari_surface_rbp8(SDL_Surface * surface, int x, int y, 
                       GariColor color, GariAlpha alpha) {    
  GariPtr   ptr   = gari_surface_pixelptr8(surface, x, y);
  GariColor oldcol= (uint32_t)(*ptr);
  GariRGBA  old   = gari_surface_getrgb(surface, oldcol);
  // Messing with the palette would probably be faster, 
  // but it's cleaner to do it like this.
  GariRGBA col    = gari_surface_getrgb(surface, color);
  GariRGBA nec;
  GariColor newcol;
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
                       GariColor color, GariAlpha alpha) {
  SDL_PixelFormat * format = surface->format;
  PixelPtr  ptr   = gari_surface_pixelptr24(surface, x, y);
  GariRGBA  old, col, nec;
  GariColor newcol;
  old.r     = *ptr.rptr;
  old.g     = *ptr.gptr;
  old.b     = *ptr.bptr;
  old.a     = *ptr.aptr;
  col.r     = (uint8_t)((color >> format->Rshift) & 0xff);
  col.g     = (uint8_t)((color >> format->Gshift) & 0xff);
  col.b     = (uint8_t)((color >> format->Bshift) & 0xff);
  col.a     = (uint8_t)((color >> format->Ashift) & 0xff);
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
                       GariColor color, GariAlpha alpha)  {
  SDL_PixelFormat * format = surface->format;                         
  uint16_t * ptr           = gari_surface_pixelptr16(surface, x, y);
  GariColor old            = (GariColor)(*ptr);
  GariColor newcol         = gari_help_blend(old, color, 
                             format->Rmask, format->Gmask,
                             format->Bmask, format->Amask,
                             alpha);
  *ptr                    = (uint16_t)(newcol);
}

// Blends a pixel with the one already there to a surface with BPP 32
// taking into account the value of alpha
void gari_surface_rbp32(SDL_Surface * surface, int x, int y, 
                       GariColor color, GariAlpha alpha)  {
  SDL_PixelFormat * format = surface->format;                         
  uint32_t * ptr           = gari_surface_pixelptr32(surface, x, y);
  GariColor old            = (GariColor)(*ptr);
  GariColor newcol         = gari_help_blend(old, color, 
                             format->Rmask, format->Gmask,
                             format->Bmask, format->Amask,
                             alpha);
  *ptr                     = (newcol);
}

// Blends a pixel depending on the BytesPerPixel of the target surface
// format. Still doesn't check the x and y coordinates for validity. 
void gari_surface_rnpbpp(SDL_Surface * surface, int x, int y, 
                          GariColor color, GariAlpha alpha) {
  switch (surface->format->BytesPerPixel) {
    case 1: gari_surface_rbp8(surface, x, y, color, alpha); break; 
    case 2: gari_surface_rbp16(surface, x, y, color, alpha); break;
    case 3: gari_surface_rbp24(surface, x, y, color, alpha); break;
    case 4: gari_surface_rbp32(surface, x, y, color, alpha); break;    
  }
}


// Puts a pixel with the given color at the given coordinates
// Takes the clipping rectangle and surface bounds into consideration
// Locks and unlocks the surface if that is needed for drawing
void gari_image_putpixel(GariImage *img, int x, int y, GariColor color) {
  SDL_Surface * s = gari_image_surface(img);
  if(gari_image_pixelclip(img, x, y))    { return ; }
  if(gari_image_pixeloutside(img, x, y)) { return ; }   
  SDL_LockSurface(s);
  gari_surface_rppbpp(s, x, y , color);  
  SDL_UnlockSurface(s); 
}

/*
// Gets the color of a pixel from this surface 
// Returns 0 if the pixel is outside of the clipping rectangle,
// or outside of the bounds of the surface
// Locks and unlocks the surface if that is needed
func (s * Surface) GetPixel(x, y int) (color uint32) {
  if s.PixelClip(x, y) { return 0 }
  s.Lock()
  res := s.InGetPixelBBP(x, y)
  s.Unlock()
  return res 
}

// Blends the color of a pixel from this surface, 
// taking alpha into consideration. 
// Returns 0 if the pixel is outside of the clipping rectangle,
// or outside of the bounds of the surface.
// Locks and unlocks the surface if that is needed for drawing
func (s * Surface) BlendPixel(x, y int, color uint32, alpha uint8) {
  if s.PixelClip(x, y) { return }
  s.Lock()
  s.InBlendPixelBBP(x, y, color, alpha)
  s.Unlock() 
}
*/

/*

// Puts a pixel depending on the BytesPerPixel of the target surface
// format. Still doesn't check the x and y coordinates for validity. 
func (s * Surface) RawBlendPixelBBP(x, y int, color uint32, 
    alpha uint8) {
    switch s.surface.format.BytesPerPixel {
    case 1:
      s.RawBlendPixel8(x, y, color, alpha)
    case 2:
      s.RawBlendPixel16(x, y, color, alpha)
    case 3:
      s.RawBlendPixel24(x, y, color, alpha)
    case 4:  
      s.RawBlendPixel32(x, y, color, alpha)
    default: 
      // Do nothing.
  }
}


// PutPixel for surfaces with 8 Bytes per pixel.
// Unsafe on the wrong BBP.
// It does protect against out of bounds x and y values, 
// hence the name starts with "In". 
// However, it does not take clipping into consideration.  
func (s * Surface) InPutPixel8(x, y int, color uint32) {
  if s.PixelOutside(x, y) { return }
  s.RawPutPixel8(x, y, color)
} 

// PutPixel for surfaces with 16 Bytes per pixel.
// Unsafe on the wrong BBP.
// It does protect against out of bounds x and y values, 
// hence the name starts with "In". 
// However, it does not take clipping into consideration.  
func (s * Surface) InPutPixel16(x, y int, color uint32) {
  if s.PixelOutside(x, y) { return }
  s.RawPutPixel16(x, y, color)
} 

// PutPixel for surfaces with 24 Bytes per pixel.
// Unsafe on the wrong BBP.
// It does protect against out of bounds x and y values, 
// hence the name starts with "In". 
// However, it does not take clipping into consideration.  
func (s * Surface) InPutPixel24(x, y int, color uint32) {
  if s.PixelOutside(x, y) { return }
  s.RawPutPixel24(x, y, color)
} 

// PutPixel for surfaces with 32 Bytes per pixel.
// Unsafe on the wrong BBP.
// It does protect against out of bounds x and y values, 
// hence the name starts with "In". 
// However, it does not take clipping into consideration.  
func (s * Surface) InPutPixel32(x, y int, color uint32) {
  if s.PixelOutside(x, y) { return }
  s.RawPutPixel32(x, y, color)
} 

// PutPixel for surfaces of all depth
// It protects against out of bounds x and y values, 
// hence the name starts with "In". 
// However, it does not take clipping into consideration.  
func (s * Surface) InPutPixelBBP(x, y int, color uint32) {
  if s.PixelOutside(x, y) { return }
  s.RawPutPixelBBP(x, y, color)
}  

// GetPixel for surfaces with 8 Bytes per pixel.
// Unsafe on the wrong BBP.
// It does protect against out of bounds x and y values, 
// hence the name starts with "In". 
// However, it does not take clipping into consideration.
// Returns 0 if x and y are out of bounds  
func (s * Surface) InGetPixel8(x, y int) (color uint32) {
  if s.PixelOutside(x, y) { return 0 }
  return s.RawGetPixel8(x, y)
} 

// GetPixel for surfaces with 16 Bytes per pixel.
// Unsafe on the wrong BBP.
// It does protect against out of bounds x and y values, 
// hence the name starts with "In". 
// However, it does not take clipping into consideration.
// Returns 0 if x and y are out of bounds  
func (s * Surface) InGetPixel16(x, y int) (color uint32) {
  if s.PixelOutside(x, y) { return 0 }
  return s.RawGetPixel16(x, y)
} 

// GetPixel for surfaces with 24 Bytes per pixel.
// Unsafe on the wrong BBP.
// It does protect against out of bounds x and y values, 
// hence the name starts with "In". 
// However, it does not take clipping into consideration.
// Returns 0 if x and y are out of bounds  
func (s * Surface) InGetPixel24(x, y int) (color uint32) {
  if s.PixelOutside(x, y) { return 0 }
  return s.RawGetPixel24(x, y)
} 

// GetPixel for surfaces with 32 Bytes per pixel.
// Unsafe on the wrong BBP.
// It does protect against out of bounds x and y values, 
// hence the name starts with "In". 
// However, it does not take clipping into consideration.
// Returns 0 if x and y are out of bounds  
func (s * Surface) InGetPixel32(x, y int) (color uint32) {
  if s.PixelOutside(x, y) { return 0 }
  return s.RawGetPixel32(x, y)
} 

// PutPixel for surfaces of all depth
// It protects against out of bounds x and y values, 
// hence the name starts with "In". 
// However, it does not take clipping into consideration.
// Returns 0 if x and y are out of bounds  
func (s * Surface) InGetPixelBBP(x, y int) (color uint32) {
  if s.PixelOutside(x, y) { return 0 }
  return s.RawGetPixelBBP(x, y)
}  


// BlendPixel for surfaces with 8 Bytes per pixel.
// Unsafe on the wrong BBP.
// It does protect against out of bounds x and y values, 
// hence the name starts with "In". 
// However, it does not take clipping into consideration.  
func (s * Surface) InBlendPixel8(x, y int, color uint32, alpha uint8) {
  if s.PixelOutside(x, y) { return }
  s.RawBlendPixel8(x, y, color, alpha)
} 

// BlendPixel for surfaces with 16 Bytes per pixel.
// Unsafe on the wrong BBP.
// It does protect against out of bounds x and y values, 
// hence the name starts with "In". 
// However, it does not take clipping into consideration.  
func (s * Surface) InBlendPixel16(x, y int, color uint32, alpha uint8) {
  if s.PixelOutside(x, y) { return }
  s.RawBlendPixel16(x, y, color, alpha)
} 

// BlendPixel for surfaces with 24 Bytes per pixel.
// Unsafe on the wrong BBP.
// It does protect against out of bounds x and y values, 
// hence the name starts with "In". 
// However, it does not take clipping into consideration.  
func (s * Surface) InBlendPixel24(x, y int, color uint32, alpha uint8) {
  if s.PixelOutside(x, y) { return }
  s.RawBlendPixel24(x, y, color, alpha)
} 

// BlendPixel for surfaces with 32 Bytes per pixel.
// Unsafe on the wrong BBP.
// It does protect against out of bounds x and y values, 
// hence the name starts with "In". 
// However, it does not take clipping into consideration.  
func (s * Surface) InBlendPixel32(x, y int, color uint32, alpha uint8) {
  if s.PixelOutside(x, y) { return }
  s.RawBlendPixel32(x, y, color, alpha)
} 

// PutPixel for surfaces of all depth
// It protects against out of bounds x and y values, 
// hence the name starts with "In". 
// However, it does not take clipping into consideration.  
func (s * Surface) InBlendPixelBBP(x, y int, color uint32, 
  alpha uint8) {
  if s.PixelOutside(x, y) { return }
  s.RawBlendPixelBBP(x, y, color, alpha)
}  


// Puts a pixel with the given color at the given coordinates
// Takes the clipping rectangle and surface bounds into consideration
// Locks and unlocks the surface if that is needed for drawing
func (s * Surface) PutPixel(x, y int, color uint32) {
  if s.PixelClip(x, y) { return }
  s.Lock()
  s.InPutPixelBBP(x, y, color)
  s.Unlock() 
}

// Gets the color of a pixel from this surface 
// Returns 0 if the pixel is outside of the clipping rectangle,
// or outside of the bounds of the surface
// Locks and unlocks the surface if that is needed
func (s * Surface) GetPixel(x, y int) (color uint32) {
  if s.PixelClip(x, y) { return 0 }
  s.Lock()
  res := s.InGetPixelBBP(x, y)
  s.Unlock()
  return res 
}

// Blends the color of a pixel from this surface, 
// taking alpha into consideration. 
// Returns 0 if the pixel is outside of the clipping rectangle,
// or outside of the bounds of the surface.
// Locks and unlocks the surface if that is needed for drawing
func (s * Surface) BlendPixel(x, y int, color uint32, alpha uint8) {
  if s.PixelClip(x, y) { return }
  s.Lock()
  s.InBlendPixelBBP(x, y, color, alpha)
  s.Unlock() 
}
*/
