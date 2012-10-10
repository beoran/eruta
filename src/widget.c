
#include "eruta.h"
#include "mem.h"
#include "str.h"
#include "image.h"
#include "dynar.h"
#include "widget.h"
#include "ui.h"

/*
* = Explanation of the Widget system. =
* 
* The actual logical structure of the UI is maintained on the Lua side.
* On the C side, we have Widgets, which are very basic and simple objects that
* can draw themselves and dispose of themselves when they are done, and
* nothing much more. The Lua script will then create these Gadgets,
* hold references to them, and  call the draw command to draw them,
* the drawing happens on the C side for speed. 
*
* The response to events and everything else happens on the Lua side.
* The C side does take the Allegro events and translates them into
* in game commands (direction of motion, etc), although for the mouse
* this will probably be clicks at given positions.
*
* Control of Eruta should be possible comfortably though joystick,
* keybroad, and a single mouse only, so people with physical limitations
* can also play the game.
*
*/


/* bounds functions */

/** Makes a new bounds box struct. */
Bounds bounds_make(int x, int y, int w, int h) {
  Point p = point(x, y);
  Point s = point(w, h);
  Bounds result = { p, s };
  return result;
}

/** Initializes a bounds pointer by copying data from a bounds struct.  */
Bounds * bounds_initbounds(Bounds * self, Bounds bounds) {
  if(!self) return NULL;
  (*self) = bounds;
  return self;
}

/** Initializes a bounds pointer from it's coordinates. */
Bounds * bounds_init(Bounds * self, int x, int y, int w, int h) {
  return bounds_initbounds(self, bounds_make(x, y, w, h));
}

/** Get x position of bounds. */
int bounds_x(Bounds * self) {
  return self->p.x;
}

/** Get y position of bounds. */
int bounds_y(Bounds * self) {
  return self->p.y;
}

/** Get width of bounds. */
int bounds_w(Bounds * self) {
  return self->s.x;
}

/** Get height of bounds. */
int bounds_h(Bounds * self) {
  return self->s.y;
}


/** Makes a new style struct. */
Style style_make(Color fore, Color back, Font * font, Image * background) {
  Style result = { fore, back, font, background };
  return result;
}

/** Initializes a style pointer by copying data from a style struct.  */
Style * style_initstyle(Style * self, Style style) {
  if(!self) return NULL;
  (*self) = style;
  return self;
}

/** Initializes a bounds pointer from it's member data. */
Style * style_init( Style * self, 
                    Color fore, Color back, Font * font, Image * background) {
  return style_initstyle(self, style_make(fore, back, font, background));
}

/** Get foreground color of style. */
Color   style_forecolor(Style * self)   {  
  return self->forecolor;  
}
/** Get background color of style. */
Color   style_backcolor(Style * self)   {  
  return self->backcolor;  
}
/** Get background image of style. */
Image * style_background(Style * self)  {  
  return self->background; 
}
/** Get font of style. */
Font  * style_font(Style * self)        {  
  return self->font;       
}



/** Type and methods of the Widgets */
struct WidgetMethods_ {
  WidgetDraw * draw;
  WidgetDraw * done;
};

// typedef struct WidgetMethods_ WidgetMethods;

/* Widgets are individual parts of the UI. Each widget can contain
sub-widgets. A note on ownership: the pointers to font and image in style
are NOT cleaned up, since style is intended to be mostly a shallow copy in which
font and background image are repeated many times.
*/
struct Widget_ {
  /* Bounds */
  Bounds          bounds;
  /* Styling elements. */
  Style           style;
  /* Class             */
  WidgetMethods * methods;
};

/** Get bounds of widget. */
Bounds widget_bounds(Widget * self) {
  return self->bounds;
}

/** Get width of widget. */
int widget_w(Widget * self) {  
  return bounds_w((Bounds *)self); 
}
/** Get height of widget. */
int widget_h(Widget * self) {  
  return bounds_h((Bounds *)self); 
}
/** Get x position of widget. */
int widget_x(Widget * self) {  
  return bounds_x((Bounds *)self); 
}
/** Get y position of widget. */
int widget_y(Widget * self) {  
  return bounds_y((Bounds *)self); 
}
/** Get z position of widget. */
int widget_z(Widget * self) {  
  return 0;
}

/** Get foreground color of widget. */
Color widget_forecolor(Widget * self) {  
  return style_forecolor(&self->style);  
}
/** Get backgrund color of widget. */
Color widget_backcolor(Widget * self) {  
  return style_backcolor(&self->style);  
}
/** Get font of widget. */
Font * widget_font(Widget * self)     {  
  return style_font(&self->style);
}

/** Get background bitmap of widget. */
Image * widget_background(Widget * self) {
  return style_background(&self->style);
}

Widget * widget_initall(Widget * self, Bounds bounds, Style style) {
  if(!self) return NULL;
  self->bounds = bounds;
  self->style  = style;
/*  
  self->parent = parent;
  self->child  = self->next = self->drawnext = NULL;
  self->can_focus = self->active = TRUE;
  self->focus     = self->closed = self->hidden = self->mouse_over 
                  = self->mouse_down = FALSE;
*/                  
  return self;
}


