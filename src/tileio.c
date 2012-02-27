#include <stdio.h>
#include <libxml/parser.h>
#include <libxml/tree.h>
#include "tilemap.h"
#include "tileio.h"

/* Functions for loading and saving tile maps, grouped here to limit the use
of xml, etc. */


/**
 * print_element_names:
 * @a_node: the initial xml node to consider.
 *
 * Prints the names of the all the xml elements
 * that are siblings or children of a given xml node.
 */
static void print_element_names(xmlNode * node) {
  xmlNode *now = NULL;
  static level = 0;
  for(now = node; now; now = now->next) {
    if (now->type == XML_ELEMENT_NODE) {
      printf("node type: Element, %d, name: %s\n", level, now->name);
    }
    level++;
    print_element_names(now->children);
    level--;
  }
}

xmlNode * xmlFindChild(xmlNode * node, const char * name) {
  xmlNode *now = node->children;
  printf("Child node: %p (of %p)\n", now, node);
  for(now = node; now; now = now->next) {
    printf("searching %s, now : %d, %s\n", name, now->type, now->name);
    if (now->type == XML_ELEMENT_NODE) {      
      if(!strcmp((char *)now->name, name)) return now;
    }
    // also recurse to look a level deeper...
    if(now->children) {
      return xmlFindChild(now->children, name);
    }
  }
  return NULL;
}

#define TILEIO_BAD -0xbad
#define XML_GET_PROP(NODE, STR)\
        ((char *) xmlGetProp(NODE, (const xmlChar *)STR))
        
/* Gets an integer property from the libxml2 node, or negative if not found. */
int xmlGetPropInt(xmlNode * node, const char * name) {
  char * prop = XML_GET_PROP(node, name);
  if(!prop) return TILEIO_BAD;
  return atoi(prop);
}




char * csv_next(char * index, int * value) {
  char * aid;
  if(!value) return NULL;
  aid = strchr(index, ',');
  if(!aid) {
    (*value) = TILEIO_BAD;
    return NULL;
  }
  (*value) = atoi(index);
  return index;  
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

Tileset * tileset_loadxml(xmlNode * node) {
  xmlNode * xima = NULL;
  Image   * image;
  const char * iname;
  xima  = xmlFindChild(node, "image"); /* Image data is in image tag. */
  if(!xima) {    
    return NULL;
  }
  iname = XML_GET_PROP(xima, "source");
  image = tileset_image_load(iname);
  printf("Loaded tile set: %s, %p\n", iname, image);  
  if(!image) { return NULL; } 
  return tileset_new(image);
}


/**
* Loads a tile map from a tmx file.
*/
Tilemap * tilemap_loadtmx(const char * filename) {
  Tilemap * result;
  Tileset * set;
  xmlDoc  *xml  = NULL;
  xmlNode *root = NULL;
  xmlNode *xset = NULL;  
  int wide, high;
  /*parse the file and get the DOM */
  xml = xmlReadFile(filename, NULL, 0);
  if (xml == NULL) {
    printf("error: could not parse file %s\n", filename);
    return NULL;
  }  
  root = xmlDocGetRootElement(xml);
  /* Get the root element node, which should be map. */
  if (strcmp((char *) root->name, "map")) {
    xmlFreeDoc(xml);
    return NULL;
  }
  wide   = xmlGetPropInt(root, "width");
  high   = xmlGetPropInt(root, "height");
  printf("Map: %d %d\n", wide, high);
  // Use the first tile set (and that one only) as the map's tile set.
  xset   = xmlFindChild(root, "tileset");
  
  if(!xset) { // check if tileset was found
    perror("Tile set not found.");
    print_element_names(root);
    xmlFreeDoc(xml);
    return NULL; 
  }  
  
  set    = tileset_loadxml(xset);
  // load the tile set 
  result = tilemap_new(set, wide, high);
  printf("Loaded map %p %p\n", result, set);

  
  // 
  /*free the document */
  xmlFreeDoc(xml);
  return result;
}



/**
* Loads a tile map.
*/
Tilemap * tilemap_load(char * filename) {
  Tilemap * result;
  FILE        * fin;
  //fin = fopen(filename, "r");
  //if(!fin) return NULL;
  result = tilemap_loadtmx(filename);
  //fclose(fin);
  return result;
}










