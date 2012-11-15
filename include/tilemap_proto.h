#ifndef TILEMAP_PROTO_H
#define TILEMAP_PROTO_H
/*
This file was autogenerated from src/tilemap.c
by bin/genproto
Please do not hand edit.
*/

/** A tile map is a game map that uses tiled panes for it's 
display and physics. */
struct Tilemap_;
typedef struct Tilemap_ Tilemap;

/** Loads an image of the given category and index as a texture. */
Image * image_loadtexture(const char * category, int index);

/** Cleans up a tilemap. */
Tilemap * tilemap_done(Tilemap * self);

/** Initializes a tile map */
Tilemap* tilemap_init(Tilemap * self, Tileset * set, int w, int h);

/** Frees the tile map and initializes it. */
Tilemap * tilemap_free(Tilemap * map);

/** Allocates a new tile map and initializes it. */
Tilemap * tilemap_new(Tileset * set, int w, int h);

/** Returns a pointer to the pane at index or NULL if out of range. */
Tilepane * tilemap_pane(Tilemap * self, int index);

/** Sets a new tile pane for the pane at indexeth index of the tile map.
* the old pane, if any, will be deleted with tilepane_free
*/
Tilepane * tilemap_pane_(Tilemap * self, int index, Tilepane * pane);

/** Makes a new tile pane for the pane at indexeth index of the tile map. */
Tilepane * tilemap_panenew(Tilemap * self, int index, int w, int h);

/** Returns the tile in the tile map in the given layer at the given coords. */
Tile * tilemap_get(Tilemap * self, int l, int x, int y);

/** Converts the tile's flags to an integer id for the best Thing to 
represent this tile. Returns THING_UNUSED if it needs no physical representation. */
int tile_thingkind(Tile * tile);

/** Makes sure that the tile gets properly physically represented in the 
Area of the tilemap. Returns the Thing generated or NULL when no physical
representation is needed.
*/
Thing * tilemap_tiletothing(Tilemap * self, int l, int x, int y, Tile * tile);

/** Sets a tile in the tile map to the given tile. */
Tile * tilemap_settile(Tilemap * self, int l, int x, int y, Tile * tile);

/** Sets a tile in the tile map to the tile with the given index. */
Tile * tilemap_setindex(Tilemap * self, int l, int x, int y, int index);

/** Sets a rectangle area in the tile map to the given tile. */
Tile * tilemap_rect(Tilemap * self, int l, 
                       int x, int y, int w, int h, Tile * tile);

/** Sets a rectangle area in the tile map to the given tile. */
Tile * tilemap_fill(Tilemap * self, int l, Tile * tile);

/** Gets the index of the tile at the given location in the tilemap. */
int tilemap_getindex(Tilemap * self, int l, int x, int y);

/** Makes a static Thing in the Area of the map at the given 
tile coordinates zith the size of one tile. Returns the Thing thus represented. */
Thing * tilemap_addtilething(Tilemap * self, int kind, int tx, int ty, int layer);

/** Draws a tile map. */
void tilemap_draw(Tilemap * map, Camera * camera);

/** Updates the tile map. Currently this animates the tiles. */
void tilemap_update(Tilemap * map, double dt);

/** Adds a dynamic thing of the given type to the tile map's area */
Thing * tilemap_addthing(Tilemap * self, int kind, int x, int y, int z, 
                         int w, int h);

#endif // TILEMAP_PROTO_H

