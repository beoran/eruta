#include "gy.h"

#define GY_INTERN_ONLY
#include "gy_intern.h"

#include <math.h>

/* Constructs drawing information */
GyDraw * gydraw_init(GyDraw          * draw,
                          GyImage         * image,
                          GyDrawFunction  * func,
                          GyColor color,
                          GyAlpha alpha
                         ) { 
  if(!draw) { return NULL; } 
  draw->image = image;
  draw->draw  = func;
  draw->color = color;
  if(draw->image) {      
    draw->dye        = gycolor_dye(draw->color, draw->image);
    // We use these function pointers to speed up drawing somewhat.
    // The function pointerwill match the color depth of the image, so
    // we can avoid a case statement, at the expendse of a pointer 
    // dereference. The latter should be faster on most architectures.    
    draw->putpixel   = gyimage_putpixelfunc_nl(draw->image); 
    draw->getpixel   = gyimage_getpixelfunc_nl(draw->image);
    draw->blendpixel = gyimage_blendpixelfunc_nl(draw->image);
  } else {
    draw->dye = 0;
  } 
  draw->alpha = alpha;
  return draw;
} 


/** Gets the image of the draw info. */
GyImage * gydraw_image(GyDraw * draw) { 
  return GY_DRAW_IMAGE(draw);
}

/** Retuns true of the point at x, y is out of bounds for this
draw info , false if not. */
int gydraw_outside(GyDraw * draw, int x, int y) {
  return GY_DRAW_OUTSIDE(draw, x, y);
} 


