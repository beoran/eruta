
#include "gari.h"
#define GARI_INTERN_ONLY
#include "gari_intern.h"


struct GariStyle_     {
  GariColor   fore;   // foreground color
  GariColor   back;   // font to use
  GariFont  * font;   // background color 
  GariImage * image;  // image
};

GariStyle * gari_style_init(GariStyle * style, 
                                GariColor   fore, 
                                GariColor   back,
                                GariFont  * font,
                                GariImage * image
                                ){
  if (!style) return NULL;
  style->fore   = fore;
  style->back   = back;
  style->font   = font;
  style->image  = image;
  return style;
}

GariStyle * gari_style_free(GariStyle * style) {
  GARI_FREE(style);
  return NULL;
}

GariStyle * gari_style_new(GariColor   fore, 
                           GariColor   back,
                           GariFont  * font,
                           GariImage * image
                          ){
  GariStyle * style = GARI_ALLOCATE(GariStyle);
  return gari_style_init(style, fore, back, font, image);
}

GariFont * gari_style_font(GariStyle * style) {
  if(!style) return NULL;
  return style->font;
}

GariColor gari_style_fore(GariStyle * style) {
  if(!style) return gari_colora(255,0,0,255);
  return style->fore;
}

GariColor gari_style_back(GariStyle * style) {
  if(!style) return gari_colora(255,0,0,255);
  return style->back;
}

GariImage * gari_style_image(GariStyle * style) {
  if(!style) return NULL;
  return style->image;
}



