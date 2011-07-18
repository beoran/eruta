#include "gy.h"
#include "si_macro.h"
#include "si_mem.h"
#include "si_block.h"


/* 
  A tile list is a list of tiles, indexed by number.
  It holds references to the individual tiles. 
  These tiles are loaded from several image files.
*/
#define TILELIST_MAX 100000
#define TILELIST_TILE_WIDE 32
#define TILELIST_TILE_HIGH 32

struct Tilelist_;
typedef struct Tilelist_ Tilelist;

struct Tilelist_ { 
  SiBlock * images;
  SiBlock * tiles;
  SiBlock * filenames;
  int tilewide;
  int tilehigh;
  int last; // last used tile index; 
};

Tilelist * tilelist_alloc() {
  return SI_ALLOCATE(Tilelist);
}
    
Tilelist * tilelist_init(Tilelist * self) {
  if(!self) return NULL;
  self->images    = siblock_new(10000, sizeof(GyImage *));
  self->tiles     = siblock_new(10000, sizeof(void *));
  self->filenames = NULL;
  self->last      = 0;
  return self;
}

Tilelist * tilelist_done(Tilelist * self) {
  siblock_free(self->images);
  self->images    = NULL;
  siblock_free(self->tiles);
  self->tiles     = NULL; 
  siblock_free(self->filenames);
  self->filenames = NULL;
  return self;
}

Tilelist * tilelist_free(Tilelist * self) {
  return si_free(tilelist_done(self));
}

// Sets the image to use for an index
GyImage * tilelist_image_(Tilelist * self, size_t index, GyImage * img) {
  return siblock_setptr(self->images, index, img);
}  

// Sets the tile to use for an index
GyImage * tilelist_tile_(Tilelist * self, size_t index, void * tile) {
  return siblock_setptr(self->tiles, index, tile);
}

GyImage * tilelist_copypart(GyImage * img, int x, int y, 
                                           int w, int h, int alpha) {
// should detect alpha itself. 
/*     result = from.copypart(x, y, w, h)
      return result.optimize(:alpha)*/
  return NULL;
}

/** Loads the image file with the given name at the given start index, or, at 
the last index if start = -1. */
Tilelist * tilelist_load_filename(Tilelist * self, char * name, size_t start) {
  int x, y, h, w;
  GyImage * image;
  if (start < 0) start = self->last;
  self->last           = start;
  image = gyimage_loadraw(name);
  if(!image) return NULL;
  h = gyimage_h(image);
  w = gyimage_w(image);
  for(y = 0; y < h; y += TILELIST_TILE_HIGH) { 
    for(x = 0; x < w; x += TILELIST_TILE_WIDE) {
      GyImage * subimage = tilelist_copypart(image, x, y, w, h, FALSE);
      tilelist_image_(self, self->last, subimage);
      self->last++;
    }
  }
  return self;
}

/*
    # Converts all images in the images hash to tiles
    def images_to_tiles(images)
      tiles = {}
      return nil unless images
      for index, image in images do
        tile          = Tile.new(index, [image])
        tiles[index]  = tile
      end
      return tiles
    end
    
    # Save to Raku format
    def to_raku
      res = StringIO.new('')
      res.puts("  tileset {")
      res.puts("    tilesize #{@tilewide} #{@tilehigh}")
      res.puts("    names {")
      for name in @filenames do
        res.puts("      name \"#{name}\"")
      end
      res.puts("    }")
      res.puts("    tiles {")
      for key, tile in @tiles do
        res << tile.to_raku
      end
      res.puts("    }")
      res.puts("  }")
      return res.string
    end
    
    def set_tile(index, tile)
      @tiles[index] = tile
    end
    
    def set_image(index, image)
      @images[index] = image
    end
    
    def get_tile(index)
      @tiles[index]
    end
    
    def get_image(index)
      @images[index]
    end

    
    # FIXME: stub
    def self.new_from_raku(raku)
      rats = raku.find_first(:map, :tileset)
      p rats
      return nil unless rats
      tsiz = rats.find_first(:tilesize)
      return nil unless tsiz
      tilewide, tilehigh = *tsiz.data
      names  = []
      rnames = rats.find_all(:names, :name)
      for rname in rnames do
        name = rname.data[0]
        names << name if name
      end
      p names, tilewide, tilehigh
      tileset = self.new(names)
      rtiles  = rats.find_all(:tiles, :tile)
      for rtile in rtiles
        tile  = Tima::Tile.new_from_raku(rtile)
        tileset.set_tile(tile.index, tile)
      end
      return tileset
    end

  end
end

*/
    
