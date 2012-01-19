#include "gy.h"
#define GY_INTERN_ONLY
#include "gy_intern.h"


/** GyBdf supports bitmapz up to 32x32 pixels. */
#define GY_BDF_MAXWIDE 32
#define GY_BDF_MAXHIGH 32

/** Struct that models a glyph. */
struct GyBdfGlyph_ {
  int bbx, bby, bbxoff, bbyoff; // bounding box.
  uint32_t encoding; 
  uint32_t pixels[GY_BDF_MAXHIGH];
};

/** GyBdf supports up to this amount of glyphs. */
#define GY_BDF_GLYPHS 65536


/** Loading and drawing of BDF fonts. The font should have a unicode encoding */
struct GyBdf_ {
  int bbx, bby, bbxoff, bbyoff; // bounding box. 
  int high;
  GyFontStyle   style;
  GyFontMode    mode;
  int             ptsize;
  int             scale;
  size_t          nglyphs; /* Amount of glyphs. */
  GyBdfGlyph *  glyphs; /* Glyphs.  */
};

/* GyBdf stores the exact amount of glyphs that the font contains to 
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
#define GY_BDF_LINE_SIZE 1024
typedef struct GyBdfValue_ GyBdfValue;

#define GY_BDF_MAXVALUES 8
#define GY_BDF_MAXKEY    512
#define GY_BDF_MAXSTR    512

struct GyBdfValue_ {
  char    key[GY_BDF_MAXKEY];
  int     type;
  int     size;
  char    str[GY_BDF_MAXSTR];
  long    values[GY_BDF_MAXVALUES];
  double  number; 
};

enum GyBdfValueType_ {
  GyBdfNone    = 0 ,  
  GyBdfString  = 1 ,
  GyBdfInteger = 2 ,
  GyBdfNumber  = 3 
};

/* Perses  a line into a bdf key/value pair.  */
GyBdfValue * gybdfvalue_line(GyBdfValue * self, char * line) {
  char * p = strchr(line, ' ');
  char * s = NULL, * e = NULL;
  int index  = 0;
  self->type = GyBdfNone;
  size_t len = p - line;
  if(!p) len = strlen(line) - 1; // no space in line
  // fail if key too long 
  if (len >= GY_BDF_MAXKEY) len = GY_BDF_MAXKEY - 1;
  memset(self, 0, sizeof(GyBdfValue)); // wipe all values. 
  strncpy(self->key, line, len); self->key[len] = '\0';
  if(!p) return self;
  s = strchr(p, '"');
  if(s) { // string value
    self->type = GyBdfString;
    e     = strrchr(p, '"');
    self->size  = e-s;
    if (self->size >= GY_BDF_MAXSTR) self->size = GY_BDF_MAXSTR - 1;
    strncpy(self->str, s, self->size); self->key[self->size] = '\0';
    return self;
  }
  s = strchr(p, '.');
  // double value 
  if(s) {
    self->type    = GyBdfNumber;
    self->number  = atof(p); 
    return self;
  }
  // values not found
  self->size = 0;
  // try to fetch numbers 
  while(p && self->size < GY_BDF_MAXVALUES) {
    self->values[self->size] = strtol(p +1 , &e, 10);
    if((!e) || e==p) return self; 
    // if this happens integer conversion failed, so give up. 
    self->type = GyBdfInteger;
    self->size++;
    p = e; // move to next position.  
  }
  return self;  
} 
 
/*
static void ParseChar(Kanji_Font* font, int index, FILE* fp, int shift) {
  char buf[GY_BDF_LINE_SIZE], *p;
  int y;

  if (font->moji[index] != 0) return;

  font->moji[index] = (Uint32*)malloc(sizeof(Uint32)*font->k_size);

  for (y = 0; y < font->k_size; y++) {
    fgets(buf, BUF, fp);
    font->moji[index][y] = (strtol(buf, 0, 16) >> shift);
  }
}
*/

GyBdfGlyph * gybdfglyph_parse(GyBdfGlyph * self, SDL_RWops* rw) {
  int index;
  char buf[GY_BDF_LINE_SIZE];
  
  for (index = 0; index < self->bby; index++) { 
    if (SDL_RWgets(buf, GY_BDF_LINE_SIZE, rw) == NULL) return NULL;
    self->pixels[index] = strtol(buf, NULL, 16);
  }
  return self;
}

