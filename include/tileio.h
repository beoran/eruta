#ifndef tileio_H_INCLUDED
#define tileio_H_INCLUDED

#include "xml.h"
#include "tile.h"
#include "tilepane.h"
#include "tilemap.h"
/* This file was generated with:
'cfunctions -c -aoff -n -w tileio_proto src/tileio.c' */
#ifndef CFH_TILEIO_PROTO
#define CFH_TILEIO_PROTO

/* From 'src/tileio.c': */

Image * tileset_image_load (const char * filename );

Tile * tile_loadxml (xmlNode * xtil , Tileset * set );

Tileset * tileset_loadxml (xmlNode * node );

char * csv_next (char * csv , int * value );

Tilepane * tilemap_loadpanexml (Tilemap * map , xmlNode * xlayer , int count );

Tilemap * tilemap_loadpanesxml (Tilemap * map , xmlNode * xlayer );

Tilemap * tilemap_loadxml (xmlDoc * xml );

Tilemap * tilemap_loadtmx (const char * filename );

Tilemap * tilemap_load (char * filename );

#endif /* CFH_TILEIO_PROTO */


#endif




