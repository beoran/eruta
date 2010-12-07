#include "gari.h"
#define GARI_INTERN_ONLY
#include "gari_intern.h"

/*
The Uint32 used in SDL internaly as colors may or may not contain an alpha 
channel, and are specific to the color depth of the image on which they are 
drawn. But, this type of color is fast to draw as it needs no conversion  
(especially on an 8 bit surface, e.g.). 

For this reason, I have termed this fast uint32_t "a Dye". It can be used to 
color a pixel, but what that color will be depends on the surface the dye is 
applied to. :)

GariColor is the color modeled as a surface-independed RGBA value, with A 
being opaque by default. 

In the public Gari interface, from now on, GariDye wil be avoided in favour of 
GariColor. 

ruby -pi -w -e 'gsub!(/GariRGBA/, "GariColor")' helped me here a lot. :)




*/



// gets the RGB components of a color for this surface
GariColor gari_surface_getrgb(SDL_Surface * surface, GariDye color) {
  GariColor result; 
  SDL_GetRGB(color, surface->format, &result.r, &result.g, &result.b);
  result.a = GARI_ALPHA_OPAQUE;
  return result;
}

// maps the rgb components of a color to a color for this surface 
GariDye gari_surface_maprgb(SDL_Surface * surface, GariColor rgba) { 
  return SDL_MapRGB(surface->format, rgba.r, rgba.g, rgba.b);
}

// gets the RGBA components of a color for this surface
GariColor gari_surface_getrgba(SDL_Surface * surface, GariDye color) {
  GariColor result; 
  SDL_GetRGBA(color, surface->format, &result.r, &result.g, &result.b, &result.a);
  return result;
}

// maps the rgb components of a color to a color for this surface 
GariDye gari_surface_maprgba(SDL_Surface * surface, GariColor rgba) { 
  return SDL_MapRGBA(surface->format, rgba.r, rgba.g, rgba.b, rgba.a);
}


GariColor * 
gari_rgba_init(GariColor * rgba, uint8_t r, uint8_t g, uint8_t b, uint8_t a) {
  if(!rgba) { return NULL; }
  rgba->r = r;
  rgba->g = g;
  rgba->b = b;
  rgba->a = a;
  return rgba;
} 


GariColor * gari_color_allocate() {
  GariColor * res = GARI_ALLOCATE(GariColor);
  return res;
}

// Frees the memory associated with a GariColor. Returns NULL (guaranteed).
GariColor * gari_color_free(GariColor * color) {
  GARI_FREE(color);
  return NULL;
}

// Allocates a new color and initializes it with the given r, g, b and a values
GariColor * gari_color_rgba(uint8_t r, uint8_t g, uint8_t b, uint8_t a) {
  GariColor * res = gari_color_allocate();
  return gari_rgba_init(res, r, g , b, a);  
} 

// Allocates a new color and initializes it with the given r, g, b values
// a will be set to GARI_ALPHA_OPAQUE
GariColor *
gari_color_rgb(uint8_t r, uint8_t g, uint8_t b) {  
  return gari_color_rgba(r, g, b, GARI_ALPHA_OPAQUE);
} 


GariColor gari_colora(uint8_t r, uint8_t g, uint8_t b, uint8_t a) {
  GariColor res;
  gari_rgba_init(&res, r, g ,b , a);
  return res;
} 

GariColor gari_color(uint8_t r, uint8_t g, uint8_t b) {  
  return gari_colora(r, g , b, GARI_ALPHA_OPAQUE);
} 

uint8_t gari_color_r(GariColor * rgba) {
  if(!rgba) return 0;
  return rgba->r;
} 

uint8_t gari_color_g(GariColor * rgba) {
  if(!rgba) return 0;
  return rgba->g;
} 

uint8_t gari_color_b(GariColor * rgba) {
  if(!rgba) return 0;
  return rgba->b;
} 

uint8_t gari_color_a(GariColor * rgba) {
  if(!rgba) return 0;
  return rgba->a;
} 

/* 
* Converts a GariColor to a gari dye for the given image. 
* If the color's A is solid, then it uses SDL_MapRGB internally.
* otherwise, it uses SDL_MapRGBA internally. 
*/
GariDye gari_color_dye(GariColor color, GariImage * image) { 
  if (color.a == GARI_ALPHA_OPAQUE) { 
    return gari_surface_maprgb(GARI_IMAGE_SURFACE(image), color);
  }
  return gari_surface_maprgba(GARI_IMAGE_SURFACE(image), color);
}


/** Converts a GariDye to a GariColor for the given image. */
GariColor gari_dye_color(GariDye dye, GariImage * image) {
  SDL_Surface *surf = gari_image_surface(image);
  if ((surf->format) && (surf->format->Amask)) {
    return gari_surface_getrgba(surf, dye); 
  }
  return gari_surface_getrgb(surf, dye);
}

/** Checks if two gariColors are equal. Colors will sort by ascending a,
r, g and b */
int gari_color_cmp(GariColor c1, GariColor c2) {
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




