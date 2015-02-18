
#include "draw.h"
#include "laytext.h"
#include "dynar.h"

/** Addinional drawing functions and wrappers for primitive drwaing 
functionality. */


/** Draws an image on the current target bitmap according to a 9 part scaling
algorithm. This splits the bitmap in 9 parts, keeps the 4 corners unscaled, but
scales the 4 edges and the center according to the desired size.
The 4 corners should be rectangles of identical size corner_w by corner_h in
the original image. Pass a non-positive number to have the corner sizes 
automatically calculated.
new_w and new_h are the new dimensions the new image should have.
This is useful for GUI backgrounds.
*/
void image_blitscale9(Image * img, int xx, int yy,
                                  int new_w, int new_h,
                                  int corner_w, int corner_h) {

  int mid_src_w, mid_src_h, mid_dst_w, mid_dst_h;
  int left_x, right_x, middle_x;
  int dst_y, src_y, part_w;
  int src_w = image_w(img);
  int src_h = image_h(img);
  corner_w  = (corner_w > 0) ? (corner_w) :  src_w / 16;
  corner_h  = (corner_h > 0) ? (corner_h) :  src_h / 16;
  CLAMP_MAX(corner_w, corner_w, (new_w / 4));
  CLAMP_MAX(corner_h, corner_h, (new_h / 4));
  mid_src_w = src_w - (corner_w * 2);
  mid_src_h = src_h - (corner_h * 2);
  mid_dst_w = new_w - (corner_w * 2);
  mid_dst_h = new_h - (corner_h * 2);
  // draw top parts first  
  left_x    = xx; // left side
  right_x   = xx    + mid_dst_w + corner_w; // right side
  middle_x  = xx    + corner_w ; // middle
  dst_y     = yy; // y is he drawing location for the top 3 draws
  // width of the first corner and middle. the second corner starts here
  part_w    = src_w - corner_w;
  // draw, take from the top corner left of the image
  image_drawpart(img, 0, 0, corner_w, corner_h, left_x, dst_y, 0);
  // draw, take from the midle of the image
  image_drawscale(img, corner_w, 0, mid_src_w, corner_h,
                  middle_x, dst_y, mid_dst_w, corner_h, 0);
  // draw, take from the right corner of the image
  image_drawpart(img, part_w, 0, corner_w, corner_h, right_x, dst_y,  0);
  // now shift down, mutatis mutandis.
  dst_y     = yy + corner_h;
  src_y     = corner_h;
  // NOTE the middle left and right must be scaled, unlike the corners
  // that must be copied as they are.
  // draw, take from the middle corner left of the image, stretch vertically 
  image_drawscale(img, 0, src_y, corner_w , mid_src_h,
                  left_x, dst_y, corner_w , mid_dst_h, 0);
  // draw, take from the midle center of the image
  image_drawscale(img, corner_w, src_y, mid_src_w, mid_src_h,
                  middle_x, dst_y, mid_dst_w, mid_dst_h, 0);
  // draw, take from the right center of the image, stretch vertically
  image_drawscale(img, part_w,  src_y, corner_w, mid_src_h,
                  right_x, dst_y, corner_w , mid_dst_h, 0);
  
  dst_y     = yy + new_h - corner_h ; // + mid_dst_h;
  src_y     = src_w - corner_h;
  // draw, take from the lower corner left of the image
  image_drawpart(img, 0, src_y, corner_h, corner_w, left_x, dst_y, 0);
  // draw, take from the lower center of the image
  image_drawscale(img, corner_w,  src_y, mid_src_w, corner_h,
                  middle_x, dst_y, mid_dst_w, corner_h, 0);
  
  // draw, take from the lower corner of the image
  image_drawpart(img, part_w, src_y, corner_h, corner_w, right_x, dst_y,  0);
}


/*** Draws a filled rectange at the given position with the given size */
void draw_slab(int x, int y, int w, int h, Color col) {
  al_draw_filled_rectangle(x, y, x+w, y+h, col);
} 

/*** Draws a rounded filled rectange at the given position with the given size */
void draw_roundslab(int x, int y, int w, int h, int rx, int ry, Color col) {
  al_draw_filled_rounded_rectangle(x, y, x+w, y+h, rx, ry, col);
} 


/*** Draws an open rectange at the given position with the given size */
void draw_box(int x, int y, int w, int h, Color col, int tt) {
  al_draw_rectangle(x, y, x+w, y+h, col, tt);
} 


/** Draws a rounded rectangle at the given position with the given size */
void draw_roundbox(int x, int y, int w, int h, int rx, int ry, Color col, int tt) {
  al_draw_rounded_rectangle(x, y, x+w, y+h, rx, ry, col, tt);
} 
  

/** Draws a filled frame of the given thickness on the active bitmap.
* The outer size of the frame will be ww and hh.
* border color is fg, background color is bg. */
void draw_frame(int xx, int yy, int ww, int hh, int tt, Color fg, Color bg)  {
  // Draw inner frame in bg color.
  draw_slab(xx, yy, ww, hh, bg);
  // Draw outer frame in fg color with the given thickness.
  draw_box(xx, yy, ww, hh, fg, tt);
}  
  
