
#include "eruta.h"
#include "store.h"


/** 
 * Store is a "global" resource container. It's simply an array of STORE_MAX 
 * resources. Rationale for using global state is that this container is a 
 * singleton for the whole application. The global state of the store isn't 
 * accessible directly, but only though safer accessor functions.
 */

#define STORE_MAX 30000

static  Resor * store_array[STORE_MAX];

/* Initialises the resource storage. */
bool store_init() {
  int index;
  for (index =0; index < STORE_MAX; index ++) { 
    store_array[index] = NULL;
  }
  return true;
}

/* Range check for the index. */
bool store_index_ok(int index) {
  if (index < 0)          return FALSE;
  if (index > STORE_MAX)  return FALSE;
  return TRUE;
}

/* Returns STORE_MAX */
int store_max() {
  return STORE_MAX;
}

/* Gets a resource with the given index. */
Resor * store_get(int index) {
  if(!store_index_ok(index)) return NULL;
  return store_array[index];
}

/* Puts a resource in the store without cleaning up what was there before. */
static Resor * store_put_raw(int index, Resor * value) {
  if(!store_index_ok(index)) return NULL;
  return store_array[index] = value;  
}

/* Puts a null without cleaning up what was there before. */
static Resor * store_put_null(int index) {
  return store_put_raw(index, NULL);
}

/* Drops the stores resouce with the given index. 
 * The resource will be cleaned up. 
 */
bool store_drop(int index) {
  bool res = false;
  Resor * old = store_get(index);
  if (old) {
    resor_free(old);
    res = true;
  } 
  store_put_null(index);
  return res;
}

/* Puts a resource in the store. Any old value at the same 
 * index  will be dropped first, unless if value is NULL. */
Resor * store_put(int index, Resor * value) {
  Resor * old; 
  if(!store_index_ok(index)) return NULL;
  store_drop(index);
  return store_put_raw(index, value);
}

/* Cleans up the store. */
bool store_done() {
  int index;
  for (index = 0; index < STORE_MAX; index ++) {
    store_drop(index);
  }
  return true;
}

/* Loads a font and puts it in the store.  */
Resor * 
store_load_ttf_font_stretch(int index, const char * vpath, int w, int h, int f) {  
  return store_put(index, resor_load_ttf_font_stretch(vpath, w, h, f));
}

/* Loads a font and puts it in the store.  */
Resor * 
store_load_ttf_font(int index, const char * vpath, int h, int f) {  
  return store_put(index, resor_load_ttf_font(vpath, h, f));
}


/* Loads a font and puts it in the store.  */
Resor * 
store_load_bitmap_font_flags(int index, const char * vpath, int f) {  
  return store_put(index, resor_load_bitmap_font_flags(vpath, f));
}


/* Loads a font and puts it in the store.  */
Resor * 
store_load_bitmap_font(int index, const char * vpath) {  
  return store_put(index, resor_load_bitmap_font(vpath));
}


/* Grabs a font from a bitmap and puts it in the store */
Resor * 
store_grab_font_from_resor(int index, Resor * resor, int count, int ranges[]) {
  return store_put(index, resor_grab_font_from_resor(resor, count, ranges));
} 

/* Grabs a font from a bitmap resouce at bmp_index and puts it in the store */
Resor * 
store_grab_font(int index, int bmp_index, int count, int ranges[]) {
  Resor * bmp_res; 
  bmp_res = store_get(index);
  if (!bmp_res) return NULL;
  return store_grab_font_from_resor(index, bmp_res, count, ranges);
} 

/* Loads an audio stream and puts it in the storage. */
Resor * 
store_load_audio_stream
(int index, const char * vpath, size_t buffer_count, int samples) {
  return store_put(index, resor_load_audio_stream(vpath, buffer_count, samples));
}

/* Loads a sample and puts it in the storage. */
Resor * 
store_load_sample
(int index, const char * vpath) {
  return store_put(index, resor_load_sample(vpath));
}

/* Loads a bitmap and puts it in the storage. */
Resor * 
store_load_bitmap_flags
(int index, const char * vpath, int flags) {
  return store_put(index, resor_load_bitmap_flags(vpath, flags));
}

/* Loads a bitmap and puts it in the storage. */
Resor * 
store_load_bitmap(int index, const char * vpath) {
  return store_put(index, resor_load_bitmap(vpath));
}


/* Loads "other" data and puts it in the storage. */
Resor * 
store_load_other(int index, const char* vpath, ResorKind kind, ResorLoader* loader, 
                 ResorDestructor* destroy, void* extra) {
  return store_put(index, resor_load_other(vpath, kind, loader, destroy, extra));
}

/* Loads a tile map and puts it in the storage. */
Resor * store_load_tilemap(int index, const char * vpath) {
  return store_put(index, xresor_load_tilemap(vpath, NULL));
}


/* Returns the kind of stored item. */
int store_kind(int index) {
  return resor_kind(store_get(index));
}

/* Returns the font stored at the index or nil if nothing there or not a font. */
ALLEGRO_FONT * store_get_font(int index) {
  return resor_font(store_get(index));
}

/* Returns the bitmap stored at the index or nil if nothing there or not a bitmap.*/
ALLEGRO_BITMAP * store_get_bitmap(int index) {
  return resor_bitmap(store_get(index));
}

/* Returns the sample stored at the index or nil if nothing there or not a sample.*/
ALLEGRO_SAMPLE * store_get_sample(int index) {
  return resor_sample(store_get(index));
}

/* Returns the audio stream stored at the index or nil if nothing there or not a 
 * sample.*/
ALLEGRO_AUDIO_STREAM * store_get_audio_stream(int index) {
  return resor_audio_stream(store_get(index));
}

/* Returns an "other" type of data from storage. */
void * store_get_other(int index, unsigned kind) {
  return resor_other(store_get(index), kind);
}


bool store_get_bitmap_format(int index, int *value) {
  return resor_get_bitmap_format(store_get(index), value);
}

bool store_get_bitmap_flags(int index,int *value) {
  return resor_get_bitmap_flags(store_get(index), value);
}

bool store_get_bitmap_height(int index,int *value) {
  return resor_get_bitmap_height(store_get(index), value);  
}


bool store_get_bitmap_width(int index,int *value) {
  return resor_get_bitmap_width(store_get(index), value);  
}


bool store_get_ustr_dimensions(int index,ALLEGRO_USTR *text, Rebox *value) {
  return resor_get_ustr_dimensions(store_get(index), text, value);  
}

bool store_get_ustr_width(int index,ALLEGRO_USTR *text, int *value) {
  return resor_get_ustr_width(store_get(index), text, value);  
}


bool store_get_text_dimensions(int index, char *text, Rebox *value) {
  return resor_get_text_dimensions(store_get(index), text, value);  
}

bool store_get_text_width(int index, char *text, int *value) {
  return resor_get_text_width(store_get(index), text, value);  
}

bool store_get_font_ascent(int index, int * value) {
  return resor_get_font_ascent(store_get(index), value);  
}

bool store_get_font_descent(int index, int * value) {
  return resor_get_font_descent(store_get(index), value);  
}

bool store_get_font_line_height(int index, int * value) {
  return resor_get_font_line_height(store_get(index), value);  
}
  

/* Returns the first unused store ID larger than minimum. */
int store_get_unused_id(int minimum) {
  int index, stop;
  if (minimum < 0) return -2;
  stop = store_max();
  for (index = minimum; index < stop; index++) {
    Resor * resource =  store_get(index);
    if (!resource) {
      return index;
    }
  }
  return -3;
}