GyBdf * gybdf_parsechar(GyBdf * self, SDL_RWops* rw, int index) {
  char buf[GY_BDF_LINE_SIZE];
  GyBdfValue value;
  GyBdfGlyph * glyph = self->glyphs + index;
  while (1) {
    if (SDL_RWgets(buf, GY_BDF_LINE_SIZE, rw) == NULL) return self;
    if(!gybdfvalue_line(&value, buf)) return NULL; 
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
      if(!gybdfglyph_parse(glyph, rw)) return NULL;
    }
  }
  return self;
}


GyBdf * gybdf_parse(GyBdf * self, SDL_RWops* rw) {
  char buf[GY_BDF_LINE_SIZE];
  GyBdfValue value; 
  
  uint32_t index = 0, size = 0, encoding = 0;
  int k_rshift, a_rshift;
  int s;
  self->glyphs = NULL;
  while (1) {
    if (SDL_RWgets(buf, GY_BDF_LINE_SIZE, rw) == NULL) goto return_ok;
    if(!gybdfvalue_line(&value, buf)) goto return_error; 
    if (strstr(value.key, "CHARS")) {
      self->nglyphs = value.values[0];
      self->glyphs  = GY_MALLOC(self->nglyphs * sizeof(GyBdfGlyph));
      if(!self->glyphs) return NULL;
    }
    
    if (strstr(value.key, "FONTBOUNDINGBOX")) {
      self->bbx    = value.values[0];
      self->bby    = value.values[1];
      self->bbxoff = value.values[2];
      self->bbyoff = value.values[3];
    }

    if (strstr(value.key, "STARTCHAR")) {
      if(!gybdf_parsechar(self, rw, index)) goto return_error;
      index ++;
      // On to the next index; if too much return ok
      if (index >= self->nglyphs) goto return_ok;
    }
  }
  return_ok: // normal return. 
  return self;   
  return_error: // error return  
  GY_FREE(self->glyphs);
  return NULL;
}


/** Gets the drawing mode of the font. */
int gybdf_mode(GyBdf * self) {
  if(!self) return -1;
  return self->mode;
}

/** Sets the drawing mode of the font. */
GyBdf * gybdf_mode_(GyBdf * self, int mode) {
  if (!self) return NULL;
  self->mode = mode; 
  return self;
}


/** Returns the style of the font.  */
int gybdf_style(GyBdf * self) {
  if(!self) return -1;
  return self->style;
}

/** Sets the style effect of the font. */
GyBdf * gybdf_style_(GyBdf * self, int style) {
  if (!self) return NULL;
  self->style = style; 
  return self;
}

GyBdf * gybdf_newrw(SDL_RWops * rw) {
  if(!rw) return NULL;
  GyBdf * self = GY_MALLOC(sizeof(GyBdf));
  if(!self) { return NULL;} 
  // self->ptsize = NOFONT_GLYPHINFO_ROWS * scale;
  gybdf_mode_(self, GyFontSolid);
  gybdf_style_(self, GyFontNormal);  
  return gybdf_parse(self, rw);
}

/** Loads a bdf font. */
GyBdf * gybdf_load(char * filename) {
  SDL_RWops * rw = SDL_RWFromFile(filename, "rb");
  GyBdf * res = gybdf_newrw(rw);
  SDL_RWclose(rw); 
  return res;
}

/** Frees the memory allocated by the font. */
GyBdf * gybdf_free(GyBdf *font) {
  GY_FREE(font);
  return NULL;
}

/** Gets the glyph at index index. Has noting to do with the encoding!  */
GyBdfGlyph * gybdf_glyph(GyBdf * font, int index) {
  if(!font) return NULL;
  if(!font->glyphs) return NULL;
  if(index < 0) return NULL;
  if(index >= font->nglyphs) return NULL;
  return font->glyphs + index;
}

/** Finds the glyph to be used for drawing the character encoded as encoding,
* recursively though a binary search. 
*/
GyBdfGlyph * gybdf_findr(GyBdf * self, int encoding, int index, int low, int high) {
  GyBdfGlyph * glyph = gybdf_glyph(self, index);
  if (!glyph) return NULL;
  if (glyph->encoding == encoding) return glyph;
  if (low==high) return NULL; // not found  
  if(glyph->encoding < encoding) { // we chose an index that is too low
    low  = index;
    index = low + (high - low) / 2;
    return gybdf_findr(self, encoding, index, low, high);    
  } else { // an index mid that is too high
    high  = index;
    index = low + (high - low) / 2;     
    return gybdf_findr(self, encoding, index, low, high);
  } 
  return NULL; // can't happen.
} 


