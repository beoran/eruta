#include <stdio.h>
#include <stdarg.h>
#include "tile.h"
#include "tilemap.h"
#include "tileio.h"
#include "silut.h"
#include "fifi.h"
#include "xml.h"



Image * tileset_image_load(const char * filename) {
  ALLEGRO_PATH * path;
  Image        * image;
  path  = fifi_tileset_filename(filename);
  printf("Loading tile set: %s for %s\n", PATH_CSTR(path), filename);
  image = image_load(PATH_CSTR(path));  
  al_destroy_path(path);
  return image;  
}

/* Helper to load property values for a given property name. Pass in the
first propery tag.  */
char * xmlPropertyValue(xmlNode * firstprop, char * name) {
  xmlNode * xprop;
  for(xprop = firstprop ; xprop; xprop = xmlFindNext(xprop->next, "property")) {
    char * propname = XML_GET_PROP(xprop, "name");
    if(!propname) continue;
    if(!strcmp(propname, name)) return  XML_GET_PROP(xprop, "value");
  }
  return NULL;  
}

/* Helper to load integer property values. */
int * xmlPropertyValueInt(xmlNode * firstprop, char * name, int * result) {
  char * aid = xmlPropertyValue(firstprop, name);
  if(!aid) return NULL;
  (*result) = atoi(aid);
  return result;
}  
 

Tile * tile_loadxml(xmlNode * xtil, Tileset * set) {
  char * sflags = NULL;
  int ianim = 0, iwait = 0;
  xmlNode * firstprop, *xaid;
  int id = xmlGetPropInt(xtil, "id");
  Tile  * tile = tileset_get(set, id);  
  if (!tile) return NULL;
  printf("Tile id: %d %p\n", id, tile);
  // use properties of tile...
  firstprop = xmlFindChildDeep(xtil, "properties", "property", NULL);
  // get type, animation and wait
  sflags    = xmlPropertyValue(firstprop, "type");  
  xmlPropertyValueInt(firstprop, "anim", &ianim);
  xmlPropertyValueInt(firstprop, "wait", &iwait);
  tile_property_(tile, sflags);
  printf("Tile type: %s, anim: %d, wait: %d, flags:%d\n", sflags, ianim, iwait,
    tile_flags(tile));
  
  return tile;  
}

Tileset * tileset_loadxml(xmlNode * node) {
  xmlNode * xima = NULL;
  xmlNode * xtil = NULL;
  Image   * image;
  Tileset * set;
  const char * iname;
  xima  = xmlFindChild(node, "image"); /* Image data is in image tag. */  
  if(!xima) {    
    return NULL;
  }
  iname = XML_GET_PROP(xima, "source");
  image = tileset_image_load(iname);
  printf("Loaded tile set: %s, %p\n", iname, image);
  if(!image) { return NULL; }
  set = tileset_new(image);
  if(!set)   { return NULL; }
  for (xtil = xmlFindChild(node, "tile"); xtil;
       xtil = xmlFindNext(xtil->next, "tile")) {
    tile_loadxml(xtil, set);
  }
  return set;
}


/* Look up table for layer names */
Silut tileio_layernames[] = {
  { 0, "layer_0" },
  { 1, "layer_1" },
  { 2, "layer_2" },
  { 3, "layer_3" },
  SILUT_DONE
};

/* Look up table for object layer names */
Silut tileio_objectnames[] = {
  { 0, "object_0" },
  { 1, "object_1" },
  { 2, "object_2" },
  { 3, "object_3" },
  SILUT_DONE
};

/* Look up table for data encoding names */
Silut tileio_encodings[] = {
  { 0, "csv" },
  SILUT_DONE
};


/* Gets the next value from csv. Returns NULL when done. */
char * csv_next(char * csv, int * value) {
  char * aid = NULL;
  if(!value) return NULL;
  if(sscanf(csv, " %d , ", value)) {
    // a number could be scanned
    aid = strchr(csv, ',');
    return aid + 1;
  }
  // no number could be scanned, stop it there
  (*value) = XML_BAD_VALUE;
  return NULL;
}