/** Draws a filled, rounded frame of the given thickness on the active bitmap.
* The rounding is autocalulated. The outer size of the frame will be ww and hh.
* border color is fg, background color is bg.
*/
void draw_roundframe(int xx, int yy, int ww, int hh, int tt, Color fg, Color bg)  {
  int rx = 4;
  int ry = 4;
  // draw inner frame in bg color.
  draw_roundslab(xx, yy, ww, hh, rx, ry, bg);
  // draw outer frame in fg color with the given thickness.
  draw_roundbox(xx, yy, ww, hh, rx, ry, fg, tt);
}



/* Function that maps black to transparent black, white to transparent white, and 
 * any grayscale or color in between to white with an alpha value
 * that corresponds to the average of the r, g and b components. Mosty for use 
 * with the "GIN" icons. The color in draw color will be used for the r, g and b
 * of the color (premultiplied by the average as well)
 */
bool draw_convert_average_to_alpha(ALLEGRO_BITMAP *bitmap, ALLEGRO_COLOR draw_color)
{
   ALLEGRO_LOCKED_REGION *lr;
   int x, y;
   float dr, dg, db;
   float avg, r, g, b;
   ALLEGRO_COLOR pixel;
   ALLEGRO_COLOR alpha_pixel;
   ALLEGRO_STATE state;

   if (!(lr = al_lock_bitmap(bitmap, ALLEGRO_PIXEL_FORMAT_ANY, 0))) {
      return FALSE;
   }

   al_store_state(&state, ALLEGRO_STATE_TARGET_BITMAP);
   al_set_target_bitmap(bitmap);

   alpha_pixel = al_map_rgba(0, 0, 0, 0);
   al_unmap_rgb_f(draw_color, &dr, &dg, &db);

   for (y = 0; y < al_get_bitmap_height(bitmap); y++) {
      for (x = 0; x < al_get_bitmap_width(bitmap); x++) {
         pixel = al_get_pixel(bitmap, x, y);
         al_unmap_rgb_f(pixel, &r, &g, &b);
         avg = (r + g + b) / 3.0;  
         alpha_pixel = al_map_rgba_f(dr * avg, dg * avg, db * avg, avg);
         al_put_pixel(x, y, alpha_pixel);
      }
   }

   al_unlock_bitmap(bitmap);

   al_restore_state(&state);
   return TRUE;
}

/* Helper for drawing multi line text */
static int laytext_allegro_font_callback
(char * str, int bytes, void * extra, float * w, float * h) {
  ALLEGRO_USTR_INFO info; 
  const ALLEGRO_USTR * ustr;
  ustr = al_ref_buffer(&info, str, bytes);
  (*w) = (float) al_get_ustr_width((ALLEGRO_FONT *)extra, ustr); 
  (*h) = 1.0;
  return bytes;
}

void draw_multi_line_text(ALLEGRO_FONT * font, ALLEGRO_COLOR color, 
                          float x, float y, float w, float h, 
                          int flags,
                          char * text) 
{
  int index;
  long start;
  Dynar * result;
  long value = 0;
  int length;
  int line_height = al_get_font_line_height(font);
  ALLEGRO_USTR_INFO info; 
  const ALLEGRO_USTR * ustr;

  result = laytext_layout(text, w, laytext_allegro_font_callback, font);
  
  length = strlen(text);
  start = 0;
  for(index = 0; index < dynar_size(result); index++) {    
    dynar_get_long(result, index, &value);
    if (value > 0) { 
      ustr = al_ref_buffer(&info, text + start, (int) (value - start));
      al_draw_ustr(font, color, x, y, flags, ustr);
      y += line_height;
    } 
    start = value + 1;
  }
  ustr = al_ref_cstr(&info, text + start);
  al_draw_ustr(font, color, x, y, flags, ustr);  
  dynar_free(result);
  (void) h;
}


/** Makes a new image by copying a part from the source image.
 * Allegro drawing flags can be passed to flip the copied image in one go.
 * The result must be freed by calling al_destroy_bitmap.
 */
Image * image_copy_region
  (Image * source, int x, int y, int wide, int high, int flags) { 
  Image * result;
  ALLEGRO_COLOR black, glass, white;
  black = al_map_rgba(0,0,0,255);
  white = al_map_rgba(255,255,255,255);
  glass = al_map_rgba(0,0,0,0);
  // al_set_new_bitmap_flags
  result = al_create_bitmap(wide, high);
  if(!result) {    
    return NULL;
  }  
  al_set_target_bitmap(result);
  al_clear_to_color(glass);  
  al_draw_bitmap_region(source, x, y, wide, high, 0, 0, flags);
  if (al_get_current_display()) { 
    al_set_target_backbuffer(al_get_current_display());
  }
  return result;
}


