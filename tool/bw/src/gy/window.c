#include <stdlib.h>

#include "gy.h"
#define GY_INTERN_ONLY
#include "gy_intern.h"

#if SDL_VERSION_ATLEAST(1, 3,0)

/** Window handling functions.  */

struct GyWindow_ {
  SDL_Window   * window;
  SDL_Renderer * renderer;
}; 

GyWindow * gywindow_alloc() {
  return GY_MALLOC(sizeof(GyWindow));
}

GyWindow * gywindow_free(GyWindow * self) {
  gywindow_done(self);
  GY_FREE(self);
  return NULL;
}

SDL_Window * gywindow_window(GyWindow * self) {
  if(!self) return NULL;
  return self->window;
}

SDL_Renderer * gywindow_renderer(GyWindow * self) {
  if(!self) return NULL;
  return self->renderer;
}

SDL_Window * gywindow_window_(GyWindow * self, SDL_Window * window) {
  if(!self) return NULL;
  return self->window = window;
}

SDL_Renderer * gywindow_renderer_(GyWindow * self, SDL_Renderer * renderer) {
  if(!self) return NULL;
  return self->renderer = renderer;
}

/** Cleans up the renderer for this window. */
GyWindow * gywindow_donerenderer(GyWindow * self) {
  SDL_Renderer   * renderer   = gywindow_renderer(self);
  if(renderer) {
    SDL_DestroyRenderer(renderer);
    gywindow_renderer_(self, NULL);
  }
  return self;
}

/** Cleans up the window, closing it's renderer and screen window. */
GyWindow * gywindow_done(GyWindow * self) {
  SDL_Window   * window   = gywindow_window(self);
  gywindow_donerenderer(self);
  if (window) {
    SDL_DestroyWindow(window);
    gywindow_window_(self, NULL);
  }
  return self;
}

/** Opens a new window with the given captions and size, and tries to set the display and fullscreen also. The renderer will set to NULL. 
*/
GyWindow * gywindow_init(GyWindow * self, char * title, 
                         int w, int h, int fullscreen) {
/*  SDL_DisplayMode want_mode;
  SDL_DisplayMode close_mode; */ 
  SDL_Window * window     = NULL;
  Uint32      flags       = SDL_WINDOW_SHOWN;
  if(fullscreen) flags   |= SDL_WINDOW_FULLSCREEN;
  
  window = SDL_CreateWindow(title,  SDL_WINDOWPOS_CENTERED,
               SDL_WINDOWPOS_CENTERED, w , h, flags);
  if(!window) return NULL;  
  gywindow_window_(self, window);
  gywindow_renderer_(self, NULL);
  SDL_RaiseWindow(window);
  return self;
}
 
/** Initializes the software or hardware renderer for this window. */
GyWindow * gywindow_initrenderer(GyWindow * self, int software) {
   SDL_Renderer * renderer = NULL;
   Uint32      flags       = 0;
   SDL_Window * window     = gywindow_window(self);
   if((!self) || (!window)) return NULL;
      
   flags    = (software ? SDL_RENDERER_SOFTWARE : SDL_RENDERER_ACCELERATED);
   renderer = SDL_CreateRenderer(window, -1, flags);
   if(!renderer) {  /* If failed, try again software only this time. */ 
    flags    = SDL_RENDERER_SOFTWARE; 
    renderer = SDL_CreateRenderer(window, -1, flags);
   }
   if(!renderer) return NULL;
   gywindow_renderer_(self, renderer);
   return self;
}

/** Returns a new window that has the given size, and captions,and may be
 fullscreen , and may use a software or accelerated handler.  */
GyWindow * gywindow_new(char * title, int w, int h, int fs, int sw) {
  GyWindow * self;
  self = gywindow_alloc();
  if(!self) return NULL;
  if(!gywindow_init(self, title, w, h, fs)) return gywindow_free(self);
  if(!gywindow_initrenderer(self, sw)) return gywindow_free(self);
  return self;
} 

/** Returns the canvas for the window. */
GyCanvas * gywindow_canvas(GyWindow * self) {
  return GY_CANVAS_WRAP(gywindow_renderer(self));
}

/** Window drawing functions. The render canvas of the wndow should have 
been set up before using these functions.*/

/** Sets the color to be used when drawing to the window. */
void gywindow_color_(GyWindow * self, GyColor color) {
  gycanvas_color_(gywindow_canvas(self), color);
}

/** Gets the color to be used when drawing to the window. */
GyColor gywindow_color(GyWindow * self) {
  return gycanvas_color(gywindow_canvas(self));
}

void gywindow_size(GyWindow * window, int * w,  int * h) { 
  SDL_GetWindowSize(gywindow_window(window), w, h);
}

int gywindow_w(GyWindow * window) {
  int w, h;
  gywindow_size(window, &w,  &h);
  return w;
}

int gywindow_h(GyWindow * window) {
  int w, h;
  gywindow_size(window, &w,  &h);
  return h;
}

/** Updates the window with the drawing commands sent before. */ 
void gywindow_update(GyWindow * self) {
  gycanvas_update(gywindow_canvas(self));
}

#if COMMENT

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
                                     GyTexture * tex) 



#endif


 
#endif