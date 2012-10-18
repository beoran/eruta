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

// console input handler, called when a line of text (a command)
// is typed. 
typedef int (ConsoleCommand)(Console * console, char * command, void * extra);


#include "widget_proto.h"

typedef Widget * (WidgetDraw)(Widget * self);





#endif




