#ifndef ERUTA_H
#define ERUTA_H

/** Commnly used includes. */
#include "gari.h"
#include "lua.h"
#include "lauxlib.h"
#include "chipmunk.h"




/** Commonly used types and typedefs. */

struct Program_;
typedef struct Program_ Program;

struct Mode_;
typedef struct Mode_ Mode;

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
