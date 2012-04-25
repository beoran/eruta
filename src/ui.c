#include "eruta.h"
#include "image.h"
#include "ui.h"
#include "widget.h"

/*
* UI handles the user interface of Eruta.
* 
* There is only one top-level widget, however, every widget can contain
* any amout of child widgets. Widgets that are children of the same parent
* widget are said to be on the same level.
*
*
* The box model used in eruta is as follows:
*  ......................................
*  .margin                              .
*  .  +-------------border-----------+  .
*  .  | padding                      |  .
*  .  |  ..........................  |  .
*  .  |  .                        .  |  .
*  .  |  . contents               .  |  .
*  .  |  .                        .  |  .
*  .  |  ..........................  |  .
*  .  | padding                      |  .
*  .  +------------------------------+  .
*  .margin                              .
*  ......................................
*
* The padding determines the minimal distance between the border or the
* parent widget and it's contents and/or child widgets.
*
* The border's thickness is only relevant for visual effects. It does not change
* the layout. The border is effectively "inside" the padding of the widget.
*
* The margin of a widget determines how closely that widget may be packed
* to it's sibling widgets.
*
* The work in UI is divided between the UI and the Widget. The UI struct
* handles everything that depends on and/or may influence several widgets at
* once, such as event dispatching but also setting the focus, determining which
* widget is being hovered, or dragged, etc. The latter fuctions change the state
* of several widgets, so they are handled on the level of the system.
* The Widget class and it's child classes handle the individual state and
* actions of the various widgets individually.
*
*/



/** UI manages the graphical user interface and menus. */
struct UI_ {
   Widget * widget; /* The top level widget, and all it's children. */
  
};


/** Draws an image on the current target bitmap according to a 9 part scaling
algorithm. This splits the bitmap in 9 parts, keeps the 4 corners unscaled, but
scales the 4 edges and the center according to the desired size.
The 4 corners should be rectangles of identical size corner_w by corner_h in
the original image. Pass a nonpositive number to have the corner sizes automatically calculated.
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

/** Draws a filled frame of the given thickness on the active bitmap.
* The outer size of the frame will be ww and hh.
* border color is fg, background color is bg. */
void draw_frame(int xx, int yy, int ww, int hh, int tt, Color fg, Color bg)  {
  // draw inner frame in bg color.
  al_draw_filled_rectangle(xx, yy, xx+ww, yy+hh, bg);
  // draw outer frame in fg color with the given thickness.
  al_draw_rectangle(xx, yy, xx+ww, yy+hh, fg, tt);
}  
  
/** Draws a filled, rounded frame of the given thickness on the active bitmap.
* The rounding is autocalulated. The outer size of the frame will be ww and hh.
* border color is fg, background color is bg.
*/
void draw_roundframe(int xx, int yy, int ww, int hh, int tt, Color fg, Color bg)  {
  int rx = 4;
  int ry = 4;
  // draw inner frame in bg color.
  al_draw_filled_rounded_rectangle(xx, yy, xx+ww, yy+hh, rx, ry, bg);
  // draw outer frame in fg color with the given thickness.
  al_draw_rounded_rectangle(xx, yy, xx+ww, yy+hh, rx, ry, fg, tt);
}

