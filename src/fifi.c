#include <stdarg.h>
#include <allegro5/allegro_ttf.h>

#include "eruta.h"
#include "fifi.h"

/* Fifi contain functionality that helps finding back the file resouces,
such as images, music, etc that Eruta needs. 

An important concept here is the "virtual path", that is the path under the 
location of the data directory. So, for example, if the data of the 
app is installed on Linux in /usr/share/app/data,
then a vpath of 
font/my_nice_font.ttf 
will be resolved as 
/usr/share/app/data/font/my_nice_font.ttf.  
Or, if, on another OS, the data of the app is installed in 
C:\Program Files\App\data
then the same vpath will refer to
C:\Program Files\App\data\font\my_nice_font.ttf.

So Fifi is a nice way to get OS-independence and location-independence of 
the data files all at once. 

For save files or scores, the vpath is similar, but relative to the
"writeable" directory of the application.

*/

/*

Some tests with al_get_standard_path:
ALLEGRO_RESOURCES_PATH: /usr/local/var/arch/bjmey/src/eruta/bin/
This could be useful to look for the resouces, look in
ALLEGRO_RESOURCES_PATH/data
ALLEGRO_RESOURCES_PATH/share/data
ALLEGRO_RESOURCES_PATH/share
ALLEGRO_RESOURCES_PATH/../data
ALLEGRO_RESOURCES_PATH/../share/data
ALLEGRO_RESOURCES_PATH/../share

for the file eruta.txt to find back the resources.

Also it looks for:
ALLEGRO_RESOURCES_PATH/data.zip
ALLEGRO_RESOURCES_PATH/share/data.zip
ALLEGRO_RESOURCES_PATH/../data.zip
ALLEGRO_RESOURCES_PATH/../share/data.zip
/usr/local/var/arch/bjmey/src/eruta/../data
 
ALLEGRO_EXENAME_PATH: ~/src/eruta/bin/eruta
RESOURCES_PATH: /usr/local/var/arch/bjmey/dl/varia/allegro-5.0.5/examples/
TEMP_PATH: /tmp/
USER_DATA_PATH: /home/bjmey/.local/share/liballeg.org/ex_get_path/

*/

/** Helper for al_path_cstr. */
const char * fifi_path_cstr(Path * path) {
  if(!path) return NULL;
  return al_path_cstr(path, ALLEGRO_NATIVE_PATH_SEP);
}

/** Helper for al_filename_exists with a path. */
int fifi_path_exists(Path * path) {
  if(!path) return 0;
  return al_filename_exists(fifi_path_cstr(path));
}

/*
static ALLEGRO_PATH * fifi_try_path(ALLEGRO_PATH base, char * name) {
  al_

} 
*/

/* This is the path where the resources of Eruta are stored.  */
ALLEGRO_PATH * fifi_data_path_ = NULL;


/* Looks for the data folder in various places, return NULL if not found. */
static ALLEGRO_PATH * fifi_find_data_path(void) {
  ALLEGRO_PATH * path = al_get_standard_path(ALLEGRO_RESOURCES_PATH);
  // ALLEGRO_RESOURCES_PATH is normally where the program itself is installed.
  
  // ALLEGRO_RESOURCES_PATH/data (Windows-ish)
  al_append_path_component(path, "data");
  if(PATH_EXISTS(path)) return path;
  al_remove_path_component(path, -1);
  // ALLEGRO_RESOURCES_PATH/../data (Local install, linux-ish)
  al_replace_path_component(path, -1, "data");
  if(PATH_EXISTS(path)) return path;
  // ALLEGRO_RESOURCES_PATH/../share/eruta/data (global linux-ish install)
  al_replace_path_component(path, -1, "share");
  al_append_path_component(path, "eruta");
  al_append_path_component(path, "data");
  if(PATH_EXISTS(path)) return path;
  al_destroy_path(path);
  return NULL;
}


ALLEGRO_PATH * fifi_make_data_path(void) {
  ALLEGRO_PATH * path = al_get_standard_path(ALLEGRO_RESOURCES_PATH);
  al_replace_path_component(path, -1, "data");
  return path;
}

/** Initializes the file finding dir  */
ALLEGRO_PATH * fifi_init(void) {
  fifi_data_path_ = fifi_find_data_path();
  if (fifi_data_path_) { 
    printf("Data path: %s\n", PATH_CSTR(fifi_data_path_));
  } else {  
    puts("NULL data path!") ;
  }
  return fifi_data_path_ ;
}

/** Returns a pointer to the data path. Must be cloned before use.*/
ALLEGRO_PATH * fifi_data_path(void) {  
  return fifi_data_path_;
}

/** Returns a pointer to the data path converted to a c string. */
const char * fifi_data_path_cstr(void) {
  return al_path_cstr(fifi_data_path(), ALLEGRO_NATIVE_PATH_SEP);
}


