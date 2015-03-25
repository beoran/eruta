#include <stdio.h>
#include <stdarg.h>
#include "tile.h"
#include "tilemap.h"
#include "tileio.h"
#include "silut.h"
#include "fifi.h"
#include "monolog.h"
#include "bxml.h"
#include "bxmlparser.h"

#define TILEMAP_MAX_WIDTH   1024
#define TILEMAP_MAX_HEIGHT  1024

/* Tile flipping. */
#define TMX_FLIPPED_HORIZONTALLY 0x80000000
#define TMX_FLIPPED_VERTICALLY   0x40000000
#define TMX_FLIPPED_DIAGONALLY   0x20000000
#define TMX_FLIPPED_FILTER      (~0xE0000000)



Image * tileset_image_load(const char * filename) {
  return (Image*) fifi_loadsimple(
                    (FifiSimpleLoader*)al_load_bitmap, filename, "map", NULL);
}

char * tileio_get_tile_property(Bxml * xtil, char * propname) {
  Bxml * proplist, * prop, * firstprop;
  firstprop     = bxml_find_child_deep(xtil, "properties", "property", NULL);
  for (prop = firstprop; prop ; 
    prop = bxml_find_next(prop->sibling, "property") ) {
      char * name = bxml_get_attribute(prop, "name");
      if ((name) && (strcmp(name, propname) == 0)) {
        return bxml_get_attribute(prop, "value");
      }
  }
  return NULL;
}
 
int * tileio_get_tile_property_int(Bxml * xtil, char * propname, int * ivalue) {
  char * value = tileio_get_tile_property(xtil, propname);
  if (!value) return NULL;
  (*ivalue) = atoi(value);
  return ivalue;
} 

Tile * tile_load_anim_from_xml(Tile * tile, Bxml * xtil, Tileset * set) {
  long index;
  double duration;
  Bxml * xframe = NULL;
  Bxml * xanim = bxml_find_child(xtil, "animation");
  
  /* No animations, no problem. */
  if (!xanim) return tile;
  
  for (xframe = bxml_find_child(xanim, "frame"); xframe;
       xframe = bxml_find_next(xframe->sibling, "frame")) {
    if (!bxml_get_attribute_long(xframe, "tileid", &index)) { 
      LOG_WARNING("Cannot parse tileid of animation frame. Frame ignored.");
      return tile; 
    }
    
    if (!bxml_get_attribute_double(xframe, "duration", &duration)) { 
      LOG_WARNING("Cannot parse duration of animation frame. Frame ignored.");
      return tile; 
    }
    
    /* Tiled uses ms for durations, eruta s. */
    duration /=  1000.0;
    (void) set;
    // confusingly, here we don't have to substract the tile set's firstgid
    tile_add_animation_frame(tile, index, duration);        
  } 
  return tile;
}

Tile * tile_loadxml(Bxml * xtil, Tileset * set) {
  char * sflags = NULL;
  int ianim     = 0, iwait  = 0, iblend = 0, ibmask = 0;
  int ilight    = 0, ilmask = 0;
  int ishadow   = 0, ismask = 0;
  Bxml * firstprop;
  long id        ;
  Tile  * tile  ;
  
  if (!bxml_get_attribute_long(xtil, "id", &id)) {
    LOG_ERROR("Cannot parse tile ID.");
    return NULL;
  }
  
  tile = tileset_get(set, id);
  if (!tile) { 
    LOG_ERROR("Cannot get tile from tile set %d.", id); 
    return NULL;
  }
  
  // Get and apply the properties of the tile...
  /* Here, sflags sometimes is null, I guess when there is a type without a type. */
  sflags        = tileio_get_tile_property(xtil, "type");
  tile_property_(tile, sflags);
  tileio_get_tile_property_int(xtil, "anim"       , &ianim);
  tileio_get_tile_property_int(xtil, "wait"       , &iwait);
  tileio_get_tile_property_int(xtil, "blend"      , &iblend);
  tileio_get_tile_property_int(xtil, "blendmask"  , &ibmask);
  tileio_get_tile_property_int(xtil, "light"      , &ilight);
  tileio_get_tile_property_int(xtil, "lightmask"  , &ilmask);
  tileio_get_tile_property_int(xtil, "shadow"     , &ishadow);
  tileio_get_tile_property_int(xtil, "shadowmask" , &ismask);

  /* Support classic style animations... */ 
  if(ianim) { 
    tile_anim_(tile, ianim);
    if(iwait > 0) tile_wait_(tile, iwait);
    else tile_wait_(tile, 200);
  }

  /* Support tiled-style animations as well. A renderng time these 
   * will take precedence over classic animations if available. */
  tile_load_anim_from_xml(tile, xtil, set);

  tile_blend_(tile, iblend);
  tile_blend_mask_(tile, ibmask);
  tile_light_(tile, ilight);
  tile_light_mask_(tile, ilmask);
  tile_shadow_(tile, ishadow);
  tile_shadow_mask_(tile, ismask);
  
  return tile;
}

