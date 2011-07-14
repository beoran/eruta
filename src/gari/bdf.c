#include "gari.h"
#define GARI_INTERN_ONLY
#include "gari_intern.h"


/** GariBdf supports bitmapz up to 32x32 pixels. */
#define GARI_BDF_MAXWIDE 32
#define GARI_BDF_MAXHIGH 32

/** Struct that models a glyph. */
struct GariBdfGlyph_ {
  int bbx, bby, bbxoff, bbyoff; // bounding box.
  uint32_t encoding; 
  uint32_t pixels[GARI_BDF_MAXHIGH];
};

/** GariBdf supports up to this amount of glyphs. */
#define GARI_BDF_GLYPHS 65536


/** Loading and drawing of BDF fonts. The font should have a unicode encoding */
struct GariBdf_ {
  int bbx, bby, bbxoff, bbyoff; // bounding box. 
  int high;
  GariFontStyle   style;
  GariFontMode    mode;
  int             ptsize;
  int             scale;
  size_t          nglyphs; /* Amount of glyphs. */
  GariBdfGlyph *  glyphs; /* Glyphs.  */
};

/* GariBdf stores the exact amount of glyphs that the font contains to 
save space. However, this means that the glyph will have to be found
by doing a binary search over the glyphs. So in short, this implementation
is slower to save space.
*/


/* fgetc equivalent for SDL_RWops */
int SDL_RWgetc(SDL_RWops *rw) {
  char c;
  int res = SDL_RWread(rw, &c, 1, 1);
  return  res == 1 ? c : EOF;
}

/* fgets equivalent for SDL_rwops. Only accepts '\n' as a newline.  */
char * SDL_RWgets(char *buf, int count, SDL_RWops *rw) {
  int index;
  buf[count - 1] = '\0'; // null terminate buffer.
  for (index = 0; index < count - 1; index++)  {
    int ch = SDL_RWgetc(rw);
    if (ch == EOF) {
      if (index == 0) return NULL;
      else break; // don't store eof.
    }
    buf[index] = ch; // Store character.
    if(ch == '\n') {
      break;
    }
  }
  if((index+1) <= (count-1)) {   
    buf[index+1] = '\0'; // null terminate buffer.
  }  
  return buf;
}


// The BDF font file can use lines up to this length
#define GARI_BDF_LINE_SIZE 1024
typedef struct GariBdfValue_ GariBdfValue;

#define GARI_BDF_MAXVALUES 8
#define GARI_BDF_MAXKEY    512
#define GARI_BDF_MAXSTR    512

struct GariBdfValue_ {
  char    key[GARI_BDF_MAXKEY];
  int     type;
  int     size;
  char    str[GARI_BDF_MAXSTR];
  long    values[GARI_BDF_MAXVALUES];
  double  number; 
};

enum GariBdfValueType_ {
  GariBdfNone    = 0 ,  
  GariBdfString  = 1 ,
  GariBdfInteger = 2 ,
  GariBdfNumber  = 3 
};

/* Perses  a line into a bdf key/value pair.  */
GariBdfValue * gari_bdfvalue_line(GariBdfValue * self, char * line) {
  char * p = strchr(line, ' ');
  char * s = NULL, * e = NULL;
  int index  = 0;
  self->type = GariBdfNone;
  size_t len = p - line;
  if(!p) len = strlen(line) - 1; // no space in line
  // fail if key too long 
  if (len >= GARI_BDF_MAXKEY) len = GARI_BDF_MAXKEY - 1;
  memset(self, 0, sizeof(GariBdfValue)); // wipe all values. 
  strncpy(self->key, line, len); self->key[len] = '\0';
  if(!p) return self;
  s = strchr(p, '"');
  if(s) { // string value
    self->type = GariBdfString;
    e     = strrchr(p, '"');
    self->size  = e-s;
    if (self->size >= GARI_BDF_MAXSTR) self->size = GARI_BDF_MAXSTR - 1;
    strncpy(self->str, s, self->size); self->key[self->size] = '\0';
    return self;
  }
  s = strchr(p, '.');
  // double value 
  if(s) {
    self->type    = GariBdfNumber;
    self->number  = atof(p); 
    return self;
  }
  // values not found
  self->size = 0;
  // try to fetch numbers 
  while(p && self->size < GARI_BDF_MAXVALUES) {
    self->values[self->size] = strtol(p +1 , &e, 10);
    if((!e) || e==p) return self; 
    // if this happens integer conversion failed, so give up. 
    self->type = GariBdfInteger;
    self->size++;
    p = e; // move to next position.  
  }
  return self;  
} 
 
