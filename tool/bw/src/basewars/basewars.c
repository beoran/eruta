#include "basewars.h"




typedef void (GyRender)(GyMain * mode , GyCanvas * canvas);
typedef void (GyUpdate) (GyMain * mode, double dt);
typedef void (GyHandler)(GyMain * mode, GyEvent * event);

struct GyMode_ {
  GyRender    * render;
  GyUpdate    * update;
  GyHandler   * handlers[GY_EVENT_LAST];
};


/** Sets the render function of the mode. */
GyMode * gymode_render_(GyMode * self, GyRender * render) {
  self->render = render;
  return self;
}

/** Sets the update function of the mode. */
GyMode * gymode_update_(GyMode * self, GyUpdate * update) {
  self->update = update;
  return self;
}

/** Sets the event handler function to all events. */
GyMode * gymode_handlers_(GyMode * self, GyHandler * handler) {
  int index;
  for(index = 0; index < GY_EVENT_LAST ; index ++) { 
    self->handlers[index] = handler;
  }
  return self;
}

/** Sets an event handler function to one event at index. */
GyMode * gymode_handler_(GyMode * self, int index, GyHandler * handler) {
  if((index < 0) || (index >= GY_EVENT_LAST)) return NULL;
  self->handlers[index] = handler;
  return self;
}


/** Initializes an empty mode. */
GyMode * gymode_initempty(GyMode * self) {
  int index;
  gymode_render_(self, NULL);
  gymode_update_(self, NULL);
  return gymode_handlers_(self, NULL);
}

/** Copies other into self. */
GyMode * gymode_copy(GyMode *self, const GyMode * other) {
  int index;
  self->render  = other->render;
  self->update  = other->update;
  for(index = 0; index < GY_EVENT_LAST ; index ++) { 
    self->handlers[index] = other->handlers[index];
  }
  return self;
}




/** Returns the n-th joystick of the game. */
GyJoystick * gymain_joystick(GyMain * self, int index) {
  if (index >= gyjoy_amount()) return NULL;
  return self->joysticks[index];
}

/** Stops the main program. */
GyMain * gymain_done(GyMain * self) {
  if(self->lua) lua_close(self->lua);
  gyjoy_closeall(self->joysticks);  
  gy_quit();
  return self;
}

/** Initializes the main program. */
GyMain * gymain_init(GyMain * self, char * title, int w, int h, 
                     int fs, int sw) {
  gy_init_everything();
  self->window    = gywindow_new(title, w, h, fs, sw);
  if( !self->window) { return NULL; } 
  self->canvas    = gywindow_canvas(self->window);
  self->joysticks = gyjoy_openall();
  
  self->mode      = NULL;
  self->busy      = TRUE;
  self->lua       = luaL_newstate();
  if(!self->lua) {
    perror("Lua not opened!");
    gymain_done(self);
    return NULL;
  }
  lb_init(self->lua);
  return self;
}


/** Sets the active mode to be used by the game engine. */
GyMain * gymain_mode_(GyMain * self, GyMode * mode) {
  self->mode = mode;
  return self;
}


static GyMode   gamemode;
static GyMode * gamemode_ptr = NULL;

GyMode * gamemode_get() {
  if(gamemode_ptr) {
    return(gamemode_ptr);
  }
  gamemode_ptr = &gamemode;
  gymode_initempty(gamemode_ptr);
  return(gamemode_ptr);
}


static GyMode   mainmenumode;
static GyMode * mainmenumode_ptr = NULL;

GyMode * mainmenumode_get() {
  if(mainmenumode_ptr) {
    return(mainmenumode_ptr);
  }
  
  mainmenumode_ptr = &mainmenumode;
  gymode_initempty(gamemode_ptr);
  return(mainmenumode_ptr);
}


static GyMode   titlescreenmode;
static GyMode * titlescreenmode_ptr = NULL;

GyMode * titlescreenmode_get() {
  if(titlescreenmode_ptr) {
    return(titlescreenmode_ptr);
  }
  gymode_initempty(gamemode_ptr);
  titlescreenmode_ptr = &titlescreenmode;
  return(titlescreenmode_ptr);
}

GyMain * gymain_play(GyMain * self) {
  while(self->busy) {
    GyEvent ev;
    while (gyevent_fetch(&ev)) {
      GyHandler * handler;
      if(ev.kind == GY_EVENT_QUIT) self->busy = FALSE;
      if(!self->mode) continue;  
      handler = self->mode->handlers[ev.kind];
      if(!handler) continue;
      handler(self, &ev);
    }
    if(!self->mode) continue;
    if(self->mode->render) self->mode->render(self, self->canvas);
    if(self->mode->update) self->mode->update(self, 1);
  }
  return self;
}

static GyMain basewars_main;

/** Gets a pointer to the main structure that contains all data. */
GyMain * bamain_get() {
  return &basewars_main;
}


int main(int argc, char * argv[]) {
  GyMain mymain;
  gymain_init(bamain_get(), "Basewars", 640 , 480, FALSE, TRUE);
  if(!bamap_load(bamain_get()->lua, 1)) {
    puts("Could not load map 1");
    gymain_done(bamain_get());
    return 1;
  }
  
  bamap_save(bamap_getcurrent(), 1);
  bamap_draw(bamap_getcurrent(), bamain_get()->window, 0, 0);
  gywindow_update(bamain_get()->window);
  
  SDL_Delay(6000);
  gymain_done(bamain_get());
  return 0;
}

