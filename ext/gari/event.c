#include "gari.h"

#define GARI_INTERN_ONLY
#include "gari_intern.h"




const GariEvent GARI_EMPTY_EVENT = {GARI_EVENT_NONE, 0, 0, 0, 0, 0, 0, 0, 0};


/** Helps converting mouse button events. */
GariEvent gari_event_mousebutton(SDL_Event ev, uint8_t kind) {  
  GariEvent res = GARI_EMPTY_EVENT;
  res.kind      = kind;
  res.x         = ev.button.x;
  res.y         = ev.button.y;
  res.button    = ev.button.button;
  res.which     = ev.button.which;
  // Translate scrolls to proper scroll events
  // In SDL 1.3, they will also be separate events.
  if (ev.button.button == SDL_BUTTON_WHEELUP) {
      res.kind  = GARI_EVENT_MOUSESCROLL;
      res.value = -1;
  } else if (ev.button.button == SDL_BUTTON_WHEELDOWN) {
      res.kind  = GARI_EVENT_MOUSESCROLL;
      res.value = 1;
  }
  return res;
}

GariEvent gari_event_fromsdl(SDL_Event ev) {
  GariEvent res = GARI_EMPTY_EVENT;
  switch (ev.type) {
    case SDL_ACTIVEEVENT:
       res.kind = GARI_EVENT_ACTIVE;
       res.gain = ev.active.gain;
       break;
    case SDL_KEYDOWN:
      res.kind    = GARI_EVENT_KEYDOWN;
      res.key     = ev.key.keysym.sym;
      res.mod     = ev.key.keysym.mod;
      res.unicode = ( (ev.key.keysym.unicode & 0xFF80) ? 
                      ev.key.keysym.unicode : ev.key.keysym.unicode & 0x7F );
      break;
      
    case SDL_KEYUP:   
      res.kind    = GARI_EVENT_KEYUP;
      res.key     = ev.key.keysym.sym;
      res.mod     = ev.key.keysym.mod;
      break;
      
   case SDL_MOUSEMOTION:
      res.kind    = GARI_EVENT_MOUSEMOVE;
      res.x       = ev.motion.x;
      res.y       = ev.motion.y;      
      res.xrel    = ev.motion.xrel;
      res.yrel    = ev.motion.yrel;
      res.button  = ev.motion.state;
      res.which   = ev.motion.which;
      break;
      
   case SDL_MOUSEBUTTONDOWN:
      res         = gari_event_mousebutton(ev, GARI_EVENT_MOUSEPRESS);
      break;
      
   case SDL_MOUSEBUTTONUP:
      res         = gari_event_mousebutton(ev, GARI_EVENT_MOUSERELEASE);
      break;
      
   case SDL_JOYBUTTONDOWN:
      res.kind    = GARI_EVENT_JOYPRESS;
      res.which   = ev.jbutton.which;
      res.button  = ev.jbutton.button;      
      break;
   
   case SDL_JOYBUTTONUP:
      res.kind    = GARI_EVENT_JOYRELEASE;
      res.which   = ev.jbutton.which;
      res.button  = ev.jbutton.button;      
      break;
      
   case SDL_JOYAXISMOTION:
      res.kind    = GARI_EVENT_JOYMOVE;
      res.which   = ev.jaxis.which;
      res.axis    = ev.jaxis.axis;
      res.value   = ev.jaxis.value;
      break;
      
   case SDL_VIDEORESIZE:
     res.kind    = GARI_EVENT_RESIZE;
     res.w       = ev.resize.w;
     res.h       = ev.resize.h;
     break;
   
   case SDL_VIDEOEXPOSE:
     res.kind    = GARI_EVENT_EXPOSE;
     break;
     
   case SDL_QUIT:
     res.kind    = GARI_EVENT_QUIT;
     break;
  }
  return res;
}



