#include "gy.h"

#define GY_INTERN_ONLY
#include "gy_intern.h"

/* A Tile map layer. This is very much simplified. */

/** 
* A tile map consists of individual tiles. Tiles are simply short integers,
* that point to a tile in the layer's texture.
*/


struct GyLayer_ {
  GyTexture     * texture;
  GyTileindex  ** tiles;
  int             texturewide; // width of map texture in pixels
  int             texturehigh; // height of map texture in pixels  
  int             tilewide; // width of map tiles in pixels
  int             tilehigh; // height of map tiles in pixels
  int             gridwide; // width of the tile map in grid points (tiles)
  int             gridhigh; // height of the tile map in grid points (tiles)
  int             realwide; // width of whole tile map in pixels
  int             realhigh; // height of whole tile map in pixels  
};

// cleans up a layer by freeing the memory references it holds internally,
// but does not free the layer itself 
GyLayer * gylayer_done(GyLayer * layer) {
  int index;
  // First free the rows.
  if (!layer) return NULL;
  if (layer->tiles) { 
    for (index = 0; index < layer->gridhigh ; index ++) {
      GY_FREE(layer->tiles[index]);
    }
  }  
  // Then the column holder array. 
  GY_FREE(layer->tiles);
  layer->tiles        = NULL;
  // Size is now zero.
  layer->gridhigh     = 0;
  layer->gridwide     = 0;
  layer->tilehigh     = 0;
  layer->tilewide     = 0;
  layer->realhigh     = 0;
  layer->realwide     = 0;
  layer->texturehigh  = 0;
  layer->texturewide  = 0;
  layer->texture      = NULL;
  return layer;
}

/** Deallocates gy layer. */
GyLayer * gylayer_free(GyLayer * layer) {
  gylayer_done(layer);
  GY_FREE(layer);
  return NULL;
}

/** Makes new gy layer. */
GyLayer * gylayer_new(GyTexture * texture,
                      int gridwide, int gridhigh,
                      int tilewide, int tilehigh) {
  GyLayer * layer = GY_ALLOCATE(GyLayer);
  return gylayer_init(layer, texture, gridwide, gridhigh, tilewide, tilehigh);
}

GyLayer * gylayer_init(GyLayer * layer, GyTexture * texture,
                           int gridwide, int gridhigh, 
                           int tilewide, int tilehigh) {
  int index;
  if((!layer)) return NULL;
  layer->texture    = texture;
  layer->texturewide= gytexture_w(texture);
  layer->texturehigh= gytexture_h(texture);
  layer->gridwide   = gridwide;
  layer->gridhigh   = gridhigh;
  layer->tilewide   = tilewide;
  layer->tilehigh   = tilehigh;
  layer->realwide   = layer->tilewide * layer->gridwide;
  layer->realhigh   = layer->tilehigh * layer->gridhigh;
  // Declare background, etc, empty
  
  // Precalculate dimensions...  
  // And allocate space for the tiles and tile indices.
  layer->tiles      = GY_MALLOC(sizeof(GyTileindex*) * layer->gridhigh);
  if(!layer->tiles) {
    return NULL;
  }   
  // First empty
  for (index = 0; index < layer->gridhigh ; index ++) {
    layer->tiles[index] = NULL; 
  }
  
  // Then allocate column rows, calling gylayer done on failure
  // that's why we need to null everything first)
  for (index = 0; index < layer->gridhigh ; index ++) {
    int size            = sizeof(GyTileindex) * layer->gridwide;
    layer->tiles[index] = GY_MALLOC(size);
    if(!layer->tiles[index]) {
      gylayer_done(layer);
      return NULL;
    }
    // Set every tile to -1
    memset(layer->tiles[index], -1,  size); 
  }
  return layer;
}

/** Returns the width of the layer in grid units. Returns -1 on error. */
int gylayer_gridwide(GyLayer * layer) {
  if(!layer) return -1;
  return layer->gridwide;
}

/** Returns the height of the layer in grid units. Returns -1 on error. */
int gylayer_gridhigh(GyLayer * layer) {
  if(!layer) return -1;
  return layer->gridhigh;
}

/** Returns the width of the layer's tiles in pixels. Returns -1 on error. */
int gylayer_tilewide(GyLayer * layer) {
  if(!layer) return -1;
  return layer->tilewide;
}

/** Returns the height of the layer's tiles in pixels. Returns -1 on error. */
int gylayer_tilehigh(GyLayer * layer) {
  if(!layer) return -1;
  return layer->tilehigh;
}

