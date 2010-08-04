#include "gari.h"

#define GARI_INTERN_ONLY
#include "gari_intern.h"



/* Constructs drawing information */
GariDraw * gari_draw_init(
                              GariDraw          * draw,
                              GariImage         * image,
                              GariDrawFunction  * func,
                              GariDye color,
                              GariAlpha alpha
                             ) { 
  if(!draw) { return NULL; } 
  draw->image = image;
  draw->draw  = func;
  draw->color = color;
  draw->alpha = alpha;
  return draw;
} 


/** Line, arc, circle drawing, etc. */


void gari_draw_doline(GariDraw * draw, int x1, int y1, int w, int h) {  
  int dx = w;
  int dy = h;
  int i1, i2;
  int x, y;
  int dd, sdx, sdy, px, py;
 
  sdx = (dx < 0 ?  -1 : 1); 
  sdy = (dy < 0 ?  -1 : 1);
  dx = sdx * dx + 1;
  dy = sdy * dy + 1;
  x  = 0;
  y  = 0;
  px = x1;
  py = y1;

  if (dx >= dy) {
    for (x = 0; x < dx; x++) {
      draw->draw(draw, px, py);  
      y += dy;
      if (y >= dx) {
        y -= dx;
        py += sdy;
      }
      px += sdx;
    }
  } else {
    for (y = 0 ; y < dy; y++) {
      draw->draw(draw, px, py);
      x += dx;
      if (x >= dy) {
        x -= dy;
        px += sdx;
      }
      py += sdy;
    }
  }
}

/* Clipping information. */ 
struct GariClipInfo_ {
  int x, y, w, h;
  int draw; // set to false if whole area clipped.
};
typedef struct GariClipInfo_ GariClipInfo;
 

GariClipInfo gari_image_clipline(GariImage * image, 
  int x, int y, int w, int h) {
  GariClipInfo clip = { x, y, w, h, TRUE };
  return clip;
}   


int gari_draw_putpixel(GariDraw * draw, int px, int py) {
  gari_image_putpixel_nolock(draw->image, px, py, draw->color);
  return TRUE;
}

int gari_draw_blendpixel(GariDraw * draw, int px, int py) {
  gari_image_blendpixel_nolock(draw->image, px, py, draw->color, draw->alpha);
  return TRUE;
}


void gari_image_line(GariImage * image, int x, int y, 
                    int w, int h, GariDye color) {
  GariDraw draw;
  gari_draw_init(&draw, image, gari_draw_putpixel, color, GARI_ALPHA_OPAQUE);
  gari_image_lock(image);
  gari_draw_doline(&draw, x, y, w, h);
  gari_image_unlock(image);
}


/* Traces a horizontal line starting at x1, y1, of width w.
   w must be greater than 0; 
*/
void gari_draw_dohline(GariDraw * draw, int x1, int y1, int w) {  
  int px    = x1;
  int py    = y1;
  int stop  = x1 + w ;
  if (w <= 0) { return; }
  for (px = x1; px < stop ; px++) {
    draw->draw(draw, px, py);
  }
}


/* Traces a vertical line starting at x1, y1, of height h, 
   which must be greater than 0. 
*/
void gari_draw_dovline(GariDraw * draw, int x1, int y1, int h) {  
  int px    = x1;
  int py    = y1;
  int stop  = y1 + h;
  if (h <= 0) { return; }
  for (py = y1; py < stop ; py++) {
    draw->draw(draw, px, py);
  }
}


/* Optimized case of drawline, draws horizontal lines only. */
void gari_image_hline_nolock(GariImage * image, int x, int y, 
                    int w, GariDye color) {
  GariDraw draw;
  gari_draw_init(&draw, image, gari_draw_putpixel, color, GARI_ALPHA_OPAQUE);
  // Adjust for negative widths.
  if (w < 0) {
    w = -w;
    x =  x - w;
  } 
  gari_draw_dohline(&draw, x, y, w);
}


/* Optimized case of blendline, blends horizontal lines only. */
void gari_image_blendhline_nolock(GariImage * image, int x, int y, 
                    int w, GariDye color, GariAlpha alpha) {
  GariDraw draw;
  gari_draw_init(&draw, image, gari_draw_blendpixel, color, alpha);
  // Adjust for negative widths.
  if (w < 0) {
    w = -w;
    x =  x - w;
  } 
  gari_draw_dohline(&draw, x, y, w);
}

/* Optimized case of drawline, draws vertical lines only. */
void gari_image_vline_nolock(GariImage * image, int x, int y, 
                    int h, GariDye color) {
  GariDraw draw;
  gari_draw_init(&draw, image, gari_draw_putpixel, color, GARI_ALPHA_OPAQUE);
  // Adjust for negative widths.
  if (h < 0) {
    h = -h;
    y =  y - h;
  } 
  gari_draw_dovline(&draw, x, y, h);
}


/* Optimized case of gari_image_line, draws horizontal 
   lines with a width of w only. */
void gari_image_hline(GariImage * image, int x, int y, int w, GariDye color) 
{
  gari_image_lock(image);
  gari_image_hline_nolock(image, x, y, w, color);
  gari_image_unlock(image);
}

/* Optimized case of gari_image_line, draws vertical 
   lines with a height h only. */
void gari_image_vline(GariImage * image, int x, int y, int h, GariDye color) 
{
  gari_image_lock(image);
  gari_image_vline_nolock(image, x, y, h, color);
  gari_image_unlock(image);
}


