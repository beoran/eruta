#include "gy.h"
#define GY_INTERN_ONLY
#include "gy_intern.h"

/*
The Uint32 used in SDL internaly as colors may or may not contain an alpha 
channel, and are specific to the color depth of the image on which they are 
drawn. But, this type of color is fast to draw as it needs no conversion  
(especially on an 8 bit surface, e.g.). 

For this reason, I have termed this fast uint32_t "a Dye". It can be used to 
color a pixel, but what that color will be depends on the surface the dye is 
applied to. :)

GyColor is the color modeled as a surface-independed RGBA value, with A 
being opaque by default. 

In the public Gy interface, from now on, GyDye wil be avoided in favour of 
GyColor. 

ruby -pi -w -e 'gsub!(/GyRGBA/, "GyColor")' helped me here a lot. :)




*/
// gets the format of the surface. Returns null if surface is null
SDL_PixelFormat * gysurface_format(SDL_Surface * surface) {
  if(!surface) return NULL;
  return surface->format;
}


// gets the RGB components of a color for this surface
GyColor gysurface_getrgb(SDL_Surface * surface, GyDye color) {
  GyColor result; 
  SDL_GetRGB(color, gysurface_format(surface), 
              &result.r, &result.g, &result.b);
  result.a = GY_ALPHA_OPAQUE;
  return result;
}

// maps the rgb components of a color to a color for this surface 
GyDye gysurface_maprgb(SDL_Surface * surface, GyColor rgba) { 
  return SDL_MapRGB(gysurface_format(surface), rgba.r, rgba.g, rgba.b);
}

// gets the RGBA components of a color for this surface
GyColor gysurface_getrgba(SDL_Surface * surface, GyDye color) {
  GyColor result; 
  SDL_GetRGBA(color, gysurface_format(surface), 
              &result.r, &result.g, &result.b, &result.a);
  return result;
}

// maps the rgb components of a color to a color for this surface 
GyDye gysurface_maprgba(SDL_Surface * surface, GyColor rgba) { 
  return SDL_MapRGBA(gysurface_format(surface), 
                     rgba.r, rgba.g, rgba.b, rgba.a);
}


GyColor * 
gyrgba_init(GyColor * rgba, uint8_t r, uint8_t g, uint8_t b, uint8_t a) {
  if(!rgba) { return NULL; }
  rgba->r = r;
  rgba->g = g;
  rgba->b = b;
  rgba->a = a;
  return rgba;
} 


GyColor * gycolor_allocate() {
  GyColor * res = GY_ALLOCATE(GyColor);
  return res;
}

// Frees the memory associated with a GyColor. Returns NULL (guaranteed).
GyColor * gycolor_free(GyColor * color) {
  GY_FREE(color);
  return NULL;
}

// Allocates a new color and initializes it with the given r, g, b and a values
GyColor * gycolor_rgba(uint8_t r, uint8_t g, uint8_t b, uint8_t a) {
  GyColor * res = gycolor_allocate();
  return gyrgba_init(res, r, g , b, a);  
} 

// Allocates a new color and initializes it with the given r, g, b values
// a will be set to GY_ALPHA_OPAQUE
GyColor *
gycolor_rgb(uint8_t r, uint8_t g, uint8_t b) {  
  return gycolor_rgba(r, g, b, GY_ALPHA_OPAQUE);
} 


GyColor gycolora(uint8_t r, uint8_t g, uint8_t b, uint8_t a) {
  GyColor res;
  gyrgba_init(&res, r, g ,b , a);
  return res;
} 

GyColor gycolor(uint8_t r, uint8_t g, uint8_t b) {  
  return gycolora(r, g , b, GY_ALPHA_OPAQUE);
} 

uint8_t gycolor_r(GyColor * rgba) {
  if(!rgba) return 0;
  return rgba->r;
} 

uint8_t gycolor_g(GyColor * rgba) {
  if(!rgba) return 0;
  return rgba->g;
} 

uint8_t gycolor_b(GyColor * rgba) {
  if(!rgba) return 0;
  return rgba->b;
} 

uint8_t gycolor_a(GyColor * rgba) {
  if(!rgba) return 0;
  return rgba->a;
} 

/* 
* Converts a GyColor to a gy dye for the given image. 
* If the color's A is solid, then it uses SDL_MapRGB internally.
* otherwise, it uses SDL_MapRGBA internally. 
*/
GyDye gycolor_dye(GyColor color, GyImage * image) { 
  if (color.a == GY_ALPHA_OPAQUE) { 
    return gysurface_maprgb(GY_IMAGE_SURFACE(image), color);
  }
  return gysurface_maprgba(GY_IMAGE_SURFACE(image), color);
}


/** Converts a GyDye to a GyColor for the given image. */
GyColor gydye_color(GyDye dye, GyImage * image) {
  SDL_Surface *surf       = gyimage_surface(image);
  SDL_PixelFormat * format= gysurface_format(surf);
  if ((format) && (format->Amask)) {
    return gysurface_getrgba(surf, dye); 
  }
  return gysurface_getrgb(surf, dye);
}

/** Optimizes a color for use with the given image. For 24 or 32 bpp images 
this does nothing. However, other image depths, such as 16, 15 or 8 bits don't have enough resolution to display 3 or 4 rgb(a) components correctly, so the 
colors used are reduced. All in all, this fuction is a round-trip between 
gycolor_dye and gydye_color.  
*/
GyColor gycolor_optimize(GyColor color, GyImage * image) {
  GyDye dye = gycolor_dye(color, image); 
  return gydye_color(dye, image); 
}


/** Checks if two gyColors are equal. Colors will sort by ascending a,
r, g and b */
int gycolor_cmp(GyColor c1, GyColor c2) {
  if (c1.a < c2.a) return -1;
  if (c1.a > c2.a) return  1;
  if (c1.r < c2.r) return -1;
  if (c1.r > c2.r) return  1;
  if (c1.b < c2.b) return -1;
  if (c1.b > c2.b) return  1;
  if (c1.g < c2.g) return -1;
  if (c1.g > c2.g) return  1;
  return 0;
} 


const GyColor GyWhite = { 255, 255, 255, 255 };
const GyColor GyBlack = {   0,   0,   0, 255 };






