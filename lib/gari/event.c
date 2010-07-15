#include "gari.h"

#define GARI_INTERN_ONLY
#include "gari_intern.h"


#define GARI_EVENT_NONE           0
#define GARI_EVENT_ACTIVE         1
#define GARI_EVENT_KEYDOWN        2
#define GARI_EVENT_KEYUP          3
#define GARI_EVENT_MOUSEPRESS     4
#define GARI_EVENT_MOUSERELEASE   5
#define GARI_EVENT_MOUSEMOVE      6
#define GARI_EVENT_JOYMOVE        7
#define GARI_EVENT_JOYPRESS       8
#define GARI_EVENT_JOYRELEASE     9
#define GARI_EVENT_VIDEORESIZE   10
#define GARI_EVENT_VIDEOEXPOSE   11
#define GARI_EVENT_QUIT          12 
#define GARI_EVENT_USER          13 
#define GARI_EVENT_SYSTEM        14 

struct GariEvent_ {
  uint8_t   kind;
  uint8_t   gain;
  uint16_t  key;
  uint16_t  mod;
  uint16_t  unicode; 
};


const GariEvent GARI_EMPTY_EVENT = {GARI_EVENT_NONE, 0};


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
      break;   
      
        
  }    
  return res;
}




GariEvent event_poll() {
  SDL_Event      event;
  if (SDL_PollEvent(&event)) {
    return gari_event_fromsdl(event);
  } else {
    GariEvent result = GARI_EMPTY_EVENT; 
    return result;  
  } 
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





