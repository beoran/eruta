#ifndef fifi_H_INCLUDED
#define fifi_H_INCLUDED

/** FifiSimpleLoader is a generic loading function. Handy to redirect 
loading of files over fifi. */
typedef void * (FifiSimpleLoader)(const char * filename);

/* For cases when just the vpath isn't enough */
typedef void * (FifiLoader)(void * extra, const char * filename);

ALLEGRO_BITMAP *fifi_load_bitmap(const char *vpath);
ALLEGRO_BITMAP *fifi_load_bitmap_flags(const char *vpath,int flags);
ALLEGRO_SAMPLE *fifi_load_sample(const char *vpath);
ALLEGRO_AUDIO_STREAM *fifi_load_audio_stream(const char *vpath,size_t buffer_count,int samples);
ALLEGRO_FONT *fifi_load_bitmap_font(const char *vpath);
ALLEGRO_FONT *fifi_load_bitmap_font_flags(const char *vpath,int flags);
ALLEGRO_FONT *fifi_load_ttf_font(const char *vpath,int h,int flags);
ALLEGRO_FONT *fifi_load_ttf_font_stretch(const char *vpath,int w,int h,int flags);
ALLEGRO_PATH *fifi_tileset_filename(const char *name);
ALLEGRO_AUDIO_STREAM *fifi_loadaudiostream(const char *filename,size_t buffer_count,unsigned int samples);
ALLEGRO_BITMAP *fifi_loadbitmap_vpath(const char *vpath);
ALLEGRO_BITMAP *fifi_loadbitmap(const char *filename,...);
ALLEGRO_BITMAP *fifi_loadbitmap_va(const char *filename,va_list args);
ALLEGRO_FONT *fifi_loadfont(const char *filename,int size,int flags);
void *fifi_loadsimple(FifiSimpleLoader *load,const char *filename,...);
void *fifi_loadsimple_va(FifiSimpleLoader *load,const char *filename,va_list args);
void *fifi_loadsimple_vpath(FifiSimpleLoader *load,const char *vpath);
void *fifi_load_vpath(FifiLoader *load,void *extra,const char *vpath);
void fifi_data_vpath_print(const char *vpath);
ALLEGRO_PATH *fifi_data_pathargs(const char *filename,...);
ALLEGRO_PATH *fifi_data_pathva(const char *filename,va_list args);
ALLEGRO_PATH *fifi_data_vpath(const char *vpath);
const char *fifi_data_path_cstr(void);
ALLEGRO_PATH *fifi_data_path(void);
ALLEGRO_PATH *fifi_init(void);
ALLEGRO_PATH *fifi_make_data_path(void);
extern ALLEGRO_PATH *fifi_data_path_;
int fifi_path_exists(Path *path);
const char *fifi_path_cstr(Path *path);




#endif