/** Initialzes a widget with given bounds and style. */
Widget * widget_initbounds(Widget * self, Bounds bounds) {
  Color fg    = color_rgb(0,0,0);
  Color bg    = color_rgb(255,0,0);
  Style style = { fg, bg, NULL, NULL };
  return widget_initall(self, bounds, style);
}

/** Initialzes a widget from another one's bounds and style. */
Widget * widget_initparent(Widget * self,  Widget * parent) {
  return widget_initall(self, parent->bounds, parent->style);
}

/** Allocates a widget */
Widget * widget_allocate() {
  return STRUCT_ALLOC(Widget);
}


/** Call when widget is not needed anymore. */
Widget * widget_done(Widget * widget) {
  // do nothing there as background image and font are NOT owned.
  return widget;
}


/** Frees a widget. Calls widget->methods->done(), then mem_free if
the latter returns not NULL. Returns NULL. */
Widget * widget_free(Widget * self) {
  if (self && self->methods && self->methods->done) {
    if(self->methods->done(self)) {
      mem_free(self);
    }
  }
  return NULL;
}


#define WIDGET_BORDER 3 

/** Draws a rounded frame as background for a widget. */
void widget_drawroundframe(Widget * self) {
  if(!self) return;
  draw_roundframe(widget_x(self), widget_y(self), 
                  widget_w(self), widget_h(self),
                  WIDGET_BORDER,
                  widget_forecolor(self), widget_backcolor(self));
}


struct WidgetBox_ {
  struct Widget_ widget;
};


/** 
* A label is simply a piece of text that is drawn at bounds.x, bounds.y
* using style.font in style.forecolor. No background is drawn.
*/
struct WidgetLabel_ {
  Widget        parent;
  USTR        * text;
};

/** Initializes a label. */
WidgetLabel * widgetlabel_init(WidgetLabel * self, Widget * parent, Bounds bounds, 
                          const char * text) {
  if(!widget_initbounds((Widget *)self, bounds)) return NULL;
  self->text = ustr_new(text);
  return self;
}

WidgetLabel * widgetlabel_done(WidgetLabel * self) {
  if(!self) return NULL;
  ustr_free(self->text);
  return NULL;
} 



struct WidgetChoose_ {
  Widget parent;
  Dynar * options;
}; 


/* A console is a console for command-line interaction and error display. When it's active it captures all input (as long as it's active) */
struct Console_ {
  Widget  parent;
  Lilis * lines;
  Lilis * last;
  int     count;
  int     max;
  int     start;
  int     active;
  int     charw;
  int     cursor;
  char  * buf;
  USTR  * input;
  ConsoleCommand * command; // called when a command has been entered, if set.
  void * command_data; // command data.
};

/** Cleans up a console */
Console * console_done(Console * self) {
  Lilis * aid;
  if(!self) return NULL;
  for (aid = self->lines; aid; aid = lilis_next(aid))  {
    ustr_free((USTR *)lilis_data(aid));
  }
  lilis_free(self->lines);
  mem_free(self->buf);
  ustr_free(self->input);
  return self;
}


/** Deallocates a console. */
Console * console_free(Console * self) {
  console_done(self);
  mem_free(self);
  return NULL;
}

/** Allocates a console. */
Console * console_alloc() {
  return STRUCT_ALLOC(Console);
}

#define CONSOLE_MAX 1000

/** Initializes a console. */
Console * console_initall(Console * self, Bounds bounds, Style style) {
  if(!self) return NULL;
  if(!widget_initall((Widget *)self, bounds, style)) return NULL;
  self->lines = lilis_newempty();
  if(!self->lines) return NULL;
  self->last  = lilis_addnew(self->lines, NULL);
  if(!self->last) { console_done(self); return NULL; }
  self->count = 0;
  self->max   = 1000; // MUST be at least 2, 3 to see anything...
  self->start = 0;
  self->active= FALSE;
  self->charw = 80; 
  self->buf   = mem_alloc(self->charw + 1);
   // one extra for NULL at end . 
  if(!self->buf) { console_done(self); return NULL; }
  self->input = ustr_new("");
  self->cursor= 0;
  if(!self->input) { console_done(self); return NULL; }
  self->command      = NULL;
  self->command_data = NULL;
  return self;
}


/** Sets the console's command function and data. */
void console_command_(Console * self, ConsoleCommand * command, void * data) {
  self->command      = command;
  self->command_data = data;
}

/** Let the console perform a command if possible. returns nonzero on error,
zero if OK. */
int console_docommand(Console * self, char * text) {
  if(!self) return -1;
  if(!self->command) return -2;
  return self->command(self, text, self->command_data);
}

/** Initializes a console. */
Console * console_new(Bounds bounds, Style style) {
  Console * self = console_alloc();
  if(!console_initall(self, bounds, style)) {
    return console_free(self);
  }
  return self;
}

