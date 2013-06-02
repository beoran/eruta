#ifndef ERUTA_H
#define ERUTA_H

/* Commonly used includes. */
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdarg.h>
#include <chipmunk.h>
#include <allegro5/allegro.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_audio.h>
#include <allegro5/allegro_acodec.h>
#include "bevec.h"

/* Nogfx mode displays merely boxes. */
// #define ERUTA_NOGFX_MODE

/* Screen size wich will always be 640x480 (real or virtually). */
#define SCREEN_W      640
#define SCREEN_H      480

/* Bitwise operations helpers. */
#define BIT_ISFLAG(NUM, FLAG)     ((NUM) & (FLAG))
#define BIT_ISBIT(NUM, BIT)       BIT_ISFLAG(NUM, (1 << BIT))
#define BIT_SETFLAG(NUM, FLAG)    ((NUM) | (FLAG))
#define BIT_UNFLAG(NUM, FLAG)     ((NUM) & (~(FLAG)))
#define BIT_SETBIT(NUM, BIT)      BIT_SETFLAG(NUM, (1 << BIT))
#define BIT_UNBIT(NUM, BIT)       BIT_UNFLAG(NUM, (1 << BIT))

/* Clamping */
#define CLAMP_MAX(SET, VALUE, MAX) do {                     \
  int value___ = (VALUE), max___ = (MAX);                   \
  (SET) =  ((value___ > max___ ) ? (max___) : (value___));  \
} while(0) 

/* */


#ifndef TRUE
#define TRUE (!0)
#endif

#ifndef FALSE
#define FALSE (0)
#endif

/* Boolean type*/
#ifndef BOOL
#define BOOL int
#endif

/* String type */
#ifndef STR
#define STR ALLEGRO_USTR
#endif

/* Use the Bump vector as the Point type.
Allegro also uses floats now for it's drawing. */
typedef BeVec Point;

/** Other shorthand types */
typedef ALLEGRO_PATH Path;
typedef ALLEGRO_EVENT Event;

/* Wrapper macros */
#define point(X, Y) bevec(X, Y);

/* Some univerally useful function declarations. */
/* Helper to convert paths to C strings */
const char * fifi_path_cstr(Path * path);
/* Helper for al_filename_exists with a Fifi path. */
int fifi_path_exists(Path * path);

/* Shorthand macros. */
#define PATH_CSTR(PATH)   (fifi_path_cstr(PATH))
#define PATH_EXISTS(PATH) (fifi_path_exists(PATH))

/*
These macros can be used in the body of a function with ... args to
map it to a functions with va_arg arguments as last. Use them like this:
blah foo_va(bar last, va_args args);

blah foo(bar last, ...) {
  VA_MAP_START(blah, last);
  VA_MAP_RESULT(foo_va(blag, VA_MAP_ARGS));
  VA_MAP_END();
}
*/
#define VA_MAP_START(TYPE, LAST)  \
  va_list args___;                \
  TYPE result___;                 \
  va_start(args___, LAST)

#define VA_MAP_ARGS args___

#define VA_MAP_RESULT(VALUE)  do { \
  result___ = (VALUE);             \
  } while (0)
  
#define VA_MAP_END() \
  va_end(args___);   \
  return result___



/* Commonly used types and typedefs. */

struct Program_;
typedef struct Program_ Program;


enum ProgramMode_ {
  // Negatieve modes are special.
  ProgramModeQuit  = -2, 
  ProgramModeNone  = -1,
  ProgramModeIntro = 0,
  ProgramModeStart = 1,
  ProgramModePlay  = 2,
  ProgramModeMenu  = 3,
  ProgramModeEdit  = 4,
  ProgramModeRes1  = 5,
  ProgramModeRes2  = 6,
  ProgramModeRes3  = 7,
  ProgramModeLast  = 8,
};

typedef enum ProgramMode_ ProgramMode;

/* Error or RESult.  Zero if OK, negative if an error occurred. */
enum Eres_ {
  ERES_OK       =  0,
  ERES_NULL     = -1,
  ERES_NOMEM    = -2,
  ERES_NOFILE   = -3,
};

typedef int ERES;




#endif
