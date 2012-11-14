/** Simple macro wrappers around al_bitmap_xxx */
#ifndef ERUTA_IMAGE_H
#define ERUTA_IMAGE_H

#include <allegro5/allegro.h>
#include <allegro5/allegro_font.h>

/** An image type. Useful as a shorthand to Allegro's long names. */
typedef ALLEGRO_BITMAP Image;

/** A color type. Useful as a shorthand to Allegro's long names. */
typedef ALLEGRO_COLOR Color;

/** A font type. Useful as a shorthand to Allegro's long names. */
typedef ALLEGRO_FONT Font;



/** Some utility macros, that wrap around allegro functions.
* Useful as shorthand for Allegro's long names.
*/
#define image_w(IMAGE)        al_get_bitmap_width(IMAGE)
#define image_h(IMAGE)        al_get_bitmap_height(IMAGE)
#define image_load(FILENAME)  al_load_bitmap(FILENAME)
#define image_drawscale(BMP, SX, SY, SW, SH, DX, DY, DW, DH, FLAGS) \
        al_draw_scaled_bitmap(BMP, SX, SY, SW, SH, DX, DY, DW, DH, FLAGS)
#define image_drawpart(BMP, SX, SY, SW, SH, DX, DY, FLAGS)          \
        al_draw_bitmap_region(BMP, SX, SY, SW, SH, DX, DY, FLAGS)
#define image_free(BMP)       al_destroy_bitmap(BMP)

#define font_free(FONT)           al_destroy_font(FONT)
#define font_lineheight(FONT)     al_get_font_line_height(FONT)
#define font_ascent(FONT)         al_get_font_ascent(FONT)
#define font_descent(FONT)        al_get_font_descent(FONT)
#define font_textwidth(FONT,CSTR) al_get_text_width((FONT), (CSTR))
#define font_strwidth(FONT,STR)   al_get_ustr_width((FONT), (CSTR))

#define FONT_LEFT    ALLEGRO_ALIGN_LEFT
#define FONT_CENTRE  ALLEGRO_ALIGN_CENTRE
#define FONT_RIGHT   ALLEGRO_ALIGN_RIGHT

#define font_drawtext(FONT,  COL, X, Y, FLAGS, CSTR) \
        al_draw_text(FONT, COL, X, Y, FLAGS, CSTR)
#define font_drawstr(FONT,  COL, X, Y, FLAGS, CSTR)  \
        al_draw_ustr(FONT, COL, X, Y, FLAGS, CSTR)

#define font_drawjtext(FONT,  COL, X1, X2, Y, DIFF, FLAGS, CSTR)  \
        al_draw_justified_text(FONT, COL, X1, X2, Y, DIFF, FLAGS, CSTR)
#define font_drawjstr(FONT,  COL, X1, X2, Y, DIFF, FLAGS, CSTR)  \
        al_draw_justified_ustr(FONT, COL, X1, X2, Y, DIFF, FLAGS, CSTR)
        
        

/** Color shorthands. */
#define color_rgb(R, G, B)  al_map_rgb(R, G, B)
#define color_rgba(R, G, B, A)  al_map_rgba(R, G, B, A)
#define color_rgbf(R, G, B)  al_map_rgb_f(R, G, B)
#define color_rgbaf(R, G, B, A)  al_map_rgba_f(R, G, B, A)

#define COLOR_WHITE  al_map_rgb(255, 255, 255)
#define COLOR_BLACK  al_map_rgb(0  ,   0,   0)
#define COLOR_BLUE   al_map_rgb(0  ,   0,  64)




#endif