/** 
* Gets an event from the event queue. Returns a special event with 
* event.kind == GARI_EVENT_NONE if no events are on the queue. 
*/
GariEvent gari_event_poll() {
  SDL_Event      event;
  if (SDL_PollEvent(&event)) {
    return gari_event_fromsdl(event);
  } else {
    GariEvent result = GARI_EMPTY_EVENT; 
    return result;  
  } 
}

/** Polls the event queue and copies it to the given event  addres, 
* wich must be a valid address and not be null.
* Return NULL if the eevnt queue was ampty, non-null if there was an event fetched. 
*/
GariEvent * gari_event_fetch(GariEvent * event) {
  SDL_Event      sdlevent;
  if (SDL_PollEvent(&sdlevent)) {
    (*event) = gari_event_fromsdl(sdlevent);
    return event;
  } else {
    (*event)  = GARI_EMPTY_EVENT; 
    return NULL;  
  } 
}

/** Polls the event queue and returns a newly alloctaded GariEvent with 
* the top event of the queue. Returns NULL, and allocates no memory if 
* the queue is empty.     
*/
GariEvent * gari_event_pollnew() {  
  SDL_Event      sdlevent;
  if (SDL_PollEvent(&sdlevent)) {
    GariEvent    * event = GARI_ALLOCATE(GariEvent);
    (*event) = gari_event_fromsdl(sdlevent);
    return event;
  } else {  
    return NULL;  
  } 
}

void gari_event_free(GariEvent * event) {
  GARI_FREE(event);
}



/*


SDL_MOUSEMOTION
  

SDL_MouseMotionEvent

SDL_MOUSEBUTTONDOWN/UP
  

SDL_MouseButtonEvent

SDL_JOYAXISMOTION
  

SDL_JoyAxisEvent

SDL_JOYBALLMOTION
  

SDL_JoyBallEvent

SDL_JOYHATMOTION
  

SDL_JoyHatEvent

SDL_JOYBUTTONDOWN/UP
  

SDL_JoyButtonEvent

SDL_VIDEORESIZE
  

SDL_ResizeEvent

SDL_VIDEOEXPOSE
  

SDL_ExposeEvent

SDL_QUIT
  

SDL_QuitEvent

SDL_USEREVENT
  

SDL_UserEvent

SDL_SYSWMEVENT
  

SDL_SysWMEvent
typedef union{
  Uint8 type;
  SDL_ActiveEvent active;
  SDL_KeyboardEvent key;
  SDL_MouseMotionEvent motion;
  SDL_MouseButtonEvent button;
  SDL_JoyAxisEvent jaxis;
  SDL_JoyBallEvent jball;
  SDL_JoyHatEvent jhat;
  SDL_JoyButtonEvent jbutton;
  SDL_ResizeEvent resize;
  SDL_ExposeEvent expose;
  SDL_QuitEvent quit;
  SDL_UserEvent user;
  SDL_SysWMEvent syswm;
} SDL_Event;



struct GariEvent_ {
  uint8_t   kind;
  uint8_t   active;
  uint16_t  key;
  uint8_t   keysym;
  char *    text;
  uint16_t  x, y;
  
  

Activation event (see SDL_ActiveEvent)

key
  

Keyboard event (see SDL_KeyboardEvent)

motion
  

Mouse motion event (see SDL_MouseMotionEvent)

button
  

Mouse button event (see SDL_MouseButtonEvent)

jaxis
  

Joystick axis motion event (see SDL_JoyAxisEvent)

jball
  

Joystick trackball motion event (see SDL_JoyBallEvent)

jhat
  

Joystick hat motion event (see SDL_JoyHatEvent)

jbutton
  

Joystick button event (see SDL_JoyButtonEvent)

resize
  

Application window resize event (see SDL_ResizeEvent)

expose
  

Application window expose event (see SDL_ExposeEvent)

quit
  

Application quit request event (see SDL_QuitEvent)

user
  

User defined event (see SDL_UserEvent)

syswm
  

Undefined window manager event (see SDL_SysWMEvent)
  
};



*/