Tileset * tileset_loadxml(Bxml * node) {
  Bxml * xima = NULL;
  Bxml * xtil = NULL;
  long firstgid = 1;
  Image   * image;
  Tileset * set;
  char * iname;
  
  if (!bxml_get_attribute_long(node, "firstgid", &firstgid)) {
    LOG_ERROR("Could not parse firstgid attribute for tileset"); 
    return NULL;  
  }
  
  xima  = bxml_find_child(node, "image"); /* Image data is in image tag. */  
  if(!xima) {    
    LOG_ERROR("Could not parse image tag for tileset"); 
    return NULL;
  }
  
  iname = bxml_get_attribute(xima, "source");
  image = tileset_image_load(iname);
  LOG("Loaded tile set: %s, %p\n", iname, image);
  // xmlFree(iname); 
  if(!image) { 
    LOG_ERROR("Could not load tile set source: %s", iname); 
    return NULL; 
  }
  
  set = tileset_new(image, firstgid);
  if(!set)   { 
    LOG_ERROR("Could not allocate tile set %s.", iname);
    return NULL; 
  }
  
  for (xtil = bxml_find_child(node, "tile"); xtil;
       xtil = bxml_find_next(xtil->sibling, "tile")) {
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
char * csv_next(char * csv, unsigned long * value) {
  char * aid = NULL;
  if(!value) return NULL;
  if(sscanf(csv, " %lu , ", value)) {
    // a number could be scanned
    aid = strchr(csv, ',');
    return aid + 1;
  }
  // no number could be scanned, stop it there
  // (*value) = XML_BAD_VALUE;
  return NULL;
}


/** Calculates the Allegro draw flags for a tile. 
 * Diagonal flipping is unsupported
 * */
 
 int tileio_allegro_flags_for(unsigned long tileindex) {
  int result = 0;
  if (tileindex & TMX_FLIPPED_HORIZONTALLY) {
    result |= ALLEGRO_FLIP_HORIZONTAL;
  }
  if (tileindex & TMX_FLIPPED_VERTICALLY) {
    result |= ALLEGRO_FLIP_VERTICAL;
  }
   
  if (tileindex & TMX_FLIPPED_DIAGONALLY) {
    LOG_WARNING("Diagonally flipped tiles are not supported by Eruta.\n");
    // Implement this in Allegro ???
  }
    
  return result;
}

/** Loads a single tile pane of the tile map from xml (tmx). */
Tilepane * tilemap_loadpanexml(Tilemap * map, Bxml * xlayer, int count) {
  long w = 0 , h = 0;
  char * name, * csv, * senc;
  Silut * laid;
  Tilepane * pane;
  Bxml * xdata;
  int layer;
  int xindex, yindex, firstgid;
  
  (void) count;
  
  firstgid = tilemap_firstgid(map);
  
  if (firstgid < 0) {
    LOG_ERROR("Tile set should be set before loading tile panes.");
    return NULL;
  }
  
  if (!bxml_get_attribute_long(xlayer, "width", &w)) {
    LOG_ERROR("Could not parse width of layer.");
    return NULL;
  }
  
  if (!bxml_get_attribute_long(xlayer, "height", &h)) {
    LOG_ERROR("Could not parse height of layer.");
    return NULL;
  }

  if ((w<0) || (w>TILEMAP_MAX_WIDTH)) {
    LOG_ERROR("Width of layer too high or negative.");
    return NULL;
  }

  if ((h<0) || (h>TILEMAP_MAX_HEIGHT)) {
    LOG_ERROR("Height of layer too high or negative.");
    return NULL;
  }
  
  name = bxml_get_attribute(xlayer, "name");
  laid = silut_lsearchcstr(tileio_layernames, name);
  if(!laid) {
    LOG_ERROR("Unknown layer name: %s.\n"
              "Must be layer_0, layer_1, layer_2 or layer_3.\n", name);
    return NULL;
  }
  
  layer = laid->integer;
  pane  = tilemap_panenew(map, layer, w, h);
  if(!pane) {
    LOG_ERROR("Could not create pane.\n");
    return NULL;
  }
  
  xdata = bxml_find_child(xlayer, "data");
  if(!xdata) {
    LOG_ERROR("Could not find pane data.\n");
    return NULL;    
  }
  
  senc = bxml_get_attribute(xdata, "encoding");
  laid = silut_lsearchcstr(tileio_encodings, senc);
  if(!laid) {
    LOG_ERROR("Unknown encoding: %s.\n"
                    "Must be csv.\n", name);
    // xmlFree(senc);
    return NULL;                    
  }
  // xmlFree(senc);  
  // Get first text child of <data> and hope that it is the data itself. 
  // Only accept csv for now...
  csv        = bxml_get_text_under(xdata);
  // csv = (char *) BxmlGetContent(xdata);
  for(yindex = 0; yindex < h; yindex ++) {
    for(xindex = 0; xindex < w; xindex ++) {
      unsigned long tileindex = 0;
      int realindex;
      int drawflags;
      csv = csv_next(csv, &tileindex);
      if(!csv) {
        LOG_ERROR("Unexpected end of csv data");
        goto csv_done;
      }
      /* We read a tile index, set it.
      * TMX's tile indexes depend on the tile set's fristgid, 
      * so just substract that.
      */
      drawflags  = tileio_allegro_flags_for(tileindex);
      if (drawflags) {
        LOG("Tile with flags found: %d\n", tileindex, drawflags);
        tilemap_set_flags(map, layer, xindex, yindex, drawflags);
      }
      tileindex &= TMX_FLIPPED_FILTER;
      realindex  = (int)(tileindex - firstgid);
      tilemap_setindex(map, layer, xindex, yindex, realindex); 
    }
  }
  csv_done:
  return pane;  
}


/** Loads the tile panes of the tile map from xml (tmx). */
Tilemap * tilemap_loadpanesxml(Tilemap * map, Bxml * xlayer) {
  Bxml * index;
  int count;
  if (!xlayer) return NULL;
  for(index = xlayer, count = 0; index ;
      index = bxml_find_next(index->sibling, "layer"), count++) {
      tilemap_loadpanexml(map, index, count);
  }
  return map;
}


/** Loads a tile map from a tmx xml document
*/
Tilemap * tilemap_loadxml(Bxml * xml, TilemapLoadExtra * extra) {
  Tilemap * result;
  Tileset * set;
  Bxml *    root   = NULL;
  Bxml *    xset   = NULL;
  Bxml *    xlayer = NULL;
  long wide = 0, high = 0;
  long tilewide = 0, tilehigh = 0; 
  (void) extra;

  root = xml;
  /* Get the root element node, which should be map. */
  if (strcmp((char *) root->name, "map")) {
    LOG_ERROR("Not a tile map file!");
    return NULL;
  }
  
  if (!bxml_get_attribute_long(root, "width", &wide)) {
    LOG_ERROR("Width not a number");
    return NULL;
  }

  if (!bxml_get_attribute_long(root, "height", &high)) {
    LOG_ERROR("Height not a number!");
    return NULL;
  }
  
  if ((wide < 1) || (high < 1)) {
    LOG_ERROR("Map dimensions too small: %d %d", high, wide);
    return NULL;
  }
  
  if ((wide > TILEMAP_MAX_WIDTH) || (high > TILEMAP_MAX_HEIGHT)) {
    LOG_ERROR("Map dimensions too big : %d %d, %Dx%d at most.", 
      high, wide, TILEMAP_MAX_WIDTH, TILEMAP_MAX_HEIGHT);
    return NULL;
  }
  
  if (!bxml_get_attribute_long(root, "tilewidth", &tilewide)) {
    LOG_ERROR("Could not parse tile width");
    return NULL;
  }

  if (!bxml_get_attribute_long(root, "tileheight", &tilehigh)) {
    LOG_ERROR("Could not parse tile height!");
    return NULL;
  }
  
  if ((tilewide != 32) || (tilehigh != 32)) {
    LOG_ERROR("Eruta only supports 32x32 tiles!");
    return NULL;
  }
  
  /* TODO check for map type and maybe also background color? */
   
  // Use the first tile set (and that one only) as the map's tile set.
  xset   = bxml_find_child(root, "tileset");

  if (!xset) { 
    bxml_show_to(root, stderr, 0);
    LOG_ERROR("Tile set not found.");
    return NULL;
  }
  
  // Look for the layers as well
  xlayer = bxml_find_child(root, "layer");
  if (!xlayer) { 
    LOG_ERROR("Layers not found.");
    return NULL;
  }

  // Load the tile set
  set    = tileset_loadxml(xset);
  if(!set) {
    
    LOG_ERROR("Tile set not loaded!");
    return NULL;
  }
  
  // Create a new tile map with the loaded set.
  result = tilemap_new(set, wide, high, NULL);
  // load the layers
  if(!result) { 
    LOG_ERROR("Out of memory when creating new tile map to load.");
    return NULL;
  }
  
  tilemap_loadpanesxml(result, xlayer);
  return result;
}  



/**
* Loads a tile map from a tmx file.
*/
Tilemap * tilemap_loadtmx(const char * filename, TilemapLoadExtra * extra) {
  Tilemap    * result = NULL;
  BxmlParser * parser = bxmlparser_new();
  Bxml       * xml;
  
  /* Parse the file and get the DOM */
  xml = bxmlparser_parse_filename(parser, (char *) filename);
  
  if ((xml == NULL) || (bxmlparser_get_error(parser))) {
    LOG_ERROR("error: could not parse file %s: %s\n", filename, bxmlparser_get_error(parser));
  } else { 
    result = tilemap_loadxml(xml, extra);
  }
  bxmlparser_free(parser);
  bxml_free(xml);
  return result;
}



/**
* Loads a tile map.
*/
Tilemap * tilemap_load(const char * filename, TilemapLoadExtra * extra) {
  Tilemap * result;
  result = tilemap_loadtmx(filename, extra);
  /* Set up blending if loaded OK. */
  if (result) {
    tilemap_init_blend(result);
  }
  return result;
}


void * tilemap_fifi_load(void * extra, const char * filename) {
  return tilemap_load(filename, extra);
}







