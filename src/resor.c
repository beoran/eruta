
/*
 * Resor: wrapper for loadable resources such as bitmaps, fonts, music,
 * etc. 
 * The main intent is to wrap this on the scripting side.
 * 
 */

#include "eruta.h"
#include "resor.h"
#include "mem.h"
#include "fifi.h"
#include "bad.h"
#include <rebox.h>
#include <assert.h>


/* typedef int ResorDestructor(Resor * self);  */

struct Resor_ { 
  ResorKind             kind; 
  ResorData             data;
  ResorStatus           status;
  ResorDestructor     * free;
};


static int resor_free_font(Resor * self) {
  ALLEGRO_FONT * data; 
  if (!self) return RESOR_NULL;
  assert(self->kind == RESOR_FONT);
  data = self->data.font;
  if (!data) return RESOR_EMPTY;
  al_destroy_font(data);
  self->data.font = NULL; 
  return RESOR_OK;
}  

static int resor_free_bitmap(Resor * self) {
  ALLEGRO_BITMAP * data; 
  if (!self) return RESOR_NULL;
  assert(self->kind == RESOR_BITMAP);
  data = self->data.bitmap;
  if (!data) return RESOR_EMPTY;
  al_destroy_bitmap(data);
  self->data.bitmap = NULL;
  return RESOR_OK;
}  

static int resor_free_sample(Resor * self) {
  ALLEGRO_SAMPLE * data; 
  if (!self) return RESOR_NULL;
  assert(self->kind == RESOR_SAMPLE);
  data = self->data.sample;
  if (!data) return RESOR_EMPTY;
  al_destroy_sample(data);
  self->data.sample = NULL; 
  return RESOR_OK;
}  

static int resor_free_audio_stream(Resor * self) {
  ALLEGRO_AUDIO_STREAM * data; 
  if (!self) return RESOR_NULL;
  assert(self->kind == RESOR_AUDIO_STREAM);
  data = self->data.stream;
  if (!data) return RESOR_EMPTY;
  al_destroy_audio_stream(data);
  self->data.stream = NULL; 
  return RESOR_OK;
}  

/* Cleans up the contents of the resource. */
int resor_done(Resor * self) {
  int res;
  if(!self) return RESOR_NULL;
  if(self->free) {
    res = self->free(self);
    self->kind = RESOR_NONE;
    return res;
  }
  return RESOR_OK;
}

/* Cleans up the contents of the resource and deallocates the memory self points to. */
int resor_free(Resor * self) {
  int res = resor_done(self);
  mem_free(self);
  return res;
}

/* Returns the resource kind.  */
int resor_kind(Resor * self) {
  if (!self) return RESOR_EMPTY;
  return self->kind;
}


/* Generic initialization for resor  */
Resor *  resor_init(Resor * self, ResorKind kind, ResorData data, ResorDestructor * free) {
  if(!self) return NULL;
  self->kind    = kind;
  self->data    = data;
  self->free    = free;
  self->status  = RESOR_OK;
  return self;
}


/* Allocate a resor struct */
Resor * resor_alloc() {
  return STRUCT_ALLOC(Resor);
}

/* Allocate and initialize a resor struct. */
Resor * resor_new(ResorKind kind, ResorData data, ResorDestructor * free) {
  Resor * self;
  self = resor_alloc();
  return resor_init(self, kind, data, free);
}

/* Allocate and initialize a resor struct for use with fonts. */
Resor * resor_new_font(ALLEGRO_FONT * font) {
  ResorData data;
  if(!font) return NULL;
  data.font = font;
  return resor_new(RESOR_FONT, data, resor_free_font);
}

/* Allocate and initialize a resor struct for use with bitmaps. */
Resor * resor_new_bitmap(ALLEGRO_BITMAP * bitmap) {
  ResorData data;
  if(!bitmap) return NULL;
  data.bitmap = bitmap;
  return resor_new(RESOR_BITMAP, data, resor_free_bitmap);
}

/* Allocate and initialize a resor struct for use with samples. */
Resor * resor_new_sample(ALLEGRO_SAMPLE * sample) {
  ResorData data;
  if (!sample) return NULL;
  data.sample = sample;
  return resor_new(RESOR_SAMPLE, data, resor_free_sample);
}

/* Allocate and initialize a resor struct for use with fonts. */
Resor * resor_new_audio_stream(ALLEGRO_AUDIO_STREAM * stream) {
  ResorData data;
  if(!stream) return NULL;
  data.stream = stream;
  return resor_new(RESOR_AUDIO_STREAM, data, resor_free_audio_stream);
}

