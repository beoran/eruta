#include <stdio.h>
#include "program.h"
#include "si_macro.h"
#include "si_mem.h"


#define SI_PATH_SEP 

#define PROGRAM_MODES ProgramModeLast

struct Program_ {
  Mode       *  mode                ; // Active mode
  GariScreen *  screen              ; // Shortcut to screen
  GariGame   *  game                ; // Game info and frames drawn 
  GariFont   *  font                ; // Default font
  void *        ui                  ; // Shortcut to user interface

  ProgramMode   modeid              ; // ID of active mode
  int           busy                ; // Still busy?
  Mode       *  modes[PROGRAM_MODES]; // Modes
};


Program * program_alloc() {
  return SI_ALLOCATE(Program); 
}



Program * program_new(int screen_w, int screen_h, int full) {
  return program_init(program_alloc(), screen_w, screen_h, full);
}

// Automatically frees any contained modes, closes the screen.
Program * program_free(Program * self) {
  int index; 
  for (index = 0; index < PROGRAM_MODES; index ++) { 
    mode_free(self->modes[index]);
  }
  gari_font_free(self->font); // Free font
  gari_game_free(self->game); // Free game, and also screen.
  return si_free(self);
}


#define PROGRAM_FONTPATH "data/font/liberationserif.ttf"
#define PROGRAM_FONTSIZE 8
  
Program * program_init(Program * self, int screen_w, int screen_h, int full) {  
  int index = 0;
  if(!self) return NULL;
  self->game    = gari_game_make(); 
  self->screen  = gari_game_openscreen(self->game, screen_w, screen_h, full);
  self->ui      = NULL;  //Zori.open(@game, @queue)
  // fontname      = File.join(*%w{data font liberationserif.ttf})
  self->font    = gari_font_load(PROGRAM_FONTPATH, PROGRAM_FONTSIZE);
  if (!self->font) {
    perror("Could not load font");
    return NULL; 
  } 
  // clear modes
  for (index = 0; index < PROGRAM_MODES; index ++) { 
    self->modes[index] = NULL;  
  }
  self->mode    = NULL;
  self->modeid  = ProgramModeNone;
  self->busy    = TRUE;
  return self;
}


int program_mode_add(Program * self, Mode * mode) {
  int id;
  if((!self) || (!mode)) return -1;
  id              = mode_id(mode);
  self->modes[id] = mode;
  return id;
}  

Mode * program_mode(Program * self) {
  if(!self) return NULL; 
  return self->mode; 
}

ProgramMode program_modeid(Program * self) { 
  return self->modeid;
}

/** Sets the program's mode to the one corresponding to given mode ID. */
ProgramMode program_modeid_(Program * self, ProgramMode modeid) { 
  Mode * mode;
  if(program_modeid(self) == modeid) return self->modeid;
  // mode must be in range and be available
  if((modeid < 0) || (modeid >= PROGRAM_MODES)) return ProgramModeNone;
  mode          = self->modes[modeid];
  if(!mode) return ProgramModeNone;
  self->modeid  = modeid;
  self->mode    = mode;
  mode_start(self->mode);
  // Tell the mode it got started.
  return self->modeid;
}  

/** End the program gracefully. */
int program_quit(Program * self) {
  self->busy = FALSE;
  return self->busy;
}

/** returns true if the program is still busy. */
int program_busy(Program * self) {
  return (self->busy);
}


static int program_switchmode(Program * self, Mode * mode) {
  // Check if the mode is done. If it is, go to the next mode. 
  ProgramMode modeid    = mode_nextmode(mode);
  // No change requested, steady as you are. :)
  if(modeid == ProgramModeNone) return FALSE;
  // Quit if quit requested.
  if(modeid == ProgramModeQuit) return program_quit(self);
  // Change mode by mode ID. 
  return program_modeid_(self, modeid);
}

// Program loop, called once every redraw
int program_runonce(Program * self) { 
// Poll the events and send them.
// XXX :coordinate this with the UI?
  ProgramMode   modeid;
  Mode        * mode;
  GariEvent     event;
  mode        = program_mode(self);
  if(!mode)     return -1;
  event       = gari_event_poll();
  while (event.kind != GARI_EVENT_NONE) {
    // Let the mode handle events.
    mode_event(mode, event);
  } 
  // Let the active mode draw to the screen.
  mode_draw(mode, self->screen);  
  // Update game FPS and screen.
  gari_game_update(self->game);
  mode_update(mode);
  // Update mode, should also update program logic, whatever that may be.
  // Check if the mode is done. If it is, go to the next mode.
  return program_switchmode(self, mode);
}

/** Runs the program, switching between active modes. 
A mode must have been installed and selected befiore calling this. */
int program_run(Program * self) {   
  gari_game_startfps(self->game);
  while (program_busy(self)) { 
    program_runonce(self);
  }
  return 0;
}

// Readers for game, screen, ui and font
GariGame * program_game(Program * self) { 
  return self->game;
}

GariScreen * program_screen(Program * self) { 
  return self->screen;
}
 
void * program_ui(Program * self) { 
  return self->ui;
}

GariFont * program_font(Program * self) { 
  return self->font;
}

