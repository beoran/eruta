
#include "eruta.h"
#include "widget.h"

/** Widgets are individual parts of the UI. Each widget can contain
sub-widgets.  */
struct Widget_ {
  /* Bounds */
  int x, y, h, w;
  int z;
  /* Related widgets. */
  Widget        * parent;     /* parent widget */
  Widget        * child;      /* child widget */
  Widget        * next;       /* next sibling, logical order  */
  Widget        * drawnext;   /* next sibling, drawing order  */

  /* Styling elements. */
  ALLEGRO_COLOR   forecolor;
  ALLEGRO_COLOR   backcolor;  
  ALLEGRO_FONT  * font;
  ALLEGRO_BITMAP* background;

  /* Status Flags. */
  int can_focus, focus, closed, hidden, active, mouse_over, mouse_down;
};












