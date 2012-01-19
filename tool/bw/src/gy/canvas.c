#include <stdlib.h>

#include "gy.h"
#define GY_INTERN_ONLY
#include "gy_intern.h"

#if SDL_VERSION_ATLEAST(1, 3,0)


/** We lie to the compiler about GyCanvas. We say it's a struct, but just
* cast a pointer from SDL_Renderer to a pointer to GyCanvas and 
* prevent the overhead of a struct wrapper. 
* This function does the dirty deed of casting a SDL_Renderer pointer to a 
* GyCanvas pointer. 
*/
GyCanvas * gycanvas_wrap(SDL_Renderer * renderer) {
  return GY_CANVAS_WRAP(renderer);
}

/**
* Gets the SDL_Texture associated with this GyTexture
*  
*/
SDL_Renderer * gycanvas_unwrap(GyCanvas * canvas) {
  return GY_CANVAS_UNWRAP(canvas);
}

void gycanvas_free(GyTexture * self) {
  if (!self) { return; }
  SDL_DestroyRenderer(GY_CANVAS_UNWRAP(self));
}

/*
int gydraw_putpixel(GyDraw * draw, int px, int py) {
  draw->putpixel(draw->image, px, py, draw->dye);
  return TRUE;
}

int gydraw_blendpixel(GyDraw * draw, int px, int py) {
  draw->blendpixel(draw->image, px, py, draw->dye, draw->alpha);
  return TRUE;
}
*/

/** Sets the color to be used when drawing to the canvas. */
void gycanvas_color_(GyCanvas * self, GyColor color) {
  SDL_SetRenderDrawColor(GY_CANVAS_UNWRAP(self), 
             color.r, color.g, color.b, color.a);
}

/** Gets the color to be used when drawing to the canvas. */
GyColor gycanvas_color(GyCanvas * self) {
  GyColor color;
  SDL_GetRenderDrawColor(GY_CANVAS_UNWRAP(self), 
             &color.r, &color.g, &color.b, &color.a);
  return color;
}


/** Gets the blend mode used whe drawing to the canvas. */
GyBlendMode gycanvas_blendmode(GyCanvas * self) {
  SDL_BlendMode        mode = 0;
  SDL_GetRenderDrawBlendMode(GY_CANVAS_UNWRAP(self),&mode);
  return (GyBlendMode) mode;
}
  
/** Sets the blend mode used whe drawing to the canvas. */
void gycanvas_blendmode_(GyCanvas * self, GyBlendMode mode) {  
  SDL_SetRenderDrawBlendMode(GY_CANVAS_UNWRAP(self), (SDL_BlendMode)mode);
}

/** Clears the canvas with the current draw color. Ignores the viewport. */
void gycanvas_clear(GyCanvas * self) {
  SDL_RenderClear(GY_CANVAS_UNWRAP(self));
}
 
 /** Sets the current draw color to color, and clears the canvas with it.
 *   Ignores the viewport. 
 */
void gycanvas_clearcolor(GyCanvas * self, GyColor color) {
  gycanvas_color_(self, color);
  gycanvas_clear(self);
}

/** Sets the view port of the canvas, that is the clipping rectangle
outside which drawing will be clipped. */
GyCanvas * gycanvas_viewport_(GyCanvas * self, int x, int y, int w, int h) {
  SDL_Rect rect = { x, y , w,  h};
  if(SDL_RenderSetViewport(GY_CANVAS_UNWRAP(self), &rect) < 0) {
    return NULL;
  } 
  return self;
}

/** Gets the view port of the canvas, that is the clipping rectangle
outside which drawing will be clipped. */
GyCanvas * gycanvas_viewport(GyCanvas * self, int *x, int *y, int *w, int *h) {
  SDL_Rect rect = { 0, 0 , 0, 0};
  SDL_RenderGetViewport(GY_CANVAS_UNWRAP(self), &rect);
  if(x) (*x) = rect.x;
  if(y) (*y) = rect.y;
  if(w) (*w) = rect.w;
  if(h) (*h) = rect.h;
  return self;
}


/** Draws an arbitrary line on the canvas in the current draw color.
 W and h are the offsets from x and y and can be negative. 
 */
void gycanvas_line(GyCanvas * self, int x, int y, int w, int h) {
  int x1 = x;
  int y1 = y;
  int x2 = x + w - 1; 
  /* this - 1 is suspicious but seems to be needed to make boxes 
    and lines match. when drawn with equal w and h. */ 
  int y2 = y + h - 1;
  SDL_RenderDrawLine(GY_CANVAS_UNWRAP(self), x1, y1, x2, y2);
}

