#include "gari.h"
#define GARI_INTERN_ONLY
#include "gari_intern.h"

typedef struct NoFontGlyph_ {
  int  index;
  char name[16];
  char pixels[16][16];
} NoFontGlyph;

char * str = " ";

#define NFGLYPH7(INDEX, NAME, L0, L1, L2, L3, L4, L5, L6) \
  { INDEX , #NAME, { #L0, #L1, #L2, #L3, #L4, #L5, #L6} }


NoFontGlyph nofont_glyphs2[] = {
  { ' ', "space",
   { ".....",
   ".....",
   ".....",
   ".....",
   ".....",
   ".....",
   } 
  },
  NFGLYPH7('!', bang,
  ..x..,
  ..x..,
  ..x..,
  .....,
  ..x..,
  ..x.., 
  ),

};

#define NOFONT_GLYPHINFO_GLYPHS   10
#define NOFONT_GLYPHINFO_ROWS     7
#define NOFONT_GLYPHINFO_SUBCOLS  6
#define NOFONT_GLYPHINFO_COLS     60
#define NOFONT_GLYPHINFO_SPACE    500


struct NofontGlyphinfo_ {
  int  glyphs[NOFONT_GLYPHINFO_GLYPHS];  
  char pixels[NOFONT_GLYPHINFO_SPACE]; 
};

struct NofontGlyphinfo_ nofont_glyphs[20] = {  
{
  {' ', '!', '#', '$', '%', '&', '(', ')', '\'', '*'},
  {
  "..... ..x.. ..... ..x.. .x..x .xxx. ..x.. ..x.. ..x.. ....."
  "..... ..x.. .x.x. .xxxx x.x.x x.... ..x.. .x... ...x. ..x.."
  "..... ..x.. xxxxx x.x.. .x.x. .x... ..... x.... ....x x.x.x"
  "..... ..x.. .x.x. .xxx. ..x.. x.x.x ..... x.... ....x .xxx."
  "..... ..... xxxxx ..x.x .x.x. x..x. ..... x.... ....x x.x.x"
  "..... ..x.. .x.x. xxxx. x.x.x .xx.x ..... .x... ...x. ..x.."
  "..... ..x.. ..... ..x.. x..x. ..... ..... ..x.. ..x.. ....."
  }
}, 

{ 
  {'+','-','.','/','0','1','2','3','4'},
  { 
  "..... ..... ..... ..... ....x ..... ..... ..... ..... ....."
  "..x.. ..... ..... ..... ....x .xxx. ..x.. .xxx. xxxx. ...x."
  "..x.. ..... ..... ..... ....x x...x .xx.. x...x ....x ..xx."
  "xxxxx ..... xxxxx ..... ...x. x.x.x x.x.. ...x. xxxx. .x.x."
  "..x.. ..... ..... ..... ..x.. x...x ..x.. ..x.. ....x xxxxx"
  "..x.. ..... ..... ..... .x... .xxx. xxxxx xxxxx xxxx. ...x."
  "..... ...x. ..... ..x.. x.... ..... ..... ..... ..... ....."
  "..... ..x.. ..... ..x.. x.... ..... ..... ..... ..... ....."
  }
},


{
  { 0, 0, 0, 0, 0 , 0 , 0, 0, 0 },
  { "" } 
}

} ;

int arr[77] =  {
  0x00000,
  0x00100,
  0x00100,
  0x11111,
  0x00100,
  0x00100,
  0x00000,
  0x00000
};

/*
  
        glyphs %w{5 6 7 8 9 : ; < = >},
        "
        ..... ..... ..... ..... ..... ..... ..... ..... ..... .....
        xxxxx .xxxx xxxxx .xxx. .xxx. ..x.. ...x. ...x. ..... .x...
        x.... x.... ...x. x...x x...x ..x.. ...x. ..x.. xxxxx ..x..
        xxxx. xxxx. xxxxx .xxx. .xxxx ..... ..... .x... ..... ...x.
        ....x x...x .x... x...x ....x ..... ..... x.... ..... ....x
        xxxx. .xxx. .x... .xxx. xxxx. ..... ..... .x... xxxxx ...x.
        ..... ..... ..... ..... ..... ..x.. ...x. ..x.. ..... ..x..
        ..... ..... ..... ..... ..... ..x.. ..x.. ...x. ..... .x...
        "
        
        glyphs %w{? @}, 
         "
        ..... ..... ..... ..... ..... ..... ..... ..... ..... .....
        .xxx. .xxx. ..... ..... ..... ..... ..... ..... ..... .....
        x...x x..xx ..... ..... ..... ..... ..... ..... ..... .....
        ...x. x.x.x ..... ..... ..... ..... ..... ..... ..... .....
        ..x.. x.x.x ..... ..... ..... ..... ..... ..... ..... .....
        ..... x..xx ..... ..... ..... ..... ..... ..... ..... .....
        ..x.. x.... ..... ..... ..... ..... ..... ..... ..... .....
        ..... .xxxx ..... ..... ..... ..... ..... ..... ..... .....
        "


       glyphs %w{A B C D E F G H I J}, 
        "
        ..... ..... ..... ..... ..... ..... ..... ..... ..... .....
        ..x.. xxxx. .xxx. xxxx. xxxxx xxxxx .xxx. x...x xxxxx xxxxx
        .x,x. x...x x...x x...x x.... x.... x.... x...x ..x.. ....x
        x,..x xxxx. x.... x...x xxxx. xxx.. x..xx xxxxx ..x.. ....x
        xxxxx x...x x...x x...x x.... x.... x...x x...x ..x.. x...x
        x,..x xxxx. .xxx. xxxx. xxxxx x.... .xxx. x...x xxxxx .xxx.
        ..... ..... ..... ..... ..... ..... ..... ..... ..... .....
        ..... ..... ..... ..... ..... ..... ..... ..... ..... .....
        "
       glyphs %w{K L M N O P Q R S T}, 
        "
        ..... ..... ..... ..... ..... ..... ..... ..... ..... .....
        x...x x.... x...x x...x .xxx. xxxx. .xxx. xxxx. .xxxx xxxxx
        x..x. x.... xx.xx xx..x x...x x...x x...x x...x x.... ..x..
        xxx.. x.... x.x.x x.x.x x...x xxxx. x.,.x xxxx. .xxx. ..x..
        x..x. x.... x...x x..xx x...x x.... x..x, x...x ....x ..x..
        x...x xxxxx x...x x...x .xxx. x.... .xx,x x...x xxxx. ..x..
        ..... ..... ..... ..... ..... ..... ..... ..... ..... .....
        ..... ..... ..... ..... ..... ..... ..... ..... ..... .....
        "
       glyphs %w{U V W X Y Z [ \\ ] ^}, 
        "
        ..... ..... ..... ..... ..... ..... xxxx. x.... .xxxx ..x..
        x...x x...x x...x x...x x...x xxxxx x.... x.... ....x .x.x.
        x...x x...x x...x .x.x. .x.x. ...x. x.... .x... ....x x...x
        x...x x...x x...x ..x.. ..x.. ..x.. x.... ..x.. ....x .....
        x...x .x.x. x.x.x .x.x. ..x.. .x... x.... ...x. ....x .....
        .xxx. ..x.. .x.x. x...x ..x.. xxxxx x.... ....x ....x .....
        ..... ..... ..... ..... ..... ..... x.... ....x ....x .....
        ..... ..... ..... ..... ..... ..... xxxx. ....x .xxxx .....
        "
        
        glyphs %w{a b c d e f g h i j}, 
        "
        ..... ..... ..... ..... ..... ..... ..... ..... ..... .....
        ..... x.... ..... ....x ..... ...xx ..... x.... ..x.. ...x.
        .xxxx xxxx. .xxxx .xxxx .xxx. ..x.. .xxxx xxxx. ..... .....
        x...x x...x x.... x...x x..xx ..x.. x...x x...x ..x.. ...x.
        x...x x...x x.... x...x x,x.. xxxxx x...x x...x ..x.. ...x.
        .xxxx xxxx. .xxxx .xxxx .xxxx ..x.. .xxxx x...x ..x.. ...x.
        ..... ..... ..... ..... ..... ..x.. ....x ..... ..... x..x.
        ..... ..... ..... ..... ..... ..x.. xxxx. ..... ..... .xx..
        "
        glyphs %w{k l m n o p q r s t}, 
        "
        ..... ..... ..... ..... ..... ..... ..... ..... ..... .....
        x.... ..x.. ..... ..... ..... ..... ..... ..... ..... .x...
        x..x. ..x.. xx.x. xxxx. .xxx. xxxx. .xxxx xxxx. .xxx. xxxxx
        xxx.. ..x.. x.x.x x...x x...x x...x x...x x...x xx... .x...
        x..x. ..x.. x.x.x x...x x...x x...x x...x x.... ..xxx .x..x
        x...x ..x.. x.x.x x...x .xxx. xxxx. .xxxx x.... xxxx. ..xx.
        ..... ...x. ..... ..... ..... x.... ....x ..... ..... .....
        ..... ..... ..... ..... ..... x.... ....x ..... ..... .....
        "
        
        glyphs %w{u v w x y z { | } ~}, 
        "
        ..... ..... ..... ..... ..... ..... ...xx ..x.. xx... .....
        ..... ..... ..... ..... ..... ..... ..x.. ..x.. ..x.. .....
        x...x x...x x...x x..x. x...x xxxxx ..x.. ..x.. ..x.. .....
        x...x x...x x.x.x .xx.. x...x ...x. xx... ..x.. ...xx .x.x.
        x...x .x.x. x.x.x .xx.. x...x ..x.. xx... ..x.. ...xx x.x..
        .xxxx ..x.. .x.x. x..x. .xxxx xxxxx ..x.. ..x.. ..x.. .....
        ..... ..... ..... ..... ....x ..... ..x.. ..x.. ..x.. .....
        ..... ..... ..... ..... xxxx. ..... ...xx ..x.. xx... .....
        "
        
        # Some utf8 encoded accented letters
        glyphs %w{ä ë ï ö ü ÿ Ä Ë Ï Ö Ü Ÿ}, 
        "
        .x.x. .x.x. .x.x. .x.x. .x.x. .x.x. .x.x. .x.x. .x.x. .x.x. .x.x. .x.x.
        ..... ..... ..... ..... ..... ..... ..x.. xxxxx xxxxx .xxx. x...x x...x
        .xxxx .xxx. ..... .xxx. x...x x...x .x.x. x.... ..x.. x...x x...x .x.x.
        x...x x..xx ..x.. x...x x...x x...x x...x xxxxx ..x.. x...x x...x ..x..
        x...x xxx.. ..x.. x...x x...x x...x xxxxx x.... ..x.. x...x x...x ..x..
        .xxxx .xxxx ..x.. .xxx. .xxxx .xxxx x...x xxxxx xxxxx .xxx. .xxx. ..x..
        ..... ..... ..... ..... ..... ....x ..... ..... ..... ..... ..... .....
        ..... ..... ..... ..... ..... .xxx. ..... ..... ..... ..... ..... .....
        "
        
        glyphs %w{á é í ó ú ý Á É Í Ó Ú Ý}, 
        "
        ...x. ...x. ...x. ...x. ...x. ...x. ...x. ...x. ...x. ...x. ...x. ...x.
        ..x.. ..x.. ..x.. ..x.. ..x.. ..x.. ..x.. xxxxx xxxxx .xxx. x.x.x x.x.x
        .xxxx .xxx. ..... .xxx. x...x x...x .x.x. x.... ..x.. x...x x...x .x.x.
        x...x x..xx ..x.. x...x x...x x...x x...x xxxxx ..x.. x...x x...x ..x..
        x...x xxx.. ..x.. x...x x...x x...x xxxxx x.... ..x.. x...x x...x ..x..
        .xxxx .xxxx ..x.. .xxx. .xxxx .xxxx x...x xxxxx xxxxx .xxx. .xxx. ..x..
        ..... ..... ..... ..... ..... ....x ..... ..... ..... ..... ..... .....
        ..... ..... ..... ..... ..... .xxx. ..... ..... ..... ..... ..... .....
        "
        
        glyphs %w{à è ì ò ù ỳ À È Ò Ù Ỳ}, 
        "
        .x... .x... .x... .x... .x... .x... .x... .x... .x... .x... .x... .x...
        ..x.. ..x.. ..x.. ..x.. ..x.. ..x.. ..x.. xxxxx xxxxx .xxx. x.x.x x.x.x
        .xxxx .xxx. ..... .xxx. x...x x...x .x.x. x.... ..x.. x...x x...x .x.x.
        x...x x..xx ..x.. x...x x...x x...x x...x xxxxx ..x.. x...x x...x ..x..
        x...x xxx.. ..x.. x...x x...x x...x xxxxx x.... ..x.. x...x x...x ..x..
        .xxxx .xxxx ..x.. .xxx. .xxxx .xxxx x...x xxxxx xxxxx .xxx. .xxx. ..x..
        ..... ..... ..... ..... ..... ....x ..... ..... ..... ..... ..... .....
        ..... ..... ..... ..... ..... .xxx. ..... ..... ..... ..... ..... .....
        "

        glyphs %w{â ê î ô û ŷ Â Ê Î Ô Û Ŷ}, 
        "
        ..x.. ..x.. ..x.. ..x.. ..x.. ..x.. ..x.. ..x.. ..x.. ..x.. ..x.. ..x..
        .x.x. .x.x. .x.x. .x.x. .x.x. .x.x. .x.x. xxxxx xxxxx .xxx. xx.xx xx.xx
        .xxxx .xxx. ..... .xxx. x...x x...x .xxx. x.... ..x.. x...x x...x .x.x.
        x...x x..xx ..x.. x...x x...x x...x x...x xxxxx ..x.. x...x x...x ..x..
        x...x xxx.. ..x.. x...x x...x x...x xxxxx x.... ..x.. x...x x...x ..x..
        .xxxx .xxxx ..x.. .xxx. .xxxx .xxxx x...x xxxxx xxxxx .xxx. .xxx. ..x..
        ..... ..... ..... ..... ..... ....x ..... ..... ..... ..... ..... .....
        ..... ..... ..... ..... ..... .xxx. ..... ..... ..... ..... ..... .....
        "
        
        # Some private use characters useful for games, etc.
        glyphs ["\uEF00", "\uEF01", "\uEF02", "\uEF03", "\uEF04", "\uEF05",
                "\uEF06", "\uEF07", "\uEF08", "\uEF09",],
        "
        x.... xxxxx .xx.. ..x.. ..x.. ...x. ..x.. ..x.. .xxx. ..x..
        .x... xxxxx .x.x. ..x.. x.x.x ..x.. .xxx. x.x.x xxxxx ..x..
        ..x.x ..x.. .x..x .xxx. .x.x. .x... xxxxx .xxx. x.x.x .xxx.
        ...x. ..x.. xxxxx .xxx. .x.x. xxxxx xxxxx xx.xx xx.xx x.x.x
        ..x.x ..x.. .x..x xxxxx x.x.x ...x. .xxx. .xxx. .xxx. .xxx.
        ..... ..x.. .x.x. xxxxx .xxx. ..x.. ..x.. x.x.x .x.x. ..x..
        ..... ..x.. .xx.. .xxx. ..x.. .x... .xxx. ..x.. ..... ..x..
        "
        
        
*/

typedef struct GariNofont_ GariNofont;

struct GariNofont_ {
  GariFontStyle style;
  GariFontMode  mode;
  int           ptsize;
  int           scale;
}; 

/** Gets the drawing mode of the font. */
int gari_nofont_mode(GariNofont * self) {
  if(!self) return -1;
  return self->mode;
}


/** Sets the drawing mode of the font. */
GariNofont * gari_nofont_mode_(GariNofont * self, int mode) {
  if (!self) return NULL;
  self->mode = mode; 
  return self;
}

/** Returns the style of the font.  */
int gari_nofont_style(GariNofont * self) {
  if(!self) return -1;
  return self->style;
}

/** Sets the style effect of the font. */
GariNofont * gari_nofont_style_(GariNofont * self, int style) {
  if (!self) return NULL;
  self->style = style; 
  return self;
}


/** Loads a built in font. scale is the scale factor. */
GariNofont * gari_nofont_load(int scale) {
  GariNofont * self = GARI_MALLOC(sizeof(GariNofont));
  if(!self) return NULL;
  self->scale  = scale;
  self->ptsize = NOFONT_GLYPHINFO_ROWS * scale;
  gari_nofont_mode_(self, GariFontSolid);
  return gari_nofont_style_(self, GariFontNormal);
}

/** Frees the memory allocated by the font. */
GariFont * gari_nofont_free(GariNofont *font) {
  GARI_FREE(font);
  return NULL;
}

/** Renders the font to a surface, depending on the font's settings. */
GariImage * gari_nofont_render(GariNofont * font, char * utf8, 
    GariColor fgrgba, GariColor bgrgba);

/** Draws font with given colors. */
void gari_nofont_drawcolor(GariImage * image, int x, int y, char * utf8, 
                         GariNofont  * font , GariColor fg, GariColor bg); 

/** Draws font with given color components. */
void gari_nofont_draw(GariImage * image, int x, int y, char * utf8, GariNofont *
font, uint8_t fg_r, uint8_t fg_g, uint8_t fg_b, uint8_t bg_r, uint8_t bg_b,
uint8_t bg_a);

/** Draws font in printf style. Won't work on platforms that lack vsnprintf.
* Will draw up to 2000 bytes of characters.  
*/
void gari_nofont_printf(GariImage * image, int x, int y, GariNofont * font,
GariColor fg, GariColor bg, char * format, ...);

/** Returns a text with details about the last error in loading or 
handling a font. */
char * gari_nofont_error();

/** Returns the width that the given UTF-8 encoded text would be if it was
rendered using gari_fonr_render. */
int gari_nofont_renderwidth(GariNofont * font, char * utf8); 

/** Returns the font's max height */
int gari_nofont_height(GariNofont * font); 

/** Returns the font's font max ascent (y above origin)*/
int gari_nofont_ascent(GariNofont * font); 

/** Returns the font's min descent (y below origin)*/
int gari_nofont_descent(GariNofont * font);

/** Returns the font's recommended line spacing. */
int gari_nofont_lineskip(GariNofont * font);

int gari_fontstyle_tottf(int style);

int gari_fontstyle_fromttf(int style); 


