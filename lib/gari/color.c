#include "gari.h"
#define GARI_INTERN_ONLY
#include "gari_intern.h"
 



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

// gets the RGBA components of a color for this surface
GariRGBA gari_surface_getrgba(SDL_Surface * surface, GariColor color) {
  GariRGBA result; 
  SDL_GetRGBA(color, surface->format, &result.r, &result.g, &result.b, &result.a);
  return result;
}

// maps the rgb components of a color to a color for this surface 
GariColor gari_surface_maprgba(SDL_Surface * surface, GariRGBA rgba) { 
  return SDL_MapRGBA(surface->format, rgba.r, rgba.g, rgba.b, rgba.a);
}


GariRGBA * 
gari_rgba_init(GariRGBA * rgba, uint8_t r, uint8_t g, uint8_t b, uint8_t a) {
  if(!rgba) { return NULL; }
  rgba->r = r;
  rgba->g = g;
  rgba->b = b;
  rgba->a = a;
  return rgba;
} 

GariRGBA *
gari_rgba_make(uint8_t r, uint8_t g, uint8_t b, uint8_t a) {
  GariRGBA * res = gari_allocate(GariRGBA);
  return gari_rgba_init(res, r, g ,b , a);
} 


uint8_t gari_rgba_r(GariRGBA rgba) {
  return rgba.r;
} 

uint8_t gari_rgba_g(GariRGBA rgba) {
  return rgba.g;
} 

uint8_t gari_rgba_b(GariRGBA rgba) {
  return rgba.b;
} 


uint8_t gari_rgba_a(GariRGBA rgba) {
  return rgba.a;
} 