/** Draws an arbitrary line on the canvas in the given color, 
which is set as the draw color.  */
void gycanvas_linecolor(GyCanvas * self, int x, int y, int w, int h, 
                        GyColor color) {
  gycanvas_color_(self, color);
  gycanvas_line(self, x, y, w, h);
}


// Draws a pixel on the canvas.
void gycanvas_dot(GyCanvas * self, int x, int y) {
  SDL_RenderDrawPoint(GY_CANVAS_UNWRAP(self), x, y);
}

/** Draws a pixel on the canvas in the given color, which is set as 
the draw color.  */
void gycanvas_dotcolor(GyCanvas * self, int x, int y, GyColor color) {
  gycanvas_color_(self, color);
  gycanvas_dot(self, x, y);
}

/** Draws a box (the outline of a rectangle) on the canvas in the current draw color. W and h are the offsets from x and y and can be negative. */
void gycanvas_box(GyCanvas * self, int x, int y, int w, int h) {
  SDL_Rect rect = { x, y, w, h};
  SDL_RenderDrawRect(GY_CANVAS_UNWRAP(self), &rect);
}

/** Draws a box (the outline of a rectangle) on the canvas in the given color, 
which is set as the draw color.  */
void gycanvas_boxcolor(GyCanvas * self, int x, int y, int w, int h, 
                        GyColor color) {
  gycanvas_color_(self, color);
  gycanvas_box(self, x, y, w, h);
}

/** Draws a slab (filled rectangle) on the canvas in the current draw color. W and h are the offsets from x and y and can be negative. */
void gycanvas_slab(GyCanvas * self, int x, int y, int w, int h) {
  SDL_Rect rect  = { x, y,  w, h};
  SDL_RenderFillRect(GY_CANVAS_UNWRAP(self), &rect);
}

/** Draws a slab (filled rectangle) on the canvas in the given color, 
which is set as the draw color.  */
void gycanvas_slabcolor(GyCanvas * self, int x, int y, int w, int h, 
                        GyColor color) {
  gycanvas_color_(self, color);
  gycanvas_slab(self, x, y, w, h);
}
 
/** Updates the canvas with the drawing commands sent before. */ 
void gycanvas_update(GyCanvas * self) {
  SDL_RenderPresent(GY_CANVAS_UNWRAP(self));
}
 
/** Scales a part of an texture to the canvas, scaling it arbitrarily 
to the wanted size.
*/
void gycanvas_scalepart(GyCanvas * self, int dstx, int dsty, 
                                        int dsth, int dstw, 
                                        GyTexture * tex,
                                        int srcx, int srcy, 
                                        int srch, int srcw) { 
  SDL_Rect srcrect = { srcx, srcy, srcw, srch };
  SDL_Rect dstrect = { dstx, dsty, dstw, dsth };
  SDL_RenderCopy(GY_CANVAS_UNWRAP(self), GY_TEXTURE_UNWRAP(tex), 
                 &srcrect, &dstrect);
}

/** Puts a part of an texture to the canvas, unscaled. */
void gycanvas_putpart(GyCanvas * self, int dstx, int dsty,
                                        GyTexture * tex,
                                        int srcx, int srcy, 
                                        int srch, int srcw) { 
  SDL_Rect srcrect = { srcx, srcy, srcw, srch };
  SDL_Rect dstrect = { dstx, dsty, srcw, srch };
  SDL_RenderCopy(GY_CANVAS_UNWRAP(self), GY_TEXTURE_UNWRAP(tex), 
                 &srcrect, &dstrect);
}

/** Scales a whole texture to the canvas. */
void gycanvas_scale(GyCanvas * self, int dstx, int dsty,
                                     int dsth, int dstw,
                                     GyTexture * tex) { 
  SDL_Rect dstrect = { dstx, dsty, dsth, dstw };
  SDL_RenderCopy(GY_CANVAS_UNWRAP(self), GY_TEXTURE_UNWRAP(tex), 
                 NULL, &dstrect);
}

/** Puts a whole texture to the canvas, unscaled. */
void gycanvas_put(GyCanvas * self, int dstx, int dsty,
                                     GyTexture * tex) {
  SDL_Rect dstrect = { dstx, dsty, 32, 32 };
  uint32_t format; int  access;  
  gytexture_query(tex, &format, &access, &(dstrect.w), &(dstrect.h));
  SDL_RenderCopy(GY_CANVAS_UNWRAP(self), GY_TEXTURE_UNWRAP(tex), 
                 NULL, &dstrect);
}

 
 #endif

