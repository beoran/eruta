#include <stdlib.h>

#include "gy.h"
#define GY_INTERN_ONLY
#include "gy_intern.h"

#if SDL_VERSION_ATLEAST(1, 3,0)


#define GY_TEXTURE_UNWRAP(GYTEXTURE) ((SDL_Texture *)(GYTEXTURE))
#define GY_TEXTURE_WRAP(SDLTEXTURE)  ((GyTexture *)(SDLTEXTURE))


/** We lie to the compiler about GyTexture. We say it's a struct, but just
* cast a pointer from SDL_Texture to a pointer to GyTexture and 
* prevent the overhead of a struct wrapper.
* This function does the dirty deed of casting a SDL_Texture pointer to a 
* GyTexture pointer. 
*/
GyTexture * gytexture_wrap(SDL_Texture * texture) {
  return GY_TEXTURE_WRAP(texture);
}

/**
* Gets the SDL_Texture associated with this GyTexture
*  
*/
SDL_Texture * gytexture_unwrap(GyTexture * texture) {
  return GY_TEXTURE_UNWRAP(texture);
}

void gytexture_free(GyTexture * self) {
  if (!self) { return; } 
  SDL_DestroyTexture(gytexture_unwrap(self));
}

GyTexture * gytexture_new(GyCanvas * canvas, GyImage * image) {
  SDL_Texture * texture; 
  texture = SDL_CreateTextureFromSurface(
              GY_CANVAS_UNWRAP(canvas),    GY_IMAGE_SURFACE(image));
  return GY_TEXTURE_WRAP(texture);
}


GyTexture * gytexture_query(GyTexture * self, uint32_t * format, 
                            int * access, int *w, int * h) {
  if(SDL_QueryTexture(GY_TEXTURE_UNWRAP(self), format, access, w, h) < 0) {
    return NULL;
  }
  return self;
}

int gytexture_w(GyTexture * self) {
  uint32_t format = 0;
  int access = 0, w = 0, h = 0;
  gytexture_query(self, &format, &access, &w, &h);
  return w;
}

int gytexture_h(GyTexture * self) {
  uint32_t format = 0;
  int access = 0, w = 0, h = 0;
  gytexture_query(self, &format, &access, &w, &h);
  return h;
}

uint32_t gytexture_format(GyTexture * self) {
  uint32_t format = 0;
  int access = 0, w = 0, h = 0;
  gytexture_query(self, &format, &access, &w, &h);
  return format;
}

int gytexture_access(GyTexture * self) {
  uint32_t format = 0;
  int access = 0, w = 0, h = 0;
  gytexture_query(self, &format, &access, &w, &h);
  return access;
}

int gytexture_color_(GyTexture * self, uint8_t r, uint8_t g, uint8_t b) {
  return SDL_SetTextureColorMod(GY_TEXTURE_UNWRAP(self), r, g, b);
}  

int gytexture_color(GyTexture * self, uint8_t *r, uint8_t *g, uint8_t *b) {
  return SDL_GetTextureColorMod(GY_TEXTURE_UNWRAP(self), r, g, b);
}  

int gytexture_alpha_(GyTexture * self, uint8_t a) {
  return SDL_SetTextureAlphaMod(GY_TEXTURE_UNWRAP(self), a);
}  

int gytexture_alpha(GyTexture * self, uint8_t *a) {
  return SDL_GetTextureAlphaMod(GY_TEXTURE_UNWRAP(self), a);
}

/** Sets the blend mode of the texture. */
GyTexture * gytexture_blendmode_(GyTexture * self, GyBlendMode blendmode) {
  int res = SDL_SetTextureBlendMode(GY_TEXTURE_UNWRAP(self),
                                (SDL_BlendMode) blendmode);
  return (res < 0 ? NULL : self);
}

/** Gets the blend mode of the texture. */
GyBlendMode gytexture_blendmode(GyTexture * self) {
 SDL_BlendMode blendmode;
 int res = SDL_GetTextureBlendMode(GY_TEXTURE_UNWRAP(self), &blendmode);
  return (res < 0 ? GyBlendModeError : (GyBlendMode)blendmode);
}










#endif
