#include <stdarg.h>

#include "eruta.h"
#include "fifi.h"

/* Fifi contain functionality that helps finding back the file resouces,
such as images, music, etc that Eruta needs. Also will have data file
handling support. */

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



 
ALLEGRO_EXENAME_PATH: /usr/local/var/arch/bjmey/src/eruta/bin/eruta

RESOURCES_PATH: /usr/local/var/arch/bjmey/dl/varia/allegro-5.0.5/examples/
TEMP_PATH: /tmp/
USER_DATA_PATH: /home/bjmey/.local/share/liballeg.org/ex_get_path/

*/

const char * fifi_path_cstr(ALLEGRO_PATH * path) {
  return al_path_cstr(path, ALLEGRO_NATIVE_PATH_SEP);
}

int fifi_path_exists(ALLEGRO_PATH * path) {
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


static ALLEGRO_PATH * fifi_make_data_path(void) {
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
ALLEGRO_PATH * path_append_va(ALLEGRO_PATH * path, va_list args) {
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
ALLEGRO_PATH * path_append(ALLEGRO_PATH * path, ...) {
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
char *help_strncpy(char * dest, const char * src, size_t amount, size_t space){
  if (space < 1) return NULL; 
  if (amount > (space-1)) amount = space - 1;
  strncpy(dest, src, amount);
  dest[amount] = '\0';
  return dest;
}


/**
* Helper function to split up character strings with a separator.
*/
char * help_strsplit(char * in, int ch, char * store, size_t space) {
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
ALLEGRO_PATH * path_append_vpath(ALLEGRO_PATH * path, const char * vpath) {
  char part[PATH_APPEND_VPATH_SIZE], * aid;
  char * done = strchr(vpath, '\0');
  aid = strchr(vpath, '/');
  aid = help_strsplit(vpath, '/', part, PATH_APPEND_VPATH_SIZE); 
  while(aid) {
    printf("append_vpath : %s\n", part);
    al_append_path_component(path, part);
    vpath = aid + 1;
    aid = help_strsplit(vpath, '/', part, PATH_APPEND_VPATH_SIZE); 
  }
  size_t len = strlen(part);
  if (len > 0) {
    printf("append_vpath last: %s\n", part);
    if(strchr(part, '.')) { // it's a file name
      al_set_path_filename(path, part);
    } else {     
      al_append_path_component(path, part);
    }  
  }
  return path;  
}


/** Returns a path to data which has the given virtual path.
*  you need to free this with al_destroy_path.
*/
ALLEGRO_PATH * fifi_data_vpath(const char * vpath) {
  ALLEGRO_PATH * path = al_clone_path(fifi_data_path());
  return path_append_vpath(path, vpath);
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
    printf("Filename not set for path: ", PATH_CSTR(path));
    goto cleanup;  
  }
  printf("Loading: %s for %s\n", PATH_CSTR(path), vpath);
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
  printf("Loading: %s for %s\n", PATH_CSTR(path), filename);
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
*/