/** Line, arc, circle drawing, etc. */
void gydraw_doline(GyDraw * draw, int x1, int y1, int w, int h) {  
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
struct GyClipInfo_ {
  int x, y, w, h;
  int draw; // set to false if whole area clipped.
};
typedef struct GyClipInfo_ GyClipInfo;
 

GyClipInfo gyimage_clipline(GyImage * image, 
  int x, int y, int w, int h) {
  GyClipInfo clip = { x, y, w, h, TRUE };
  return clip;
}   


int gydraw_putpixel(GyDraw * draw, int px, int py) {
  draw->putpixel(draw->image, px, py, draw->dye);
  return TRUE;
}

int gydraw_blendpixel(GyDraw * draw, int px, int py) {
  draw->blendpixel(draw->image, px, py, draw->dye, draw->alpha);
  return TRUE;
}

// Draws an arbitrary line
void gyimage_line(GyImage * image, int x, int y, 
                    int w, int h, GyColor color) {
  GyDraw draw;  
  gydraw_init(&draw, image, gydraw_putpixel, color, GY_ALPHA_OPAQUE);
  gyimage_lock(image);
  gydraw_doline(&draw, x, y, w, h);
  gyimage_unlock(image);
}

// Blends an arbitrary line
void gyimage_blendline(GyImage * image, int x, int y, 
                    int w, int h, GyColor color) {
  GyDraw draw;  
  gydraw_init(&draw, image, gydraw_blendpixel, color, GY_ALPHA_OPAQUE);
  gyimage_lock(image);
  gydraw_doline(&draw, x, y, w, h);
  gyimage_unlock(image);
}


/* Traces a horizontal line starting at x1, y1, of width w.
   w must be greater than 0; 
*/
void gydraw_dohline(GyDraw * draw, int x1, int y1, int w) {  
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
void gydraw_dovline(GyDraw * draw, int x1, int y1, int h) {  
  int px    = x1;
  int py    = y1;
  int stop  = y1 + h;
  if (h <= 0) { return; }
  for (py = y1; py <= stop ; py++) {
    draw->draw(draw, px, py);
  }
}


/* Optimized case of drawline, draws horizontal lines only. */
void gyimage_hline_nolock(GyImage * image, int x, int y, 
                    int w, GyColor color) {
  GyDraw draw;
  gydraw_init(&draw, image, gydraw_putpixel, color, GY_ALPHA_OPAQUE);
  // Adjust for negative widths.
  if (w < 0) {
    w = -w;
    x =  x - w;
  } 
  gydraw_dohline(&draw, x, y, w);
}


/* Optimized case of blendline, blends horizontal lines only. */
void gyimage_blendhline_nolock(GyImage * image, int x, int y, 
                    int w, GyColor color) {
  GyDraw draw;
  gydraw_init(&draw, image, gydraw_blendpixel, color, color.a);
  // Adjust for negative widths.
  if (w < 0) {
    w = -w;
    x =  x - w;
  } 
  gydraw_dohline(&draw, x, y, w);
}

/* Optimized case of drawline, draws vertical lines only. */
void gyimage_vline_nolock(GyImage * image, int x, int y, 
                    int h, GyColor color) {
  GyDraw draw;
  gydraw_init(&draw, image, gydraw_putpixel, color, GY_ALPHA_OPAQUE);
  // Adjust for negative heights.  
  if (h < 0) {
    h = -h;
    y =  y - h;
  } 
  gydraw_dovline(&draw, x, y, h);
}

/* Optimized case of blendline, blends vertical lines only. */
void gyimage_blendvline_nolock(GyImage * image, int x, int y, 
                    int h, GyColor color) {
  GyDraw draw;
  gydraw_init(&draw, image, gydraw_blendpixel, color, GY_ALPHA_OPAQUE);
  // Adjust for negative heights.  
  if (h < 0) {
    h = -h;
    y =  y - h;
  } 
  gydraw_dovline(&draw, x, y, h);
}


/* Optimized case of gyimage_line, draws horizontal 
   lines with a width of w only. */
void gyimage_hline(GyImage * image, int x, int y, int w, GyColor color) 
{
  gyimage_lock(image);
  gyimage_hline_nolock(image, x, y, w, color);
  gyimage_unlock(image);
}

/* Optimized case of gyimage_line, draws vertical 
   lines with a height h only. */
void gyimage_vline(GyImage * image, int x, int y, int h, GyColor color) 
{
  gyimage_lock(image);
  gyimage_vline_nolock(image, x, y, h, color);
  gyimage_unlock(image);
}


void gydraw_doslab(GyDraw * draw, int x, int y, int w, int h) {
  int stopx, stopy, xi, yi;
  stopy = y + h;
  // printf("doslab: y, w, stopx, stopy: %d %d %d \n", y, h, stopy);
  for( yi = y ; yi < stopy ; yi++) {
    gydraw_dohline(draw, x, yi, w);
  }
} 

// Helps to adjust negatives
static void adjust_negative(int * u, int * v) {
  if(*u < 0) {
    (*u) =    - (*u);
    (*v) = *v - (*u);
  }
}   

#define ADJUST_NEGATIVE(U, V) adjust_negative(&U, &V)

/** Draws a blended slab, which is a rectange, on the image. */
void gyimage_blendslab(GyImage * image, int x, int y, int w, int h,  
                      GyColor color) {
  GyDraw draw;
  gydraw_init(&draw, image, gydraw_blendpixel, color, color.a);
  // Adjust for negative widths and heights.
  ADJUST_NEGATIVE(w, x);
  ADJUST_NEGATIVE(h, y);
  gyimage_lock(image); 
  gydraw_doslab(&draw, x, y, w, h);
  gyimage_unlock(image);
} 


/** Draws a slab, which is a filled rectange, on the image. */
void gyimage_slab( GyImage * image, int x, int y, int w, int h, 
                      GyColor color) {  
  GyDraw draw;  
  gydraw_init(&draw, image, gydraw_putpixel, color, GY_ALPHA_OPAQUE);
   
  // Adjust for negative widths and heights.
  ADJUST_NEGATIVE(w, x);
  ADJUST_NEGATIVE(h, y);
  gyimage_lock(image); 
  gydraw_doslab(&draw, x, y, w, h);
  gyimage_unlock(image);
} 


/** Draws a box, which is an open rectange, on the image. */
void gyimage_box( GyImage * image, int x, int y, int w, int h,  
                      GyColor color) {
  gyimage_lock(image);
  // Two horizontal lines...
  gyimage_hline_nolock(image, x, y    , w, color);
  gyimage_hline_nolock(image, x, y + h, w, color);
  // And two vertical ones.
  gyimage_vline_nolock(image, x    , y, h, color);
  gyimage_vline_nolock(image, x + w, y, h, color); 
  gyimage_unlock(image);
} 

/** Blends a box, which is an open rectange, on the image. */
void gyimage_blendbox( GyImage * image, int x, int y, int w, int h,  
                      GyColor color) {
  gyimage_lock(image);
  // Two horizontal lines...
  gyimage_blendhline_nolock(image, x, y    , w, color);
  gyimage_blendhline_nolock(image, x, y + h, w, color);
  // And two vertical ones.
  gyimage_blendvline_nolock(image, x    , y, h, color);
  gyimage_blendvline_nolock(image, x + w, y, h, color); 
  gyimage_unlock(image);
} 


/* Traces a circle outline with the midpoint of the circle at x, y, and the radius being radius. Inspired by an algorithm by Kevin Matz. */
void gydraw_dohoop(GyDraw * draw, int x, int y, int radius) {  
    
    int xp, yp, d, delta_e, delta_se;
    if (radius < 0) return; // don't draw if radius is negative.  

    xp        = -1;
    yp        = radius;
    d         = 1 - radius;
    delta_e   = -1;
    delta_se  = (-radius << 1) + 3;

    while (yp > xp) {
        delta_e += 2;
        xp++;

        if (d < 0) {
            d += delta_e;
            delta_se += 2;
        } else {
            d += delta_se;
            delta_se += 4;
            yp--;
        }

        draw->draw(draw, x - xp, y + yp);
        draw->draw(draw, x - xp, y - yp);
        draw->draw(draw, x + xp, y + yp);
        draw->draw(draw, x + xp, y - yp);

        draw->draw(draw, x - yp, y + xp); // left side
        draw->draw(draw, x - yp, y - xp);         
        draw->draw(draw, x + yp, y + xp); // right side
        draw->draw(draw, x + yp, y - xp); // also
        
    }
}


/** Draws a hoop, that is, an empty circle, on the image. */
void gyimage_hoop(GyImage * image, int x, int y, int d, GyColor color) {
  GyDraw draw;
  int r = d / 2;
  gydraw_init(&draw, image, gydraw_putpixel, color, GY_ALPHA_OPAQUE);
  gyimage_lock(image);
  gydraw_dohoop(&draw, x + r, y + r, r);
  gyimage_unlock(image);
}

/** Blends a hoop, that is, an empty circle, on the image. */
void gyimage_blendhoop(GyImage * image, int x, int y, int d, GyColor color) {
  GyDraw draw;
  int r = d / 2;
  gydraw_init(&draw, image, gydraw_blendpixel, color, color.a);
  gyimage_lock(image);
  gydraw_dohoop(&draw, x + r, y + r, r);
  gyimage_unlock(image);
}


/* Traces a circle surface with the midpoint of the circle at x, y, and the 
radius being radius.  Calls gydraw_dohline. Does not handle blends well, 
and small circles are deformed. But it is faster. 
*/
void gydraw_dodisk_fast(GyDraw * draw, int x, int y, int radius) {  
  int xp, yp, d, delta_e, delta_se, w, oldy;
  
  if (radius < 0) return; // don't draw if radius is negative.

  xp = -1;
  yp = radius;
  d = 1 - radius;
  delta_e = -1;
  delta_se = (-radius << 1) + 3;
  // oldy     = y - yp - 1; // as to make it unequal
  // oldy     = y - yp; 

  while (yp > xp) {
    delta_e += 2;
    xp++;

    if (d < 0) {
      d += delta_e;
      delta_se += 2;
    } else {
      d += delta_se;
      delta_se += 4;
      yp--;
    }
    w = xp * 2;
    
    gydraw_dohline(draw, x - xp, y - yp, w); // top
    gydraw_dohline(draw, x - xp, y + yp, w); // bottom
    
    w = (yp * 2);
    gydraw_dohline(draw, x - yp, y - xp, w); // above middle
    gydraw_dohline(draw, x - yp, y + xp, w); // lower middle
    
    
 }
}

/* Traces a circle surface with the midpoint of the circle at x, y, and the 
radius being radius.  Calls gydraw_dohline and also works fine for blends,
and does not deform small circle, but is slower due to float math. 
*/

void gydraw_dodisk(GyDraw * draw, int x, int y, int radius) {  
  int xp, yp, d, delta_e, delta_se, w, oldy;
  
  if (radius < 0) return; // don't draw if radius is negative.

  xp = -1;
  yp = radius;
  d  = 1 - radius;
  delta_e = -1;
  delta_se = (-radius << 1) + 3;
  while (yp > 0) {
    yp--;
    xp = (int)(round(sqrt(radius*radius - yp*yp)));
    w  = xp * 2;
    gydraw_dohline(draw, x - xp, y - yp, w); // top
    if (yp) { // prevent double line through the middle.
      gydraw_dohline(draw, x - xp, y + yp, w); // bottom
    }     
 }
}


/** Draws a disk, or a filled circle, on the image. */
void gyimage_disk(GyImage * image, int x, int y, int d, GyColor color) {
  GyDraw draw;
  int r = d / 2; 
  gydraw_init(&draw, image, gydraw_putpixel, color, GY_ALPHA_OPAQUE);
  gyimage_lock(image);
  gydraw_dodisk(&draw, x + r, y + r, r);
  gyimage_unlock(image);
}

/** Blends a disk, or a filled circle, on the image. */
void gyimage_blenddisk(GyImage * image, int x, int y, int d, 
                          GyColor   color) {
  GyDraw draw;
  int r = d / 2;
  gydraw_init(&draw, image, gydraw_blendpixel, color, color.a);
  gyimage_lock(image);
  gydraw_dodisk(&draw, x + r, y + r, r);
  gyimage_unlock(image);
}


/** Copies a pixel from one image to another, without locking, but mapping 
the colors as needed, and blending when needed as well. **/
void gyimage_copypixel_nolock(GyImage * dst, int dstx, int dsty,
                                 GyImage * src, int srcx, int srcy ) {
  GyDye srcpix, dstpix;
  GyColor srccol;
  srccol = gyimage_getdot_nolock(src, srcx, srcy);
  gyimage_blenddot_nolock(dst, dstx, dsty, srccol);
}

/*
GyColor gyimage_getdot_nolock(GyImage *img, int x, int y)
  
  srcpix = gyimage_getpixel_nolock(src, srcx, srcy);
  // there's a logical error here: if the pixel gotten has an alpha value
  // mapcolor might destroy the alpha if the target has no alpha channel. 
  
  dstpix = gyimage_mapcolor(dst, src, srcpix);
  gyimage_putpixel_nolock(dst, dstx, dsty, dstpix);
}  
*/

/** Blits (a part of) an image to another one, 
    scaling it in integer increments by simple repetition 
*/
void gyimage_iscaleblit(GyImage * dst, int dstx, int dsty, 
                                      int xmul, int ymul, GyImage * src,
                                      int srcx, int srcy, int srch, int srcw) {
  int xin, yin, xout, yout, stopx, stopy; 
  GyDye pixel;
  if ((xmul < 1) || (ymul < 1 )) { return; } 
  //gyimage_lock(dst);
  //gyimage_lock(src);  
  stopx = dstx + (srcw*xmul) ; stopy = dsty + (srch*ymul) ;   
    
  for (yout  = dsty ; yout < stopy; yout++)  {
    yin = srcy + ((yout - dsty) / ymul);
    for (xout  = dstx ; xout < stopx; xout++)  {
      xin = srcx + (( xout - dstx) / xmul);
      gyimage_copypixel(dst, xout, yout, src, xin, yin);
    }
  } 
  
  gyimage_unlock(src);
  gyimage_unlock(dst);
}


/** Arbitrary scaling, without interpolation. Uses linear stepping. */
void gyimage_scaleblit(GyImage * dst, int dstx, int dsty, 
                                      int dstw, int dsth, 
                                      GyImage * src,
                                      int srcx, int srcy, 
                                      int srcw, int srch) {
  int xin, yin, xout, yout, stopx, stopy, xrest, yrest, xrat, yrat, xmod, ymod; 
  GyDye pixel;
  if ((dsth < 1) || (dstw < 1 )) { return; } 
  //gyimage_lock(dst);
  //gyimage_lock(src);  
  stopx = dstx + dstw        ; stopy = dsty + dsth        ;
  xin   = srcx               ; yin   = srcy               ;
  xrat  = srcw / dstw        ; yrat  = srch / dsth        ;
  yrest = 0                  ; xrest = 0                  ;
  xmod  = srcw % dstw        ; ymod  = srch % dsth        ;  
  for (yout  = dsty ; yout < stopy; yout++)  {
    for (xout  = dstx ; xout < stopx; xout++)  {
      gyimage_copypixel(dst, xout, yout, src, xin, yin);
      xrest += xmod;
      xin   += xrat; 
      if (xrest >= dstw ) {
        xin   ++;
        xrest = 0;
      }
    }
    xin    = srcx;
    xrest  = 0;
    yrest += ymod;
    yin   += yrat;
    if (yrest >= dsth ) {
      yin    ++;
      yrest = 0;
    }
  } 
  
  gyimage_unlock(src);
  gyimage_unlock(dst);
}




void gydraw_doflood(GyDraw * draw, int x, int y, GyDye old) {
  GyDye now;
  if (GY_DRAW_OUTSIDE(draw, x, y)) return;  
  now = gyimage_getpixel_nolock(GY_DRAW_IMAGE(draw), x, y);  
  if (now != old) return;   
  draw->draw(draw, x, y);
  gydraw_doflood(draw, x+1,   y, old);
  gydraw_doflood(draw,   x, y+1, old);
  gydraw_doflood(draw, x-1,   y, old);
  gydraw_doflood(draw,   x, y-1, old);
}



/** Flood fills with color starting from x and y. */
void gyimage_flood(GyImage * image, int x, int y, GyColor color) {
  GyDraw draw;
  GyDye  dye;  
  gydraw_init(&draw, image, gydraw_putpixel, color, GY_ALPHA_OPAQUE);
  if (gyimage_pixeloutorclip(image, x, y)) return;
  gyimage_lock(image);
  dye = gyimage_getpixel_nolock(image, x, y);
  gydraw_doflood(&draw, x, y, dye);
  gyimage_unlock(image);
}


/** Flood blends with color starting from x and y. */
void gyimage_blendflood(GyImage * image, int x, int y, GyColor color) {
  GyDraw draw;
  GyDye  dye;  
  gydraw_init(&draw, image, gydraw_blendpixel, color, color.a);
  if (gyimage_pixeloutorclip(image, x, y)) return;  
  gyimage_lock(image);
  dye = gyimage_getpixel_nolock(image, x, y);
  gydraw_doflood(&draw, x, y, dye);
  gyimage_unlock(image);
}

