#include "gy.h"

#define GY_INTERN_ONLY
#include "gy_intern.h"
#include <stdarg.h>

struct GyFont_ {
  TTF_Font   * ttf;
  int          ptsize;
  int          mode;
  long         index;
  char       * filename;
  GyImage ** glyphs;
};


int gyfont_ttf_start() {
  if(!TTF_WasInit() && TTF_Init()==-1) {
    return FALSE;
  }
  return TRUE;
} 


/** Sets the drawing mode of the font. */
GyFont * gyfont_mode_(GyFont * font, int mode) {
  if(!font) { return NULL ; }
  font->mode = mode;
  return font;
}


/** Gets the drawing mode of the font. */
int gyfont_mode(GyFont * font) {
  if(!font) { return 0 ; }
  return font->mode;
}

GyFont * gyfont_init(GyFont * font, TTF_Font * ttffont, 
           char *name, int ptsize, long index) {  
  if(!font) { return NULL ; }
  font->ptsize  = ptsize;
  font->index   = index;
  font->filename= name; 
  font->ttf     = ttffont; 
  if(!font->ttf) { return NULL ; }
  return gyfont_mode_(font, GyFontBlended);
}

char * gyfont_error() {
  return TTF_GetError();
}

#define GY_FONT_TTF(FONT) ((FONT)->ttf)

/** Loads font from data in memory */
GyFont * gyfont_dataindex(const unsigned char * data, int datasize, 
                               int ptsize, long index) {
  GyFont * font;
  TTF_Font * ttf;  
  SDL_RWops * rw;

  gyfont_ttf_start(); // start ttf lib in case it wasn't started.
  rw  = SDL_RWFromConstMem((const void *)data, datasize);
  if (!rw)  { return NULL; }                               
  ttf = TTF_OpenFontIndexRW(rw, 1, ptsize, index);
  // SDL_RWclose(rw);
  if (!ttf) { return NULL; }  
  font         = GY_ALLOCATE(GyFont);
  // If malloc failed, free ttf and get out of here.
  if (!font) {  TTF_CloseFont(ttf); return NULL; }
  return gyfont_init(font, ttf, "<data>", ptsize, index); 
}

/** Loads font from data in memory */
GyFont * gyfont_data(const unsigned char * data, int datasize, int ptsize){
  return gyfont_dataindex(data, datasize, ptsize, 0);  
} 



GyFont * gyfont_loadindex(char * filename, int ptsize, long index) {
  GyFont * font;
  TTF_Font * ttf;
  gyfont_ttf_start(); // start ttf lib in case it wasn't started.
  ttf = TTF_OpenFontIndex(filename, ptsize, index);
  if (!ttf) {  return NULL; }  
  font         = GY_ALLOCATE(GyFont);
  // If malloc failed, free ttf and get out of here.
  if (!font) {  TTF_CloseFont(ttf); return NULL; }
  return gyfont_init(font, ttf, filename, ptsize, index); 
}

GyFont * gyfont_load(char * filename, int ptsize) {
  return gyfont_loadindex(filename, ptsize, 0);
}

void gyfont_free(GyFont * font) {
  if(!font) { return; } 
  if(!font->ttf) { return; }
  TTF_CloseFont(font->ttf);
  font->ttf = NULL;
  GY_FREE(font);
}

// transforms a gy Color (rgba) into an SDL_Color 
SDL_Color gyrgba_sdl(GyColor rgba) {
  SDL_Color result;
  result.r = rgba.r;
  result.g = rgba.g;
  result.b = rgba.b;
  return result;
}

/** Renders the font to a surface, depending on the font's settings. */
GyImage * gyfont_render(GyFont * font, char * utf8, 
    GyColor fgrgba, GyColor bgrgba) {
  TTF_Font * ttf = GY_FONT_TTF(font);
  SDL_Color fg = gyrgba_sdl(fgrgba);
  SDL_Color bg = gyrgba_sdl(fgrgba);
  switch(font->mode) { 
    case GyFontSolid:
      return gyimage_wrap(TTF_RenderUTF8_Solid(ttf, utf8, fg));
    case GyFontShaded:
      return gyimage_wrap(TTF_RenderUTF8_Shaded(ttf, utf8, fg, bg));
    case GyFontBlended:
    default:
      return gyimage_wrap(TTF_RenderUTF8_Blended(ttf, utf8, fg));    
  }  
  return NULL;

} 

