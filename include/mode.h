/** A mode is a specific mode in which the game can be.
* for example, menu mode, play mode, main screen, edit mode, etc. 
* In every mode, inputs and drawing may work in a different way.
*/
#ifndef MODE_H
#define MODE_H

#include "eruta.h"



/** Planned modes. */
enum ErutaModes_ {
  ERUTA_MODE_INTRO      = 1,
  ERUTA_MODE_MAINMENU   = 2,
  ERUTA_MODE_EXPLORE    = 3,
  ERUTA_MODE_BATTLE     = 4,
  ERUTA_MODE_DIALOG     = 5,
  ERUTA_MODE_MENU       = 6,
  ERUTA_MODE_LOAD       = 7,
  ERUTA_MODE_SAVE       = 8,
  ERUTA_MODE_NEW        = 9,
  ERUTA_MODE_TEST       = 10,
  ERUTA_MODE_MAX        = 16,
};

/**
* A Mode is a mode of the program in which the display and controls may 
* be different. Examples of this may be a menu mode, play mode,
* edit mode, intro mode, etc.
* 
*/
struct Mode_;
typedef struct Mode_ Mode;


/**
* ModeActs contains the actions , that is the function poMode *ers of a mode.
* These consist of start, enter, leave, done, event and paMode *
*/
struct ModeActs_;
typedef struct ModeActs_ ModeActs;


typedef Mode * (ModeActEvent)(Mode * self, ALLEGRO_EVENT   * event);
typedef Mode * (ModeActPaint)(Mode * self, ALLEGRO_DISPLAY * display);
typedef Mode * (ModeActMove)(Mode * self, Mode * other);
typedef Mode * (ModeActSelf)(Mode * self);

struct ModeActs_ {
  ModeActSelf   * start;
  ModeActMove   * enter;
  ModeActMove   * leave;
  ModeActPaint  * paint;
  ModeActEvent  * event;
  ModeActSelf   * stop;
};

#include "state.h"
#include "react.h"


#include "mode_proto.h"



#endif
