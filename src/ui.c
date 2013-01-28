#include "eruta.h"
#include "image.h"
#include "ui.h"
#include "widget.h"

/*
* UI handles the user interface of Eruta.
* 
* There is only one top-level widget, however, every widget can contain
* any amout of child widgets. BBWidgets that are children of the same parent
* widget are said to be on the same level.
*
*
* The box model used in eruta is as follows:
*  ......................................
*  .margin                              .
*  .  +-------------border-----------+  .
*  .  | padding                      |  .
*  .  |  ..........................  |  .
*  .  |  .                        .  |  .
*  .  |  . contents               .  |  .
*  .  |  .                        .  |  .
*  .  |  ..........................  |  .
*  .  | padding                      |  .
*  .  +------------------------------+  .
*  .margin                              .
*  ......................................
*
* The padding determines the minimal distance between the border or the
* parent widget and it's contents and/or child widgets.
*
* The border's thickness is only relevant for visual effects. It does not change
* the layout. The border is effectively "inside" the padding of the widget.
*
* The margin of a widget determines how closely that widget may be packed
* to it's sibling widgets.
*
* The work in UI is divided between the UI and the BBWidget. The UI struct
* handles everything that depends on and/or may influence several widgets at
* once, such as event dispatching but also setting the focus, determining which
* widget is being hovered, or dragged, etc. The latter fuctions change the state
* of several widgets, so they are handled on the level of the system.
* The BBWidget class and it's child classes handle the individual state and
* actions of the various widgets individually.
*
*/

#include "ui.h"
#include "dynar.h"

#include <stdarg.h>


/** UI manages the graphical user interface and menus. */
struct UI_ {
  /* Ths UI is a widget itself. How meta that is! :) */
  BBWidget widget;
  /* ID generator. */
  int last_id; 
  /* The widgets in the UI, in a dynamic array. */
  Dynar * widgets;
};


/** Allocates an unitinialized UI. */
UI * ui_alloc() {
  return STRUCT_ALLOC(UI);
}





/** Draws the user interface */


/** Initializes the user interface. */

typedef void void_function(void); 

typedef void_function * void_function_ptr; 


/** Generic utility */

/** Helps to scan variable arguments with a format string, 
much like how scanf would work fro stdin. */
void vva_list_scan(va_list args, char * format, va_list data) {
  for( ; (*format); format++) {
    switch((*format)) {
      case 'p': (*va_arg(data, void**) ) = va_arg(args, void*); break;
      
      case 'd':
      case 'i': (*va_arg(data, int*))     = va_arg(args,   int); break;
      case 'o':
      case 'u': (*va_arg(data, unsigned int*))  
              = va_arg(args,   unsigned int); break;
      case 'l': (*va_arg(data, long*))    = va_arg(args, long); break;
      case 's': (*va_arg(data, char**))   = va_arg(args, char*); break;
      case 'f': (*va_arg(data, double*))  = va_arg(args, double); break;
      case 'F': (*va_arg(data, void_function_ptr*))
                = va_arg(args, void_function_ptr); break;
      default:
        break;
    }
  }
}

/** Helps to scan variable arguments with a format string, 
much like how scanf would work for stdin. */
void va_list_scan(va_list args, char * format, ...) {
  va_list data;
  va_start(data, format);
  vva_list_scan(args, format, data);
  va_end(data);
}

