
#include "eruta.h"
#include "toruby.h"
#include "rh.h"
#include "state.h"
#include "image.h"
#include "fifi.h"
#include "store.h"
#include "scegra.h"
#include "sound.h"
#include <mruby/hash.h>
#include <mruby/class.h>
#include <mruby/data.h>
#include <mruby/array.h>
#include "tr_macro.h"
#include "tr_graph.h"


TORUBY_0_ICALLER(tr_scegra_nodes_max, scegra_nodes_max)

SCEGRA_ICALLER(tr_scegra_z, scegra_z)
SCEGRA_ICALLER(tr_scegra_disable_node, scegra_disable_node)
SCEGRA_ICALLER(tr_scegra_get_id, scegra_get_id)
SCEGRA_ICALLER(tr_scegra_out_of_bounds, scegra_out_of_bounds)

SCEGRA_ISETTER(tr_scegra_z_, scegra_z_)
SCEGRA_ISETTER(tr_scegra_visible_, scegra_visible_)
SCEGRA_ISETTER(tr_scegra_image_id_, scegra_image_id_)
SCEGRA_ISETTER(tr_scegra_font_id_, scegra_font_id_)
SCEGRA_ISETTER(tr_scegra_background_image_id_, scegra_background_image_id_)
SCEGRA_ISETTER(tr_scegra_border_thickness_, scegra_border_thickness_)
SCEGRA_ISETTER(tr_scegra_margin_, scegra_margin_)

SCEGRA_PSETTER(tr_scegra_speed_, scegra_speed_)
SCEGRA_PSETTER(tr_scegra_size_ , scegra_size_)
SCEGRA_PSETTER(tr_scegra_position_, scegra_position_)
  
SCEGRA_CSETTER(tr_scegra_background_color_, scegra_background_color_)
SCEGRA_CSETTER(tr_scegra_border_color_, scegra_border_color_)
SCEGRA_CSETTER(tr_scegra_color_, scegra_color_)
  
SCEGRA_FSETTER(tr_scegra_angle_, scegra_angle_)

static mrb_value tr_scegra_make_box(mrb_state * mrb, mrb_value self) {
  mrb_int id            = -1, sindex = -1;
  mrb_int x             =  0, y      =  0;
  mrb_int w             = 32, h      = 32;
  mrb_int rx            =  4, ry     =  4;  
  mrb_get_args(mrb, "iiiiiiii", &id, &x, &y, &w, &h, &rx, &ry, &sindex);
  return mrb_fixnum_value(scegra_make_box_style_from(id, bevec(x, y), bevec(w, h), bevec(rx, ry), sindex));
}

static mrb_value tr_scegra_make_text(mrb_state * mrb, mrb_value self) {
  char * str            = NULL;
  mrb_int id            = -1, size   =  0, sindex = -1;
  mrb_int x             =  0, y      =  0;
  mrb_int w             = 64, h      = 32;
  mrb_get_args(mrb, "iiiiisi", &id, &x, &y, &w, &h, &str, &size, &sindex);
  return mrb_fixnum_value(scegra_make_text_style_from(id, bevec(x, y), bevec(w, h), str, sindex));
}

static mrb_value tr_scegra_make_longtext(mrb_state * mrb, mrb_value self) {
  char * str            = NULL;
  mrb_int id            = -1, size   =  0, sindex = -1;
  mrb_int x             =  0, y      =  0;
  mrb_int w             = 64, h      = 32;
  mrb_get_args(mrb, "iiiiisi", &id, &x, &y, &w, &h, &str, &size, &sindex);
  return mrb_fixnum_value(scegra_make_longtext_style_from(id, bevec(x, y), bevec(w, h), str, sindex));
}


static mrb_value tr_scegra_make_image(mrb_state * mrb, mrb_value self) {
  char * str            = NULL;
  mrb_int id            = -1, image_id  =  -1, sindex = -1;
  mrb_int x             =  0, y      =  0;
  mrb_int w             =  0, h      = 0;  
  mrb_get_args(mrb, "iiiiiii", &id, &x, &y, &w, &h, &image_id, &sindex);
  return mrb_fixnum_value(scegra_make_image_style_from(id, bevec(x, y),  bevec(w, h), image_id, sindex));
}



TR_WRAP_II_INT(tr_scegra_image_flags_ ,  scegra_image_flags_)
TR_WRAP_II_INT(tr_scegra_text_flags_  ,  scegra_text_flags_)
TR_WRAP_IZ_INT(tr_scegra_text_        ,  scegra_text_)

SCEGRA_ISETTER(tr_scegra_line_stop_   , scegra_line_stop_)
SCEGRA_ISETTER(tr_scegra_line_start_  , scegra_line_start_)
SCEGRA_FSETTER(tr_scegra_delay_       , scegra_delay_)

SCEGRA_ICALLER(tr_scegra_line_stop     , scegra_line_stop)
SCEGRA_ICALLER(tr_scegra_line_start    , scegra_line_start)
SCEGRA_FCALLER(tr_scegra_delay         , scegra_delay)

TR_PAIR_DO(SCEGRA_ISETTER, scegra_page_lines_)
TR_PAIR_DO(SCEGRA_ICALLER, scegra_page_lines)
TR_PAIR_DO(SCEGRA_BSETTER, scegra_paused_)

TR_PAIR_DO(SCEGRA_BCALLER, scegra_paused)
TR_PAIR_DO(SCEGRA_ISETTER, scegra_page_)
TR_PAIR_DO(SCEGRA_ICALLER, scegra_page)
TR_PAIR_DO(TR_WRAP_I_INT, scegra_next_page)
TR_PAIR_DO(TR_WRAP_I_INT, scegra_previous_page)



