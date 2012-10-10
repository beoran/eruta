#ifndef WIDGET_PROTO_H
#define WIDGET_PROTO_H
/*
This file was autogenerated from src/widget.c
by bin/genproto
Please do not hand edit.
*/

/** Makes a new bounds box struct. */
Bounds bounds_make(int x, int y, int w, int h);

/** Initializes a bounds pointer by copying data from a bounds struct.  */
Bounds * bounds_initbounds(Bounds * self, Bounds bounds);

/** Initializes a bounds pointer from it's coordinates. */
Bounds * bounds_init(Bounds * self, int x, int y, int w, int h);

/** Get x position of bounds. */
int bounds_x(Bounds * self);

/** Get y position of bounds. */
int bounds_y(Bounds * self);

/** Get width of bounds. */
int bounds_w(Bounds * self);

/** Get height of bounds. */
int bounds_h(Bounds * self);

/** Makes a new style struct. */
Style style_make(Color fore, Color back, Font * font, Image * background);

/** Initializes a style pointer by copying data from a style struct.  */
Style * style_initstyle(Style * self, Style style);

/** Initializes a bounds pointer from it's member data. */
Style * style_init( Style * self, 
                    Color fore, Color back, Font * font, Image * background);

/** Get foreground color of style. */
Color   style_forecolor(Style * self);  

/** Get background color of style. */
Color   style_backcolor(Style * self);  

/** Get background image of style. */
Image * style_background(Style * self);  

/** Get font of style. */
Font  * style_font(Style * self);  

/** Type and methods of the Widgets */
struct WidgetMethods_;
typedef struct WidgetMethods_ WidgetMethods;

/** Get bounds of widget. */
Bounds widget_bounds(Widget * self);

/** Get width of widget. */
int widget_w(Widget * self);  

/** Get height of widget. */
int widget_h(Widget * self);  

/** Get x position of widget. */
int widget_x(Widget * self);  

/** Get y position of widget. */
int widget_y(Widget * self);  

/** Get z position of widget. */
int widget_z(Widget * self);  

/** Get foreground color of widget. */
Color widget_forecolor(Widget * self);  

/** Get backgrund color of widget. */
Color widget_backcolor(Widget * self);  

/** Get font of widget. */
Font * widget_font(Widget * self);  

/** Get background bitmap of widget. */
Image * widget_background(Widget * self);

/** Initialzes a widget with given bounds and style. */
Widget * widget_initbounds(Widget * self, Bounds bounds);

/** Initialzes a widget from another one's bounds and style. */
Widget * widget_initparent(Widget * self,  Widget * parent);

/** Allocates a widget */
Widget * widget_allocate();

/** Call when widget is not needed anymore. */
Widget * widget_done(Widget * widget);

/** Frees a widget. Calls widget->methods->done(), then mem_free if
the latter returns not NULL. Returns NULL. */
Widget * widget_free(Widget * self);

/** Draws a rounded frame as background for a widget. */
void widget_drawroundframe(Widget * self);

/** 
* A label is simply a piece of text that is drawn at bounds.x, bounds.y
* using style.font in style.forecolor. No background is drawn.
*/
struct WidgetLabel_;
typedef struct WidgetLabel_ WidgetLabel;

/** Initializes a label. */
WidgetLabel * widgetlabel_init(WidgetLabel * self, Widget * parent, Bounds bounds, 
                          const char * text);

/** Cleans up a console */
Console * console_done(Console * self);

/** Deallocates a console. */
Console * console_free(Console * self);

/** Allocates a console. */
Console * console_alloc();

/** Initializes a console. */
Console * console_initall(Console * self, Bounds bounds, Style style);

/** Sets the console's command function and data. */
void console_command_(Console * self, ConsoleCommand * command, void * data);

/** Let the console perform a command if possible. returns nonzero on error,
zero if OK. */
int console_docommand(Console * self, char * text);

/** Initializes a console. */
Console * console_new(Bounds bounds, Style style);

/** Puts a string on the console .*/
int console_puts(Console * self, const char * str);

/** Draws a console. */
void console_draw(Console * self);

/** Activates or deactivates the console. */
void console_active_(Console * self, int active);

/** Returns nonzero if console is active zero if not. */
int console_active(Console * self);

/** scrolls the console 1 step in the given direction. */
int console_scroll(Console * self, int direction);

/** Let the console handle allegro events. Will return true if it was consumed,
false if not. */

int console_handle(Console * self, ALLEGRO_EVENT * event); 

#endif // WIDGET_PROTO_H

