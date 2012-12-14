#ifndef fifi_H_INCLUDED
#define fifi_H_INCLUDED

/** FifiSimpleLoader is a generic loading function. Handy to redirect 
loading of files over fifi. */
typedef void * (FifiSimpleLoader)(const char * filename);

/* This file was generated with:
'cfunctions -c -aoff -n -w fifi_proto src/fifi.c' */
#ifndef CFH_FIFI_PROTO
#define CFH_FIFI_PROTO

/* From 'src/fifi.c': */

const char * fifi_path_cstr (Path * path );

int fifi_path_exists (Path * path );

extern ALLEGRO_PATH * fifi_data_path_;

ALLEGRO_PATH * fifi_make_data_path (void);

ALLEGRO_PATH * fifi_init (void);

ALLEGRO_PATH * fifi_data_path (void);

const char * fifi_data_path_cstr (void);

ALLEGRO_PATH * path_append_va (ALLEGRO_PATH * path , va_list args );

ALLEGRO_PATH * path_append (ALLEGRO_PATH * path , ... );

char * help_strncpy (char * dest , const char * src , size_t amount , size_t space );

char * help_strsplit (const char * in , int ch , char * store , size_t space );

ALLEGRO_PATH * path_append_vpath (ALLEGRO_PATH * path , const char * vpath );

ALLEGRO_PATH * fifi_data_vpath (const char * vpath );

ALLEGRO_PATH * fifi_data_pathva (const char * filename , va_list args );

ALLEGRO_PATH * fifi_data_pathargs (const char * filename , ... );

void fifi_data_vpath_print (const char * vpath );

void * fifi_loadsimple_vpath (FifiSimpleLoader * load , const char * vpath );

void * fifi_loadsimple_va (FifiSimpleLoader * load , const char * filename , va_list args );

void * fifi_loadsimple (FifiSimpleLoader * load , const char * filename , ... );

ALLEGRO_FONT * fifi_loadfont (const char * filename , int size , int flags );

ALLEGRO_BITMAP * fifi_loadbitmap_va (const char * filename , va_list args );

ALLEGRO_BITMAP * fifi_loadbitmap (const char * filename , ... );

ALLEGRO_BITMAP * fifi_loadbitmap_vpath (const char * vpath );

ALLEGRO_AUDIO_STREAM * fifi_loadaudiostream (const char *filename , size_t buffer_count , unsigned int samples );

ALLEGRO_PATH * fifi_tileset_filename (const char * name );

#endif /* CFH_FIFI_PROTO */


#endif