/**
* Returns an ALLEGRO_PATH path concatenated with the 
* const char * strings in args, of which the last one should be NULL.
* modifies * path
*/
static ALLEGRO_PATH * path_append_va(ALLEGRO_PATH * path, va_list args) {
  const char * aid; 
   if(!path) return NULL;
  for(aid = va_arg(args, const char *); aid; aid = va_arg(args, const char *)){
    al_append_path_component(path, aid);
  }
  return path;
}  
       
/**
* Returns an ALLEGRO_PATH path concatednated with the 
* const char * strings, of which the last one should be NULL.
*/
static ALLEGRO_PATH * path_append(ALLEGRO_PATH * path, ...) {
  ALLEGRO_PATH * result;
  va_list args;
  va_start(args, path);
  result = path_append_va(path, args);
  va_end(args);
  return result;
}


/**
* helper like strncpy, but null terminates. Takes space and size
* amount is actual characters to copy , space is space available.
* Does nothing if space is less than 1.
*/
static char *help_strncpy(char * dest, const char * src, size_t amount, size_t space){
  if (space < 1) return NULL; 
  if (amount > (space-1)) amount = space - 1;
  strncpy(dest, src, amount);
  dest[amount] = '\0';
  return dest;
}


/**
* Helper function to split up character strings with a separator.
*/
static char * help_strsplit(const char * in, int ch, char * store, size_t space) {
  char * aid = strchr(in, ch);
  if (aid) {
    help_strncpy(store, in, aid - in, space);
  } else {   
    help_strncpy(store, in, strlen(in), space);
  }
  return aid;
}



#define PATH_APPEND_VPATH_SIZE 256
/**
* Returns an ALLEGRO_PATH path concatenated with the path in vpath.
* The const char * vpath is split on '/' characters, and the path
* is constructed like that. .. or . is not supported.
* if the last part contains a . it will be added as a file part.
**/
static ALLEGRO_PATH * path_append_vpath(ALLEGRO_PATH * path, const char * vpath) {
  char part[PATH_APPEND_VPATH_SIZE], * aid;
  if(!vpath) return path;
  aid = strchr(vpath, '/');
  aid = help_strsplit(vpath, '/', part, PATH_APPEND_VPATH_SIZE); 
  while(aid) {
    al_append_path_component(path, part);
    vpath = aid + 1;
    aid = help_strsplit(vpath, '/', part, PATH_APPEND_VPATH_SIZE); 
  }
  size_t len = strlen(part);
  if (len > 0) {
    if(strchr(part, '.')) { // it's a file name
      al_set_path_filename(path, part);
    } else {
      al_append_path_component(path, part);
    }
  }
  return path;
}


/** Returns a path to data which has the given virtual path.
*  You need to free the result with al_destroy_path.
*/
ALLEGRO_PATH * fifi_data_vpath(const char * vpath) {
  ALLEGRO_PATH * path = al_clone_path(fifi_data_path());
  return path_append_vpath(path, vpath);
}

/**
* Returns a path to data. Last of the arguments should be NULL.
* You need to free the result with al_destroy_path.
* returns NULL if no such file exists.
*/
ALLEGRO_PATH * fifi_data_pathva(const char * filename, va_list args) {
  ALLEGRO_PATH * path = al_clone_path(fifi_data_path());
  if(!path) return NULL;
  path_append_va(path, args);
  al_set_path_filename(path, filename);
  // printf("Loading: %s for %s\n", PATH_CSTR(path), filename);
  if(PATH_EXISTS(path)) { return path;  }
  // if we get here, we must destroy the path any way.
  al_destroy_path(path);
  return NULL;
}

/**
* Returns a path to data. Last of the arguments should be NULL.
* You need to free the result with al_destroy_path.
* returns NULL if no such file exists.
*/
ALLEGRO_PATH * fifi_data_pathargs(const char * filename, ...) {
  ALLEGRO_PATH * path;
  va_list args;
  va_start(args, filename);
  path        = fifi_data_pathva(filename, args);
  va_end(args);
  return path;
}



/**
* simple test function
*/
void fifi_data_vpath_print(const char * vpath) {
  ALLEGRO_PATH * path = fifi_data_vpath(vpath);
  printf("Fifi data vpath: %s\n", PATH_CSTR(path));
  al_destroy_path(path);
}


