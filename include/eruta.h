#ifndef ERUTA_H
#define ERUTA_H

/** Commnly used includes. */
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdarg.h>
#include <lua.h>
#include <lauxlib.h>
#include <chipmunk.h>
#include <allegro5/allegro.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_audio.h>
#include <allegro5/allegro_acodec.h>

/** Screen size wich will always be 640x480 (real or virtually). */
#define SCREEN_W      640
#define SCREEN_H      480

/** Bitwise operations helpers. */
#define BIT_ISFLAG(NUM, FLAG)     ((NUM) & (FLAG))
#define BIT_ISBIT(NUM, BIT)       BIT_ISFLAG(NUM, (1 << BIT))
#define BIT_SETFLAG(NUM, FLAG)    ((NUM) | (FLAG))
#define BIT_UNFLAG(NUM, FLAG)     ((NUM) & (~(FLAG)))
#define BIT_SETBIT(NUM, BIT)      BIT_SETFLAG(NUM, (1 << BIT))
#define BIT_UNBIT(NUM, BIT)       BIT_UNFLAG(NUM, (1 << BIT))

/** Clamping */
#define CLAMP_MAX(SET, VALUE, MAX) do {                     \
  int value___ = (VALUE), max___ = (MAX);                   \
  (SET) =  ((value___ > max___ ) ? (max___) : (value___));  \
} while(0) 

/** */


#ifndef TRUE
#define TRUE (!0)
#endif

#ifndef FALSE
#define FALSE (0)
#endif

/** Boolean type*/
#ifndef BOOL
#define BOOL int
#endif

/** String type */
#ifndef STR
#define STR ALLEGRO_USTR
#endif


/** Use the chipmunk vector as the Point type.
Allegro also uses floats now for it's drawing. */
typedef cpVect Point;

/** Wrapper macros */
#define point(X, Y) cpv(X, Y);

/** Shorthand macros. */
#define PATH_CSTR(PATH)    al_path_cstr(PATH, ALLEGRO_NATIVE_PATH_SEP)
#define PATH_EXISTS(PATH) (al_filename_exists(PATH_CSTR(PATH)))

/**
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



/** Commonly used types and typedefs. */

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





#endif