/** Loads a single tile pane of the tile map from xml (tmx). */
Tilepane * tilemap_loadpanexml(Tilemap * map, xmlNode * xlayer, int count) {
  int w, h, code;
  char * name, * csv, * senc;
  Silut * laid;
  Tilepane * pane;
  xmlNode * xdata;
  int layer;
  int xindex, yindex;
  w    = xmlGetPropInt(xlayer, "width");
  h    = xmlGetPropInt(xlayer, "height");
  if ((w<0) || (w>1000)) {
    perror("Width of layer too high or negative.");
    return NULL;
  }
  if ((h<0) || (h>1000)) {
    perror("Height of layer too high or negative.");
    return NULL;
  }
  name = XML_GET_PROP(xlayer, "name");
  laid = silut_lsearchcstr(tileio_layernames, name);
  if(!laid) {
    fprintf(stderr, "Unknown layer name: %s.\n"
                    "Must be layer_0, layer_1, layer_2 or layer_3.", name);
    return NULL;
  }
  layer = laid->integer;
  pane  = tilemap_panenew(map, layer, w, h);
  if(!pane) {
    perror("Could not create pane.");
    return NULL;
  }
  xdata = xmlFindChild(xlayer, "data");
  if(!xdata) {
    perror("Could not find pane data.");
    return NULL;    
  }
  senc = XML_GET_PROP(xdata, "encoding");
  laid = silut_lsearchcstr(tileio_encodings, senc);
  if(!laid) {
    fprintf(stderr, "Unknown encoding: %s.\n"
                    "Must be csv.", name);
    return NULL;                    
  }
  
  // Only accept csv for now...
  csv = xmlNodeGetContent(xdata);
  // printf("data: %s\n", );  
  for(yindex = 0; yindex < h; yindex ++) {
    for(xindex = 0; xindex < w; xindex ++) {
      int tileindex = 0;
      csv = csv_next(csv, &tileindex);
      if(!csv) {
        perror("Unexpected end of csv data");
        goto csv_done;
      }
      // we read a tile index, set it
      // printf("set tile pane : %d %d %d\n", xindex, yindex, tileindex);
      // TMX's tile indexes are 1 bigger than eruta's, so just substract one.
      tilepane_setindex(pane, xindex, yindex, tileindex - 1);
    }
  }
  csv_done:
  printf("Loaded pane: %p, %d, %s\n", pane, layer, name);
    
  // print_element_names(xdata);
  return pane;  
}


/** Loads the tile panes of the tile map from xml (tmx). */
Tilemap * tilemap_loadpanesxml(Tilemap * map, xmlNode * xlayer) {
  xmlNode * index;
  int count;
  if(!xlayer) return NULL;
  for(index = xlayer, count = 0; index ;
      index = xmlFindNext(index->next, "layer"), count++) {
      print_all_attributes(index);
      Tilepane * pane = tilemap_loadpanexml(map, index, count);
  }
}


/** Loads a tile map from a tmx xml document
*/
Tilemap * tilemap_loadxml(xmlDoc * xml) {
  Tilemap * result;
  Tileset * set;
  xmlNode *root   = NULL;
  xmlNode *xset   = NULL;
  xmlNode *xlayer = NULL;
  int wide, high;

  root = xmlDocGetRootElement(xml);
  /* Get the root element node, which should be map. */
  if (strcmp((char *) root->name, "map")) {
    perror("Not a tile map file!");
    return NULL;
  }
  wide   = xmlGetPropInt(root, "width");
  high   = xmlGetPropInt(root, "height");
  printf("Map: %d %d\n", wide, high);
  // Use the first tile set (and that one only) as the map's tile set.
  xset   = xmlFindChild(root, "tileset");

  if(!xset) { // check if tileset was found
    perror("Tile set not found.");
    return NULL;
  }
  xlayer = xmlFindChild(root, "layer");

  if(!xlayer) { // check if layer was found
    perror("Layer not found.");
    return NULL;
  }

  set    = tileset_loadxml(xset);
  if(!set) {
    perror("Tile set not loaded");
    return NULL;
  }
  // load the tile set
  result = tilemap_new(set, wide, high);
  // load the layers
  if(!result) return NULL;
  
  tilemap_loadpanesxml(result, xlayer);
  

  printf("Loaded map %p %p\n", result, set);
  // print_element_names(root);
  return result;
}  



/**
* Loads a tile map from a tmx file.
*/
Tilemap * tilemap_loadtmx(const char * filename) {
  Tilemap * result;
  xmlDoc  *xml    = NULL;
  /* Parse the file and get the DOM */
  xml = xmlReadFile(filename, NULL, 0);
  if (xml == NULL) {
    printf("error: could not parse file %s\n", filename);
    return NULL;
  }
  result = tilemap_loadxml(xml);
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










