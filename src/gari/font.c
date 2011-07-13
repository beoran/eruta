#include "gari.h"

#define GARI_INTERN_ONLY
#include "gari_intern.h"
#include <stdarg.h>

struct GariFont_ {
  TTF_Font   * ttf;
  int          ptsize;
  int          mode;
  long         index;
  char       * filename;
  GariImage ** glyphs;
};


int gari_font_ttf_start() {
  if(!TTF_WasInit() && TTF_Init()==-1) {
    return FALSE;
  }
  return TRUE;
} 


/** Sets the drawing mode of the font. */
GariFont * gari_font_mode_(GariFont * font, int mode) {
  if(!font) { return NULL ; }
  font->mode = mode;
  return font;
}


/** Gets the drawing mode of the font. */
int gari_font_mode(GariFont * font) {
  if(!font) { return 0 ; }
  return font->mode;
}

GariFont * gari_font_init(GariFont * font, TTF_Font * ttffont, 
           char *name, int ptsize, long index) {  
  if(!font) { return NULL ; }
  font->ptsize  = ptsize;
  font->index   = index;
  font->filename= name; 
  font->ttf     = ttffont; 
  if(!font->ttf) { return NULL ; }
  return gari_font_mode_(font, GariFontBlended);
}

char * gari_font_error() {
  return TTF_GetError();
}

#define GARI_FONT_TTF(FONT) ((FONT)->ttf)

/** Loads font from data in memory */
GariFont * gari_font_dataindex(const unsigned char * data, int datasize, 
                               int ptsize, long index) {
  GariFont * font;
  TTF_Font * ttf;  
  SDL_RWops * rw;

  gari_font_ttf_start(); // start ttf lib in case it wasn't started.
  rw  = SDL_RWFromConstMem((const void *)data, datasize);
  if (!rw)  { return NULL; }                               
  ttf = TTF_OpenFontIndexRW(rw, 1, ptsize, index);
  // SDL_RWclose(rw);
  if (!ttf) { return NULL; }  
  font         = GARI_ALLOCATE(GariFont);
  // If malloc failed, free ttf and get out of here.
  if (!font) {  TTF_CloseFont(ttf); return NULL; }
  return gari_font_init(font, ttf, "<data>", ptsize, index); 
}

/** Loads font from data in memory */
GariFont * gari_font_data(const unsigned char * data, int datasize, int ptsize){
  return gari_font_dataindex(data, datasize, ptsize, 0);  
} 



GariFont * gari_font_loadindex(char * filename, int ptsize, long index) {
  GariFont * font;
  TTF_Font * ttf;
  gari_font_ttf_start(); // start ttf lib in case it wasn't started.
  ttf = TTF_OpenFontIndex(filename, ptsize, index);
  if (!ttf) {  return NULL; }  
  font         = GARI_ALLOCATE(GariFont);
  // If malloc failed, free ttf and get out of here.
  if (!font) {  TTF_CloseFont(ttf); return NULL; }
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

// transforms a gari Color (rgba) into an SDL_Color 
SDL_Color gari_rgba_sdl(GariColor rgba) {
  SDL_Color result;
  result.r = rgba.r;
  result.g = rgba.g;
  result.b = rgba.b;
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
    default:
      return gari_image_wrap(TTF_RenderUTF8_Blended(ttf, utf8, fg));    
  }  
  return NULL;

} 

GariImage * gari_image_convert(GariImage * srci, 
                               GariImage * dsti, Uint32 flags) {
  SDL_Surface * src = gari_image_surface(srci);
  SDL_Surface * dst = gari_image_surface(dsti);
  if((!src) || (!dst)) return NULL;
  return gari_image_wrap(SDL_ConvertSurface(src, dst->format, flags));
}

