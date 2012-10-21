#ifndef widget_H_INCLUDED
#define widget_H_INCLUDED

#include "eruta.h"
#include "image.h"




/** Bounds is a bounds rectangle of a widget. */
struct Bounds_ { 
  Point p; // position
  Point s; // size
  int   z; // Priority.
};

typedef struct Bounds_ Bounds;

/** Style describes the style of a widget. */
struct Style_ {
  Color   forecolor;
  Color   backcolor;
  Font  * font;
  Image * background;
};

typedef struct Style_ Style;




/** Predefine Widget typedef. */
typedef struct Widget_ Widget;

/** Predefine Console typedef. */
typedef struct Console_ Console;


/** Predefine widget method table. */
typedef struct WidgetMetab_ WidgetMetab;

/** Predefine widget state flags . */
typedef enum WidgetFlags_ WidgetFlags;


/** Widget flags. A widget can be : visible or not
* listening (accepting input) input or not (if it  invisble and not listening 
* input it's inactive, otherwise active) 
* it can have focus or not (no focus listens to input 
* but may ignore it, focus accepts input).
* Selected is for checkboxes that are selected or buttons that are down, etc...
* 
*/
enum WidgetFlags {
  WIDGET_FLAG_VISIBLE    = 1,
  WIDGET_FLAG_LISTENING  = 2,
  WIDGET_FLAG_ACTIVE     = 3,
  WIDGET_FLAG_FOCUSED    = 4,
  WIDGET_FLAG_SELECTED   = 8,
};

/** Flag testing macros. */

/** Checks if flag is set.
Flag will be evaluated 2 times so must be a constant or a variable that is 
only read. 
*/
#define WIDGET_FLAG_P(WIDGET, FLAG) (((WIDGET)->flag&(FLAG))==(FLAG))

#define WIDGET_VISIBLE_P(WIDGET, FLAG) (((WIDGET)->flag&(FLAG))==(FLAG))

#define WIDGET_HANDLE_OK      0
#define WIDGET_HANDLE_IGNORE  1
#define WIDGET_HANDLE_ERROR  -1

/** Widget method table */
struct WidgetMetab_ {
  void *    (*free)   (void   * ptr);
  void *    (*done)   (void   * ptr);
  void      (*draw)   (Widget * self);
  int       (*handle) (Widget * self, ALLEGRO_EVENT * event);
  int       (*update) (Widget * self, double dt);
};

/** Widget interface */
struct WidgetIfa_ {
  Widget      * self;
  WidgetMetab * metab;
};


/** Widgets are individual parts of the UI.  
As a simplification, Widgets are considered to occupy "panes" ordered 
in the user intterface from back to front. They do not contain other widgets 
and do not have any generic relations between them.

A note on pointer ownership: the pointers to font and image in style
are NOT cleaned up, since style is intended to be mostly a shallow copy in which
font and background image are repeated many times.
*/
struct Widget_ {
  /* Methods           */
  WidgetMetab   * metab;
  /* Bounds */
  Bounds          bounds;
  /* Styling elements. */
  Style           style;
  /* Widget elements: */
  /* Unique ID. */
  int id;
  /* Flags (active, disabled, etc) */
  int flags;
};

// Console input handler, called when a line of text (a command)
// is typed.
typedef int (ConsoleCommand)
            (Console * console, const char * command, void * extra);


#include "widget_proto.h"

typedef Widget * (WidgetDraw)(Widget * self);





#endif




