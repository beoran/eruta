/* This file was generated with:
'cfunctions -c -aoff -n -w tilemap_proto src/tilemap.c' */
#ifndef CFH_TILEMAP_PROTO
#define CFH_TILEMAP_PROTO

/* From 'src/tilemap.c': */

#line 31 "src/tilemap.c"
Image * image_loadtexture (const char * category , int index );

#line 41 "src/tilemap.c"
Tilemap * tilemap_done (Tilemap * self );

#line 56 "src/tilemap.c"
Tilemap * tilemap_init (Tilemap * self , Tileset * set , int w , int h );

#line 73 "src/tilemap.c"
Tilemap * tilemap_free (Tilemap * map );

#line 80 "src/tilemap.c"
Tilemap * tilemap_new (Tileset * set , int w , int h );

#line 90 "src/tilemap.c"
Tilepane * tilemap_pane (Tilemap * self , int index );

#line 97 "src/tilemap.c"
Tilepane * tilemap_pane_ (Tilemap * self , int index , Tilepane * pane );

#line 107 "src/tilemap.c"
Tilepane * tilemap_panenew (Tilemap * self , int index , int w , int h );

#line 114 "src/tilemap.c"
Tile * tilemap_get (Tilemap * self , int l , int x , int y );

#line 123 "src/tilemap.c"
int tile_thingkind (Tile * tile );

#line 136 "src/tilemap.c"
Thing * tilemap_tiletothing (Tilemap * self , int l , int x , int y , Tile * tile );

#line 146 "src/tilemap.c"
Tile * tilemap_settile (Tilemap * self , int l , int x , int y , Tile * tile );

#line 157 "src/tilemap.c"
Tile * tilemap_setindex (Tilemap * self , int l , int x , int y , int index );

#line 169 "src/tilemap.c"
Tile * tilemap_rect (Tilemap * self , int l , int x , int y , int w , int h , Tile * tile );

#line 176 "src/tilemap.c"
Tile * tilemap_fill (Tilemap * self , int l , Tile * tile );

#line 182 "src/tilemap.c"
int tilemap_getindex (Tilemap * self , int l , int x , int y );

#line 189 "src/tilemap.c"
Thing * tilemap_addtilething (Tilemap * self , int kind , int tx , int ty , int layer );

#line 195 "src/tilemap.c"
void tilemap_draw (Tilemap * map , Camera * camera );

#line 210 "src/tilemap.c"
void tilemap_update (Tilemap * map , double dt );

#line 217 "src/tilemap.c"
Thing * tilemap_addthing (Tilemap * self , int kind , int x , int y , int z , int w , int h );

#line 223 "src/tilemap.c"

#ifdef COMMENT_
Tilemap * tilemap_load (lua_State * lua , int index );

#line 239 "src/tilemap.c"
Tilepane * tilepane_savefile (Tilepane *pane , int paneid , FILE *fout );

#line 254 "src/tilemap.c"
Tilemap * tilemap_savefile (Tilemap * map , FILE * fout );

#line 286 "src/tilemap.c"
Tilemap * tilemap_save (Tilemap * map , int index );

#line 303 "src/tilemap.c"
int tilemap_track (Tracker * tracker , void * data );

#endif

#endif /* CFH_TILEMAP_PROTO */
