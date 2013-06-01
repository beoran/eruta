#include <stdio.h>
#include <stdarg.h>
#include "tile.h"
#include "tilemap.h"
#include "tileio.h"
#include "silut.h"
#include "fifi.h"
#include "xml.h"



Image * tileset_image_load(const char * filename) {
  return (Image*) fifi_loadsimple(
                    (FifiSimpleLoader*)al_load_bitmap, filename, "map", NULL);
}


 

Tile * tile_loadxml(xmlNode * xtil, Tileset * set) {
  char * sflags = NULL;
  int ianim     = 0, iwait = 0;
  xmlNode * firstprop;
  int id        = xmlGetPropInt(xtil, "id");
  Tile  * tile  = tileset_get(set, id);
  if (!tile) return NULL;
  // printf("Tile id: %d %p\n", id, tile);
  // use properties of tile...
  firstprop     = xmlFindChildDeep(xtil, "properties", "property", NULL);
  // get type, animation and wait
  sflags        = xmlPropertyValue(firstprop, "type");
  xmlPropertyValueInt(firstprop, "anim", &ianim);
  xmlPropertyValueInt(firstprop, "wait", &iwait);
  tile_property_(tile, sflags);
  if(ianim) { 
    tile_anim_(tile, ianim);
    if(iwait > 0) tile_wait_(tile, iwait);
    else tile_wait_(tile, 200);
  }
  //printf("Tile type: %s, anim: %d, wait: %d, flags:%d\n", sflags, ianim, iwait, tile_flags(tile));
  return tile;
}

Tileset * tileset_loadxml(xmlNode * node) {
  xmlNode * xima = NULL;
  xmlNode * xtil = NULL;
  Image   * image;
  Tileset * set;
  char * iname;
  xima  = xmlFindChild(node, "image"); /* Image data is in image tag. */  
  if(!xima) {    
    return NULL;
  }
  iname = xmlGetAttr(xima, "source");
  image = tileset_image_load(iname);
  // printf("Loaded tile set: %s, %p\n", iname, image);
  // xmlFree(iname); 
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
  int w, h;
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
  name = xmlGetAttr(xlayer, "name");
  laid = silut_lsearchcstr(tileio_layernames, name);
  if(!laid) {
    fprintf(stderr, "Unknown layer name: %s.\n"
                    "Must be layer_0, layer_1, layer_2 or layer_3.", name);
    // xmlFree(name);
    return NULL;
  }
  // xmlFree(name);
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
  senc = xmlGetAttr(xdata, "encoding");
  laid = silut_lsearchcstr(tileio_encodings, senc);
  if(!laid) {
    fprintf(stderr, "Unknown encoding: %s.\n"
                    "Must be csv.", name);
    // xmlFree(senc);
    return NULL;                    
  }
  // xmlFree(senc);  
  // Only accept csv for now...
  csv = xmlNodeChildContentRef(xdata);
  // csv = (char *) xmlNodeGetContent(xdata);
  // printf("data: %s\n", );  
  for(yindex = 0; yindex < h; yindex ++) {
    for(xindex = 0; xindex < w; xindex ++) {
      int tileindex = 0;
      int realindex;
      csv = csv_next(csv, &tileindex);
      if(!csv) {
        perror("Unexpected end of csv data");
        goto csv_done;
      }
      // we read a tile index, set it
      // printf("set tile pane : %d %d %d\n", xindex, yindex, tileindex);
      // TMX's tile indexes are 1 bigger than eruta's, so just substract one.
      realindex = tileindex - 1;
      tilemap_setindex(map, layer, xindex, yindex, realindex);
 
    }
  }
  csv_done:
  // don't forget to free the data 
  // xmlFree((void *)csv);  
  // printf("Loaded pane: %p, %d, %s\n", pane, layer, name);
    
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
      // print_all_attributes(index);
      tilemap_loadpanexml(map, index, count);
  }
  return map;
}


/** Loads a tile map from a tmx xml document
*/
Tilemap * tilemap_loadxml(xmlDoc * xml, TilemapLoadExtra * extra) {
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
  // printf("Map: %d %d\n", wide, high);
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
  
  /// How to integrate with Area? 
  result = tilemap_new(set, wide, high, extra->area);
  // load the layers
  if(!result) return NULL;
  
  tilemap_loadpanesxml(result, xlayer);
  

  // printf("Loaded map %p %p\n", result, set);
  // print_element_names(root);
  return result;
}  



/**
* Loads a tile map from a tmx file.
*/
Tilemap * tilemap_loadtmx(const char * filename, TilemapLoadExtra * extra) {
  Tilemap * result;
  xmlDoc  * xml    = NULL;
  /* Parse the file and get the DOM */
  xml = xmlReadFile(filename, NULL, 0);
  if (xml == NULL) {
    printf("error: could not parse file %s\n", filename);
    return NULL;
  }
  result = tilemap_loadxml(xml, extra);
  /*free the document XXX:this crashes after here, and not of it's not freed, so
  it means we have stray pointers into the xml doc */
  xmlFreeDoc(xml);
  return result;
}



/**
* Loads a tile map.
*/
Tilemap * tilemap_load(char * filename, TilemapLoadExtra * extra) {
  Tilemap * result;
  result = tilemap_loadtmx(filename, extra);
  return result;
}


Tilemap * tilemap_fifi_load(void * extra, char * filename) {
  return tilemap_load(filename, extra);
}







