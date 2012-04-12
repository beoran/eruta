
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

static ALLEGRO_PATH * fifi_make_data_path(void) {
  ALLEGRO_PATH * path = al_get_standard_path(ALLEGRO_RESOURCES_PATH);
  al_replace_path_component(path, -1, "data");
  return path;
}

/** Returns a pointer to the data path. Must be cloned before use.*/
ALLEGRO_PATH * fifi_data_path(void) {
  if (fifi_data_path_) return fifi_data_path_;
  fifi_data_path_ = fifi_make_data_path();
  return fifi_data_path_;
}

/** Returns a pointer to the data path converted to a c string. */
const char * fifi_data_path_cstr(void) {
  return al_path_cstr(fifi_data_path(), ALLEGRO_NATIVE_PATH_SEP);
}

#define DATA_PATH_BUFSIZE 1024
/**
* returns an ALLEGRO_PATH that pooints to the tileset image for the given
* file name. Must be destroyed after use. 
*/
ALLEGRO_PATH * fifi_tileset_filename(const char * name) {
  ALLEGRO_PATH * path = al_clone_path(fifi_data_path());
  al_append_path_component(path, "map");
  al_set_path_filename(path, name);
  return path;
}