/** Returns the width of the layer in pixels. Returns -1 on error. */
int gylayer_wide(GyLayer * layer) {
  if(!layer) return -1;
  return layer->realwide;
}

/** Returns the height of the layer in pixels. Returns -1 on error. */
int gylayer_high(GyLayer * layer) {
  if(!layer) return -1;
  return layer->realhigh;
}

/** Returns TRUE if the given gridx and gridy are outside the grid
 Returns FALSE if inside the grid.
*/
int gylayer_outsidegrid(GyLayer * layer, int gridx, int gridy) {
  if (!layer) return TRUE;
  if ((gridx < 0) || (gridy < 0)) return TRUE;
  if ((gridx >= layer->gridwide) || (gridx >= layer->gridhigh)) return TRUE;
  return FALSE;
}

/** Sets the tile at the given location to the given GySheet pointer, 
* which may be NULL. Returns the layer, or NULL on error.  
*/
GyLayer * gylayer_set(GyLayer * layer, 
                          int gridx, int gridy, GyTileindex tile) { 
  if (gylayer_outsidegrid(layer, gridx, gridy)) return NULL;
  layer->tiles[gridy][gridx]    = tile;
  return layer;
}  

/** Set a rectangle in the layer to the given tile index */
GyLayer * gylayer_setslab(GyLayer * layer, 
                      int gridx, int gridy, int gridw, int gridh, 
                      GyTileindex tile) { 
  int x, y;
  for(y = gridy; y < gridh ; y++) { 
    for(x = gridx; x < gridw ; x++) { 
      gylayer_set(layer, x, y, tile);
    }
  }
  return layer;
}

/** Fills layer to the given tile index */
GyLayer * gylayer_fill(GyLayer * layer, GyTileindex tile) { 
  return gylayer_setslab(layer, 0, 0, layer->gridwide, layer->gridhigh, tile);
}


/** Returns the sheet in the layer's grid at the given grid coordinates,
* returns NULL if the fcoordinates are out of bounds or if it was an empty tile.
*/
GyTileindex gylayer_get(GyLayer * layer, int gridx, int gridy) { 
  if (gylayer_outsidegrid(layer, gridx, gridy)) return -1;
  return layer->tiles[gridy][gridx];
}  

/** Draws the tile layer, with x and y as the top left corner. 
*  X and y may be negative. 
*/
void gylayer_draw(GyLayer * layer, GyWindow * window, int x, int y) {
  // Copy everything to the stack since that should be faster than always
  // referring to pointers.
  int gridwide      = layer->gridwide;
  int gridhigh      = layer->gridhigh;
  int tilewide      = layer->tilewide;
  int tilehigh      = layer->tilehigh;
  int txstart       = x / tilewide;
  int tystart       = y / tilehigh;
  int xtilestop     = (gywindow_w(window) / tilewide) + 1;
  int ytilestop     = (gywindow_h(window) / tilehigh) + 1;
  int txstop        = xtilestop + txstart;
  int tystop        = ytilestop + tystart;
  int drawx         = 0;
  int drawy         = 0;
  int ty_index      = 0;
  int tx_index      = 0;
  int realwide      = layer->realwide;
  int realhigh      = layer->realhigh;
  int srcx          = 0;
  int srcy          = 0;
  GyCanvas * canvas = gywindow_canvas(window);
  GyTileindex * row = NULL;
  GyTileindex  tile = -1;
  if (txstart >= realwide) return;
  if (tystart >= realhigh) return;
  txstart         = (txstart < 0) ? 0 : txstart;
  tystart         = (tystart < 0) ? 0 : tystart;
  txstop          = (txstop > gridwide) ? gridwide : txstop;
  tystop          = (tystop > gridhigh) ? gridhigh : tystop;  
  drawy           = -y + ((tystart-1) * tilehigh);  
  for (ty_index = tystart; ty_index < tystop ; ty_index++) {
    drawy        += tilehigh;
    drawx         = -x + ((txstart-1) * tilewide);
    row           = layer->tiles[ty_index];     
    if(!row) continue; 
    for(tx_index = txstart; tx_index < txstop ; tx_index++) { 
      drawx      += tilewide;
      tile        = row[tx_index];
      if(tile >= 0) {
        srcx      = (tile * tilewide) % layer->texturewide;
        srcy      = (tile * tilewide) / layer->texturewide;
        gycanvas_putpart(canvas, drawx, drawy, layer->texture,
                         srcx, srcy, tilewide, tilehigh);
      }
    }
  } 
}

/*
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