/*
static void ParseChar(Kanji_Font* font, int index, FILE* fp, int shift) {
  char buf[GARI_BDF_LINE_SIZE], *p;
  int y;

  if (font->moji[index] != 0) return;

  font->moji[index] = (Uint32*)malloc(sizeof(Uint32)*font->k_size);

  for (y = 0; y < font->k_size; y++) {
    fgets(buf, BUF, fp);
    font->moji[index][y] = (strtol(buf, 0, 16) >> shift);
  }
}
*/

GariBdfGlyph * gari_bdfglyph_parse(GariBdfGlyph * self, SDL_RWops* rw) {
  int index;
  char buf[GARI_BDF_LINE_SIZE];
  
  for (index = 0; index < self->bby; index++) { 
    if (SDL_RWgets(buf, GARI_BDF_LINE_SIZE, rw) == NULL) return NULL;
    self->pixels[index] = strtol(buf, NULL, 16);
  }
  return self;
}

GariBdf * gari_bdf_parsechar(GariBdf * self, SDL_RWops* rw, int index) {
  char buf[GARI_BDF_LINE_SIZE];
  GariBdfValue value;
  GariBdfGlyph * glyph = self->glyphs + index;
  while (1) {
    if (SDL_RWgets(buf, GARI_BDF_LINE_SIZE, rw) == NULL) return self;
    if(!gari_bdfvalue_line(&value, buf)) return NULL; 
    if (!strcmp(value.key, "ENDCHAR")) return self; // end of character
    if (!strcmp(value.key, "ENCODING")) { // encoding 
      glyph->encoding    = value.values[0];
    }  
    if (!strcmp(value.key, "BBX")) { 
      // read glyph bound box
      glyph->bbx    = value.values[0];
      glyph->bby    = value.values[1];
      glyph->bbxoff = value.values[2];
      glyph->bbyoff = value.values[3];
    }
    if (!strcmp(value.key, "BITMAP")) {
      if(!gari_bdfglyph_parse(glyph, rw)) return NULL;
    }
  }
  return self;
}


GariBdf * gari_bdf_parse(GariBdf * self, SDL_RWops* rw) {
  char buf[GARI_BDF_LINE_SIZE];
  GariBdfValue value; 
  
  uint32_t index = 0, size = 0, encoding = 0;
  int k_rshift, a_rshift;
  int s;
  self->glyphs = NULL;
  while (1) {
    if (SDL_RWgets(buf, GARI_BDF_LINE_SIZE, rw) == NULL) goto return_ok;
    if(!gari_bdfvalue_line(&value, buf)) goto return_error; 
    if (strstr(value.key, "CHARS")) {
      self->nglyphs = value.values[0];
      self->glyphs  = GARI_MALLOC(self->nglyphs * sizeof(GariBdfGlyph));
      if(!self->glyphs) return NULL;
    }
    
    if (strstr(value.key, "FONTBOUNDINGBOX")) {
      self->bbx    = value.values[0];
      self->bby    = value.values[1];
      self->bbxoff = value.values[2];
      self->bbyoff = value.values[3];
    }

    if (strstr(value.key, "STARTCHAR")) {
      if(!gari_bdf_parsechar(self, rw, index)) goto return_error;
      index ++;
      // On to the next index; if too much return ok
      if (index >= self->nglyphs) goto return_ok;
    }
  }
  return_ok: // normal return. 
  return self;   
  return_error: // error return  
  GARI_FREE(self->glyphs);
  return NULL;
}


/** Gets the drawing mode of the font. */
int gari_bdf_mode(GariBdf * self) {
  if(!self) return -1;
  return self->mode;
}

/** Sets the drawing mode of the font. */
GariBdf * gari_bdf_mode_(GariBdf * self, int mode) {
  if (!self) return NULL;
  self->mode = mode; 
  return self;
}


/** Returns the style of the font.  */
int gari_bdf_style(GariBdf * self) {
  if(!self) return -1;
  return self->style;
}

/** Sets the style effect of the font. */
GariBdf * gari_bdf_style_(GariBdf * self, int style) {
  if (!self) return NULL;
  self->style = style; 
  return self;
}

