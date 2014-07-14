/**
* This is a test for scegra in $package$
*/
#include "si_test.h"
#include "scegra.h"



TEST_FUNC(scegra) {
  ScegraStyle style;
  float x, y, z;
  scegra_init();
  scegrastyle_initempty(&style);
  TEST_INTEQ(123, scegra_make_box(123, bevec(100,200), bevec(21,43), bevec(4, 7), style));
  TEST_INTEQ(321, scegra_make_text(321, bevec(200, 300), "Hello", style));
  TEST_INTEQ(501, scegra_make_image(501, bevec(200, 300), 77, style));
  scegra_speed(123, &x, &y);
  TEST_FLOATEQ(0.0, x);
  TEST_FLOATEQ(0.0, y);
  
  scegra_speed(123, &x, &y);
  TEST_FLOATEQ(0.0, x);
  TEST_FLOATEQ(0.0, y);
  
  
  scegra_done();
  
  TEST_DONE();
}

void scegra_update(double dt);

int scegra_make_box(int id, BeVec pos, BeVec siz, BeVec round, ScegraStyle style);


int scegra_z(int index);
int scegra_z_(int index, int z);

int scegra_make_text(int id, BeVec pos, const char * text, ScegraStyle style);
int scegra_make_image(int id, BeVec pos, int image_id, ScegraStyle style);

int scegra_visible_(int index,int is_visible);
int scegra_image_id_(int index,int rindex);
int scegra_font_id_(int index,int rindex);
int scegra_background_image_id_(int index,int rindex);
int scegra_speed_(int index,float x,float y);
int scegra_size_(int index,float w,float h);
int scegra_position_(int index,float x,float y);
int scegra_border_thickness_(int index,float t);
int scegra_background_color_(int index,int r,int g,int b,int a);
int scegra_border_color_(int index,int r,int g,int b,int a);
int scegra_color_(int index,int r,int g,int b,int a);
int scegra_z_(int index,int z);

int scegra_speed(int index, float * x, float * y);
int scegra_size(int index, float * w, float * h);
int scegra_position(int index, float *x, float * y);
int scegra_border_thickness(int index, float * t);
int scegra_background_color(int index, int * r, int * g,int * b,int * a);
int scegra_border_color(int index,int * r,int * g,int * b, int * a);
int scegra_color(int index, int * r, int * g, int * b,int * a);


int scegra_make_image(int id,BeVec pos,int image_id,ScegraStyle style);
int scegra_make_text(int id,BeVec pos,const char *text,ScegraStyle style);
int scegra_make_box(int id,BeVec pos,BeVec siz,BeVec round,ScegraStyle style);
int scegra_disable_node(int index);
int scegra_get_id(int index);
ScegraNode *scegra_get_node(int index);
int scegra_out_of_bounds(int index);
void scegra_draw();
void scegra_update(double dt);

void scegra_init();
int scegra_nodes_max();
ScegraStyle *scegrastyle_initempty(ScegraStyle *self);
void scegra_draw_image(ScegraNode *self);
void scegra_draw_text(ScegraNode *self);
void scegra_draw_box(ScegraNode *self);
void scegra_update_generic(ScegraNode *self,double dt);

int scegra_angle_(int index, float angle);

int scegra_make_box_style_from(int id, BeVec pos, BeVec siz, BeVec round, int sindex);
int scegra_make_text_style_from(int id, BeVec pos, const char * text, int sindex);
int scegra_make_image_style_from(int id, BeVec pos, int image_id, int sindex);

int scegra_image_flags_(int index, int flags);
int scegra_text_flags_(int index, int flags);

int scegra_show_system_mouse_cursor(int show);


int main(void) {
  TEST_INIT();  
  TEST_RUN(scegra);
  TEST_REPORT();
}



