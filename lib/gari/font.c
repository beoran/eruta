#include "gari.h"

#define GARI_INTERN_ONLY
#include "gari_intern.h"

struct GariFont_ {
  TTF_Font   * ttf;
  int          ptsize;
  int          mode;
  long         index;  
  char       * filename;
  GariImage ** glyphs;
};



/** Sets the drawing mode of the font. */
GariFont * gari_font_mode(GariFont * font, int mode) {
  if(!font) { return NULL ; }
  font->mode = mode;
  return font;
}


GariFont * gari_font_init(GariFont * font, TTF_Font * ttffont, 
           char *name, int ptsize, long index) {
  if(!font) { return NULL ; }
  font->ptsize  = ptsize;
  font->index   = index;
  font->filename= name; 
  font->ttf     = ttffont; 
  if(!font->ttf) { return NULL ; }
  return gari_font_mode(font, GariFontBlended);
}


#define GARI_FONT_TTF(FONT) ((FONT)->ttf)

GariFont * gari_font_loadindex(char * filename, int ptsize, long index) {
  GariFont * font;
  TTF_Font * ttf = TTF_OpenFontIndex(filename, ptsize, index);
  if (!ttf) return NULL;
  font         = GARI_ALLOCATE(GariFont);
  // If malloc failed, free ttf and get out of here.
  if (!font) { TTF_CloseFont(ttf); return NULL; }
  return gari_font_init(font, ttf, filename, ptsize, index); 
}

GariFont * gari_font_load(char * filename, int ptsize) {
  return gari_font_loadindex(filename, ptsize, 0);
}

void gari_font_free(GariFont * font) {
  if(!font) { return; } 
  if(!font->ttf) { return; }
  TTF_CloseFont(font->ttf);
  font->ttf = NULL;
  GARI_FREE(font);
}

// transforms a gari RGBA into an SDL_Color 
SDL_Color gari_rgba_sdl(GariColor rgba) {
  SDL_Color result;
  result.b = rgba.b;
  result.g = rgba.g;
  result.r = rgba.r;
  return result;
}

/** Renders the font to a surface, depending on the font's settings. */
GariImage * gari_font_render(GariFont * font, char * utf8, 
    GariColor fgrgba, GariColor bgrgba) {
  TTF_Font * ttf = GARI_FONT_TTF(font);
  SDL_Color fg = gari_rgba_sdl(fgrgba);
  SDL_Color bg = gari_rgba_sdl(fgrgba);
  switch(font->mode) { 
    case GariFontSolid:
      return gari_image_wrap(TTF_RenderUTF8_Solid(ttf, utf8, fg));
    case GariFontShaded:
      return gari_image_wrap(TTF_RenderUTF8_Shaded(ttf, utf8, fg, bg));
    case GariFontBlended:
      return gari_image_wrap(TTF_RenderUTF8_Blended(ttf, utf8, fg));    
  }  
  return NULL;

} 

void gari_font_drawrgba(GariImage * image, int x, int y, char * utf8, GariFont * font, GariColor fg, GariColor bg) {  
  GariImage * rendered;
  rendered = gari_font_render(font, utf8, fg, bg);
  gari_image_blit(image, x, y, rendered);
  gari_image_free(rendered);  
}  

void gari_font_draw(GariImage * image, int x, int y, char * utf8, GariFont * font, uint8_t fg_r, uint8_t fg_g, uint8_t fg_b, uint8_t bg_r, uint8_t bg_g, uint8_t bg_b) {
  GariColor fg = { fg_r, fg_g, fg_b, 0};
  GariColor bg = { bg_r, bg_g, bg_b, 0};
  gari_font_drawrgba(image, x, y, utf8, font, fg, bg);  
}