void gari_draw_doslab(GariDraw * draw, int x, int y, int w, int h) {
  int stopx, stopy, xi, yi;
  stopy = y + h;
  printf("doslab: y, w, stopx, stopy: %d %d %d \n", y, h, stopy);
  for( yi = y ; yi < stopy ; yi++) {
    gari_draw_dohline(draw, x, yi, w);
  }
} 

/** Draws a blended slab, which is a rectange, on the image. */
void gari_image_blendslab( GariImage * image, int x, int y, int w, int h,  
                      GariDye color, GariAlpha alpha) {
  GariDraw draw;
  gari_draw_init(&draw, image, gari_draw_blendpixel, color, alpha);
  // Adjust for negative widths and heights.
  if (w < 0) {
    w = -w;
    x =  x - w;
  }
  if (h < 0) {
    h = -h;
    y =  y - h;
  } 
  gari_image_lock(image); 
  gari_draw_doslab(&draw, x, y, w, h);
  gari_image_unlock(image);
} 


/** Draws a slab, which is a filled rectange, on the image. */
void gari_image_slab( GariImage * image, int x, int y, int w, int h, 
                      GariDye color) {
    GariDraw draw;
  gari_draw_init(&draw, image, gari_draw_putpixel, color, GARI_ALPHA_OPAQUE);
  // Adjust for negative widths and heights.
  if (w < 0) {
    w = -w;
    x =  x - w;
  }
  if (h < 0) {
    h = -h;
    y =  y - h;
  } 
  gari_image_lock(image); 
  gari_draw_doslab(&draw, x, y, w, h);
  gari_image_unlock(image);
} 


/** Draws a box, which is a rectange, on the image. */
void gari_image_box( GariImage * image, int x, int y, int w, int h,  
                      GariDye color) {
  gari_image_lock(image);
  // Two horizontal lines...
  gari_image_hline_nolock(image, x, y    , w, color);
  gari_image_hline_nolock(image, x, y + h, w, color + 10);
  // And two vertical ones.
  gari_image_vline_nolock(image, x    , y, h, color);
  gari_image_vline_nolock(image, x + w, y, h, color); 
  gari_image_unlock(image);
} 



/** Copies a pixel from one image to another, without locking, but mapping 
the colors as needed **/
void gari_image_copypixel_nolock(GariImage * dst, int dstx, int dsty,
                                 GariImage * src, int srcx, int srcy ) {
  GariDye srcpix, dstpix;
  srcpix = gari_image_getpixel_nolock(src, srcx, srcy);
  dstpix = gari_image_mapcolor(dst,  src, srcpix);
  gari_image_putpixel_nolock(dst, dstx, dsty, dstpix);
}  


/** Blits (a part of) an image to another one, 
    scaling it in integer increments by simple repitition 
*/
void gari_image_iscaleblit(GariImage * dst, int dstx, int dsty, 
                                      int xmul, int ymul, GariImage * src,
                                      int srcx, int srcy, int srch, int srcw) {
  int xin, yin, xout, yout, stopx, stopy; 
  GariDye pixel;
  if ((xmul < 1) || (ymul < 1 )) { return; } 
  gari_image_lock(dst);
  gari_image_lock(src);  
  stopx = dstx + (srcw*xmul) ; stopy = dsty + (srch*ymul) ;   
    
  for (yout  = dsty ; yout < stopy; yout++)  {
    yin = srcy + ((yout - dsty) / ymul);
    for (xout  = dstx ; xout < stopx; xout++)  {
      xin = srcx + (( xout - dstx) / xmul);
      gari_image_copypixel_nolock(dst, xout, yout, src, xin, yin);
    }
  } 
  
  gari_image_unlock(src);
  gari_image_unlock(dst);
}


/** Arbitrary scaling, without interpolation. Uses linear stepping. */
void gari_image_scaleblit(GariImage * dst, int dstx, int dsty, 
                                      int dsth, int dstw, 
                                      GariImage * src,
                                      int srcx, int srcy, 
                                      int srch, int srcw) {
  int xin, yin, xout, yout, stopx, stopy, xrest, yrest, xrat, yrat, xmod, ymod; 
  GariDye pixel;
  if ((dsth < 1) || (dstw < 1 )) { return; } 
  gari_image_lock(dst);
  gari_image_lock(src);  
  stopx = dstx + dstw - 1    ; stopy = dsty + dsth  - 1   ;
  xin   = srcx               ; yin   = srcy               ;
  xrat  = srcw / dstw        ; yrat  = srch / dsth        ;
  yrest = 0                  ; xrest = 0                  ;
  xmod  = srch % dsth        ; ymod  = srcw % dstw        ;  
  for (yout  = dsty ; yout < stopy; yout++)  {
    yrest += ymod;
    yin   += yrat;
    if (yrest >= dsth ) {
      yin    ++;
      yrest = 0;
    }     
    xin   = srcx;
    xrest = 0;
    for (xout  = dstx ; xout < stopx; xout++)  {
      xrest += xmod;
      xin   += xrat; 
      if (xrest >= dstw ) {
        xin   ++;
        xrest = 0;
      }         
      gari_image_copypixel_nolock(dst, xout, yout, src, xin, yin);
    }
  } 
  
  gari_image_unlock(src);
  gari_image_unlock(dst);
}