GariBdf * gari_bdf_newrw(SDL_RWops * rw) {
  if(!rw) return NULL;
  GariBdf * self = GARI_MALLOC(sizeof(GariBdf));
  if(!self) { return NULL;} 
  // self->ptsize = NOFONT_GLYPHINFO_ROWS * scale;
  gari_bdf_mode_(self, GariFontSolid);
  gari_bdf_style_(self, GariFontNormal);  
  return gari_bdf_parse(self, rw);
}

/** Loads a bdf font. */
GariBdf * gari_bdf_load(char * filename) {
  SDL_RWops * rw = SDL_RWFromFile(filename, "rb");
  GariBdf * res = gari_bdf_newrw(rw);
  SDL_RWclose(rw); 
  return res;
}

/** Frees the memory allocated by the font. */
GariBdf * gari_bdf_free(GariBdf *font) {
  GARI_FREE(font);
  return NULL;
}

/** Gets the glyph at index index. Has noting to do with the encoding!  */
GariBdfGlyph * gari_bdf_glyph(GariBdf * font, int index) {
  if(!font) return NULL;
  if(!font->glyphs) return NULL;
  if(index < 0) return NULL;
  if(index >= font->nglyphs) return NULL;
  return font->glyphs + index;
}

/** Finds the glyph to be used for drawing the character encoded as encoding,
* recursively though a binary search. 
*/
GariBdfGlyph * gari_bdf_findr(GariBdf * self, int encoding, int index, int low, int high) {
  GariBdfGlyph * glyph = gari_bdf_glyph(self, index);
  if (!glyph) return NULL;
  if (glyph->encoding == encoding) return glyph;
  if (low==high) return NULL; // not found  
  if(glyph->encoding < encoding) { // we chose an index that is too low
    low  = index;
    index = low + (high - low) / 2;
    return gari_bdf_findr(self, encoding, index, low, high);    
  } else { // an index mid that is too high
    high  = index;
    index = low + (high - low) / 2;     
    return gari_bdf_findr(self, encoding, index, low, high);
  } 
  return NULL; // can't happen.
} 


/** Finds the glyph to be used for drawing the character encoded as encoding. 
*/
GariBdfGlyph * gari_bdf_find(GariBdf * self, int encoding) {
  int low, high, mid; 
  if(!self) return NULL;
  low  = 0 ; high = self->nglyphs;
  return gari_bdf_findr(self, encoding, high / 2, low, high);
}

/** Calculate nex tpower of two */
uint32_t gari_nextpower2(uint32_t v) { 
  v--;
  v |= v >> 1;
  v |= v >> 2;
  v |= v >> 4;
  v |= v >> 8;
  v |= v >> 16;
  v++;
  return v;
}

uint32_t gari_nextmultiple(uint32_t v, uint32_t of) {
  return (v+(of-1))&~(of-1);
}