/* Allocate and initialize a resor struct for use with other data . */
Resor * resor_new_other(ResorKind kind, void * data, ResorDestructor * free) {
  Resor * self;
  ResorData rdata;
  rdata.other = data;
  self = resor_alloc();
  return resor_init(self, kind, rdata, free);
}

/*
* Loads a TTF font from the data directory as a resource.
*/
Resor * resor_load_ttf_font_stretch(const char * vpath, int w, int h,  int flags) {  
  return resor_new_font(fifi_load_ttf_font_stretch(vpath, w, h, flags));
}


/**
* Loads a TTF font from the data directory as a resource.
*/
Resor * resor_load_ttf_font(const char * vpath, int h, int flags) {  
  return resor_new_font(fifi_load_ttf_font(vpath, h, flags));
}


/*
* Loads a bitmap font from the data directory as a resource 
*/
Resor * resor_load_bitmap_font_flags(const char * vpath, int flags) {  
  return resor_new_font(fifi_load_bitmap_font_flags(vpath, flags));
}

/*
* Loads a bitmap font from the data directory as a resource. 
*/
Resor * resor_load_bitmap_font(const char * vpath) {  
  return resor_new_font(fifi_load_bitmap_font(vpath));
}

/*
* Loads an audio stream from the data directory as a resource 
*/
Resor * 
resor_load_audio_stream(const char * vpath, size_t buffer_count, int samples) {  
  return resor_new_audio_stream(fifi_load_audio_stream(vpath, buffer_count, samples));
}

/*
* Loads a sample from the data directory as a resource. 
*/
Resor * resor_load_sample(const char * vpath) {  
  return resor_new_sample(fifi_load_sample(vpath));
}

/*
* Loads a bitmap from the data directory as a resource. 
*/
Resor * resor_load_bitmap_flags(const char * vpath, int flags) {  
  return resor_new_bitmap(fifi_load_bitmap_flags(vpath, flags));
}

/*
* Loads a bitmap from the data directory as a resource. 
*/
Resor * resor_load_bitmap(const char * vpath) {  
  return resor_new_bitmap(fifi_load_bitmap(vpath));
}

/* Loads an other resource using the loader callback */
Resor *
resor_load_other(const char *vpath, ResorKind kind, 
                 ResorLoader * loader, ResorDestructor * destroy, void *extra) {
  void * data = loader(vpath, extra);  
  if (!data) return NULL;
  return resor_new_other(kind, data, destroy);
}




/* Grabs a font from an ALLEGRO_BITMAP and puts it in a resor */
Resor * 
resor_grab_font_from_bitmap(ALLEGRO_BITMAP * bmp, int  count, int ranges[]) {
  return resor_new_font(al_grab_font_from_bitmap(bmp, count, ranges));  
}

/* Grabs a font from an Resor that must contain a bitmap and puts it in a resor */
Resor * 
resor_grab_font_from_resor(Resor * res, int  count, int ranges[]) {
  ALLEGRO_BITMAP * bmp;
  bmp = resor_bitmap(res);
  if (!bmp) return NULL;
  return resor_grab_font_from_bitmap(bmp, count, ranges);  
}


/* Gets the font stored in the resource. 
 * Returns NULL if not a font or not a valid resource.
 */
ALLEGRO_FONT * resor_font(Resor * self) {
  if(!self) return NULL;
  if(self->kind != RESOR_FONT) return NULL;
  return self->data.font;
}

/* Gets the bitmap stored in the resource. 
 * Returns NULL if not a font or not a valid resource.
 */
ALLEGRO_BITMAP * resor_bitmap(Resor * self) {
  if(!self) return NULL;
  if(self->kind != RESOR_BITMAP) return NULL;
  return self->data.bitmap;
}

/* Gets the audio stream stored in the resource. 
 * Returns NULL if not a font or not a valid resource.
 */
ALLEGRO_AUDIO_STREAM * resor_audio_stream(Resor * self) {
  if(!self) return NULL;
  if(self->kind != RESOR_AUDIO_STREAM) return NULL;
  return self->data.stream;
}

/* Gets the sample stored in the resource. 
 * Returns NULL if not a font or not a valid resource.
 */
ALLEGRO_SAMPLE * resor_sample(Resor * self) {
  if(!self) return NULL;
  if(self->kind != RESOR_SAMPLE) return NULL;
  return self->data.sample;
}

/** Gets the other data stored in resor, but checks the type. If type not correct returns NULL too. */
void * resor_other(Resor * self, unsigned kind) {
  if(!self) return NULL;
  if(self->kind != kind) return NULL;
  return self->data.other;
}



