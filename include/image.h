/** Simple macro wrappers around al_bitmap_xxx */
#ifndef ERUTA_IMAGE_H
#define ERUTA_IMAGE_H

#include <allegro5/allegro.h>

/** An image type. Useful as a shorthand to Allegro's long names. */
typedef ALLEGRO_BITMAP Image;

/** A color type. Useful as a shorthand to Allegro's long names. */
typedef ALLEGRO_COLOR Color;


/** Some utility macros. */
#define image_w(IMAGE) al_get_bitmap_width(IMAGE)
#define image_h(IMAGE) al_get_bitmap_height(IMAGE)
#define image_load(FILENAME) al_load_bitmap(FILENAME)
#define image_drawscale(BMP, SX, SY, SW, SH, DX, DY, DW, DH, FLAGS) \
        al_draw_scaled_bitmap(BMP, SX, SY, SW, SH, DX, DY, DW, DH, FLAGS)
#define image_drawpart(BMP, SX, SY, SW, SH, DX, DY, FLAGS)          \
        al_draw_bitmap_region(BMP, SX, SY, SW, SH, DX, DY, FLAGS)

        
#define color_rgb(R, G, B)  al_map_rgb(R, G, B)
#define COLOR_WHITE  al_map_rgb(255, 255, 255)
#define COLOR_BLACK  al_map_rgb(0  ,   0,   0)
#define COLOR_BLUE   al_map_rgb(0  ,   0,  64)


#endif

