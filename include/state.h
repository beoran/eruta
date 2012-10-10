#ifndef ERUTA_STATE_H
#define ERUTA_STATE_H

#include "eruta.h"
#include "tilemap.h"
#include "mode.h"
#include "lh.h"
#include "rh.h"

#define STATE_COLORS   16
#define STATE_BLACK   0
#define STATE_WHITE   1
/** Amount of samples that can be played at the same time. */
#define STATE_SAMPLES 16

/** The data struct contains all global state and other data of the application.
*/
struct State_;
typedef struct State_ State;

#include "state_proto.h"


#endif
