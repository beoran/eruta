#ifndef TILEPANE_PROTO_H
#define TILEPANE_PROTO_H
/*
This file was autogenerated from src/tilepane.c
by bin/genproto on 2012-01-18 22:50:09 +0100
Please do not hand edit.
*/

/**
This comment is copied verbatim to the header but no prototype is generated.
**/

/**
* A Tilepane is a pane or layer of tiles for use in a tile map or tiled
* background. A Tilepane consists of individual tiles from the same
* tile set. Different panes may have different tile sets.
*/
struct Tilepane_;
typedef struct Tilepane_ Tilepane;

/** 
* Cleans up a pane by freeing the memory references it holds internally,
* but does not free the pane itself.
*/
Tilepane * tilepane_done(Tilepane * pane);

/** Deallocates a Tilepane. */
Tilepane * tilepane_free(Tilepane * pane);

/** Initializes a tile pane. */
Tilepane * tilepane_init(Tilepane * pane, Tileset * set,
                           int gridwide, int gridhigh);

/** Makes new gy pane. */
Tilepane * tilepane_new(Tileset * set, int gridwide, int gridhigh);

/** Returns the width of the pane in grid units. Returns -1 on error. */
int tilepane_gridwide(Tilepane * pane);

/** Returns the height of the pane in grid units. Returns -1 on error. */
int tilepane_gridhigh(Tilepane * pane);

/** Returns the width of the pane's tiles in pixels. Returns -1 on error. */
int tilepane_tilewide(Tilepane * pane);

/** Returns the height of the pane's tiles in pixels. Returns -1 on error. */
int tilepane_tilehigh(Tilepane * pane);

/** Returns the width of the pane in pixels. Returns -1 on error. */
int tilepane_wide(Tilepane * pane);

/** Returns the height of the pane in pixels. Returns -1 on error. */
int tilepane_high(Tilepane * pane);

/** Returns TRUE if the given gridx and gridy are outside the grid
* Returns FALSE if inside the grid.
*/
int tilepane_outsidegrid(Tilepane * pane, int gridx, int gridy);

/** Sets the tile at the given location to the given Tile pointer,
* which may be NULL. Returns the pane, or NULL on error.
*/
Tilepane * tilepane_set(Tilepane * pane,
                          int gridx, int gridy, Tile * tile);

/** Returns the sheet in the pane's grid at the given grid coordinates,
* returns NULL if the fcoordinates are out of bounds or if it was an empty tile.
*/
Tile * tilepane_get(Tilepane * pane, int gridx, int gridy);

/** Sets the tile in the given rectangle  to the given Tile pointer,
* which may be NULL. Returns the pane, or NULL on error.
*/
Tilepane * tilepane_rect(Tilepane * pane,
                             int gridx, int gridy, int gridw, int gridh, 
                             Tile * tile);

/** Fills the while tile pane with the given tile */
Tilepane * tilepane_fill(Tilepane * pane, Tile * tile);

/** Draws the tile pane, with x and y as the top left corner,
* to the current active display  X and y may be negative.
*/
void tilepane_draw(Tilepane * pane, int x, int y);

/** Sets this Tilepane's tile set. */
Tileset * tilepane_tileset_(Tilepane * pane, Tileset * set);

/** Gets this Tilepane's tile set. */
Tileset * tilepane_tileset(Tilepane * pane);

/** Gets a tile from a the tilepane's tile set by it's tile id. **/
Tile * tilepane_getfromset(Tilepane * pane, int index);  

/** Sets the tile at the given location to tile at index index in the
* Tilepane's Tileset, in index < 0, sets NULL; 
*/
Tilepane * tilepane_setindex(Tilepane * pane,
                             int gridx, int gridy, int index);

#endif // TILEPANE_PROTO_H

