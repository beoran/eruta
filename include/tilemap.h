#ifndef TILEMAP_H
#define TILEMAP_H

typedef struct Tilemap_ Tilemap;

#include "area.h"
#include "tile.h"
#include "tilepane.h"
/* This file was generated with:
'cfunctions -c -aoff -n -w tilemap_proto src/tilemap.c' */
#ifndef CFH_TILEMAP_PROTO
#define CFH_TILEMAP_PROTO

/* From 'src/tilemap.c': */

Image * image_loadtexture (const char * category , int index );

Tilemap * tilemap_done (Tilemap * self );

Tilemap * tilemap_init (Tilemap * self , Tileset * set , int w , int h );

Tilemap * tilemap_free (Tilemap * map );

Tilemap * tilemap_new (Tileset * set , int w , int h );

Tilepane * tilemap_pane (Tilemap * self , int index );

Tilepane * tilemap_pane_ (Tilemap * self , int index , Tilepane * pane );

Tilepane * tilemap_panenew (Tilemap * self , int index , int w , int h );

Tile * tilemap_get (Tilemap * self , int l , int x , int y );

int tile_thingkind (Tile * tile );

Thing * tilemap_tiletothing (Tilemap * self , int l , int x , int y , Tile * tile );

Tile * tilemap_settile (Tilemap * self , int l , int x , int y , Tile * tile );

Tile * tilemap_setindex (Tilemap * self , int l , int x , int y , int index );

Tile * tilemap_rect (Tilemap * self , int l , int x , int y , int w , int h , Tile * tile );

Tile * tilemap_fill (Tilemap * self , int l , Tile * tile );

int tilemap_getindex (Tilemap * self , int l , int x , int y );

Thing * tilemap_addtilething (Tilemap * self , int kind , int tx , int ty , int layer );

void tilemap_draw (Tilemap * map , Camera * camera );

void tilemap_update (Tilemap * map , double dt );

Thing * tilemap_addthing (Tilemap * self , int kind , int x , int y , int z , int w , int h );

Lockin * tilepane_lockin (Tilepane * pane , Camera * camera );

Lockin * tilemap_layer_lockin (Tilemap * map , int layer , Camera * camera );


#ifdef COMMENT_
Tilemap * tilemap_load (lua_State * lua , int index );

Tilepane * tilepane_savefile (Tilepane *pane , int paneid , FILE *fout );

Tilemap * tilemap_savefile (Tilemap * map , FILE * fout );

Tilemap * tilemap_save (Tilemap * map , int index );

int tilemap_track (Tracker * tracker , void * data );

#endif

#endif /* CFH_TILEMAP_PROTO */


#endif
