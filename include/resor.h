#ifndef RESOR_H_INCLUDED
#define RESOR_H_INCLUDED 

#include "rebox.h"

enum ResorKind_ {
  RESOR_NONE          = 0,
  RESOR_BITMAP        = 1, 
  RESOR_FONT          = 2, 
  RESOR_AUDIO_STREAM  = 3, 
  RESOR_SAMPLE        = 4,
  RESOR_CONFIG        = 5,
  RESOR_OTHER         = 8,
  RESOR_TILEMAP       = 9,
};

enum ResorStatus_ {
  RESOR_OK            = 0,
  RESOR_EMPTY         = 1,
  RESOR_ENOENT        = 2,
  RESOR_ENOMEM        = 3,
  RESOR_NULL          = 4,
};


typedef enum ResorKind_         ResorKind;
typedef enum ResorStatus_       ResorStatus;
typedef union ResorData_        ResorData;
typedef struct Resor_           Resor;

union ResorData_ { 
  ALLEGRO_BITMAP        * bitmap;
  ALLEGRO_FONT          * font;
  ALLEGRO_SAMPLE        * sample;
  ALLEGRO_AUDIO_STREAM  * stream;
  ALLEGRO_CONFIG        * config;
  void                  * other;
};


typedef int ResorDestructor(Resor * self); 
typedef void* ResorLoader(const char * vpath, void * extra); 

ALLEGRO_SAMPLE *resor_sample(Resor *self);
ALLEGRO_AUDIO_STREAM *resor_audio_stream(Resor *self);
ALLEGRO_BITMAP *resor_bitmap(Resor *self);
ALLEGRO_FONT *resor_font(Resor *self);
void * resor_other(Resor * self, unsigned kind);

Resor * 
resor_grab_font_from_resor(Resor * res, int  count, int ranges[]);

Resor *resor_load_bitmap(const char *vpath);
Resor *resor_load_bitmap_flags(const char *vpath,int flags);
Resor *resor_load_sample(const char *vpath);
Resor *resor_load_audio_stream(const char *vpath,size_t buffer_count,int samples);
Resor *resor_load_bitmap_font(const char *vpath);
Resor *resor_load_bitmap_font_flags(const char *vpath,int flags);
Resor *resor_load_ttf_font(const char *vpath,int h,int flags);
Resor *resor_load_ttf_font_stretch(const char *vpath,int w,int h,int flags);

Resor *
resor_load_other(const char *vpath, ResorKind kind, 
                 ResorLoader * loader, ResorDestructor * destroy, void *extra);

Resor *resor_new_audio_stream(ALLEGRO_AUDIO_STREAM *stream);
Resor *resor_new_sample(ALLEGRO_SAMPLE *sample);
Resor *resor_new_bitmap(ALLEGRO_BITMAP *bitmap);
Resor *resor_new_font(ALLEGRO_FONT *font);
Resor *resor_new(ResorKind kind,ResorData data,ResorDestructor *free);
Resor *resor_new_other(ResorKind kind, void * data, ResorDestructor * free);

Resor *resor_alloc();
Resor *resor_init(Resor *self,ResorKind kind,ResorData data,ResorDestructor *free);
int resor_kind(Resor *self);
int resor_free(Resor *self);
int resor_done(Resor *self);

bool resor_get_bitmap_format(Resor *self,int *value);
bool resor_get_bitmap_flags(Resor *self,int *value);
bool resor_get_bitmap_height(Resor *self,int *value);
bool resor_get_bitmap_width(Resor *self,int *value);
bool resor_get_ustr_dimensions(Resor *self,ALLEGRO_USTR *text,Rebox *value);
bool resor_get_ustr_width(Resor *self,ALLEGRO_USTR *text,int *value);
bool resor_get_text_dimensions(Resor *self,char *text,Rebox *value);
bool resor_get_text_width(Resor *self,char *text,int *value);
bool resor_get_font_line_height(Resor *self,int *value);
bool resor_get_font_descent(Resor *self,int *value);
bool resor_get_font_ascent(Resor *self,int *value);




#endif /* RESOR_H_INCLUDED */

