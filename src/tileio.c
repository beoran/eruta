#include <mxml.h>

#include "tileio.h"

/* Shorthand for mxml_node_t */
#define XML mxml_node_t

/* Functions for loading and saving tile maps, grouped here to limit the use
of xml, etc. */

#define TILEIO_BAD -0xbad

const char * mxmlNameForType(mxml_type_t type) {
  switch(type) {
    case MXML_CUSTOM  : return "custom";
    case MXML_ELEMENT : return "element";
    case MXML_IGNORE  : return "ignore";
    case MXML_INTEGER : return "integer";
    case MXML_OPAQUE  : return "opaque";
    case MXML_REAL    : return "real";
    case MXML_TEXT    : return "text";
    default           : return "unknown";
  }
}

const char * mxmlTypeName(mxml_node_t * node) {
   mxml_type_t type = mxmlGetType(node);
   return mxmlNameForType(type);
}
    
int mxmlElementGetAttrInt(mxml_node_t * node, const char * name) {
   const char * attr =  mxmlElementGetAttr(node, name);   
   if(!attr) {
     fprintf(stderr, "attribute %s not found in %s %s\n", name,
     mxmlGetElement(node), mxmlTypeName(node));
     return TILEIO_BAD;
   }  
   return atoi(attr);
}

const char * mxmlFindPathGetAttr
            (XML * xml, const char * path, const char * name) {
  XML * node  = mxmlFindPath(xml, path);
  if(!node) {
    fprintf(stderr, "node %s not found\n", path); return NULL;
  } else {    
    fprintf(stderr, "node %s found\n", path);
  } 
  return mxmlElementGetAttr(node, name);
}

int mxmlFindPathGetAttrInt(XML * xml, const char * path, const char * name) {
  XML * node  = mxmlFindPath(xml, path);
  if(!node) {
    fprintf(stderr, "node %s not found\n", path); return TILEIO_BAD;
  } else {
    fprintf(stderr, "node %s found: %s\n", path, mxmlGetElement(node));
  } 
  return mxmlElementGetAttrInt(node, name);  
}

XML * mxmlTagIterator(XML * xml, const char * tagname) {
  return mxmlFindElement(xml, xml, tagname, NULL, NULL, MXML_DESCEND);
}

XML * mxmlTagIterate(XML * node, XML * xml, const char * tagname) {
  return mxmlFindElement(node, xml, tagname, NULL, NULL, MXML_DESCEND);
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

/*
* Loads the panes from a tmx document.
*/
Tilemap * tilemap_loadxmlpanes(Tilemap * map, XML * xml) {
  XML * node, * dnode, * tnode;
  char * enc, * index, * csv;
  int value   = 0;
  for (node = mxmlTagIterator(xml, "layer");
       node  != NULL;
       node   = mxmlTagIterate(node, xml, "layer")) {
       dnode  = mxmlFindPath(node, "data");
       if(!dnode) {
          fprintf(stderr, "Data node not found!\n");
          return NULL;          
       }
       enc    = mxmlElementGetAttr(dnode, "encoding");
       if((!enc) || strcmp(enc, "csv")) {
          fprintf(stderr, "Can only read CSV encoded map file.\n");
          return NULL;          
       }
       csv = mxmlGetText(dnode, 0);
       if(!csv) {
          fprintf(stderr, "No CSV in map file.\n");
          return NULL;
       }
       for(index = csv; index ; index = csv_next(index, &value)) {
          printf("%d ", value);
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
  printf("size: %d, %d\n", w, h);
  if ((w < 0) || (h<0)) goto fail;
  map = tilemap_new(set, w, h);
  if(!map) goto fail;
  printf("map: %p, %d, %d\n", map, w, h);
  if(!(tilemap_loadxmlpanes(map, xml))) goto fail;
  printf("\n tiles loaded.\n");
  
  return map;
  fail:
  fprintf(stderr, "Failed to load map.\n");
  tilemap_free(map);
  tileset_free(set);
  return NULL;
}



/*
 * mxmlElementGetNumAttr() Returns number of attributes.
 *
 * This function returns -1 if the node is not an element.
 */

int mxmlElementGetNumAttrs(mxml_node_t *node) {
  if (!node || node->type != MXML_ELEMENT)  return -1;
  return node->value.element.num_attrs;
}

/* Returns the name for the n-th attribute of this element. */
char * mxmlElementGetAttrName(mxml_node_t *node, int index) {
  if (!node || node->type != MXML_ELEMENT)  return NULL;
  if(index < 0)                               return NULL;
  if (index >= mxmlElementGetNumAttrs(node))  return NULL;  
  return node->value.element.attrs[index].name;
}

/* Returns the value for the n-th attribute of this element. */
char * mxmlElementGetAttrValue(mxml_node_t *node, int index) {
  if (!node || node->type != MXML_ELEMENT)    return NULL;
  if(index < 0)                               return NULL;
  if (index >= mxmlElementGetNumAttrs(node))  return NULL;
  return node->value.element.attrs[index].value;
}

#define MXML_NODETEXT_SIZE 1024
static char mxmlNodeTextBuffer[MXML_NODETEXT_SIZE];

const char * mxmlNode2Cstr(mxml_node_t * node) {
  const char *type, *name;
  int index, amount;
  mxmlNodeTextBuffer[0] = '\0'; 
  name = mxmlGetElement(node);
  type = mxmlTypeName(node);
  snprintf(mxmlNodeTextBuffer, MXML_NODETEXT_SIZE, "xml node: %s %s [", name, type);
  // XXX not safe but this is a debugging function only.
  amount = mxmlElementGetNumAttrs(node);
  for (index = 0; index < amount; index ++) {
    strcat(mxmlNodeTextBuffer, " ");
    strcat(mxmlNodeTextBuffer, mxmlElementGetAttrName(node, index));
    strcat(mxmlNodeTextBuffer, "=");
    strcat(mxmlNodeTextBuffer, mxmlElementGetAttrValue(node, index));
    strcat(mxmlNodeTextBuffer, ", ");
  }
  strcat(mxmlNodeTextBuffer, "]");
  return mxmlNodeTextBuffer;
}

static mxml_type_t mxml_load_callback(mxml_node_t *node) {
  const char *type, *name;
  type = mxmlTypeName(node);
  name = mxmlGetElement(node);
  printf("xml load: %s\n", mxmlNode2Cstr(node));
  if(strcmp(name, "data") != 0)  return MXML_ELEMENT;
  return MXML_TEXT;
}


/**
* Loads a tile map from a tmx file.
*/
Tilemap * tilemap_loadtmx(FILE * fin) {
  Tilemap * result;
  XML * xml;  
  xml = mxmlLoadFile(NULL, fin, mxml_load_callback);
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