void gari_font_drawcolor(GariImage * image, int x, int y, 
  char * utf8, GariFont * font, GariColor fg, GariColor bg) {  
  GariImage * rendered, * converted;
  rendered  = gari_font_render(font, utf8, fg, bg);
  // converted = gari_image_convert(rendered, image, SDL_SRCALPHA);
  // gari_image_blit(image, x, y, converted); 
  if (rendered) {
    gari_image_blit(image, x, y, rendered);
    gari_image_free(rendered);
  }  
  // gari_image_free(converted);
}

void gari_font_draw(GariImage * image, int x, int y, char * utf8, GariFont * font, uint8_t fg_r, uint8_t fg_g, uint8_t fg_b, uint8_t bg_r, uint8_t bg_g, uint8_t bg_b) {
  GariColor fg = { fg_r, fg_g, fg_b, 0};
  GariColor bg = { bg_r, bg_g, bg_b, 0};
  gari_font_drawcolor(image, x, y, utf8, font, fg, bg);  
}



void gari_font_printf(GariImage * image, int x, int y, GariFont * font, GariColor fg, GariColor bg, char * format, ...) {  
  GariImage * rendered;
  va_list ap;
  char buffer[2001];
  va_start(ap, format);   
  vsnprintf(buffer, 2000, format, ap);
  va_end(ap);
  gari_font_drawcolor(image, x, y, buffer, font, fg, bg);  
}  


/** Returns the width that the given UTF-8 encoded text would be if it was rendered using gari_fonr_render. */
int gari_font_renderwidth(GariFont * font, char * utf8) {
  int w; 
  TTF_Font * ttf = GARI_FONT_TTF(font);
  TTF_SizeUTF8(ttf, (const char *) utf8, &w, NULL);
  return w; 
}

/** Returns the font's max height */
int gari_font_height(GariFont * font) {
  TTF_Font * ttf = GARI_FONT_TTF(font);
  return TTF_FontHeight(ttf); 
}

/** Returns the font's font max ascent (y above origin)*/
int gari_font_ascent(GariFont * font) {
  TTF_Font * ttf = GARI_FONT_TTF(font);
  return TTF_FontAscent(ttf); 
}

/** Returns the font's min descent (y below origin)*/
int gari_font_descent(GariFont * font) {
  TTF_Font * ttf = GARI_FONT_TTF(font);
  return TTF_FontDescent(ttf); 
}

/** Returns the font's recommended line spacing. */
int gari_font_lineskip(GariFont * font) {
  TTF_Font * ttf = GARI_FONT_TTF(font);
  return TTF_FontLineSkip(ttf); 
}

int gari_fontstyle_tottf(int style) {
  int result = TTF_STYLE_NORMAL;
  if (style & GariFontItalic    ) result |= TTF_STYLE_ITALIC;
  if (style & GariFontBold      ) result |= TTF_STYLE_BOLD;
  if (style & GariFontUnderline ) result |= TTF_STYLE_UNDERLINE;
  return result;  
} 

int gari_fontstyle_fromttf(int style) {
  int result = GariFontNormal;
  if (style & TTF_STYLE_ITALIC    ) result |= GariFontItalic;
  if (style & TTF_STYLE_BOLD      ) result |= GariFontBold;
  if (style & TTF_STYLE_UNDERLINE ) result |= GariFontUnderline;
  return result;
} 

/** Returns the style of the font.  */
int gari_font_style(GariFont * font) {
  TTF_Font * ttf = GARI_FONT_TTF(font);
  return gari_fontstyle_fromttf(TTF_GetFontStyle(ttf));
}  

/** Sets the style of the font. Note that the style may be "faked" 
by the underlaying implementation. Use a different font for better effects. */
void gari_font_style_(GariFont * font, int style) {
  TTF_Font * ttf = GARI_FONT_TTF(font);
  TTF_SetFontStyle(ttf, gari_fontstyle_tottf(style));
}  

/** Returns the amount of font faces in a font. */
int gari_font_faces(GariFont * font) {
  TTF_Font * ttf = GARI_FONT_TTF(font);
  return TTF_FontFaces(ttf);
}



