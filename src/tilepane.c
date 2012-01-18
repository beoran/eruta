#include "eruta.h"
#include "mem.h"
#include "tile.h"
#include "tilepane.h"

/**
This comment is copied verbatim to the header but no prototype is generated.
**/

/**
* A Tilepane is a pane or layer of tiles for use in a tile map or tiled
* background. A Tilepane consists of individual tiles from the same
* tile set. Different panes may have different tile sets.
*/
struct Tilepane_ {
  Tileset     * set;
  Tile      *** tiles;
  int           gridwide; // width of the tile map in grid points (tiles)
  int           gridhigh; // height of the tile map in grid points (tiles)
  int           realwide; // width of whole tile map in pixels
  int           realhigh; // height of whole tile map in pixels
};


/** 
* Cleans up a pane by freeing the memory references it holds internally,
* but does not free the pane itself.
*/
Tilepane * tilepane_done(Tilepane * pane) {
  int index;
  // First free the rows.
  if (!pane) return NULL;
  if (pane->tiles) {
    for (index = 0; index < pane->gridhigh ; index ++) {
      mem_free(pane->tiles[index]);
    }
  }  
  // Then the column holder arrays. 
  mem_free(pane->tiles);
  // Size is now zero.
  pane->gridhigh = 0;
  pane->gridwide = 0;
  return pane;
}

/** Deallocates a Tilepane. */
Tilepane * tilepane_free(Tilepane * pane) {
  tilepane_done(pane);
  mem_free(pane);
  return NULL;
}


/** Initializes a tile pane. */
Tilepane * tilepane_init(Tilepane * pane, Tileset * set,
                           int gridwide, int gridhigh) {
  int index, jdex;
  if (!pane) return NULL;
  tilepane_tileset_(pane, set);
  pane->gridwide   = gridwide;
  pane->gridhigh   = gridhigh;
  pane->realwide   = TILE_W * pane->gridwide;
  pane->realhigh   = TILE_H * pane->gridhigh;
  // declare background, etc, empty
  
  // Precalculate dimensions...  
  // And allocate space for the tiles and tile indices.
  
  pane->tiles    = (Tile***) mem_calloc(pane->gridhigh, sizeof(Tile**));
  if(!pane->tiles) {
    return NULL;
  }
  
  // First empty...
  for (index = 0; index < pane->gridhigh ; index ++) {
    pane->tiles[index] = NULL;
  }
  
  // Then allocate column rows, calling gypane done on failiure
  // that's why we need to null everything first) 
  for (index = 0; index < pane->gridhigh ; index ++) {
    pane->tiles[index] = (Tile**) mem_calloc(pane->gridwide, sizeof(Tile*));
    if(!pane->tiles[index]) {
      tilepane_done(pane);
      return NULL;
    }
    // set all tile pointers to NULL
    for(jdex = 0; jdex < pane->gridwide ; jdex++) {
      pane->tiles[index][jdex] = NULL;
    }
  }
  
  return pane;
}

/** Makes new gy pane. */
Tilepane * tilepane_new(Tileset * set, int gridwide, int gridhigh) {
  Tilepane * pane = STRUCT_ALLOC(Tilepane);
  return tilepane_init(pane, set, gridwide, gridhigh);
}


/** Returns the width of the pane in grid units. Returns -1 on error. */
int tilepane_gridwide(Tilepane * pane) {
  if(!pane) return -1;
  return pane->gridwide;
}

/** Returns the height of the pane in grid units. Returns -1 on error. */
int tilepane_gridhigh(Tilepane * pane) {
  if(!pane) return -1;
  return pane->gridhigh;
}

/** Returns the width of the pane's tiles in pixels. Returns -1 on error. */
int tilepane_tilewide(Tilepane * pane) {
  if(!pane) return -1;
  return TILE_W;
}

/** Returns the height of the pane's tiles in pixels. Returns -1 on error. */
int tilepane_tilehigh(Tilepane * pane) {
  if(!pane) return -1;
  return TILE_H;
}

/** Returns the width of the pane in pixels. Returns -1 on error. */
int tilepane_wide(Tilepane * pane) {
  if(!pane) return -1;
  return pane->realwide;
}

/** Returns the height of the pane in pixels. Returns -1 on error. */
int tilepane_high(Tilepane * pane) {
  if(!pane) return -1;
  return pane->realhigh;
}

/** Returns TRUE if the given gridx and gridy are outside the grid
* Returns FALSE if inside the grid.
*/
int tilepane_outsidegrid(Tilepane * pane, int gridx, int gridy) {
  if (!pane) return TRUE;
  if ((gridx < 0) || (gridy < 0)) return TRUE;
  if ((gridx >= pane->gridwide) || (gridx >= pane->gridhigh)) return TRUE;
  return FALSE;
}

