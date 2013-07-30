#ifndef STORE_H_INCLUDED
#define STORE_H_INCLUDED

#include "resor.h"
#include "xresor.h"

int store_kind(int index);
Resor *store_load_bitmap(int index,const char *vpath);
Resor *store_load_bitmap_flags(int index,const char *vpath,int flags);
Resor *store_load_sample(int index,const char *vpath);
Resor *store_load_audio_stream(int index,const char *vpath,size_t buffer_count,int samples);
Resor *store_grab_font(int index,int bmp_index,int count,int ranges[]);
Resor *store_grab_font_from_resor(int index,Resor *resor,int count,int ranges[]);
Resor *store_load_bitmap_font(int index,const char *vpath);
Resor *store_load_bitmap_font_flags(int index,const char *vpath,int f);
Resor *store_load_ttf_font(int index,const char *vpath,int h,int f);
Resor *store_load_ttf_font_stretch(int index,const char *vpath,int w,int h,int f);
bool store_done();
Resor *store_put(int index,Resor *value);
bool store_drop(int index);
Resor *store_get(int index);
int store_max();
bool store_index_ok(int index);
bool store_init();

ALLEGRO_FONT    * store_get_font(int index);
ALLEGRO_BITMAP  * store_get_bitmap(int index);
ALLEGRO_SAMPLE  * store_get_sample(int index);
ALLEGRO_AUDIO_STREAM * store_get_audio_stream(int index);

bool store_get_bitmap_format(int index,int *value);
bool store_get_bitmap_flags(int index,int *value);
bool store_get_bitmap_height(int index,int *value);
bool store_get_bitmap_width(int index,int *value);
bool store_get_ustr_dimensions(int index,ALLEGRO_USTR *text,Rebox *value);
bool store_get_ustr_width(int index,ALLEGRO_USTR *text,int *value);
bool store_get_text_dimensions(int index,char *text,Rebox *value);
bool store_get_text_width(int index,char *text,int *value);
bool store_get_font_line_height(int index,int *value);
bool store_get_font_descent(int index,int *value);
bool store_get_font_ascent(int index,int *value);




#endif /* STORE_H_INCLUDED */ 


