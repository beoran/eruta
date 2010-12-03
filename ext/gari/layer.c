#include "gari.h"

#define GARI_INTERN_ONLY
#include "gari_intern.h"

/* Layers in a tile map. This is very much simplified. */

/** 
* A tile map consists of individual tiles. Tiles are simply indirect 
* pointers to images, where the pointer can be changed when animation is needed. 
* The same idea is used for sprites, and unified in the GariSheet struct.
*/

struct GariSheet_ {
  GariImage * image;  
}; 

GariSheet * gari_sheet_image_(GariSheet * sheet, GariImage * image) {
  if(!sheet) return NULL;
  sheet->image = image;
  return sheet;
}

GariImage * gari_sheet_image(GariSheet * sheet) {
  if(!sheet) return NULL;
  if(!sheet->image) return NULL;
  return sheet->image;
}

GariSheet * gari_sheet_new(GariImage * image) {
  GariSheet * sheet = GARI_ALLOCATE(GariSheet);
  return gari_sheet_image_(sheet, image);
}

GariSheet * gari_sheet_free(GariSheet * sheet) {
  GARI_FREE(sheet);
  return NULL;
}

void gari_image_blitsheet(GariImage * dst, int x, int y, GariSheet * sheet) {
  GariImage * img = sheet->image; 
  if(!img) return; 
  gari_image_blit(dst, x, y, img);
} 



struct GariLayer_ {
  GariSheet   *** tiles;
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
  if (layer->tiles) { 
    for (index = 0; index < layer->gridhigh ; index ++) {
      GARI_FREE(layer->tiles[index]);
    }
  }  
  // Then the column holder arrays. 
  GARI_FREE(layer->tiles);
  // Size is now zero.
  layer->gridhigh = 0;
  layer->gridwide = 0;
  return layer;  
}

/** Deallocates gari layer. */
GariLayer * gari_layer_free(GariLayer * layer) {
  gari_layer_done(layer);
  GARI_FREE(layer);
  return NULL;
}

/** Makes new gari layer. */
GariLayer * gari_layer_new(int gridwide, int gridhigh, 
                           int tilewide, int tilehigh) {
  GariLayer * layer = GARI_ALLOCATE(GariLayer);
  return gari_layer_init(layer, gridwide, gridhigh, tilewide, tilehigh);
}

GariLayer * gari_layer_init(GariLayer * layer,  
                           int gridwide, int gridhigh, 
                           int tilewide, int tilehigh) {
  int index;
  if((!layer)) return NULL;
  layer->gridwide   = gridwide;
  layer->gridhigh   = gridhigh;
  layer->tilewide   = tilewide;  
  layer->tilehigh   = tilehigh;
  layer->realwide   = layer->tilewide * layer->gridwide;
  layer->realhigh   = layer->tilehigh * layer->gridhigh;
  // declare background, etc, empty
  
  // Precalculate dimensions...  
  // And allocate space for the tiles and tile indices.
  layer->tiles    = GARI_MALLOC(sizeof(GariSheet**) * layer->gridhigh);
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
    layer->tiles[index] = GARI_MALLOC(sizeof(GariSheet *) * layer->gridwide);
    if(!layer->tiles[index]) {
      gari_layer_done(layer);
      return NULL;
    } 
  }
  return layer;
}

/** Returns the width of the layer in grid units. Returns -1 on error. */
int gari_layer_gridwide(GariLayer * layer) {
  if(!layer) return -1;
  return layer->gridwide;
}

/** Returns the height of the layer in grid units. Returns -1 on error. */
int gari_layer_gridhigh(GariLayer * layer) {
  if(!layer) return -1;
  return layer->gridhigh;
}

/** Returns the width of the layer's tiles in pixels. Returns -1 on error. */
int gari_layer_tilewide(GariLayer * layer) {
  if(!layer) return -1;
  return layer->tilewide;
}

/** Returns the height of the layer's tiles in pixels. Returns -1 on error. */
int gari_layer_tilehigh(GariLayer * layer) {
  if(!layer) return -1;
  return layer->tilehigh;
}

/** Returns the width of the layer in pixels. Returns -1 on error. */
int gari_layer_wide(GariLayer * layer) {
  if(!layer) return -1;
  return layer->realwide;
}

/** Returns the height of the layer in pixels. Returns -1 on error. */
int gari_layer_high(GariLayer * layer) {
  if(!layer) return -1;
  return layer->realhigh;
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

/** Sets the tile at the given location to the given GariSheet pointer, 
* which may be NULL. Returns the layer, or NULL on error.  
*/
GariLayer * gari_layer_set(GariLayer * layer, 
                          int gridx, int gridy, GariSheet * tile) { 
  if (gari_layer_outsidegrid(layer, gridx, gridy)) return NULL;
  layer->tiles[gridy][gridx]    = tile;
  return layer;
}  

/** Returns the sheet in the layer's grid at the given grid coordinates,
* returns NULL if the fcoordinates are out of bounds or if it was an empty tile.
*/
GariSheet * gari_layer_get(GariLayer * layer, int gridx, int gridy) { 
  if (gari_layer_outsidegrid(layer, gridx, gridy)) return NULL;
  return layer->tiles[gridy][gridx];
}  

/** Draws the tile layer, with x and y as the top left corner. 
*  X and y may be negative. 
*/
void gari_layer_draw(GariLayer * layer, GariImage * image, int x, int y) {
  // Copy everything to the stack since that should be faster than always
  // referring to pointers.
  int gridwide    = layer->gridwide;
  int gridhigh    = layer->gridhigh;
  int tilewide    = layer->tilewide;
  int tilehigh    = layer->tilehigh;
  int txstart     = x / tilewide;
  int tystart     = y / tilehigh;
  int xtilestop   = (gari_image_w(image) / tilewide) + 1;
  int ytilestop   = (gari_image_h(image) / tilehigh) + 1;
  int txstop      = xtilestop + txstart;
  int tystop      = ytilestop + tystart;
  int drawx       = 0;
  int drawy       = 0;
  int ty_index    = 0;
  int tx_index    = 0;
  int realwide    = layer->realwide;
  int realhigh    = layer->realhigh;
  GariSheet ** row= NULL;
  GariSheet * tile= NULL;
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
    for(tx_index = txstart; tx_index < txstop ; tx_index++) { 
      drawx      += tilewide;
      tile        = row[tx_index];
      if(tile) {
        gari_image_blitsheet(image, drawx, drawy, tile);
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




