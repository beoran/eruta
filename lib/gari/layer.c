#include "gari.h"

#define GARI_INTERN_ONLY
#include "gari_intern.h"

/* Layers in a tile map. */

struct GariLayer_ {
  GariCamera  *   camera;
  GariTileset *   set;
  GariTile    *** tiles;
  GariImage   *   background;
  int         **  tilesave;
  int             tilewide; // width of map tiles in pixels
  int             tilehigh; // height of map tiles in pixels
  int             gridwide; // width of the tile map in grid points (tiles)
  int             gridhigh; // height of the tile map in grid points (tiles)
  int             realwide; // width of whole tile map in pixels
  int             realhigh; // height of whole tile map in pixels  
};

// cleans up a layer by freeing the memory references it holds internally,
// but does not free the layer itself 
GariLayer * gari_layer_done(GariLayer * layer) {
  int index;
  // First free the rows.
  if (!layer) return NULL;
  for (index = 0; index < layer->gridhigh ; index ++) {
    GARI_FREE(layer->tiles[index]);
    GARI_FREE(layer->tilesave[index]);
  }
  // Then the column holder arrays. 
  GARI_FREE(layer->tiles);
  GARI_FREE(layer->tilesave);
  // Size is now zero.
  layer->gridhigh = 0;
  layer->gridwide = 0;
  return layer;  
}

GariLayer * gari_layer_init(GariLayer * layer, GariTileset * set, 
                           int gridwide, int gridhigh, 
                           int tilewide, int tilehigh) {
  int index;
  if((!layer) || (!set)) return NULL;
  layer->set        = set;
  layer->gridwide   = gridwide;
  layer->gridhigh   = gridhigh;
  layer->tilewide   = tilewide;  
  layer->tilehigh   = tilehigh;
  layer->realwide   = layer->tilewide * layer->gridwide;
  layer->realhigh   = layer->tilehigh * layer->gridhigh;
  // declare background, etc, empty
  layer->camera     = NULL;
  layer->background = NULL;
  
  // Precalculate dimensions...  
  // And allocate space for the tiles and tile indices.
  layer->tiles    = GARI_MALLOC(sizeof(GariTile**) * layer->gridhigh);
  if(!layer->tiles) {
    return NULL;
  }   
  // First empty...
  for (index = 0; index < layer->gridhigh ; index ++) {
    layer->tiles[index] = NULL; 
  }
  
  // Then allocate column rows, calling gari_layer done on failiure
  // that's why nwe neede dto null everything first) 
  for (index = 0; index < layer->gridhigh ; index ++) {
    layer->tiles[index] = GARI_MALLOC(sizeof(GariTile *) * layer->gridwide);
    if(!layer->tiles[index]) {
      gari_layer_done(layer);
      return NULL;
    } 
  }
  // Now also the editing support matrix...
  layer->tilesave = GARI_MALLOC(sizeof(int*) * layer->gridhigh);
  if(!layer->tilesave) { // free up again on failiure
    gari_layer_done(layer);
    return NULL;
  }
  // And the rows of the editing matrix.
  for (index = 0; index < layer->gridhigh ; index ++) {
    layer->tilesave[index] = GARI_MALLOC(sizeof(int) * layer->gridwide);
    if(!layer->tilesave[index]) {
      gari_layer_done(layer);
      return NULL;
    }
  }
  
  return layer;
}

/** Returns TRUE if the given gridx and gridy are outside the grid
 Returns FALSE if inside the grid.
*/
int gari_layer_outsidegrid(GariLayer * layer, int gridx, int gridy) {
  if (!layer) return TRUE;
  if ((gridx < 0) || (gridy < 0)) return TRUE;
  if ((gridx >= layer->gridwide) || (gridx >= layer->gridhigh)) return TRUE;
  return FALSE;
}

/** Sets the tile in this layer to the tile with this tile index in the 
layer's Tileset. Returns the tile thus set or NULL on error, or if a NULL 
tile was set (index -1).  */
GariTile * gari_layer_set(GariLayer * layer, int gridx, int gridy, int tileid) { 
  GariTile * realtile = NULL;
  if (gari_layer_outsidegrid(layer, gridx, gridy)) return NULL;                 
  if (tileid < 0) {
    realtile = NULL;
    tileid   = -1; 
  } else {
    realtile = gari_tileset_get(layer->set, tileid);
    if(!realtile) return NULL;
    // bail out if no such tile found. 
  }
  layer->tiles[gridy][gridx]    = realtile;
  layer->tilesave[gridy][gridx] = tileid;
  return realtile;
}  

/** Returns the tile in the layer's grid at the given grid coordinates,
* returns -2 if the fcoordinates are out of bounds, -1 if it was an empty tile.  
*/
int gari_layer_get(GariLayer * layer, int gridx, int gridy) { 
  GariTile * realtile = NULL;
  if (gari_layer_outsidegrid(layer, gridx, gridy)) return -2;
  return layer->tilesave[gridy][gridx];
}  


/*
  
  # Returns the actual tile at the given x and y coordinates, 
  # or nil if the coordinates are out of bounds
  def get_tile(x, y) 
    gx = x; gy = y
    return nil if gy > @high or gx > @wide or gy < 0 or gx < 0  
    return @layer[gy][gx]
  end
  
  # Iterate over all tiles in this layer, column by column. 
  # Yields Tile objects, and the world x and y coordinate of this tile 
  # in the layer.
  def each_tile()
    for y in (0...@high)
      for x in (0...@wide)          
          wx, wy = tile_to_world(x, y)
          yield @layer[y][x], wx, wy
      end
    end
  end

  def draw(screen, x, y)
    txstart     = ( x / @tilewide )
    tystart     = ( y / @tilehigh )
    xtilestop   = (screen.w / @tilewide) + 1
    ytilestop   = (screen.h / @tilehigh) + 1
    txstop      = xtilestop + txstart
    tystop      = ytilestop + tystart
    drawx       = 0
    drawy       = 0
    row         = nil
    aidimage    = nil
    nilcount    = 0
    return if (txstart >= @wide or tystart >= @high) 
    txstart = 0    if (txstart < 0) 
    tystart = 0    if (tystart < 0)
    txstop  = @wide if (txstop > @wide) 
    tystop  = @high if (tystop > @high) 
    drawy   = -y + ( (tystart-1) * @tilehigh )
    tydex   = tystart
    # for tydex in (tystart...tystop)
    while tydex < tystop do 
      drawy   += @tilehigh;
      drawx   = -x + ( (txstart-1) * @tilewide )
      row     = @layer[tydex]
      # for txdex in (txstart...txstop) do
      txdex = txstart
      while txdex < txstop
        drawx   += @tilewide        
        aidtile  = row[txdex]
        unless aidtile.nil? then
          aidimage    = aidtile.get
          SDL::Surface.blit(aidimage, 0,0,0,0, screen, drawx, drawy)
        end
        txdex += 1
      end
      tydex  += 1
    end
  end
*/




