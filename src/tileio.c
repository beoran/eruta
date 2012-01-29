#include <mxml.h>
#include "tile.h"
#include "tilepane.h"
#include "tilemap.h"
#include "tileio.h"

/* Shorthand for mxml_node_t */
#define XML mxml_node_t

/* Functions for loading and saving tile maps, grouped here to limit the use
of xml, etc. */

int mxmlElementGetAttrInt(mxml_node_t * node, const char * name) {
   const char * attr =  mxmlElementGetAttr(node, name);
   if(!attr) return -0xbad;
   return atoi(attr);
}

const char * mxmlFindPathGetAttr
            (XML * xml, const char * path, const char * name) {
  XML * node  = mxmlFindPath(xml, path);
  if(!node) return NULL;
  return mxmlElementGetAttr(node, name);
}

int mxmlFindPathGetAttrInt(XML * xml, const char * path, const char * name) {
  XML * node  = mxmlFindPath(xml, path);  
  return mxmlElementGetAttrInt(node, name);  
}

XML * mxmlTagIterator(XML * xml, const char * tagname) {
  return mxmlFindElement(xml, xml, tagname, NULL, NULL, MXML_DESCEND);
}

XML * mxmlTagIterate(XML * node, XML * xml, const char * tagname) {
  return mxmlFindElement(node, xml, tagname, NULL, NULL, MXML_DESCEND);
}


ALLEGRO_PATH * data_path_ = NULL;

static ALLEGRO_PATH * make_data_path(void) {
  ALLEGRO_PATH * path = al_get_standard_path(ALLEGRO_RESOURCES_PATH);
  al_replace_path_component(path, -1, "data");
  return path;
}

ALLEGRO_PATH * data_path(void) {
  if (data_path_) return data_path_;
  data_path_ = make_data_path();
  return data_path_;
}

#define PATH_CSTR(PATH) al_path_cstr(PATH, ALLEGRO_NATIVE_PATH_SEP)

const char * data_path_cstr(void) {
  return al_path_cstr(data_path(), ALLEGRO_NATIVE_PATH_SEP);
}

#define DATA_PATH_BUFSIZE 1024
ALLEGRO_PATH * data_tileset_filename(const char * name) {
  ALLEGRO_PATH * path = al_clone_path(data_path());
  al_append_path_component(path, "map");
  al_set_path_filename(path, name);
  return path;  
}

Image * tileset_image_load(const char * filename) {
  ALLEGRO_PATH * path;
  Image        * image;
  path  = data_tileset_filename(filename);
  printf("Loading tile set: %s for %s\n", PATH_CSTR(path), filename);
  image = image_load(PATH_CSTR(path));  
  al_destroy_path(path);
  return image;  
}


/*
* Loads a tile set from a tmx xml document.
*/
Tileset * tileset_loadxml(XML * xml) {
  Tileset * set = NULL;
  XML     * node;
  const char * imgn;
  Image * image;    
  imgn  = mxmlFindPathGetAttr(xml, "map/tileset/image", "source");
  if(!imgn) // external tile map reference, need to load it
  { // but loading like that not implemented yet.
    fprintf(stderr, "Cannot load tile maps with external tile sets.\n");
    return NULL;
  }
    
  image = tileset_image_load(imgn);
  if(!image) return NULL;
  set   = tileset_new(image);    
  printf("Tile set %p\n", set);
  return set;
}

/**
* Loads the panes from a tmx document.
*/
Tilemap * tilemap_loadxmlpanes(Tilemap * map, XML * xml) {
  XML * node, * dnod, * tnode;
  for (node = mxmlTagIterator(xml, "layer");
       node != NULL;
       node = mxmlTagIterate(node, xml, "layer")) {
      for (tnode = mxmlTagIterator(node, "tile");
           tnode != NULL;
           tnode = mxmlTagIterate(tnode, node, "tile")) {
        int gid = mxmlElementGetAttrInt(tnode, "gid");
        printf("%d ", gid);
      }
  }
  return NULL;
}

/*
* Loads a tile map from a tmx xml document.
*/
Tilemap * tilemap_loadxml(XML * xml) {
  XML     * node;
  Tilemap * map = NULL;
  Tileset * set = NULL;
  int w, h;
  puts("Tree loaded OK!");
  set = tileset_loadxml(xml);
  if(!set) return NULL;
  w   = mxmlFindPathGetAttrInt(xml, "map", "width");
  h   = mxmlFindPathGetAttrInt(xml, "map", "height");
  if ((w < 0) || (h<0)) goto fail;
  map = tilemap_new(set, w, h);
  if(!map) goto fail;
  printf("map: %p, %d, %d\n", map, w, h);
  if(!(tilemap_loadxmlpanes(map, xml))) goto fail;
  printf("\n tiles loaded.\n");
  
  return map;
  fail:
  tilemap_free(map);
  tileset_free(set);
  return NULL;
}


/**
* Loads a tile map from a tmx file.
*/
Tilemap * tilemap_loadtmx(FILE * fin) {
  Tilemap * result;
  XML * xml;  
  xml = mxmlLoadFile(NULL, fin, MXML_INTEGER_CALLBACK);
  if(!xml) return NULL;
  result = tilemap_loadxml(xml);
  mxmlRelease(xml);
  return result;
}



/**
* Loads a tile map.
*/
Tilemap * tilemap_load(char * filename) {
  Tilemap * result;
  FILE        * fin;
  fin = fopen(filename, "r");
  if(!fin) return NULL;
  result = tilemap_loadtmx(fin);
  fclose(fin);
  return result;
}