/** Initialize mruby bindings to 2D scene graph functionality.
 * Eru is the parent module, which is normally named "Eruta" on the
 * ruby side. */
int tr_graph_init(mrb_state * mrb, struct RClass * eru) {
  struct RClass * gra;
  /* graph class/module and class/module methods. */
  gra = mrb_define_class_under(mrb, eru, "Graph" , mrb->object_class);

    TR_CLASS_METHOD_ARGC(mrb, gra, "nodes_max"       , tr_scegra_nodes_max, 0);
  TR_CLASS_METHOD_ARGC(mrb, gra, "z"               , tr_scegra_z, 1);
  TR_CLASS_METHOD_ARGC(mrb, gra, "disable"         , tr_scegra_disable_node, 1);
  TR_CLASS_METHOD_ARGC(mrb, gra, "id"              , tr_scegra_get_id, 1);
  TR_CLASS_METHOD_ARGC(mrb, gra, "out_of_bounds?"  , tr_scegra_out_of_bounds, 1);
  
  TR_CLASS_METHOD_ARGC(mrb, gra, "z_"               , tr_scegra_z_, 2);
  TR_CLASS_METHOD_ARGC(mrb, gra, "visible_"         , tr_scegra_visible_, 2);
  TR_CLASS_METHOD_ARGC(mrb, gra, "image_"           , tr_scegra_image_id_, 2);
  TR_CLASS_METHOD_ARGC(mrb, gra, "font_"            , tr_scegra_font_id_, 2);
  TR_CLASS_METHOD_ARGC(mrb, gra, "background_image_", tr_scegra_background_image_id_, 2);
  TR_CLASS_METHOD_ARGC(mrb, gra, "border_thickness_", tr_scegra_border_thickness_, 2);
  TR_CLASS_METHOD_ARGC(mrb, gra, "margin_"          , tr_scegra_margin_, 2);
  /*TR_CLASS_METHOD_ARGC(mrb, gra, "speed"          , tr_scegra_speed, 1);*/
  TR_CLASS_METHOD_ARGC(mrb, gra, "size_"            , tr_scegra_size_, 3);
  TR_CLASS_METHOD_ARGC(mrb, gra, "position_"        , tr_scegra_position_, 3);
  TR_CLASS_METHOD_ARGC(mrb, gra, "speed_"           , tr_scegra_speed_, 3);
  TR_CLASS_METHOD_ARGC(mrb, gra, "text_"            , tr_scegra_text_, 2); 

  
  TR_CLASS_METHOD_ARGC(mrb, gra, "image_flags_"     , tr_scegra_image_flags_, 2);
  TR_CLASS_METHOD_ARGC(mrb, gra, "text_flags_"      , tr_scegra_text_flags_ , 2);
  TR_CLASS_METHOD_ARGC(mrb, gra, "angle_"           , tr_scegra_angle_, 2);
  TR_CLASS_METHOD_ARGC(mrb, gra, "background_color_", tr_scegra_background_color_, 4);
  TR_CLASS_METHOD_ARGC(mrb, gra, "border_color_"    , tr_scegra_border_color_, 4);
  TR_CLASS_METHOD_ARGC(mrb, gra, "color_"           , tr_scegra_color_       , 4);
  TR_CLASS_METHOD_ARGC(mrb, gra, "make_box"         , tr_scegra_make_box,  8); 
  TR_CLASS_METHOD_ARGC(mrb, gra, "make_image"       , tr_scegra_make_image, 5); 
  TR_CLASS_METHOD_ARGC(mrb, gra, "make_text"        , tr_scegra_make_text, 5); 
  TR_CLASS_METHOD_ARGC(mrb, gra, "make_longtext"    , tr_scegra_make_longtext, 7);

  TR_CLASS_METHOD_ARGC(mrb, gra, "line_stop_" , tr_scegra_line_stop_, 2);
  TR_CLASS_METHOD_ARGC(mrb, gra, "line_start_", tr_scegra_line_start_, 2);
  TR_CLASS_METHOD_ARGC(mrb, gra, "delay_"     , tr_scegra_delay_, 2);

  TR_CLASS_METHOD_ARGC(mrb, gra, "line_stop"  , tr_scegra_line_stop, 1);
  TR_CLASS_METHOD_ARGC(mrb, gra, "line_start" , tr_scegra_line_start, 1);
  TR_CLASS_METHOD_ARGC(mrb, gra, "delay"      , tr_scegra_delay, 1);

  /* Long text paging. */ 
  TR_CLASS_METHOD_ARGC(mrb, gra, "page_lines_", tr_scegra_page_lines_, 2);
  TR_CLASS_METHOD_ARGC(mrb, gra, "page_lines" , tr_scegra_page_lines, 1);
  TR_CLASS_METHOD_ARGC(mrb, gra, "paused_"    , tr_scegra_paused_, 2);
  TR_CLASS_METHOD_ARGC(mrb, gra, "paused"     , tr_scegra_paused, 1);
  TR_CLASS_METHOD_ARGC(mrb, gra, "page_"      , tr_scegra_page_, 2);
  TR_CLASS_METHOD_ARGC(mrb, gra, "page"       , tr_scegra_page, 1);
  TR_CLASS_METHOD_ARGC(mrb, gra, "next_page"  , tr_scegra_next_page, 1);
  TR_CLASS_METHOD_ARGC(mrb, gra, "previous_page"  , tr_scegra_previous_page, 1);  

  return 0;
}
  





