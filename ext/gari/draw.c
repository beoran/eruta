#include "gari.h"

#define GARI_INTERN_ONLY
#include "gari_intern.h"

#include <math.h>

/* Constructs drawing information */
GariDraw * gari_draw_init(GariDraw          * draw,
                          GariImage         * image,
                          GariDrawFunction  * func,
                          GariColor color,
                          GariAlpha alpha
                         ) { 
  if(!draw) { return NULL; } 
  draw->image = image;
  draw->draw  = func;
  draw->color = color;
  if(draw->image) {      
    draw->dye         = gari_color_dye(draw->color, draw->image);
    // We use these function pointers to speed up drawing somewhat.
    // The function pointerwill match the color depth of the image, so
    // we can avoid a case statement, at the expendse of a pointer 
    // dereference. The latter should be faster on most architectures.    
    draw->putpixel   = gari_image_putpixelfunc_nl(draw->image); 
    draw->getpixel   = gari_image_getpixelfunc_nl(draw->image);
    draw->blendpixel = gari_image_blendpixelfunc_nl(draw->image);
  } else {
    draw->dye = 0;
  } 
  draw->alpha = alpha;
  return draw;
} 


/** Gets the image of the draw info. */
GariImage * gari_draw_image(GariDraw * draw) { 
  return GARI_DRAW_IMAGE(draw);
}

/** Retuns true of the point at x, y is out of bounds for this
draw info , false if not. */
int gari_draw_outside(GariDraw * draw, int x, int y) {
  return GARI_DRAW_OUTSIDE(draw, x, y);
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
  draw->putpixel(draw->image, px, py, draw->dye);
  return TRUE;
}

int gari_draw_blendpixel(GariDraw * draw, int px, int py) {
  draw->blendpixel(draw->image, px, py, draw->dye, draw->alpha);
  return TRUE;
}

// Draws an arbitrary line
void gari_image_line(GariImage * image, int x, int y, 
                    int w, int h, GariColor color) {
  GariDraw draw;  
  gari_draw_init(&draw, image, gari_draw_putpixel, color, GARI_ALPHA_OPAQUE);
  gari_image_lock(image);
  gari_draw_doline(&draw, x, y, w, h);
  gari_image_unlock(image);
}

// Blends an arbitrary line
void gari_image_blendline(GariImage * image, int x, int y, 
                    int w, int h, GariColor color) {
  GariDraw draw;  
  gari_draw_init(&draw, image, gari_draw_blendpixel, color, GARI_ALPHA_OPAQUE);
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
                    int w, GariColor color) {
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
                    int w, GariColor color) {
  GariDraw draw;
  gari_draw_init(&draw, image, gari_draw_blendpixel, color, color.a);
  // Adjust for negative widths.
  if (w < 0) {
    w = -w;
    x =  x - w;
  } 
  gari_draw_dohline(&draw, x, y, w);
}

/* Optimized case of drawline, draws vertical lines only. */
void gari_image_vline_nolock(GariImage * image, int x, int y, 
                    int h, GariColor color) {
  GariDraw draw;
  gari_draw_init(&draw, image, gari_draw_putpixel, color, GARI_ALPHA_OPAQUE);
  // Adjust for negative heights.  
  if (h < 0) {
    h = -h;
    y =  y - h;
  } 
  gari_draw_dovline(&draw, x, y, h);
}

/* Optimized case of blendline, blends vertical lines only. */
void gari_image_blendvline_nolock(GariImage * image, int x, int y, 
                    int h, GariColor color) {
  GariDraw draw;
  gari_draw_init(&draw, image, gari_draw_blendpixel, color, GARI_ALPHA_OPAQUE);
  // Adjust for negative heights.  
  if (h < 0) {
    h = -h;
    y =  y - h;
  } 
  gari_draw_dovline(&draw, x, y, h);
}


/* Optimized case of gari_image_line, draws horizontal 
   lines with a width of w only. */
void gari_image_hline(GariImage * image, int x, int y, int w, GariColor color) 
{
  gari_image_lock(image);
  gari_image_hline_nolock(image, x, y, w, color);
  gari_image_unlock(image);
}

/* Optimized case of gari_image_line, draws vertical 
   lines with a height h only. */
void gari_image_vline(GariImage * image, int x, int y, int h, GariColor color) 
{
  gari_image_lock(image);
  gari_image_vline_nolock(image, x, y, h, color);
  gari_image_unlock(image);
}


