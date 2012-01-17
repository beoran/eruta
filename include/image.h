/** Simple macro wrappers around al_bitmap_xxx */
#ifndef ERUTA_IMAGE_H
#define ERUTA_IMAGE_H

#include <allegro5/allegro.h>

/** An image type. */
typedef ALLEGRO_BITMAP Image;

#define image_w(IMAGE) al_get_bitmap_width(IMAGE)
#define image_h(IMAGE) al_get_bitmap_height(IMAGE)





#endif