int console_addstr(Console * self, char * str) {
  USTR * storestr;
  if(!self) return -1;
  storestr = ustr_new(str);
  if(!storestr) return -2;
  if(!lilis_addnew(self->lines, storestr)) { 
    ustr_free(storestr);
    return -3;
  }
  self->count++;
  if(self->count > self->max) { // remove last node
    Lilis * prev, * last;
    // free data
    USTR * data = (USTR *) lilis_data(self->last);
    ustr_free(data);
    // get prev node
    prev        = lilis_previous(self->last);
    // free last node
    lilis_erase(self->last);
    // prev to last is now last node.
    self->last  = prev;
    self->count --; // reduce count again.
  }
  return self->count;
}

/** Puts a string on the console .*/
int console_puts(Console * self, const char * str) {
  int index;
  int size     = strlen(str);
  int leftsize = size;
  int lines = 0;
  for (index = 0; index < size; 
       index += self->charw, 
       leftsize -= (self->charw) ) {
    int copysize = (leftsize > self->charw ? self->charw : leftsize);
    help_strncpy(self->buf, str + index, copysize, self->charw + 1);
    console_addstr(self, self->buf);
    lines++;
  }
  return lines;
} 




/** Draws a console. */
void console_draw(Console * self) {
  Font * font = widget_font((Widget *)self);
  Color color = widget_forecolor((Widget *)self);
  int high, linehigh, index, x, y, skip;
  if(!self->active) return;
  widget_drawroundframe((Widget *)self);
  high        = widget_h((Widget *) self) - 10;
  x           = widget_x((Widget *) self) +  5;
  y           = widget_y((Widget *) self) -  5;
  linehigh    = font_lineheight(font);
  Lilis * now = self->lines;
  // skip start lines (to allow scrolling backwards) 
  for (skip = self->start; now && (skip > 0); skip --) {
    now = lilis_next(now); // move to next line.
  }
  for (index = high-linehigh; index > 0; index -= linehigh) {
    USTR * textstr;
    if(!now) break;
    textstr = (USTR *) lilis_data(now);
    if(textstr) {
      font_drawstr(font, color, x, y + index, 0, textstr);
    }
    now = lilis_next(now);
  }
  // draw input string
  font_drawstr(font, color, x, y + high - linehigh, 0, self->input);
  // draw start for debugging
  al_draw_textf(font, color, x, y, 0, "start: %d", self->start);
  
}

/** Activates or deactivates the console. */
void console_active_(Console * self, int active) {
  if(!self) return;
  self->active = active;
}

/** Returns nonzero if console is active zero if not. */
int console_active(Console * self) {
  if(!self) return 0;
  return self->active;
}

/** scrolls the console 1 step in the given direction. */
int console_scroll(Console * self, int direction) {
  if((!self) || (!direction)) return FALSE;
  if(direction < 0) self->start--;
  if(direction > 0) self->start++;
  self->start = (self->start < 1) ? 0 : self->start;
  self->start = (self->start > self->max) ? self->max : self->start;
  return TRUE;
}


int console_handle_keychar(Console * self, ALLEGRO_EVENT * event) { 
  int ch = event->keyboard.unichar;
  int kc = event->keyboard.keycode;
  switch(kc) {
    case ALLEGRO_KEY_F1: return TRUE; // ignore the start-console key
    case ALLEGRO_KEY_PGUP: return console_scroll(self, 1);
    case ALLEGRO_KEY_PGDN: return console_scroll(self, -1);
    case ALLEGRO_KEY_BACKSPACE:
      // remove last character typed.
      ustr_remove_chr(self->input, ustr_offset(self->input, -1));
      return TRUE;
    break;    
    case ALLEGRO_KEY_ENTER: {
      char * command = ustr_c(self->input);
      // execute command
      if(console_docommand(self, command)) { 
        console_puts(self, "Error in running comand");
        console_puts(self, command);
      }
      ustr_truncate(self->input, 0);
      // empty string by truncating it
      return TRUE;
      }
    default: break;
  }
  
  ustr_appendch(self->input, ch);
  if(event->keyboard.keycode == ALLEGRO_KEY_ESCAPE) {
    console_active_(self, false); // disable console if esc is pressed.
  }
  return TRUE;
}



int console_handle_mouseaxes(Console * self, ALLEGRO_EVENT * event) { 
  int z = event->mouse.dz;
  // only capture mouse scroll wheel...
  if(z == 0) return FALSE;
  if(z < 0) return console_scroll(self, -1);
  if(z > 0) return console_scroll(self, +1);
  return TRUE;
}


/** Let the console handle allegro events. Will return true if it was consumed,
false if not. */

int console_handle(Console * self, ALLEGRO_EVENT * event) { 
  if(!console_active(self)) return FALSE;
  
  switch(event->type) {
    case ALLEGRO_EVENT_KEY_DOWN:
      return TRUE;
    case ALLEGRO_EVENT_KEY_UP:
      return TRUE;
    case ALLEGRO_EVENT_KEY_CHAR:
      return console_handle_keychar(self, event);
    case ALLEGRO_EVENT_MOUSE_AXES:
      return console_handle_mouseaxes(self, event);
  }
  
  return FALSE;
}