void gari_draw_doslab(GariDraw * draw, int x, int y, int w, int h) {
  int stopx, stopy, xi, yi;
  stopy = y + h;
  // printf("doslab: y, w, stopx, stopy: %d %d %d \n", y, h, stopy);
  for( yi = y ; yi < stopy ; yi++) {
    gari_draw_dohline(draw, x, yi, w);
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
void gari_image_blendslab(GariImage * image, int x, int y, int w, int h,  
                      GariColor color) {
  GariDraw draw;
  gari_draw_init(&draw, image, gari_draw_blendpixel, color, color.a);
  // Adjust for negative widths and heights.
  ADJUST_NEGATIVE(w, x);
  ADJUST_NEGATIVE(h, y);
  gari_image_lock(image); 
  gari_draw_doslab(&draw, x, y, w, h);
  gari_image_unlock(image);
} 


/** Draws a slab, which is a filled rectange, on the image. */
void gari_image_slab( GariImage * image, int x, int y, int w, int h, 
                      GariColor color) {  
  GariDraw draw;  
  gari_draw_init(&draw, image, gari_draw_putpixel, color, GARI_ALPHA_OPAQUE);
   
  // Adjust for negative widths and heights.
  ADJUST_NEGATIVE(w, x);
  ADJUST_NEGATIVE(h, y);
  gari_image_lock(image); 
  gari_draw_doslab(&draw, x, y, w, h);
  gari_image_unlock(image);
} 


/** Draws a box, which is an open rectange, on the image. */
void gari_image_box( GariImage * image, int x, int y, int w, int h,  
                      GariColor color) {
  gari_image_lock(image);
  // Two horizontal lines...
  gari_image_hline_nolock(image, x, y    , w, color);
  gari_image_hline_nolock(image, x, y + h, w, color);
  // And two vertical ones.
  gari_image_vline_nolock(image, x    , y, h, color);
  gari_image_vline_nolock(image, x + w, y, h, color); 
  gari_image_unlock(image);
} 

/** Blends a box, which is an open rectange, on the image. */
void gari_image_blendbox( GariImage * image, int x, int y, int w, int h,  
                      GariColor color) {
  gari_image_lock(image);
  // Two horizontal lines...
  gari_image_blendhline_nolock(image, x, y    , w, color);
  gari_image_blendhline_nolock(image, x, y + h, w, color);
  // And two vertical ones.
  gari_image_blendvline_nolock(image, x    , y, h, color);
  gari_image_blendvline_nolock(image, x + w, y, h, color); 
  gari_image_unlock(image);
} 


/* Traces a circle outline with the midpoint of the circle at x, y, and the radius being radius. Inspired by an algorithm by Kevin Matz. */
void gari_draw_dohoop(GariDraw * draw, int x, int y, int radius) {  
    
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
void gari_image_hoop(GariImage * image, int x, int y, int d, GariColor color) {
  GariDraw draw;
  int r = d / 2;
  gari_draw_init(&draw, image, gari_draw_putpixel, color, GARI_ALPHA_OPAQUE);
  gari_image_lock(image);
  gari_draw_dohoop(&draw, x + r, y + r, r);
  gari_image_unlock(image);
}

/** Blends a hoop, that is, an empty circle, on the image. */
void gari_image_blendhoop(GariImage * image, int x, int y, int d, GariColor color) {
  GariDraw draw;
  int r = d / 2;
  gari_draw_init(&draw, image, gari_draw_blendpixel, color, color.a);
  gari_image_lock(image);
  gari_draw_dohoop(&draw, x + r, y + r, r);
  gari_image_unlock(image);
}


/* Traces a circle surface with the midpoint of the circle at x, y, and the radius being radius.  Calls gari_draw_dohline. Does not handle blends well, 
and small circles are deformed. But it is faster. 
*/
void gari_draw_dodisk_fast(GariDraw * draw, int x, int y, int radius) {  
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
    
    gari_draw_dohline(draw, x - xp, y - yp, w); // top
    gari_draw_dohline(draw, x - xp, y + yp, w); // bottom
    
    w = (yp * 2);
    gari_draw_dohline(draw, x - yp, y - xp, w); // above middle
    gari_draw_dohline(draw, x - yp, y + xp, w); // lower middle
    
    
 }
}

/* Traces a circle surface with the midpoint of the circle at x, y, and the radius being radius.  Calls gari_draw_dohline and also works fine for blends,
and does not deform small circle, but is slower due to float math. 
*/

void gari_draw_dodisk(GariDraw * draw, int x, int y, int radius) {  
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
    gari_draw_dohline(draw, x - xp, y - yp, w); // top
    if (yp) { // prevent double line through the middle.
      gari_draw_dohline(draw, x - xp, y + yp, w); // bottom
    }     
 }
}


/** Draws a disk, or a filled circle, on the image. */
void gari_image_disk(GariImage * image, int x, int y, int d, GariColor color) {
  GariDraw draw;
  int r = d / 2; 
  gari_draw_init(&draw, image, gari_draw_putpixel, color, GARI_ALPHA_OPAQUE);
  gari_image_lock(image);
  gari_draw_dodisk(&draw, x + r, y + r, r);
  gari_image_unlock(image);
}

/** Blends a disk, or a filled circle, on the image. */
void gari_image_blenddisk(GariImage * image, int x, int y, int d, 
                          GariColor   color) {
  GariDraw draw;
  int r = d / 2;
  gari_draw_init(&draw, image, gari_draw_blendpixel, color, color.a);
  gari_image_lock(image);
  gari_draw_dodisk(&draw, x + r, y + r, r);
  gari_image_unlock(image);
}


/** Copies a pixel from one image to another, without locking, but mapping 
the colors as needed, and blending when needed as well. **/
void gari_image_copypixel_nolock(GariImage * dst, int dstx, int dsty,
                                 GariImage * src, int srcx, int srcy ) {
  GariDye srcpix, dstpix;
  GariColor srccol;
  srccol = gari_image_getdot_nolock(src, srcx, srcy);
  gari_image_blenddot_nolock(dst, dstx, dsty, srccol);
}

/*
GariColor gari_image_getdot_nolock(GariImage *img, int x, int y)
  
  srcpix = gari_image_getpixel_nolock(src, srcx, srcy);
  // there's a logical error here: if the pixel gotten has an alpha value
  // mapcolor might destroy the alpha if the target has no alpha channel. 
  
  dstpix = gari_image_mapcolor(dst, src, srcpix);
  gari_image_putpixel_nolock(dst, dstx, dsty, dstpix);
}  
*/

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
                                      int dstw, int dsth, 
                                      GariImage * src,
                                      int srcx, int srcy, 
                                      int srcw, int srch) {
  int xin, yin, xout, yout, stopx, stopy, xrest, yrest, xrat, yrat, xmod, ymod; 
  GariDye pixel;
  if ((dsth < 1) || (dstw < 1 )) { return; } 
  gari_image_lock(dst);
  gari_image_lock(src);  
  stopx = dstx + dstw        ; stopy = dsty + dsth        ;
  xin   = srcx               ; yin   = srcy               ;
  xrat  = srcw / dstw        ; yrat  = srch / dsth        ;
  yrest = 0                  ; xrest = 0                  ;
  xmod  = srcw % dstw        ; ymod  = srch % dsth        ;  
  for (yout  = dsty ; yout < stopy; yout++)  {
    for (xout  = dstx ; xout < stopx; xout++)  {
      gari_image_copypixel_nolock(dst, xout, yout, src, xin, yin);
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
  
  gari_image_unlock(src);
  gari_image_unlock(dst);
}




void gari_draw_doflood(GariDraw * draw, int x, int y, GariDye old) {
  GariDye now;
  if (GARI_DRAW_OUTSIDE(draw, x, y)) return;  
  now = gari_image_getpixel_nolock(GARI_DRAW_IMAGE(draw), x, y);  
  if (now != old) return;   
  draw->draw(draw, x, y);
  gari_draw_doflood(draw, x+1,   y, old);
  gari_draw_doflood(draw,   x, y+1, old);
  gari_draw_doflood(draw, x-1,   y, old);
  gari_draw_doflood(draw,   x, y-1, old);
}



/** Flood fills with color starting from x and y. */
void gari_image_flood(GariImage * image, int x, int y, GariColor color) {
  GariDraw draw;
  GariDye  dye;  
  gari_draw_init(&draw, image, gari_draw_putpixel, color, GARI_ALPHA_OPAQUE);
  if (gari_image_pixeloutorclip(image, x, y)) return;
  gari_image_lock(image);
  dye = gari_image_getpixel_nolock(image, x, y);
  gari_draw_doflood(&draw, x, y, dye);
  gari_image_unlock(image);
}


/** Flood blends with color starting from x and y. */
void gari_image_blendflood(GariImage * image, int x, int y, GariColor color) {
  GariDraw draw;
  GariDye  dye;  
  gari_draw_init(&draw, image, gari_draw_blendpixel, color, color.a);
  if (gari_image_pixeloutorclip(image, x, y)) return;  
  gari_image_lock(image);
  dye = gari_image_getpixel_nolock(image, x, y);
  gari_draw_doflood(&draw, x, y, dye);
  gari_image_unlock(image);
}