/** Finds the glyph to be used for drawing the character encoded as encoding. 
*/
GyBdfGlyph * gybdf_find(GyBdf * self, int encoding) {
  int low, high, mid; 
  if(!self) return NULL;
  low  = 0 ; high = self->nglyphs;
  return gybdf_findr(self, encoding, high / 2, low, high);
}

/** Calculate nex tpower of two */
uint32_t gynextpower2(uint32_t v) { 
  v--;
  v |= v >> 1;
  v |= v >> 2;
  v |= v >> 4;
  v |= v >> 8;
  v |= v >> 16;
  v++;
  return v;
}

uint32_t gynextmultiple(uint32_t v, uint32_t of) {
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
void gybdf_putglyph(GyImage * image, int x, int y, GyBdfGlyph * glyph,                            GyBdf * font, GyColor fg, GyColor bg) {
  int xindex, yindex, wide, high;
  int xdraw;
  int ydraw; 
  uint32_t pixwide;
  GyDye fgdye        = gycolor_dye(fg, image);
  GyDye bgdye        = gycolor_dye(bg, image);
  GyColor red        = { 255, 0, 0, 255};
  GyDye reddye       = gycolor_dye(red, image);
  if(!glyph) return;
  wide  = glyph->bbx;
  high  = glyph->bby;
  xdraw = x - glyph->bbxoff;
  ydraw = y - glyph->bbyoff;
  pixwide = gynextmultiple(wide, 8);
     
    
  for(yindex = 0; yindex < high; yindex++, ydraw++) {
    for(xindex = 0; xindex <= wide; xindex++, xdraw++) {
      if((1<<(pixwide-xindex)) & glyph->pixels[yindex]) {
        gyimage_putpixel_nolock(image, xdraw, ydraw, fgdye);
      } else if(font->mode == GyFontShaded) {
        gyimage_putpixel_nolock(image, xdraw, ydraw, bgdye);
      }
    }
    xdraw = x - glyph->bbxoff;
  }
  gyimage_putpixel_nolock(image, x, y, reddye);
  gyimage_putpixel_nolock(image, x + wide, y + wide, reddye);
  
}


/** Draws a single utf8 character to a surface at the given coordinates 
using the font to a surface, depending on the font's settings. */
void gybdf_putc(GyImage * image, int x, int y, int utf8, GyBdf * font,  
              GyColor fg, GyColor bg) {
  GyBdfGlyph * glyph = gybdf_find(font, utf8);
  gyimage_lock(image);
  gybdf_putglyph(image, x, y, glyph, font, fg, bg); 
  gyimage_unlock(image);
}



/** Renders the font to a surface, depending on the font's settings. */
GyImage * gybdf_render(GyBdf * font, char * utf8, 
    GyColor fgrgba, GyColor bgrgba);

/** Draws font with given colors. */
void gybdf_drawcolor(GyImage * image, int x, int y, char * utf8, 
                         GyBdf  * font , GyColor fg, GyColor bg); 

/** Draws font with given color components. */
void gybdf_draw(GyImage * image, int x, int y, char * utf8, GyBdf *
font, uint8_t fg_r, uint8_t fg_g, uint8_t fg_b, uint8_t bg_r, uint8_t bg_b,
uint8_t bg_a);

/** Draws font in printf style. Won't work on platforms that lack vsnprintf.
* Will draw up to 2000 bytes of characters.  
*/
void gybdf_printf(GyImage * image, int x, int y, GyBdf * font,
GyColor fg, GyColor bg, char * format, ...);

/** Returns a text with details about the last error in loading or 
handling a font. */
char * gybdf_error();

/** Returns the width that the given UTF-8 encoded text would be if it was
rendered using gyfonr_render. */
int gybdf_renderwidth(GyBdf * font, char * utf8); 

/** Returns the font's max height */
int gybdf_height(GyBdf * font); 

/** Returns the font's font max ascent (y above origin)*/
int gybdf_ascent(GyBdf * font); 

/** Returns the font's min descent (y below origin)*/
int gybdf_descent(GyBdf * font);

/** Returns the font's recommended line spacing. */
int gybdf_lineskip(GyBdf * font);

int gyfontstyle_tottf(int style);

int gyfontstyle_fromttf(int style); 


