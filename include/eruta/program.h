/** Program models the main eruta program with all it's modes. */
#ifndef PROGRAM_H
#define PROGRAM_H

#include "eruta.h"
#include "mode.h"


Program * program_alloc();
Program * program_init(Program * self, int screen_w, int screen_h, int full);
Program * program_new(int screen_w, int screen_h, int full);
Program * program_free(Program * self);
    
int program_mode_add(Program * self, Mode * mode);
 
int program_mode_add(Program * self, Mode * mode);

int program_mode_set(Program * self, ProgramMode modeid);

/** End the program gracefully. */
int program_quit(Program * self);

/** Runs the program, switching between active modes
A mode must have been installed and selected before calling this */
int program_run(Program * self);

/** Program loop, called once for every redraw. */
int program_run_once(Program * self);

GariGame * program_game(Program * self);
GariScreen * program_screen(Program * self);
void * program_ui(Program * self);
GariFont * program_font(Program * self);



#endif