/* If the resource is a font, stores the ascent of the font into value 
 * and returns true . Otherwise, returns false
 */
bool resor_get_font_ascent(Resor * self, int * value) {
  ALLEGRO_FONT * font= resor_font(self);
  if (!font)  return false;
  if (!value) return false;
  (*value) = al_get_font_ascent(font);
  return true;
}

/* If the resource is a font, stores the descent of the font into value 
 * and returns true . Otherwise, returns false
 */
bool resor_get_font_descent(Resor * self, int * value) {
  ALLEGRO_FONT * font= resor_font(self);
  if (!font)  return false;
  if (!value) return false;
  (*value) = al_get_font_descent(font);
  return true; 
}

/* If the resource is a font, stores the line height of the font into value 
 * and returns true . Otherwise, returns false
 */
bool resor_get_font_line_height(Resor * self, int * value) {
  ALLEGRO_FONT * font = resor_font(self);
  if (!font)  return false;
  if (!value) return false;
  (*value) = al_get_font_line_height(font);
  return true;
}

/* If the resource is a font, stores the width of the given text in the font into 
 * value and returns true . Otherwise, returns false
 */
bool resor_get_text_width(Resor * self, char * text, int * value) {
  ALLEGRO_FONT * font = resor_font(self);
  if (!font)  return false;
  if (!value) return false;
  (*value) = al_get_text_width(font, text);
  return true;
}


/* If the resource is a font, stores the dimensions of the given text in the 
 * font into value and returns true. Otherwise, returns false
 */
bool resor_get_text_dimensions(Resor * self, char * text, Rebox * value) {
  int bbx = 0, bby = 0, bbw = 0, bbh = 0;
  ALLEGRO_FONT * font = resor_font(self);
  if (!font)  return false;
  if (!value) return false;
  al_get_text_dimensions(font, text, &bbx, &bby, &bbw, &bbh);
  value->at.x   = bbx;
  value->at.y   = bby;
  value->size.x = bbw;
  value->size.y = bbh;
  return true;
}


/* If the resource is a font, stores the width of the given text in the font into 
 * value and returns true . Otherwise, returns false
 */
bool resor_get_ustr_width(Resor * self, ALLEGRO_USTR * text, int * value) {
  ALLEGRO_FONT * font = resor_font(self);
  if (!font)  return false;
  if (!value) return false;
  (*value) = al_get_ustr_width(font, text);
  return true;
}


/* If the resource is a font, stores the dimensions of the given text in the 
 * font into value and returns true. Otherwise, returns false
 */
bool resor_get_ustr_dimensions(Resor * self, ALLEGRO_USTR * text, Rebox * value) {
  int bbx = 0, bby = 0, bbw = 0, bbh = 0;
  ALLEGRO_FONT * font = resor_font(self);
  if (!font)  return false;
  if (!value) return false;
  al_get_ustr_dimensions(font, text, &bbx, &bby, &bbw, &bbh);
  value->at.x   = bbx;
  value->at.y   = bby;
  value->size.x = bbw;
  value->size.y = bbh;
  return true;
}


/* If the resource is a bitmap, stores the width of the bitmap into 
 * value and returns true . Otherwise, returns false
 */
bool resor_get_bitmap_width(Resor * self, int * value) {
  ALLEGRO_BITMAP * bmp = resor_bitmap(self);
  if (!bmp)   return false;
  if (!value) return false;
  (*value) = al_get_bitmap_width(bmp);
  return true;
}


/* If the resource is a bitmap, stores the height of the bitmap into 
 * value and returns true . Otherwise, returns false
 */
bool resor_get_bitmap_height(Resor * self, int * value) {
  ALLEGRO_BITMAP * bmp = resor_bitmap(self);
  if (!bmp)   return false;
  if (!value) return false;
  (*value) = al_get_bitmap_height(bmp);
  return true;
}


/* If the resource is a bitmap, stores the flags of the bitmap into 
 * value and returns true . Otherwise, returns false
 */
bool resor_get_bitmap_flags(Resor * self, int * value) {
  ALLEGRO_BITMAP * bmp = resor_bitmap(self);
  if (!bmp)   return false;
  if (!value) return false;
  (*value) = al_get_bitmap_flags(bmp);
  return true;
}


/* If the resource is a bitmap, stores the format of the bitmap into 
 * value and returns true . Otherwise, returns false
 */
bool resor_get_bitmap_format(Resor * self, int * value) {
  ALLEGRO_BITMAP * bmp = resor_bitmap(self);
  if (!bmp)   return false;
  if (!value) return false;
  (*value) = al_get_bitmap_format(bmp);
  return true;
}





