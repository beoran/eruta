#ifndef bbwidget_H_INCLUDED
#define bbwidget_H_INCLUDED

#include "eruta.h"
#include "image.h"
#include "rebox.h"

/* BBBBWidget, Beoran's Bad BBWidget set for allegro.  */

/* Custom action types compatible with Allegro events */

enum BBWidgetEventNumbers_ {
  BBWIDGET_EVENT_DRAW   =   AL_ID('B', 'W', 'I', 0),
  BBWIDGET_EVENT_UPDATE =   AL_ID('B', 'W', 'I', 1),
  BBWIDGET_EVENT_DONE   =   AL_ID('B', 'W', 'I', 2),
  BBWIDGET_EVENT_FREE   =   AL_ID('B', 'W', 'I', 3),
  BBWIDGET_EVENT_FOCUS  =   AL_ID('B', 'W', 'I', 4)
};




/* Style describes the style of a widget. */
struct Style_ {
  Color   forecolor;
  Color   backcolor;
  Font  * font;
  Image * background;
};

typedef struct Style_ Style;

/* Predefine BBWidget typedef. */
typedef struct BBWidget_ BBWidget;

/* Predefine BBConsole typedef. */
typedef struct BBConsole_ BBConsole;


/* Predefine widget method table. */
typedef struct BBWidgetMetab_ BBWidgetMetab;


/* Very simple array based event handler. It's O(N) for now,
 but N is very small here, so the simplicity of creating a method table 
 is more important. */
typedef struct BBWidgetAction_ BBWidgetAction;

typedef int BBWidgetHandler(BBWidget * widget, void * data);

struct BBWidgetAction_ { 
  int              type;
  BBWidgetHandler  * handler;
};


/* BBWidget flags. A widget can be : visible or not
* listening (accepting input) input or not (if it  invisble and not listening 
* input it's inactive, otherwise active) 
* it can have focus or not (no focus listens to input 
* but may ignore it, focus accepts input).
* Selected is for checkboxes that are selected or buttons that are down, etc...
* 
*/
enum BBWidgetFlags_ {
  BBWIDGET_FLAG_VISIBLE    = 1,
  BBWIDGET_FLAG_LISTENING  = 2,
  BBWIDGET_FLAG_ACTIVE     = 3,
  BBWIDGET_FLAG_FOCUSED    = 4,
  BBWIDGET_FLAG_SELECTED   = 8,
};

/* Widget state flags . */
typedef enum BBWidgetFlags_ BBWidgetFlags;



/* Flag testing macros. */

/* Checks if flag is set.
Flag will be evaluated 2 times so must be a constant or a variable that is 
only read. 
*/
#define BBWIDGET_FLAG_P(BBWIDGET, FLAG) (((BBWIDGET)->flag&(FLAG))==(FLAG))

#define BBWIDGET_VISIBLE_P(BBWIDGET, FLAG) (((BBWIDGET)->flag&(FLAG))==(FLAG))

#define BBWIDGET_HANDLE_OK      0
#define BBWIDGET_HANDLE_IGNORE  1
#define BBWIDGET_HANDLE_ERROR  -1

/* BBWidget method cache for commonly used methods */
struct BBWidgetMetab_ {
  BBWidgetHandler * free;
  BBWidgetHandler * done;
  BBWidgetHandler * draw;
  BBWidgetHandler * update;
};

/* BBWidget interface */
struct BBWidgetIfa_ {
  BBWidget      * self;
  BBWidgetMetab * metab;
};


/* BBWidgets are individual parts of the UI.  
As a simplification, BBWidgets are considered to occupy "panes" ordered 
in the user interface from back to front. They do not contain other widgets 
and do not have any generic relations between them.

A note on pointer ownership: the pointers to font and image in style
are NOT cleaned up, since style is intended to be mostly a shallow copy in which
font and background image are repeated many times.
*/
struct BBWidget_ {
  /* Event handler table. */
  BBWidgetAction  * acts;
  /* Method cache           */
  BBWidgetMetab     metab;
  /* Bounds, this is a rectangular box. */
  Rebox           bounds;
  /* Styling elements. */
  Style           style;
  /* BBWidget elements: */
  /* Unique ID. */
  int id;
  /* Flags (active, disabled, etc) */
  int flags;
  /* Priority of widget */
  int z;
};