/**
* Loads file that is in the data directory using the given loader.
* returns NULL if the file doesn't exist or wasn't loaded correctly.
*/
void * fifi_load_vpath(FifiLoader * load, void * extra, const char * vpath) {
  void * data   = NULL;
  ALLEGRO_PATH * path;
  if(!load) return NULL;
  path          = fifi_data_vpath(vpath);
  if(!path) return NULL;
  if(!al_get_path_filename(path)) {
    printf("Filename not set for path: %s.\n", PATH_CSTR(path));
    goto cleanup;  
  }
  // printf("Loading: %s for %s\n", PATH_CSTR(path), vpath);
  if(PATH_EXISTS(path)) {
    data = load(extra, PATH_CSTR(path)); // load the data
  } else {
   printf("File %s does not exist!?", PATH_CSTR(path));
  }  
  cleanup:
  // if we get here, we must destroy the path any way.
  al_destroy_path(path);
  // return the data anyway.
  return data;
}



/**
* Loads file  that is in the data directory using the given loader.
* returns NULL if the file doesn't exist or wasn't loaded correctly.
*/
void * fifi_loadsimple_vpath(FifiSimpleLoader * load, const char * vpath) {
  void * data   = NULL;
  ALLEGRO_PATH * path;
  if(!load) return NULL;
  path          = fifi_data_vpath(vpath);
  if(!path) return NULL;
  if(!al_get_path_filename(path)) {
    printf("Filename not set for path: %s.\n", PATH_CSTR(path));
    goto cleanup;  
  }
  // printf("Loading: %s for %s\n", PATH_CSTR(path), vpath);
  if(PATH_EXISTS(path)) {
    data = load(PATH_CSTR(path)); // load the data
  } else {
   printf("File %s does not exist!?", PATH_CSTR(path));
  }  
  cleanup:
  // if we get here, we must destroy the path any way.
  al_destroy_path(path);
  // return the data anyway.
  return data;
}


/**
* Loads file  that is in the data directory using the given loader.
* returns NULL if the file doesn't exist or wasn't loaded correctly.
*/
void * fifi_loadsimple_va(FifiSimpleLoader * load, const char * filename, 
                          va_list args) {
  void * data   = NULL;
  ALLEGRO_PATH * path;
  if(!load) return NULL;
  path          = al_clone_path(fifi_data_path());
  if(!path) return NULL;
  path_append_va(path, args);
  al_set_path_filename(path, filename);
  // printf("Loading: %s for %s\n", PATH_CSTR(path), filename);
  if(PATH_EXISTS(path)) {
    data = load(PATH_CSTR(path)); // load the data
  } else {
    printf("File %s does not exist!?", PATH_CSTR(path));
  }
  // if we get here, we must destroy the path any way.
  al_destroy_path(path);
  // return the data anyway.
  return data;
}

/**
* Loads file  that is in the data directory using the given loader.
* the filename is specified first, then the directories under which 
* it should be, one by one, ended with NULL
* returns NULL if the file doesn't exist or wasn't loaded correctly.
*/
void * fifi_loadsimple(FifiSimpleLoader * load, const char * filename, ...) {
  void * result;
  va_list args;
  va_start(args, filename);
  result = fifi_loadsimple_va(load, filename, args);
  va_end(args);
  return result;
}


/**
* Loads a font from the data directory, with the given filename, size and 
* flags. No directory name is needed, the fonts must be in te data/font dir.
*/
ALLEGRO_FONT * fifi_loadfont(const char * filename, int size, int flags) {
  ALLEGRO_FONT * font = NULL;
  ALLEGRO_PATH * path;  
  path         = al_clone_path(fifi_data_path());
  if(!path) return NULL;
  path_append(path, "font", NULL);
  al_set_path_filename(path, filename);
  font = al_load_font(PATH_CSTR(path), size, flags);
  al_destroy_path(path);
  return font;
}

/**
* Loads a bitmap with the given filename under the NULL-terminated list 
* of subdirectory names
*/
ALLEGRO_BITMAP * fifi_loadbitmap_va(const char * filename, va_list args) {
  return fifi_loadsimple_va(
    (FifiSimpleLoader *)al_load_bitmap, filename, args);
}

/**
* Loads a bitmap with the given filename under the NULL-terminated list 
* of subdirectory names
*/
ALLEGRO_BITMAP * fifi_loadbitmap(const char * filename, ...) {
  void * result;
  va_list args;
  va_start(args, filename);
  result = fifi_loadsimple_va(
            (FifiSimpleLoader *)al_load_bitmap, filename, args);
  va_end(args);
  return result;
}

/**
* Loads a bitmap with the given vpath
*/
ALLEGRO_BITMAP * fifi_loadbitmap_vpath(const char * vpath) {  
  return fifi_loadsimple_vpath(
            (FifiSimpleLoader *)al_load_bitmap, vpath);
}



/** Loads an audio stream from the data directory. Since audi streams are usually music, no there's no nedto inicatet hedi, but all music must go under 
data/music */
ALLEGRO_AUDIO_STREAM * fifi_loadaudiostream(const char *filename,
                        size_t buffer_count, unsigned int samples) { 
  ALLEGRO_AUDIO_STREAM * result = NULL;
  ALLEGRO_PATH * path;  
  path         = al_clone_path(fifi_data_path());
  if(!path) return NULL;
  path_append(path, "music", NULL);
  al_set_path_filename(path, filename);
  result = al_load_audio_stream(PATH_CSTR(path), buffer_count, samples);
  al_destroy_path(path);
  return result;
}


