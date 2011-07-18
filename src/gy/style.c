
#include "gy.h"
#define GY_INTERN_ONLY
#include "gy_intern.h"


struct GyStyle_     {
  GyColor   fore;   // foreground color
  GyColor   back;   // font to use
  GyFont  * font;   // background color 
  GyImage * image;  // image
};

GyStyle * gystyle_init(GyStyle * style, 
                                GyColor   fore, 
                                GyColor   back,
                                GyFont  * font,
                                GyImage * image
                                ){
  if (!style) return NULL;
  style->fore   = fore;
  style->back   = back;
  style->font   = font;
  style->image  = image;
  return style;
}

GyStyle * gystyle_free(GyStyle * style) {
  GY_FREE(style);
  return NULL;
}

GyStyle * gystyle_new(GyColor   fore, 
                           GyColor   back,
                           GyFont  * font,
                           GyImage * image
                          ){
  GyStyle * style = GY_ALLOCATE(GyStyle);
  return gystyle_init(style, fore, back, font, image);
}

GyFont * gystyle_font(GyStyle * style) {
  if(!style) return NULL;
  return style->font;
}

GyColor gystyle_fore(GyStyle * style) {
  if(!style) return gycolora(255,0,0,255);
  return style->fore;
}

GyColor gystyle_back(GyStyle * style) {
  if(!style) return gycolora(255,0,0,255);
  return style->back;
}

GyImage * gystyle_image(GyStyle * style) {
  if(!style) return NULL;
  return style->image;
}



