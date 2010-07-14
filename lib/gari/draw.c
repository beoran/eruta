#include "gari.h"

#define GARI_INTERN_ONLY
#include "gari_intern.h"



/* Constructs drawing information */
GariDraw * gari_draw_init(
                              GariDraw          * draw,
                              GariImage         * image,
                              GariDrawFunction  * func,
                              GariColor color,
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
      draw->draw(draw, x, y);  
      y += dy;
      if (y >= dx) {
        y -= dx;
        py += sdy;
      }
      px += sdx;
    }
  } else {
    for (y = 0 ; y < dy; y++) {
      draw->draw(draw, x, y);
      x += dx;
      if (x >= dy) {
        x -= dy;
        px += sdx;
      }
      py += sdy;
    }
  }
}

void gari_draw_putpixel(GariDraw * draw, int px, int py) {
  gari_image_putpixel_nolock(draw->image, px, py, draw->color);  
}


void gari_image_line(GariImage * image, int x, int y, 
                    int w, int h, GariColor color) {
  GariDraw draw;
  gari_draw_init(&draw, image, gari_draw_putpixel, color, GARI_ALPHA_OPAQUE);
  gari_image_lock(image);
  gari_draw_doline(&draw, x, y, w, h);
  gari_image_unlock(image);
}



