#ifndef widget_H_INCLUDED
#define widget_H_INCLUDED

#include "eruta.h"
#include "image.h"

/** Parent/child tree */
struct Pachi_ {
  void * parent;
  void * child;
  void * sibling;
};

typedef struct Pachi_ Pachi;

#define PACHI_DECLARE()     struct Pachi_ pachi_
#define PACHI_CHILD(OBJ)    ((OBJ).pachi_->child)
#define PACHI_PARENT(OBJ)   ((OBJ).pachi_->parent)
#define PACHI_SIBLING(OBJ)  ((OBJ).pachi_->sibling)

#define PACHI_CHILD_(OBJ, SET)    ((OBJ).pachi_->child    = (SET))
#define PACHI_PARENT_(OBJ, SET)   ((OBJ).pachi_->parent   = (SET))
#define PACHI_SIBLING_(OBJ), SET  ((OBJ).pachi_->sibling  = (SET))



/** Bounds is a bounds rectangle of a widget. */
struct Bounds_ { 
  Point p; // position
  Point s; // size
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


// console input handler, called when a line of text (a command)
// is typed. 
typedef int (ConsoleCommand)(void * extra, char * command);


/** Predefine Widget typedef. */
typedef struct Widget_ Widget;

#include "widget_proto.h"

typedef Widget * (WidgetDraw)(Widget * self);





#endif