/** Sets the tile at the given location to the given Tile pointer,
* which may be NULL. Returns the pane, or NULL on error.
*/
Tilepane * tilepane_set(Tilepane * pane,
                          int gridx, int gridy, Tile * tile) {
  if (tilepane_outsidegrid(pane, gridx, gridy)) return NULL;
  pane->tiles[gridy][gridx]    = tile;
  return pane;
}  

/** Returns the sheet in the pane's grid at the given grid coordinates,
* returns NULL if the fcoordinates are out of bounds or if it was an empty tile.
*/
Tile * tilepane_get(Tilepane * pane, int gridx, int gridy) {
  if (tilepane_outsidegrid(pane, gridx, gridy)) return NULL;
  return pane->tiles[gridy][gridx];
}  

/** Sets the tile in the given rectangle  to the given Tile pointer,
* which may be NULL. Returns the pane, or NULL on error.
*/
Tilepane * tilepane_rect(Tilepane * pane,
                             int gridx, int gridy, int gridw, int gridh, 
                             Tile * tile) {
  int ii, jj;
  for (jj = gridy; jj < (gridy + gridh) ; jj++){
    for (ii = gridx; ii < (gridx + gridw) ; ii++) {
      tilepane_set(pane, ii, jj, tile);
    }
  }
  return pane;
}

/** Fills the while tile pane with the given tile */
Tilepane * tilepane_fill(Tilepane * pane, Tile * tile) {
  int ww = tilepane_gridwide(pane);
  int hh = tilepane_gridhigh(pane);
  return tilepane_rect(pane, 0, 0, ww, hh, tile);
}


/** Draws the tile pane, with x and y as the top left corner,
* to the current active display  X and y may be negative.
*/
void tilepane_draw(Tilepane * pane, int x, int y) {
  // Copy everything to the stack since that should be faster than always
  // referring to pointers.
  int gridwide    = pane->gridwide;
  int gridhigh    = pane->gridhigh;
  int tilewide    = TILE_W;
  int tilehigh    = TILE_H;
  int txstart     = x / tilewide;
  int tystart     = y / tilehigh;
  int xtilestop   = (SCREEN_W / tilewide) + 1;
  int ytilestop   = (SCREEN_H / tilehigh) + 1;
  int txstop      = xtilestop + txstart;
  int tystop      = ytilestop + tystart;
  int drawx       = 0;
  int drawy       = 0;
  int ty_index    = 0;
  int tx_index    = 0;
  int realwide    = pane->realwide;
  int realhigh    = pane->realhigh;
  Tile ** row     = NULL;
  Tile * tile     = NULL;
  if (txstart >= realwide) return;
  if (tystart >= realhigh) return;
  // hold drawing since all tile draws come from the same tile sheet
  al_hold_bitmap_drawing(TRUE);
  txstart         = (txstart < 0) ? 0 : txstart;
  tystart         = (tystart < 0) ? 0 : tystart;
  txstop          = (txstop > gridwide) ? gridwide : txstop;
  tystop          = (tystop > gridhigh) ? gridhigh : tystop;  
  drawy           = -y + ((tystart-1) * tilehigh);  
  for (ty_index = tystart; ty_index < tystop ; ty_index++) {
    drawy        += tilehigh;
    drawx         = -x + ((txstart-1) * tilewide);
    row           = pane->tiles[ty_index];
    if(!row) continue; 
    for(tx_index = txstart; tx_index < txstop ; tx_index++) { 
      drawx      += tilewide;
      tile        = row[tx_index];
      if(tile) {
        tile_draw(tile, drawx, drawy);
      } 
    }
  }
  // Let go of hold 
  al_hold_bitmap_drawing(FALSE);  
}


/** Sets this Tilepane's tile set. */
Tileset * tilepane_tileset_(Tilepane * pane, Tileset * set) {
  if (!pane) return NULL;
  return pane->set = set;
}

/** Gets this Tilepane's tile set. */
Tileset * tilepane_tileset(Tilepane * pane) {
  if (!pane) return NULL;
  return pane->set;
}

/** Gets a tile from a the tilepane's tile set by it's tile id. **/
Tile * tilepane_getfromset(Tilepane * pane, int index) {  
  Tileset * set = tilepane_tileset(pane);
  if (!set) return NULL;
  if (index < 0) return NULL;
  return tileset_get(set, index);
}

/** Sets the tile at the given location to tile at index index in the
* Tilepane's Tileset, in index < 0, sets NULL; 
*/
Tilepane * tilepane_setindex(Tilepane * pane,
                             int gridx, int gridy, int index) {
  Tile * tile = tilepane_getfromset(pane, index);
  return tilepane_set(pane, gridx, gridy, tile);
}



