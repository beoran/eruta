#include "gy.h"

#define GY_INTERN_ONLY
#include "gy_intern.h"




const GyEvent GY_EMPTY_EVENT = {GY_EVENT_NONE, 0, 0, 0, 0, 0, 0, 0, 0};


/** Helps converting mouse button events. */
GyEvent gyevent_mousebutton(SDL_Event ev, uint8_t kind) {  
  GyEvent res = GY_EMPTY_EVENT;
  res.kind      = kind;
  res.x         = ev.button.x;
  res.y         = ev.button.y;
  res.button    = ev.button.button;
  res.which     = ev.button.which;
  // Translate scrolls to proper scroll events
  // In SDL 1.3, they will also be separate events.
  if (ev.button.button == SDL_BUTTON_WHEELUP) {
      res.kind  = GY_EVENT_MOUSESCROLL;
      res.value = -1;
  } else if (ev.button.button == SDL_BUTTON_WHEELDOWN) {
      res.kind  = GY_EVENT_MOUSESCROLL;
      res.value = 1;
  }
  return res;
}

GyEvent gyevent_fromsdl(SDL_Event ev) {
  GyEvent res = GY_EMPTY_EVENT;
  switch (ev.type) {
    case SDL_ACTIVEEVENT:
       res.kind = GY_EVENT_ACTIVE;
       res.gain = ev.active.gain;
       break;
    case SDL_KEYDOWN:
      res.kind    = GY_EVENT_KEYDOWN;
      res.key     = ev.key.keysym.sym;
      res.mod     = ev.key.keysym.mod;
      res.unicode = ( (ev.key.keysym.unicode & 0xFF80) ? 
                      ev.key.keysym.unicode : ev.key.keysym.unicode & 0x7F );
      break;
      
    case SDL_KEYUP:   
      res.kind    = GY_EVENT_KEYUP;
      res.key     = ev.key.keysym.sym;
      res.mod     = ev.key.keysym.mod;
      break;
      
    case SDL_MOUSEMOTION:
      res.kind    = GY_EVENT_MOUSEMOVE;
      res.x       = ev.motion.x;
      res.y       = ev.motion.y;      
      res.xrel    = ev.motion.xrel;
      res.yrel    = ev.motion.yrel;
      res.button  = ev.motion.state;
      res.which   = ev.motion.which;
      break;
      
    case SDL_MOUSEBUTTONDOWN:
      res         = gyevent_mousebutton(ev, GY_EVENT_MOUSEPRESS);
      break;
      
    case SDL_MOUSEBUTTONUP:
      res         = gyevent_mousebutton(ev, GY_EVENT_MOUSERELEASE);
      break;
      
    case SDL_JOYBUTTONDOWN:
      res.kind    = GY_EVENT_JOYPRESS;
      res.which   = ev.jbutton.which;
      res.button  = ev.jbutton.button;      
      break;
   
    case SDL_JOYBUTTONUP:
      res.kind    = GY_EVENT_JOYRELEASE;
      res.which   = ev.jbutton.which;
      res.button  = ev.jbutton.button;      
      break;
      
    case SDL_JOYAXISMOTION:
      res.kind    = GY_EVENT_JOYMOVE;
      res.which   = ev.jaxis.which;
      res.sub     = ev.jaxis.axis;
      res.value   = ev.jaxis.value;
      break;
      
    case SDL_JOYBALLMOTION:
      res.kind    = GY_EVENT_JOYBALL;
      res.which   = ev.jball.which;
      res.sub     = ev.jball.ball;
      res.xrel    = ev.jball.xrel;
      res.yrel    = ev.jball.yrel;
      break;

    case SDL_JOYHATMOTION:
      res.kind    = GY_EVENT_JOYHAT;
      res.which   = ev.jhat.which;
      res.sub     = ev.jhat.hat;
      res.value   = ev.jhat.value;
      break;

      
    case SDL_VIDEORESIZE:
      res.kind    = GY_EVENT_RESIZE;
      res.w       = ev.resize.w;
      res.h       = ev.resize.h;
      break;
   
    case SDL_VIDEOEXPOSE:
      res.kind    = GY_EVENT_EXPOSE;
      break;
     
    case SDL_QUIT:
      res.kind    = GY_EVENT_QUIT;
      break;
  }
  return res;
}



/** 
* Gets an event from the event queue. Returns a special event with 
* event.kind == GY_EVENT_NONE if no events are on the queue. 
*/
GyEvent gyevent_poll() {
  SDL_Event      event;
  if (SDL_PollEvent(&event)) {
    return gyevent_fromsdl(event);
  } else {
    GyEvent result = GY_EMPTY_EVENT; 
    return result;  
  } 
}

/** Polls the event queue and copies it to the given event  addres, 
* wich must be a valid address and not be null.
* Return NULL if the eevnt queue was ampty, non-null if there was an event fetched. 
*/
GyEvent * gyevent_fetch(GyEvent * event) {
  SDL_Event      sdlevent;
  if (SDL_PollEvent(&sdlevent)) {
    (*event) = gyevent_fromsdl(sdlevent);
    return event;
  } else {
    (*event)  = GY_EMPTY_EVENT; 
    return NULL;  
  } 
}

/** Polls the event queue and returns a newly alloctaded GyEvent with 
* the top event of the queue. Returns NULL, and allocates no memory if 
* the queue is empty.     
*/
GyEvent * gyevent_pollnew() {  
  SDL_Event      sdlevent;
  if (SDL_PollEvent(&sdlevent)) {
    GyEvent    * event = GY_ALLOCATE(GyEvent);
    (*event) = gyevent_fromsdl(sdlevent);
    return event;
  } else {  
    return NULL;  
  } 
}

void gyevent_free(GyEvent * event) {
  GY_FREE(event);
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



struct GyEvent_ {
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





