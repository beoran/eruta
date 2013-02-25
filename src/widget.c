
#include "eruta.h"
#include "mem.h"
#include "str.h"
#include "image.h"
#include "dynar.h"
#include "flags.h"
#include "ui.h"
#include "widget.h"
#include "draw.h"
#include "bad.h"


/*
*
* Control of Eruta should be possible comfortably though joystick,
* keybroad, and a single mouse only, so people with physical limitations
* can also play the game.
*
* The following widgets will be needed:
* 1 ) BBConsole to log errors and interact with ruby/state.
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

/* Finds the BBWidgetHandler in the actions table for the given type. */
BBWidgetHandler * bbwidgetactions_find(BBWidgetAction * actions, int type) {
  while((actions->type != -1) && (actions->handler != NULL)) {
    if (actions->type == type) {
      return actions->handler;
    }
    actions++;
  }
  return NULL;
} 


/* Finds the BBWidgetHandler in the actions table for the given type and calls it
 * with widget and data as parameters. */
int bbwidgetactions_handle(BBWidgetAction * actions, int type,
                         BBWidget * widget, void * data) {
  BBWidgetHandler * handler = bbwidgetactions_find(actions, type);
  if (handler) { return handler(widget, data); }
  return BBWIDGET_HANDLE_IGNORE;
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



// typedef struct BBWidgetMethods_ BBWidgetMethods;



/** Get bounds of widget. */
Rebox bbwidget_bounds(BBWidget * self) {
  return self->bounds;
}

/** Get width of widget. */
int bbwidget_w(BBWidget * self) {  
  return rebox_w(&self->bounds); 
}
/** Get height of widget. */
int bbwidget_h(BBWidget * self) {  
  return rebox_h(&self->bounds); 
}
/** Get x position of widget. */
int bbwidget_x(BBWidget * self) {  
  return rebox_x(&self->bounds); 
}
/** Get y position of widget. */
int bbwidget_y(BBWidget * self) {  
  return rebox_y(&self->bounds); 
}
/** Get z position of widget. */
int bbwidget_z(BBWidget * self) {  
  return self->z;
}

/** Get foreground color of widget. */
Color bbwidget_forecolor(BBWidget * self) {  
  return style_forecolor(&self->style);  
}
/** Get backgrund color of widget. */
Color bbwidget_backcolor(BBWidget * self) {  
  return style_backcolor(&self->style);  
}
/** Get font of widget. */
Font * bbwidget_font(BBWidget * self)     {  
  return style_font(&self->style);
}

/** Get background bitmap of widget. */
Image * bbwidget_background(BBWidget * self) {
  return style_background(&self->style);
}

/** Gets the flags of a widget. */
int bbwidget_flags(BBWidget * self) {
  return self->flags;
}

/** Gets the id of a widget. */
int bbwidget_id(BBWidget * self, int id) {
  return self->id;
}

/** Sets all the flags of a widget at once. */
int bbwidget_flags_(BBWidget * self, int flags) {
  return self->flags = flags;
}

/** Sets the id of a widget. */
int bbwidget_id_(BBWidget * self, int id) {
  return self->id = id;
}


/** Sets an individual flag on the widget. */
int bbwidget_flag(BBWidget * self, int flag) {
  return flags_set(&self->flags, flag);
}

/** Unsets an individual flag on the widget. */
int bbwidget_unflag(BBWidget * self, int flag) {
  register int wflags = self->flags;
  return flags_unset(&self->flags, flag);
}

/** Sets or unsets an individual flag on the widget. 
If set is true the flag is set, if false it's unset. */
int bbwidget_doflag(BBWidget * self, int flag, int set) {
  return flags_put(&self->flags, flag, set);
}

/** Checks if an individual flag is set */
int bbwidget_flag_p(BBWidget * self, int flag) {
  return flags_get(self->flags, flag);
}

/** Checks if the widget is visible or not.  */
int bbwidget_visible(BBWidget * self) {
  return bbwidget_flag_p(self, BBWIDGET_FLAG_VISIBLE);
}

/** Checks if the widget is listening to input or not.  */
int bbwidget_listening(BBWidget * self) {
  return bbwidget_flag_p(self, BBWIDGET_FLAG_LISTENING);
}

/** Checks if the widget is active, hat is both visible and 
listening to input or not.  */
int bbwidget_active(BBWidget * self) {
  return bbwidget_flag_p(self, BBWIDGET_FLAG_ACTIVE);
}

/** Checks if the widget is focused or not.  */
int bbwidget_focused(BBWidget * self) {
  return bbwidget_flag_p(self, BBWIDGET_FLAG_FOCUSED);
}

/** Checks if the widget selected or not.  */
int bbwidget_selected(BBWidget * self) {
  return bbwidget_flag_p(self, BBWIDGET_FLAG_SELECTED);
}

/** Sets the widget to be visible or not depending on set. */
int bbwidget_visible_(BBWidget * self, int set) {
  return bbwidget_doflag(self, BBWIDGET_FLAG_VISIBLE, set);
}

/** Sets the widget if the widget is listening to input or not depending
on set. */
int bbwidget_listening_(BBWidget * self, int set) {
  return bbwidget_doflag(self, BBWIDGET_FLAG_VISIBLE, set);
}

/** Sets the widget to be active or not */
int bbwidget_active_(BBWidget * self, int set) {
  return bbwidget_doflag(self, BBWIDGET_FLAG_ACTIVE, set);
}

/** Sets if the widget is focused or not.  */
int bbwidget_focused_(BBWidget * self, int set) {
  return bbwidget_doflag(self, BBWIDGET_FLAG_FOCUSED, set);
}

/* Sets if the widget selected or not.  */
int bbwidget_selected_(BBWidget * self, int set) {
  return bbwidget_doflag(self, BBWIDGET_FLAG_SELECTED, set);
}

/* Sets up the method cache of a widget based on the actions. */
BBWidget * bbwidget_metabfromacts(BBWidget * self, BBWidgetAction * acts) {
  if(!self) return NULL;
  if(!acts) return NULL;
  self->metab.done = bbwidgetactions_find(acts, BBWIDGET_EVENT_DONE);
  self->metab.draw = bbwidgetactions_find(acts, BBWIDGET_EVENT_DRAW);
  self->metab.free = bbwidgetactions_find(acts, BBWIDGET_EVENT_FREE);
  return self;
}

/* Sets up the acts, and also updates the method cache of a widget */
BBWidget * bbwidget_acts_(BBWidget * self, BBWidgetAction * acts) {
  if(!self) return NULL;
  self->acts = acts;
  return bbwidget_metabfromacts(self, acts);
}

/* Fully initializes a widget. */
BBWidget * 
bbwidget_initall(BBWidget * self,  int id, BBWidgetAction * acts, 
                 Rebox bounds, Style style) {
  if(!self) return NULL;
  self->id     = id;
  self->acts   = acts; 
  bbwidget_metabfromacts(self, acts);
  self->bounds = bounds;
  self->style  = style;
  bbwidget_active_(self, TRUE);
  return self;
}

/* Initializes a widget with given bounds and style. */
BBWidget * 
bbwidget_initbounds(BBWidget * self, int id,  BBWidgetAction * acts, Rebox bounds) {
  Color fg    = color_rgb(0,0,0);
  Color bg    = color_rgb(255,0,0);
  Style style = { fg, bg, NULL, NULL };
  return bbwidget_initall(self, id, acts, bounds, style);
}

/* Initializes a widget from another one's bounds and style. */
BBWidget * bbwidget_initparent(BBWidget * self, int id, BBWidget * parent) {
  return bbwidget_initall(self, id, parent->acts, parent->bounds, parent->style);
}

/* Allocates a widget */
BBWidget * bbwidget_allocate() {
  return STRUCT_ALLOC(BBWidget);
}

/* Call when widget is not needed anymore. */
BBWidget * bbwidget_done(BBWidget * widget) {
  // do nothing there as background image and font are NOT owned.
  return widget;
}


/* Frees a widget. Calls widget->methods->done(), then mem_free if
the latter returns not NULL. Returns NULL. */
BBWidget * bbwidget_free(BBWidget * self) {
  if (self && self->metab.done) {
    if(self->metab.done(self, NULL)) {
      mem_free(self);
    }
  }
  return NULL;
}

/** Generic widget drawing. */
void bbwidget_draw(BBWidget * self) {
  if(self->metab.draw) {
    self->metab.draw(self, NULL);
  }
}


/** Generic widget event handling. */
void bbwidget_handle(BBWidget * self, ALLEGRO_EVENT * event) {
  if(self->acts) {
    bbwidgetactions_handle(self->acts, event->any.type, self, event);
  }
}

/** Generic widget update. */
void bbwidget_update(BBWidget * self, ALLEGRO_EVENT * event) {
  if(self->metab.update) {
    self->metab.update(self, event);
  }
}



/* Helper struct that keeps track of the BYTE positions within 
a c string or USTR where a line or word starts or ends with a given maxwidth. */
typedef struct BBTextInfo_ BBTextInfo;

struct BBTextInfo_ {
  int from_char;
  int start_char;
  int stop_char;
  int maxwidth;
};


/* Creates a temporary ustr as per al_ref_ustr but with 
 start and stop as code positions, not byte positions. */
const USTR * 
ustrinfo_newref(
  USTR_INFO * uinfo, const USTR * ustr, int start, int stop) {
  return ustr_refustr(uinfo, ustr, 
                      ustr_offset(ustr, start),
                      ustr_offset(ustr, stop)
                     );
}



/* Creates a temporary ustr that refers ustr but respecds the bounds of the 
textinfo (start_char and enc_char) */
const USTR * bbtextinfo_refustr(BBTextInfo * self, 
                                USTR_INFO  * uinfo,
                                const USTR * ustr) { 
  return ustrinfo_newref(uinfo, ustr, self->start_char, self->stop_char);
}


BBTextInfo * 
bbtextinfo_wordfromtext(BBTextInfo * self, USTR * ustr, Font * font) {
  int found;
  int start_pos;
  int end_pos;
  int now_char;
  int end_char;
  int len;
  int ch;
  if(!self) return NULL;
  now_char         = self->from_char;
  self->start_char = now_char;
  ch = ustr_getnext(ustr, &now_char); 
  while (ch > 0) { 
    switch(ch) { 
      case ' ': /* Found a space, here the word ends, include the space. */
        self->stop_char = now_char;
        return self;
      case '\n': /* A newline ends a word, include the newline. */
        self->stop_char = now_char;
        return self;
      default: /* Other characters mean the word is not finished yet. */
        break;
    }
    /* XXX: Should handle the case for languages that use no spaces, 
    * by checking with al_get_ustr_width but it's not a pressing matter yet.
    */
    ch = ustr_getnext(ustr, &now_char); 
  } 
  // no word found, just set end here and be done. 
  self->stop_char = now_char;
  /* return nULL to signify end */
  return NULL;
}


/** Prints a ustring, since puts or printf print too much some
 times for a refstring.*/
int ustr_print(USTR * word) {
  size_t index;
    for(index = 0; index < ustr_length(word) ; index++) {
      putchar(ustr_get(word, index));
    }
  return index;
}


/** Gets the positions of the next line of text fort he given Unicode string 
and store them in the given info. If the info's from is set to 0 or less, 
the first line is assumed, otherwise, the line will be started from "from".
Uses the given font to determine the width of the text as it is built.
*/
BBTextInfo * 
bbtextinfo_linefromtext(BBTextInfo * self, USTR * ustr, Font * font) {
  BBTextInfo wordinfo;
  USTR_INFO  lineuinfo;
  const USTR     * line;

  USTR_INFO  worduinfo = { 0 };
  const USTR     * word;
  int ch;
  int index;
  int width;
  int last_stop;
  self->start_char   = self->from_char;
  wordinfo.from_char = self->from_char;
  
  while(bbtextinfo_wordfromtext(&wordinfo, ustr, font)) {
    word = bbtextinfo_refustr(&wordinfo, &worduinfo, ustr);
    line = ustrinfo_newref(&lineuinfo, ustr, self->start_char, wordinfo.stop_char);
    width = al_get_ustr_width(font, line);
    if (width > self->maxwidth) { 
      /* XXX: handle case of text overflow by bluntly retuning the word as is.
      Should split single word based on length too.
      There is overflow if this is still the first word as see from wordinfo_start_char.
      */
      if (wordinfo.start_char == self->start_char) {
        self->stop_char  = wordinfo.stop_char;
      } else { 
        self->stop_char  = wordinfo.start_char;
      }
      return self;
    }
    // If we get here, the word way still end on a newline character 
    // check this case. XXX: It works like this because 
    // stop_char is a bit wonky... it points at the first character of the 
    // next word in this case...
    ch = ustr_get(ustr, wordinfo.stop_char - 1);
    if (ch == '\n') {
      self->start_char = self->from_char;
      self->stop_char  = wordinfo.stop_char - 1;
      return self;
    }
    wordinfo.from_char = wordinfo.stop_char;
  }
  /* if we get here, the whole string fits. */
  self->start_char = self->from_char;
  self->stop_char  = wordinfo.stop_char;
  /* Return NULL to tell caller text has been completely split up. */
  return NULL;
}



#define BBWIDGET_BORDER 3 

/** Draws a rounded frame as background for a widget. */
void bbwidget_drawroundframe(BBWidget * self) {
  if(!self) return;
  draw_roundframe(bbwidget_x(self), bbwidget_y(self), 
                  bbwidget_w(self), bbwidget_h(self),
                  BBWIDGET_BORDER,
                  bbwidget_forecolor(self), bbwidget_backcolor(self));
}


/** Skips the text info to the next word or line of text. Must be called 
when looping over bbtextinfo_linefromtext. */
BBTextInfo * bbtextinfo_next(BBTextInfo * self) { 
  if(!self) return NULL;
  self->from_char  = self->stop_char + 1;
  self->start_char = self->from_char;
  return self;
}


    
struct BBWidgetBox_ {
  struct BBWidget_ widget;
};


#ifndef COMMENT_
/* 
* A label is simply a piece of text that is drawn at bounds.x, bounds.y
* using style.font in style.forecolor. No background is drawn.

struct BBWidgetLabel_ {
  BBWidget        parent;
  USTR        * text;
};


BBWidgetLabel * widgetlabel_init(BBWidgetLabel * self, BBWidget * parent, Rebox bounds, 
                          const char * text) {
  if(!bbwidget_initbounds((BBWidget *)self, bounds)) return NULL;
  self->text = ustr_new(text);
  return self;
}

BBWidgetLabel * widgetlabel_done(BBWidgetLabel * self) {
  if(!self) return NULL;
  ustr_free(self->text);
  return NULL;
} 



struct BBWidgetChoose_ {
  BBWidget parent;
  Dynar * options;
}; 
*/
#endif





/* A console is a console for command-line interaction and error display. When it's active it captures all input (as long as it's active) */
struct BBConsole_ {
  BBWidget  widget;
  USTRList  text;
  int       count;
  int       max;
  int       start;
  int       charw;
  int       cursor;
  char    * buf;
  USTR    * input;
  BBConsoleCommand * command; // called when a command has been entered, if set.
  void    * command_data; // command data.
};


/* Converts a widget to a console. Only works if the pointer is wrapped correctly,
 by a console. */
BBConsole * bbwidget_console(BBWidget * widget) { 
  if (!widget) return NULL;
  return bad_container(widget, BBConsole, widget);
}


/** Sets the console's command function and data. */
void bbconsole_command_(BBConsole * self, BBConsoleCommand * command, void * data) {
  self->command      = command;
  self->command_data = data;
}

/** Let the console perform a command if possible. returns nonzero on error,
zero if OK. */
int bbconsole_docommand(BBConsole * self, const char * text) {
  if(!self) return -1;
  if(!self->command) return -2;
  return self->command(self, text, self->command_data);
}



/** Adds a line of text to the console. */
int bbconsole_addstr(BBConsole * self, const char * str) {
  if(!self) return -1;
  if(!ustrlist_shiftcstr(&self->text, str)) { 
    return -3;
  }  
  while(ustrlist_size(&self->text) >= self->max) { // remove last node(s)
    ustrlist_droplast(&self->text);
  }
  return ustrlist_size(&self->text);
}

/** Adds a line of text to the console. */
int bbconsole_addustr(BBConsole * self, const USTR * ustr) {
  if(!self) return -1;
  if(!ustrlist_shiftustr(&self->text, ustr)) { 
    return -3;
  }  
  while(ustrlist_size(&self->text) >= self->max) { // remove last node(s)
    ustrlist_droplast(&self->text);
  }
  return ustrlist_size(&self->text);
}


/** Puts a string on the console .*/
int bbconsole_puts(BBConsole * self, const char * str) {
  int index;
  int size     = strlen(str);
  int leftsize = size;
  int lines = 0;
  USTR_INFO uinfo;
  BBTextInfo info = { 0 };
  info.maxwidth   = bbwidget_w(&self->widget) - 10;
  USTR * ustr;
  const USTR * uline;
  ustr = ustr_new(str);
  while(bbtextinfo_linefromtext(&info, ustr, self->widget.style.font)) {
    uline = bbtextinfo_refustr(&info, &uinfo, ustr);
    bbconsole_addustr(self, uline);
    // don't forget to skip to next line!!!
    bbtextinfo_next(&info);
  }
  uline = bbtextinfo_refustr(&info, &uinfo, ustr);
  bbconsole_addustr(self, uline);
  ustr_free(ustr);
  return lines;
} 


/** Draws a console. */
int bbconsole_draw(BBWidget * widget, void * data) {
  BBConsole * self  ;
  Font * font       ;
  Color color       ;
  USTRListNode * now;
  int high, linehigh, index, x, y, skip;
  int linew;
  if (!bbwidget_visible(widget)) return BBWIDGET_HANDLE_IGNORE;
  
  self  = bbwidget_console(widget);
  font  = bbwidget_font(widget);
  color = bbwidget_forecolor(widget);
  
  bbwidget_drawroundframe(widget);
  high        = bbwidget_h(widget) - 10;
  x           = bbwidget_x(widget) +  5;
  y           = bbwidget_y(widget) -  5;
  linehigh    = font_lineheight(font);
  
  now         = ustrlist_head(&self->text);
  // skip start lines (to allow scrolling backwards) 
  now         = ustrlist_skipnode(&self->text, self->start);
  
  for (index = high-(linehigh*2); index > 0; index -= linehigh) {
    USTR * textstr;
    if(!now) break;
    textstr = ustrlistnode_ustr(now);
    if(textstr) {
      font_drawstr(font, color, x, y + index, 0, textstr);
    }
    now = ustrlistnode_next(now);
  }
  // draw input string
  font_drawstr(font, color, x, y + high - linehigh, 0, self->input);
  // Draw cursor
  linew = al_get_ustr_width(font, self->input);
  al_draw_line(x + linew, y + high - linehigh, x + linew, y + high, color, 1);
  // draw start for debugging
  al_draw_textf(font, color, x, y, 0, "start: %d, size: %d", self->start, 
                ustrlist_size(&self->text));
  return BBWIDGET_HANDLE_OK;
}

/** Activates or deactivates the console. */
void bbconsole_active_(BBConsole * self, int active) {
  if(!self) return;
  bbwidget_active_(&self->widget, active);
}

/** Returns nonzero if console is active zero if not. */
int bbconsole_active(BBConsole * self) {
  if(!self) return 0;
  return bbwidget_active(&self->widget);
}

/** scrolls the console 1 step in the given direction. */
int bbconsole_scroll(BBConsole * self, int direction) {
  if((!self) || (!direction)) return FALSE;
  if(direction < 0) self->start--;
  if(direction > 0) self->start++;
  /* Clamp start between 0 and size of list. */
  self->start = bad_clampi(self->start, 0, ustrlist_size(&self->text));
  return BBWIDGET_HANDLE_OK;
}



/* Key input event handler for console. */
int bbconsole_handle_keychar(BBWidget * widget, void * data) { 
  BBConsole * self        = bbwidget_console(widget);
  ALLEGRO_EVENT * event = (ALLEGRO_EVENT *) data;
  int ch = event->keyboard.unichar;
  int kc = event->keyboard.keycode;
  switch(kc) {
    // ignore the start-console key
    case ALLEGRO_KEY_F1   : return BBWIDGET_HANDLE_OK;
    case ALLEGRO_KEY_PGUP : return bbconsole_scroll(self, 1);
    case ALLEGRO_KEY_PGDN : return bbconsole_scroll(self, -1);
    case ALLEGRO_KEY_BACKSPACE:
      // remove last character typed.
      ustr_remove_chr(self->input, ustr_offset(self->input, -1));
      return BBWIDGET_HANDLE_OK;
    break;    
    case ALLEGRO_KEY_ENTER: {
      const char * command = ustr_c(self->input);
      // execute command
      if(bbconsole_docommand(self, command)) { 
        bbconsole_puts(self, "Error in running comand");
        bbconsole_puts(self, command);
      }
      ustr_truncate(self->input, 0);
      // empty string by truncating it
      return BBWIDGET_HANDLE_OK;
      }
    case ALLEGRO_KEY_ESCAPE:
      bbconsole_active_(self, false); 
      // disable console if esc is pressed.
      return BBWIDGET_HANDLE_OK;
    default: break;
  }
  
  ustr_appendch(self->input, ch);
  return BBWIDGET_HANDLE_OK;
}


/* Mouse axe event handler for console */
int bbconsole_handle_mouseaxes(BBWidget * widget, void * data) { 
  BBConsole * self        = bbwidget_console(widget);
  ALLEGRO_EVENT * event = (ALLEGRO_EVENT *) data;  int z = event->mouse.dz;
  // only capture mouse scroll wheel...
  if(z == 0) return BBWIDGET_HANDLE_IGNORE;
  if(z < 0) return bbconsole_scroll(self, -1);
  if(z > 0) return bbconsole_scroll(self, +1);
  return BBWIDGET_HANDLE_OK;
}

int bbconsole_handle_ok(BBWidget * widget, void * data) { 
  return BBWIDGET_HANDLE_OK;
}


static BBWidgetAction bbconsole_actions[] = {  
  { ALLEGRO_EVENT_KEY_DOWN  , bbconsole_handle_ok         }, 
  { ALLEGRO_EVENT_KEY_UP    , bbconsole_handle_ok         },
  { ALLEGRO_EVENT_KEY_CHAR  , bbconsole_handle_keychar    },
  { ALLEGRO_EVENT_MOUSE_AXES, bbconsole_handle_mouseaxes  },
  { BBWIDGET_EVENT_DRAW     , bbconsole_draw              },
  { BBWIDGET_EVENT_DONE     , bbconsole_done              },
  { BBWIDGET_EVENT_FREE     , bbconsole_free              },
  { -1, NULL }
};


/** Let the console handle allegro events. Returns 0 if event was consumed,
positive if not, and negative on error. */

int bbconsole_handle(BBWidget * widget, ALLEGRO_EVENT * event) { 
  if (!widget) return BBWIDGET_HANDLE_ERROR;
  if (!bbwidget_active(widget)) return BBWIDGET_HANDLE_IGNORE;
  return bbwidgetactions_handle(bbconsole_actions, event->type, widget, event);
}


/** Cleans up a console. */
int bbconsole_done(BBWidget * widget, void * data) {
  BBConsole * self = bbwidget_console(widget);
  Lilis * aid;
  if(!self) return BBWIDGET_HANDLE_IGNORE;
  self->buf     = mem_free(self->buf);
  ustr_free(self->input);
  self->input   = NULL;
  ustrlist_done(&self->text);
  return BBWIDGET_HANDLE_OK;
}


/** Deallocates a console. */
int bbconsole_free(BBWidget * widget, void * data) {
  BBConsole * self = bbwidget_console(widget);
  bbconsole_done(&self->widget, data);
  mem_free(self);
  return BBWIDGET_HANDLE_OK;
}

/** Allocates a console. */
BBConsole * bbconsole_alloc() {
  return STRUCT_ALLOC(BBConsole);
}

/* Amount of lines of display the console hat o keep track of. */
#define BBCONSOLE_MAX 200

/** Initializes a console. */
BBConsole * bbconsole_initall(BBConsole * self, int id, Rebox bounds, Style style) {
  if(!self) return NULL;
  if(!bbwidget_initall(&self->widget, id, bbconsole_actions, bounds, style)) { 
    return NULL;
  }
  ustrlist_init(&self->text);
  // ustrlist_shiftcstr(&self->text, "empty line");
  bbwidget_active_(&self->widget, FALSE);
  self->count = 0;
  // max MUST be at least 2, 3 to see anything...
  self->max   = BBCONSOLE_MAX;
  self->start = 0;
  self->charw = 80; 
  self->buf   = mem_alloc(self->charw + 1);
   // one extra for NULL at end . 
  if(!self->buf) { bbconsole_done(&self->widget, NULL); return NULL; }
  self->input = ustr_new("");
  self->cursor= 0;
  if(!self->input) { bbconsole_done(&self->widget, NULL); return NULL; }
  self->command      = NULL;
  self->command_data = NULL;
  return self;
}

/** Initializes a new console. */
BBConsole * bbconsole_new(int id, Rebox bounds, Style style) {
  BBConsole * self = bbconsole_alloc();
  if(!bbconsole_initall(self, id, bounds, style)) {
    bbconsole_free(&self->widget, NULL);
    return NULL;
  }
  return self;
}