// BBConsole input handler, called when a line of text (a command)
// is typed.
typedef int (BBConsoleCommand)
            (BBConsole * console, const char * command, void * extra);


/* This file was generated with:
'cfunctions -c -aoff -n -w bbwidget_proto src/widget.c' */
#ifndef CFH_BBWIDGET_PROTO
#define CFH_BBWIDGET_PROTO

/* From 'src/widget.c': */

Style style_make (Color fore , Color back , Font * font , Image * background );

Style * style_initstyle (Style * self , Style style );

Style * style_init (Style * self , Color fore , Color back , Font * font , Image * background );

Color style_forecolor (Style * self );

Color style_backcolor (Style * self );

Image * style_background (Style * self );

Font * style_font (Style * self );

Rebox bbwidget_bounds (BBWidget * self );

int bbwidget_w (BBWidget * self );

int bbwidget_h (BBWidget * self );

int bbwidget_x (BBWidget * self );

int bbwidget_y (BBWidget * self );

int bbwidget_z (BBWidget * self );

Color bbwidget_forecolor (BBWidget * self );

Color bbwidget_backcolor (BBWidget * self );

Font * bbwidget_font (BBWidget * self );

Image * bbwidget_background (BBWidget * self );

int bbwidget_flags (BBWidget * self );

int bbwidget_id (BBWidget * self , int id );

int bbwidget_flags_ (BBWidget * self , int flags );

int bbwidget_id_ (BBWidget * self , int id );

int bbwidget_flag (BBWidget * self , int flag );

int bbwidget_unflag (BBWidget * self , int flag );

int bbwidget_doflag (BBWidget * self , int flag , int set );

int bbwidget_flag_p (BBWidget * self , int flag );

int bbwidget_visible (BBWidget * self );

int bbwidget_listening (BBWidget * self );

int bbwidget_active (BBWidget * self );

int bbwidget_focused (BBWidget * self );

int bbwidget_selected (BBWidget * self );

int bbwidget_visible_ (BBWidget * self , int set );

int bbwidget_listening_ (BBWidget * self , int set );

int bbwidget_active_ (BBWidget * self , int set );

int bbwidget_focused_ (BBWidget * self , int set );

int bbwidget_selected_ (BBWidget * self , int set );

BBWidget * 
bbwidget_acts_(BBWidget * self, BBWidgetAction * acts);

BBWidget * 
bbwidget_initall(BBWidget * self,  int id, BBWidgetAction * acts, 
                 Rebox bounds, Style style);

BBWidget * 
bbwidget_initbounds(BBWidget * self, int id,  BBWidgetAction * acts, Rebox bounds);

BBWidget * 
bbwidget_initparent (BBWidget * self , int id , BBWidget * parent );

BBWidget * 
bbwidget_allocate(void);

BBWidget * 
bbwidget_done (BBWidget * widget );

BBWidget * 
bbwidget_free (BBWidget * self );

void bbwidget_draw (BBWidget * self );

void bbwidget_handle (BBWidget * self , ALLEGRO_EVENT * event );

void bbwidget_update (BBWidget * self , ALLEGRO_EVENT * event );

void bbwidget_drawroundframe (BBWidget * self );


int bbconsole_handle(BBWidget * widget, ALLEGRO_EVENT * event);

void 
bbconsole_command_(BBConsole * self , BBConsoleCommand * command , void * data );

int bbconsole_docommand (BBConsole * self , const char * text );

int bbconsole_addstr (BBConsole * self , const char * str );

int bbconsole_puts(BBConsole * self , const char * str );

int bbconsole_vprintf(BBConsole * self, const char * format, va_list args);

int bbconsole_printf(BBConsole * self, const char * format, ...);

int bbconsole_draw(BBWidget * widget, void * data);

void bbconsole_active_ (BBConsole * self , int active );

int bbconsole_active (BBConsole * self );

int bbconsole_scroll (BBConsole * self , int direction );

int bbconsole_done(BBWidget * widget, void * data );

int bbconsole_free(BBWidget * widget, void * data );

BBConsole * bbconsole_alloc(void);

BBConsole * bbconsole_initall (BBConsole * self , int id , Rebox bounds , Style style );

BBConsole * bbconsole_new (int id , Rebox bounds , Style style );

#endif /* CFH_BBWIDGET_PROTO */

typedef BBWidget * (BBWidgetDraw)(BBWidget * self);





#endif




