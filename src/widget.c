
#include "eruta.h"
#include "mem.h"
#include "str.h"
#include "image.h"
#include "dynar.h"
#include "widget.h"

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

typedef struct WidgetMethods_ WidgetMethods;

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


struct WidgetBox_ {
  struct Widget_ widget;
};


/** 
* A label is simply a piece of text that is drawn at bounds.x, bounds.y
* using style.font in style.forecolor. No background is drawn.
*/
struct WidgetLabel_ {
  Widget        parent;
  STR         * text;
};

/** Initializes a label. */
WidgetLabel * widgetlabel_init(WidgetLabel * self, Widget * parent, Bounds bounds, 
                          const char * text) {
  if(!widget_initbounds((Widget *)self, bounds)) return NULL;
  self->text = str_new(text);
  return self;
}

WidgetLabel * widgetlabel_done(WidgetLabel * self) {
  if(!self) return NULL;
  str_free(self->text);
  
  return NULL;
} 



struct WidgetChoose_ {
  Widget parent;
  Dynar * options;
}; 