/** Utf8 decoder */
// Copyright (c) 2008-2010 Bjoern Hoehrmann <bjoern@hoehrmann.de>
// See http://bjoern.hoehrmann.de/utf-8/decoder/dfa/ for details.
/*
License

Copyright (c) 2008-2009 Bjoern Hoehrmann <bjoern@hoehrmann.de>

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#define UTF8_ACCEPT 0
#define UTF8_REJECT 12

static const uint8_t utf8d[] = {
  // The first part of the table maps bytes to character classes that
  // to reduce the size of the transition table and create bitmasks.
   0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
   0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
   0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
   0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
   1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,  9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,
   7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,  7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,
   8,8,2,2,2,2,2,2,2,2,2,2,2,2,2,2,  2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,
  10,3,3,3,3,3,3,3,3,3,3,3,3,4,3,3, 11,6,6,6,5,8,8,8,8,8,8,8,8,8,8,8,

  // The second part is a transition table that maps a combination
  // of a state of the automaton and a character class to a state.
   0,12,24,36,60,96,84,12,12,12,48,72, 12,12,12,12,12,12,12,12,12,12,12,12,
  12, 0,12,12,12,12,12, 0,12, 0,12,12, 12,24,12,12,12,12,12,24,12,24,12,12,
  12,12,12,12,12,12,12,24,12,12,12,12, 12,24,12,12,12,12,12,12,12,24,12,12,
  12,12,12,12,12,12,12,36,12,36,12,12, 12,36,12,12,12,12,12,36,12,36,12,12,
  12,36,12,12,12,12,12,12,12,12,12,12, 
};

uint32_t utf8_decoder(uint32_t* state, uint32_t* codep, uint32_t byte) {
  uint32_t type = utf8d[byte];

  *codep = (*state != UTF8_ACCEPT) ?
    (byte & 0x3fu) | (*codep << 6) :
    (0xff >> type) & (byte);

  *state = utf8d[256 + *state + type];
  return *state;
}

uint32_t utf8_decode(char * str, int * ok) {
  uint32_t codepoint;
  uint32_t state = UTF8_ACCEPT;
  int      count;

  for ( ; str ; str++) { 
    utf8_decoder(&state, &codepoint, *str);
    if(state == UTF8_ACCEPT) {
      (*ok) = 1;
      return codepoint; 
    }
    if(state == UTF8_REJECT) {
      (*ok) = 0;
      return 0; 
    }
  }
  (*ok) = 0;
  return 0; 
}




/** Draws a signle glyph from the font. Does not lock the image, 
call ! */
void gari_bdf_putglyph(GariImage * image, int x, int y, GariBdfGlyph * glyph,                            GariBdf * font, GariColor fg, GariColor bg) {
  int xindex, yindex, wide, high;
  int xdraw;
  int ydraw; 
  uint32_t pixwide;
  GariDye fgdye        = gari_color_dye(fg, image);
  GariDye bgdye        = gari_color_dye(bg, image);
  GariColor red        = { 255, 0, 0, 255};
  GariDye reddye       = gari_color_dye(red, image);
  if(!glyph) return;
  wide  = glyph->bbx;
  high  = glyph->bby;
  xdraw = x - glyph->bbxoff;
  ydraw = y - glyph->bbyoff;
  pixwide = gari_nextmultiple(wide, 8);
     
    
  for(yindex = 0; yindex < high; yindex++, ydraw++) {
    for(xindex = 0; xindex <= wide; xindex++, xdraw++) {
      if((1<<(pixwide-xindex)) & glyph->pixels[yindex]) {
        gari_image_putpixel_nolock(image, xdraw, ydraw, fgdye);
      } else if(font->mode == GariFontShaded) {
        gari_image_putpixel_nolock(image, xdraw, ydraw, bgdye);
      }
    }
    xdraw = x - glyph->bbxoff;
  }
  gari_image_putpixel_nolock(image, x, y, reddye);
  gari_image_putpixel_nolock(image, x + wide, y + wide, reddye);
  
}


/** Draws a single utf8 character to a surface at the given coordinates 
using the font to a surface, depending on the font's settings. */
void gari_bdf_putc(GariImage * image, int x, int y, int utf8, GariBdf * font,  
              GariColor fg, GariColor bg) {
  GariBdfGlyph * glyph = gari_bdf_find(font, utf8);
  gari_image_lock(image);
  gari_bdf_putglyph(image, x, y, glyph, font, fg, bg); 
  gari_image_unlock(image);
}



/** Renders the font to a surface, depending on the font's settings. */
GariImage * gari_bdf_render(GariBdf * font, char * utf8, 
    GariColor fgrgba, GariColor bgrgba);

/** Draws font with given colors. */
void gari_bdf_drawcolor(GariImage * image, int x, int y, char * utf8, 
                         GariBdf  * font , GariColor fg, GariColor bg); 

/** Draws font with given color components. */
void gari_bdf_draw(GariImage * image, int x, int y, char * utf8, GariBdf *
font, uint8_t fg_r, uint8_t fg_g, uint8_t fg_b, uint8_t bg_r, uint8_t bg_b,
uint8_t bg_a);

/** Draws font in printf style. Won't work on platforms that lack vsnprintf.
* Will draw up to 2000 bytes of characters.  
*/
void gari_bdf_printf(GariImage * image, int x, int y, GariBdf * font,
GariColor fg, GariColor bg, char * format, ...);

/** Returns a text with details about the last error in loading or 
handling a font. */
char * gari_bdf_error();

/** Returns the width that the given UTF-8 encoded text would be if it was
rendered using gari_fonr_render. */
int gari_bdf_renderwidth(GariBdf * font, char * utf8); 

/** Returns the font's max height */
int gari_bdf_height(GariBdf * font); 

/** Returns the font's font max ascent (y above origin)*/
int gari_bdf_ascent(GariBdf * font); 

/** Returns the font's min descent (y below origin)*/
int gari_bdf_descent(GariBdf * font);

/** Returns the font's recommended line spacing. */
int gari_bdf_lineskip(GariBdf * font);

int gari_fontstyle_tottf(int style);

int gari_fontstyle_fromttf(int style); 


