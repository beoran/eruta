
#include "eruta.h"
#include "mem.h"
#include "str.h"
#include "image.h"
#include "dynar.h"
#include "flags.h"
#include "ui.h"
#include "widget.h"


/*
*
* Control of Eruta should be possible comfortably though joystick,
* keybroad, and a single mouse only, so people with physical limitations
* can also play the game.
*
* The following widgets will be needed:
* 1 ) Console to log errors and interact with ruby/state.
* 2 ) Menu (vertical, horizontal) 
* 3 ) Ring menu ?
* 4 ) Notification/explanation display.
* 5 ) Talk bubbles.
* 6 ) Talk dialog with text display and option selection.
* User interface is different depending on the game "mode"
* Possible modes could be intro, start menu, 
* configuration, image/text display (story mode/cutscenes, etc)
* normal mode, battle mode and menu mode.
* 
* Idea for simplification: actually an widget and a mode
* are comparable in that they deal with inpuut each differently, and
* output each in differen ways. Only, a mode is a combination of 
* UI elements. For example, the main menu is a static background 
* with a vertical menu over it, and the menu has the focus.
*
* Ideas on event handling: The "problem" with allegro is that the
* even types are sparse, and potentially stretch the whole integer range.
* Hence, it's not realistic to use a simple array as a jump table. Some
* more complex structure, like a hsah table, tree, etc, would be needed 
* for fast lookup of the event handler function. Hence, and probably by 
* design, a case statement is the best way to handle and dispatch the 
* input. The react system I wrote has the disadvantage that it will add
* an additional pointer indirection to that switch statement, a problem the 
* console doesn't have. Hence, I'll go for a simple event handler
* for every function that takes the allegro event like the console does.
* 
* Hence every widget will have tthe following basic interface 
* functions: 
* free (on destruction)
* done (on deinit)
* event (on any allegro or user event or action)
* update (called every n ticks, when the widget should recalculate it's position, etc)
* draw (called when the widget should draw itself)
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

/** Get priority of bounds. */
int bounds_z(Bounds * self) {
  return self->z;
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



// typedef struct WidgetMethods_ WidgetMethods;



/** Get bounds of widget. */
Bounds widget_bounds(Widget * self) {
  return self->bounds;
}

/** Get width of widget. */
int widget_w(Widget * self) {  
  return bounds_w(&self->bounds); 
}
/** Get height of widget. */
int widget_h(Widget * self) {  
  return bounds_h(&self->bounds); 
}
/** Get x position of widget. */
int widget_x(Widget * self) {  
  return bounds_x(&self->bounds); 
}
/** Get y position of widget. */
int widget_y(Widget * self) {  
  return bounds_y(&self->bounds); 
}
/** Get z position of widget. */
int widget_z(Widget * self) {  
  return bounds_z(&self->bounds);
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

/** Gets the flags of a widget. */
int widget_flags(Widget * self) {
  return self->flags;
}

/** Gets the id of a widget. */
int widget_id(Widget * self, int id) {
  return self->id;
}

/** Sets all the flags of a widget at once. */
int widget_flags_(Widget * self, int flags) {
  return self->flags = flags;
}

/** Sets the id of a widget. */
int widget_id_(Widget * self, int id) {
  return self->id = id;
}


/** Sets an individual flag on the widget. */
int widget_flag(Widget * self, int flag) {
  return flags_set(&self->flags, flag);
}

/** Unsets an individual flag on the widget. */
int widget_unflag(Widget * self, int flag) {
  register int wflags = self->flags;
  return flags_unset(&self->flags, flag);
}

/** Sets or unsets an individual flag on the widget. 
If set is true the flag is set, if false it's unset. */
int widget_doflag(Widget * self, int flag, int set) {
  return flags_put(&self->flags, flag, set);
}

/** Checks if an individual flag is set */
int widget_flag_p(Widget * self, int flag) {
  return flags_get(self->flags, flag);
}

/** Checks if the widget is visible or not.  */
int widget_visible(Widget * self) {
  return widget_flag_p(self, WIDGET_FLAG_VISIBLE);
}

/** Checks if the widget is listening to input or not.  */
int widget_listening(Widget * self) {
  return widget_flag_p(self, WIDGET_FLAG_LISTENING);
}

/** Checks if the widget is active, hat is both visible and 
listening to input or not.  */
int widget_active(Widget * self) {
  return widget_flag_p(self, WIDGET_FLAG_ACTIVE);
}

/** Checks if the widget is focused or not.  */
int widget_focused(Widget * self) {
  return widget_flag_p(self, WIDGET_FLAG_FOCUSED);
}

/** Checks if the widget selected or not.  */
int widget_selected(Widget * self) {
  return widget_flag_p(self, WIDGET_FLAG_SELECTED);
}

/** Sets the widget to be visible or not depending on set. */
int widget_visible_(Widget * self, int set) {
  return widget_doflag(self, WIDGET_FLAG_VISIBLE, set);
}

/** Sets the widget if the widget is listening to input or not depending
on set. */
int widget_listening_(Widget * self, int set) {
  return widget_doflag(self, WIDGET_FLAG_VISIBLE, set);
}

/** Sets the widget to be active or not */
int widget_active_(Widget * self, int set) {
  return widget_doflag(self, WIDGET_FLAG_ACTIVE, set);
}

/** Sets if the widget is focused or not.  */
int widget_focused_(Widget * self, int set) {
  return widget_doflag(self, WIDGET_FLAG_FOCUSED, set);
}

/** Sets if the widget selected or not.  */
int widget_selected_(Widget * self, int set) {
  return widget_doflag(self, WIDGET_FLAG_SELECTED, set);
}

/** Sets up the method table of a widget. */
Widget * widget_metab_(Widget * self, WidgetMetab * metab) {
  if(!self) return NULL;
  self->metab = metab;
  return self;
}

/** Fully initializes a widget. */
Widget * widget_initall(Widget * self, 
                        int id, WidgetMetab * metab, 
                        Bounds bounds, Style style) {
  if(!self) return NULL;
  self->id     = id;
  widget_metab_(self, metab);
  self->bounds = bounds;
  self->style  = style;
  widget_active_(self, TRUE);
  return self;
}


/** Initializes a widget with given bounds and style. */
Widget * widget_initbounds(Widget * self, int id,
                           WidgetMetab * metab, Bounds bounds)
{
  Color fg    = color_rgb(0,0,0);
  Color bg    = color_rgb(255,0,0);
  Style style = { fg, bg, NULL, NULL };
  return widget_initall(self, id, metab, bounds, style);
}

/** Initialzes a widget from another one's bounds and style. */
Widget * widget_initparent(Widget * self, int id, Widget * parent) {
  return widget_initall(self, id, parent->metab, parent->bounds, parent->style);
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
  if (self && self->metab && self->metab->done) {
    if(self->metab->done(self)) {
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


#ifndef COMMENT_
/* 
* A label is simply a piece of text that is drawn at bounds.x, bounds.y
* using style.font in style.forecolor. No background is drawn.

struct WidgetLabel_ {
  Widget        parent;
  USTR        * text;
};


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
*/
#endif


/* A console is a console for command-line interaction and error display. When it's active it captures all input (as long as it's active) */
struct Console_ {
  Widget  widget;
  Lilis * lines;
  Lilis * last;
  int     count;
  int     max;
  int     start;
  int     charw;
  int     cursor;
  char  * buf;
  USTR  * input;
  ConsoleCommand * command; // called when a command has been entered, if set.
  void * command_data; // command data.
};


/** Sets the console's command function and data. */
void console_command_(Console * self, ConsoleCommand * command, void * data) {
  self->command      = command;
  self->command_data = data;
}

/** Let the console perform a command if possible. returns nonzero on error,
zero if OK. */
int console_docommand(Console * self, const char * text) {
  if(!self) return -1;
  if(!self->command) return -2;
  return self->command(self, text, self->command_data);
}



/** Adds a line of text to the console. */
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
void console_draw(Widget * widget) {
  Console * self ;
  Font * font    ;
  Color color    ;
  int high, linehigh, index, x, y, skip;
  if (!widget_visible(widget)) return;
  
  self  = (Console *) widget;
  font  = widget_font(widget);
  color = widget_forecolor(widget);
  
  widget_drawroundframe(widget);
  high        = widget_h(widget) - 10;
  x           = widget_x(widget) +  5;
  y           = widget_y(widget) -  5;
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
  widget_active_(&self->widget, active);
}

/** Returns nonzero if console is active zero if not. */
int console_active(Console * self) {
  if(!self) return 0;
  return widget_active(&self->widget);
}

/** scrolls the console 1 step in the given direction. */
int console_scroll(Console * self, int direction) {
  if((!self) || (!direction)) return FALSE;
  if(direction < 0) self->start--;
  if(direction > 0) self->start++;
  self->start = (self->start < 1) ? 0 : self->start;
  self->start = (self->start > self->max) ? self->max : self->start;
  return WIDGET_HANDLE_OK;
}


/* Key input event handler for console. */
int console_handle_keychar(Console * self, ALLEGRO_EVENT * event) { 
  int ch = event->keyboard.unichar;
  int kc = event->keyboard.keycode;
  switch(kc) {
    // ignore the start-console key
    case ALLEGRO_KEY_F1   : return WIDGET_HANDLE_OK;
    case ALLEGRO_KEY_PGUP : return console_scroll(self, 1);
    case ALLEGRO_KEY_PGDN : return console_scroll(self, -1);
    case ALLEGRO_KEY_BACKSPACE:
      // remove last character typed.
      ustr_remove_chr(self->input, ustr_offset(self->input, -1));
      return WIDGET_HANDLE_OK;
    break;    
    case ALLEGRO_KEY_ENTER: {
      const char * command = ustr_c(self->input);
      // execute command
      if(console_docommand(self, command)) { 
        console_puts(self, "Error in running comand");
        console_puts(self, command);
      }
      ustr_truncate(self->input, 0);
      // empty string by truncating it
      return WIDGET_HANDLE_OK;
      }
    default: break;
  }
  
  ustr_appendch(self->input, ch);
  if(event->keyboard.keycode == ALLEGRO_KEY_ESCAPE) {
    console_active_(self, false); // disable console if esc is pressed.
  }
  return WIDGET_HANDLE_OK;
}


/* Mouse axe event handler for console */
int console_handle_mouseaxes(Console * self, ALLEGRO_EVENT * event) { 
  int z = event->mouse.dz;
  // only capture mouse scroll wheel...
  if(z == 0) return WIDGET_HANDLE_IGNORE;
  if(z < 0) return console_scroll(self, -1);
  if(z > 0) return console_scroll(self, +1);
  return WIDGET_HANDLE_OK;
}


/** Let the console handle allegro events. Returns 0 if event was consumed,
postoive if not, and negative on error. */

int console_handle(Widget * widget, ALLEGRO_EVENT * event) { 
  Console * self = (Console *) widget;
  if(!widget) return WIDGET_HANDLE_ERROR;
  if(!widget_active(widget)) return WIDGET_HANDLE_IGNORE;
  
  
  switch(event->type) {
    case ALLEGRO_EVENT_KEY_DOWN:
      return WIDGET_HANDLE_OK;
    case ALLEGRO_EVENT_KEY_UP:
      return WIDGET_HANDLE_OK;
    case ALLEGRO_EVENT_KEY_CHAR:
      return console_handle_keychar(self, event);
    case ALLEGRO_EVENT_MOUSE_AXES:
      return console_handle_mouseaxes(self, event);
  }
  
  return WIDGET_HANDLE_IGNORE;
}

/* Global console method table. */
static WidgetMetab console_metab_ = {
  console_free,
  console_done,
  console_draw,
  console_handle,
  NULL,
};


/** Cleans up a console */
void * console_done(void * widget) {
  Console * self = (Console *) widget;
  Lilis * aid;
  if(!self) return NULL;
  for (aid = self->lines; aid; aid = lilis_next(aid))  {
    ustr_free((USTR *)lilis_data(aid));
  }
  self->lines   = lilis_free(self->lines);
  self->buf     = mem_free(self->buf);
  ustr_free(self->input);
  self->input   = NULL;
  return widget;
}


/** Deallocates a console. */
void * console_free(void * widget) {
  Console * self = (Console *) widget;
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
Console * console_initall(Console * self, int id, Bounds bounds, Style style) {
  if(!self) return NULL;
  if(!widget_initall(&self->widget, id, &console_metab_, bounds, style)) { 
    return NULL;
  }
  self->lines = lilis_newempty();
  if(!self->lines) return NULL;
  self->last  = lilis_addnew(self->lines, NULL);
  if(!self->last) { console_done(self); return NULL; }
  widget_active_(&self->widget, FALSE);
  self->count = 0;
  // max MUST be at least 2, 3 to see anything...
  self->max   = 1000; 
  self->start = 0;
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

/** Initializes a new console. */
Console * console_new(int id, Bounds bounds, Style style) {
  Console * self = console_alloc();
  if(!console_initall(self, id, bounds, style)) {
    return console_free(self);
  }
  return self;
}






