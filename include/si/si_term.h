#ifndef SI_TERM_H
#define SI_TERM_H

#include "si_macro.h"

enum SiTermAttribute_ { 
  SiTermReset     = 0 ,    SiTermBold      = 1 ,    SiTermDark      = 2 ,
  SiTermUnderline = 4 ,    SiTermBlink     = 5 ,    SiTermNegative  = 7 ,
  SiTermConcealed = 8 ,    SiTermBlack     = 30,    SiTermRed       = 31,
  SiTermGreen     = 32,    SiTermYellow    = 33,    SiTermBlue      = 34,
  SiTermMagenta   = 35,    SiTermCyan      = 36,    SiTermWhite     = 37, 
  SiTermOnBlack   = 40,    SiTermOnRed     = 41,    SiTermOnGreen   = 42, 
  SiTermOnYellow  = 43,    SiTermOnBlue    = 44,    SiTermOnMagenta = 45, 
  SiTermOnCyan    = 46,    SiTermOnWhite   = 47,    SiTermLastAttr  = 48
};

typedef enum SiTermAttribute_ SiTermAttribute;


struct SiPointInt_ {
  int x;
  int y;
};

typedef struct SiPointInt_ SiTermPosition;

/** SiTerm Models the terminal and it's attributes */
struct SiTerm_;
typedef struct SiTerm_ SiTerm;


SI_API SiTerm * siterm_new();

SI_API SiTerm * siterm_free(SiTerm * term);
 
SI_API int siterm_clear(SiTerm * term);

SI_API int siterm_attribute(SiTerm * term, SiTermAttribute attr);

SI_API int siterm_attribute_(SiTerm * term, SiTermAttribute attr, int set);

SI_API int siterm_getchar(SiTerm * term, int x, int y);

SI_API int siterm_goto(SiTerm * term, int x, int y);

SI_API SiTermPosition siterm_screensize(SiTerm * term);

SI_API int siterm_rows(SiTerm * term);

SI_API int siterm_columns(SiTerm * term);

SI_API SiTermPosition siterm_at(SiTerm * term);

SI_API int siterm_cursor_(SiTerm * term, int visible);



#endif