#define DATA_PATH_BUFSIZE 1024
/**
* returns an ALLEGRO_PATH that points to the tileset image for the given
* file name. Must be destroyed after use. 
*/
ALLEGRO_PATH * fifi_tileset_filename(const char * name) {  
  ALLEGRO_PATH * path = al_clone_path(fifi_data_path());
  path_append(path, "map", NULL);
  al_set_path_filename(path, name);
  return path;
}



/** The follwoing loadable objects exist: 
* 1) maps
* 2) map scripts
* 3) tilesets
* 4) sound effects
* 4) music
* 5) tiles
* 6) gui elements
* 7) backgrounds
* 8) Sprites of course!
*/


/**
* Loads a TTF font from the data directory, with the given virtual path, width, height 
* and flags. 
*/
ALLEGRO_FONT * fifi_load_ttf_font_stretch(const char * vpath, int w, int h,  int flags) {
  ALLEGRO_FONT * font = NULL;
  ALLEGRO_PATH * path;  
  path         = fifi_data_vpath(vpath);
  if (!path) return NULL;
  font = al_load_ttf_font_stretch(PATH_CSTR(path), w, h, flags);
  al_destroy_path(path);
  return font;
}


/**
* Loads a TTF font from the data directory, with the given virtual path, height
* and flags. 
*/
ALLEGRO_FONT * fifi_load_ttf_font(const char * vpath, int h,  int flags) {
  ALLEGRO_FONT * font = NULL;
  ALLEGRO_PATH * path;  
  path         = fifi_data_vpath(vpath);
  if (!path) return NULL;
  font = al_load_ttf_font(PATH_CSTR(path), h, flags);
  al_destroy_path(path);
  return font;
}


/**
* Loads a bitmap font from the data directory, with the given virtual path, 
* and flags. 
*/
ALLEGRO_FONT * fifi_load_bitmap_font_flags(const char * vpath, int flags) {
  ALLEGRO_FONT * font = NULL;
  ALLEGRO_PATH * path;  
  path         = fifi_data_vpath(vpath);
  if (!path) return NULL;
  font = al_load_bitmap_font_flags(PATH_CSTR(path), flags);
  al_destroy_path(path);
  return font;
}


/**
* Loads a bitmap font from the data directory, with the given virtual path. 
*/
ALLEGRO_FONT * fifi_load_bitmap_font(const char * vpath) {
  ALLEGRO_FONT * font = NULL;
  ALLEGRO_PATH * path;  
  path         = fifi_data_vpath(vpath);
  if (!path) return NULL;
  font = al_load_bitmap_font(PATH_CSTR(path));
  al_destroy_path(path);
  return font;
}


/**
* Loads an audio stream from the data directory, with the given virtual path, 
* and parameters as per al_load_audio_stream. 
*/
ALLEGRO_AUDIO_STREAM * fifi_load_audio_stream(const char * vpath, size_t buffer_count, int samples) {
  ALLEGRO_AUDIO_STREAM * data = NULL;
  ALLEGRO_PATH        * path;  
  path         = fifi_data_vpath(vpath);
  if (!path) return NULL;
  data = al_load_audio_stream(PATH_CSTR(path), buffer_count, samples);
  al_destroy_path(path);
  return data;
}


/**
* Loads a sample from the data directory, with the given virtual path, 
* and parameters as per al_load_sample. 
*/
ALLEGRO_SAMPLE * fifi_load_sample(const char * vpath) {
  ALLEGRO_SAMPLE      * data = NULL;
  ALLEGRO_PATH        * path;  
  path         = fifi_data_vpath(vpath);
  if (!path) return NULL;
  data = al_load_sample(PATH_CSTR(path));
  al_destroy_path(path);
  return data;
}


/**
* Loads a bitmap from the data directory, with the given virtual path, 
* and parameters as per al_load_bitmap_flags. 
*/
ALLEGRO_BITMAP * fifi_load_bitmap_flags(const char * vpath, int flags) {
  ALLEGRO_BITMAP      * data = NULL;
  ALLEGRO_PATH        * path;  
  path         = fifi_data_vpath(vpath);
  if (!path) return NULL;
  data = al_load_bitmap_flags(PATH_CSTR(path), flags);
  al_destroy_path(path);
  return data;
}

/**
* Loads a bitmap from the data directory, with the given virtual path. 
*/
ALLEGRO_BITMAP * fifi_load_bitmap(const char * vpath) {
  return fifi_load_bitmap_flags(vpath, 0);
}