GyImage * gyimage_convert(GyImage * srci, 
                               GyImage * dsti, Uint32 flags) {
  SDL_Surface * src = gyimage_surface(srci);
  SDL_Surface * dst = gyimage_surface(dsti);
  if((!src) || (!dst)) return NULL;
  return gyimage_wrap(SDL_ConvertSurface(src, dst->format, flags));
}

void gyfont_drawcolor(GyImage * image, int x, int y, 
  char * utf8, GyFont * font, GyColor fg, GyColor bg) {  
  GyImage * rendered, * converted;
  rendered  = gyfont_render(font, utf8, fg, bg);
  // converted = gyimage_convert(rendered, image, SDL_SRCALPHA);
  // gyimage_blit(image, x, y, converted); 
  if (rendered) {
    gyimage_blit(image, x, y, rendered);
    gyimage_free(rendered);
  }  
  // gyimage_free(converted);
}

void gyfont_draw(GyImage * image, int x, int y, char * utf8, GyFont * font, uint8_t fg_r, uint8_t fg_g, uint8_t fg_b, uint8_t bg_r, uint8_t bg_g, uint8_t bg_b) {
  GyColor fg = { fg_r, fg_g, fg_b, 0};
  GyColor bg = { bg_r, bg_g, bg_b, 0};
  gyfont_drawcolor(image, x, y, utf8, font, fg, bg);  
}



void gyfont_printf(GyImage * image, int x, int y, GyFont * font, GyColor fg, GyColor bg, char * format, ...) {  
  GyImage * rendered;
  va_list ap;
  char buffer[2001];
  va_start(ap, format);   
  vsnprintf(buffer, 2000, format, ap);
  va_end(ap);
  gyfont_drawcolor(image, x, y, buffer, font, fg, bg);  
}  


/** Returns the width that the given UTF-8 encoded text would be if it was rendered using gyfonr_render. */
int gyfont_renderwidth(GyFont * font, char * utf8) {
  int w; 
  TTF_Font * ttf = GY_FONT_TTF(font);
  TTF_SizeUTF8(ttf, (const char *) utf8, &w, NULL);
  return w; 
}

/** Returns the font's max height */
int gyfont_height(GyFont * font) {
  TTF_Font * ttf = GY_FONT_TTF(font);
  return TTF_FontHeight(ttf); 
}

/** Returns the font's font max ascent (y above origin)*/
int gyfont_ascent(GyFont * font) {
  TTF_Font * ttf = GY_FONT_TTF(font);
  return TTF_FontAscent(ttf); 
}

/** Returns the font's min descent (y below origin)*/
int gyfont_descent(GyFont * font) {
  TTF_Font * ttf = GY_FONT_TTF(font);
  return TTF_FontDescent(ttf); 
}

/** Returns the font's recommended line spacing. */
int gyfont_lineskip(GyFont * font) {
  TTF_Font * ttf = GY_FONT_TTF(font);
  return TTF_FontLineSkip(ttf); 
}

int gyfontstyle_tottf(int style) {
  int result = TTF_STYLE_NORMAL;
  if (style & GyFontItalic    ) result |= TTF_STYLE_ITALIC;
  if (style & GyFontBold      ) result |= TTF_STYLE_BOLD;
  if (style & GyFontUnderline ) result |= TTF_STYLE_UNDERLINE;
  return result;  
} 

int gyfontstyle_fromttf(int style) {
  int result = GyFontNormal;
  if (style & TTF_STYLE_ITALIC    ) result |= GyFontItalic;
  if (style & TTF_STYLE_BOLD      ) result |= GyFontBold;
  if (style & TTF_STYLE_UNDERLINE ) result |= GyFontUnderline;
  return result;
} 

/** Returns the style of the font.  */
int gyfont_style(GyFont * font) {
  TTF_Font * ttf = GY_FONT_TTF(font);
  return gyfontstyle_fromttf(TTF_GetFontStyle(ttf));
}  

/** Sets the style of the font. Note that the style may be "faked" 
by the underlaying implementation. Use a different font for better effects. */
void gyfont_style_(GyFont * font, int style) {
  TTF_Font * ttf = GY_FONT_TTF(font);
  TTF_SetFontStyle(ttf, gyfontstyle_tottf(style));
}  

/** Returns the amount of font faces in a font. */
int gyfont_faces(GyFont * font) {
  TTF_Font * ttf = GY_FONT_TTF(font);
  return TTF_FontFaces(ttf);
}



